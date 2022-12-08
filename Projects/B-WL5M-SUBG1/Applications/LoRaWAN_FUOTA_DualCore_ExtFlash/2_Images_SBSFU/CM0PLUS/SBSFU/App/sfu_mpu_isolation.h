/**
  ******************************************************************************
  * @file    sfu_mpu_isolation.h
  * @author  MCD Application Team
  * @brief   SFU MPU isolation primitives
  *          This file provides functions to manage the MPU isolation of the Secure Engine.
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




/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SFU_MPU_ISOLATION_H
#define SFU_MPU_ISOLATION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sfu_def.h"

/* Exported types ------------------------------------------------------------*/
/**
  *  @brief SFU BOOT Privileged operation identifier.
  *         SB_SFU can trigger a SB_SFU privileged operation by calling a syscall (@ref SFU_MPU_SysCall) with one of
  *         the values of this enum.
  */
typedef enum
{
  SB_SYSCALL_LAUNCH_APP = 1, /*!< SB_SFU service call identifier: launch the user application */
  SB_SYSCALL_RESET,          /*!< SB_SFU service call identifier: system reset */
  SB_SYSCALL_MPU_CONFIG,     /*!< SB_SFU service call identifier: MPU re-configuration */
  SB_SYSCALL_DMA_CONFIG,     /*!< SB_SFU service call identifier: DMA re-configuration */
  SB_SYSCALL_DAP_CONFIG,     /*!< SB_SFU service call identifier: DAP re-configuration */
#ifdef SFU_GTZC_PROTECT_ENABLE
  SB_SYSCALL_GTZC_CONFIG,    /*!< SB_SFU service call identifier: GTZC re-configuration */
#endif /* SFU_GTZC_PROTECT_ENABLE */
  SB_SYSCALL_TAMPER_CONFIG   /*!< SB_SFU service call identifier: TAMPER re-configuration */
} SFU_MPU_PrivilegedOpId;

/* Exported functions ------------------------------------------------------- */
/* SVC call handler for Secure Engine: this function is defined in se_interface_common.c */
extern void SE_SVC_Handler(uint32_t *args);

/* SB_SFU SVC handling */
void MPU_SVC_Handler(uint32_t *args);
void SFU_MPU_SVC_Handler(uint32_t *args);
void SFU_MPU_SysCall(uint32_t syscall, ...);

/* Helpers for unprivileged mode handling */
void SFU_MPU_EnterUnprivilegedMode(void);
uint32_t SFU_MPU_IsUnprivileged(void);

/**
  * @brief This function sets the Stack Pointer and Program Counter to jump into a function
  * @param  vector_address Start Address (where to jump)
  * @retval void
  */
void jump_to_function(uint8_t *vector_address);

/**
  * @brief This function returns from an interrupt and launches the user application code
  * @param  applicationVectorAddress Start address of the user application (active slot+offset: vectors)
  * @param  exitFunctionAddress Function to jump into the user application code
  * @param  address Function to jump into the BL code in case of exit_sticky usage
  * @param  magic number used by BL code in case of exit_sticky usage
  * @retval void
  */
void launch_application(uint32_t applicationVectorAddress, uint32_t exitFunctionAddress);

#ifdef __cplusplus
}
#endif

#endif /* SFU_MPU_ISOLATION_H */

