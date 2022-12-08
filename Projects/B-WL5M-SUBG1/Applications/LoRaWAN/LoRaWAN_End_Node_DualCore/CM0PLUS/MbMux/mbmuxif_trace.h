/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_trace.h
  * @author  MCD Application Team
  * @brief   API for CM0PLUS application to register and handle TRACE via MBMUX
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
#ifndef __MBMUXIF_TRACE_CM0PLUS_H__
#define __MBMUXIF_TRACE_CM0PLUS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mbmux.h"
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
  * @brief   Registers TRACE feature to the mailbox
  * @param   verboseLevel level of trace verbosity
  * @retval  0: OK; -1: if ch hasn't been registered by CM4
  * @note    this function is supposed to be called by the System on request (Cmd) of CM4
  */
int8_t MBMUXIF_TraceInit(uint8_t verboseLevel);

/**
  * @brief   gives back the pointer to the com buffer associated to Trace feature Notif
  * @retval  return pointer to the com param buffer
  */
MBMUX_ComParam_t *MBMUXIF_GetTraceFeatureNotifComPtr(void);

/**
  * @brief Sends a Trace-Notif via Ipcc without waiting for the ack
  */
void MBMUXIF_TraceSendNotif_NoWait(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__MBMUXIF_TRACE_CM0PLUS_H__ */
