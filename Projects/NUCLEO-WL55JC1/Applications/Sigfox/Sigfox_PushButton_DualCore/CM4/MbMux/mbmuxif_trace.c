/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_trace.c
  * @author  MCD Application Team
  * @brief   allows CM4 application to receive by CM0 TRACE via MBMUX
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
#include "mbmuxif_trace.h"
#include "mbmuxif_sys.h"
#include "sys_app.h"
#include "msg_id.h"
#include "stm32_mem.h"
#include "stm32_adv_trace.h"

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
/**
  * @brief trace buffer to exchange data between CM4 and CM0+
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aTraceNotifAckBuff[MAX_PARAM_OF_TRACE_NOTIF_FUNCTIONS];/*shared*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  TRACE notification callbacks: schedules a task in order to quit the ISR
  * @param  ComObj pointer to the TRACE com param buffer
  */
static void MBMUXIF_IsrTraceNotifRcvCb(void *ComObj);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int8_t MBMUXIF_TraceInit(void)
{
  int8_t ret;
  /* USER CODE BEGIN MBMUXIF_TraceInit_1 */

  /* USER CODE END MBMUXIF_TraceInit_1 */

  ret = MBMUX_RegisterFeature(FEAT_INFO_TRACE_ID, MBMUX_NOTIF_ACK,
                              MBMUXIF_IsrTraceNotifRcvCb,
                              aTraceNotifAckBuff, sizeof(aTraceNotifAckBuff));
  if (ret >= 0)
  {
    /* Note: trace works on ISR, no need to associate a SEQ_TASK */

    ret = MBMUXIF_SystemSendCm0plusRegistrationCmd(FEAT_INFO_TRACE_ID);
    if (ret < 0)
    {
      ret = -3;
    }
  }

  /* USER CODE BEGIN MBMUXIF_TraceInit_Last */

  /* USER CODE END MBMUXIF_TraceInit_Last */
  return ret;
}

void MBMUXIF_TraceSendAck(void)
{
  /* USER CODE BEGIN MBMUXIF_TraceSendAck_1 */

  /* USER CODE END MBMUXIF_TraceSendAck_1 */
  if (MBMUX_AcknowledgeSnd(FEAT_INFO_TRACE_ID) != 0)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_TraceSendAck_Last */

  /* USER CODE END MBMUXIF_TraceSendAck_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Private functions ---------------------------------------------------------*/
static void MBMUXIF_IsrTraceNotifRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrTraceNotifRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrTraceNotifRcvCb_1 */
  MBMUX_ComParam_t *TraceComObj;
  uint32_t  notif_ack_id;
  uint8_t *buffer;
  uint16_t bufsize;
  UTIL_ADV_TRACE_Status_t ret;

  TraceComObj = (MBMUX_ComParam_t *) ComObj;

  notif_ack_id = TraceComObj->MsgId;

  switch (notif_ack_id)
  {
    case TRACE_SEND_MSG_ID:
      buffer = (uint8_t *) TraceComObj->ParamBuf[0];
      bufsize = (uint16_t) TraceComObj->ParamBuf[1];
      ret = UTIL_ADV_TRACE_Send(buffer, bufsize);
      /* prepare ack buffer */
      TraceComObj->ParamCnt = 0;
      TraceComObj->ReturnVal = (uint32_t)ret;
      break;

    default:
      break;
  }

  /* Send ack */
  MBMUXIF_TraceSendAck();
  /* USER CODE BEGIN MBMUXIF_IsrTraceNotifRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrTraceNotifRcvCb_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
