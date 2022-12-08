/**
  ******************************************************************************
  * @file    b_wl5m_subg_radio.h
  * @author  MCD Application Team
  * @brief   Header for b_wl5m_sug_radio.c
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
#ifndef B_WL5M_SUBG_RADIO_H
#define B_WL5M_SUBG_RADIO_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "b_wl5m_subg_errno.h"
#include "b_wl5m_subg_conf.h"


/** @addtogroup BSP
  * @{
  */

/** @addtogroup B_WL5M_SUBG B-WL5M-SUBG
  * @{
  */

/** @defgroup B_WL5M_SUBG_RADIO B-WL5M-SUBG RADIO
  * @{
  */

/** @defgroup B_WL5M_SUBG_RADIO_Exported_Constants B-WL5M-SUBG RADIO Exported Constants
  * @{
  */

/** @defgroup B_WL5M_SUBG_RADIO_CONFIG B-WL5M-SUBG RADIO CONFIG Constants
  * @{
  */
#define RADIO_CONF_RFO_LP_HP                     0U
#define RADIO_CONF_RFO_LP                        1U
#define RADIO_CONF_RFO_HP                        2U

/**
  * @}
  */

/**
  * @}
  */

/** @defgroup B_WL5M_SUBG_RADIO_Exported_Functions B-WL5M-SUBG RADIO Exported Functions
  * @{
  */
int32_t BSP_RADIO_GetTxConfig(void);

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

#endif /* B_WL5M_SUBG_RADIO_H */
