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
#include <string.h>

#include "platform.h"

#ifdef BETAFLIGHT
#include "drivers/io.h"

typedef struct spracingPixelOSDIO_s {
    IO_t blackPin;
    IO_t whitePin;
    IO_t syncInPin;
    IO_t debug1Pin;
    IO_t debug2Pin;
#ifdef DEBUG_BLANKING
    IO_t blankingDebugPin;
#endif
#ifdef DEBUG_GATING
    IO_t gatingDebugPin;
#endif
    IO_t whiteSourceSelectPin;
    IO_t maskEnablePin;
} spracingPixelOSDIO_t;

spracingPixelOSDIO_t spracingPixelOSDIO = {
    .blackPin               = IO_NONE,
    .whitePin               = IO_NONE,
    .syncInPin              = IO_NONE,
    .debug1Pin              = IO_NONE,
    .debug2Pin              = IO_NONE,
#ifdef DEBUG_BLANKING
    .blankingDebugPin       = IO_NONE,
#endif
#ifdef DEBUG_GATING
    .gatingDebugPin         = IO_NONE,
#endif
    .whiteSourceSelectPin   = IO_NONE,
    .maskEnablePin          = IO_NONE,
};

#define IO_PIXEL_BLACK_CFG                  IO_CONFIG(GPIO_MODE_OUTPUT_OD, GPIO_SPEED_FREQ_MEDIUM,  GPIO_NOPULL)
#define IO_PIXEL_WHITE_CFG                  IO_CONFIG(GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_MEDIUM,  GPIO_NOPULL)

#define IO_PIXEL_MASK_ENABLE_CFG            IO_CONFIG(GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_MEDIUM,  GPIO_PULLDOWN)
#define IO_PIXEL_WHITE_SOURCE_SELECT_CFG    IO_CONFIG(GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_MEDIUM,  GPIO_PULLDOWN)

#define IO_PIXEL_DEBUG_CFG                  IO_CONFIG(GPIO_MODE_OUTPUT_PP, GPIO_SPEED_FREQ_MEDIUM,  GPIO_PULLDOWN)

#define IO_VIDEO_SYNC_IN_CFG                IO_CONFIG(GPIO_MODE_INPUT,     GPIO_SPEED_FREQ_LOW,     GPIO_NOPULL)

//
// Debug
//

void pixelDebug1Set(bool state)
{
    IOWrite(spracingPixelOSDIO.debug1Pin, state);
}

void pixelDebug1Low(void)
{
    IOLo(spracingPixelOSDIO.debug1Pin);
}

void pixelDebug1High(void)
{
    IOHi(spracingPixelOSDIO.debug1Pin);
}

void pixelDebug1Toggle(void)
{
    IOToggle(spracingPixelOSDIO.debug1Pin);
}

void pixelDebug2Set(bool state)
{
    IOWrite(spracingPixelOSDIO.debug2Pin, state);
}

void pixelDebug2Low(void)
{
    IOLo(spracingPixelOSDIO.debug2Pin);
}

void pixelDebug2High(void)
{
    IOHi(spracingPixelOSDIO.debug2Pin);
}

void pixelDebug2Toggle(void)
{
    IOToggle(spracingPixelOSDIO.debug2Pin);
}

//
// Init
//

void spracingPixelOSD_initIO(void)
{
    spracingPixelOSDIO.blackPin = IOGetByTag(IO_TAG(SPRACING_PIXEL_OSD_BLACK_PIN));
    IOHi(spracingPixelOSDIO.blackPin);
    IOInit(spracingPixelOSDIO.blackPin, OWNER_OSD, 0);
    IOConfigGPIO(spracingPixelOSDIO.blackPin, IO_PIXEL_BLACK_CFG);

#ifdef SPRACING_PIXEL_OSD_MASK_ENABLE_PIN
    spracingPixelOSDIO.maskEnablePin = IOGetByTag(IO_TAG(SPRACING_PIXEL_OSD_MASK_ENABLE_PIN));
    IOLo(spracingPixelOSDIO.maskEnablePin); // Low = Mask disabled, High = Mask Enabled.
    IOInit(spracingPixelOSDIO.maskEnablePin, OWNER_OSD, 0);
    IOConfigGPIO(spracingPixelOSDIO.maskEnablePin, IO_PIXEL_MASK_ENABLE_CFG);
#endif

    spracingPixelOSDIO.whitePin = IOGetByTag(IO_TAG(SPRACING_PIXEL_OSD_WHITE_PIN));
    IOLo(spracingPixelOSDIO.whitePin);
    IOInit(spracingPixelOSDIO.whitePin, OWNER_OSD, 0);
    IOConfigGPIO(spracingPixelOSDIO.whitePin, IO_PIXEL_WHITE_CFG);

#ifdef SPRACING_PIXEL_OSD_WHITE_SOURCE_SELECT_PIN
    spracingPixelOSDIO.whiteSourceSelectPin = IOGetByTag(IO_TAG(SPRACING_PIXEL_OSD_WHITE_SOURCE_SELECT_PIN));
    IOLo(spracingPixelOSDIO.whiteSourceSelectPin); // Low = Fixed Voltage, High = Linked to DAC1_OUT1 voltage.
    IOInit(spracingPixelOSDIO.whiteSourceSelectPin, OWNER_OSD, 0);
    IOConfigGPIO(spracingPixelOSDIO.whiteSourceSelectPin, IO_PIXEL_WHITE_SOURCE_SELECT_CFG);
#endif

    spracingPixelOSDIO.syncInPin = IOGetByTag(IO_TAG(SPRACING_PIXEL_OSD_SYNC_IN_PIN));
    IOLo(spracingPixelOSDIO.syncInPin);
    IOInit(spracingPixelOSDIO.syncInPin, OWNER_OSD, 0);
    IOConfigGPIO(spracingPixelOSDIO.syncInPin, IO_VIDEO_SYNC_IN_CFG);

#ifdef DEBUG_BLANKING
    spracingPixelOSDIO.blankingDebugPin = IOGetByTag(IO_TAG(SPRACING_PIXEL_OSD_PIXEL_BLANKING_DEBUG_PIN));
    IOLo(spracingPixelOSDIO.blankingDebugPin);
    IOInit(spracingPixelOSDIO.blankingDebugPin, OWNER_OSD, 0);
    IOConfigGPIO(spracingPixelOSDIO.blankingDebugPin, IO_PIXEL_DEBUG_CFG);
#endif

#ifdef DEBUG_GATING
    spracingPixelOSDIO.gatingDebugPin = IOGetByTag(IO_TAG(SPRACING_PIXEL_OSD_PIXEL_GATING_DEBUG_PIN));
    IOLo(spracingPixelOSDIO.gatingDebugPin);
    IOInit(spracingPixelOSDIO.gatingDebugPin, OWNER_OSD, 0);
    IOConfigGPIO(spracingPixelOSDIO.gatingDebugPin, IO_PIXEL_DEBUG_CFG);
#endif

    spracingPixelOSDIO.debug1Pin = IOGetByTag(IO_TAG(SPRACING_PIXEL_OSD_PIXEL_DEBUG_1_PIN));
    IOLo(spracingPixelOSDIO.debug1Pin);
    IOInit(spracingPixelOSDIO.debug1Pin, OWNER_OSD, 0);
    IOConfigGPIO(spracingPixelOSDIO.debug1Pin, IO_PIXEL_DEBUG_CFG);

    spracingPixelOSDIO.debug2Pin = IOGetByTag(IO_TAG(SPRACING_PIXEL_OSD_PIXEL_DEBUG_2_PIN));
    IOLo(spracingPixelOSDIO.debug2Pin);
    IOInit(spracingPixelOSDIO.debug2Pin, OWNER_OSD, 0);
    IOConfigGPIO(spracingPixelOSDIO.debug2Pin, IO_PIXEL_DEBUG_CFG);
}

#endif // BETAFLIGHT
