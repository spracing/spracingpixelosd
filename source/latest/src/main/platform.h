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
 * SP Racing Pixel OSD by Dominic Clifton
 *
 * Author: Dominic Clifton - Sync generation, Sync Detection, Video Overlay and first-cut of working OSD system.
 */

#if defined(STM32H743xx) || defined(STM32H750xx) || defined(STM32H730xx)
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "system_stm32h7xx.h"

#include "stm32h7xx_ll_spi.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_dma.h"
#include "stm32h7xx_ll_rcc.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_tim.h"
#include "stm32h7xx_ll_system.h"

// Chip Unique ID on H7
#define U_ID_0 (*(uint32_t*)UID_BASE)
#define U_ID_1 (*(uint32_t*)(UID_BASE + 4))
#define U_ID_2 (*(uint32_t*)(UID_BASE + 8))

#ifndef STM32H7
#define STM32H7
#endif

#define USE_VIDEO_SYNC_DMA_CACHE_MANAGEMENT

#endif // STM32H743xx || defined(STM32H750xx

#if defined(STM32L432xx)
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "system_stm32l4xx.h"

#include "stm32l4xx_ll_tim.h"

#ifndef STM32L4
#define STM32L4
#endif

#endif // STM32L432xx

#include "target.h"

#if !defined(STANDALONE_BUILD)
#define USE_HAL_COMP_CALLBACK
#endif
