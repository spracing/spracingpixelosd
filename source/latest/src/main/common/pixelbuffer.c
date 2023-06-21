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

#include "api/spracingpixelosd_framebuffer_api.h"

#include "pixelbuffer.h"

#define PIXEL_WHITE_ON 1
#define PIXEL_WHITE_OFF 0

#define PIXEL_MASK_ON 1
#define PIXEL_MASK_OFF 0

// black is inverted (open drain)
#define PIXEL_BLACK_ON 0
#define PIXEL_BLACK_OFF 1

#define PIXEL_WHITE       ((PIXEL_WHITE_ON  << PIXEL_WHITE_BIT) | (PIXEL_BLACK_OFF << PIXEL_BLACK_BIT))
#define PIXEL_BLACK       ((PIXEL_WHITE_OFF << PIXEL_WHITE_BIT) | (PIXEL_BLACK_ON  << PIXEL_BLACK_BIT))
#define PIXEL_GREY        ((PIXEL_WHITE_ON  << PIXEL_WHITE_BIT) | (PIXEL_BLACK_ON  << PIXEL_BLACK_BIT))
#define PIXEL_TRANSPARENT ((PIXEL_WHITE_OFF << PIXEL_WHITE_BIT) | (PIXEL_BLACK_OFF << PIXEL_BLACK_BIT))

#define PIXEL_WITH_MASK     (PIXEL_MASK_ON << PIXEL_MASK_ENABLE_BIT)
#define PIXEL_WITHOUT_MASK  (PIXEL_MASK_OFF << PIXEL_MASK_ENABLE_BIT)

void pixelBuffer_fillFromFrameBuffer(uint8_t *destinationPixelBuffer, uint8_t *frameBuffer, uint16_t lineIndex)
{
    // Rev B has 4 IO lines for White Source, Black, Mask and White, black and white are NOT adjacent so the bits cannot be copied and shifted together...
#ifdef DEBUG_PIXEL_BUFFER_FILL
    pixelDebug2Toggle();
#endif

    uint8_t *frameBufferLine = frameBuffer + (FRAME_BUFFER_LINE_SIZE * lineIndex);

    uint32_t *pixels = (uint32_t *)destinationPixelBuffer;
    for (int i = 0; i < FRAME_BUFFER_LINE_SIZE; i++) {
        uint8_t frameBlock = *(frameBufferLine + i);

        uint32_t frameBlockBits = (
            ((frameBlock & (0x03 << 0)) >> (BITS_PER_PIXEL * 0) << 24) |
            ((frameBlock & (0x03 << 2)) >> (BITS_PER_PIXEL * 1) << 16) |
            ((frameBlock & (0x03 << 4)) >> (BITS_PER_PIXEL * 2) << 8) |
            ((frameBlock & (0x03 << 6)) >> (BITS_PER_PIXEL * 3) << 0)
        );

        uint32_t blackGpioBitMask  = ((1 << 24) | (1 << 16) | (1 << 8) | (1 << 0)) << PIXEL_BLACK_BIT;
        uint32_t whiteGpioBitMask  = ((1 << 24) | (1 << 16) | (1 << 8) | (1 << 0)) << PIXEL_WHITE_BIT;
        uint32_t whiteSourceSelectGpioBitMask  = ((1 << 24) | (1 << 16) | (1 << 8) | (1 << 0)) << PIXEL_WHITE_SOURCE_SELECT_BIT;
        uint32_t maskGpioBitMask   = ((1 << 24) | (1 << 16) | (1 << 8) | (1 << 0)) << PIXEL_MASK_ENABLE_BIT;

        // gpio/frame level for black is inverted, so 0 = ON, 1 = OFF.

        uint32_t gpioBlackBits = (frameBlockBits << (PIXEL_BLACK_BIT - FRAME_BLACK_BIT_OFFSET)) & blackGpioBitMask;
        uint32_t gpioWhiteBits = (frameBlockBits << (PIXEL_WHITE_BIT - FRAME_WHITE_BIT_OFFSET)) & whiteGpioBitMask;
        //uint32_t gpioWhiteSourceSelectBits = (frameBlockBits << (PIXEL_WHITE_SOURCE_SELECT_BIT - FRAME_WHITE_BIT_OFFSET)) & whiteSourceSelectGpioBitMask;

        uint32_t gpioNotBlackBits = ~(gpioBlackBits) & blackGpioBitMask; // now 1 = ON, 0 = OFF, for each black bit.

        uint32_t frameMaskOnBlackBits    = gpioBlackBits >> (PIXEL_BLACK_BIT);
        //uint32_t frameMaskOnNotBlackBits = gpioNotBlackBits >> (PIXEL_BLACK_BIT);
        uint32_t frameMaskOnWhiteBits    = gpioWhiteBits >> (PIXEL_WHITE_BIT);

        uint32_t gpioMaskOnBlackBits = (frameMaskOnBlackBits << PIXEL_MASK_ENABLE_BIT) & maskGpioBitMask;
        //uint32_t gpioMaskOnNotBlackBits = (frameMaskOnNotBlackBits << PIXEL_MASK_ENABLE_BIT) & maskGpioBitMask;
        uint32_t gpioMaskOnWhiteBits = (frameMaskOnWhiteBits << PIXEL_MASK_ENABLE_BIT) & maskGpioBitMask;

        //uint32_t gpioWhiteBitsForEachBlackOn = (frameMaskOnNotBlackBits << PIXEL_WHITE_BIT) & whiteGpioBitMask;

        //uint32_t gpioWhiteSourceSelectBitsForEachBlackOn = (frameMaskOnNotBlackBits << PIXEL_WHITE_SOURCE_SELECT_BIT) & whiteSourceSelectGpioBitMask;
        uint32_t gpioWhiteSourceSelectBitsForEachBlackOn = (frameMaskOnBlackBits << PIXEL_WHITE_SOURCE_SELECT_BIT) & whiteSourceSelectGpioBitMask;

        //uint32_t gpioBlackBitsForEachWhiteOn = ~(frameMaskOnWhiteBits << PIXEL_BLACK_BIT) & blackGpioBitMask;

        // Black = DAC, never-sinked, masked, White = fixed,masked
        uint32_t gpioBits = blackGpioBitMask | gpioWhiteBits | gpioMaskOnWhiteBits | gpioMaskOnBlackBits | gpioWhiteSourceSelectBitsForEachBlackOn;

#ifdef DEBUG_PATTERN_BARS
        const int lineOffset = 32;
        const int linesPerPattern = 4;
        const int patternCount = 16; // 4 IO lines = 16 combinations
        if (lineIndex < lineOffset || lineIndex >= lineOffset + (patternCount * linesPerPattern)) {
            *pixels++ = gpioBits;
        } else {

            uint8_t pattern = (((lineIndex - lineOffset) / linesPerPattern) % patternCount);

            uint32_t patternBits = (pattern << 24) | (pattern << 16) | (pattern << 8) | (pattern << 0);
            uint32_t gpioPatternBits = patternBits << PIXEL_CONTROL_FIRST_BIT;
            *pixels++ = gpioPatternBits;
        }
#else
        *pixels++ = gpioBits;
#endif // DEBUG_PATTERN_BARS

    }
    destinationPixelBuffer[PIXEL_COUNT] = PIXEL_TRANSPARENT & ~(PIXEL_MASK_ON << PIXEL_MASK_ENABLE_BIT); // IMPORTANT!  The white source/black sink must be disabled before the SYNC signal, otherwise we change the sync voltage level.
#ifdef DEBUG_PIXEL_BUFFER_FILL
    pixelDebug2Toggle();
#endif
}

#ifdef DEBUG
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
#endif
