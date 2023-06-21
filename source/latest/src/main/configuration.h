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

#include "api/spracingpixelosd_conf.h"

//
// Memory
//
#ifdef BETAFLIGHT
#define PIXEL_BUFFER_DMA_RAM DMA_RAM
#define FRAME_BUFFER_DMA_RAM DMA_RAM
#endif

#ifndef PIXEL_BUFFER_DMA_RAM
#define PIXEL_BUFFER_DMA_RAM                     __attribute__((section(".pixelbuffer_ram")))
#endif

#ifndef FRAME_BUFFER_DMA_RAM
#define FRAME_BUFFER_DMA_RAM                     __attribute__((section(".framebuffer_ram")))
#endif

#ifndef LIBRARY_D1_RAM
#define LIBRARY_D1_RAM                           __attribute__((section(".library_d1_ram")))
#endif

#ifndef LIBRARY_D1_DATA
#define LIBRARY_D1_DATA                          __attribute__((section(".library_d1_data")))
#endif

#ifndef LIBRARY_D2_DATA
#define LIBRARY_D2_DATA                          __attribute__((section(".library_d2_data")))
#endif

//
// Horizontal
//

#define HORIZONTAL_RESOLUTION 720
#define RESOLUTION_SCALE 2

#define PIXEL_COUNT (HORIZONTAL_RESOLUTION / RESOLUTION_SCALE)

//
// Vertical
//

#define PAL_VISIBLE_LINES 288  // MAX7456 (16 rows * 18 character height)
#define NTSC_VISIBLE_LINES 234 // MAX7456 (13 rows * 18 character height)

//
// Timing
//

#define HZ_TO_US(hz)                                (hz / 1000000)
#define HZ_AND_NS_TO_CLOCKS(hz, ns)                 ((uint32_t)(((uint32_t)(ns) * HZ_TO_US(hz)) / 1000))


//
// It takes some time between the comparator being triggered and the IRQ handler being called.
// it can be measured by togging a GPIO high/low in the IRQ handler and measuring the time between
// the input signal and the gpio being toggled.
// Note: the value varies based on CPU clock-speed and compiler optimisations, i.e. DEBUG build = more time, faster CPU = less time.
//
#define VIDEO_COMPARATOR_TO_IRQ_OFFSET_US 0.4 // us
#define VIDEO_COMPARATOR_TO_IRQ_OFFSET_NS ((uint32_t)(VIDEO_COMPARATOR_TO_IRQ_OFFSET_US * 1000)) // ns

//
// Voltage
//

#define VIDEO_DAC_VCC 3.3


//
// DEBUG
//

#if 1
#define DEBUG_PIXEL_DMA           // debug led 1
#define DEBUG_COMP_TRIGGER        // debug led 2
// move debug items from below to this #if block to enable them
#else
#define DEBUG_PULSE_STATISTICS    // requires an additional 2.5KB ram for statistics.
#define DEBUG_PULSE_ERRORS        // debug led 2
#define DEBUG_OSD_EVENTS          // requires additional 2KB ram for event log.
#define DEBUG_BLANKING            // signal on M8
#define DEBUG_SYNC_DURATION
#define DEBUG_VIDEO_ADC           // debug led 1
#define DEBUG_FRAMEBUFFER_COMMITS // debug led 1
#define DEBUG_PIXEL_DMA           // debug led 1
#define DEBUG_COMP_TRIGGER        // debug led 2
#define DEBUG_GATING              // signal on M7
#define DEBUG_PATTERN_BARS
#define DEBUG_PIXEL_BUFFER_FILL
#define DEBUG_PIXEL_TIMER
#define DEBUG_LAST_HALF_LINE
#define DEBUG_PIXEL_BUFFER
#define DEBUG_SYNC_PWM
#define DEBUG_FIELD_START
#define DEBUG_SHORT_PULSE
#define DEBUG_FIRST_SYNC_PULSE
#endif
