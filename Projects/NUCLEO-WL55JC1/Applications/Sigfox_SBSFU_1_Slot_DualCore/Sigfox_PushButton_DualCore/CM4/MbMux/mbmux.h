/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmux.h
  * @author  MCD Application Team
  * @brief   API which interfaces CM4 to IPCC
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
#ifndef __MBMUX_CM4_H__
#define __MBMUX_CM4_H__

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
  * @brief   The application informs MBMUX about the CM0PLUS supported feature list
  * @param   pCM0PLUS_FeatureList pointer to the list of CM0PLUS features
  */
void MBMUX_SetCm0plusFeatureListPtr(FEAT_INFO_List_t *pCM0PLUS_FeatureList);

/**
  * @brief   Assigns an ipcc channel to a feature (for a requested direction) and registers associated application cb and buffer
  * @param   e_featID   identifier of the feature
  * @param   ComType    0 for CMd/Resp (TX), 1 for Notif/Ack (RX)
  * @param   MsgCb      application callback for notification processing
  * @param   ComBuffer  application buffer where Msg values (params) are stored
  * @param   ComBufSize max size allocated by the application for the buffer
  * @return  channel index or -1: (no more ipcc channel available) or -2: feature not provided by CM0PLUS
  */
int8_t MBMUX_RegisterFeature(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType,
                             void (*MsgCb)(void *ComObj), uint32_t *const ComBuffer, uint16_t ComBufSize);

/**
  * @brief   Release an ipcc channel from the given feature and direction
  * @param   e_featID  identifier of the feature
  * @param   ComType  0 for CMd/Resp (TX), 1 for Notif/Ack (RX)
  * @note    this function has not been fully tested, because never required by our application
  */
void MBMUX_UnregisterFeature(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType);

/**
  * @brief   gives back the pointer to the MailBox com buffer associated to the feature
  * @param   e_featID  identifier of the feature
  * @param   ComType   0 for CMd/Resp (TX), 1 for Notif/Ack (RX)
  * @return  pointer to the com param object associated to the feature
  */
MBMUX_ComParam_t *MBMUX_GetFeatureComPtr(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType);

/**
  * @brief   Send Cmd to remote MCU for a requested feature by abstracting the channel idx
  * @param   e_featID  identifier of the feature
  * @retval  OK: 0 , fail: -1
  */
int32_t MBMUX_CommandSnd(FEAT_INFO_IdTypeDef e_featID);

/**
  * @brief   Send ack to remote MCU for a requested feature by abstracting the channel idx
  * @param   e_featID  identifier of the feature
  * @retval  OK: 0 , fail: -1
  */
uint32_t MBMUX_AcknowledgeSnd(FEAT_INFO_IdTypeDef e_featID);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__MBMUX_CM4_H__ */
