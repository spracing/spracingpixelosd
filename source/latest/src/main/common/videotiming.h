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

#pragma once

//
// **GPL** FIXME Some of the time definitions in this file come from TinyOSD which is GPL code, replace prior to release.
//

#include "videosystem.h"

//
// Video Format
//

#define NTSC_LINES 525
#define VIDEO_NTSC_LINE_LEN            63556  // ns
#define VIDEO_NTSC_SYNC_SHORT           2000  // ns
#define VIDEO_NTSC_SYNC_HSYNC           4700  // ns
#define VIDEO_NTSC_BLANKING            10900  // ns
#define VIDEO_NTSC_FRONT_PORCH          1500  // ns
#define VIDEO_NTSC_BACK_PORCH           (VIDEO_NTSC_BLANKING - VIDEO_NTSC_FRONT_PORCH - VIDEO_NTSC_SYNC_HSYNC) // 6200
#define VIDEO_NTSC_SYNC_LO_BROAD       ((VIDEO_NTSC_LINE_LEN / 2) - VIDEO_NTSC_SYNC_HSYNC)

#define VIDEO_NTSC_BROAD_HSYNC_PULSES      6
#define VIDEO_NTSC_TOTAL_HSYNC_PULSES      (6 + VIDEO_NTSC_BROAD_HSYNC_PULSES + 6)

#define PAL_LINES 625
#define VIDEO_PAL_LINE_LEN            64000  // ns
#define VIDEO_PAL_SYNC_SHORT           2000  // ns
#define VIDEO_PAL_SYNC_HSYNC           4700  // ns
#define VIDEO_PAL_BLANKING            12050  // ns
#define VIDEO_PAL_FRONT_PORCH          1650  // ns
#define VIDEO_PAL_BACK_PORCH           (VIDEO_PAL_BLANKING - VIDEO_PAL_FRONT_PORCH - VIDEO_PAL_SYNC_HSYNC) // 5700
#define VIDEO_PAL_SYNC_LO_BROAD       ((VIDEO_PAL_LINE_LEN / 2) - VIDEO_PAL_SYNC_HSYNC)

#define VIDEO_PAL_BROAD_HSYNC_PULSES      5
#define VIDEO_PAL_TOTAL_HSYNC_PULSES      (5 + VIDEO_PAL_BROAD_HSYNC_PULSES + 5)


//
// Video Timing
//

typedef struct videoTimings_s {
  uint16_t lineNs;
  uint16_t syncShortNs;
  uint16_t syncHSyncNs;
//  uint16_t blankingNs;
  uint16_t frontPorchNs;
  uint16_t backPorchNs;

  uint16_t lineCount;
} videoTimings_t; // TODO maybe rename to videoFormatConfiguration_t; `lineCount` is not a `Timing` now, but it belongs in this structure.

typedef struct videoPulse_s {
  uint16_t periodNs;
  // TODO remove intermediate variable min/maxNs variables if possible.
  uint16_t minNs;
  uint16_t maxNs;
  uint16_t minClocks;
  uint16_t maxClocks;
} videoPulse_t;

typedef struct videoPulseTimings_s {
  videoPulse_t highBroad;
  videoPulse_t highVSync;
  videoPulse_t highShort;
  videoPulse_t lowBroad;
  videoPulse_t lowVSync;
  videoPulse_t lowShort;
} videoPulseTimings_t;

//
// Active timings/configuration
//
extern videoPulseTimings_t *videoPulseTimings;
extern const videoTimings_t *videoTimings;

void refreshVideoTimings(uint32_t syncTimerHz, videoSystem_t videoSystem);
