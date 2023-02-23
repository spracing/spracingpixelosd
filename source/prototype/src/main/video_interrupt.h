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

extern uint8_t *fillPixelBuffer;
extern uint8_t *outputPixelBuffer;

typedef enum fieldType_e {
    FIELD_EVEN = 0,
    FIELD_ODD = 1,

    FIELD_FIRST = 0,
    FIELD_SECOND = 1
} fieldType_t;

typedef enum fieldPhase_e {
    FIELD_UNKNOWN = 0,
    FIELD_PRE_EQUALIZING,
    FIELD_SYNCRONIZING,
    FIELD_POST_EQUALIZING,
    FIELD_HSYNC,
} fieldPhase_t;

typedef enum frameStatus_e {
    WAITING_FOR_FIRST_FIELD = 0,
    COUNTING_PRE_EQUALIZING_PULSES,
    COUNTING_SYNCRONIZING_PULSES,
    COUNTING_POST_EQUALIZING_PULSES,
    COUNTING_HSYNC_PULSES,
} frameStatus_t;

typedef struct fieldState_s {
    uint8_t preEqualizingPulses;
    uint8_t syncronizingPulses;
    uint8_t postEqualizingPulses;
    fieldType_t type;
    fieldPhase_t phase;
    uint16_t lineNumber;

    uint16_t highestFieldLineNumber;
} fieldState_t;

typedef struct frameState_s {
    uint32_t frameStartCounter;
    uint16_t lineNumber;
    uint16_t pulseErrors;
    frameStatus_t status;
} frameState_t;

extern fieldState_t fieldState;
extern frameState_t frameState;

void pixelInit(void);
//void pixelStartDMA(void);

void syncInit(void);
void syncStartDMA(void);
void syncStopDMA(void);
void syncStartPWM(void);
void syncStopPWM(void);


volatile bool fillLineNow;
volatile bool frameStartFlag;
volatile uint16_t fillLineIndex;
