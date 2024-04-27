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

#include "videotiming.h"
//
// Implementation only
//


typedef enum {
    TIMER_TIM1 = 1,
    TIMER_TIM2 = 2,
    TIMER_TIM15 = 15
} timerId_e;

#define USED_TIMER_COUNT 3

extern TIM_HandleTypeDef htim1;   // Video Timer
extern TIM_HandleTypeDef htim2;   // Pulse timer
extern TIM_HandleTypeDef htim15;  // Pixel generation
extern COMP_HandleTypeDef hcomp; // Sync comparator
extern DAC_HandleTypeDef hdac1;   // Sync threshold and video level
extern ADC_HandleTypeDef hadc1;   // Video ADC measurement
extern DMA_HandleTypeDef hdma_adc1;

extern volatile bool cameraConnected;
extern uint8_t *fillPixelBuffer;
extern uint8_t *outputPixelBuffer;

void spracingPixelOSDPause(void);
void spracingPixelOSDRestart(void);

void setVideoSourceVoltageMv(uint32_t whiteMv);
void reconfigureVideoTimers(const videoTimings_t *vt);

uint32_t getTimerHz(TIM_TypeDef *instance);
uint32_t getTimerBusClockById(timerId_e id);
uint32_t getTimerBusClockByInstance(TIM_TypeDef *instance);
