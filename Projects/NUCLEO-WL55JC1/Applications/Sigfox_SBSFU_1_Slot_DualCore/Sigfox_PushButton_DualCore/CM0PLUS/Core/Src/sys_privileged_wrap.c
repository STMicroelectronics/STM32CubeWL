/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sys_privileged_wrap.c
  * @author  MCD Application Team
  * @brief   Wrapper functions to call SVC when in Unprivileged mode
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sys_privileged_wrap.h"
#include "sys_privileged_services.h"
#include "stm32_seq.h"
#include "subghz.h"
#include "sys_conf.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

uint32_t SYS_PRIVIL_EnterCriticalSection(void)
{
  uint32_t nvic_iser_state;

  if (ThumbState_IsUnprivileged() != 0)
  {
    /* disable NVIC irqs, then back to PSP and Unpriv */
    SYS_CRITICALSECTION_SvcCall(&nvic_iser_state, SVC_DISABLE_ALL_NVIC_IRQS);
  }
  else
  {
    nvic_iser_state = NVIC->ISER[0];
    NVIC->ICER[0] = nvic_iser_state;
  }
  return nvic_iser_state;
}

void SYS_PRIVIL_ExitCriticalSection(uint32_t nvic_iser_state)
{
  if (ThumbState_IsUnprivileged() != 0)
  {
    /* enable NVIC irqs, then back to PSP and Unpriv */
    uint32_t dummy_ret = 0;
    SYS_CRITICALSECTION_SvcCall(&dummy_ret, SVC_RESTORE_NVIC_IRQS, nvic_iser_state);
  }
  else
  {
    NVIC->ISER[0] = nvic_iser_state | NVIC->ISER[0];
  }
}

void SYS_PRIVIL_DisableIrqsAndRemainPriv(void)
{
  if (ThumbState_IsUnprivileged() != 0)
  {
    /* disable exceptions and after SVC keeps MODE Privileged! (stack goes back to PSP)*/
    uint32_t dummy_ret = 0;
    SYS_CRITICALSECTION_SvcCall(&dummy_ret, SVC_DISABLE_ALL_EXCEPTIONS);
  }
  else
  {
    __disable_irq();
  }
}

void SYS_PRIVIL_EnableIrqsAndGoUnpriv(void)
{
  __enable_irq();
#if defined (SECURE_UNPRIVILEGE_ENABLE) && (SECURE_UNPRIVILEGE_ENABLE == 1)
  ThumbState_EnterUnprivilegedMode(); /* Goes always Unpriv */
#elif !defined (SECURE_UNPRIVILEGE_ENABLE)
#error SECURE_UNPRIVILEGE_ENABLE not defined
#endif /* SECURE_UNPRIVILEGE_ENABLE */
}

void SYS_PRIVIL_NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if (ThumbState_IsUnprivileged() != 0)
  {
    uint32_t dummy_ret = 0;
    SYS_NVIC_SvcCall(&dummy_ret, SVC_NVIC_ENABLE_IRQ, IRQn);
  }
  else
  {
    __NVIC_EnableIRQ(IRQn);
  }
}

uint32_t SYS_PRIVIL_NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
  uint32_t irqn_state;

  if (ThumbState_IsUnprivileged() != 0)
  {
    SYS_NVIC_SvcCall(&irqn_state, SVC_NVIC_GET_ENABLE_IRQ, IRQn);
  }
  else
  {
    irqn_state = __NVIC_GetEnableIRQ(IRQn);
  }
  return irqn_state;
}

void SYS_PRIVIL_NVIC_DisableIRQ(IRQn_Type IRQn)
{
  if (ThumbState_IsUnprivileged() != 0)
  {
    uint32_t dummy_ret = 0;
    SYS_NVIC_SvcCall(&dummy_ret, SVC_NVIC_DISABLE_IRQ, IRQn);
  }
  else
  {
    __NVIC_DisableIRQ(IRQn);
  }
}

uint32_t SYS_PRIVIL_NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  uint32_t irqn_pending_state;

  if (ThumbState_IsUnprivileged() != 0)
  {
    SYS_NVIC_SvcCall(&irqn_pending_state, SVC_NVIC_GET_PENDING_IRQ, IRQn);
  }
  else
  {
    irqn_pending_state = __NVIC_GetPendingIRQ(IRQn);
  }
  return irqn_pending_state;
}

void SYS_PRIVIL_NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if (ThumbState_IsUnprivileged() != 0)
  {
    uint32_t dummy_ret = 0;
    SYS_NVIC_SvcCall(&dummy_ret, SVC_NVIC_SET_PENDING_IRQ, IRQn);
  }
  else
  {
    __NVIC_SetPendingIRQ(IRQn);
  }
}

void SYS_PRIVIL_NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if (ThumbState_IsUnprivileged() != 0)
  {
    uint32_t dummy_ret = 0;
    SYS_NVIC_SvcCall(&dummy_ret, SVC_NVIC_CLEAR_PENDING_IRQ, IRQn);
  }
  else
  {
    __NVIC_ClearPendingIRQ(IRQn);
  }
}

void SYS_PRIVIL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if (ThumbState_IsUnprivileged() != 0)
  {
    uint32_t dummy_ret = 0;
    SYS_NVIC_SvcCall(&dummy_ret, SVC_NVIC_SET_PRIORITY, IRQn, priority);
  }
  else
  {
    __NVIC_SetPriority(IRQn, priority);
  }
}

uint32_t SYS_PRIVIL_NVIC_GetPriority(IRQn_Type IRQn)
{
  uint32_t irqn_prio;

  if (ThumbState_IsUnprivileged() != 0)
  {
    SYS_NVIC_SvcCall(&irqn_prio, SVC_NVIC_GET_PRIORITY, IRQn);
  }
  else
  {
    irqn_prio = __NVIC_GetPriority(IRQn);
  }
  return irqn_prio;
}

void SYS_PRIVIL_NVIC_SystemReset(void)
{
  if (ThumbState_IsUnprivileged() != 0)
  {
    uint32_t dummy_ret = 0;
    SYS_NVIC_SvcCall(&dummy_ret, SVC_NVIC_SYSTEM_RESET);
  }
  else
  {
    __NVIC_SystemReset();
  }
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
