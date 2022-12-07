/**
  ******************************************************************************
  * @file    stm32_systime_if_template.h
  * @author  MCD Application Team
  * @brief   This file provides the ll driver for  the time server
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_SYSTIME_IF_H__
#define __STM32_SYSTIME_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "stm32_systime.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/*!
 * @brief Get rtc time
 * @param [OUT] subSeconds in ticks
 * @retval returns time seconds
 */
uint32_t PPP_GetTime(uint16_t *subSeconds);

/*!
 * @brief write seconds in backUp register
 * @Note Used to store seconds difference between RTC time and Unix time
 * @param [IN] time in seconds
 * @retval None
 */
void PPP_BkUp_Write_Seconds(uint32_t Seconds);

/*!
 * @brief reads seconds from backUp register
 * @Note Used to store seconds difference between RTC time and Unix time
 * @param [IN] None
 * @retval Time in seconds
 */
uint32_t PPP_BkUp_Read_Seconds(void);

/*!
 * @brief writes SubSeconds in backUp register
 * @Note Used to store SubSeconds difference between RTC time and Unix time
 * @param [IN] time in SubSeconds
 * @retval None
 */
void PPP_BkUp_Write_SubSeconds(uint32_t SubSeconds);

/*!
 * @brief reads SubSeconds from backUp register
 * @Note Used to store SubSeconds difference between RTC time and Unix time
 * @param [IN] None
 * @retval Time in SubSeconds
 */
uint32_t PPP_BkUp_Read_SubSeconds(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32_TIMER_IF_H__ */
