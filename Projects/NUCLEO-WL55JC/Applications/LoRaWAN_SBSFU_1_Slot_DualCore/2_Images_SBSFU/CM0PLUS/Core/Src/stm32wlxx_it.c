/**
  ******************************************************************************
  * @file    stm32wlxx_it.c
  * @author  MCD Application Team
  * @brief   Interrupt Service Routines.
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32wlxx_it.h"
#include "sfu_low_level_security.h"

/* Global variables ----------------------------------------------------------*/
/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;

/* Functions Definition ------------------------------------------------------*/
/******************************************************************************/
/*            Cortex-M0+ Processor Interruption and Exception Handlers         */
/******************************************************************************/

/* Exception handlers are implemented in the various source files of sbsfu application */

/* NMI exception handler is implemented in sfu_low_level_flash_int.c file */
/* Hard Fault exception handler is implemented in sfu_boot.c file */

/* SVC call handler is implemented in se_sfu_mpu.s */


/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/*                 STM32WLxx Peripherals Interrupt Handlers                  */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32wlxx.s).                                               */
/******************************************************************************/
#ifdef SFU_TAMPER_PROTECT_ENABLE
/**
  * @brief  This function handles Tamper interrupt request.
  * @param  None
  * @retval None
  */
void RTC_LSECSS_IRQHandler()
{
  /* Handle only the tamper enabled by SBSFU */
  uint32_t tmp = READ_REG(TAMP->MISR) & RTC_TAMPER_ID;

  /* Immediately clear flags to avoid eternal resets */
  WRITE_REG(TAMP->SCR, tmp);

  /* SRAM2 is erased and RtcHandle is corrupted: no specific handling is possible. */
  NVIC_SystemReset();
}
#endif /* SFU_TAMPER_PROTECT_ENABLE */

#ifdef SFU_GTZC_PROTECT_ENABLE
/**
  * @brief  This function handles TZIC interrupt request.
  * @param  None
  * @retval None
  */
void TZIC_ILA_IRQHandler(void)
{
  HAL_GTZC_IRQHandler();
}
#endif /* SFU_GTZC_PROTECT_ENABLE */




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
