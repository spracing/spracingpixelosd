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

//
// Frame Buffer
//

#include "configuration.h"

#ifdef FRAMEBUFFER_ERASE
void frameBuffer_eraseInit(void);
void frameBuffer_erase(uint8_t *frameBuffer);
#endif


uint8_t *frameBuffer_getBuffer(uint8_t index);
uint8_t frameBuffer_getBufferIndex(uint8_t *frameBuffer);

// Use FRAME_PIXEL_* for 'mode` arguments below.

#if defined(FRAMEBUFFER_EXTENDED_API)
void frameBuffer_setPixel(uint8_t *frameBuffer, uint16_t x, uint16_t y, uint8_t mode);


void frameBuffer_writeString(uint8_t *frameBuffer, uint16_t x, uint16_t y, const uint8_t *message, uint8_t messageLength);
void frameBuffer_writeCharacter(uint8_t *frameBuffer, uint16_t x, uint16_t y, uint8_t characterIndex);
//void framebuffer_drawVerticalLine(uint8_t *frameBuffer, uint16_t x, uint16_t y0, uint16_t y1, uint8_t mode);
//void framebuffer_drawLine(uint8_t *frameBuffer, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t mode);
//void framebuffer_drawRectangle(uint8_t *frameBuffer, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t mode);

void frameBuffer_createTestPattern2(uint8_t *frameBuffer);
#endif
