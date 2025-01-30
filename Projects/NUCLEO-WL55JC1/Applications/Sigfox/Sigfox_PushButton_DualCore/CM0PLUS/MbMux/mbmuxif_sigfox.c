/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_sigfox.c
  * @author  MCD Application Team
  * @brief   allows CM0PLUS application to register and handle Sigfox to MBMUX
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
#include "mbmuxif_sigfox.h"
#include "mbmux.h"
#include "sys_app.h"
#include "stm32_seq.h"
#include "sigfox_mbwrapper.h"
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
static MBMUX_ComParam_t *SigfoxComObj;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Sigfox acknowledge callbacks: set event to release waiting task
  * @param  ComObj pointer to the Sigfox com param buffer
  */
static void MBMUXIF_IsrSigfoxAckRcvCb(void *ComObj);

/**
  * @brief  Sigfox command callbacks: schedules a task in order to quit the ISR
  * @param  ComObj pointer to the Sigfox com param buffer
  */
static void MBMUXIF_IsrSigfoxCmdRcvCb(void *ComObj);

/**
  * @brief  Sigfox task to process the command
  */
static void MBMUXIF_TaskSigfoxCmdRcv(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int8_t MBMUXIF_SigfoxInit(void)
{
  int8_t ret;

  /* USER CODE BEGIN MBMUXIF_SigfoxInit_1 */

  /* USER CODE END MBMUXIF_SigfoxInit_1 */

  ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_SIGFOX_ID, MBMUX_NOTIF_ACK, MBMUXIF_IsrSigfoxAckRcvCb);
  if (ret >= 0)
  {
    ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_SIGFOX_ID, MBMUX_CMD_RESP, MBMUXIF_IsrSigfoxCmdRcvCb);
  }
  if (ret >= 0)
  {
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_MbSigfoxCmdRcv), UTIL_SEQ_RFU, MBMUXIF_TaskSigfoxCmdRcv);
    ret = 0;
  }

  /* USER CODE BEGIN MBMUXIF_SigfoxInit_Last */

  /* USER CODE END MBMUXIF_SigfoxInit_Last */

  return ret;
}

MBMUX_ComParam_t *MBMUXIF_GetSigfoxFeatureNotifComPtr(void)
{
  /* USER CODE BEGIN MBMUXIF_GetSigfoxFeatureNotifComPtr_1 */

  /* USER CODE END MBMUXIF_GetSigfoxFeatureNotifComPtr_1 */
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(FEAT_INFO_SIGFOX_ID, MBMUX_NOTIF_ACK);
  if (com_param_ptr == NULL)
  {
    Error_Handler(); /* feature isn't registered */
  }
  return com_param_ptr;
  /* USER CODE BEGIN MBMUXIF_GetSigfoxFeatureNotifComPtr_Last */

  /* USER CODE END MBMUXIF_GetSigfoxFeatureNotifComPtr_Last */
}

void MBMUXIF_SigfoxSendNotif(void)
{
  /* USER CODE BEGIN MBMUXIF_SigfoxSendNotif_1 */

  /* USER CODE END MBMUXIF_SigfoxSendNotif_1 */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbSigfoxNotifRcv), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN MBMUXIF_SigfoxSendNotif_Last */

  /* USER CODE END MBMUXIF_SigfoxSendNotif_Last */
}

void MBMUXIF_SigfoxSendNotifTask(void)
{
  /* USER CODE BEGIN MBMUXIF_SigfoxSendNotifTask_1 */

  /* USER CODE END MBMUXIF_SigfoxSendNotifTask_1 */
  if (MBMUX_NotificationSnd(FEAT_INFO_SIGFOX_ID) == 0)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_MbSigfoxAckRcv);
  }
  else
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_SigfoxSendNotifTask_Last */

  /* USER CODE END MBMUXIF_SigfoxSendNotifTask_Last */
}

void MBMUXIF_SigfoxSendResp(void)
{
  /* USER CODE BEGIN MBMUXIF_SigfoxSendResp_1 */

  /* USER CODE END MBMUXIF_SigfoxSendResp_1 */
  if (MBMUX_ResponseSnd(FEAT_INFO_SIGFOX_ID) != 0)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_SigfoxSendResp_Last */

  /* USER CODE END MBMUXIF_SigfoxSendResp_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/

static void MBMUXIF_IsrSigfoxAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSigfoxAckRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSigfoxAckRcvCb_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_MbSigfoxAckRcv);
  /* USER CODE BEGIN MBMUXIF_IsrSigfoxAckRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSigfoxAckRcvCb_Last */
}

static void MBMUXIF_IsrSigfoxCmdRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSigfoxCmdRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSigfoxCmdRcvCb_1 */
  SigfoxComObj = (MBMUX_ComParam_t *) ComObj;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbSigfoxCmdRcv), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN MBMUXIF_IsrSigfoxCmdRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSigfoxCmdRcvCb_Last */
}

static void MBMUXIF_TaskSigfoxCmdRcv(void)
{
  /* USER CODE BEGIN MBMUXIF_TaskSigfoxCmdRcv_1 */

  /* USER CODE END MBMUXIF_TaskSigfoxCmdRcv_1 */
  Process_Sigfox_Cmd(SigfoxComObj);
  /* USER CODE BEGIN MBMUXIF_TaskSigfoxCmdRcv_Last */

  /* USER CODE END MBMUXIF_TaskSigfoxCmdRcv_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
