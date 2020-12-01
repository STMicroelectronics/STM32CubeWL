/**
  ******************************************************************************
  * @file    mbmux.c
  * @author  MCD Application Team
  * @brief   CM4 side multiplexer to map features to IPCC channels
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
static FEAT_INFO_List_t *p_MBMUX_Cm0plusFeatureList = NULL;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static uint8_t MBMUX_CheckIfFeatureSupportedByCm0plus(FEAT_INFO_IdTypeDef e_featID);
static void MBMUX_IsrResponseRcvCb(uint32_t channelIdx);
static void MBMUX_IsrNotificationRcvCb(uint32_t channelIdx);
static uint8_t MBMUX_FindChStillUnregistered(MBMUX_ComType_t ComType);
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
  IPCC_IF_Init(MBMUX_IsrResponseRcvCb, MBMUX_IsrNotificationRcvCb);

  p_MBMUX_ComTable = pMBMUX_ComTable;

  for (i = 0; i < FEAT_INFO_CNT; i++)
  {
    p_MBMUX_ComTable->MBMUXMapping[i][MBMUX_CMD_RESP] = MB_CHANNEL_NOT_REGISTERED;
    p_MBMUX_ComTable->MBMUXMapping[i][MBMUX_NOTIF_ACK] = MB_CHANNEL_NOT_REGISTERED;
  }

  for (i = 0; i < MBMUX_CHANNEL_NUMBER; i++)
  {
    p_MBMUX_ComTable->MBCmdRespParam[i].MsgId = 0;
    p_MBMUX_ComTable->MBCmdRespParam[i].MsgCm4Cb = MBMUX_IsrNotRegistered;
    p_MBMUX_ComTable->MBCmdRespParam[i].ParamCnt = 0;
    p_MBMUX_ComTable->MBCmdRespParam[i].ParamBuf = NULL;
    p_MBMUX_ComTable->MBNotifAckParam[i].MsgId = 0;
    p_MBMUX_ComTable->MBNotifAckParam[i].MsgCm4Cb = MBMUX_IsrNotRegistered;
    p_MBMUX_ComTable->MBNotifAckParam[i].ParamCnt = 0;
    p_MBMUX_ComTable->MBNotifAckParam[i].ParamBuf = NULL;
  }
  /* USER CODE BEGIN MBMUX_Init_Last */

  /* USER CODE END MBMUX_Init_Last */
}

/**
  * @brief The application informs MBMUX about the CM0PLUS supported feature list
  * @param p_CM0PLUS_FeatureList  pointer to the list of CM0PLUS features
  * @retval  none
  */
void MBMUX_SetCm0plusFeatureListPtr(FEAT_INFO_List_t *pCM0PLUS_FeatureList)
{
  /* USER CODE BEGIN MBMUX_SetCm0plusFeatureListPtr_1 */

  /* USER CODE END MBMUX_SetCm0plusFeatureListPtr_1 */
  p_MBMUX_Cm0plusFeatureList = pCM0PLUS_FeatureList;
  /* USER CODE BEGIN MBMUX_SetCm0plusFeatureListPtr_Last */

  /* USER CODE END MBMUX_SetCm0plusFeatureListPtr_Last */
}

/**
  * @brief Assigns an ipcc channel to a feature (for a requested direction) and registers associated applic cb and buffer
  * @param e_featID  identifier of the feature (Lora, Sigfox, etc).
  * @param ComType  0 for CMd/Resp, 1 for Notif/Ack
  * @param MsgCb   applic callback for notification processing
  * @param ComBuffer   applic buffer where Msg values (params) are stored
  * @param ComBufSize  max size allocated by the applic for the buffer
  * @retval   channel index or -1: (no more ipcc channel available) or -2: feature not provided by CM0PLUS
  */
int8_t MBMUX_RegisterFeature(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType, void (*MsgCb)(void *ComObj), uint32_t *const ComBuffer, uint16_t ComBufSize)
{
  int8_t check_if_feature_provided_by_cm0plus = 0;
  uint8_t channel_idx = MB_CHANNEL_NOT_REGISTERED;
  int8_t ret = -1; /* no more ipcc channels available */
  uint8_t check_existing_feature_registration;
  /* USER CODE BEGIN MBMUX_RegisterFeatureC_1 */

  /* USER CODE END MBMUX_RegisterFeatureC_1 */

  if (e_featID == FEAT_INFO_SYSTEM_ID)
  {
    check_if_feature_provided_by_cm0plus = 1;
  }
  else
  {
    check_if_feature_provided_by_cm0plus = MBMUX_CheckIfFeatureSupportedByCm0plus(e_featID);
  }

  if (check_if_feature_provided_by_cm0plus == 1)
  {
    check_existing_feature_registration = MBMUX_GetFeatureChIdx(e_featID, ComType);

    if (check_existing_feature_registration == MB_CHANNEL_NOT_REGISTERED)
    {
      if (e_featID == FEAT_INFO_SYSTEM_ID)
      {
        channel_idx = 0;
      }
      else
      {
        channel_idx = MBMUX_FindChStillUnregistered(ComType);
      }
    }
    else
    {
      /* previous registration will be replaced by the given buffers */
      channel_idx = check_existing_feature_registration;
    }
  }
  else
  {
    ret = -2; /* feature not provided by CM0PLUS */
  }

  if (channel_idx != MB_CHANNEL_NOT_REGISTERED)
  {
    if (ComType == MBMUX_CMD_RESP)
    {
      p_MBMUX_ComTable->MBMUXMapping[e_featID][MBMUX_CMD_RESP] = channel_idx;
      p_MBMUX_ComTable->MBCmdRespParam[channel_idx].MsgCm4Cb = MsgCb;
      p_MBMUX_ComTable->MBCmdRespParam[channel_idx].BufSize = ComBufSize;
      p_MBMUX_ComTable->MBCmdRespParam[channel_idx].ParamBuf = ComBuffer;
      p_MBMUX_ComTable->MBCmdRespParam[channel_idx].ReturnVal = 0;
    }
    else
    {
      p_MBMUX_ComTable->MBMUXMapping[e_featID][MBMUX_NOTIF_ACK] = channel_idx;
      p_MBMUX_ComTable->MBNotifAckParam[channel_idx].MsgCm4Cb = MsgCb;
      p_MBMUX_ComTable->MBNotifAckParam[channel_idx].BufSize = ComBufSize;
      p_MBMUX_ComTable->MBNotifAckParam[channel_idx].ParamBuf = ComBuffer;
      p_MBMUX_ComTable->MBNotifAckParam[channel_idx].ReturnVal = 0;
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

  /* Function not completely implemented:
     Missing code that sends via Mbmux a SysCmd to tell CM0PLUS to unregister it's callback */
  /* ... */

  if (mb_ch < MBMUX_CHANNEL_NUMBER)
  {
    if (ComType == MBMUX_CMD_RESP)
    {
      /* Channel should have been unregistered first on CM0PLUS to avoid getting Irq with no cb */
      p_MBMUX_ComTable->MBCmdRespParam[mb_ch].MsgCm4Cb = MBMUX_IsrNotRegistered;
      p_MBMUX_ComTable->MBCmdRespParam[mb_ch].BufSize = 0;
      p_MBMUX_ComTable->MBCmdRespParam[mb_ch].ParamBuf = NULL;
      p_MBMUX_ComTable->MBCmdRespParam[mb_ch].ReturnVal = 0;
    }
    else
    {
      /* Make sure to clear pending IRQ Notification before unregistering the callback */
      /* necessary because the HAL_cb only mask the register, the SR is cleaned by the task */
      /* if the mask is removed by anyone, the unwhished IRQ is generated if SR isn't cleaned */
      IPCC_IF_AcknowledgeSnd(mb_ch);

      p_MBMUX_ComTable->MBNotifAckParam[mb_ch].MsgCm4Cb = MBMUX_IsrNotRegistered;
      p_MBMUX_ComTable->MBNotifAckParam[mb_ch].BufSize = 0;
      p_MBMUX_ComTable->MBNotifAckParam[mb_ch].ParamBuf = NULL;
      p_MBMUX_ComTable->MBNotifAckParam[mb_ch].ReturnVal = 0;
    }
  }

  p_MBMUX_ComTable->MBMUXMapping[e_featID][ComType] = MB_CHANNEL_NOT_REGISTERED;

  /* USER CODE BEGIN MBMUX_UnregisterFeature_Last */

  /* USER CODE END MBMUX_UnregisterFeature_Last */
}

/**
  * @brief gives back the pointer to the MailBox com buffer associated to the feature
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
  * @brief Send Cmd to remote MCU for a requested feature by abstracting the channel idx
  * @param e_featID  identifier of the feature
  * @retval OK: 0 , fail: -1
  */
int32_t MBMUX_CommandSnd(FEAT_INFO_IdTypeDef e_featID)
{
  /* USER CODE BEGIN MBMUX_CommandSnd_1 */

  /* USER CODE END MBMUX_CommandSnd_1 */
  uint32_t mb_ch;

  mb_ch = MBMUX_GetFeatureChIdx(e_featID, MBMUX_CMD_RESP);
  if (p_MBMUX_ComTable->MBCmdRespParam[mb_ch].ParamCnt > p_MBMUX_ComTable->MBCmdRespParam[mb_ch].BufSize)
  {
    Error_Handler();
  }
  return IPCC_IF_CommandSnd(mb_ch);
  /* USER CODE BEGIN MBMUX_CommandSnd_Last */

  /* USER CODE END MBMUX_CommandSnd_Last */
}

/**
  * @brief Send ack to remote MCU for a requested feature by abstracting the channel idx
  * @param e_featID  identifier of the feature
  * @retval OK: 0 , fail: -1
  */
uint32_t MBMUX_AcknowledgeSnd(FEAT_INFO_IdTypeDef e_featID)
{
  /* USER CODE BEGIN MBMUX_AcknowledgeSnd_1 */

  /* USER CODE END MBMUX_AcknowledgeSnd_1 */
  uint32_t mb_ch;

  mb_ch = MBMUX_GetFeatureChIdx(e_featID, MBMUX_NOTIF_ACK);
  return IPCC_IF_AcknowledgeSnd(mb_ch);
  /* USER CODE BEGIN MBMUX_AcknowledgeSnd_Last */

  /* USER CODE END MBMUX_AcknowledgeSnd_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief Check if the FEATURE is in the CM0 table and return it's position
  * @param e_featID  identifier of the feature
  * @retval 0 not present, 1 present
  */
static uint8_t MBMUX_CheckIfFeatureSupportedByCm0plus(FEAT_INFO_IdTypeDef e_featID)
{
  int8_t ret = 0;
  uint8_t i;
  uint8_t cm0plus_nr_of_supported_features;
  FEAT_INFO_Param_t  i_feature;
  /* USER CODE BEGIN MBMUX_CheckIfFeatureSupportedByCm0plus_1 */

  /* USER CODE END MBMUX_CheckIfFeatureSupportedByCm0plus_1 */

  if (p_MBMUX_Cm0plusFeatureList != NULL)
  {
    cm0plus_nr_of_supported_features = p_MBMUX_Cm0plusFeatureList->Feat_Info_Cnt;

    for (i = 0; i < cm0plus_nr_of_supported_features;  i++)
    {
      i_feature = *(i + p_MBMUX_Cm0plusFeatureList->Feat_Info_TableAddress);
      if (i_feature.Feat_Info_Feature_Id == e_featID)
      {
        ret = 1;
        break;
      }

    }
  }
  /* USER CODE BEGIN MBMUX_CheckIfFeatureSupportedByCm0plus_Last */

  /* USER CODE END MBMUX_CheckIfFeatureSupportedByCm0plus_Last */
  return ret;
}

/**
  * @brief Isr executed when Ipcc receive IRQ notification: forwards to upper layer
  * @param channelIdx  ipcc channel number
  * @retval none
  */
static void MBMUX_IsrResponseRcvCb(uint32_t channelIdx)
{
  /* USER CODE BEGIN MBMUX_IsrResponseRcvCb_1 */

  /* USER CODE END MBMUX_IsrResponseRcvCb_1 */
  /* retrieve pointer to com params */
  void *com_obj = (void *) &p_MBMUX_ComTable->MBCmdRespParam[channelIdx];
  /* call registered callback */
  p_MBMUX_ComTable->MBCmdRespParam[channelIdx].MsgCm4Cb(com_obj);
  /* USER CODE BEGIN MBMUX_IsrResponseRcvCb_Last */

  /* USER CODE END MBMUX_IsrResponseRcvCb_Last */
  return;
}

/**
  * @brief Isr executed when Ipcc receive IRQ notification: forwards to upper layer
  * @param channelIdx  ipcc channel number
  * @retval none
  */
static void MBMUX_IsrNotificationRcvCb(uint32_t channelIdx)
{
  /* USER CODE BEGIN MBMUX_IsrNotificationRcvCb_1 */

  /* USER CODE END MBMUX_IsrNotificationRcvCb_1 */
  /* retrieve pointer to com params */
  void *com_obj = (void *) &p_MBMUX_ComTable->MBNotifAckParam[channelIdx];
  /* call registered callback */
  p_MBMUX_ComTable->MBNotifAckParam[channelIdx].MsgCm4Cb(com_obj);
  /* USER CODE BEGIN MBMUX_IsrNotificationRcvCb_Last */

  /* USER CODE END MBMUX_IsrNotificationRcvCb_Last */
  return;
}

/**
  * @brief Find the first channel not yet associated to a feature (available)
  * @param ComType  0 for CMd/Resp (TX), 1 for Notif/Ack (RX)
  * @retval ipcc channel number: if 0xFF means no channel available
  */
static uint8_t MBMUX_FindChStillUnregistered(MBMUX_ComType_t ComType)
{
  /* USER CODE BEGIN MBMUX_FindChStillUnregistered_1 */

  /* USER CODE END MBMUX_FindChStillUnregistered_1 */
  uint8_t feat_id;
  uint8_t ch;
  uint8_t already_used;

  /* First two channels are reserved to SYSTEM, TRACE and SKS */
  if (ComType == MBMUX_CMD_RESP)
  {
    for (ch = 1; ch < MBMUX_CHANNEL_NUMBER; ch++)
    {
      already_used = 0;
      for (feat_id = 0; feat_id < FEAT_INFO_CNT; feat_id++)
      {
        if (p_MBMUX_ComTable->MBMUXMapping[feat_id][MBMUX_CMD_RESP] == ch)
        {
          already_used = 1;
        }
      }
      if (already_used == 0)
      {
        break;
      }
    }
  }
  else  /* RX */
  {
    for (ch = 1; ch < MBMUX_CHANNEL_NUMBER; ch++)
    {
      already_used = 0;
      for (feat_id = 0; feat_id < FEAT_INFO_CNT; feat_id++)
      {
        if (p_MBMUX_ComTable->MBMUXMapping[feat_id][MBMUX_NOTIF_ACK] == ch)
        {
          already_used = 1;
        }
      }
      if (already_used == 0)
      {
        break;
      }
    }
  }

  if (ch == MBMUX_CHANNEL_NUMBER)
  {
    return MB_CHANNEL_NOT_REGISTERED;
  }
  else
  {
    return (ch);
  }
  /* USER CODE BEGIN MBMUX_FindChStillUnregistered_Last */

  /* USER CODE END MBMUX_FindChStillUnregistered_Last */
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
