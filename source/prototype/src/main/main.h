/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

#define CPU_CLOCK 80000000

#define TIMER_CLOCKS_PER_US                      (CPU_CLOCK / 1000000)
#define _US_TO_CLOCKS(__us)                      ((uint32_t)((__us) * TIMER_CLOCKS_PER_US))

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VIDEO_IN_Pin GPIO_PIN_1
#define VIDEO_IN_GPIO_Port GPIOA
#define VIDEO_THRESHOLD_OUT_Pin GPIO_PIN_4
#define VIDEO_THRESHOLD_OUT_GPIO_Port GPIOA
#define SYNC_OUT_Pin GPIO_PIN_7
#define SYNC_OUT_GPIO_Port GPIOA
#define BLACK_SINK_Pin GPIO_PIN_0
#define BLACK_SINK_GPIO_Port GPIOB
#define WHITE_SOURCE_Pin GPIO_PIN_1
#define WHITE_SOURCE_GPIO_Port GPIOB
#define DEBUG_OUT_Pin GPIO_PIN_8
#define DEBUG_OUT_GPIO_Port GPIOA
#define PIXEL_START_DEBUG_Pin GPIO_PIN_11
#define PIXEL_START_DEBUG_GPIO_Port GPIOA
#define INT_OUT_Pin GPIO_PIN_3
#define INT_OUT_GPIO_Port GPIOH
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
