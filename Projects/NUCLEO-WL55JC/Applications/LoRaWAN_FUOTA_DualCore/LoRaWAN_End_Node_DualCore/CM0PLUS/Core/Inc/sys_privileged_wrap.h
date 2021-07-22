/**
  ******************************************************************************
  * @file    sys_privileged_wrap.h
  * @brief   This file contains all the function prototypes for
  *          the sys_privileged_wrap.c file
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
#ifndef __SYS_PRIVILEGE_WRAP_H__
#define __SYS_PRIVILEGE_WRAP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

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

/**
  * @brief If execution in unprivil mode, calls SVC and Disable All Nvic Irqs
  * @param void
  * @retval uint32_t nvic_iser_state
  */
uint32_t SYS_PRIVIL_EnterCriticalSection(void);

/**
  * @brief If execution in unprivil mode, calls SVC and restore NVIC ISER register
  * @param uint32_t nvic_iser_state
  * @retval void
  */
void SYS_PRIVIL_ExitCriticalSection(uint32_t nvic_iser_state);

/**
  * @brief disable all exceptions and after SVC keeps MODE Privileged! (stack goes back to PSP)
  * @param  void
  * @retval void
  */
void SYS_PRIVIL_DisableIrqsAndRemainPriv(void);

/**
  * @brief enable all exceptions and go to Unprivileged mode
  * @param  void
  * @retval void
  */
void SYS_PRIVIL_EnableIrqsAndGoUnpriv(void);


/**
  * @note All SYS_PRIVIL_NVIC_*** functions are a remapping such thatIf
  *       if execution in unprivil mode  call SVC before __NVIC_***
  */
void SYS_PRIVIL_NVIC_EnableIRQ(IRQn_Type IRQn);
uint32_t SYS_PRIVIL_NVIC_GetEnableIRQ(IRQn_Type IRQn);
void SYS_PRIVIL_NVIC_DisableIRQ(IRQn_Type IRQn);
uint32_t SYS_PRIVIL_NVIC_GetPendingIRQ(IRQn_Type IRQn);
void SYS_PRIVIL_NVIC_SetPendingIRQ(IRQn_Type IRQn);
void SYS_PRIVIL_NVIC_ClearPendingIRQ(IRQn_Type IRQn);
void SYS_PRIVIL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);
uint32_t SYS_PRIVIL_NVIC_GetPriority(IRQn_Type IRQn);
void SYS_PRIVIL_NVIC_SystemReset(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __SYS_PRIVILEGE_WRAP_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
