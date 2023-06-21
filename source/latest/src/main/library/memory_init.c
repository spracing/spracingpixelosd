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
#include <stdint.h>

#include "common/memory_sections.h"

extern uint8_t __library_bss;
extern uint8_t __library_bss_end;

extern uint8_t __library_d1_ram_start;
extern uint8_t __library_d1_ram_end;

static const memorySection_t memorySectionsToClear[] = {
    {&__library_bss, &__library_bss_end},
    {&__library_d1_ram_start, &__library_d1_ram_end},
};

extern uint8_t __library_initalized_data;
extern uint8_t __library_data_start;
extern uint8_t __library_data_end;

extern uint8_t __library_d1_initalized_data;
extern uint8_t __library_d1_data_start;
extern uint8_t __library_d1_data_end;

extern uint8_t __library_d2_initalized_data;
extern uint8_t __library_d2_data_start;
extern uint8_t __library_d2_data_end;

static const initializedMemory_t memorySectionsToInitialize[] = {
    {.source = &__library_initalized_data, .section = {&__library_data_start, &__library_data_end}},
    {.source = &__library_d1_initalized_data, .section = {&__library_d1_data_start, &__library_d1_data_end}},
    {.source = &__library_d2_initalized_data, .section = {&__library_d2_data_start, &__library_d2_data_end}},
};

void memoryInit(void)
{
    clearMemorySections(memorySectionsToClear, sizeof(memorySectionsToClear) / sizeof(memorySectionsToClear[0]));
    initialiseMemorySections(memorySectionsToInitialize, sizeof(memorySectionsToInitialize) / sizeof(memorySectionsToInitialize[0]));
}
