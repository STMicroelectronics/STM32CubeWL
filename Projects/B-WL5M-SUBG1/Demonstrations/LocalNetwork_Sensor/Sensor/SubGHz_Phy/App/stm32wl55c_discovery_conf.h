/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32wl55c_discovery_conf.h
  * @author  MCD Application Team
  * @brief   configuration file.
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
#ifndef STM32WL55C_DISCOVERY_CONF_H
#define STM32WL55C_DISCOVERY_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"

/* Environmental Sensors usage */
#define USE_ENV_SENSOR_HTS221_0              1U
#define USE_ENV_SENSOR_LPS22HH_0             1U

/* Motion Sensors usage */
#define USE_MOTION_SENSOR_ISM330DLC_0        1U
#define USE_MOTION_SENSOR_IIS2MDC_0          1U

/* COM  port usage */
#define USE_BSP_COM_FEATURE                  0U
#define USE_COM_LOG                          0U

/* IRQ priorities */
#define BSP_BUTTON_USERx_IT_PRIORITY         0x0FUL

/* I2C2 Frequency in Hz  */
#define BUS_I2C2_FREQUENCY                   100000UL /* Frequency of I2C2 = 100 KHz*/

/* Indicates whether or not TCXO is supported by the board
 * 0: TCXO not supported
 * 1: TCXO supported
 */
#define IS_TCXO_SUPPORTED                   1U

/* Indicates whether or not DCDC is supported by the board
 * 0: DCDC not supported
 * 1: DCDC supported
 */
#define IS_DCDC_SUPPORTED                   1U

#define STM32WL55C_DISCOVERY_I2C_Init BSP_I2C2_Init
#define STM32WL55C_DISCOVERY_I2C_DeInit BSP_I2C2_DeInit
#define STM32WL55C_DISCOVERY_I2C_ReadReg BSP_I2C2_ReadReg
#define STM32WL55C_DISCOVERY_I2C_WriteReg BSP_I2C2_WriteReg

#define STM32WL55C_DISCOVERY_GetTick BSP_GetTick


#ifdef __cplusplus
}
#endif

#endif /* STM32WL55C_DISCOVERY_CONF_H */
