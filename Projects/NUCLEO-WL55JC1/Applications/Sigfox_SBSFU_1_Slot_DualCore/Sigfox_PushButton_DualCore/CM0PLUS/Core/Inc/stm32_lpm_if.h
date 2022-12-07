/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32_lpm_if.h
  * @author  MCD Application Team
  * @brief   Header for Low Power Manager interface configuration
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
#ifndef __STM32_LPM_IF_H__
#define __STM32_LPM_IF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_lpm.h"

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
  * @brief Enters Low Power Off Mode
  */
void PWR_EnterOffMode(void);

/**
  * @brief Exits Low Power Off Mode
  */
void PWR_ExitOffMode(void);

/**
  * @brief Enters Low Power Stop Mode
  * @note ARM exists the function when waking up
  */
void PWR_EnterStopMode(void);

/**
  * @brief Exits Low Power Stop Mode
  * @note Enable the pll at 32MHz
  */
void PWR_ExitStopMode(void);

/**
  * @brief Enters Low Power Sleep Mode
  * @note ARM exits the function when waking up
  */
void PWR_EnterSleepMode(void);

/**
  * @brief Exits Low Power Sleep Mode
  * @note ARM exits the function when waking up
  */
void PWR_ExitSleepMode(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__STM32_LPM_IF_H__ */
