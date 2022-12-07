/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_trace.c
  * @author  MCD Application Team
  * @brief   Interface layer CM0PLUS Trace to MBMUX (Mailbox Multiplexer)
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
#include "mbmux.h"
#include "sys_app.h"
#include "msg_id.h"
#include "stm32_seq.h"
#include "stm32_adv_trace.h"
#include "utilities_def.h"   /* for CFG_SEQ_Evt_MbTraceAckRcv */

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
/**
  * @brief util trace callback
  */
static void (*TraceUtilCpltCallback)(void *);

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief   TRACE acknowledge callbacks: set event to release waiting task
  *          and schedule callback to check if other trace need to be sent to CM4
  * @param   ComObj pointer to the TRACE com param buffer
  */
static void MBMUXIF_IsrTraceAckRcvCb(void *ComObj);

/**
  * @brief   Call callback to check if other trace need to be sent to CM4
  */
static void MBMUXIF_TaskTraceAckRcv(void);

/**
  * @brief   The trace utilities inform the mbmuxif which func to call
  *          once the Mb transmission is completed
  * @param   cb "stm32_adv_trace.c callback"
  * @retval  0
  */
static UTIL_ADV_TRACE_Status_t MBMUXIF_TraceBufferInit(void (*cb)(void *));

/**
  * @brief   TraceBufferSend sends the formatted buffer to CM4 which
  *          posts it to the circular queue for printing
  * @param   buf string to be printed
  * @param   bufSize size of buffer
  * @retval  0 when ok, -1 when circular queue is full
  */
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
int8_t MBMUXIF_TraceInit(uint8_t verboseLevel)
{
  int8_t ret;

  /* USER CODE BEGIN MBMUXIF_TraceInit_1 */

  /* USER CODE END MBMUXIF_TraceInit_1 */

  ret = MBMUX_RegisterFeatureCallback(FEAT_INFO_TRACE_ID, MBMUX_NOTIF_ACK, MBMUXIF_IsrTraceAckRcvCb);
  if (ret >= 0)
  {
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_MbTraceAckRcv), UTIL_SEQ_RFU, MBMUXIF_TaskTraceAckRcv);

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

MBMUX_ComParam_t *MBMUXIF_GetTraceFeatureNotifComPtr(void)
{
  /* USER CODE BEGIN MBMUXIF_GetTraceFeatureNotifComPtr_1 */

  /* USER CODE END MBMUXIF_GetTraceFeatureNotifComPtr_1 */
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(FEAT_INFO_TRACE_ID, MBMUX_NOTIF_ACK);
  if (com_param_ptr == NULL)
  {
    Error_Handler(); /* feature isn't registered */
  }
  return com_param_ptr;
  /* USER CODE BEGIN MBMUXIF_GetTraceFeatureNotifComPtr_Last */

  /* USER CODE END MBMUXIF_GetTraceFeatureNotifComPtr_Last */
}

void MBMUXIF_TraceSendNotif_NoWait(void)
{
  /* USER CODE BEGIN MBMUXIF_TraceSendNotif_NoWait_1 */

  /* USER CODE END MBMUXIF_TraceSendNotif_NoWait_1 */
  if (MBMUX_NotificationSnd(FEAT_INFO_TRACE_ID) != 0)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_TraceSendNotif_NoWait_Last */

  /* USER CODE END MBMUXIF_TraceSendNotif_NoWait_Last */
}

/* USER CODE BEGIN EFD */

/* USER CODE END EFD */

/* Exported services --------------------------------------------------------*/
static UTIL_ADV_TRACE_Status_t MBMUXIF_TraceBufferInit(void (*cb)(void *))
{
  /* USER CODE BEGIN MBMUXIF_TraceBufferInit_1 */

  /* USER CODE END MBMUXIF_TraceBufferInit_1 */
  TraceUtilCpltCallback = cb;
  return UTIL_ADV_TRACE_OK;
  /* USER CODE BEGIN MBMUXIF_TraceBufferInit_Last */

  /* USER CODE END MBMUXIF_TraceBufferInit_Last */
}

static UTIL_ADV_TRACE_Status_t MBMUXIF_TraceBufferSend(uint8_t *buf, uint16_t bufSize)
{
  /* USER CODE BEGIN MBMUXIF_TraceBufferSend_1 */

  /* USER CODE END MBMUXIF_TraceBufferSend_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *pbuf_validated = NULL;

  /* notify CM4 that CM0PLUS is initialized */
  com_obj = MBMUXIF_GetTraceFeatureNotifComPtr();
  if (com_obj != NULL)
  {
    pbuf_validated = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);

    com_obj->MsgId = TRACE_SEND_MSG_ID;
    pbuf_validated[0] = (uint32_t) buf;
    pbuf_validated[1] = (uint32_t) bufSize;
    com_obj->ParamCnt = 2;
    MBMUXIF_TraceSendNotif_NoWait();
  }
  return UTIL_ADV_TRACE_OK;
  /* USER CODE BEGIN MBMUXIF_TraceBufferSend_Last */

  /* USER CODE END MBMUXIF_TraceBufferSend_Last */
}

/* Private functions ---------------------------------------------------------*/
static void MBMUXIF_IsrTraceAckRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrTraceAckRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrTraceAckRcvCb_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_MbTraceAckRcv); /* not necessary */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbTraceAckRcv), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN MBMUXIF_IsrTraceAckRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrTraceAckRcvCb_Last */
}

static void MBMUXIF_TaskTraceAckRcv(void)
{
  /* USER CODE BEGIN MBMUXIF_TaskTraceAckRcv_1 */

  /* USER CODE END MBMUXIF_TaskTraceAckRcv_1 */
  TraceUtilCpltCallback(NULL);
  /* USER CODE BEGIN MBMUXIF_TaskTraceAckRcv_Last */

  /* USER CODE END MBMUXIF_TaskTraceAckRcv_Last */
}
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
