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

#pragma once

static inline int16_t cmp16(uint16_t a, uint16_t b) { return (int16_t)(a - b); }
static inline int32_t cmp32(uint32_t a, uint32_t b) { return (int32_t)(a - b); }

#ifndef FALLTHROUGH
#if __GNUC__ > 6
#define FALLTHROUGH __attribute__ ((fallthrough))
#else
#define FALLTHROUGH do {} while(0)
#endif
#endif

#ifndef NOOP
#define NOOP do {} while (0)
#endif

#ifndef ARRAYLEN
#define ARRAYLEN(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef ARRAYEND
#define ARRAYEND(x) (&(x)[ARRAYLEN(x)])
#endif
