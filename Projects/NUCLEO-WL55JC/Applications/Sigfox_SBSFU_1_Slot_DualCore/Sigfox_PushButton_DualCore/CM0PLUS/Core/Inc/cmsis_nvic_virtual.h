/**
  ******************************************************************************
  * @file    cmsis_nvic_virtual.h
  * @brief   Maps the NVIC functions on _SVC for Unpriviledge usage
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
