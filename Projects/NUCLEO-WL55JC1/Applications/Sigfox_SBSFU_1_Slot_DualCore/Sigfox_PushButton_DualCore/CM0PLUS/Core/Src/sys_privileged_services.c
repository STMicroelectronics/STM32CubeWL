/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sys_privileged_services.c
  * @author  MCD Application Team
  * @brief   gives access to COrtex services like ThumbState, SVC, MPU
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
#include "sys_privileged_services.h" /* shall be placed before se_interface_application.h */
#include "se_interface_application.h"
#include "stm32_seq.h"
#include "subghz.h"
#if defined(__ARMCC_VERSION)
#include "mapping_sbsfu.h"
#elif defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#elif defined(__CC_ARM)
#include "mapping_fwimg.h"
#include "mapping_sbsfu.h"
#endif /* __ARMCC_VERSION */

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
void APP_NVIC_SVC_Handler(uint32_t *args);
void APP_CRITICALSECTION_SVC_Handler(uint32_t *args);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/

/**
  * @brief This functions triggers a CRITICAL_SECTION system call (supervisor call): request privileged operation
  * @param syscall The identifier of the operation to be called
  * @param arguments arguments of the privileged operation
  * @retval void
  */
#if defined (__ICCARM__) || defined(__GNUC__)
void SYS_CRITICALSECTION_SvcCall(uint32_t *ret, uint32_t syscall, ...)
{
#if defined ( __GNUC__ )
  /*
    * With GCC high C/C++++ optimisation level, as syscall parameter is unused, compiler does not initialize
    * syscall parameter (in r0) when caller is in same file. Forcing the parameter copy in r0 avoid the issue.
    */
  __ASM volatile("MOV r1, %0" : : "r"(syscall));
#endif /* __GNUC__ */
  /*
    * You cannot directly change to privileged mode from unprivileged mode without going through an exception, for
    * example an SVC.
    * Handled by @ref SVC_APP_Handler() and finally @ref SFU_SVC_APP_Handler()
    */
  __ASM volatile("SVC #1");  /* 1 is the hard-coded value to indicate NVIC syscall */
}
#elif defined(__CC_ARM)
/*
  * With Keil v5.x, as syscall parameter is unused, compiler does not initialize syscall parameter when
  * caller is in same file. Defining the function as ASM avoids the issue.
  */
void __ASM SYS_CRITICALSECTION_SvcCall(uint32_t *ret, uint32_t syscall, ...)
{
  /*
   * You cannot directly change to privileged mode from unprivileged mode without going through an exception, for
   * example an SVC.
   * Handled by @ref SVC_APP_Handler() and finally @ref SFU_SVC_APP_Handler()
   */
  SVC #1
  BX LR
}
#endif /* (__ICCARM__) || defined(__GNUC__) */

/**
  * @brief This functions triggers a NVIC system call (supervisor call): request privileged operation
  * @param syscall The identifier of the operation to be called
  * @param arguments arguments of the privileged operation
  * @retval void
  */
#if defined (__ICCARM__) || defined(__GNUC__)
void SYS_NVIC_SvcCall(uint32_t *ret, uint32_t syscall, ...)
{
#if defined ( __GNUC__ )
  /*
    * With GCC high C/C++++ optimisation level, as syscall parameter is unused, compiler does not initialize
    * syscall parameter (in r0) when caller is in same file. Forcing the parameter copy in r0 avoid the issue.
    */
  __ASM volatile("MOV r1, %0" : : "r"(syscall));
#endif /* __GNUC__ */
  /*
    * You cannot directly change to privileged mode from unprivileged mode without going through an exception, for
    * example an SVC.
    * Handled by @ref SVC_APP_Handler() and finally @ref SFU_SVC_APP_Handler()
    */
  __ASM volatile("SVC #2");   /* 2 is the hard-coded value to indicate NVIC syscall */
}
#elif defined(__CC_ARM)
/*
  * With Keil v5.x, as syscall parameter is unused, compiler does not initialize syscall parameter when
  * caller is in same file. Defining the function as ASM avoids the issue.
  */
void __ASM SYS_NVIC_SvcCall(uint32_t *ret, uint32_t syscall, ...)
{
  /*
   * You cannot directly change to privileged mode from unprivileged mode without going through an exception, for
   * example an SVC.
   * Handled by @ref SVC_APP_Handler() and finally @ref SFU_SVC_APP_Handler()
   */
  SVC #2
  BX LR
}
#endif /* (__ICCARM__) || defined(__GNUC__) */

/**
  * @brief This is the SVC Handler of the CM0 application (the one from vector table).
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

  switch (svc_index)
  {
    case 0x0: /* SE SVC CALL : called by SECoreBin*/
    {
      SE_APP_SVC_Handler(args);
      break;
    }
    case 0x1:
    {
      APP_CRITICALSECTION_SVC_Handler(args);
      break;
    }
    case 0x2:
    {
      APP_NVIC_SVC_Handler(args);
      break;
    }
    default:
    {
      break;
    }
  }
}

/**
  * @brief This is the handler that groups CRITICAL_SESSION functions via SVC calls.
  * @param args SVC arguments
  * @retval void
  */
void APP_CRITICALSECTION_SVC_Handler(uint32_t *args)
{
  uint32_t nvic_iser_state = 0;
  uint32_t *pRet = (uint32_t *)(args[0]);

  switch (args[1])
  {
    case SVC_DISABLE_ALL_NVIC_IRQS:
    {
      /*all __NVIC_GetEnableIRQ*/
      nvic_iser_state = NVIC->ISER[0];
      /* clear all positive interrupt e.g. no see IRQn_Type in core_cm0plus.h*/
      NVIC->ICER[0] = nvic_iser_state;
      break;
    }
    case SVC_RESTORE_NVIC_IRQS:
    {
      NVIC->ISER[0] = args[2] | NVIC->ISER[0];
      break;
    }
    case SVC_DISABLE_ALL_EXCEPTIONS:
    {
      __disable_irq();
      __set_CONTROL(__get_CONTROL() & 0xFFFE); /* bit 0 = 0: remain privileged   */
      /* note: exiting the SVC will go back to PSP stack but remain priv mode */
      break;
    }
    default:
    {
      break;
    }
  }
  *pRet = nvic_iser_state;
}

/**
  * @brief This is the handler that groups NVIC functions via SVC calls.
  * @param args SVC arguments
  * @retval void
  */
void APP_NVIC_SVC_Handler(uint32_t *args)
{
  uint32_t ret = 0;
  uint32_t *pRet = (uint32_t *)(args[0]);

  switch (args[1])
  {
    case SVC_NVIC_ENABLE_IRQ:
    {
      __NVIC_EnableIRQ((IRQn_Type) args[2]);
      break;
    }
    case SVC_NVIC_GET_ENABLE_IRQ:
    {
      ret = __NVIC_GetEnableIRQ((IRQn_Type) args[2]);
      break;
    }
    case SVC_NVIC_DISABLE_IRQ:
    {
      __NVIC_DisableIRQ((IRQn_Type) args[2]);
      break;
    }
    case SVC_NVIC_GET_PENDING_IRQ:
    {
      ret = __NVIC_GetPendingIRQ((IRQn_Type) args[2]);
      break;
    }
    case SVC_NVIC_SET_PENDING_IRQ:
    {
      __NVIC_SetPendingIRQ((IRQn_Type) args[2]);
      break;
    }
    case SVC_NVIC_CLEAR_PENDING_IRQ:
    {
      __NVIC_ClearPendingIRQ((IRQn_Type) args[2]);
      break;
    }
    case SVC_NVIC_SET_PRIORITY:
    {
      __NVIC_SetPriority((IRQn_Type) args[2], args[3]);
      break;
    }
    case SVC_NVIC_GET_PRIORITY:
    {
      ret = __NVIC_GetPriority((IRQn_Type) args[2]);
      break;
    }
    case SVC_NVIC_SYSTEM_RESET:
    {
      __NVIC_SystemReset();
      break;
    }
    default:
    {
      HAL_NVIC_SystemReset();
      break;
    }
  }
  *pRet = ret;
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
  __set_CONTROL(__get_CONTROL() | 3); /* bit 0 = 1: unprivileged      bit 1=1: stack=PSP */
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
