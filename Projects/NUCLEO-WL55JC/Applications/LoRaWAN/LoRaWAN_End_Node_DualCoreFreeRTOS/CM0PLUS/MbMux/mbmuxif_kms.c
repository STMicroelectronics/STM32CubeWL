/**
  ******************************************************************************
  * @file    mbmuxif_kms.c
  * @author  MCD Application Team
  * @brief   Interface layer CM0PLUS Kms to MBMUX (Mailbox Multiplexer)
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
#include "mbmux.h"
#include "sys_app.h"
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
static MBMUX_ComParam_t *KmsComObj;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MBMUXIF_IsrKmsCmdRcvCb(void *ComObj);
static void MBMUXIF_TaskKmsCmdRcv(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

/**
  * @brief   Registers KMS feature to the mailbox and to the sequencer
  * @param   none
  * @retval  0: OK; -1: if ch hasn't been registered by CM4
  * @note    this function is supposed to be called by the System on request (Cmd) of CM4
  */
int8_t MBMUXIF_KmsInit(void)
{
  int8_t ret;
  /* USER CODE BEGIN MBMUXIF_KmsInit_1 */

  /* USER CODE END MBMUXIF_KmsInit_1 */

  ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_KMS_ID, MBMUX_CMD_RESP, MBMUXIF_IsrKmsCmdRcvCb);
  if (ret >= 0)
  {
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_MbKmsCmdRcv), UTIL_SEQ_RFU, MBMUXIF_TaskKmsCmdRcv);
    ret = 0;
  }

  /* USER CODE BEGIN MBMUXIF_KmsInit_Last */

  /* USER CODE END MBMUXIF_KmsInit_Last */
  return ret;
}

/**
  * @brief Sends a Kms-Resp  via Ipcc without waiting for the response
  * @param none
  * @retval   none
  */
void MBMUXIF_KmsSendResp(void)
{
  /* USER CODE BEGIN MBMUXIF_KmsSendResp_1 */

  /* USER CODE END MBMUXIF_KmsSendResp_1 */
  if (MBMUX_ResponseSnd(FEAT_INFO_KMS_ID) != 0)
  {
    while (1) {} /* ErrorHandler(); */
  }
  /* USER CODE BEGIN MBMUXIF_KmsSendResp_Last */

  /* USER CODE END MBMUXIF_KmsSendResp_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  KMS command callbacks: schedules a task in order to quit the ISR
  * @param  pointer to the KMS com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrKmsCmdRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrKmsCmdRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrKmsCmdRcvCb_1 */
  KmsComObj = (MBMUX_ComParam_t *) ComObj;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbKmsCmdRcv), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN MBMUXIF_IsrKmsCmdRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrKmsCmdRcvCb_Last */
}

/**
  * @brief  KMS task to process the command
  * @param  pointer to the KMS com param buffer
  * @retval  none
  */
static void MBMUXIF_TaskKmsCmdRcv(void)
{
  /* USER CODE BEGIN MBMUXIF_TaskKmsCmdRcv_1 */

  /* USER CODE END MBMUXIF_TaskKmsCmdRcv_1 */
#ifdef ALLOW_KMS_VIA_MBMUX /* currently not supported */
  Process_Kms_Cmd(KmsComObj);
#endif /* ALLOW_KMS_VIA_MBMUX */
  /* USER CODE BEGIN MBMUXIF_TaskKmsCmdRcv_Last */

  /* USER CODE END MBMUXIF_TaskKmsCmdRcv_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
