/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_sigfox.h
  * @author  MCD Application Team
  * @brief   API provided to CM0 application to register and handle Sigfox to MBMUX
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MBMUXIF_SIGFOX_CM0PLUS_H__
#define __MBMUXIF_SIGFOX_CM0PLUS_H__

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
  * @brief   Registers Sigfox feature to the mailbox and to the sequencer
  * @retval  0: OK; -1: no more ipcc channel available; -2: feature not provided by CM0PLUS; -3: callback error on CM0PLUS
  */
int8_t MBMUXIF_SigfoxInit(void);
/**
  * @brief   gives back the pointer to the com buffer associated to Sigfox feature Notif
  * @return  pointer to the com param buffer
  */
MBMUX_ComParam_t *MBMUXIF_GetSigfoxFeatureNotifComPtr(void);

/**
  * @brief   Set a Task to prevent being blocked on ISR
  */
void MBMUXIF_SigfoxSendNotif(void);

/**
  * @brief   Sends a Sigfox-Notif via Ipcc and Wait for the Ack
  */
void MBMUXIF_SigfoxSendNotifTask(void);

/**
  * @brief   Sends a Sigfox-Resp  via Ipcc without waiting for the response
  */
void MBMUXIF_SigfoxSendResp(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__MBMUXIF_SIGFOX_CM0PLUS_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
