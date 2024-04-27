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

#pragma once

typedef struct {
  uint8_t *start;
  uint8_t *end;
} memorySection_t;

typedef struct {
  uint8_t *source;
  memorySection_t section;
} initializedMemory_t;


void clearMemorySection(const memorySection_t *memorySection);
void clearMemorySections(const memorySection_t *memorySections, int count);

void initialiseMemorySection(const initializedMemory_t *initializedMemory);
void initialiseMemorySections(const initializedMemory_t *initializedMemorySections, int count);
