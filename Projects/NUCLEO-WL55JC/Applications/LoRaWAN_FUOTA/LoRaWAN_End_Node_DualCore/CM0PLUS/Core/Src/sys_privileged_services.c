/**
  ******************************************************************************
  * @file    sys_privileged_services.c
  * @author  MCD Application Team
  * @brief   gives access to COrtex services like ThumbState, SVC, MPU
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
#include "sys_privileged_services.h" /* shall be placed before se_interface_application.h */
#include "se_interface_application.h"
#include "stm32_seq.h"
#include "subghz.h"
#if defined(__ARMCC_VERSION)
#include "mapping_sbsfu.h"
#elif defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#endif /* __ARMCC_VERSION */

#if defined(__ARMCC_VERSION)
void SVC_MPU_Disable(void)
{
  __ASM volatile ("SVC #2");
}

void SVC_SYS_DisableAllNvicIrqs(uint32_t *nvic_iser_state)
{
  __ASM volatile ("SVC #2" : : "r" (nvic_iser_state));
}

void SVC_SYS_RestoreNvicIrqs(uint32_t nvic_iser_state)
{
  __ASM volatile ("SVC #3" : : "r" (nvic_iser_state));
}

void SVC_SYS_DisableAllExceptions(void)
{
  __ASM volatile ("SVC #4");
}

void SVC_MX_SUBGHZ_Init(void)
{
  __ASM volatile ("SVC #5");
}

void SVC_NVIC_EnableIRQ(IRQn_Type IRQn)
{
  __ASM volatile ("SVC #16" : : "r" (IRQn));
}

void SVC_NVIC_GetEnableIRQ(IRQn_Type IRQn, uint32_t *irqn_state)
{
  __ASM volatile ("SVC #17" : : "r" (IRQn), "r" (irqn_state));
}

void SVC_NVIC_DisableIRQ(IRQn_Type IRQn)
{
  __ASM volatile ("SVC #18" : : "r" (IRQn));
}

void SVC_NVIC_GetPendingIRQ(IRQn_Type IRQn, uint32_t *irqn_pending_state)
{
  __ASM volatile ("SVC #19" : : "r" (IRQn), "r" (irqn_pending_state));
}

void SVC_NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  __ASM volatile ("SVC #20" : : "r" (IRQn));
}

void SVC_NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  __ASM volatile ("SVC #21" : : "r" (IRQn));
}

void SVC_NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  __ASM volatile ("SVC #22" : : "r" (IRQn), "r" (priority));
}

void SVC_NVIC_GetPriority(IRQn_Type IRQn, uint32_t *irqn_prio)
{
  __ASM volatile ("SVC #23" : : "r" (IRQn), "r" (irqn_prio));
}

void SVC_NVIC_SystemReset(void)
{
  __ASM volatile ("SVC #24");
}
#endif /* __ARMCC_VERSION */

/**
  * @brief This is the SVC Handler of the CM0 application (the one downloaded via SBSFU).
  * @param args SVC arguments
  * @retval void
  */
void SVC_APP_Handler(uint32_t *args)
{
  /* in PSP (args) contains caller context as follow *
    * args[0] :R0
    * args[1] :R1
    * args[2] :R2
    * args[3] :R3
    * args[4] :R12
    * args[5] :LR
    * args[6] :PC to return after exception
    * args[7] :xPSR
  */
  uint8_t svc_index = ((char *)args[6])[-2];  
  uint32_t nvic_iser_state;
#if (__ARMCC_VERSION)
  uint32_t *ptr_args[2] = {(uint32_t *)args[0], (uint32_t *)args[1]};
#endif /*__ARMCC_VERSION */

  switch (svc_index)
  {
    case SVC_SE_APP_X_KMS: /* SE SVC CALL : called by SECoreBin*/
    {
      SE_APP_SVC_Handler(args);
      break;
    }
    case SVC_HAL_MPU_DISABLE :        /* HAL Disable MPU */
    {
      HAL_MPU_Disable();
      break;
    }
    case SVC_DISABLE_ALL_NVIC_IRQS:
    {
      /*all __NVIC_GetEnableIRQ*/
      nvic_iser_state = NVIC->ISER[0];
      /* clear all positive interrupt e.g. no see IRQn_Type in core_cm0plus.h*/
      NVIC->ICER[0] = nvic_iser_state;
#if (__ARMCC_VERSION)
      *ptr_args[0] = nvic_iser_state;
#else
      args[0] = nvic_iser_state;
#endif /*__ARMCC_VERSION */
      break;
    }
    case SVC_RESTORE_NVIC_IRQS:
    {
      NVIC->ISER[0] = args[0] | NVIC->ISER[0];
      break;
    }
    case SVC_DISABLE_ALL_EXCEPTIONS:
    {
      __disable_irq();
      __set_CONTROL(__get_CONTROL() & 0xFFFE); /* bit 0 = 0: remain privileged   */
      /* note: exiting the SVC will go back to PSP stack but remain priv mode */
      break;
    }
    case SVC_SUBGHZ_INIT:
    {
      MX_SUBGHZ_Init();
      break;
    }
    case SVC_NVIC_ENABLE_IRQ:
    {
      __NVIC_EnableIRQ((IRQn_Type) args[0]);
      break;
    }
    case SVC_NVIC_GET_ENABLE_IRQ:
    {
#if __ARMCC_VERSION
      *ptr_args[1] = __NVIC_GetEnableIRQ((IRQn_Type) args[0]);
#else
      __NVIC_GetEnableIRQ((IRQn_Type) args[0]);
#endif
      break;
    }
    case SVC_NVIC_GET_PENDING_IRQ:
    {
#if __ARMCC_VERSION
      *ptr_args[1] = __NVIC_GetPendingIRQ((IRQn_Type) args[0]);
#else
      __NVIC_GetPendingIRQ((IRQn_Type) args[0]);
#endif
      break;
    }
    case SVC_NVIC_SET_PENDING_IRQ:
    {
      __NVIC_SetPendingIRQ((IRQn_Type) args[0]);
      break;
    }
    case SVC_NVIC_CLEAR_PENDING_IRQ:
    {
      __NVIC_ClearPendingIRQ((IRQn_Type) args[0]);
      break;
    }
    case SVC_NVIC_SET_PRIORITY:
    {
      __NVIC_SetPriority((IRQn_Type) args[0], args[1]);
      break;
    }
    case SVC_NVIC_GET_PRIORITY:
    {
#if __ARMCC_VERSION
      *ptr_args[1] = __NVIC_GetPriority((IRQn_Type) args[0]);
#else
      __NVIC_GetPriority((IRQn_Type) args[0]);
#endif
      break;
    }
    case SVC_NVIC_SYSTEM_RESET:
    {
      __NVIC_SystemReset();
      break;
    }
    default:
    {
      break;
    }
  }
}

/**
  * @brief Maps MSP on Privileged region and set up PSP to current stack pointer 
  * @param void
  * @retval void
  */
void ThumbState_RemapMspAndSwitchToPspStack(void)
{
  __set_PSP(__get_MSP()); /* set up Process Stack Pointer to current stack pointer */
  __set_MSP(SE_REGION_RAM_STACK_TOP); /* change Main Stack to point on privileged stack */
  __ISB();
}

/**
  * @brief This is a helper function to enter the unprivileged level for software execution
  * @param void
  * @retval void
  */
void ThumbState_EnterUnprivilegedMode(void)
{
  __set_CONTROL(__get_CONTROL() | 3); /* bit 0 = 1: unpriviledged      bit 1=1: stack=PSP */
  __ISB();
}


/**
  * @brief This is a helper function to determine if we are currently running in non-privileged mode or not
  * @param void
  * @retval 0 if we are in privileged mode, 1 if we are in non-privileged mode
  */
uint32_t ThumbState_IsUnprivileged(void)
{
  return ((__get_IPSR() == 0U) && ((__get_CONTROL() & 1U) == 1U));
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
