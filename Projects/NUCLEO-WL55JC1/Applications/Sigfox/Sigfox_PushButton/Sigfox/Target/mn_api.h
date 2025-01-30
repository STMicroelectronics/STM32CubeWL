/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mn_api.h
  * @author  MCD Application Team
  * @brief   monarch library interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MN_API_H__
#define __MN_API_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief Initialize the Monarch Low level
  * @param Callback for rssi sample process
  * @retval None
  */
void MN_API_Init(void (*MN_API_Timer_CB)(int16_t rssi));

/**
  * @brief DeInitialize the Monarch Low level
  * @param None
  * @retval None
  */
void MN_API_DeInit(void);

/**
  * @brief enable the 16k timer for sampling
  * @param None
  * @retval None
  */
void MN_API_Enable16KHzSamplingTimer(void);

/**
  * @brief enable the 16k timer for sampling
  * @param None
  * @retval None
  */
void MN_API_Disable16KHzSamplingTimer(void);

/**
  * @brief change the RF frequency of the receiver
  * @param frequency
  * @retval None
  */
void MN_API_change_frequency(uint32_t frequency);

/**
  * @brief starts receiver
  * @param None
  * @retval None
  */
void MN_API_StartRx(void);

/**
  * @brief stops receiver
  * @param None
  * @retval None
  */
void MN_API_StopRx(void);

/**
  * @brief Notifies that pattern has been found
  * @note  implementation is optional
  * @param  window_type 0:sweep pattern, 1:window pattern
  * @param  pattern: 1:delta_f1, :delta_f2, 1:delta_f2
  * @param  frequency at which pattern has been detected
  * @param best_rssi rssi at which pattern has been detected
  * @retval None
  */
void MN_API_Pattern_Found(int32_t window_type, int32_t pattern, int32_t frequency, int32_t best_rssi);

/**
  * @brief starts timer
  * @param timer_value_ms timer value in ms
  * @param call back when timer elapses
  * @note Argument can be left unused
  * @retval None
  */void MN_API_TimerSart(uint32_t timer_value_ms, void (*TimeoutHandle)(void *Argument));

/**
  * @brief stops timer
  * @param None
  * @retval None
  */
void MN_API_TimerStop(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __MN_API_H__ */
