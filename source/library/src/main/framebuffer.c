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

#include <spracingpixelosd_conf.h>
#include <spracingpixelosd_framebuffer_api.h>

#include "framebuffer.h"

// frame buffer memory configured via linker scripts
extern uint8_t __frame_buffer_start;
extern uint8_t __frame_buffer_end;

// framebuffers must be located in RAM_D2 region as they need to be accessible by DMA2 and MDMA.
// they must be 32 bit aligned.

uint8_t *frameBuffers = &__frame_buffer_start;

FRAME_BUFFER_DMA_RAM uint8_t frameBufferReserved[2][FRAME_BUFFER_SIZE] __attribute__((used));

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
