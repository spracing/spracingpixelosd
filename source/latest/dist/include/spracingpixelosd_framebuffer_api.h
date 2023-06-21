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

#define BITS_PER_PIXEL 2

#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE 8
#endif

#define FRAME_BUFFER_LINE_SIZE  ((SPRACING_PIXEL_OSD_HORIZONTAL_RESOLUTION / BITS_PER_BYTE) * BITS_PER_PIXEL)
#define FRAME_BUFFER_SIZE       (FRAME_BUFFER_LINE_SIZE * SPRACING_PIXEL_OSD_PAL_VISIBLE_LINES)

#define FRAME_WHITE_ON 1
#define FRAME_WHITE_OFF 0
#define FRAME_BLACK_ON 1
#define FRAME_BLACK_OFF 0

#define FRAME_BLACK_BIT_OFFSET 0
#define FRAME_WHITE_BIT_OFFSET 1

#define FRAME_PIXEL_WHITE       ((FRAME_WHITE_ON  << FRAME_WHITE_BIT_OFFSET) | (FRAME_BLACK_OFF << FRAME_BLACK_BIT_OFFSET))
#define FRAME_PIXEL_BLACK       ((FRAME_WHITE_OFF << FRAME_WHITE_BIT_OFFSET) | (FRAME_BLACK_ON  << FRAME_BLACK_BIT_OFFSET))
#define FRAME_PIXEL_GREY        ((FRAME_WHITE_ON  << FRAME_WHITE_BIT_OFFSET) | (FRAME_BLACK_ON  << FRAME_BLACK_BIT_OFFSET))
#define FRAME_PIXEL_TRANSPARENT ((FRAME_WHITE_OFF << FRAME_WHITE_BIT_OFFSET) | (FRAME_BLACK_OFF << FRAME_BLACK_BIT_OFFSET))
