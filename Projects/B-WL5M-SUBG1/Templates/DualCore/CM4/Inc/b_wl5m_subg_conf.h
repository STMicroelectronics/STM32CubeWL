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

/** @defgroup B_WL5M_SUBG_CONFIG CONFIG
  * @{
  */

/** @defgroup B_WL5M_SUBG_CONFIG_Exported_Constants Exported Constants
  * @{
  */
/* COM usage define */
#define USE_BSP_COM_FEATURE                 0U

/* COM log define */
#define USE_COM_LOG                         0U

/* IRQ priorities */
#define BSP_BUTTON_USERx_IT_PRIORITY        0x0FU

/* I2C2 Frequency in Hz  */
#define BUS_I2C2_FREQUENCY                   100000UL /* Frequency of I2C2 = 100 KHz*/

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
