/**
  ******************************************************************************
  * @file    mbmux.c
  * @author  MCD Application Team
  * @brief   Interface CPU2 to IPCC: multiplexer to map features to IPCC channels
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

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "assert.h"
#include "stddef.h"
#include "ipcc_if.h"
#include "mbmux.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define MB_CHANNEL_NOT_REGISTERED 0xFF

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static MBMUX_ComTable_t *p_MBMUX_ComTable;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MBMUX_IsrCommandRcvCb(uint32_t channelIdx);
static void MBMUX_IsrAcknowledgeRcvCb(uint32_t channelIdx);
static uint8_t MBMUX_GetFeatureChIdx(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType);
static void MBMUX_IsrNotRegistered(void *ComObj);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
/**
  * @brief Init the mailbox feature table and init the ipcc
  * @param MBMUX_ComTable_t  Mailbox intra-MCUs communication table
  * @retval  none
  */
void MBMUX_Init(MBMUX_ComTable_t *const pMBMUX_ComTable)
{
  /* USER CODE BEGIN MBMUX_Init_1 */

  /* USER CODE END MBMUX_Init_1 */
  uint8_t i;
  IPCC_IF_Init(MBMUX_IsrCommandRcvCb, MBMUX_IsrAcknowledgeRcvCb);

  p_MBMUX_ComTable = pMBMUX_ComTable;

  for (i = 0; i < MBMUX_CHANNEL_NUMBER; i++)
  {
    p_MBMUX_ComTable->MBCmdRespParam[i].MsgCm0plusCb = MBMUX_IsrNotRegistered;
    p_MBMUX_ComTable->MBNotifAckParam[i].MsgCm0plusCb = MBMUX_IsrNotRegistered;
  }
  /* USER CODE BEGIN MBMUX_Init_Last */

  /* USER CODE END MBMUX_Init_Last */
}

/**
  * @brief Assigns an ipcc channel to a feature (for a requested direction) and registers associated applic cb and buffer
  * @param e_featID  identifier of the feature (Lora, Sigfox, etc).
  * @param ComType  0 for CMd/Resp, 1 for Notif/Ack
  * @param MsgCb   applic callback for notification processing
  * @param ComBuffer   applic buffer where Msg values (params) are stored
  * @param ComBufSize  max size allocated by the applic for the buffer
  * @retval  return -1 if ch hasn't been registered by CM4, otherwise the nr of the assigned ch
  */
int8_t MBMUX_RegisterFeatureCallback(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType, void (*MsgCb)(void *ComObj))
{
  uint8_t channel_idx;
  int8_t ret = -1;
  uint8_t check_existing_feature_registration;
  /* USER CODE BEGIN MBMUX_RegisterFeatureC_1 */

  /* USER CODE END MBMUX_RegisterFeatureC_1 */

  check_existing_feature_registration = MBMUX_GetFeatureChIdx(e_featID, ComType);

  if (check_existing_feature_registration != MB_CHANNEL_NOT_REGISTERED)
  {
    /* channel has been registered by CM4 registered */
    channel_idx = check_existing_feature_registration;
    if (ComType == MBMUX_CMD_RESP)
    {
      p_MBMUX_ComTable->MBCmdRespParam[channel_idx].MsgCm0plusCb = MsgCb;
    }
    else
    {
      p_MBMUX_ComTable->MBNotifAckParam[channel_idx].MsgCm0plusCb = MsgCb;
    }
    ret = channel_idx;
  }
  /* USER CODE BEGIN MBMUX_RegisterFeatureC_Last */

  /* USER CODE END MBMUX_RegisterFeatureC_Last */
  return ret;
}

/**
  * @brief Release an ipcc channel from the given feature and direction
  * @param e_featID  identifier of the feature (Lora, Sigfox, etc).
  * @param ComType  0 for CMd/Resp (TX), 1 for Notif/Ack (RX)
  * @retval  none
  * @note  this function has not been fully tested, because never required by our applic
  */
void MBMUX_UnregisterFeature(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType)
{
  /* USER CODE BEGIN MBMUX_UnregisterFeature_1 */

  /* USER CODE END MBMUX_UnregisterFeature_1 */
  uint8_t mb_ch;

  mb_ch = MBMUX_GetFeatureChIdx(e_featID, ComType);

  if (mb_ch < IPCC_CHANNEL_NUMBER)
  {
    if (ComType == MBMUX_CMD_RESP)  /* TX */
    {
      /* Make sure to clear pending IRQ from Cmd before unregistering the callback */
      IPCC_IF_ResponseSnd(mb_ch);

      p_MBMUX_ComTable->MBCmdRespParam[mb_ch].MsgCm0plusCb = MBMUX_IsrNotRegistered;
    }
    else
    {
      p_MBMUX_ComTable->MBNotifAckParam[mb_ch].MsgCm0plusCb = MBMUX_IsrNotRegistered;
    }
  }
  /* USER CODE BEGIN MBMUX_UnregisterFeature_Last */

  /* USER CODE END MBMUX_UnregisterFeature_Last */
}

/**
  * @brief gives back the pointer to the com buffer associated to the feature
  * @param e_featID  identifier of the feature (Lora, Sigfox, etc).
  * @param ComType  0 for CMd/Resp (TX), 1 for Notif/Ack (RX)
  * @retval  return pointer to the com param buffer
  */
MBMUX_ComParam_t *MBMUX_GetFeatureComPtr(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType)
{
  /* USER CODE BEGIN MBMUX_GetFeatureComPtr_1 */

  /* USER CODE END MBMUX_GetFeatureComPtr_1 */
  uint32_t channel_idx;
  MBMUX_ComParam_t *com_param_ptr = NULL;

  channel_idx = MBMUX_GetFeatureChIdx(e_featID, ComType);
  if (channel_idx < MBMUX_CHANNEL_NUMBER)
  {
    if (ComType == MBMUX_CMD_RESP)  /* TX */
    {
      com_param_ptr = (MBMUX_ComParam_t *) &p_MBMUX_ComTable->MBCmdRespParam[channel_idx];
    }
    else
    {
      com_param_ptr = (MBMUX_ComParam_t *) &p_MBMUX_ComTable->MBNotifAckParam[channel_idx];
    }
  }
  return com_param_ptr;
  /* USER CODE BEGIN MBMUX_GetFeatureComPtr_Last */

  /* USER CODE END MBMUX_GetFeatureComPtr_Last */
}

/**
  * @brief Send Notif to remote MCU for a requested feature by abstracting the channel idx
  * @param e_featID  identifier of the feature (Lora, Sigfox, etc).
  * @retval OK: 0 , fail: -1
  */
int32_t MBMUX_NotificationSnd(FEAT_INFO_IdTypeDef e_featID)
{
  /* USER CODE BEGIN MBMUX_NotificationSnd_1 */

  /* USER CODE END MBMUX_NotificationSnd_1 */
  uint32_t mb_ch;

  mb_ch = MBMUX_GetFeatureChIdx(e_featID, MBMUX_NOTIF_ACK);
  if (p_MBMUX_ComTable->MBNotifAckParam[mb_ch].ParamCnt > p_MBMUX_ComTable->MBNotifAckParam[mb_ch].BufSize)
  {
    while (1) {}
  }
  return IPCC_IF_NotificationSnd(mb_ch);
  /* USER CODE BEGIN MBMUX_NotificationSnd_Last */

  /* USER CODE END MBMUX_NotificationSnd_Last */
}

/**
  * @brief Send response to remote MCU for a requested feature by abstracting the channel idx
  * @param e_featID  identifier of the feature (Lora, Sigfox, etc).
  * @retval OK: 0 , fail: -1
  */
uint32_t MBMUX_ResponseSnd(FEAT_INFO_IdTypeDef e_featID)
{
  /* USER CODE BEGIN MBMUX_ResponseSnd_1 */

  /* USER CODE END MBMUX_ResponseSnd_1 */
  uint32_t mb_ch;

  mb_ch = MBMUX_GetFeatureChIdx(e_featID, MBMUX_CMD_RESP);
  return IPCC_IF_ResponseSnd(mb_ch);
  /* USER CODE BEGIN MBMUX_ResponseSnd_Last */

  /* USER CODE END MBMUX_ResponseSnd_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief Isr executed when Ipcc receive IRQ notification: forwards to upper layer
  * @param channelIdx  ipcc channel number
  * @retval none
  */
static void MBMUX_IsrCommandRcvCb(uint32_t channelIdx)
{
  /* USER CODE BEGIN MBMUX_IsrCommandRcvCb_1 */

  /* USER CODE END MBMUX_IsrCommandRcvCb_1 */
  /* retrieve pointer to com params */
  void *com_obj = (void *) &p_MBMUX_ComTable->MBCmdRespParam[channelIdx];
  /* call registered callback */
  p_MBMUX_ComTable->MBCmdRespParam[channelIdx].MsgCm0plusCb(com_obj);
  /* USER CODE BEGIN MBMUX_IsrCommandRcvCb_Last */

  /* USER CODE END MBMUX_IsrCommandRcvCb_Last */
  return;
}

/**
  * @brief Isr executed when Ipcc receive IRQ notification: forwards to upper layer
  * @param channelIdx  ipcc channel number
  * @retval none
  */
static void MBMUX_IsrAcknowledgeRcvCb(uint32_t channelIdx)
{
  /* USER CODE BEGIN MBMUX_IsrAcknowledgeRcvCb_1 */

  /* USER CODE END MBMUX_IsrAcknowledgeRcvCb_1 */
  /* retrieve pointer to com params */
  void *com_obj = (void *) &p_MBMUX_ComTable->MBNotifAckParam[channelIdx];
  /* call registered callback */
  p_MBMUX_ComTable->MBNotifAckParam[channelIdx].MsgCm0plusCb(com_obj);
  /* USER CODE BEGIN MBMUX_IsrAcknowledgeRcvCb_Last */

  /* USER CODE END MBMUX_IsrAcknowledgeRcvCb_Last */
  return;
}

/**
  * @brief gives back channel associated to the feature
  * @param e_featID  identifier of the feature (Lora, Sigfox, etc).
  * @param ComType  0 for CMd/Resp (TX), 1 for Notif/Ack (RX)
  * @retval ipcc channel number: if 0xFF means the feature isn't registered
  */
static uint8_t MBMUX_GetFeatureChIdx(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType)
{
  /* USER CODE BEGIN MBMUX_GetFeatureChIdx_1 */

  /* USER CODE END MBMUX_GetFeatureChIdx_1 */
  return p_MBMUX_ComTable->MBMUXMapping[e_featID][ComType];
  /* USER CODE BEGIN MBMUX_GetFeatureChIdx_Last */

  /* USER CODE END MBMUX_GetFeatureChIdx_Last */
}

/**
  * @brief To facilitate the debug in case a function is not registered
  * @param ComType  Dummy
  * @retval None
  */
static void MBMUX_IsrNotRegistered(void *ComObj)
{
  /* USER CODE BEGIN MBMUX_IsrNotRegistered_1 */

  /* USER CODE END MBMUX_IsrNotRegistered_1 */
  while(1);
  /* USER CODE BEGIN MBMUX_IsrNotRegistered_Last */

  /* USER CODE END MBMUX_IsrNotRegistered_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
