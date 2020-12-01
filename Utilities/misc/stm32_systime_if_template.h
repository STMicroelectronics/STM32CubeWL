/**
 ******************************************************************************
 * File Name          : stm32_timer_if.h
 * Description        : This file provides the ll driver for  the time server
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32_SYSTIME_IF_H__
#define STM32_SYSTIME_IF_H__

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

#endif /* STM32_TIMER_IF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
