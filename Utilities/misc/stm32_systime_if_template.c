 /*******************************************************************************
 * File Name          : stm32_timer_if.c
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

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include "stm32_systime_if.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables ---------------------------------------------------------*/
/*System Time driver*/
const UTIL_SYSTIM_Driver_s UTIL_SYSTIMDriver =
{
  PPP_BkUp_Write_Seconds,
  PPP_BkUp_Read_Seconds,
  PPP_BkUp_Write_SubSeconds,
  PPP_BkUp_Read_SubSeconds,
  PPP_GetTime,
};

uint32_t RTC_IF_GetTime(uint16_t *mSeconds)
{
  return 0;
}

void RTC_IF_BkUp_Write_Seconds(uint32_t Seconds)
{

}

void RTC_IF_BkUp_Write_SubSeconds(uint32_t SubSeconds)
{

}

uint32_t RTC_IF_BkUp_Read_Seconds(void)
{
  return 0;
}

uint32_t RTC_IF_BkUp_Read_SubSeconds(void)
{
  return 0;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

