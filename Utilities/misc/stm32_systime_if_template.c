/**
  ******************************************************************************
  * @file    stm32_systime_if_template.c
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

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include "stm32_systime_if.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
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
