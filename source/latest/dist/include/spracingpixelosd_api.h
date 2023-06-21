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
 * SP Racing Pixel OSD Library by Dominic Clifton.
 */

#pragma once

#include <spracingpixelosd_conf.h>

#define PIXELOSD_FLAG_INITIALISED       (1 << 0)
#define PIXELOSD_FLAG_ERROR             (1 << 1)
#define PIXELOSD_FLAG_SERVICE_REQUIRED  (1 << 2)
#define PIXELOSD_FLAG_VSYNC             (1 << 3)
#define PIXELOSD_FLAG_FIELD_SYNC        (1 << 4)
#define PIXELOSD_FLAG_NTSC_DETECTED     (1 << 5)
#define PIXELOSD_FLAG_PAL_DETECTED      (1 << 6)

// CF = Configuration Flag
#define PIXELOSD_CF_VIDEO_SYSTEM_PAL    (1 << 0)
#define PIXELOSD_CF_VIDEO_SYSTEM_NTSC   (0 << 1)

// EC = ErrorCode
#define PIXELOSD_EC_UNSUPPORTED_TIMER_BUS_CLK       1

typedef struct spracingPixelOSDFrameState_s {
    uint16_t frameErrorCounter;
    uint16_t validFrameCounter;
    uint16_t totalPulseErrors;
} spracingPixelOSDFrameState_t;

typedef struct spracingPixelOSDSyncVoltages_s {
    uint16_t minimumLevelForLineThreshold;
    uint16_t minimumLevelForValidFrameMv;
    uint16_t maximumLevelForValidFrameMv;
    uint16_t syncThresholdMv;
} spracingPixelOSDSyncVoltages_t;

typedef struct spracingPixelOSDState_s {
    uint32_t flags;
    uint16_t errorCode;
} spracingPixelOSDState_t;

typedef struct pixelOSDDefaultConfig_s {
  uint32_t flags;
} spracingPixelOSDDefaultConfig_t;

typedef struct spracingPixelOSDHostAPI_s {
  // called by the OSD system when it needs to know the time, in microseconds.
  uint32_t (*micros)(void);

  // called by the OSD system when the next frame is about to be rendered
  // this is an ISR callback, primarily used by the OSD client system to perform the following tasks:
  // 1) swap frame buffers
  // 2) signal to the rest of the system that the previous framebuffer can now be rendered into.
  void (*onVSync)(void);
} spracingPixelOSDHostAPI_t;

typedef struct spracingPixelOSDLibraryVTable_s {
    // call once at system startup, non-reentrant.
    void (*init)(const spracingPixelOSDHostAPI_t *hostAPI, const spracingPixelOSDDefaultConfig_t *defaultConfig);

    // call once after initialisation, store the state handle for future use, contents not valid until `refreshState` is called.
    // memory for pixelOSDState_t is managed by OSD system
    spracingPixelOSDState_t *(*getState)(void);

    // call periodically and act on the updated spracingPixelOSDState obtained by spracingPixelOSDState_t as appropriate
    void (*refreshState)(uint32_t currentTimeUs);

    // memory for spracingPixelOSDFrameState_t is managed by OSD client system
    void (*refreshFrameState)(spracingPixelOSDFrameState_t *);

    // memory for spracingPixelOSDFrameState_t is managed by OSD system
    spracingPixelOSDSyncVoltages_t *(*getSyncVoltages)(void);

    // call when indicated by the PIXELOSD_FLAG_SERVICE_REQUIRED
    void (*service)(uint32_t currentTimeUs);

    // call once the framebuffer has been prepared
    void (*frameBufferCommit)(uint8_t *frameBuffer);

    void (*comparatorIRQHandler)(void);
    void (*syncDMAHandler)(void);
    void (*pixelDMAHandler)(void);
    void (*adcDMAHandler)(void);

} spracingPixelOSDLibraryVTable_t;

typedef struct spracingPixelOSDLibraryDescriptor_s {
    int16_t apiVersion;
    int16_t code;
} spracingPixelOSDLibraryDescriptor_t;

#define SPRACINGPIXELOSD_LIBRARY_CODE 0x4f30
#define SPRACINGPIXELOSD_LIBRARY_API_VERSION 0x01

