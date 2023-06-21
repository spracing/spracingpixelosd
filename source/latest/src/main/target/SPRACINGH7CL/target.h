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
 * SP Racing H7 CL - by Dominic Clifton
 *
 * Author: Dominic Clifton - Sync generation, Sync Detection, Video Overlay and first-cut of working OSD system.
 */
#pragma once

#ifndef SPRACINGH7CL_REV
#define SPRACINGH7CL_REV 1 // REV A
#endif

#define SPRACING_PIXEL_OSD_BLACK_SINK_PIN               PE12
#define SPRACING_PIXEL_OSD_BLACK_SINK_Pin               GPIO_PIN_12
#define SPRACING_PIXEL_OSD_BLACK_SINK_GPIO_Port         GPIOE

#define SPRACING_PIXEL_OSD_WHITE_SOURCE_FIXED_PIN       PE13
#define SPRACING_PIXEL_OSD_WHITE_SOURCE_FIXED_Pin       GPIO_PIN_13
#define SPRACING_PIXEL_OSD_WHITE_SOURCE_FIXED_GPIO_Port GPIOE

#define SPRACING_PIXEL_OSD_WHITE_SOURCE_SELECT_PIN      PE15
#define SPRACING_PIXEL_OSD_WHITE_SOURCE_SELECT_Pin          GPIO_PIN_15
#define SPRACING_PIXEL_OSD_WHITE_SOURCE_SELECT_GPIO_Port    GPIOE

#define SPRACING_PIXEL_OSD_MASK_ENABLE_PIN              PE14
#define SPRACING_PIXEL_OSD_MASK_ENABLE_Pin              GPIO_PIN_14
#define SPRACING_PIXEL_OSD_MASK_ENABLE_GPIO_Port        GPIOE

#define SPRACING_PIXEL_OSD_SYNC_IN_PIN                  PE11 // COMP2_INP
#define SPRACING_PIXEL_OSD_SYNC_IN_Pin                  GPIO_PIN_11
#define SPRACING_PIXEL_OSD_SYNC_IN_GPIO_Port            GPIOE
#define SPRACING_PIXEL_OSD_SYNC_IN_COMP                 COMP2
#define SPRACING_PIXEL_OSD_SYNC_TIM1_ETR_REMAP          TIM_TIM1_ETR_COMP2_OUT
#define SPRACING_PIXEL_OSD_SYNC_IN_COMP_INPUT           COMP_INPUT_PLUS_IO2

#define SPRACING_PIXEL_OSD_SYNC_OUT_PIN                 PA8  // TIM1_CH1
#define SPRACING_PIXEL_OSD_SYNC_OUT_Pin                 GPIO_PIN_8
#define SPRACING_PIXEL_OSD_SYNC_OUT_GPIO_Port           GPIOA
#define USE_TIM1_CH1_FOR_SYNC
#define SYNC_TIMER_CHANNEL TIM_CHANNEL_1

#define SPRACING_PIXEL_OSD_WHITE_SOURCE_DAC_PIN         PA4  // DAC1_OUT1
#define SPRACING_PIXEL_OSD_VIDEO_THRESHOLD_DEBUG_PIN    PA5  // DAC1_OUT2
#define SPRACING_PIXEL_OSD_PIXEL_DEBUG_1_PIN            PE5  // TIM15_CH1 - For DMA updates
#define SPRACING_PIXEL_OSD_PIXEL_DEBUG_2_PIN            PE6  // TIM15_CH2 - Spare
#define SPRACING_PIXEL_OSD_PIXEL_GATING_DEBUG_PIN       PB0 // TIM1_CH2N // actual gating is on CH4
#define SPRACING_PIXEL_OSD_PIXEL_BLANKING_DEBUG_PIN     PB1 // TIM1_CH3N // actual blanking is on CH5

#define SPRACING_PIXEL_OSD_PIXEL_DEBUG_1_Pin            GPIO_PIN_5
#define SPRACING_PIXEL_OSD_PIXEL_DEBUG_1_GPIO_Port      GPIOE
#define SPRACING_PIXEL_OSD_PIXEL_DEBUG_2_Pin            GPIO_PIN_6
#define SPRACING_PIXEL_OSD_PIXEL_DEBUG_2_GPIO_Port      GPIOE

#define SPRACING_PIXEL_OSD_GATING_DEBUG_Pin             GPIO_PIN_0
#define SPRACING_PIXEL_OSD_GATING_DEBUG_GPIO_Port       GPIOB

#define SPRACING_PIXEL_OSD_BLANKING_DEBUG_Pin           GPIO_PIN_1
#define SPRACING_PIXEL_OSD_BLANKING_DEBUG_GPIO_Port     GPIOB

#define SPRACING_PIXEL_OSD_PIXEL_DEBUG_1_GPIO_AF        GPIO_AF4_TIM15
#define SPRACING_PIXEL_OSD_PIXEL_DEBUG_2_GPIO_AF        GPIO_AF4_TIM15
#define SPRACING_PIXEL_OSD_GATING_DEBUG_GPIO_AF         GPIO_AF1_TIM1
#define SPRACING_PIXEL_OSD_BLANKING_DEBUG_GPIO_AF       GPIO_AF1_TIM1

#define USE_PIXEL_OUT_GPIOE
#ifdef USE_PIXEL_OUT_GPIOE
#define PIXEL_BLACK_BIT                 4 // PE12
#define PIXEL_WHITE_BIT                 5 // PE13
#define PIXEL_WHITE_SOURCE_SELECT_BIT   7 // PE15
#define PIXEL_MASK_ENABLE_BIT           6 // PE14
#define PIXEL_CONTROL_FIRST_BIT PIXEL_BLACK_BIT
#define PIXEL_ODR_OFFSET 8 // 0 = PE0-PE7, 8 = PE8-PE15
#endif

#define SPRACING_PIXEL_OSD_VIDEO_IN_ADC_PIN             PC5
#define SPRACING_PIXEL_OSD_VIDEO_IN_ADC_Pin             GPIO_PIN_5
#define SPRACING_PIXEL_OSD_VIDEO_IN_GPIO_Port           GPIOC

#define SPRACING_PIXEL_OSD_VIDEO_OUT_ADC_PIN            PC4
#define SPRACING_PIXEL_OSD_VIDEO_OUT_ADC_Pin            GPIO_PIN_4
#define SPRACING_PIXEL_OSD_VIDEO_OUT_GPIO_Port          GPIOC

