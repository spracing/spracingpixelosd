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
#include <stdint.h>

#include "platform.h"

#include "utils.h"

#include "configuration.h"
#include "glue.h"
#include "videotiming.h"
#include "spracingpixelosd_impl.h"

#include "syncgeneration.h"

//
// Sync Generation
//

bool syncDMAActive = false;

DMA_HandleTypeDef *hSyncOutDMA;

typedef struct syncBufferItem_s {
    // the order and size of these structure is fixed.  the data is transferred by DMA to the timer peripheral, starting with the ARR register
    uint16_t arrValue;
    uint16_t repetitions; // timers only support 8 bit RCR.
    uint16_t cc1Value;
} syncBufferItem_t;

#define SYNC_BUST_TRANSFER_COUNT TIM_DMABURSTLENGTH_3TRANSFERS

typedef enum {
    HALF,
    FULL,
} dsiLineWidth_e;

typedef struct syncBufferItemSource_s {
    dsiLineWidth_e lineWidth;
    uint8_t repetitions;
    uint16_t ns;
} syncBufferItemSource_t;

const syncBufferItemSource_t dynamicPalSyncSourceItems[] = {
    { HALF, 5,   VIDEO_PAL_SYNC_LO_BROAD},  // start of first field
    { HALF, 5,   VIDEO_PAL_SYNC_SHORT},
    { FULL, 153, VIDEO_PAL_SYNC_HSYNC},     // start of picture data
    { FULL, 152, VIDEO_PAL_SYNC_HSYNC},
    { HALF, 5,   VIDEO_PAL_SYNC_SHORT},
    { HALF, 5,   VIDEO_PAL_SYNC_LO_BROAD},  // start of second field
    { HALF, 4,   VIDEO_PAL_SYNC_SHORT},
    { FULL, 1,   VIDEO_PAL_SYNC_SHORT},     // second half of a line
    { FULL, 152, VIDEO_PAL_SYNC_HSYNC},     // start of picture data
    { FULL, 152, VIDEO_PAL_SYNC_HSYNC},
    { HALF, 1,   VIDEO_PAL_SYNC_HSYNC},     // first half of a line/frame
    { HALF, 5,   VIDEO_PAL_SYNC_SHORT},
    // 625 lines (2.5+2.5+153+152+2.5+2.5+2+1+152+152+.5+2.5)
};

const syncBufferItemSource_t dynamicNtscSyncSourceItems[] = {
    { HALF, 6,   VIDEO_NTSC_SYNC_LO_BROAD},  // start of first field
    { HALF, 6,   VIDEO_NTSC_SYNC_SHORT},
    { FULL, 127, VIDEO_NTSC_SYNC_HSYNC},     // start of picture data
    { FULL, 127, VIDEO_NTSC_SYNC_HSYNC},
    { HALF, 1,   VIDEO_NTSC_SYNC_HSYNC},     // first half of a line/frame
    { HALF, 6,   VIDEO_NTSC_SYNC_SHORT},
    { HALF, 6,   VIDEO_NTSC_SYNC_LO_BROAD},  // start of second field
    { HALF, 5,   VIDEO_NTSC_SYNC_SHORT},
    { FULL, 1,   VIDEO_NTSC_SYNC_SHORT},     // second half of a line
    { FULL, 126, VIDEO_NTSC_SYNC_HSYNC},     // start of picture data
    { FULL, 127, VIDEO_NTSC_SYNC_HSYNC},
    { HALF, 6,   VIDEO_NTSC_SYNC_SHORT},
    // 525 lines (3+3+127+127+.5+3+3+2.5+1+127+127+3)
};

LIBRARY_D1_RAM syncBufferItem_t dynamicSyncItems[12];


const syncBufferItem_t *videoSyncItems = dynamicSyncItems;
uint16_t videoSyncItemsSize = 0;

#define REPETITIONS_TO_RCR(repetitions) (repetitions - 1) // RCR = 0 = ONCE, RCR = 1 = TWICE, ...

void generateDynamicSyncItems(videoSystem_t videoSystem, uint32_t timerHz, const syncBufferItemSource_t *syncBufferSourceItems, const int syncBufferSourceItemCount, syncBufferItem_t *syncItems, const uint8_t syncItemsSize, uint16_t *syncItemsBufferSize)
{
    const int maximumSyncItems = syncItemsSize / sizeof(syncBufferItem_t);

    if (syncBufferSourceItemCount > maximumSyncItems) {
        return; // destination buffer too small
    }

    uint16_t lineLength = (videoSystem == VIDEO_SYSTEM_PAL ? VIDEO_PAL_LINE_LEN : VIDEO_NTSC_LINE_LEN);


    for (int index = 0; index < syncBufferSourceItemCount; index++) {
        const syncBufferItemSource_t *sourceItem = &syncBufferSourceItems[index];
        syncBufferItem_t *destinationItem = &syncItems[index];

        destinationItem->repetitions = REPETITIONS_TO_RCR(sourceItem->repetitions);

        if (sourceItem->lineWidth == HALF)
            destinationItem->arrValue = HZ_AND_NS_TO_CLOCKS(timerHz, lineLength / 2) - 1;
        else {
            destinationItem->arrValue = HZ_AND_NS_TO_CLOCKS(timerHz, lineLength) - 1;
        }
        destinationItem->cc1Value = HZ_AND_NS_TO_CLOCKS(timerHz, sourceItem->ns) - 1;
    }
    *syncItemsBufferSize = syncBufferSourceItemCount * sizeof(syncBufferItem_t);
}

void configureSyncGeneration(uint32_t syncTimerHz, videoSystem_t videoSystem)
{
    switch (videoSystem) {
    default:
    case VIDEO_SYSTEM_PAL:
        generateDynamicSyncItems(videoSystem, syncTimerHz, dynamicPalSyncSourceItems, ARRAYLEN(dynamicPalSyncSourceItems), dynamicSyncItems, sizeof(dynamicSyncItems), &videoSyncItemsSize);
        break;
    case VIDEO_SYSTEM_NTSC:
        generateDynamicSyncItems(videoSystem, syncTimerHz, dynamicNtscSyncSourceItems, ARRAYLEN(dynamicNtscSyncSourceItems), dynamicSyncItems, sizeof(dynamicSyncItems), &videoSyncItemsSize);
        break;
    }

#if defined(USE_VIDEO_SYNC_DMA_CACHE_MANAGEMENT)
    // DMA might be active NOW, 'clean' the cache.  like the solution is to stop the sync DMA if this needs to be updated.
    uint32_t alignedAddr = (uint32_t)videoSyncItems & ~0x1F;
    SCB_CleanDCache_by_Addr((uint32_t*)alignedAddr, videoSyncItemsSize + ((uint32_t)videoSyncItems - alignedAddr));
#endif
}

void syncStartPWM(void)
{
    if (HAL_TIM_PWM_Start(&htim1, SYNC_TIMER_CHANNEL) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_TIMEx_PWMN_Start(&htim1, SYNC_TIMER_CHANNEL) != HAL_OK)
    {
      Error_Handler();
    }
}

void syncStopPWM(void)
{
    if (HAL_TIM_PWM_Stop(&htim1, SYNC_TIMER_CHANNEL) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_TIMEx_PWMN_Stop(&htim1, SYNC_TIMER_CHANNEL) != HAL_OK)
    {
      Error_Handler();
    }

    //HAL_GPIO_WritePin(SYNC_OUT_GPIO_Port, SYNC_OUT_Pin, GPIO_PIN_RESET); // XXX - In GPIO AF TIM mode this has no effect, PWM IDLE state is what matters.
}

void syncStartDMA(void)
{
    if (HAL_TIM_DMABurst_MultiWriteStart(
        &htim1,
        TIM_DMABASE_ARR,
        TIM_DMA_UPDATE,
        (uint32_t *)videoSyncItems,
        SYNC_BUST_TRANSFER_COUNT,
        videoSyncItemsSize / 2 // 2 because each item is uint16_t, not uint32_t?
    ) != HAL_OK) {
        Error_Handler();
    }

    syncDMAActive = true;
}

void syncStopDMA(void)
{
    if (hSyncOutDMA->State == HAL_DMA_STATE_BUSY) {
        if (HAL_TIM_DMABurst_WriteStop(&htim1, TIM_DMA_UPDATE) != HAL_OK) {
            Error_Handler();
        }
    }
}

void syncInit(void)
{
    hSyncOutDMA = htim1.hdma[TIM_DMA_ID_UPDATE];

    syncStopPWM();
    syncStopDMA();

    if (!cameraConnected) {
        syncStartDMA();
        syncStartPWM();
    }
}
