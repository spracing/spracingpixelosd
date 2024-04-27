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
#include "drivers/io.h"
#include "drivers/time.h"
#elif defined(FLIGHT_ONE)
#else
#include "time.h"
#include "utils.h"
#endif


#include "configuration.h"
#include "io.h"
#include "syncgeneration.h"
#include "videosystem.h"
#include "videotiming.h"
#include "pixelgeneration.h"
#include "pixelbuffer.h"

#include "spracingpixelosd_impl.h"

#include "api/spracingpixelosd_api.h"

#include "videoframe.h"

extern COMP_HandleTypeDef hcompx;
extern const spracingPixelOSDHostAPI_t *hostAPI;

typedef enum fieldType_e {
    FIELD_EVEN = 0,
    FIELD_ODD = 1,

    FIELD_FIRST = 0,
    FIELD_SECOND = 1
} fieldType_t;

typedef enum fieldPhase_e {
    FIELD_UNKNOWN = 0,
    FIELD_PRE_EQUALIZING,
    FIELD_SYNCRONIZING,
    FIELD_POST_EQUALIZING,
    FIELD_HSYNC,
} fieldPhase_t;

typedef struct fieldState_s {
    uint8_t preEqualizingPulses;
    uint8_t syncronizingPulses;
    uint8_t postEqualizingPulses;
    fieldType_t type;
    fieldPhase_t phase;
    uint16_t lineNumber;

    uint16_t highestFieldLineNumber;
    videoSystem_t videoSystem;
} fieldState_t;


volatile int16_t frameFallingEdgeIndex = 0;
volatile uint8_t fallingEdgesRemainingBeforeNewField = 0;

uint16_t firstVisibleLine;
uint16_t lastVisibleLine;
bool nextLineIsVisible;
volatile uint16_t visibleLineIndex;

// flags are set in ISR and cleared by state retrieval
volatile bool vSyncFlag = false;
volatile bool fieldSyncFlag = false;

volatile uint16_t fillLineIndex = 0;

uint8_t *committedFrameBuffer = NULL;
uint8_t *outputFrameBuffer = NULL;


#ifdef DEBUG_PULSE_STATISTICS
uint16_t syncPulseRisingStatisticIndex = 0;
uint16_t syncPulseRisingStatistics[PAL_LINES] __attribute__((used));
uint16_t syncPulseFallingStatisticIndex = 0;
uint16_t syncPulseFallingStatistics[PAL_LINES] __attribute__((used));
#endif

typedef struct {
#ifdef DEBUG_COMP_TRIGGER
    uint32_t triggerLowCount;
    uint32_t triggerHighCount;
#endif
    bool fallingEdge;
} compState_t;

compState_t compState = { 0 };

static volatile uint16_t pulseLength __attribute__((used)) = 0;
static volatile uint16_t previousPulseLength __attribute__((used)) = 0;

fieldState_t fieldState = { 0 };
frameState_t frameState = { 0 };


uint32_t targetMv = 0;

//
// Sync blanking
//

uint32_t blankingClocks = 0;

void recalculateBlankingTimings(const videoTimings_t *vt)
{
    uint32_t syncTimerHz = getTimerHz(htim1.Instance);

    uint32_t blankingNs = vt->lineNs - vt->frontPorchNs - vt->syncHSyncNs - VIDEO_COMPARATOR_TO_IRQ_OFFSET_NS;
    blankingClocks = HZ_AND_NS_TO_CLOCKS(syncTimerHz, blankingNs); // ~57250ns for PAL
}

void disableComparatorBlanking(void)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_5, htim1.Init.Period + 1); // never reached.
    LL_TIM_OC_SetMode((&htim1)->Instance, LL_TIM_CHANNEL_CH5, LL_TIM_OCMODE_FORCED_ACTIVE);
    //__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_5, htim1.Init.Period + 1);
#ifdef DEBUG_BLANKING
    LL_TIM_OC_SetMode((&htim1)->Instance, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_FORCED_ACTIVE);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, htim1.Init.Period + 1); // never reached.
    //__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, htim1.Init.Period + 1);
#endif
}

static inline void setBlankingPeriod(uint32_t clocks) {
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_5, clocks);
#ifdef DEBUG_BLANKING
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, clocks);
#endif
}

//
// Sync level
//

void setComparatorTargetMv(uint32_t newTargetMv)
{
    // High-saturation colors in the picture data should not cause false comparator triggers.
    //
    //      color burst          /----\             /-----\.
    //                |    -----/      \----   ----/       \---
    //                v   |                | |                 |
    //  --        --\/\/\--                | |                 --
    //    | SYNC |                         |_|   <-- false comparator trigger needs to be avoided
    //    |______|
    //    ^      ^
    //    |      |
    //    |      Rising edge of Sync
    //    Falling edge of Sync
    //
    // Comparator threshold MV should be as low as possible to detect sync voltages without triggering
    // on low voltages causes by color bust or high-saturation colors.

    // IMPORTANT: The voltage keeps drifting the longer the camera has been on (rises over time)
    // TODO: auto-correct targetMv based on sync length (shorter = nearer lower level, longer = nearer high level)

    targetMv = newTargetMv;

    // TODO get measured VREF via ADC and use instead of VIDEO_DAC_VCC here?
    uint32_t dacComparatorRaw = (targetMv * 0x0FFF) / (VIDEO_DAC_VCC * 1000);

    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_12B_R, dacComparatorRaw);
}

//
// Sync handling
//

void videoFrame_reset(void)
{
    memset(&frameState, 0x00, sizeof(frameState));
    memset(&fieldState, 0x00, sizeof(fieldState));
}

static inline void pulseError(void) {
#ifdef DEBUG_PULSE_ERRORS
    pixelDebug2Toggle();
#endif
    frameState.pulseErrors++;
    frameState.totalPulseErrors++;
#ifdef DEBUG_PULSE_ERRORS
    pixelDebug2Toggle();
#endif
}

#ifdef USE_HAL_COMP_CALLBACK
void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp)
{
#else
void RAW_COMP_TriggerCallback(void)
{
    COMP_HandleTypeDef *hcomp = &hcompx;

    uint32_t exti_line = COMP_GET_EXTI_LINE(hcomp->Instance);
    __HAL_COMP_EXTI_CLEAR_FLAG(exti_line);

#endif
    static uint16_t previousCompare = 0;
    uint16_t currentCompare = (&htim2)->Instance->CCR4;
    pulseLength = currentCompare - previousCompare;

    compState.fallingEdge = (HAL_COMP_GetOutputLevel(hcomp) == COMP_OUTPUT_LEVEL_HIGH);
    if (compState.fallingEdge) {
#ifdef DEBUG_COMP_TRIGGER
        compState.triggerLowCount++;
        pixelDebug2Low();
#endif

        // VIDEO_SYNC_VSYNC_MIN ((uint32_t)((((64.000 / 2.0) - 4.700) - (((64.000 / 2.0) - 4.700) - (4.700))/2.0) * (80000000 / 1000000)))
        // VIDEO_SYNC_VSYNC_MAX ((uint32_t)((((64.000 / 2.0) - 4.700) + (((64.000 / 2.0) - 2.000) - ((64.000 / 2.0) - 4.700))/2.0) * (80000000 / 1000000)))
        if (pulseLength > videoPulseTimings->highVSync.minClocks && pulseLength < videoPulseTimings->highVSync.maxClocks) {
            if (previousPulseLength > videoPulseTimings->lowVSync.minClocks) {

                // depending on the video mode a half frame pulse occurs at different times
                // use the detected mode to figure out if this is the first or second field.

                switch (fieldState.videoSystem) {
                case VIDEO_SYSTEM_PAL:
                    fieldState.type = FIELD_SECOND; // pulse occurs at end of second field
                    break;
                case VIDEO_SYSTEM_NTSC:
                    fieldState.type = FIELD_FIRST;  // pulse occurs at end of first field
                    break;
                default:
                    frameState.status = WAITING_FOR_FIRST_FIELD;
                    break;
                }

#ifdef DEBUG_LAST_HALF_LINE
                pixelDebug2Low();
                pixelDebug2High();
#endif
            } else {
                pulseError();
            }
        }

        frameFallingEdgeIndex++;

#ifdef DEBUG_PULSE_STATISTICS
        syncPulseFallingStatistics[syncPulseFallingStatisticIndex] = pulseLength;
        syncPulseFallingStatisticIndex++;
        if (syncPulseFallingStatisticIndex >= sizeof(syncPulseFallingStatistics) / sizeof(syncPulseFallingStatistics[0])) {
            syncPulseFallingStatisticIndex = 0;
        }
#endif


    } else {
#ifdef DEBUG_COMP_TRIGGER
        compState.triggerHighCount++;
        pixelDebug2High();
#endif

        disableComparatorBlanking();

        //
        // check pulse lengths in order from shortest to longest.
        //
        if (pulseLength < videoPulseTimings->lowShort.minClocks) {
            pulseError();
        } else if (pulseLength < videoPulseTimings->lowShort.maxClocks) {
#ifdef DEBUG_SHORT_PULSE
            pixelDebug2Low();
#endif

            if (frameState.status == WAITING_FOR_FIRST_FIELD) {
                frameState.status = COUNTING_PRE_EQUALIZING_PULSES;

                fieldState.phase = FIELD_PRE_EQUALIZING;
                fieldState.preEqualizingPulses = 1; // this one

            } else if (frameState.status == COUNTING_PRE_EQUALIZING_PULSES) {
                fieldState.preEqualizingPulses++;
            } else if (frameState.status == COUNTING_POST_EQUALIZING_PULSES) {
                fieldState.postEqualizingPulses++;
            } else if (frameState.status == COUNTING_SYNCRONIZING_PULSES) {
                frameState.status = COUNTING_POST_EQUALIZING_PULSES;

                fieldState.phase = FIELD_POST_EQUALIZING;
                fieldState.postEqualizingPulses = 1; // this one

                videoSystem_t previousVideoMode = fieldState.videoSystem;
                if (fieldState.syncronizingPulses == 5) { // PAL
                    fieldState.videoSystem = VIDEO_SYSTEM_PAL;
                    firstVisibleLine = 15;
                    lastVisibleLine = (PAL_VISIBLE_LINES - 1);
                } else if (fieldState.syncronizingPulses == 6) { // NTSC
                    fieldState.videoSystem = VIDEO_SYSTEM_NTSC;
                    firstVisibleLine = 15;
                    lastVisibleLine = (NTSC_VISIBLE_LINES - 1);
                }

                if (previousVideoMode == VIDEO_SYSTEM_UNKNOWN && fieldState.videoSystem != VIDEO_SYSTEM_UNKNOWN) {
                    uint32_t syncTimerHz = getTimerHz(htim1.Instance);

                    refreshVideoTimings(syncTimerHz, fieldState.videoSystem);
                    configureSyncGeneration(syncTimerHz, fieldState.videoSystem);
                    recalculateBlankingTimings(videoTimings);
                    reconfigureVideoTimers(videoTimings);
                }


                if (fieldState.type == FIELD_ODD) {
                    firstVisibleLine--;
                }
                lastVisibleLine += firstVisibleLine;

            } else if (frameState.status == COUNTING_HSYNC_PULSES) {
                frameState.status = COUNTING_PRE_EQUALIZING_PULSES;

                if (fieldState.type == FIELD_SECOND) {
                    if (frameState.pulseErrors == 0) {
                        frameState.validFrameCounter++;
                        if (fieldState.videoSystem != detectedVideoSystem) {
                            detectedVideoSystem = fieldState.videoSystem; // maybe always do it. this line is handy for breakpoints though.
                        }
                    }
                    frameState.pulseErrors = 0;
                }

                fieldState.phase = FIELD_PRE_EQUALIZING;
                fieldState.preEqualizingPulses = 1; // this one
            } else {
                pulseError();
            }

#ifdef DEBUG_SHORT_PULSE
            pixelDebug2High();
#endif

        } else if (pulseLength < videoPulseTimings->lowVSync.maxClocks) {

            //
            // Important start DMA *NOW* - then deal with remaining state.
            //
            bool thisLineIsVisible = nextLineIsVisible;
            if (thisLineIsVisible /* && pixelOsdState == GENERATING_VIDEO*/) {
                // DMA configured for next line (below) or by transfer-complete handler.
                pixelStartDMA();

                visibleLineIndex++;

                //
                // blank the comparator if the line is visible and later when the DMA completes, disable blanking.
                //

                // Now = rising pulse of HSYNC.

                LL_TIM_OC_SetMode((&htim1)->Instance, LL_TIM_CHANNEL_CH5, LL_TIM_OCMODE_FORCED_INACTIVE);
                LL_TIM_OC_SetMode((&htim1)->Instance, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_FORCED_INACTIVE);

                setBlankingPeriod(((&htim1)->Instance->CNT + blankingClocks) % (&htim1)->Instance->ARR);

                LL_TIM_OC_SetMode((&htim1)->Instance, LL_TIM_CHANNEL_CH5, TIM_OCMODE_ACTIVE);
                LL_TIM_OC_SetMode((&htim1)->Instance, LL_TIM_CHANNEL_CH3, TIM_OCMODE_ACTIVE);

            }

            switch(frameState.status) {
            case COUNTING_POST_EQUALIZING_PULSES:
                frameState.status = COUNTING_HSYNC_PULSES;

                //
                // HSync detected
                //
                fieldState.phase = FIELD_HSYNC;

            FALLTHROUGH;

            case COUNTING_HSYNC_PULSES:

                fieldState.lineNumber++;
                frameState.lineCounter++;

                // prepare for next line
                nextLineIsVisible = fieldState.lineNumber >= firstVisibleLine && fieldState.lineNumber <= lastVisibleLine;

                if (nextLineIsVisible) {
                    uint8_t *previousOutputPixelBuffer = outputPixelBuffer;

                    outputPixelBuffer = fillPixelBuffer;

                    fillLineIndex = visibleLineIndex;

                    fillPixelBuffer = previousOutputPixelBuffer;
                    if (!thisLineIsVisible) {
                        // if this line is visible the transfer-complete handler would configure the DMA
                        pixelConfigureDMAForNextField();
                    }

                    pixelBuffer_fillFromFrameBuffer(fillPixelBuffer, outputFrameBuffer, fillLineIndex);
                }

                if (fieldState.lineNumber > fieldState.highestFieldLineNumber) {
                    fieldState.highestFieldLineNumber = fieldState.lineNumber;
                }
            break;
            default:
                pulseError();
            }

        } else if (pulseLength >= videoPulseTimings->lowBroad.minClocks && pulseLength <= videoPulseTimings->lowBroad.maxClocks) {

            if (frameState.status == COUNTING_PRE_EQUALIZING_PULSES) {
                frameState.status = COUNTING_SYNCRONIZING_PULSES;

                fieldState.syncronizingPulses = 1; // this one

                fieldState.phase = FIELD_SYNCRONIZING;

                if (fieldState.type == FIELD_SECOND) {
                    fieldState.type = FIELD_FIRST;

                    frameState.frameStartCounter++;
                    vSyncFlag = true;
                } else {
                    fieldState.type = FIELD_SECOND;
                }

                fieldSyncFlag = true;

                if (committedFrameBuffer) {
                    outputFrameBuffer = committedFrameBuffer;
                    committedFrameBuffer = NULL;

#ifdef DEBUG_FRAMEBUFFER_COMMITS
                    pixelDebug1Toggle();
#endif
                }

                fieldState.lineNumber = 0;
                visibleLineIndex = 0;
                nextLineIsVisible = false;

#ifdef DEBUG_FIELD_START
                pixelDebug2Low();
#endif

                hostAPI->onVSync();
            } else if (frameState.status == COUNTING_POST_EQUALIZING_PULSES) {
                fieldState.type = FIELD_SECOND;
#ifdef DEBUG_FIELD_START
                pixelDebug2High();
#endif
            } else if (frameState.status == COUNTING_SYNCRONIZING_PULSES) {
                fieldState.syncronizingPulses++;
            } else {
                pulseError();
            }

#ifdef DEBUG_FIRST_SYNC_PULSE
            bool firstSyncPulse = (fieldState.syncronizingPulses == 0);
            if (firstSyncPulse) {
                pixelDebug2Low();
            } else if (fieldState.syncronizingPulses == 1) {
                pixelDebug2High();
            }
#endif
        } else {
            pulseError();
        }


#ifdef DEBUG_PULSE_STATISTICS
        syncPulseRisingStatistics[syncPulseRisingStatisticIndex] = pulseLength;
        syncPulseRisingStatisticIndex++;
        if (syncPulseRisingStatisticIndex >= sizeof(syncPulseRisingStatistics) / sizeof(syncPulseRisingStatistics[0])) {
            syncPulseRisingStatisticIndex = 0;
        }
#endif
    }

    previousPulseLength = pulseLength;
    previousCompare = currentCompare;
}

void COMPx_IRQHandler(void)
{
#ifdef USE_HAL_COMP_CALLBACK
  HAL_COMP_IRQHandler(&hcompx);
#else
  RAW_COMP_TriggerCallback();
#endif
}

void spracingPixelOSDRefreshFrameState(spracingPixelOSDFrameState_t *spracingPixelOSDFrameState)
{
    spracingPixelOSDFrameState->frameErrorCounter = frameState.frameStartCounter - frameState.validFrameCounter;
    spracingPixelOSDFrameState->validFrameCounter = frameState.validFrameCounter;
    spracingPixelOSDFrameState->totalPulseErrors = frameState.totalPulseErrors;
}
