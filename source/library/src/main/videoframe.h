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

typedef enum frameStatus_e {
    WAITING_FOR_FIRST_FIELD = 0,
    COUNTING_PRE_EQUALIZING_PULSES,
    COUNTING_SYNCRONIZING_PULSES,
    COUNTING_POST_EQUALIZING_PULSES,
    COUNTING_HSYNC_PULSES,
} frameStatus_t;

typedef struct frameState_s {
    uint32_t frameStartCounter;
    uint32_t validFrameCounter;
    uint32_t lineCounter;
    uint16_t pulseErrors;
    uint16_t totalPulseErrors;
    frameStatus_t status;
} frameState_t;

void setComparatorTargetMv(uint32_t newTargetMv);
void disableComparatorBlanking(void);
void recalculateBlankingTimings(const videoTimings_t *vt);

void videoFrame_reset(void);

extern frameState_t frameState;
extern uint8_t *outputFrameBuffer;
