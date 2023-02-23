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

#include "main.h"
#include "stdbool.h"
#include "video.h"
#include "video_interrupt.h"

volatile bool cameraConnected = true;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim15;
extern COMP_HandleTypeDef hcomp1;

// FRAME = [sync pulses] [pre equalizing pulses] + [odd field] + [post equalizing pulses] [sync pulses] [pre equalizing pulses] + [even field] + [post equalizing pulses]
//         |                                       |                                                                              |
//         frameFallingEdgeIndex = 0               odd field = true                                                               odd field = false
//                                                                                                                                           | frameFallingEdgeIndex = -X (reset in DMA)

volatile int16_t frameFallingEdgeIndex = 0;
//volatile int16_t fieldRisingEdgeIndex = 0;
volatile uint8_t fallingEdgesRemainingBeforeNewField = 0;

uint16_t firstVisibleLine;
uint16_t lastVisibleLine;
bool nextLineIsVisible;
volatile uint16_t visibleLineIndex;

#if 1
#define DEBUG_LAST_HALF_LINE
#define DEBUG_PIXEL_BUFFER
#define DEBUG_PULSE_ERRORS
#else
#define DEBUG_COMP_TRIGGER
#define DEBUG_SYNC_PWM
#define DEBUG_FIELD_START
#define DEBUG_SHORT_PULSE
#define DEBUG_PIXEL_DMA
#define DEBUG_FIRST_SYNC_PULSE
#endif

#define DEBUG_PULSE_STATISTICS
#ifdef DEBUG_PULSE_STATISTICS
uint16_t syncPulseRisingStatisticIndex = 0;
uint16_t syncPulseRisingStatistics[PAL_LINES] __attribute__((used));
uint16_t syncPulseFallingStatisticIndex = 0;
uint16_t syncPulseFallingStatistics[PAL_LINES] __attribute__((used));
#endif

volatile bool fillLineNow = false;
volatile bool frameStartFlag = false;
volatile uint16_t fillLineIndex = 0;


//
// Pixel
//

DMA_HandleTypeDef *hPixelOutDMA;
bool pixelDMAActive = false;

void pixelOutputDisable(void)
{
    bool blackOn = cameraConnected ? false : true;
    //bool blackOn = true;

    HAL_GPIO_WritePin(BLACK_SINK_GPIO_Port, BLACK_SINK_Pin, blackOn ? GPIO_PIN_RESET : GPIO_PIN_SET);

    // turn off black/white
    HAL_GPIO_WritePin(WHITE_SOURCE_GPIO_Port, WHITE_SOURCE_Pin, GPIO_PIN_RESET);
}

uint8_t *fillPixelBuffer = NULL;
uint8_t *outputPixelBuffer = NULL;

void pixelConfigureDMAForNextField(void)
{
#ifdef DEBUG_PIXEL_BUFFER
    HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, fillPixelBuffer == pixelBufferA ? GPIO_PIN_SET : GPIO_PIN_RESET);
#endif

    if (HAL_DMA_Start_IT(hPixelOutDMA, (uint32_t)fillPixelBuffer, PIXEL_ADDRESS, PIXEL_BUFFER_SIZE) != HAL_OK)
    {
      Error_Handler();
    }

}

void pixelXferCpltCallback(struct __DMA_HandleTypeDef * hdma)
{

#ifdef DEBUG_PIXEL_DMA
    HAL_GPIO_TogglePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin);
#endif

#ifdef STOP_START_PIXEL_TIMER
    (&htim15)->Instance->CR1 &= ~(TIM_CR1_CEN); // stop
    (&htim15)->Instance->SMCR &= ~TIM_SMCR_SMS; // disable slave mode
#endif

    pixelOutputDisable();

    __HAL_TIM_DISABLE_DMA(&htim15, TIM_DMA_CC1);

    pixelConfigureDMAForNextField();

    pixelDMAActive = false;
}

static inline void pixelStartDMA(void);

void pixelInit(void)
{
    hPixelOutDMA = htim15.hdma[TIM_DMA_ID_CC1];
    hPixelOutDMA->XferCpltCallback = pixelXferCpltCallback;

    HAL_TIM_Base_Start(&htim15);
    if (HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1) != HAL_OK)
    {
      // PWM Generation Error
      Error_Handler();
    }

    // Pre-configure DMA, DMA is started in COMP IRQ Handler
    outputPixelBuffer = pixelBufferA;
    fillPixelBuffer = pixelBufferB;
    if (HAL_DMA_Start_IT(hPixelOutDMA, (uint32_t)outputPixelBuffer, PIXEL_ADDRESS, PIXEL_BUFFER_SIZE) != HAL_OK)
    {
      Error_Handler();
    }

    // OC4REF used to gate TIM15
    if (HAL_TIM_OC_Start(&htim1, TIM_CHANNEL_4) != HAL_OK)
    {
      Error_Handler();
    }
}

static inline void pixelStartDMA(void)
{
#ifdef DEBUG_PIXEL_DMA
    HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_SET);
#endif

    (&htim15)->Instance->CNT = 0;
#ifdef STOP_START_PIXEL_TIMER
    (&htim15)->Instance->SMCR |= TIM_SLAVEMODE_GATED;
    (&htim15)->Instance->CR1 |= (TIM_CR1_CEN);
#endif

    __HAL_TIM_ENABLE_DMA(&htim15, TIM_DMA_CC1);

    pixelDMAActive = true;
}

typedef struct {
#ifdef DEBUG_COMP_TRIGGER
    uint32_t triggerLowCount;
    uint32_t triggerHighCount;
#endif
    bool fallingEdge;
} compState_t;

compState_t compState = { 0 };

static volatile uint16_t pulseLength __attribute__((used)) = 0;
static volatile uint16_t previousPulseLength __attribute__((used)) = 0;


fieldState_t fieldState = { 0 };
frameState_t frameState = { 0 };

static inline void pulseError(void) {
#ifdef DEBUG_PULSE_ERRORS
    HAL_GPIO_TogglePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin);
#endif
    frameState.pulseErrors++;
#ifdef DEBUG_PULSE_ERRORS
    HAL_GPIO_TogglePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin);
#endif
}

#ifdef USE_HAL_COMP_CALLBACK
void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp)
{
#else
void RAW_COMP_TriggerCallback(void)
{
    COMP_HandleTypeDef *hcomp = &hcomp1;
    uint32_t exti_line = COMP_GET_EXTI_LINE(hcomp->Instance);
    LL_EXTI_ClearFlag_0_31(exti_line);

#endif
    static uint16_t previousCompare = 0;
    uint16_t currentCompare = (&htim2)->Instance->CCR4;
    pulseLength = currentCompare - previousCompare;

    compState.fallingEdge = (HAL_COMP_GetOutputLevel(hcomp) == COMP_OUTPUTLEVEL_LOW);
    if (compState.fallingEdge) {
#ifdef DEBUG_COMP_TRIGGER
        compState.triggerLowCount++;
        HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_RESET);
#endif

        // VIDEO_SYNC_VSYNC_MIN ((uint32_t)((((64.000 / 2.0) - 4.700) - (((64.000 / 2.0) - 4.700) - (4.700))/2.0) * (80000000 / 1000000)))
        // VIDEO_SYNC_VSYNC_MAX ((uint32_t)((((64.000 / 2.0) - 4.700) + (((64.000 / 2.0) - 2.000) - ((64.000 / 2.0) - 4.700))/2.0) * (80000000 / 1000000)))
        if (pulseLength > VIDEO_SYNC_VSYNC_MIN && pulseLength < VIDEO_SYNC_VSYNC_MAX) {
            if (previousPulseLength > VIDEO_SYNC_HSYNC_MIN) {

                // depending on the video mode a half frame pulse occurs at different times
                // use the detected mode to figure out if this is the first or second field.

                switch (detectedVideoMode) {
                case MODE_PAL:
                    fieldState.type = FIELD_SECOND; // pulse occurs at end of second field
                    break;
                case MODE_NTSC:
                    fieldState.type = FIELD_FIRST;  // pulse occurs at end of first field
                    break;
                default:
                    frameState.status = WAITING_FOR_FIRST_FIELD;
                    break;
                }

#ifdef DEBUG_LAST_HALF_LINE
                HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_RESET);
#endif
#ifdef DEBUG_LAST_HALF_LINE
                HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_SET);
#endif
            }
        }

        frameFallingEdgeIndex++;
        
#ifdef DEBUG_PULSE_STATISTICS
        syncPulseFallingStatistics[syncPulseFallingStatisticIndex] = pulseLength;
        syncPulseFallingStatisticIndex++;
        if (syncPulseFallingStatisticIndex >= sizeof(syncPulseFallingStatistics) / sizeof(syncPulseFallingStatistics[0])) {
            syncPulseFallingStatisticIndex = 0;
        }
#endif
        

    } else {
#ifdef DEBUG_COMP_TRIGGER
        compState.triggerHighCount++;
        HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_SET);
#endif

        //
        // check pulse lengths in order from shortest to longest.
        //
        if (pulseLength < VIDEO_SYNC_SHORT_MAX) {
#ifdef DEBUG_SHORT_PULSE
            HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_RESET);
#endif

            if (frameState.status == WAITING_FOR_FIRST_FIELD) {
                frameState.status = COUNTING_PRE_EQUALIZING_PULSES;

                fieldState.phase = FIELD_PRE_EQUALIZING;
                fieldState.preEqualizingPulses = 1; // this one

            } else if (frameState.status == COUNTING_PRE_EQUALIZING_PULSES) {
                fieldState.preEqualizingPulses++;
            } else if (frameState.status == COUNTING_POST_EQUALIZING_PULSES) {
                fieldState.postEqualizingPulses++;
            } else if (frameState.status == COUNTING_SYNCRONIZING_PULSES) {
                frameState.status = COUNTING_POST_EQUALIZING_PULSES;

                fieldState.phase = FIELD_POST_EQUALIZING;
                fieldState.postEqualizingPulses = 1; // this one

                if (fieldState.syncronizingPulses == 5) { // PAL
                    detectedVideoMode = MODE_PAL;
                    firstVisibleLine = 15;
                    lastVisibleLine = (PAL_VISIBLE_LINES - 1);
                } else if (fieldState.syncronizingPulses == 6) { // NTSC
                    detectedVideoMode = MODE_NTSC;
                    firstVisibleLine = 15;
                    lastVisibleLine = (NTSC_VISIBLE_LINES - 1);
                }

                if (fieldState.type == FIELD_ODD) {
                    firstVisibleLine--;
                }
                lastVisibleLine += firstVisibleLine;

            } else if (frameState.status == COUNTING_HSYNC_PULSES) {
                frameState.status = COUNTING_PRE_EQUALIZING_PULSES;

                fieldState.phase = FIELD_PRE_EQUALIZING;
                fieldState.preEqualizingPulses = 1; // this one
            } else {
                pulseError();
            }

#ifdef DEBUG_SHORT_PULSE
            HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_SET);
#endif

        } else if (pulseLength < VIDEO_SYNC_HSYNC_MAX) {

            //
            // Important start DMA *NOW* - then deal with remaining state.
            //
            bool thisLineIsVisible = nextLineIsVisible;
            if (thisLineIsVisible) {
                // DMA configured for next line (below) or by transfer-complete handler.
                pixelStartDMA();

                visibleLineIndex++;
            }

            switch(frameState.status) {
            case COUNTING_POST_EQUALIZING_PULSES:
                frameState.status = COUNTING_HSYNC_PULSES;

                fieldState.phase = FIELD_HSYNC;
            // FALLTHROUGH
            case COUNTING_HSYNC_PULSES:

                fieldState.lineNumber++;
                frameState.lineNumber++;

                // prepare for next line
                nextLineIsVisible = fieldState.lineNumber >= firstVisibleLine && fieldState.lineNumber <= lastVisibleLine;

                if (nextLineIsVisible) {
                    uint8_t *previousOutputPixelBuffer = outputPixelBuffer;

                    outputPixelBuffer = fillPixelBuffer;

                    fillLineNow = true;
                    fillLineIndex = visibleLineIndex;

                    fillPixelBuffer = previousOutputPixelBuffer;
                    if (!thisLineIsVisible) {
                        // if this line is visible the transfer-complete handler would configure the DMA
                        pixelConfigureDMAForNextField();
                    }
                }

                if (fieldState.lineNumber > fieldState.highestFieldLineNumber) {
                    fieldState.highestFieldLineNumber = fieldState.lineNumber;
                }
            break;
            default:
                pulseError();
            }

        } else if (pulseLength >= VIDEO_SYNC_LO_BROAD_MIN && pulseLength <= VIDEO_SYNC_LO_BROAD_MAX) {

            if (frameState.status == COUNTING_PRE_EQUALIZING_PULSES) {
                frameState.status = COUNTING_SYNCRONIZING_PULSES;

                fieldState.syncronizingPulses = 0; // incremented below for this one

                fieldState.phase = FIELD_SYNCRONIZING;

                if (fieldState.type == FIELD_SECOND) {
                    fieldState.type = FIELD_FIRST;

                    frameState.frameStartCounter++;
                    frameStartFlag = true;
                } else {
                    fieldState.type = FIELD_SECOND;
                }

                fieldState.lineNumber = 0;
                visibleLineIndex = 0;
                nextLineIsVisible = false;

                fieldState.phase = FIELD_SYNCRONIZING;
#ifdef DEBUG_FIELD_START
                HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_RESET);
#endif
            }

            if (frameState.status == COUNTING_POST_EQUALIZING_PULSES) {
                fieldState.type = FIELD_SECOND;
#ifdef DEBUG_FIELD_START
                HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_SET);
#endif
            }

            if (frameState.status == COUNTING_SYNCRONIZING_PULSES) {
                fieldState.syncronizingPulses++;
            }

#ifdef DEBUG_FIRST_SYNC_PULSE
            bool firstSyncPulse = (fieldState.syncronizingPulses == 0);
            if (firstSyncPulse) {
                HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_SET);
            } else if (fieldState.syncronizingPulses == 1) {
                HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_RESET);
            }
#endif
        } else {
            pulseError();
        }


#ifdef DEBUG_PULSE_STATISTICS
        syncPulseRisingStatistics[syncPulseRisingStatisticIndex] = pulseLength;
        syncPulseRisingStatisticIndex++;
        if (syncPulseRisingStatisticIndex >= sizeof(syncPulseRisingStatistics) / sizeof(syncPulseRisingStatistics[0])) {
            syncPulseRisingStatisticIndex = 0;
        }
#endif
    }
    
    previousPulseLength = pulseLength;
    previousCompare = currentCompare;
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    UNUSED(htim);
#ifdef DEBUG_SYNC_PWM
    static bool flag = false;

    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
        //
        // SYNC
        //
        flag = !flag;
        if (flag) {
            HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_SET);
        }
    }
#endif
}


//
// SYNC
//

typedef struct syncBufferItem_s {
    // the order and size of these structure is fixed.  the data is transferred by DMA to the timer peripheral, starting with the ARR register
    uint16_t arrValue;
    uint16_t repetitions; // timers only support 8 bit ARR.
    uint16_t ccValue;
} syncBufferItem_t;

#define HALF_LINE(period, repetitions, pulse) _US_TO_CLOCKS(period / 2) - 1, (repetitions) - 1, _US_TO_CLOCKS(pulse) - 1
#define FULL_LINE(period, repetitions, pulse) _US_TO_CLOCKS(period) - 1, (repetitions) - 1, _US_TO_CLOCKS(pulse) - 1

const syncBufferItem_t palSyncItems[] = {
        { HALF_LINE(VIDEO_LINE_LEN,  5,   VIDEO_SYNC_LO_BROAD)},  // start of first field
        { HALF_LINE(VIDEO_LINE_LEN,  5,   VIDEO_SYNC_SHORT)},
        { FULL_LINE(VIDEO_LINE_LEN,  153, VIDEO_SYNC_HSYNC)},     // start of picture data
        { FULL_LINE(VIDEO_LINE_LEN,  152, VIDEO_SYNC_HSYNC)},
        { HALF_LINE(VIDEO_LINE_LEN,  5,   VIDEO_SYNC_SHORT)},
        { HALF_LINE(VIDEO_LINE_LEN,  5,   VIDEO_SYNC_LO_BROAD)},  // start of second field
        { HALF_LINE(VIDEO_LINE_LEN,  4,   VIDEO_SYNC_SHORT)},
        { FULL_LINE(VIDEO_LINE_LEN,  1,   VIDEO_SYNC_SHORT)},     // second half of a line
        { FULL_LINE(VIDEO_LINE_LEN,  152, VIDEO_SYNC_HSYNC)},     // start of picture data
        { FULL_LINE(VIDEO_LINE_LEN,  152, VIDEO_SYNC_HSYNC)},
        { HALF_LINE(VIDEO_LINE_LEN,  1,   VIDEO_SYNC_HSYNC)},     // first half of a line/frame
        { HALF_LINE(VIDEO_LINE_LEN,  5,   VIDEO_SYNC_SHORT)},
        // 625 lines (2.5+2.5+153+152+2.5+2.5+2+1+152+152+.5+2.5)
};


bool syncDMAActive = false;
DMA_HandleTypeDef *hSyncOutDMA;

// called via by HAL DMA handler, callback configured in HAL_TIM_DMABurst_WriteStart implementation
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
#if 0
    // this is called after the timer is reconfigured with the last syncBufferItem
    // the timer is still active, and generating pulses and only starts again with the first syncBufferItem
    // once the timer has decremented the repetition counter to zero.

    HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_RESET);
    // +5 for pal, +6 for ntsc due to when the callback occurs (i.e. after the last DMA timer update, but before the timer repetition counter has reached 0.
    // -1 because the this occurs before the last falling edge of the last row of the last field is detected.
    // -1 more because we're counting falling edges.
    fallingEdgesRemainingBeforeNewField = 2 + 5;
    HAL_GPIO_WritePin(DEBUG_OUT_GPIO_Port, DEBUG_OUT_Pin, GPIO_PIN_SET);
#endif
}

void syncInit()
{
    hSyncOutDMA = htim1.hdma[TIM_DMA_ID_UPDATE];

    if (!cameraConnected) {
        syncStartPWM();
        syncStartDMA();
    }
}

void syncStartPWM(void)
{
    if (HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_1) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_TIMEx_PWMN_Start_IT(&htim1, TIM_CHANNEL_1) != HAL_OK)
    {
      Error_Handler();
    }
}

void syncStopPWM(void)
{
    if (HAL_TIM_PWM_Stop_IT(&htim1, TIM_CHANNEL_1) != HAL_OK)
    {
      Error_Handler();
    }
    if (HAL_TIMEx_PWMN_Stop_IT(&htim1, TIM_CHANNEL_1) != HAL_OK)
    {
      Error_Handler();
    }
}

void syncStartDMA(void)
{
    HAL_TIM_DMABurst_WriteStart(
        &htim1,
        TIM_DMABASE_ARR,
        TIM_DMA_UPDATE,
        (uint32_t *)palSyncItems,
        sizeof(palSyncItems) / 2, // 2 because each item is uint16_t, not uint32_t?
        TIM_DMABURSTLENGTH_3TRANSFERS
    );

    syncDMAActive = true;
}

void syncStopDMA(void)
{
    HAL_TIM_DMABurst_WriteStop(&htim1, TIM_DMA_UPDATE);
}
