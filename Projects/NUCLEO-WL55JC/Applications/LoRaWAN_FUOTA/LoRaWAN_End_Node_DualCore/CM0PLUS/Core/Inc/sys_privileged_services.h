/**
  ******************************************************************************
  * @file    sys_privileged_services.h
  * @brief   Maps sys_privileged_wrap functions on SVC and API to cortex services
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYS_PRIVILEGED_SERVICES_H__
#define __SYS_PRIVILEGED_SERVICES_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "platform.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define SFU_ISOLATE_SE_WITH_MPU

#define SVC_SE_APP_X_KMS                     0x0
#define SVC_HAL_MPU_DISABLE                  0x1
#define SVC_DISABLE_ALL_NVIC_IRQS            0x2
#define SVC_RESTORE_NVIC_IRQS                0x3
#define SVC_DISABLE_ALL_EXCEPTIONS           0x4
#define SVC_SUBGHZ_INIT                      0x5

#define SVC_NVIC_ENABLE_IRQ                 0x10
#define SVC_NVIC_GET_ENABLE_IRQ             0x11
#define SVC_NVIC_DISABLE_IRQ                0x12
#define SVC_NVIC_GET_PENDING_IRQ            0x13
#define SVC_NVIC_SET_PENDING_IRQ            0x14
#define SVC_NVIC_CLEAR_PENDING_IRQ          0x15
#define SVC_NVIC_SET_PRIORITY               0x16
#define SVC_NVIC_GET_PRIORITY               0x17
#define SVC_NVIC_SYSTEM_RESET               0x18

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

#if defined(__CC_ARM)
void     __svc(SVC_HAL_MPU_DISABLE)         SVC_MPU_Disable(void);
uint32_t __svc(SVC_DISABLE_ALL_NVIC_IRQS)   SVC_SYS_DisableAllNvicIrqs(void);
void     __svc(SVC_RESTORE_NVIC_IRQS)       SVC_SYS_RestoreNvicIrqs(uint32_t nvic_iser_state);
void     __svc(SVC_DISABLE_ALL_EXCEPTIONS)  SVC_SYS_DisableAllExceptions(void);
void     __svc(SVC_SUBGHZ_INIT)             SVC_MX_SUBGHZ_Init(void);

void     __svc(SVC_NVIC_ENABLE_IRQ)         SVC_NVIC_EnableIRQ(IRQn_Type IRQn);
uint32_t __svc(SVC_NVIC_GET_ENABLE_IRQ)     SVC_NVIC_GetEnableIRQ(IRQn_Type IRQn);
void     __svc(SVC_NVIC_DISABLE_IRQ)        SVC_NVIC_DisableIRQ(IRQn_Type IRQn);
uint32_t __svc(SVC_NVIC_GET_PENDING_IRQ)    SVC_NVIC_GetPendingIRQ(IRQn_Type IRQn);
void     __svc(SVC_NVIC_SET_PENDING_IRQ)    SVC_NVIC_SetPendingIRQ(IRQn_Type IRQn);
void     __svc(SVC_NVIC_CLEAR_PENDING_IRQ)  SVC_NVIC_ClearPendingIRQ(IRQn_Type IRQn);
void     __svc(SVC_NVIC_SET_PRIORITY)       SVC_NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);
uint32_t __svc(SVC_NVIC_GET_PRIORITY)       SVC_NVIC_GetPriority(IRQn_Type IRQn);
void     __svc(SVC_NVIC_SYSTEM_RESET)       SVC_NVIC_SystemReset(void);

#elif defined(__ICCARM__)

#pragma swi_number=SVC_HAL_MPU_DISABLE
 __swi void SVC_MPU_Disable(void);
#pragma swi_number=SVC_DISABLE_ALL_NVIC_IRQS
 __swi uint32_t SVC_SYS_DisableAllNvicIrqs(void);
#pragma swi_number=SVC_RESTORE_NVIC_IRQS
 __swi void  SVC_SYS_RestoreNvicIrqs(uint32_t nvic_iser_state);
#pragma swi_number=SVC_DISABLE_ALL_EXCEPTIONS
 __swi void  SVC_SYS_DisableAllExceptions(void);
#pragma swi_number=SVC_SUBGHZ_INIT
 __swi void  SVC_MX_SUBGHZ_Init(void);

#pragma swi_number=SVC_NVIC_ENABLE_IRQ
 __swi void  SVC_NVIC_EnableIRQ(IRQn_Type IRQn);
#pragma swi_number=SVC_NVIC_GET_ENABLE_IRQ
 __swi uint32_t  SVC_NVIC_GetEnableIRQ(IRQn_Type IRQn); 
#pragma swi_number=SVC_NVIC_DISABLE_IRQ
 __swi void  SVC_NVIC_DisableIRQ(IRQn_Type IRQn);
#pragma swi_number=SVC_NVIC_GET_PENDING_IRQ
 __swi uint32_t  SVC_NVIC_GetPendingIRQ(IRQn_Type IRQn);
#pragma swi_number=SVC_NVIC_SET_PENDING_IRQ
 __swi void  SVC_NVIC_SetPendingIRQ(IRQn_Type IRQn); 
#pragma swi_number=SVC_NVIC_CLEAR_PENDING_IRQ
 __swi void  SVC_NVIC_ClearPendingIRQ(IRQn_Type IRQn);
#pragma swi_number=SVC_NVIC_SET_PRIORITY
 __swi void  SVC_NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);
#pragma swi_number=SVC_NVIC_GET_PRIORITY
 __swi uint32_t  SVC_NVIC_GetPriority(IRQn_Type IRQn);
#pragma swi_number=SVC_NVIC_SYSTEM_RESET
 __swi void  SVC_NVIC_SystemReset(void);

#elif (__ARMCC_VERSION)
void SVC_MPU_Disable(void);
void SVC_SYS_DisableAllNvicIrqs(uint32_t *nvic_iser_state);
void SVC_SYS_RestoreNvicIrqs(uint32_t nvic_iser_state);
void SVC_SYS_DisableAllExceptions(void);
void SVC_MX_SUBGHZ_Init(void);

void SVC_NVIC_EnableIRQ(IRQn_Type IRQn);
void SVC_NVIC_GetEnableIRQ(IRQn_Type IRQn, uint32_t *irqn_state);
void SVC_NVIC_DisableIRQ(IRQn_Type IRQn);
void SVC_NVIC_GetPendingIRQ(IRQn_Type IRQn, uint32_t *irqn_pending_state);
void SVC_NVIC_SetPendingIRQ(IRQn_Type IRQn);
void SVC_NVIC_ClearPendingIRQ(IRQn_Type IRQn);
void SVC_NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);
void SVC_NVIC_GetPriority(IRQn_Type IRQn, uint32_t *irqn_prio);
void SVC_NVIC_SystemReset(void);
#else 
  /*GCC*/
#endif

/**
  * @brief This is the SVC Handler of the CM0 application (the one downloaded via SBSFU).
  * @param args SVC arguments
  * @retval void
  */
void SVC_APP_Handler(uint32_t *args);
/**
  * @brief Maps MSP on Privileged region and set up PSP to current stack pointer 
  * @param void
  * @retval void
  */
void ThumbState_RemapMspAndSwitchToPspStack(void);
/**
  * @brief This is a helper function to enter the unprivileged level for software execution
  * @param void
  * @retval void
  */
void ThumbState_EnterUnprivilegedMode(void);
/**
  * @brief This is a helper function to determine if we are currently running in non-privileged mode or not
  * @param void
  * @retval 0 if we are in privileged mode, 1 if we are in non-privileged mode
  */
uint32_t ThumbState_IsUnprivileged(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __SYS_PRIVILEGED_SERVICES_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
