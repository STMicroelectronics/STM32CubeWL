/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_lora.c
  * @author  MCD Application Team
  * @brief   allows CM0 application to register and handle LoraWAN to MBMUX
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
#include "platform.h"
#include "mbmuxif_lora.h"
#include "mbmux.h"
#include "sys_app.h"
#include "stm32_seq.h"
#include "LmHandler_mbwrapper.h"
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
static MBMUX_ComParam_t *LoraComObj;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief   LORA acknowledge callbacks: set event to release waiting task
  * @param   ComObj pointer to the LORA com param buffer
  */
static void MBMUXIF_IsrLoraAckRcvCb(void *ComObj);

/**
  * @brief   LORA command callbacks: schedules a task in order to quit the ISR
  * @param   ComObj pointer to the LORA com param buffer
  */
static void MBMUXIF_IsrLoraCmdRcvCb(void *ComObj);

/**
  * @brief   LORA task to process the command
  */
static void MBMUXIF_TaskLoraCmdRcv(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int8_t MBMUXIF_LoraInit(void)
{
  int8_t ret;
  /* USER CODE BEGIN MBMUXIF_LoraInit_1 */

  /* USER CODE END MBMUXIF_LoraInit_1 */

  ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_LORAWAN_ID, MBMUX_NOTIF_ACK, MBMUXIF_IsrLoraAckRcvCb);
  if (ret >= 0)
  {
    ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_LORAWAN_ID, MBMUX_CMD_RESP, MBMUXIF_IsrLoraCmdRcvCb);
  }
  if (ret >= 0)
  {
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_MbLoRaCmdRcv), UTIL_SEQ_RFU, MBMUXIF_TaskLoraCmdRcv);
    ret = 0;
  }

  /* USER CODE BEGIN MBMUXIF_LoraInit_Last */

  /* USER CODE END MBMUXIF_LoraInit_Last */
  return ret;
}

MBMUX_ComParam_t *MBMUXIF_GetLoraFeatureNotifComPtr(void)
{
  /* USER CODE BEGIN MBMUXIF_GetLoraFeatureNotifComPtr_1 */

  /* USER CODE END MBMUXIF_GetLoraFeatureNotifComPtr_1 */
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(FEAT_INFO_LORAWAN_ID, MBMUX_NOTIF_ACK);
  if (com_param_ptr == NULL)
  {
    Error_Handler(); /* feature isn't registered */
  }
  return com_param_ptr;
  /* USER CODE BEGIN MBMUXIF_GetLoraFeatureNotifComPtr_Last */

  /* USER CODE END MBMUXIF_GetLoraFeatureNotifComPtr_Last */
}

void MBMUXIF_LoraSendNotif(void)
{
  /* to check vs radio. radio call this on irq so it moves here on task */
  /* for lora is already called from task .. */
  /* USER CODE BEGIN MBMUXIF_LoraSendNotif_1 */

  /* USER CODE END MBMUXIF_LoraSendNotif_1 */
  if (MBMUX_NotificationSnd(FEAT_INFO_LORAWAN_ID) == 0)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_MbLoraAckRcv);
  }
  else
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_LoraSendNotif_Last */

  /* USER CODE END MBMUXIF_LoraSendNotif_Last */
}

void MBMUXIF_LoraSendResp(void)
{
  /* USER CODE BEGIN MBMUXIF_LoraSendResp_1 */

  /* USER CODE END MBMUXIF_LoraSendResp_1 */
  if (MBMUX_ResponseSnd(FEAT_INFO_LORAWAN_ID) != 0)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_LoraSendResp_Last */

  /* USER CODE END MBMUXIF_LoraSendResp_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/
static void MBMUXIF_IsrLoraAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrLoraAckRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrLoraAckRcvCb_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_MbLoraAckRcv);
  /* USER CODE BEGIN MBMUXIF_IsrLoraAckRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrLoraAckRcvCb_Last */
}

static void MBMUXIF_IsrLoraCmdRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrLoraCmdRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrLoraCmdRcvCb_1 */
  LoraComObj = (MBMUX_ComParam_t *) ComObj;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbLoRaCmdRcv), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN MBMUXIF_IsrLoraCmdRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrLoraCmdRcvCb_Last */
}

static void MBMUXIF_TaskLoraCmdRcv(void)
{
  /* USER CODE BEGIN MBMUXIF_TaskLoraCmdRcv_1 */

  /* USER CODE END MBMUXIF_TaskLoraCmdRcv_1 */
  Process_Lora_Cmd(LoraComObj);
  /* USER CODE BEGIN MBMUXIF_TaskLoraCmdRcv_Last */

  /* USER CODE END MBMUXIF_TaskLoraCmdRcv_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
