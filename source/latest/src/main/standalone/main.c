#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"

#include "platform.h"
#include "api/spracingpixelosd_api.h"
#include "api/spracingpixelosd_framebuffer_api.h"
#include "standalone/system.h"
#include "utils.h"
#include "common/framebuffer.h"
#include "common/glue.h"
#include "common/io.h"

void memoryInit(void)
{
}

// provided by linker script
extern const uint8_t __library_descriptor_start;
extern const uint8_t __library_vtable_start;

const spracingPixelOSDLibraryVTable_t * const spracingPixelOSDLibraryVTable = (spracingPixelOSDLibraryVTable_t *)&__library_vtable_start;
const spracingPixelOSDLibraryDescriptor_t * const spracingPixelOSDLibraryDescriptor = (spracingPixelOSDLibraryDescriptor_t *)&__library_descriptor_start;

bool spracingPixelOSDIsLibraryAvailable(void)
{
    return ((spracingPixelOSDLibraryDescriptor->code == SPRACINGPIXELOSD_LIBRARY_CODE) && (spracingPixelOSDLibraryDescriptor->apiVersion == SPRACINGPIXELOSD_LIBRARY_API_VERSION));
}

static volatile bool vsyncFlag = false;

static void onVSync(void) // ISR callback
{
    vsyncFlag = true;
}

uint32_t micros()
{
    return HAL_GetTick() * 1000;
}

spracingPixelOSDState_t *pixelOSDState;

bool init(void) {
    static const spracingPixelOSDHostAPI_t pixelOSDHostAPI = {
        .micros = micros,
        .onVSync = onVSync,
    };

    if (!spracingPixelOSDIsLibraryAvailable()) {
        return false;
    }

    spracingPixelOSDDefaultConfig_t defaultConfig = {
        .flags = PIXELOSD_CF_VIDEO_SYSTEM_PAL,
    };

    spracingPixelOSDLibraryVTable->init(&pixelOSDHostAPI, &defaultConfig);
    pixelOSDState = spracingPixelOSDLibraryVTable->getState();
    if ((pixelOSDState->flags & PIXELOSD_FLAG_INITIALISED) == 0) {
        return false;
    }

    return true;
}

int main(void) {

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    if (!init()) {
        Error_Handler();
    }

    uint8_t *fb0 = frameBuffer_getBuffer(0);
    {
        frameBuffer_writeString(fb0, 50, 8, (uint8_t*)"SPRacingEVO-ELRSOSDVTX", 22);
#if defined(PCB_REV) && defined(PCB_PANEL_INDEX)
        // example command line: make TARGET=SPRACINGEVO OPTIONS='PCB_REV=\"C\" PCB_PANEL_INDEX=\"1\"'
        frameBuffer_writeString(fb0, 20, 24, (uint8_t*)("Rev " PCB_REV), 5);
        frameBuffer_writeString(fb0, 280, 24, (uint8_t*)("No. " PCB_PANEL_INDEX), 6);
#endif
    }

    bool led1State = false;
    uint32_t serviceDeadlineAtUs = 0;

    do {
        uint32_t currentTimeUs = micros();

        bool serviceNow = cmp32(currentTimeUs, serviceDeadlineAtUs) > 0;
        if (serviceNow) {
            led1State = !led1State;
            led1Set(led1State);

            serviceDeadlineAtUs += 1000000 / 100; // 100hz
            spracingPixelOSDLibraryVTable->service(currentTimeUs);
        }

        //
        // If the vsyncflag was set, quickly update the framebuffer.
        //
        // If this code takes too long then flashing/flickering/corruption will occur.
        // This example currently always uses framebuffer 0, it's possible to use two framebuffers and switch them
        // when drawing is complete, which gives the CPU more time for drawing routines, at the expense of frame-rate.
        // ideally everything should be drawn in the time it takes to output/display one frame.

        if (vsyncFlag) {
            vsyncFlag = false;

            //
            // frame counter
            //
            static uint16_t frameCounter = 0;
            frameCounter++;

            static char message[11] = {0};
            snprintf(message, 11, "%04x", frameCounter);

            uint8_t *fb0 = frameBuffer_getBuffer(0);
            frameBuffer_writeString(fb0, 160, 24, (uint8_t*)message, strlen(message));

            //
            // Chaser pixel, advanced each vsync
            //
            static uint16_t chaserPixelX[3] = {0,1,2};
            static uint8_t chaserPixelMode[3] = {FRAME_PIXEL_BLACK, FRAME_PIXEL_TRANSPARENT, FRAME_PIXEL_WHITE};
            for (uint8_t i = 0; i < 3; i++) {
                frameBuffer_setPixel(fb0, chaserPixelX[i], 42, chaserPixelMode[i]);
                chaserPixelX[i]+= 1;
                if (chaserPixelX[i] >= PIXEL_COUNT) {
                    chaserPixelX[i] = 0;
                }
            }
        }

    } while (1);
}
