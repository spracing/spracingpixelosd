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

#include "configuration.h"

void spracingPixelOSD_initIO(void);

//
// OSD Pin Debugging
//
void pixelDebug1Set(bool state);
void pixelDebug2Set(bool state);
void pixelDebug1Low(void);
void pixelDebug2Low(void);
void pixelDebug1High(void);
void pixelDebug2High(void);
void pixelDebug1Toggle(void);
void pixelDebug2Toggle(void);

//
// Status LEDs
//
void led1Set(bool state);
