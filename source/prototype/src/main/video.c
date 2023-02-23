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

#include "main.h"
#include "stdbool.h"
#include "string.h"
#include "video.h"

#include "font_max7456_12x18.h"

videoMode_t videoMode = MODE_NTSC;
volatile videoMode_t detectedVideoMode = MODE_UNKNOWN;
volatile bool videoModeChanged = false;

uint8_t pixelBufferA[PIXEL_BUFFER_SIZE];
uint8_t pixelBufferB[PIXEL_BUFFER_SIZE];

uint8_t frameBuffers[2][FRAME_BUFFER_SIZE];

#define PIXEL_WHITE_ON 1
#define PIXEL_WHITE_OFF 0
// black is inverted (open drain)
#define PIXEL_BLACK_ON 0
#define PIXEL_BLACK_OFF 1

#define PIXEL_WHITE       ((PIXEL_WHITE_ON  << PIXEL_WHITE_BIT) | (PIXEL_BLACK_OFF << PIXEL_BLACK_BIT))
#define PIXEL_BLACK       ((PIXEL_WHITE_OFF << PIXEL_WHITE_BIT) | (PIXEL_BLACK_ON  << PIXEL_BLACK_BIT))
#define PIXEL_GREY        ((PIXEL_WHITE_ON  << PIXEL_WHITE_BIT) | (PIXEL_BLACK_ON  << PIXEL_BLACK_BIT))
#define PIXEL_TRANSPARENT ((PIXEL_WHITE_OFF << PIXEL_WHITE_BIT) | (PIXEL_BLACK_OFF << PIXEL_BLACK_BIT))

void pixelBuffer_createTestPattern1(uint8_t *destinationPixelBuffer, uint8_t bands)
{
    uint8_t pattern = 0;
    uint8_t patterns = 8;
    uint8_t bandWidth = PIXEL_COUNT / bands;
    for (int i = 0; i < PIXEL_COUNT; i++) {
        uint8_t band = i / bandWidth;

        pattern = band % patterns;

        uint8_t pixelValue = 0x00;

        if (pattern == 0) {
            pixelValue = PIXEL_BLACK;
            if (i & 1) {
                pixelValue = PIXEL_WHITE;
            }
        } else if (pattern == 1) {
            pixelValue = PIXEL_WHITE;
        } else if (pattern == 2) {
            pixelValue = PIXEL_TRANSPARENT;
            if (i & 1) {
                pixelValue = PIXEL_BLACK;
            }
        } else if (pattern == 3) {
            pixelValue = PIXEL_TRANSPARENT;
        } else if (pattern == 4) {
            pixelValue = PIXEL_GREY;
        } else if (pattern == 5) {
            pixelValue = PIXEL_TRANSPARENT;
            if (i & 1) {
                pixelValue = PIXEL_WHITE;
            }
        } else if (pattern == 6) {
            pixelValue = PIXEL_WHITE;
            if (i & 1) {
                pixelValue = PIXEL_BLACK;
            }
        } else if (pattern == 7){
            pixelValue = PIXEL_BLACK;
        }

        destinationPixelBuffer[i] = pixelValue;
    }
    destinationPixelBuffer[PIXEL_COUNT] = PIXEL_TRANSPARENT; // IMPORTANT!  The white source/black sink must be disabled before the SYNC signal, otherwise we change the sync voltage level.
}

#define FRAME_PIXEL_WHITE       ((PIXEL_WHITE_ON  << 1) | (PIXEL_BLACK_OFF << 0))
#define FRAME_PIXEL_BLACK       ((PIXEL_WHITE_OFF << 1) | (PIXEL_BLACK_ON  << 0))
#define FRAME_PIXEL_GREY        ((PIXEL_WHITE_ON  << 1) | (PIXEL_BLACK_ON  << 0))
#define FRAME_PIXEL_TRANSPARENT ((PIXEL_WHITE_OFF << 1) | (PIXEL_BLACK_OFF << 0))

#define FRAME_PIXEL_MASK        ((1 << 1) | (1 << 0))

#define BLOCK_TRANSPARENT ((FRAME_PIXEL_TRANSPARENT << 6) | (FRAME_PIXEL_TRANSPARENT << 4) | (FRAME_PIXEL_TRANSPARENT << 2) | (FRAME_PIXEL_TRANSPARENT << 0))

#define PIXELS_PER_BYTE (8 / BITS_PER_PIXEL)

void frameBuffer_erase(uint8_t *frameBuffer)
{
    memset(frameBuffer, BLOCK_TRANSPARENT, FRAME_BUFFER_SIZE);
}

void pixelBuffer_fillFromFrameBuffer(uint8_t *destinationPixelBuffer, uint8_t *frameBuffer, uint16_t lineIndex)
{
    HAL_GPIO_TogglePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin);
    uint8_t *frameBufferLine = frameBuffer + (FRAME_BUFFER_LINE_SIZE * lineIndex);
    uint8_t *pixel = destinationPixelBuffer;
    for (int i = 0; i < FRAME_BUFFER_LINE_SIZE; i++) {
        uint8_t pixelBlock = *(frameBufferLine + i);

        uint8_t mask = (1 << 7) | ( 1 << 6); // only for BITS_PER_PIXEL == 2
        *pixel++ = (pixelBlock & mask) >> (BITS_PER_PIXEL * 3) << PIXEL_BLACK_BIT;

        mask = mask >> BITS_PER_PIXEL;
        *pixel++ = (pixelBlock & mask) >> (BITS_PER_PIXEL * 2) << PIXEL_BLACK_BIT;

        mask = mask >> BITS_PER_PIXEL;
        *pixel++ = (pixelBlock & mask) >> (BITS_PER_PIXEL * 1) << PIXEL_BLACK_BIT;

        mask = mask >> BITS_PER_PIXEL;
        *pixel++ = (pixelBlock & mask) >> (BITS_PER_PIXEL * 0) << PIXEL_BLACK_BIT;
    }

    destinationPixelBuffer[PIXEL_COUNT] = PIXEL_TRANSPARENT; // IMPORTANT!  The white source/black sink must be disabled before the SYNC signal, otherwise we change the sync voltage level.
    HAL_GPIO_TogglePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin);
}

void frameBuffer_createTestPattern1(uint8_t *frameBuffer)
{
    for (int lineIndex = 0; lineIndex < PAL_VISIBLE_LINES; lineIndex++) {
        uint8_t *lineBuffer = frameBuffer + (lineIndex * FRAME_BUFFER_LINE_SIZE);

        if (lineIndex & 0x8) {
            continue; // empty vertical band.
        }

        for (int i = 0; i < PIXEL_COUNT / PIXELS_PER_BYTE; i ++) {

            lineBuffer[i] =
                    (FRAME_PIXEL_WHITE << (BITS_PER_PIXEL * 3)) |
                    (FRAME_PIXEL_GREY << (BITS_PER_PIXEL * 2)) |
                    (FRAME_PIXEL_BLACK << (BITS_PER_PIXEL * 1)) |
                    (FRAME_PIXEL_TRANSPARENT << (BITS_PER_PIXEL * 0));
        }
    }
}

void frameBuffer_setPixel(uint8_t *frameBuffer, uint16_t x, uint16_t y, uint8_t mode)
{
    uint8_t *lineBuffer = frameBuffer + (y * FRAME_BUFFER_LINE_SIZE);

    uint8_t pixelOffsetInBlock = (PIXELS_PER_BYTE - 1) - (x % PIXELS_PER_BYTE);

    uint8_t pixelBitOffset = BITS_PER_PIXEL * pixelOffsetInBlock;

    uint8_t mask = ~(FRAME_PIXEL_MASK << pixelBitOffset);

    uint8_t before = lineBuffer[x / PIXELS_PER_BYTE];
    uint8_t withMaskCleared = before & mask;
    lineBuffer[x / PIXELS_PER_BYTE] = withMaskCleared |
            (mode << pixelBitOffset);
}

void frameBuffer_createTestPattern2(uint8_t *frameBuffer)
{
    for (int lineIndex = 0; lineIndex < PAL_VISIBLE_LINES; lineIndex++) {
        int x;

        x = lineIndex;
        frameBuffer_setPixel(frameBuffer, x, lineIndex, FRAME_PIXEL_BLACK);
        frameBuffer_setPixel(frameBuffer, x+1, lineIndex, FRAME_PIXEL_WHITE);
        frameBuffer_setPixel(frameBuffer, x+2, lineIndex, FRAME_PIXEL_BLACK);

        x = PIXEL_COUNT - 1 - lineIndex;
        frameBuffer_setPixel(frameBuffer, x, lineIndex, FRAME_PIXEL_BLACK);
        frameBuffer_setPixel(frameBuffer, x-1, lineIndex, FRAME_PIXEL_WHITE);
        frameBuffer_setPixel(frameBuffer, x-2, lineIndex, FRAME_PIXEL_BLACK);

    }
}

// unoptimized for now
void frameBuffer_slowWriteCharacter(uint8_t *frameBuffer, uint16_t x, uint16_t y, uint8_t characterIndex)
{
    uint16_t fontCharacterOffset = characterIndex * FONT_MAX7456_12x18_BYTES_PER_CHARACTER;

    for (int row = 0; row < FONT_MAX7456_HEIGHT; row++) {
        uint16_t fy = y + row;
        uint16_t fx = x;



        for (int b = 0; b < 3; b++) {
            uint8_t c = font_max7456_12x18[fontCharacterOffset];
            fontCharacterOffset++;

            for (int p = 0; p <= 3; p++) {
                uint8_t mp = (c >> (2 * (3 - p))) & ((1 << 1) | (1 << 0)); // extract max7456 pixel from character
                uint8_t mode = FRAME_PIXEL_TRANSPARENT;

                if (mp == ((0 << 1) | (0 << 0))) {
                    mode = FRAME_PIXEL_BLACK;
                } else if (mp == ((1 << 1) | (0 << 0))) {
                    mode = FRAME_PIXEL_WHITE;
                }
                if (mode != 0xFF) {
                    frameBuffer_setPixel(frameBuffer, fx, fy, mode);
                }
                fx++;
            }
        }

    }
}

void frameBuffer_writeString(uint8_t *frameBuffer, uint16_t x, uint16_t y, uint8_t *message, uint8_t messageLength)
{
    uint16_t fx = x;
    for (int mi = 0; mi < messageLength; mi++) {
        uint8_t c = message[mi];

        frameBuffer_slowWriteCharacter(frameBuffer, fx, y, font_max7456_12x18_asciiToFontMapping[c]);
        fx+= 12; // font width
    }
}
