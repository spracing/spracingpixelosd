/*
 * Author: Dominic Clifton - Sync generation, Sync Detection, Video Overlay and first-cut of working OSD system.
 */

#include <stdbool.h>
#include <stdint.h>
#ifdef FRAMEBUFFER_ERASE
#include <string.h>
#endif

#include "platform.h"

#include "api/spracingpixelosd_conf.h"
#include "api/spracingpixelosd_framebuffer_api.h"

#include "framebuffer.h"

#if defined(FRAMEBUFFER_EXTENDED_API)
#include "common/fonts/font_max7456_12x18.h"
#endif


// STM32H7 - framebuffers must be located in RAM_D2 region as they need to be accessible by DMA2 and MDMA.
// All     - framebuffers must always 32 bit aligned.

#if defined(RUNTIME_LIBRARY_BUILD)
// frame buffer memory configured via linker scripts
extern uint8_t __frame_buffer_start;
extern uint8_t __frame_buffer_end;

uint8_t *frameBuffers = &__frame_buffer_start;

FRAME_BUFFER_DMA_RAM uint8_t frameBufferReserved[2][FRAME_BUFFER_SIZE] __attribute__((used));
#else
uint8_t frameBuffersReserved[2][FRAME_BUFFER_SIZE];
uint8_t *frameBuffers = &frameBuffersReserved[0][0];
#endif


//
// Frame Buffer API
//

uint8_t *frameBuffer_getBuffer(uint8_t index)
{
    uint8_t *frameBuffer = frameBuffers + (FRAME_BUFFER_SIZE * index);
    return frameBuffer;
}

uint8_t frameBuffer_getBufferIndex(uint8_t *frameBuffer)
{
    return (frameBuffer - frameBuffers) / FRAME_BUFFER_SIZE;
}

#ifdef FRAMEBUFFER_ERASE

#define BLOCK_TRANSPARENT ((FRAME_PIXEL_TRANSPARENT << 6) | (FRAME_PIXEL_TRANSPARENT << 4) | (FRAME_PIXEL_TRANSPARENT << 2) | (FRAME_PIXEL_TRANSPARENT << 0))

void frameBuffer_eraseInit(void)
{
}

void frameBuffer_erase(uint8_t *frameBuffer)
{
    memset(frameBuffer, BLOCK_TRANSPARENT, FRAME_BUFFER_SIZE);
}
#endif

#if defined(FRAMEBUFFER_EXTENDED_API)

#define FRAME_PIXEL_MASK        ((1 << 1) | (1 << 0))
#define PIXELS_PER_BYTE (8 / BITS_PER_PIXEL)

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
void frameBuffer_writeCharacter(uint8_t *frameBuffer, uint16_t x, uint16_t y, uint8_t characterIndex)
{
    uint16_t fontCharacterOffset = characterIndex * FONT_MAX7456_12x18_BYTES_PER_CHARACTER;

    for (int row = 0; row < FONT_MAX7456_HEIGHT; row++) {
        uint16_t fy = y + row;
        uint16_t fx = x;


        // MAX7456, pixel bits:
        // 00 = black
        // 10 = white
        // X1 = transparent (external sync) or grey (internal sync).
        // X  = Don't care

        for (int b = 0; b < 3; b++) {
            uint8_t c = font_max7456_12x18[fontCharacterOffset];
            fontCharacterOffset++;

            for (int p = 0; p <= 3; p++) {
                uint8_t mp = (c >> (2 * (3 - p))) & ((1 << 1) | (1 << 0)); // extract max7456 pixel from character
                // FUTURE allow caller to choose rendering mode.
                uint8_t mode = 0xFF; // don't render transparent pixels in the font.
                // uint8_t mode = FRAME_PIXEL_TRANSPARENT; // render not-black and not-white pixels as transparent.

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

void frameBuffer_writeString(uint8_t *frameBuffer, uint16_t x, uint16_t y, const uint8_t *message, uint8_t messageLength)
{
    uint16_t fx = x;
    for (int mi = 0; mi < messageLength; mi++) {
        uint8_t c = message[mi];

        frameBuffer_writeCharacter(frameBuffer, fx, y, font_max7456_12x18_asciiToFontMapping[c]);
        fx+= 12; // font width
    }
}
#endif // FRAMEBUFFER_EXTENDED_API
