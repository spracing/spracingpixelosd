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

#include "platform.h"

#include "utils.h"

#include "configuration.h"

#include "videotiming.h"
#include "videosystem.h"

static const videoTimings_t ntscVideoTimings = {
  .lineNs = VIDEO_NTSC_LINE_LEN,
  .syncShortNs = VIDEO_NTSC_SYNC_SHORT,
  .syncHSyncNs = VIDEO_NTSC_SYNC_HSYNC,
//  .blankingNs = VIDEO_NTSC_BLANKING,
  .frontPorchNs = VIDEO_NTSC_FRONT_PORCH,
  .backPorchNs = VIDEO_NTSC_BACK_PORCH,

  .lineCount = NTSC_LINES,
};

static const videoTimings_t palVideoTimings = {
  .lineNs = VIDEO_PAL_LINE_LEN,
  .syncShortNs = VIDEO_PAL_SYNC_SHORT,
  .syncHSyncNs = VIDEO_PAL_SYNC_HSYNC,
//  .blankingNs = VIDEO_PAL_BLANKING,
  .frontPorchNs = VIDEO_PAL_FRONT_PORCH,
  .backPorchNs = VIDEO_PAL_BACK_PORCH,

  .lineCount = PAL_LINES,
};

static videoPulseTimings_t localVideoPulseTimings = {0};
videoPulseTimings_t *videoPulseTimings = &localVideoPulseTimings;

const videoTimings_t *videoTimings;

static void recalculatePulseTimings(uint32_t syncTimerHz, const videoTimings_t *vt, videoPulseTimings_t *vp)
{
  vp->lowBroad.periodNs = (vt->lineNs / 2) - vt->syncHSyncNs;

  vp->lowBroad.minNs = vp->lowBroad.periodNs - (vt->syncHSyncNs / 2);
  vp->lowBroad.maxNs = vp->lowBroad.periodNs + (vt->syncHSyncNs / 2);

  vp->lowBroad.minClocks = HZ_AND_NS_TO_CLOCKS(syncTimerHz, vp->lowBroad.minNs);
  vp->lowBroad.maxClocks = HZ_AND_NS_TO_CLOCKS(syncTimerHz, vp->lowBroad.maxNs);

  vp->highBroad.periodNs = vt->syncHSyncNs;

  vp->highVSync.periodNs = (vt->lineNs / 2) - vt->syncHSyncNs;
  vp->highShort.periodNs = (vt->lineNs / 2) - vt->syncShortNs;

  vp->highVSync.minNs = vp->highVSync.periodNs - (vp->highVSync.periodNs - vp->highBroad.periodNs) / 2;
  vp->highVSync.maxNs = vp->highVSync.periodNs + (vp->highShort.periodNs - vp->highVSync.periodNs) / 2;

  vp->highVSync.minClocks = HZ_AND_NS_TO_CLOCKS(syncTimerHz, vp->highVSync.minNs);
  vp->highVSync.maxClocks = HZ_AND_NS_TO_CLOCKS(syncTimerHz, vp->highVSync.maxNs);

  vp->lowShort.minNs = vt->syncShortNs / 2;
  vp->lowShort.maxNs = vt->syncShortNs + (vt->syncHSyncNs - vt->syncShortNs) / 2;

  vp->lowShort.minClocks = HZ_AND_NS_TO_CLOCKS(syncTimerHz, vp->lowShort.minNs);
  vp->lowShort.maxClocks = HZ_AND_NS_TO_CLOCKS(syncTimerHz, vp->lowShort.maxNs);

  vp->lowVSync.minNs = vt->syncHSyncNs - (vt->syncHSyncNs - vt->syncShortNs) / 2;
  vp->lowVSync.maxNs = vt->syncHSyncNs + (vp->lowBroad.periodNs - vt->syncHSyncNs) / 2;

  vp->lowVSync.minClocks = HZ_AND_NS_TO_CLOCKS(syncTimerHz, vp->lowVSync.minNs);
  vp->lowVSync.maxClocks = HZ_AND_NS_TO_CLOCKS(syncTimerHz, vp->lowVSync.maxNs);
}

void refreshVideoTimings(uint32_t syncTimerHz, videoSystem_t videoSystem)
{
  switch (videoSystem) {
  default:
  case VIDEO_SYSTEM_PAL:
    videoTimings = &palVideoTimings;
    break;
  case VIDEO_SYSTEM_NTSC:
    videoTimings = &ntscVideoTimings;
    break;
  }

  recalculatePulseTimings(syncTimerHz, videoTimings, videoPulseTimings);
}
