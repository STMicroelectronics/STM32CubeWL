/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_sigfox.h
  * @author  MCD Application Team
  * @brief   API provided to CM0 application to register and handle Sigfox to MBMUX
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
#ifndef __MBMUXIF_SIGFOX_CM4_H__
#define __MBMUXIF_SIGFOX_CM4_H__

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
  * @retval   0: OK;
             -1: no more ipcc channel available;
             -2: feature not provided by CM0PLUS;
             -3: callback error on CM0PLUS
             -4: mismatch between CM4 and CM0PLUS sigfox versions
  */
int8_t MBMUXIF_SigfoxInit(void);
/**
  * @brief   gives back the pointer to the com buffer associated to Sigfox feature Cmd
  * @return  pointer to the com param buffer
  */
MBMUX_ComParam_t *MBMUXIF_GetSigfoxFeatureCmdComPtr(void);

/**
  * @brief   Sends a Sigfox-Cmd via Ipcc and Wait for the response
  */
void MBMUXIF_SigfoxSendCmd(void);

/**
  * @brief   Sends a Sigfox-Ack  via Ipcc without waiting for the ack
  */
void MBMUXIF_SigfoxSendAck(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__MBMUXIF_SIGFOX_CM4_H__ */
