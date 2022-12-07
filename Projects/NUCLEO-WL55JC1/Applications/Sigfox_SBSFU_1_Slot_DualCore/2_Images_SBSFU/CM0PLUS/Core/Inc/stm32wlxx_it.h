/**
  ******************************************************************************
  * @file    stm32wlxx_it.h
  * @author  MCD Application Team
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32WLxx_IT_H
#define STM32WLxx_IT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"

/* External variables --------------------------------------------------------*/
extern RTC_HandleTypeDef    RtcHandle;

/* Exported functions ------------------------------------------------------- */
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
#ifdef SFU_TAMPER_PROTECT_ENABLE
void RTC_LSECSS_IRQHandler(void);
#endif /* SFU_TAMPER_PROTECT_ENABLE */
#ifdef SFU_GTZC_PROTECT_ENABLE
void TZIC_ILA_IRQHandler(void);
#endif /* SFU_GTZC_PROTECT_ENABLE */



#ifdef __cplusplus
}
#endif

#endif /* STM32WLxx_IT_H */