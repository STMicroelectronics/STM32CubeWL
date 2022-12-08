/**
  ******************************************************************************
  * @file    b_wl5m_subg_radio.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage:
  *          - RF circuitry available on B-WL5M-SUBG
  *            Kit from STMicroelectronics
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

/* Includes ------------------------------------------------------------------*/
#include "b_wl5m_subg_radio.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup B_WL5M_SUBG
  * @{
  */

/** @addtogroup B_WL5M_SUBG_RADIO
  * @brief This file provides set of firmware functions to Radio switch
  *        available on B-WL5M-SUBG Kit from STMicroelectronics.
  * @{
  */

/** @addtogroup B_WL5M_SUBG_RADIO_Exported_Functions
  * @{
  */

/**
  * @brief  Return Board Configuration
  * @retval
  *    RADIO_CONF_RFO_LP_HP
  *    RADIO_CONF_RFO_LP
  *    RADIO_CONF_RFO_HP
  */
int32_t BSP_RADIO_GetTxConfig(void)
{
  return RADIO_CONF_RFO_LP_HP;
}

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
