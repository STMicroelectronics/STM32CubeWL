/**
  ******************************************************************************
  * @file    mbmuxif_trace.c
  * @author  MCD Application Team
  * @brief   Interface layer CM0PLUS Trace to MBMUX (Mailbox Multiplexer)
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
#include "mbmuxif_trace.h"
#include "mbmux.h"
#include "sys_app.h"
#include "msg_id.h"
#include "stm32_seq.h"
#include "stm32_adv_trace.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

static void MBMUXIF_IsrTraceAckRcvCb(void *ComObj);
static void (*TraceUtilCpltCallback)(void *);
static UTIL_ADV_TRACE_Status_t MBMUXIF_TraceBufferInit(void (*cb)(void *));
static UTIL_ADV_TRACE_Status_t MBMUXIF_TraceBufferSend(uint8_t *buf, uint16_t bufSize);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private typedef -----------------------------------------------------------*/
const UTIL_ADV_TRACE_Driver_s UTIL_TraceDriver =
{
  MBMUXIF_TraceBufferInit,
  NULL,
  NULL,
  MBMUXIF_TraceBufferSend,
};

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Exported functions --------------------------------------------------------*/

/**
  * @brief Registers TRACE feature to the mailbox
  * @retval  0: OK; -1: if ch hasn't been registered by CM4
  * @note    this function is supposed to be called by the System on request (Cmd) of CM4
  */
int8_t MBMUXIF_TraceInit(uint8_t verboseLevel)
{
  int8_t ret;

  /* USER CODE BEGIN MBMUXIF_TraceInit_1 */

  /* USER CODE END MBMUXIF_TraceInit_1 */

  ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_TRACE_ID, MBMUX_NOTIF_ACK, MBMUXIF_IsrTraceAckRcvCb);
  if (ret >= 0)
  {
    /*Initialize the terminal */
    UTIL_ADV_TRACE_Init();
    UTIL_ADV_TRACE_RegisterTimeStampFunction(TimestampNow);

    /*Set verbose LEVEL*/
    UTIL_ADV_TRACE_SetVerboseLevel(verboseLevel);
    ret = 0;
  }

  /* USER CODE BEGIN MBMUXIF_TraceInit_Last */

  /* USER CODE END MBMUXIF_TraceInit_Last */
  return ret;
}

/**
  * @brief gives back the pointer to the com buffer associated to Trace feature Notif
  * @param none
  * @retval  return pointer to the com param buffer
  */
MBMUX_ComParam_t *MBMUXIF_GetTraceFeatureNotifComPtr(void)
{
  /* USER CODE BEGIN MBMUXIF_GetTraceFeatureNotifComPtr_1 */

  /* USER CODE END MBMUXIF_GetTraceFeatureNotifComPtr_1 */
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(FEAT_INFO_TRACE_ID, MBMUX_NOTIF_ACK);
  if (com_param_ptr == NULL)
  {
    while (1) {} /* ErrorHandler() : feature isn't registered */
  }
  return com_param_ptr;
  /* USER CODE BEGIN MBMUXIF_GetTraceFeatureNotifComPtr_Last */

  /* USER CODE END MBMUXIF_GetTraceFeatureNotifComPtr_Last */
}

/**
  * @brief Sends a Trace-Notif via Ipcc without waiting for the ack
  * @param none
  * @retval   none
  */
void MBMUXIF_TraceSendNotif_NoWait(void)
{
  /* USER CODE BEGIN MBMUXIF_TraceSendNotif_NoWait_1 */

  /* USER CODE END MBMUXIF_TraceSendNotif_NoWait_1 */
  if (MBMUX_NotificationSnd(FEAT_INFO_TRACE_ID) != 0)
  {
    while (1) {} /* ErrorHandler(); */
  }
  /* USER CODE BEGIN MBMUXIF_TraceSendNotif_NoWait_Last */

  /* USER CODE END MBMUXIF_TraceSendNotif_NoWait_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Exported services --------------------------------------------------------*/

/**
  * @brief The trace utilities inform the mbmuxif which func to call
  *        once the Mb transmission is completed
  * @param:  "stm32_adv_trace.c callback"
  * @retval: none
  */

static UTIL_ADV_TRACE_Status_t MBMUXIF_TraceBufferInit(void (*cb)(void *))
{
  /* USER CODE BEGIN MBMUXIF_TraceBufferInit_1 */

  /* USER CODE END MBMUXIF_TraceBufferInit_1 */
  TraceUtilCpltCallback = cb;
  return UTIL_ADV_TRACE_OK;
  /* USER CODE BEGIN MBMUXIF_TraceBufferInit_Last */

  /* USER CODE END MBMUXIF_TraceBufferInit_Last */
}

/**
  * @brief TraceBufferSend sends the formatted buffer to CM4 which
  *        posts it to the circular queue for printing
  * @param:  strFormat: string to be printed
  * @retval: 0 when ok, -1 when circular queue is full
  */
static UTIL_ADV_TRACE_Status_t MBMUXIF_TraceBufferSend(uint8_t *buf, uint16_t bufSize)
{
  /* USER CODE BEGIN MBMUXIF_TraceBufferSend_1 */

  /* USER CODE END MBMUXIF_TraceBufferSend_1 */
  MBMUX_ComParam_t *com_obj;

  /* notify CM4 that CM0PLUS is initialised */
  com_obj = MBMUXIF_GetTraceFeatureNotifComPtr();
  if (com_obj != NULL)
  {
    com_obj->MsgId = TRACE_SEND_MSG_ID;
    com_obj->ParamBuf[0] = (uint32_t) buf;
    com_obj->ParamBuf[1] = (uint32_t) bufSize;
    com_obj->ParamCnt = 2;
    MBMUXIF_TraceSendNotif_NoWait();
  }
  return UTIL_ADV_TRACE_OK;
  /* USER CODE BEGIN MBMUXIF_TraceBufferSend_Last */

  /* USER CODE END MBMUXIF_TraceBufferSend_Last */
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  TRACE acknowledge callbacks: set event to release waiting task
  *             and use callback to check if other trace need to be sent to CM4
  * @param  pointer to the TRACE com param buffer
  * @retval  none
  */
static void MBMUXIF_IsrTraceAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrTraceAckRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrTraceAckRcvCb_1 */
  TraceUtilCpltCallback(NULL);
  /* USER CODE BEGIN MBMUXIF_IsrTraceAckRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrTraceAckRcvCb_Last */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
