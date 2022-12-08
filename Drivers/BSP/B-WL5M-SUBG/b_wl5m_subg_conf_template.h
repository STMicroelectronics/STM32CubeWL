/**
  ******************************************************************************
  * @file    b_wl5m_subg_conf.h
  * @author  MCD Application Team
  * @brief   B-WL5M-SUBG board configuration file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef B_WL5M_SUBG_CONF_H
#define B_WL5M_SUBG_CONF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup B_WL5M_SUBG
  * @{
  */

/** @defgroup B_WL5M_SUBG_CONFIG B-WL5M-SUBG CONFIG
  * @{
  */

/** @defgroup B_WL5M_SUBG_CONFIG_Exported_Constants B-WL5M-SUBG CONFIG Exported Constants
  * @{
  */

/* Environmental Sensors usage */
#define USE_ENV_SENSOR_STTS22H_0             0U
#define USE_ENV_SENSOR_ILPS22QS_0            0U

/* Motion Sensors usage */
#define USE_MOTION_SENSOR_ISM330DHCX_0       0U
#define USE_MOTION_SENSOR_IIS2MDC_0          0U

/* COM port usage */
#if defined(USE_VCP_CONNECTION)
#define USE_BSP_COM_FEATURE                  1U
#define USE_COM_LOG                          1U
#else
#define USE_BSP_COM_FEATURE                  0U
#define USE_COM_LOG                          0U
#endif /* USE_VCP_CONNECTION */

/* IRQ priorities */
#define BSP_BUTTON_USERx_IT_PRIORITY         15U

/* I2C2 Frequency in Hz  */
#define BUS_I2C2_FREQUENCY                   100000UL /* Frequency of I2C2 = 100 KHz */

#define B_WL5M_SUBG_I2C_Init BSP_I2C2_Init
#define B_WL5M_SUBG_I2C_DeInit BSP_I2C2_DeInit
#define B_WL5M_SUBG_I2C_ReadReg BSP_I2C2_ReadReg
#define B_WL5M_SUBG_I2C_WriteReg BSP_I2C2_WriteReg

#define B_WL5M_SUBG_GetTick BSP_GetTick

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* B_WL5M_SUBG_CONF_H */
