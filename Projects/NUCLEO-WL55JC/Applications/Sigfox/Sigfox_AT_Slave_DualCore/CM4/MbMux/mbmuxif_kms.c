/**
  ******************************************************************************
  * @file    mbmuxif_kms.c
  * @author  MCD Application Team
  * @brief   Interface layer CM4 Kms to MBMUX (Mailbox Multiplexer)
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
#include "platform.h"
#include "mbmuxif_kms.h"
#include "mbmuxif_sys.h"
#include "stm32_mem.h"
#include "stm32_seq.h"
#include "utilities_def.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aKmsCmdRespBuff[MAX_PARAM_OF_KMS_CMD_FUNCTIONS];/*shared*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MBMUXIF_IsrKmsRespRcvCb(void *ComObj);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

/**
  * @brief Registers KMS feature to the mailbox
  * @param none
  * @retval   0: OK; -1: no more ipcc channel available; -2: feature not provided by CM0PLUS
  */
int8_t MBMUXIF_KmsInit(void)
{
  int8_t ret;
  /* USER CODE BEGIN MBMUXIF_KmsInit_1 */

  /* USER CODE END MBMUXIF_KmsInit_1 */

  ret = MBMUX_RegisterFeature(FEAT_INFO_KMS_ID, MBMUX_CMD_RESP, MBMUXIF_IsrKmsRespRcvCb, aKmsCmdRespBuff, sizeof(aKmsCmdRespBuff));
  if (ret >= 0)
  {
    /* Note: Kms uses only NOTIF_ACK channel, the SEQ_TASK only on CM0PLUS */

    ret = MBMUXIF_SystemSendCm0plusRegistrationCmd(FEAT_INFO_KMS_ID);
    if (ret < 0)
    {
      ret = -3;
    }
  }

  /* USER CODE BEGIN MBMUXIF_KmsInit_Last */

  /* USER CODE END MBMUXIF_KmsInit_Last */
  return ret;
}

/**
  * @brief gives back the pointer to the com buffer associated to Kms feature Cmd
  * @param none
  * @retval  return pointer to the com param buffer
  */
MBMUX_ComParam_t *MBMUXIF_GetKmsFeatureCmdComPtr(void)
{
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(FEAT_INFO_KMS_ID, MBMUX_CMD_RESP);
  /* USER CODE BEGIN MBMUXIF_GetKmsFeatureCmdComPtr_1 */

  /* USER CODE END MBMUXIF_GetKmsFeatureCmdComPtr_1 */

  if (com_param_ptr == NULL)
  {
    Error_Handler(); /* ErrorHandler() : feature isn't registered */
  }

  /* USER CODE BEGIN MBMUXIF_GetKmsFeatureCmdComPtr_Last */

  /* USER CODE END MBMUXIF_GetKmsFeatureCmdComPtr_Last */
  return com_param_ptr;
}

/**
  * @brief Sends a Kms-Cmd via Ipcc and Wait for the response
  * @param none
  * @retval   none
  */
void MBMUXIF_KmsSendCmd(void)
{
  /* USER CODE BEGIN MBMUXIF_KmsSendCmd_1 */

  /* USER CODE END MBMUXIF_KmsSendCmd_1 */
  if (MBMUX_CommandSnd(FEAT_INFO_KMS_ID) == 0)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_MbKmsRespRcv);
  }
  else
  {
    while (1) {} /* ErrorHandler(); */
  }
  /* USER CODE BEGIN MBMUXIF_KmsSendCmd_Last */

  /* USER CODE END MBMUXIF_KmsSendCmd_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  KMS response callbacks: set event to release waiting task
  * @param  pointer to the KMS com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrKmsRespRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrKmsRespRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrKmsRespRcvCb_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_MbKmsRespRcv);
  /* USER CODE BEGIN MBMUXIF_IsrKmsRespRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrKmsRespRcvCb_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
