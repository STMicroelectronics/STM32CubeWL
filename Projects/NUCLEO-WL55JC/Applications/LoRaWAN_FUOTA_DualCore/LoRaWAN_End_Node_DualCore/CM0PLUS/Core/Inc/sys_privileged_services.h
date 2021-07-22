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
/* Critical section functions group */
#define SVC_DISABLE_ALL_NVIC_IRQS           0x10
#define SVC_RESTORE_NVIC_IRQS               0x11
#define SVC_DISABLE_ALL_EXCEPTIONS          0x12
/* Native NVIC functions group */
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

/**
  * @brief This is the main SVC Handler of the CM0 application (the one from vector table).
  * @param args SVC arguments
  * @retval void
  */
void SVC_APP_Handler(uint32_t *args);

/**
  * @brief This functions triggers a CRITICAL_SECTION system call (supervisor call): request privileged operation
  * @param syscall The identifier of the operation to be called
  * @param arguments arguments of the privileged operation
  * @retval void
  */
void SYS_CRITICALSECTION_SvcCall(uint32_t *ret, uint32_t syscall, ...);

/**
  * @brief This functions triggers a NVIC system call (supervisor call): request privileged operation
  * @param syscall The identifier of the operation to be called
  * @param arguments arguments of the privileged operation
  * @retval void
  */
void SYS_NVIC_SvcCall(uint32_t *ret, uint32_t syscall, ...);



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
