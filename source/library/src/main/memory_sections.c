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

#include <stdint.h>
#include <stdlib.h>

#include "memory_sections.h"

void clearMemorySection(const memorySection_t *memorySection)
{
  size_t size = (size_t)(memorySection->end - memorySection->start);

  uint8_t* memory = memorySection->start;
  for (uint32_t i = 0; i < size; i++)
  {
    memory[i] = 0;
  }
}

void clearMemorySections(const memorySection_t *memorySections, int count)
{
  for (int index = 0; index < count; index++) {
    const memorySection_t *memorySection = &memorySections[index];
    clearMemorySection(memorySection);
  }
}

void initialiseMemorySection(const initializedMemory_t *initializedMemory)
{
  size_t size = (size_t)(initializedMemory->section.end - initializedMemory->section.start);

  uint8_t* dest = initializedMemory->section.start;
  uint8_t* src = initializedMemory->source;

  for (uint32_t i = 0; i < size; i++)
  {
    dest[i] = src[i];
  }
}

void initialiseMemorySections(const initializedMemory_t *initializedMemorySections, int count)
{
  for (int index = 0; index < count; index++) {
    const initializedMemory_t *initializedMemorySection = &initializedMemorySections[index];
    initialiseMemorySection(initializedMemorySection);
  }
}
