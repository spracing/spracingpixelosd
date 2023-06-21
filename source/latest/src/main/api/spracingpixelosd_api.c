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

#include <stdint.h>

#include "time.h"

#include "spracingpixelosd_api.h"

// Unused, but linker should provide them.
extern const uint8_t __library_descriptor_start;
extern const uint8_t __library_vtable_start;

#define LIBRARY_DESCRIPTOR __attribute__((section(".library_descriptor")))  __attribute__((used))
#define LIBRARY_VTABLE __attribute__((section(".library_vtable")))  __attribute__((used))

// init.c
void spracingPixelFrameBufferCommit(uint8_t *frameBuffer);
void spracingPixelOSDInit(const spracingPixelOSDHostAPI_t *hostAPIFromClient, const spracingPixelOSDDefaultConfig_t *defaultConfig);
spracingPixelOSDState_t *spracingPixelOSDGetState(void);

// syncdetection.c
void spracingPixelOSDRefreshState(timeUs_t currentTimeUs);
void spracingPixelOSDService(timeUs_t currentTimeUs);
spracingPixelOSDSyncVoltages_t *spracingPixelOSDGetSyncVoltages(void);

// videoframe.c
void spracingPixelOSDRefreshFrameState(spracingPixelOSDFrameState_t *spracingPixelOSDFrameState);

#if !(defined(FLIGHT_ONE) || defined(BETAFLIGHT))
void SYNC_DMA_IRQHandler(void);
void PIXEL_DMA_IRQHandler(void);
void ADC_DMA_IRQHandler(void);
void COMPx_IRQHandler(void);
#endif

LIBRARY_VTABLE const spracingPixelOSDLibraryVTable_t libraryVTable = {
    .init = spracingPixelOSDInit,
    .getState = spracingPixelOSDGetState,
    .refreshState = spracingPixelOSDRefreshState,
    .refreshFrameState = spracingPixelOSDRefreshFrameState,
    .getSyncVoltages = spracingPixelOSDGetSyncVoltages,
    .service = spracingPixelOSDService,
    .frameBufferCommit = spracingPixelFrameBufferCommit,
    .comparatorIRQHandler = COMPx_IRQHandler,
    .syncDMAHandler = SYNC_DMA_IRQHandler,
    .pixelDMAHandler = PIXEL_DMA_IRQHandler,
    .adcDMAHandler = ADC_DMA_IRQHandler,
};

LIBRARY_DESCRIPTOR const spracingPixelOSDLibraryDescriptor_t libraryDescriptor = {
    .apiVersion = 1,
    .code = 0x4f30 // "O0"
};

const spracingPixelOSDLibraryDescriptor_t *spracingPixelOSDGetDescriptor(void) {
    return &libraryDescriptor;
}

const spracingPixelOSDLibraryVTable_t *spracingPixelOSDGetAPI(void) {
    return &libraryVTable;
}
