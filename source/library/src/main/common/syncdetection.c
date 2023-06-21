/*
 * Copyright 2019-2023 Dominic Clifton
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Author: Dominic Clifton - Sync generation, Sync Detection, Video Overlay and first-cut of working OSD system.
 */

#include <stdbool.h>
#include <string.h>

#include "platform.h"


#if defined(BETAFLIGHT)
#include "build/debug.h"
#include "common/time.h"
#elif defined(FLIGHT_ONE)
#else
#include "time.h"
#include "utils.h"
#include "debug.h"
#endif

#include "configuration.h"
#include "videotiming.h"
#include "videoframe.h"
#include "spracingpixelosd_impl.h"

#include "api/spracingpixelosd_api.h"

#include "syncdetection.h"

typedef enum {
    NO_EVENT = 0,
    STATE_TRANSITION,
    ATTEMPTING_WITH_CAMERA,
    ATTEMPTING_WITHOUT_CAMERA,
    SYNC_OBTAINED,
    SYNC_LOST,
} eventId_e;

#ifdef DEBUG_OSD_EVENTS
typedef struct eventLogItem_s {
    timeUs_t at;
    eventId_e eventId;
} eventLogItem_t;

eventLogItem_t eventLog[256] = {0};
unsigned int eventLogIndex = 0;

void logEvent(timeUs_t at, eventId_e eventId)
{
    eventLogIndex++;
    if (eventLogIndex >= ARRAYLEN(eventLog)) {
        eventLogIndex = 0;
    }

    eventLogItem_t *item = &eventLog[eventLogIndex];
    item->eventId = eventId;
    item->at = at;
}
#else
#define logEvent(us, state) {}
#endif

static uint32_t nextEventAt = 0;
static timeUs_t previousServiceAt = 0;

static uint32_t syncLossUpdateEventAt = 0;

spracingPixelOSDSyncVoltages_t syncVoltages = { 0 };

/*
 * The double-sync problem, explained.
 *  *
 * NORMAL             TYPE A           TYPE B             TYPE C
 * _      _^ ^ _ .. _       _^ ^ _ .. _       _^ ^ _ .. _       _^ ^ _ ..
 *  |    |  v v      |     |           |     |           |     |
 *  |    |           |__   |           |_____|           |   __|  <-- BAD SYNC THRESHOLD
 *  |    |              |  |                             |  |     <-- GOOD SYNC THRESHOLD
 *  |____|              |__|                             |__|
 *
 * NORMAL = Sync pulse at normal voltage
 * TYPE A/B/C = Generated sync pulse raises a portion, or all, of the camera's sync voltage.
 *
 * If sync generation is enabled, pules of 'NORMAL', and 'TYPE A,B & C' can occur.  We must ignore them and only lock onto 'NORMAL' sync pulses.
 *
 * Currently there is no detection of TYPE A, B & C.  They are most easily observed on the scope during the VSYNC period.
 * An improvement might be to add counting of pulse errors during the VSYNC period and resync.
 * Scanning only for VSYNC pulse errors would be slower, since you have to wait longer for the VSYNC lines...
 */

/*
 *  Scan voltage range to find best threshold value
 *
 *  The idea is to build statistics for each MV range, for example (fictional numbers for pe and lines)
 *  | mv                |  0 | 25  | 50  | 75  | 100 | 125  | 150 | 175 | 200  | 225  | 250 | 275
 *  | pulse errors      |  0 | 200 | 180 | 100 | 50  | 25   | 0   | 0   | 10   | 12   | 50  | 100
 *  | lines             |  0 | 10  | 12  | 50  | 100 | 125  | 150 | 200 | 190  | 180  | 50  | 10
 *  | ratio (PE/lines)  |    | 20  | 15  | 2   | 0.5 | 0.2  | 0   | 0   | 0.05 | 0.06 | 1   | 10
 *
 *  The sweet spot for the threshold                                ^
 *  Determined by lowest ratio and most lines.
 *
 *  - Don't need to wait for entire frame, just have to scan wait enough time to find a reasonable amount of lines
 *  - No need to record all the scan attempts, but for debugging probably 3 is enough...
 *    1) first MV with non-zero lines
 *    2) best MV with lowest ratio and most lines (calculate current values and compare to 'best')
 *    3) last MV with zero lines.
 *
 *    With the above information, a mv-limited rescan and re-verification of 'BEST' is possible.
 *    e.g. repeat X times and find the average 'BEST'.
 *
 *    Possibly rescan using finer grained threshold increments, e.g.  25mv steps the first time, then 10mv, then, 5mv steps.
 *
 */

typedef enum {
    SEARCHING = 0,
    GENERATING_VIDEO,
    RETRY,
} pixelOsdVideoState_t;

pixelOsdVideoState_t pixelOsdVideoState = SEARCHING;

typedef struct syncDetectionState_s {
    uint8_t syncDetectionFailureCount;
    uint8_t syncDetectionFailureThreshold;
    uint8_t syncLossCount;
    uint8_t syncLossThreshold;
    bool uptimeIsHigh;

#ifdef DEBUG_SYNC_MIN_MAX_THRESHOLD
    uint16_t minMaxDifference; // FIXME remove this, it's transient
#endif

#ifdef DEBUG_SYNC_DURATION
    timeUs_t syncStartedAt;
    timeUs_t syncCompletedAt;
    timeUs_t syncDuration;
#endif
} syncDetectionState_t;

syncDetectionState_t syncDetectionState = { 0 };

// update only when video mode changes.
typedef struct syncVideoTimings_s {
    uint32_t lineCounterDelayUs;
    uint32_t minimumFrameDelayUs; // FIXME not needed
} syncVideoTimings_t;

syncVideoTimings_t syncVideoTimings = { 0 };

void syncDetection_refreshSyncVideoTimings(void)
{
    const uint16_t requiredLines = 100; // ~64us * 100 = 6.4ms

    syncVideoTimings.lineCounterDelayUs = (videoTimings->lineNs / 1000) * (requiredLines);
    syncVideoTimings.minimumFrameDelayUs = (videoTimings->lineNs / 1000) * (videoTimings->lineCount + 10);
}

void syncDetection_reset(void)
{
    memset(&syncDetectionState, 0x00, sizeof(syncDetectionState));
}

typedef void (*eventHandlerFn_t)(uint32_t currentTime);
typedef void (*stateTransitionFn_t)(uint32_t currentTime);


void handleEvent(uint32_t currentTimeUs, uint32_t at, eventHandlerFn_t eventHandlerFn)
{
    bool handleEventNow = cmp32(currentTimeUs, at) > 0;

    if (handleEventNow) {
        eventHandlerFn(currentTimeUs);
    }
}

void handleStateTransition(uint32_t currentTimeUs, uint32_t at, stateTransitionFn_t stateTransitionFn)
{
    if (nextEventAt == 0) {
        logEvent(currentTimeUs, STATE_TRANSITION);

        stateTransitionFn(currentTimeUs);
    }
}

typedef enum {
    FIRST,
    BEST,
    LAST
} searchPhase_e; // maybe searchPoint is a better name?

#define SEARCH_PHASE_COUNT 3

typedef struct scanResult_s {
    uint16_t mv;
    uint16_t pulseErrorCount; // transient used to determine ratio.
    uint16_t lineCount;

    float pulseErrorsPerPeriod;
    float linesPerPeriod;
    float ratio;
} scanResult_t;

typedef struct searchState_s {
    searchPhase_e phase;
    uint16_t mv;
    uint16_t limitMv;

    uint32_t lineCounterAtStart;
    uint32_t pulseErrorsAtStart;

    scanResult_t scanResults[SEARCH_PHASE_COUNT];

    uint32_t lastTimeUs;
} searchState_t;

searchState_t searchState;

#define MAXIMIM_LINE_LEVEL_THRESHOLD_MV 2000
#define MAXIMIM_FRAME_LEVEL_THRESHOLD_MV (MAXIMIM_LINE_LEVEL_THRESHOLD_MV + 1000)
#define MAXIMIM_LINE_LEVEL_THRESHOLD_DIFFERENCE_MV 300

void syncDetection_prepareEvent_searching(uint32_t currentTimeUs)
{
#ifdef DEBUG_SYNC_DURATION
    syncDetectionState.syncStartedAt = currentTimeUs;
    syncDetectionState.syncCompletedAt = 0;
    syncDetectionState.syncDuration = 0;
#endif

    syncVoltages.minimumLevelForLineThreshold = 0;
    syncVoltages.minimumLevelForValidFrameMv = 0;
    syncVoltages.maximumLevelForValidFrameMv = 0;

    memset(&searchState, 0x00, sizeof(searchState));

    searchState.phase = FIRST;
    searchState.limitMv = MAXIMIM_FRAME_LEVEL_THRESHOLD_MV;
    searchState.lineCounterAtStart = frameState.lineCounter;
    searchState.pulseErrorsAtStart = frameState.totalPulseErrors;

    searchState.lastTimeUs = currentTimeUs;

    // always reset comparator target.
    setComparatorTargetMv(searchState.mv);

    disableComparatorBlanking();
}

void syncDetection_recordResult(scanResult_t *source, scanResult_t *destination)
{
    memcpy(destination, source, sizeof(scanResult_t));
}

void syncDetection_handleEvent_searching(uint32_t currentTimeUs)
{
    searchPhase_e nextSearchPhase = searchState.phase;
    int32_t timeDeltaUs = cmp32(currentTimeUs, searchState.lastTimeUs);

    if (searchState.mv != 0 && timeDeltaUs > 0) {
        // generate results and record them

        scanResult_t result;

        result.mv = searchState.mv;
        result.lineCount = frameState.lineCounter - searchState.lineCounterAtStart;
        result.pulseErrorCount = frameState.totalPulseErrors - searchState.pulseErrorsAtStart;

        result.linesPerPeriod = (float)timeDeltaUs / (result.lineCount + 1); // + 1 to avoid div/0 errors
        result.pulseErrorsPerPeriod = (float)timeDeltaUs / (result.pulseErrorCount + 1); // + 1 to avoid div/0 errors;

        result.ratio = result.linesPerPeriod / result.pulseErrorsPerPeriod;

        scanResult_t *scanResultToCompare = &searchState.scanResults[searchState.phase];

        switch(searchState.phase) {
            case FIRST: {
                 syncDetection_recordResult(&result, &searchState.scanResults[FIRST]);
                 
                if (result.lineCount > 0) {
                    syncDetection_recordResult(&result, &searchState.scanResults[BEST]);
                    nextSearchPhase = BEST;

                    syncVoltages.minimumLevelForLineThreshold = searchState.mv;

                    searchState.limitMv = searchState.mv + MAXIMIM_LINE_LEVEL_THRESHOLD_DIFFERENCE_MV;
                }
                break;
            }
            case BEST: {

                if (result.ratio < scanResultToCompare->ratio) {
                    syncDetection_recordResult(&result, &searchState.scanResults[BEST]);
                }
                break;
            }
            case LAST: {
                // Unused
                break;
            }
        }

        if (result.lineCount > 0) {
            syncDetection_recordResult(&result, &searchState.scanResults[LAST]);
            syncVoltages.maximumLevelForValidFrameMv = searchState.mv; // FIXME rename maximumLevelForValidFrameMv to maximumLevelForLineThreshold
        }
    }

    if (searchState.mv < searchState.limitMv) {

        // scan next band

        searchState.mv += 25;
        setComparatorTargetMv(searchState.mv);
        syncVoltages.syncThresholdMv = searchState.mv;

        searchState.lineCounterAtStart = frameState.lineCounter;
        searchState.pulseErrorsAtStart = frameState.pulseErrors;

        nextEventAt = currentTimeUs + syncVideoTimings.lineCounterDelayUs;

    } else {
        // reached end of scan range

        if (searchState.phase == FIRST) {
            syncDetectionState.syncDetectionFailureCount++;
            pixelOsdVideoState = RETRY;
            nextEventAt = 0;
            return;
        }
        syncDetectionState.syncDetectionFailureCount = 0; // Reset when sync is obtained.
        
        syncVoltages.minimumLevelForValidFrameMv = 0; // FIXME remove minimumLevelForValidFrameMv

        syncVoltages.syncThresholdMv = searchState.scanResults[BEST].mv;

#ifdef DEBUG_SYNC_MIN_MAX_THRESHOLD
        syncDetectionState.minMaxDifference = syncVoltages.maximumLevelForValidFrameMv - syncVoltages.minimumLevelForValidFrameMv;
#endif
        setComparatorTargetMv(syncVoltages.syncThresholdMv);

        setVideoSourceVoltageMv(syncVoltages.syncThresholdMv + 1100);

        pixelOsdVideoState = GENERATING_VIDEO;


        nextEventAt = 0;
    }

    searchState.lastTimeUs = currentTimeUs;
    searchState.phase = nextSearchPhase;
}

typedef struct generatingVideoState_s {
    uint32_t lastTimeUs;
    uint32_t lastTotalPulseErrors;
    uint32_t lastValidFrameCounter;
    uint32_t lastLineCounter;
    bool errorDetectionEnabled;

    uint32_t pulseErrorThreshold;
    uint32_t checkPeriodUs;
} generatingVideoState_t;

static generatingVideoState_t generatingVideoState;

const int framesRequiredPerCheckPeriod = 1;
const int maximumFramesPerCheckPeriod = framesRequiredPerCheckPeriod + 1;

void syncDetection_prepareEvent_generatingVideo(uint32_t currentTimeUs)
{
    logEvent(currentTimeUs, SYNC_OBTAINED);

#ifdef DEBUG_SYNC_DURATION
    syncDetectionState.syncCompletedAt = currentTimeUs;
    syncDetectionState.syncDuration = syncDetectionState.syncCompletedAt - syncDetectionState.syncStartedAt;
#endif

    generatingVideoState.lastTimeUs = currentTimeUs;
    generatingVideoState.lastTotalPulseErrors = frameState.totalPulseErrors;
    generatingVideoState.lastValidFrameCounter = frameState.validFrameCounter;
    generatingVideoState.errorDetectionEnabled = false;

    uint32_t maximumPulsesPerCheckPeriod = videoTimings->lineCount * maximumFramesPerCheckPeriod;
    generatingVideoState.pulseErrorThreshold = 10 * maximumPulsesPerCheckPeriod / 100;  // 10%

    generatingVideoState.checkPeriodUs = (syncVideoTimings.minimumFrameDelayUs * maximumFramesPerCheckPeriod);

    nextEventAt = currentTimeUs + generatingVideoState.checkPeriodUs;
    syncLossUpdateEventAt = currentTimeUs + (1000000); // 1 second
}

void syncDetection_handleEvent_generatingVideo(uint32_t currentTimeUs)
{
    if (generatingVideoState.errorDetectionEnabled) {

        // need to have had the `last*` variables initialised before it's possible to notice any change.

        uint32_t recentPulseErrors = frameState.totalPulseErrors - generatingVideoState.lastTotalPulseErrors;
        int32_t recentFrames = frameState.validFrameCounter - generatingVideoState.lastValidFrameCounter;
        uint32_t recentLines = frameState.lineCounter - generatingVideoState.lastLineCounter;

        DEBUG_SET(DEBUG_SPRACING_PIXEL_OSD, 2, recentPulseErrors);
        DEBUG_SET(DEBUG_SPRACING_PIXEL_OSD, 3, recentFrames);

        // look for at
        bool notEnoughFrames = recentFrames < framesRequiredPerCheckPeriod;
        bool tooManyPulseErrors = recentPulseErrors > generatingVideoState.pulseErrorThreshold;

        int32_t timeDeltaUs = cmp32(currentTimeUs, generatingVideoState.lastTimeUs);
        uint32_t maximumExpectedLines = (timeDeltaUs * 1000) / videoTimings->lineNs;

        bool notEnoughLines = recentLines < maximumExpectedLines / 2; // i.e. 50% of the lines are missing.
        if (notEnoughFrames || tooManyPulseErrors || notEnoughLines) {
            logEvent(currentTimeUs, SYNC_LOST);

            syncDetectionState.syncLossCount++;

            pixelOsdVideoState = RETRY;
            nextEventAt = 0;
            return;
        }
    } else {
        generatingVideoState.errorDetectionEnabled = true;
    }

    generatingVideoState.lastTimeUs = currentTimeUs;
    generatingVideoState.lastTotalPulseErrors = frameState.totalPulseErrors;
    generatingVideoState.lastValidFrameCounter = frameState.validFrameCounter;
    generatingVideoState.lastLineCounter = frameState.lineCounter;

    nextEventAt = currentTimeUs + generatingVideoState.checkPeriodUs;
}

void syncDetection_prepareEvent_retry(uint32_t currentTimeUs)
{
    nextEventAt = currentTimeUs + (1000000/10); // 1/10 of a second

    if (!syncDetectionState.uptimeIsHigh) {
        syncDetectionState.syncDetectionFailureThreshold = 1;
        syncDetectionState.syncLossThreshold = 1;
    } else {
        syncDetectionState.syncDetectionFailureThreshold = 2;
        syncDetectionState.syncLossThreshold = 10;
    }

    if (syncDetectionState.syncDetectionFailureCount > syncDetectionState.syncDetectionFailureThreshold || syncDetectionState.syncLossCount > syncDetectionState.syncLossThreshold) {

        // reset when switching source
        syncDetectionState.syncLossCount = 0;

        spracingPixelOSDPause();


        if (cameraConnected) {
            logEvent(currentTimeUs, ATTEMPTING_WITHOUT_CAMERA);

            cameraConnected = false;
        } else {

            logEvent(currentTimeUs, ATTEMPTING_WITH_CAMERA);
            cameraConnected = true;
        }

        spracingPixelOSDRestart();

        syncDetectionState.syncDetectionFailureCount = 0;

        nextEventAt = 0;
    }
}

void syncDetection_handleEvent_retry(uint32_t currentTimeUs)
{
    pixelOsdVideoState = SEARCHING;
    nextEventAt = 0;
}

void syncLoss_handleEvent_update(uint32_t currentTimeUs)
{
    syncLossUpdateEventAt = currentTimeUs + (1000000); // 1 second

    if (syncDetectionState.syncLossCount > 1) {
        syncDetectionState.syncLossCount--;
    }
}

void spracingPixelOSDService(timeUs_t currentTimeUs)
{
    // set a flag after uptime exceeds 5 seconds.
    if (!syncDetectionState.uptimeIsHigh && currentTimeUs > 5000000 ) {
        syncDetectionState.uptimeIsHigh = true;
    }

    syncDetection_refreshSyncVideoTimings(); // PERFORMANCE only update when video mode changed.

    switch(pixelOsdVideoState) {
        case RETRY:
        {
            handleStateTransition(currentTimeUs, nextEventAt, syncDetection_prepareEvent_retry);
            handleEvent(currentTimeUs, nextEventAt, syncDetection_handleEvent_retry);
            break;
        }
        case SEARCHING:
        {
            handleStateTransition(currentTimeUs, nextEventAt, syncDetection_prepareEvent_searching);
            handleEvent(currentTimeUs, nextEventAt, syncDetection_handleEvent_searching);
            break;
        }
        case GENERATING_VIDEO:
        {
            handleStateTransition(currentTimeUs, nextEventAt, syncDetection_prepareEvent_generatingVideo);
            handleEvent(currentTimeUs, nextEventAt, syncDetection_handleEvent_generatingVideo);

            handleEvent(currentTimeUs, syncLossUpdateEventAt, syncLoss_handleEvent_update);
            break;
        }
    }


    previousServiceAt = currentTimeUs;
}

#define DELAY_60_HZ (1000000 / 60)

void spracingPixelOSDRefreshState(timeUs_t currentTimeUs)
{
    extern volatile bool vSyncFlag;
    extern volatile bool fieldSyncFlag;
    extern spracingPixelOSDState_t spracingPixelOSDState;

    timeDelta_t serviceDeltaUs = currentTimeUs - previousServiceAt;

    bool deltaTooLong = serviceDeltaUs > DELAY_60_HZ; // FIXME use current video mode, (50/60hz)
    bool handleEventNow = cmp32(currentTimeUs, nextEventAt) > 0;

    spracingPixelOSDState.flags = 0;

    if (deltaTooLong || handleEventNow) {
      spracingPixelOSDState.flags |= PIXELOSD_FLAG_SERVICE_REQUIRED;
    }

    if (vSyncFlag) {
      spracingPixelOSDState.flags |= PIXELOSD_FLAG_VSYNC;

      vSyncFlag = false;
    }

    if (fieldSyncFlag) {
      spracingPixelOSDState.flags |= PIXELOSD_FLAG_FIELD_SYNC;

      fieldSyncFlag = false;
    }

    switch (detectedVideoSystem) {
        case VIDEO_SYSTEM_NTSC:
            spracingPixelOSDState.flags |= PIXELOSD_FLAG_NTSC_DETECTED;
        break;

        case VIDEO_SYSTEM_PAL:
            spracingPixelOSDState.flags |= PIXELOSD_FLAG_PAL_DETECTED;
        break;
        default:
        break;
    }
}

spracingPixelOSDSyncVoltages_t *spracingPixelOSDGetSyncVoltages(void)
{
    return &syncVoltages;
}
