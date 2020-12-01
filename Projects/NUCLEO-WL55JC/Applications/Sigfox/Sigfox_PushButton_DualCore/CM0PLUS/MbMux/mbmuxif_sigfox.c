/**
  ******************************************************************************
  * @file    mbmuxif_sigfox.c
  * @author  MCD Application Team
  * @brief   allows CM0PLUS applic to register and handle Sigfox to MBMUX
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
static void MBMUXIF_IsrSigfoxAckRcvCb(void *ComObj);
static void MBMUXIF_IsrSigfoxCmdRcvCb(void *ComObj);
static void MBMUXIF_TaskSigfoxCmdRcv(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

/**
  * @brief   Registers Sigfox feature to the mailbox and to the sequencer
  * @param   none
  * @retval  0: OK; -1: if ch hasn't been registered by CM4
  * @note    this function is supposed to be called by the System on request (Cmd) of CM4
  */
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

/**
  * @brief gives back the pointer to the com buffer associated to Sigfox feature Notif
  * @param none
  * @retval  return pointer to the com param buffer
  */
MBMUX_ComParam_t *MBMUXIF_GetSigfoxFeatureNotifComPtr(void)
{
  /* USER CODE BEGIN MBMUXIF_GetSigfoxFeatureNotifComPtr_1 */

  /* USER CODE END MBMUXIF_GetSigfoxFeatureNotifComPtr_1 */
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(FEAT_INFO_SIGFOX_ID, MBMUX_NOTIF_ACK);
  if (com_param_ptr == NULL)
  {
    while (1) {} /* ErrorHandler() : feature isn't registered */
  }
  return com_param_ptr;
  /* USER CODE BEGIN MBMUXIF_GetSigfoxFeatureNotifComPtr_Last */

  /* USER CODE END MBMUXIF_GetSigfoxFeatureNotifComPtr_Last */
}

/**
  * @brief  Set a Task to prevent being blocked on ISR
  * @param  none
  * @retval none
  */
void MBMUXIF_SigfoxSendNotif(void)
{
  /* USER CODE BEGIN MBMUXIF_SigfoxSendNotif_1 */

  /* USER CODE END MBMUXIF_SigfoxSendNotif_1 */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbSigfoxNotifRcv), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN MBMUXIF_SigfoxSendNotif_Last */

  /* USER CODE END MBMUXIF_SigfoxSendNotif_Last */
}

/**
  * @brief  Sends a Sigfox-Notif via Ipcc and Wait for the Ack
  * @param  none
  * @retval none
  */
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
    while (1) {} /* ErrorHandler(); */
  }
  /* USER CODE BEGIN MBMUXIF_SigfoxSendNotifTask_Last */

  /* USER CODE END MBMUXIF_SigfoxSendNotifTask_Last */
}

/**
  * @brief Sends a Sigfox-Resp  via Ipcc without waiting for the response
  * @param none
  * @retval   none
  */
void MBMUXIF_SigfoxSendResp(void)
{
  /* USER CODE BEGIN MBMUXIF_SigfoxSendResp_1 */

  /* USER CODE END MBMUXIF_SigfoxSendResp_1 */
  if (MBMUX_ResponseSnd(FEAT_INFO_SIGFOX_ID) != 0)
  {
    while (1) {} /* ErrorHandler(); */
  }
  /* USER CODE BEGIN MBMUXIF_SigfoxSendResp_Last */

  /* USER CODE END MBMUXIF_SigfoxSendResp_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Sigfox acknowledge callbacks: set event to release waiting task
  * @param  pointer to the Sigfox com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSigfoxAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSigfoxAckRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSigfoxAckRcvCb_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_MbSigfoxAckRcv);
  /* USER CODE BEGIN MBMUXIF_IsrSigfoxAckRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSigfoxAckRcvCb_Last */
}

/**
  * @brief  Sigfox command callbacks: schedules a task in order to quit the ISR
  * @param  pointer to the Sigfox com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrSigfoxCmdRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSigfoxCmdRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSigfoxCmdRcvCb_1 */
  SigfoxComObj = (MBMUX_ComParam_t *) ComObj;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbSigfoxCmdRcv), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN MBMUXIF_IsrSigfoxCmdRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSigfoxCmdRcvCb_Last */
}

/**
  * @brief  Sigfox task to process the command
  * @param  pointer to the Sigfox com param buffer
  * @retval  none
  */
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
