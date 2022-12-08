/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmux.h
  * @author  MCD Application Team
  * @brief   API which interfaces CM0PLUS to IPCC
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
#ifndef __MBMUX_CM0PLUS_H__
#define __MBMUX_CM0PLUS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "mbmux_table.h"
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
  * @brief   Init the mailbox feature table and init the ipcc
  * @param   pMBMUX_ComTable Mailbox intra-MCUs communication table
  */
void MBMUX_Init(MBMUX_ComTable_t *const pMBMUX_ComTable);

/**
  * @brief   Assigns an ipcc channel to a feature (for a requested direction) and registers associated application cb and buffer
  * @param   e_featID   identifier of the feature
  * @param   ComType    0 for CMd/Resp (TX), 1 for Notif/Ack (RX)
  * @param   MsgCb      application callback for notification processing
  * @retval  -1 if ch hasn't been registered by CM4, otherwise the nr of the assigned ch
  */
int8_t MBMUX_RegisterFeatureCallback(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType,
                                     void (*MsgCb)(void *ComObj));

/**
  * @brief   Release an ipcc channel from the given feature and direction
  * @param   e_featID  identifier of the feature
  * @param   ComType  0 for CMd/Resp (TX), 1 for Notif/Ack (RX)
  * @note    this function has not been fully tested, because never required by our application
  */
void MBMUX_UnregisterFeature(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType);

/**
  * @brief   gives back the pointer to the com object retrieved from shared memory
  * @param   e_featID  identifier of the feature
  * @param   ComType   0 for CMd/Resp (TX), 1 for Notif/Ack (RX)
  * @return  pointer to the com param object associated to the feature
  */
MBMUX_ComParam_t *MBMUX_GetFeatureComPtr(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType);

/**
  * @brief   Send Notif to remote MCU for a requested feature by abstracting the channel idx
  * @param   e_featID  identifier of the feature
  * @retval  OK: 0 , fail: -1
  */
int32_t MBMUX_NotificationSnd(FEAT_INFO_IdTypeDef e_featID);

/**
  * @brief   Send response to remote MCU for a requested feature by abstracting the channel idx
  * @param   e_featID  identifier of the feature
  * @retval  OK: 0 , fail: -1
  */
uint32_t MBMUX_ResponseSnd(FEAT_INFO_IdTypeDef e_featID);

/**
  * @brief   Double check buffer pointer (to avoid fault HW injection attacks)
  * @param   pBufferAddress buffer start address
  * @param   bufferSize buffer size
  * @return  pointer to validated address
  */
uint32_t *MBMUX_SEC_VerifySramBufferPtr(uint32_t *pBufferAddress, uint32_t bufferSize);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__MBMUX_CM0PLUS_H__ */
