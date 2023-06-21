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
#include <string.h>

#if !(defined(FLIGHT_ONE) || defined(BETAFLIGHT))

#include "platform.h"

#define HAL_GPIO_SET(NAME, VALUE) HAL_GPIO_WritePin( NAME ## _GPIO_Port, NAME ## _Pin, VALUE )
#define HAL_GPIO_HIGH(NAME) HAL_GPIO_SET(NAME, GPIO_PIN_SET)
#define HAL_GPIO_LOW(NAME) HAL_GPIO_SET(NAME, GPIO_PIN_RESET)
#define HAL_GPIO_TOGGLE(NAME) HAL_GPIO_TogglePin( NAME ## _GPIO_Port, NAME ## _Pin )

//
// OSD Pin Debugging
//
void pixelDebug1Set(bool state)
{
    if (state) {
        HAL_GPIO_HIGH(SPRACING_PIXEL_OSD_PIXEL_DEBUG_1);
    } else {
        HAL_GPIO_LOW(SPRACING_PIXEL_OSD_PIXEL_DEBUG_1);
    }
}

void pixelDebug2Set(bool state)
{
    if (state) {
        HAL_GPIO_HIGH(SPRACING_PIXEL_OSD_PIXEL_DEBUG_2);
    } else {
        HAL_GPIO_LOW(SPRACING_PIXEL_OSD_PIXEL_DEBUG_2);
    }
}

#if defined(SPRACING_PIXEL_OSD_LED_1_PIN)
void led1Set(bool state)
{
    if (state) {
        HAL_GPIO_HIGH(SPRACING_PIXEL_OSD_LED_1);
    } else {
        HAL_GPIO_LOW(SPRACING_PIXEL_OSD_LED_1);
    }
}
#endif

void pixelDebug1Low(void)
{
    HAL_GPIO_LOW(SPRACING_PIXEL_OSD_PIXEL_DEBUG_1);
}

void pixelDebug2Low(void)
{
    HAL_GPIO_LOW(SPRACING_PIXEL_OSD_PIXEL_DEBUG_2);
}

void pixelDebug1High(void)
{
    HAL_GPIO_HIGH(SPRACING_PIXEL_OSD_PIXEL_DEBUG_1);
}

void pixelDebug2High(void)
{
    HAL_GPIO_HIGH(SPRACING_PIXEL_OSD_PIXEL_DEBUG_2);
}

void pixelDebug1Toggle(void)
{
    HAL_GPIO_TOGGLE(SPRACING_PIXEL_OSD_PIXEL_DEBUG_1);
}

void pixelDebug2Toggle(void)
{
    HAL_GPIO_TOGGLE(SPRACING_PIXEL_OSD_PIXEL_DEBUG_2);
}

//
// Init
//


void spracingPixelOSD_initIO(void)
{
#if defined(STANDALONE_BUILD)
    // FUTURE just enabled the used ports.
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
#  if defined(STM32H7)
#    error MCU implementation required.
#  endif
#endif

  GPIO_InitTypeDef gpioInitStruct = {0};

  HAL_GPIO_HIGH(SPRACING_PIXEL_OSD_BLACK_SINK);

  gpioInitStruct.Mode  = GPIO_MODE_OUTPUT_OD;
  gpioInitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  gpioInitStruct.Pull  = GPIO_NOPULL;
  gpioInitStruct.Pin   = SPRACING_PIXEL_OSD_BLACK_SINK_Pin;
  HAL_GPIO_Init(SPRACING_PIXEL_OSD_BLACK_SINK_GPIO_Port, &gpioInitStruct);

#ifdef SPRACING_PIXEL_OSD_MASK_ENABLE_PIN
  HAL_GPIO_LOW(SPRACING_PIXEL_OSD_MASK_ENABLE);

  gpioInitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  gpioInitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  gpioInitStruct.Pull  = GPIO_PULLDOWN;
  gpioInitStruct.Pin   = SPRACING_PIXEL_OSD_MASK_ENABLE_Pin;
  HAL_GPIO_Init(SPRACING_PIXEL_OSD_MASK_ENABLE_GPIO_Port, &gpioInitStruct);
#endif

  HAL_GPIO_LOW(SPRACING_PIXEL_OSD_WHITE_SOURCE_FIXED);

  gpioInitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  gpioInitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  gpioInitStruct.Pull  = GPIO_NOPULL;
  gpioInitStruct.Pin   = SPRACING_PIXEL_OSD_WHITE_SOURCE_FIXED_Pin;
  HAL_GPIO_Init(SPRACING_PIXEL_OSD_WHITE_SOURCE_FIXED_GPIO_Port, &gpioInitStruct);

#ifdef SPRACING_PIXEL_OSD_WHITE_SOURCE_SELECT_PIN
  HAL_GPIO_LOW(SPRACING_PIXEL_OSD_WHITE_SOURCE_SELECT); // Low = Fixed Voltage, High = Linked to DAC1_OUT1 voltage.

  gpioInitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  gpioInitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  gpioInitStruct.Pull  = GPIO_PULLDOWN;
  gpioInitStruct.Pin   = SPRACING_PIXEL_OSD_WHITE_SOURCE_SELECT_Pin;
  HAL_GPIO_Init(SPRACING_PIXEL_OSD_WHITE_SOURCE_SELECT_GPIO_Port, &gpioInitStruct);
#endif

  HAL_GPIO_LOW(SPRACING_PIXEL_OSD_SYNC_IN);

  gpioInitStruct.Mode  = GPIO_MODE_INPUT;
  gpioInitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  gpioInitStruct.Pull  = GPIO_NOPULL;
  gpioInitStruct.Pin   = SPRACING_PIXEL_OSD_SYNC_IN_Pin;
  HAL_GPIO_Init(SPRACING_PIXEL_OSD_SYNC_IN_GPIO_Port, &gpioInitStruct);


#ifdef DEBUG_BLANKING
  HAL_GPIO_LOW(SPRACING_PIXEL_OSD_PIXEL_BLANKING_DEBUG);

  gpioInitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  gpioInitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  gpioInitStruct.Pull  = GPIO_PULLDOWN;
  gpioInitStruct.Pin   = SPRACING_PIXEL_OSD_PIXEL_BLANKING_DEBUG_Pin;
  HAL_GPIO_Init(SPRACING_PIXEL_OSD_PIXEL_BLANKING_DEBUG_GPIO_Port, &gpioInitStruct);
#endif

#ifdef DEBUG_GATING
  HAL_GPIO_LOW(SPRACING_PIXEL_OSD_PIXEL_GATING_DEBUG);

  gpioInitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  gpioInitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  gpioInitStruct.Pull  = GPIO_PULLDOWN;
  gpioInitStruct.Pin   = SPRACING_PIXEL_OSD_PIXEL_GATING_DEBUG_Pin;
  HAL_GPIO_Init(SPRACING_PIXEL_OSD_PIXEL_GATING_DEBUG_GPIO_Port, &gpioInitStruct);
#endif

  HAL_GPIO_LOW(SPRACING_PIXEL_OSD_PIXEL_DEBUG_1);

  gpioInitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  gpioInitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  gpioInitStruct.Pull  = GPIO_PULLDOWN;
  gpioInitStruct.Pin   = SPRACING_PIXEL_OSD_PIXEL_DEBUG_1_Pin;
  HAL_GPIO_Init(SPRACING_PIXEL_OSD_PIXEL_DEBUG_1_GPIO_Port, &gpioInitStruct);

  HAL_GPIO_LOW(SPRACING_PIXEL_OSD_PIXEL_DEBUG_2);

  gpioInitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  gpioInitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  gpioInitStruct.Pull  = GPIO_PULLDOWN;
  gpioInitStruct.Pin   = SPRACING_PIXEL_OSD_PIXEL_DEBUG_2_Pin;
  HAL_GPIO_Init(SPRACING_PIXEL_OSD_PIXEL_DEBUG_2_GPIO_Port, &gpioInitStruct);

#if defined(SPRACING_PIXEL_OSD_LED_1_PIN)
  HAL_GPIO_LOW(SPRACING_PIXEL_OSD_LED_1);

  gpioInitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  gpioInitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  gpioInitStruct.Pull  = GPIO_PULLDOWN;
  gpioInitStruct.Pin   = SPRACING_PIXEL_OSD_LED_1_Pin;
  HAL_GPIO_Init(SPRACING_PIXEL_OSD_LED_1_GPIO_Port, &gpioInitStruct);
#endif
}

#endif // !(FLIGHT_ONE || BETAFLIGHT)
