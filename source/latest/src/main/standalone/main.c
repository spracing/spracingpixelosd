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

    // There are two frame buffers, one is used to display the video output, the other is used to draw into.
    // using two framebuffers and switching between them when drawing is complete gives the CPU more time for drawing
    // routines, at the expense of frame-rate.
    // Ideally everything should be drawn in the time it takes to output/display one frame.
    // Note that framebuffers are not cleared/erased automatically.

    frameBuffer_eraseInit();

    uint8_t *activefb = frameBuffer_getBuffer(0);
    uint8_t *preparingfb = frameBuffer_getBuffer(1);

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

        if (vsyncFlag) {
            vsyncFlag = false;

            // erasing a framebuffer is slow, depending on the MCU there are ways to offload this to hardware.
            frameBuffer_erase(preparingfb);

            frameBuffer_writeString(preparingfb, (360 - (22 * 12))/2, 8, (uint8_t*)"SPRacingEVO-ELRSOSDVTX", 22);
#if defined(PCB_REV) && defined(PCB_PANEL_INDEX)
            // example command line: make TARGET=SPRACINGEVO OPTIONS='PCB_REV=\"C\" PCB_PANEL_INDEX=\"1\"'
            frameBuffer_writeString(preparingfb, 20, 44, (uint8_t*)("Rev " PCB_REV), 5);
            frameBuffer_writeString(preparingfb, 280, 44, (uint8_t*)("No. " PCB_PANEL_INDEX), 6);
#endif

            //
            // frame counter
            //
            static uint16_t frameCounter, missedFrameCounter = 0;
            frameCounter++;

            static char message[5] = {0};
            snprintf(message, sizeof(message), "%04x", frameCounter);

            frameBuffer_writeString(preparingfb, (360/4 * 1) - (4 * 12)/2, 26, (uint8_t*)message, strlen(message));

            snprintf(message, sizeof(message), "%04x", missedFrameCounter);

            frameBuffer_writeString(preparingfb, (360/4 * 3) - (4 * 12)/2, 26, (uint8_t*)message, strlen(message));

            //
            // Chaser pixel, advanced each vsync
            //
            static uint16_t chaserPixelX[3] = {0,1,2};
            static uint8_t chaserPixelMode[3] = {FRAME_PIXEL_BLACK, FRAME_PIXEL_TRANSPARENT, FRAME_PIXEL_WHITE};
            for (uint8_t i = 0; i < 3; i++) {
                frameBuffer_setPixel(preparingfb, chaserPixelX[i], 44, chaserPixelMode[i]);
                chaserPixelX[i]+= 1;
                if (chaserPixelX[i] >= PIXEL_COUNT) {
                    chaserPixelX[i] = 0;
                }
            }

            if (vsyncFlag) {
                // if the ISR has set the vsyncFlag before we committed the frame buffer, then the drawing code took longer than one frame.
                missedFrameCounter++;
            }

            spracingPixelOSDLibraryVTable->frameBufferCommit(preparingfb);

            // swap the pointers, so we render into the other frame buffer.
            uint8_t* swapfb = preparingfb;
            preparingfb = activefb;
            activefb = swapfb;
        }

    } while (1);
}
