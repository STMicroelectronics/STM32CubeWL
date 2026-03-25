/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sys_debug.h
  * @author  MCD Application Team
  * @brief   Configuration of the debug.c instances
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
#ifndef __SYS_DEBUG_H__
#define __SYS_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sys_conf.h"
#include "platform.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* Pin defines */

/**  Definition for Probe Line 1   **/
/**
  * @brief Pin of Probe Line 1
  */
#define PROBE_LINE1_PIN                           GPIO_PIN_12

/**
  * @brief Port of Probe Line 1
  */
#define PROBE_LINE1_PORT                          GPIOB

/**
  * @brief Enable GPIOs clock of Probe Line 1
  */
#define PROBE_LINE1_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()

/**
  * @brief Disable GPIOs clock of Probe Line 1
  */
#define PROBE_LINE1_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

/**  Definition for Probe Line 2   **/
/**
  * @brief Pin of Probe Line 2
  */
#define PROBE_LINE2_PIN                           GPIO_PIN_13

/**
  * @brief Port of Probe Line 2
  */
#define PROBE_LINE2_PORT                          GPIOB

/**
  * @brief Enable GPIOs clock of Probe Line 2
  */
#define PROBE_LINE2_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()

/**
  * @brief Disable GPIOs clock of Probe Line 2
  */
#define PROBE_LINE2_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
#if !defined (DISABLE_PROBE_GPIO)

/**
  * @brief Set pin to x value
  */
#define PROBE_GPIO_WRITE( gpio, n, x )     HAL_GPIO_WritePin( gpio, n, (GPIO_PinState)(x) )

/**
  * @brief Set pin to high level
  */
#define PROBE_GPIO_SET_LINE( gpio, n )     LL_GPIO_SetOutputPin( gpio, n )

/**
  * @brief Set pin to low level
  */
#define PROBE_GPIO_RST_LINE( gpio, n )     LL_GPIO_ResetOutputPin( gpio, n )

#else  /* DISABLE_PROBE_GPIO */

/**
  * @brief not usable
  */
#define PROBE_GPIO_WRITE( gpio, n, x )

/**
  * @brief not usable
  */
#define PROBE_GPIO_SET_LINE( gpio, n )

/**
  * @brief not usable
  */
#define PROBE_GPIO_RST_LINE( gpio, n )

#endif /* DISABLE_PROBE_GPIO */

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief Initializes the SW probes pins and the monitor RF pins via Alternate Function
  */
void DBG_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __SYS_DEBUG_H__ */
