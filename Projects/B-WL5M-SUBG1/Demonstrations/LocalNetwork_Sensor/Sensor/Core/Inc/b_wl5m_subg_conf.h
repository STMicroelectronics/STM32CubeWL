/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    b_wl5m_subg_conf.h
  * @author  MCD Application Team
  * @brief   configuration file.
  *          This file is a copy of template b_wl5m_subg_conf_template.h
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
#ifndef B_WL5M_SUBG_CONF_H
#define B_WL5M_SUBG_CONF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* USER CODE BEGIN EC */

/* Environmental Sensors usage */
#define USE_ENV_SENSOR_STTS22H_0             1U  /* Temperature */
#define USE_ENV_SENSOR_ILPS22QS_0            1U  /* Pressure */

/* Motion Sensors usage */
#define USE_MOTION_SENSOR_ISM330DHCX_0       1U
#define USE_MOTION_SENSOR_IIS2MDC_0          1U

/* COM  port usage */
#define USE_BSP_COM_FEATURE                  0U
#define USE_COM_LOG                          0U

/* IRQ priorities */
#define BSP_BUTTON_USERx_IT_PRIORITY         14U

/* I2C2 Frequency in Hz  */
#define BUS_I2C2_FREQUENCY                   100000UL /* Frequency of I2C2 = 100 KHz*/

#define B_WL5M_SUBG_I2C_Init BSP_I2C2_Init
#define B_WL5M_SUBG_I2C_DeInit BSP_I2C2_DeInit
#define B_WL5M_SUBG_I2C_ReadReg BSP_I2C2_ReadReg
#define B_WL5M_SUBG_I2C_WriteReg BSP_I2C2_WriteReg

#define B_WL5M_SUBG_GetTick BSP_GetTick

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* B_WL5M_SUBG_CONF_H */
