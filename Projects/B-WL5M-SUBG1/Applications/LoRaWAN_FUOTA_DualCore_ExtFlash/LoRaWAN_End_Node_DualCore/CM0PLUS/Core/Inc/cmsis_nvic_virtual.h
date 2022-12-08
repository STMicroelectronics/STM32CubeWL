/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    cmsis_nvic_virtual.h
  * @brief   Maps the NVIC functions on _SVC for Unprivileged usage
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CMSIS_NVIC_VIRTUAL_H__
#define __CMSIS_NVIC_VIRTUAL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "sys_privileged_wrap.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
#define NVIC_SetPriorityGrouping    __NVIC_SetPriorityGrouping
#define NVIC_GetPriorityGrouping    __NVIC_GetPriorityGrouping
#define NVIC_EnableIRQ              SYS_PRIVIL_NVIC_EnableIRQ
#define NVIC_GetEnableIRQ           SYS_PRIVIL_NVIC_GetEnableIRQ
#define NVIC_DisableIRQ             SYS_PRIVIL_NVIC_DisableIRQ
#define NVIC_GetPendingIRQ          SYS_PRIVIL_NVIC_GetPendingIRQ
#define NVIC_SetPendingIRQ          SYS_PRIVIL_NVIC_SetPendingIRQ
#define NVIC_ClearPendingIRQ        SYS_PRIVIL_NVIC_ClearPendingIRQ
/* #define NVIC_GetActive              __NVIC_GetActive             not available for Cortex-M0+ */
#define NVIC_SetPriority            SYS_PRIVIL_NVIC_SetPriority
#define NVIC_GetPriority            SYS_PRIVIL_NVIC_GetPriority
#define NVIC_SystemReset            SYS_PRIVIL_NVIC_SystemReset

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __CMSIS_NVIC_VIRTUAL_H__ */
