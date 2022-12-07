/**
  ******************************************************************************
  * @file    stm32wlxx_it.c
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32wlxx_it.h"
#include "test_protections.h"
#include "mpu.h"

/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup USER_APP_COMMON Common
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef RtcHandle;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0+ Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
  /* Go to infinite loop when NMI exception occurs */
  while (1)
  {
    printf("NMI exception!\r");
    NVIC_SystemReset();
  }
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
    printf("Hard Fault exception!\r");
    NVIC_SystemReset();
  }
}


/* SVC call handler is implemented in svc_handler.s */


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
/*                 STM32WLxx Peripherals Interrupt Handlers             */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32wlxx.s).                                         */
/******************************************************************************/

/**
  * @brief  This function handles Flash operation error interrupt request.
  * @param  None
  * @retval None
  */
void FLASH_IRQHandler(void)
{
  HAL_FLASH_IRQHandler();
}

/**
  * @brief  This function handles Tamper interrupt request.
  * @param  None
  * @retval None
  */
void RTC_LSECSS_IRQHandler(void)
{
  /* Handle only the tamper enabled by SBSFU */
  uint32_t tmp = READ_REG(TAMP->MISR) & RTC_TAMPER_ID;

  /* Immediately clear flags to avoid eternal resets */
  WRITE_REG(TAMP->SCR, tmp);

  /* SRAM2 is erased and RtcHandle is corrupted: no specific handling is possible. */
  NVIC_SystemReset();
}

/**
  * @brief  This function handles TZIC interrupt request.
  * @param  None
  * @retval None
  */
void TZIC_ILA_IRQHandler(void)
{
  HAL_GTZC_IRQHandler();
}

void BUTTON_IRQHANDLER(void)
{
  HAL_GPIO_EXTI_IRQHandler(BUTTON_EXTI_LINE);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == BUTTON_EXTI_LINE)
  {
    printf("\r\nUser IRQ Handler: User Button pressed\r\n");
  }
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */

/**
  * @}
  */
