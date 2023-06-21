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

#if defined(BETAFLIGHT)
#include "drivers/io.h"
#endif

#include "configuration.h"
#include "io.h"
#include "glue.h"

#include "spracingpixelosd_impl.h"

#include "pixelgeneration.h"

//
// Pixel Generation
//

#define PIXEL_BUFFER_SIZE PIXEL_COUNT + 1 // one more pixel which must always be transparent to reset output level during sync

// NOTE: for optimal CPU usage the current design requires that the pixel black and white GPIO bits are adjacent.
// BLACK bit must be before WHITE bit.
#ifdef USE_PIXEL_OUT_GPIOE
#define PIXEL_ODR       GPIOE->ODR
#endif

#ifdef USE_PIXEL_OUT_GPIOC
#define PIXEL_ODR       GPIOC->ODR
#endif

#ifdef USE_PIXEL_OUT_GPIOB
#define PIXEL_ODR       GPIOB->ODR
#endif

#define PIXEL_ADDRESS   ((uint32_t)&(PIXEL_ODR) + (PIXEL_ODR_OFFSET / 8)) // +1 for upper 8 bits

PIXEL_BUFFER_DMA_RAM uint8_t pixelBufferA[PIXEL_BUFFER_SIZE] __attribute__((aligned(32))) __attribute__((used));
PIXEL_BUFFER_DMA_RAM uint8_t pixelBufferB[PIXEL_BUFFER_SIZE] __attribute__((aligned(32))) __attribute__((used));

uint8_t *fillPixelBuffer = NULL;
uint8_t *outputPixelBuffer = NULL;


DMA_HandleTypeDef *hPixelOutDMA;
bool pixelDMAActive = false;

void pixelOutputDisable(void)
{
    bool blackOn = cameraConnected ? false : true;
    //bool blackOn = true;

    HAL_GPIO_WritePin(SPRACING_PIXEL_OSD_BLACK_SINK_GPIO_Port, SPRACING_PIXEL_OSD_BLACK_SINK_Pin, blackOn ? GPIO_PIN_RESET : GPIO_PIN_SET);

    // turn off black/white
    HAL_GPIO_WritePin(SPRACING_PIXEL_OSD_WHITE_SOURCE_FIXED_GPIO_Port, SPRACING_PIXEL_OSD_WHITE_SOURCE_FIXED_Pin, GPIO_PIN_RESET);

    // TODO reset mask and white source select
}

void pixelConfigureDMAForNextField(void)
{
#ifdef DEBUG_PIXEL_BUFFER
    pixelDebug2Set(fillPixelBuffer == pixelBufferA);
#endif

    if (HAL_DMA_Start_IT(hPixelOutDMA, (uint32_t)fillPixelBuffer, PIXEL_ADDRESS, PIXEL_BUFFER_SIZE) != HAL_OK)
    {
      //Error_Handler(); // FIXME the first time this is called it fails.
    }

}

void pixelXferCpltCallback(struct __DMA_HandleTypeDef *hdma)
{
    UNUSED(hdma);

#ifdef DEBUG_PIXEL_DMA
    pixelDebug1Low();
#endif

#ifdef STOP_START_PIXEL_TIMER
    (&htim15)->Instance->CR1 &= ~(TIM_CR1_CEN); // stop
    (&htim15)->Instance->SMCR &= ~TIM_SMCR_SMS; // disable slave mode
#endif

    pixelOutputDisable();

    __HAL_TIM_DISABLE_DMA(&htim15, TIM_DMA_CC1);

    //disableComparatorBlanking();


    pixelConfigureDMAForNextField();

    pixelDMAActive = false;
}

void pixelInit(void)
{
    hPixelOutDMA = htim15.hdma[TIM_DMA_ID_CC1];
    hPixelOutDMA->XferCpltCallback = pixelXferCpltCallback;

    HAL_TIM_Base_Start(&htim15);
    if (HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1) != HAL_OK)
    {
      // PWM Generation Error
      Error_Handler();
    }

    // Pre-configure DMA, DMA is started in COMP IRQ Handler
    outputPixelBuffer = pixelBufferA;
    fillPixelBuffer = pixelBufferB;
    if (HAL_DMA_Start_IT(hPixelOutDMA, (uint32_t)outputPixelBuffer, PIXEL_ADDRESS, PIXEL_BUFFER_SIZE) != HAL_OK)
    {
      Error_Handler();
    }
}

void pixelGateAndBlankStart(void)
{
    // OC4REF used to gate TIM15
    if (HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_4) != HAL_OK)
    {
      Error_Handler();
    }

    // OC2 used to trigger ADC conversion.
    if (HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2) != HAL_OK)
    {
      Error_Handler();
    }

#ifdef DEBUG_GATING
#error DEBUG_GATING now clashes with ADC triggering, fix it.
    if (HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_2) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_2) != HAL_OK)
    {
      Error_Handler();
    }
#endif

    // OC5 used as comparator blanking
    if (HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_5) != HAL_OK)
    {
      Error_Handler();
    }
#ifdef DEBUG_BLANKING
    if (HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_3) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_TIMEx_OCN_Start(&htim1, TIM_CHANNEL_3) != HAL_OK)
    {
      Error_Handler();
    }
#endif
}

void pixelStartDMA(void)
{
#ifdef DEBUG_PIXEL_DMA
    pixelDebug1High();
#endif

    (&htim15)->Instance->CNT = 0;
#ifdef STOP_START_PIXEL_TIMER
    (&htim15)->Instance->SMCR |= TIM_SLAVEMODE_GATED;
    (&htim15)->Instance->CR1 |= (TIM_CR1_CEN);
#endif

    __HAL_TIM_ENABLE_DMA(&htim15, TIM_DMA_CC1);

    pixelDMAActive = true;
}

void pixelStopDMA(void)
{
    __HAL_TIM_DISABLE_DMA(&htim15, TIM_DMA_CC1);

    HAL_DMA_Abort(hPixelOutDMA);

    pixelDMAActive = false;

    pixelOutputDisable();

}
