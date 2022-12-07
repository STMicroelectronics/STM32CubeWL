/**
  ******************************************************************************
  * @file    mpu.c
  * @author  MCD Application Team
  * @brief   virtual HAL api for non privilieged :
  *          implementing HAL service performing access to register being
  *          mapped by mpu  on privilieged area
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
#include "mpu.h"
#include "se_def.h"
#include "se_interface_application.h"
#include "common.h"
#include "flash_if.h"
#include "se_callgate.h"
#if defined(__ARMCC_VERSION)
#include "mapping_sbsfu.h"
#elif defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#endif /* __ARMCC_VERSION */

#define HAL_SVC_NVIC_SYSTEM_RESET            0
#define HAL_SVC_MPU_DISABLE                  1

static void HAL_SysCall(uint32_t syscall, ...);

/**
  * @brief This functions triggers a HAL system call (supervisor call): request privileged operation
  * @param syscall The identifier of the operation to be called (see @ref SFU_MPU_PrivilegedOpId)
  * @param arguments arguments of the privileged operation
  * @retval void
  */
#if defined (__ICCARM__) || defined(__GNUC__)
static void HAL_SysCall(uint32_t syscall, ...)
{
#if defined ( __GNUC__ )
  /*
    * With GCC high C/C++++ optimisation level, as syscall parameter is unused, compiler does not initialize
    * syscall parameter (in r0) when caller is in same file. Forcing the parameter copy in r0 avoid the issue.
    */
  __ASM volatile("MOV r0, %0" : : "r"(syscall));
#endif /* __GNUC__ */
  /*
    * You cannot directly change to privileged mode from unprivileged mode without going through an exception, for
    * example an SVC.
    * Handled by @ref MPU_SVC_Handler() and finally @ref SFU_MPU_SVC_Handler()
    */
  __ASM volatile("SVC #1");   /* 1 is the hard-coded value to indicate HAL syscall */
}
#elif defined(__CC_ARM)
/*
  * With Keil v5.x, as syscall parameter is unused, compiler does not initialize syscall parameter when
  * caller is in same file. Defining the function as ASM avoids the issue.
  */
static void __ASM HAL_SysCall(uint32_t syscall, ...)
{
  /*
   * You cannot directly change to privileged mode from unprivileged mode without going through an exception, for
   * example an SVC.
   * Handled by @ref MPU_SVC_Handler() and finally @ref SFU_MPU_SVC_Handler()
   */
  SVC #1
  BX LR
}
#endif /* (__ICCARM__) || defined(__GNUC__) */

/**
  * @brief  Request a system reset after switching in privileged mode.
  * @param  None
  * @retval None
  */
void SVC_NVIC_SystemReset(void)
{
  HAL_SysCall(HAL_SVC_NVIC_SYSTEM_RESET);
}


/**
  * @brief  Request to disable the MPU after switching in privileged mode.
  * @param  None
  * @retval None
  */
void SVC_MPU_Disable(void)
{
  HAL_SysCall(HAL_SVC_MPU_DISABLE);
}


/**
  * @brief This is the Supervisor calls handler.
  * @param args SVC arguments
  * @retval void
  * @note Installed in startup_stm32xxxx.s
  *
  * This handler handles 2 requests:
  * \li Secure Engine SVC: run a Secure Engine privileged operation provided as a parameter (see @ref
  * \li SE_FunctionIDTypeDef)
  * \li HAL SVC: run a HAL privileged operation provided as a parameter
  */
void MPU_SVC_Handler(uint32_t *args)
{
  uint8_t code = ((char *)args[6])[-2];
  switch (code)
  {
    case 0: /* SE SVC CALL */
    {
      SE_APP_SVC_Handler(args);
      break;
    }
    case 1:  /* HAL SVC CALL */
    {
      HAL_APP_SVC_Handler(args);
      break;
    }
    default:
    {
      break;
    }
  }
}
/**
  * @brief This is the HAL Supervisor calls handler.
  * @param args SVC arguments
  * @retval void
  *
  * This handler handles 2 requests:
  * \li NVIC system reset
  * \li MPU disable request
  */
void HAL_APP_SVC_Handler(uint32_t *args)
{
  switch (args[0])
  {
    case HAL_SVC_NVIC_SYSTEM_RESET :  /* HAL NVIC system reset */
    {
      HAL_NVIC_SystemReset();
      break;
    }
    case HAL_SVC_MPU_DISABLE :        /* HAL Disable MPU */
    {
      HAL_MPU_Disable();
      break;
    }
    default:
    {
      HAL_NVIC_SystemReset();
      break;
    }
  }
}


/**
  * @brief This is a helper function to enter the unprivileged level for software execution
  * @param void
  * @retval void
  */
void MPU_EnterUnprivilegedMode(void)
{
  __set_PSP(__get_MSP()); /* set up Process Stack Pointer to current stack pointer */
  __set_MSP(SE_REGION_RAM_STACK_TOP); /* change main stack to point on privileged stack */
  __set_CONTROL(__get_CONTROL() | 3); /* bit 0 = 1: unpriviledged      bit 1=1: stack=PSP */
  __ISB();
}
