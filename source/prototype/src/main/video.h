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

//
// Video Format
//

#define PAL_LINES 625
#define NTSC_LINES 525


//
// Output Specification
//

#define PAL_VISIBLE_LINES 288  // MAX7456 (16 rows * 18 character height)
#define NTSC_VISIBLE_LINES 234 // MAX7456 (13 rows * 18 character height)

// 48us * 80 = 3840 clocks.  3840 clocks / 640 = 6 clocks per pixel.
// resolution scale of 2 = 12 clocks per pixel = 320 pixels.

#define HORIZONTAL_RESOLUTION 640
#define RESOLUTION_SCALE 2
#define OVERLAY_LENGTH 48.000 // us

//
// Pixel Buffer
//

#define PIXEL_COUNT (HORIZONTAL_RESOLUTION / RESOLUTION_SCALE)
#define PIXEL_BUFFER_SIZE PIXEL_COUNT + 1 // one more pixel which must always be transparent to reset output level during sync

extern uint8_t pixelBufferA[PIXEL_BUFFER_SIZE];
extern uint8_t pixelBufferB[PIXEL_BUFFER_SIZE];

#define USE_PIXEL_OUT_GPIOB

// NOTE: for optimal CPU usage the current design requires that the pixel black and white GPIO bits are adjacent.
// BLACK bit must be before WHITE bit.
#ifdef USE_PIXEL_OUT_GPIOC
#define PIXEL_ODR       GPIOC->ODR
#define PIXEL_ODR_OFFSET 8

#define PIXEL_BLACK_BIT 6 // PC14
#define PIXEL_WHITE_BIT 7 // PC15
#endif
#ifdef USE_PIXEL_OUT_GPIOB
#define PIXEL_ODR       GPIOB->ODR
#define PIXEL_ODR_OFFSET 0
//#define PIXEL_BLACK_BIT 6 // PB6
//#define PIXEL_WHITE_BIT 7 // PB7
#define PIXEL_BLACK_BIT 0 // PB0
#define PIXEL_WHITE_BIT 1 // PB1
#endif

#define BITS_PER_PIXEL 2 // the current implementation only supports 2.

#define PIXEL_ADDRESS   ((uint32_t)&(PIXEL_ODR) + (PIXEL_ODR_OFFSET / 8)) // +1 for upper 8 bits

//
// Frame Buffer
//

#define BITS_PER_BYTE 8

#define FRAME_BUFFER_LINE_SIZE ((PIXEL_COUNT / BITS_PER_BYTE) * BITS_PER_PIXEL)
#define FRAME_BUFFER_SIZE (FRAME_BUFFER_LINE_SIZE * PAL_VISIBLE_LINES)

extern uint8_t frameBuffers[2][FRAME_BUFFER_SIZE];

//
// Timing
//


#ifdef USE_NTSC
#define VIDEO_LINE_LEN            63.556  // us
#define VIDEO_SYNC_SHORT           2.000  // us
#define VIDEO_SYNC_HSYNC           4.700  // us
#else
#define VIDEO_LINE_LEN            64.000  // us
#define VIDEO_SYNC_SHORT           2.000  // us
#define VIDEO_SYNC_HSYNC           4.700  // us
#endif
#define VIDEO_FIELD_ODD            1
#define VIDEO_FIELD_EVEN           (1-VIDEO_FIELD_ODD)
#define VIDEO_FIRST_FIELD          VIDEO_FIELD_ODD     // ODD (NTSC)
#define VIDEO_SECOND_FIELD         (1-(VIDEO_FIRST_FIELD))

// timing for high level (from shortest to longest period)
// (1) [HI] BROAD SYNC: t ~ VIDEO_SYNC_HSYNC
// (2) [HI] VSYNC+DATA: t ~ (VIDEO_LINE_LEN / 2) - VIDEO_SYNC_HSYNC
// (3) [HI] SHORT SYNC: t ~ (VIDEO_LINE_LEN / 2) - VIDEO_SYNC_SHORT
// (4) [HI] VIDEO DATA: t ~ VIDEO_LINE_LEN - VIDEO_SYNC_HSYNC
//
#define VIDEO_SYNC_HI_BROAD     (VIDEO_SYNC_HSYNC)
#define VIDEO_SYNC_HI_VSYNC     ((VIDEO_LINE_LEN / 2.0) - VIDEO_SYNC_HSYNC)
#define VIDEO_SYNC_HI_SHORT     ((VIDEO_LINE_LEN / 2.0) - VIDEO_SYNC_SHORT)
#define VIDEO_SYNC_HI_DATA      (VIDEO_LINE_LEN)
//
// -> valid vsync = .... (1)---[xxx(2)xxx]---(3)------(4)
//
#define VIDEO_SYNC_VSYNC_MIN        _US_TO_CLOCKS(VIDEO_SYNC_HI_VSYNC - (VIDEO_SYNC_HI_VSYNC - VIDEO_SYNC_HI_BROAD)/2.0)
#define VIDEO_SYNC_VSYNC_MAX        _US_TO_CLOCKS(VIDEO_SYNC_HI_VSYNC + (VIDEO_SYNC_HI_SHORT - VIDEO_SYNC_HI_VSYNC)/2.0)

// timing for low level (from shortest to longest period)
// (1) [LO] SHORT SYNC: t ~ 2.0us
// (2) [LO] HSYNC     : t ~ 4.7us
// (3) [LO] BROAD     : t ~ (VIDEO_LINE_LEN / 2) - VIDEO_SYNC_HSYNC
//
//
// short sync =  (1)xxx]---(2)------(3)
//
#define VIDEO_SYNC_SHORT_MIN    _US_TO_CLOCKS(0)
#define VIDEO_SYNC_SHORT_MAX    _US_TO_CLOCKS(VIDEO_SYNC_SHORT +  (VIDEO_SYNC_HSYNC - VIDEO_SYNC_SHORT)/2.0)
//
// hsync      =  (1)---[xxx(2)xxx]---(3)
//
#define VIDEO_SYNC_HSYNC_MIN    _US_TO_CLOCKS(VIDEO_SYNC_HSYNC - (VIDEO_SYNC_HSYNC - VIDEO_SYNC_SHORT)/2.0)
#define VIDEO_SYNC_HSYNC_MAX    _US_TO_CLOCKS(VIDEO_SYNC_HSYNC + (VIDEO_SYNC_LO_BROAD - VIDEO_SYNC_HSYNC)/2.0)
//
// broad      = (1)------(2)---[xxx(3)]
//
#define VIDEO_SYNC_LO_BROAD       (VIDEO_LINE_LEN / 2.0) - VIDEO_SYNC_HSYNC
#define VIDEO_SYNC_LO_BROAD_MIN   _US_TO_CLOCKS(VIDEO_SYNC_LO_BROAD - VIDEO_SYNC_HSYNC/2.0)
#define VIDEO_SYNC_LO_BROAD_MAX   _US_TO_CLOCKS(VIDEO_SYNC_LO_BROAD + VIDEO_SYNC_HSYNC/2.0)

typedef enum {
    MODE_UNKNOWN = 0,
    MODE_PAL,
    MODE_NTSC
} videoMode_t;

extern videoMode_t videoMode;
extern volatile videoMode_t detectedVideoMode;
extern volatile bool videoModeChanged;

extern volatile bool cameraConnected;

void pixelBuffer_createTestPattern1(uint8_t *destinationPixelBuffer, uint8_t bands);

void frameBuffer_erase(uint8_t *frameBuffer);
void frameBuffer_createTestPattern1(uint8_t *frameBuffer);
void frameBuffer_createTestPattern2(uint8_t *frameBuffer);

void pixelBuffer_fillFromFrameBuffer(uint8_t *destinationPixelBuffer, uint8_t *frameBuffer, uint16_t lineIndex);

void frameBuffer_slowWriteCharacter(uint8_t *frameBuffer, uint16_t x, uint16_t y, uint8_t characterIndex);
void frameBuffer_writeString(uint8_t *frameBuffer, uint16_t x, uint16_t y, uint8_t *message, uint8_t messageLength);
