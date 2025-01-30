/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_radio.c
  * @author  MCD Application Team
  * @brief   allows CM0PLUS application to register and handle RADIO driver via MBMUX
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
#include "mbmuxif_radio.h"
#include "mbmux.h"
#include "sys_app.h"
#include "stm32_seq.h"
#include "radio_mbwrapper.h"
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
static MBMUX_ComParam_t *RadioComObj;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief   RADIO acknowledge callbacks: set event to release waiting task
  * @param   ComObj pointer to the RADIO com param buffer
  */
static void MBMUXIF_IsrRadioAckRcvCb(void *ComObj);

/**
  * @brief   RADIO command callbacks: schedules a task in order to quit the ISR
  * @param   ComObj pointer to the RADIO com param buffer
  */
static void MBMUXIF_IsrRadioCmdRcvCb(void *ComObj);

/**
  * @brief   RADIO task to process the command
  */
static void MBMUXIF_TaskRadioCmdRcv(void);

/**
  * @brief   RADIO task to use the IPCC and the blocking WaitEvt on Task rather then Irq
  */
static void MBMUXIF_TaskRadioNotifSnd(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int8_t MBMUXIF_RadioInit(void)
{
  int8_t ret;

  /* USER CODE BEGIN MBMUXIF_RadioInit_1 */

  /* USER CODE END MBMUXIF_RadioInit_1 */

  ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_RADIO_ID, MBMUX_NOTIF_ACK, MBMUXIF_IsrRadioAckRcvCb);
  if (ret >= 0)
  {
    ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_RADIO_ID, MBMUX_CMD_RESP, MBMUXIF_IsrRadioCmdRcvCb);
  }
  if (ret >= 0)
  {
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_MbRadioCmdRcv), UTIL_SEQ_RFU, MBMUXIF_TaskRadioCmdRcv);
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_MbRadioNotifSnd), UTIL_SEQ_RFU, MBMUXIF_TaskRadioNotifSnd);
    ret = 0;
  }

  /* USER CODE BEGIN MBMUXIF_RadioInit_Last */

  /* USER CODE END MBMUXIF_RadioInit_Last */
  return ret;
}

MBMUX_ComParam_t *MBMUXIF_GetRadioFeatureNotifComPtr(void)
{
  /* USER CODE BEGIN MBMUXIF_GetRadioFeatureNotifComPtr_1 */

  /* USER CODE END MBMUXIF_GetRadioFeatureNotifComPtr_1 */
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(FEAT_INFO_RADIO_ID, MBMUX_NOTIF_ACK);
  if (com_param_ptr == NULL)
  {
    Error_Handler(); /* feature isn't registered */
  }
  return com_param_ptr;
  /* USER CODE BEGIN MBMUXIF_GetRadioFeatureNotifComPtr_Last */

  /* USER CODE END MBMUXIF_GetRadioFeatureNotifComPtr_Last */
}

void MBMUXIF_RadioSendNotif(void)
{
  /* USER CODE BEGIN MBMUXIF_RadioSendNotif_1 */

  /* USER CODE END MBMUXIF_RadioSendNotif_1 */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbRadioNotifSnd), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN MBMUXIF_RadioSendNotif_Last */

  /* USER CODE END MBMUXIF_RadioSendNotif_Last */
}

void MBMUXIF_RadioSendResp(void)
{
  /* USER CODE BEGIN MBMUXIF_RadioSendResp_1 */

  /* USER CODE END MBMUXIF_RadioSendResp_1 */
  if (MBMUX_ResponseSnd(FEAT_INFO_RADIO_ID) != 0)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_RadioSendResp_Last */

  /* USER CODE END MBMUXIF_RadioSendResp_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/
static void MBMUXIF_IsrRadioAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrRadioAckRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrRadioAckRcvCb_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_MbRadioAckRcv);
  /* USER CODE BEGIN MBMUXIF_IsrRadioAckRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrRadioAckRcvCb_Last */
}

static void MBMUXIF_IsrRadioCmdRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrRadioCmdRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrRadioCmdRcvCb_1 */
  RadioComObj = (MBMUX_ComParam_t *) ComObj;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbRadioCmdRcv), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN MBMUXIF_IsrRadioCmdRcvCb_Last*/

  /* USER CODE END MBMUXIF_IsrRadioCmdRcvCb_Last */
}

static void MBMUXIF_TaskRadioCmdRcv(void)
{
  /* USER CODE BEGIN MBMUXIF_TaskRadioCmdRcv_1 */

  /* USER CODE END MBMUXIF_TaskRadioCmdRcv_1 */
  Process_Radio_Cmd(RadioComObj);
  /* USER CODE BEGIN MBMUXIF_TaskRadioCmdRcv_Last */

  /* USER CODE END MBMUXIF_TaskRadioCmdRcv_Last */
}

static void MBMUXIF_TaskRadioNotifSnd(void)
{
  /* USER CODE BEGIN MBMUXIF_TaskRadioNotifSnd_1 */

  /* USER CODE END MBMUXIF_TaskRadioNotifSnd_1 */
  if (MBMUX_NotificationSnd(FEAT_INFO_RADIO_ID) == 0)
  {
    UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_MbRadioAckRcv);
  }
  else
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_TaskRadioNotifSnd_Last */

  /* USER CODE END MBMUXIF_TaskRadioNotifSnd_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
