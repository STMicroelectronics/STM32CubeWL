/**
  ******************************************************************************
  * @file    test_protections.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Test Protections functionalities.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef TEST_PROTECTIONS_H
#define TEST_PROTECTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported constants --------------------------------------------------------*/
/* Tamper instance used by SBSFU */
#define RTC_TAMPER_ID RTC_TAMPER_1

/* Exported macros -----------------------------------------------------------*/
/* SFU Redirect of RTC Tamper Event Callback*/
#define  CALLBACK_Antitamper(void) HAL_RTCEx_Tamper1EventCallback(RTC_HandleTypeDef *hrtc)
/* SFU Redirect of GTZC_TZIC Event Callback*/
#define  CALLBACK_GTZC_TZIC(void) HAL_GTZC_TZIC_Callback(uint32_t PeriphId)

/* Exported functions ------------------------------------------------------- */
void     TEST_PROTECTIONS_RunMenu(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_PROTECTIONS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

