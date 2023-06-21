#include "stdint.h"
#include "stdbool.h"

#include "platform.h"
#include "api/spracingpixelosd_api.h"
#include "standalone/system.h"
#include "utils.h"
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

static void onVSync(void) // ISR callback
{
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

    init();

    bool led1State = false;
    uint32_t serviceDeadlineAtUs = 0;
    // TODO
    do {
        uint32_t currentTimeUs = micros();

        bool serviceNow = cmp32(currentTimeUs, serviceDeadlineAtUs) > 0;
        if (serviceNow) {
            led1State = !led1State;
            led1Set(led1State);

            serviceDeadlineAtUs += 1000000 / 100; // 100hz
            spracingPixelOSDLibraryVTable->service(currentTimeUs);
        }

    } while (1);
}
