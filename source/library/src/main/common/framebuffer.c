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
