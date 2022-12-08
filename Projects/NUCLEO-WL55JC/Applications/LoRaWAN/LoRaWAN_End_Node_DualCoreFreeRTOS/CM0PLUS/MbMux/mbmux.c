/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmux.c
  * @author  MCD Application Team
  * @brief   Interface CPU2 to IPCC: multiplexer to map features to IPCC channels
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

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "assert.h"
#include "stddef.h"
#include "stm32_mem.h"
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
static MBMUX_MsgCbPointersTab_t MBMUX_MsgCbPointersTabCm0 UTIL_MEM_ALIGN(4);
static FLASH_OBProgramInitTypeDef OptionsBytesStruct;
static uint32_t unsecure_sram1_end;
static uint32_t unsecure_sram2_end;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief   Isr executed when Ipcc receive IRQ notification: forwards to upper layer
  * @param   channelIdx  ipcc channel number
  */
static void MBMUX_IsrCommandRcvCb(uint32_t channelIdx);

/**
  * @brief   Isr executed when Ipcc receive IRQ notification: forwards to upper layer
  * @param   channelIdx  ipcc channel number
  */
static void MBMUX_IsrAcknowledgeRcvCb(uint32_t channelIdx);

/**
  * @brief   Retrieve Sram Security Configuration from AB and store in static global var
  */
static void MBMUX_RetrieveSecureSramConfig(void);

/**
  * @brief   verify if buffer is in the allowed SRAM range otherwise ErrorHandler
  * @param   pBufferAddress buffer start address
  * @param   bufferSize  buffer size
  * @retval  pointer to validated address
  */
static uint32_t *MBMUX_SEC_VerifySramBuffer(uint32_t *pBufferAddress, uint32_t bufferSize);

/**
  * @brief   gives back channel associated to the feature
  * @param   e_featID  identifier of the feature (Lora, Sigfox, etc).
  * @param   ComType  0 for CMd/Resp (TX), 1 for Notif/Ack (RX)
  * @retval  ipcc channel number: if 0xFF means the feature isn't registered
  */
static uint8_t MBMUX_GetFeatureChIdx(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType);

/**
  * @brief   To facilitate the debug in case a function is not registered
  * @param   ComObj Dummy
  */
static void MBMUX_IsrNotRegistered(void *ComObj);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
void MBMUX_Init(MBMUX_ComTable_t *const pMBMUX_ComTable)
{
  /* USER CODE BEGIN MBMUX_Init_1 */

  /* USER CODE END MBMUX_Init_1 */
  uint8_t i;
  IPCC_IF_Init(MBMUX_IsrCommandRcvCb, MBMUX_IsrAcknowledgeRcvCb);

  /* Retrieve SRAM  Security Config from Option Bytes */
  MBMUX_RetrieveSecureSramConfig();
  /* Validate pMBMUX_ComTable address range */
  p_MBMUX_ComTable = (MBMUX_ComTable_t *) MBMUX_SEC_VerifySramBufferPtr((uint32_t *) pMBMUX_ComTable, sizeof(MBMUX_ComTable_t));

  for (i = 0; i < MBMUX_CHANNEL_NUMBER; i++)
  {
    MBMUX_MsgCbPointersTabCm0.MBCmdRespCb[i].MsgCm0plusCb = MBMUX_IsrNotRegistered;
    MBMUX_MsgCbPointersTabCm0.MBNotifAckCb[i].MsgCm0plusCb = MBMUX_IsrNotRegistered;
    p_MBMUX_ComTable->MBCmdRespParam[i].MsgCm0plusCb = MBMUX_IsrNotRegistered; /* not used anymore */
    p_MBMUX_ComTable->MBNotifAckParam[i].MsgCm0plusCb = MBMUX_IsrNotRegistered; /* not used anymore */
  }
  /* USER CODE BEGIN MBMUX_Init_Last */

  /* USER CODE END MBMUX_Init_Last */
}

int8_t MBMUX_RegisterFeatureCallback(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType,
                                     void (*MsgCb)(void *ComObj))
{
  uint8_t channel_idx;
  int8_t ret = -1;
  uint8_t check_existing_feature_registration;

  /* USER CODE BEGIN MBMUX_RegisterFeatureCallback_1 */

  /* USER CODE END MBMUX_RegisterFeatureCallback_1 */
  check_existing_feature_registration = MBMUX_GetFeatureChIdx(e_featID, ComType);

  if (check_existing_feature_registration != MB_CHANNEL_NOT_REGISTERED)
  {
    /* channel has been registered by CM4 registered */
    channel_idx = check_existing_feature_registration;

    if (channel_idx != MB_CHANNEL_NOT_REGISTERED)
    {
      if (ComType == MBMUX_CMD_RESP)
      {
        MBMUX_MsgCbPointersTabCm0.MBCmdRespCb[channel_idx].MsgCm0plusCb = MsgCb;
      }
      else
      {
        MBMUX_MsgCbPointersTabCm0.MBNotifAckCb[channel_idx].MsgCm0plusCb = MsgCb;
      }
      ret = channel_idx;
    }
  }
  /* USER CODE BEGIN MBMUX_RegisterFeatureCallback_Last */

  /* USER CODE END MBMUX_RegisterFeatureCallback_Last */
  return ret;
}

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

      MBMUX_MsgCbPointersTabCm0.MBCmdRespCb[mb_ch].MsgCm0plusCb = MBMUX_IsrNotRegistered;
    }
    else
    {
      MBMUX_MsgCbPointersTabCm0.MBNotifAckCb[mb_ch].MsgCm0plusCb = MBMUX_IsrNotRegistered;
    }
  }
  /* USER CODE BEGIN MBMUX_UnregisterFeature_Last */

  /* USER CODE END MBMUX_UnregisterFeature_Last */
}

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

int32_t MBMUX_NotificationSnd(FEAT_INFO_IdTypeDef e_featID)
{
  /* USER CODE BEGIN MBMUX_NotificationSnd_1 */

  /* USER CODE END MBMUX_NotificationSnd_1 */
  uint32_t mb_ch;

  mb_ch = MBMUX_GetFeatureChIdx(e_featID, MBMUX_NOTIF_ACK);
  if (p_MBMUX_ComTable->MBNotifAckParam[mb_ch].ParamCnt > p_MBMUX_ComTable->MBNotifAckParam[mb_ch].BufSize)
  {
    Error_Handler();
  }
  return IPCC_IF_NotificationSnd(mb_ch);
  /* USER CODE BEGIN MBMUX_NotificationSnd_Last */

  /* USER CODE END MBMUX_NotificationSnd_Last */
}

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

uint32_t *MBMUX_SEC_VerifySramBufferPtr(uint32_t *pBufferAddress, uint32_t bufferSize)
{
  uint32_t *pbuf_validated = NULL;

  /* USER CODE BEGIN MBMUX_SEC_VerifySramBufferPtr_1 */

  /* USER CODE END MBMUX_SEC_VerifySramBufferPtr_1 */

  pbuf_validated = MBMUX_SEC_VerifySramBuffer(pBufferAddress, bufferSize);

  /* USER CODE BEGIN MBMUX_SEC_VerifySramBufferPtr_Last */

  /* USER CODE END MBMUX_SEC_VerifySramBufferPtr_Last */
  return (pbuf_validated);
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/
static uint32_t *MBMUX_SEC_VerifySramBuffer(uint32_t *pBufferAddress, uint32_t bufferSize)
{
  uint32_t *p_validated_address = NULL;
  /* USER CODE BEGIN MBMUX_SEC_VerifySramBuffer_1 */

  /* USER CODE END MBMUX_SEC_VerifySramBuffer_1 */

  /* Check the global security is enabled */
  if ((OptionsBytesStruct.SecureMode & OB_SECURE_SYSTEM_AND_FLASH_ENABLE) != 0)
  {
    /** The security is enabled */

    if (((((uint32_t)(pBufferAddress)) >= SRAM1_BASE)
         && ((((uint32_t)(pBufferAddress)) + bufferSize) <= unsecure_sram1_end)) ||
        ((((uint32_t)(pBufferAddress)) >= SRAM2_BASE) && ((((uint32_t)(pBufferAddress)) + bufferSize) <= unsecure_sram2_end))
       )
    {
      /* check against (pBufferAddress + bufferSize) overflow */
      if (((uint32_t)pBufferAddress <= unsecure_sram2_end) && (bufferSize < (SRAM1_SIZE + SRAM2_SIZE)))
      {
        /** The address is validated because part of the unsecure SRAM  */
        p_validated_address = pBufferAddress;
      }
      else
      {
        /**
          * The address is outside the unsecure SRAM
          * This is considered as a security attack
          * Hold everything to avoid any security issue
          */
        Error_Handler();
      }
    }
    else
    {
      /**
        * The address is outside the unsecure SRAM
        * This is considered as a security attack
        * Hold everything to avoid any security issue
        */
      Error_Handler();
    }
  }
  else
  {
    /** The security is disabled */
    p_validated_address = pBufferAddress;
  }

  /* USER CODE BEGIN MBMUX_SEC_VerifySramBuffer_Last */

  /* USER CODE END MBMUX_SEC_VerifySramBuffer_Last */
  return (p_validated_address);
}

static void MBMUX_RetrieveSecureSramConfig(void)
{
  /* USER CODE BEGIN MBMUX_RetrieveSecureSramConfig_1 */

  /* USER CODE END MBMUX_RetrieveSecureSramConfig_1 */
  /* Get the Dual boot configuration status */
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

  /* Check the SRAM1 security */
  if ((OptionsBytesStruct.SecureMode & OB_SECURE_SRAM1_DISABLE) != 0)
  {
    /** SRAM1 is not secure */
    unsecure_sram1_end = SRAM1_BASE + SRAM1_SIZE;
  }
  else
  {
    /** part of SRAM1 is secure */
    unsecure_sram1_end = OptionsBytesStruct.SecureSRAM1StartAddr;
  }

  /** Check the SRAM2 security */
  if ((OptionsBytesStruct.SecureMode & OB_SECURE_SRAM2_DISABLE) != 0)
  {
    /** SRAM2 is not secure */
    unsecure_sram2_end = SRAM2_BASE + SRAM2_SIZE;
  }
  else
  {
    /** part of SRAM2 is secure */
    unsecure_sram2_end = OptionsBytesStruct.SecureSRAM2StartAddr;
  }
  /* USER CODE BEGIN MBMUX_RetrieveSecureSramConfig_Last */

  /* USER CODE END MBMUX_RetrieveSecureSramConfig_Last */
}

static void MBMUX_IsrCommandRcvCb(uint32_t channelIdx)
{
  /* USER CODE BEGIN MBMUX_IsrCommandRcvCb_1 */

  /* USER CODE END MBMUX_IsrCommandRcvCb_1 */
  /* retrieve pointer to com params */
  void *com_obj = (void *) &p_MBMUX_ComTable->MBCmdRespParam[channelIdx];
  /* call registered callback */
  MBMUX_MsgCbPointersTabCm0.MBCmdRespCb[channelIdx].MsgCm0plusCb(com_obj);
  /* USER CODE BEGIN MBMUX_IsrCommandRcvCb_Last */

  /* USER CODE END MBMUX_IsrCommandRcvCb_Last */
  return;
}

static void MBMUX_IsrAcknowledgeRcvCb(uint32_t channelIdx)
{
  /* USER CODE BEGIN MBMUX_IsrAcknowledgeRcvCb_1 */

  /* USER CODE END MBMUX_IsrAcknowledgeRcvCb_1 */
  /* retrieve pointer to com params */
  void *com_obj = (void *) &p_MBMUX_ComTable->MBNotifAckParam[channelIdx];
  /* call registered callback */
  MBMUX_MsgCbPointersTabCm0.MBNotifAckCb[channelIdx].MsgCm0plusCb(com_obj);
  /* USER CODE BEGIN MBMUX_IsrAcknowledgeRcvCb_Last */

  /* USER CODE END MBMUX_IsrAcknowledgeRcvCb_Last */
  return;
}

static uint8_t MBMUX_GetFeatureChIdx(FEAT_INFO_IdTypeDef e_featID, MBMUX_ComType_t ComType)
{
  /* USER CODE BEGIN MBMUX_GetFeatureChIdx_1 */

  /* USER CODE END MBMUX_GetFeatureChIdx_1 */
  return p_MBMUX_ComTable->MBMUXMapping[e_featID][ComType];
  /* USER CODE BEGIN MBMUX_GetFeatureChIdx_Last */

  /* USER CODE END MBMUX_GetFeatureChIdx_Last */
}

static void MBMUX_IsrNotRegistered(void *ComObj)
{
  /* USER CODE BEGIN MBMUX_IsrNotRegistered_1 */

  /* USER CODE END MBMUX_IsrNotRegistered_1 */
  Error_Handler();
  /* USER CODE BEGIN MBMUX_IsrNotRegistered_Last */

  /* USER CODE END MBMUX_IsrNotRegistered_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
