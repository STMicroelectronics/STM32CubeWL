/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    radio_mbwrapper.c
  * @author  MCD Application Team
  * @brief   This file implements the CM0 side wrapper of the Radio interface
  *          shared between M0 and M4.
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
#include "radio_mbwrapper.h"
#include "main.h"
#include "stm32_mem.h"
#include "mbmux_table.h"
#include "msg_id.h"
#include "mbmux.h"
#include "mbmuxif_radio.h"
#include "sys_app.h"

#include "radio.h"
#include "radio_conf.h"

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
/**
  * @brief radio buffer to exchange data between CM4 and CM0+
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM3") uint8_t aRadioMbWrapRxBuffer[RADIO_RX_BUF_SIZE];

/**
  * @brief local structure of radio callbacks for command processes
  */
static  RadioEvents_t radioevents_mbwrapper;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
 * \brief  Tx Done callback prototype.
 */
static void RadioTxDone_mbwrapper(void);

/*!
 * \brief  Tx Timeout callback prototype.
 */
static void RadioTxTimeout_mbwrapper(void);

/*!
 * \brief Rx Done callback prototype.
 *
 * \param[in] payload Received buffer pointer
 * \param[in] size    Received buffer size
 * \param[in] rssi    RSSI value computed while receiving the frame [dBm]
 * \param[in] snr     Raw SNR value given by the radio hardware
 *                     FSK : N/A ( set to 0 )
 *                     LoRa: SNR value in dB
 */
static void RadioRxDone_mbwrapper(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

/*!
 * \brief  Rx Timeout callback prototype.
 */
static void RadioRxTimeout_mbwrapper(void);

/*!
 * \brief Rx Error callback prototype.
 */
static void RadioRxError_mbwrapper(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
void Process_Radio_Cmd(MBMUX_ComParam_t *ComObj)
{
  /* USER CODE BEGIN Process_Radio_Cmd_1 */

  /* USER CODE END Process_Radio_Cmd_1 */
  uint32_t *com_buffer = NULL;
  uint32_t ret_uint;
  int32_t ret_int;
  radio_status_t ret_status;
  RadioState_t state;

  APP_LOG(TS_ON, VLEVEL_H, ">CM0PLUS(Radio)\r\n");

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(ComObj->ParamBuf, ComObj->BufSize);

  /* process Command */
  switch (ComObj->MsgId)
  {
    case RADIO_INIT_ID:
      radioevents_mbwrapper.TxDone = &RadioTxDone_mbwrapper;
      radioevents_mbwrapper.TxTimeout = &RadioTxTimeout_mbwrapper;
      radioevents_mbwrapper.RxDone = &RadioRxDone_mbwrapper;
      radioevents_mbwrapper.RxTimeout = &RadioRxTimeout_mbwrapper;
      radioevents_mbwrapper.RxError = &RadioRxError_mbwrapper;

      Radio.Init(&radioevents_mbwrapper);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_GET_STATUS_ID:
      state = Radio.GetStatus();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) state; /* */
      break;

    case RADIO_SET_MODEM_ID:
      Radio.SetModem((RadioModems_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_SET_CHANNEL_ID:
      Radio.SetChannel((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_IS_CHANNEL_FREE_ID:
      ret_uint = Radio.IsChannelFree(com_buffer[0], com_buffer[1], (int16_t) com_buffer[2], com_buffer[3]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_uint; /* */
      break;

    case RADIO_RANDOM_ID:
      ret_uint = Radio.Random();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_uint; /* */
      break;

    case RADIO_SET_RX_CONFIG_ID:
      Radio.SetRxConfig((RadioModems_t) com_buffer[0],  com_buffer[1],
                        com_buffer[2], (uint8_t) com_buffer[3],
                        com_buffer[4], (uint16_t) com_buffer[5],
                        (uint16_t) com_buffer[6], (bool) com_buffer[7],
                        (uint8_t) com_buffer[8],
                        (bool) com_buffer[9], (bool) com_buffer[10], (uint8_t) com_buffer[11],
                        (bool) com_buffer[12], (bool) com_buffer[13]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_SET_TX_CONFIG_ID:
      Radio.SetTxConfig((RadioModems_t) com_buffer[0], (int8_t) com_buffer[1],
                        com_buffer[2], com_buffer[3],  com_buffer[4],
                        (uint8_t) com_buffer[5], (uint16_t) com_buffer[6],
                        (bool) com_buffer[7], (bool) com_buffer[8], (bool) com_buffer[9],
                        (uint8_t) com_buffer[10], (bool) com_buffer[11],  com_buffer[12]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_CHECK_RF_FREQUENCY_ID:
      ret_uint = Radio.CheckRfFrequency(com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_uint; /* */
      break;

    case RADIO_GET_TIME_ON_AIR_ID:
      ret_uint = Radio.TimeOnAir((RadioModems_t) com_buffer[0], (uint32_t) com_buffer[1],
                                 (uint32_t) com_buffer[2], (uint8_t) com_buffer[3],
                                 (uint16_t) com_buffer[4], (bool) com_buffer[5],
                                 (uint8_t) com_buffer[6], (bool) com_buffer[7]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_uint; /* */
      break;

    case RADIO_SEND_ID:
      ret_status = Radio.Send((uint8_t *) com_buffer[0], (uint8_t) com_buffer[1]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_status; /* */
      break;

    case RADIO_SLEEP_ID:
      Radio.Sleep();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_STANDBY_ID:
      Radio.Standby();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_RX_ID:
      Radio.Rx(com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_START_CAD_ID:
      Radio.StartCad();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_SET_TX_CONTINUOUS_WAVE_ID:
      Radio.SetTxContinuousWave(com_buffer[0], (int8_t) com_buffer[1], (uint16_t) com_buffer[2]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_RSSI_ID:
      ret_int =  Radio.Rssi((RadioModems_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_int; /* */
      break;

    case RADIO_WRITE_ID:
      Radio.Write((uint16_t) com_buffer[0], (uint8_t) com_buffer[1]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_READ_ID:
      ret_uint = Radio.Read((uint16_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_uint; /* */
      break;

    case RADIO_WRITE_BUFFER_ID:
      Radio.WriteRegisters((uint16_t) com_buffer[0], (uint8_t *) com_buffer[1], (uint8_t) com_buffer[2]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_READ_BUFFER_ID:
      Radio.ReadRegisters((uint16_t) com_buffer[0], (uint8_t *) com_buffer[1], (uint8_t) com_buffer[2]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_SET_MAX_PAYLOAD_LENGTH_ID:
      Radio.SetMaxPayloadLength((RadioModems_t) com_buffer[0], (uint8_t) com_buffer[1]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_SET_PUBLIC_NETWORK_ID:
      Radio.SetPublicNetwork((bool) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_GET_WAKEUP_TIME_ID:
      ret_uint = Radio.GetWakeupTime();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_uint; /* */
      break;

    case RADIO_IRQ_PROCESS_ID:
      Radio.IrqProcess();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_RX_BOOSTED_ID:
      Radio.RxBoosted(com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_SET_RX_DUTY_CYCLE_ID:
      Radio.SetRxDutyCycle(com_buffer[0], com_buffer[1]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_TX_CW_ID:
      Radio.TxCw((int8_t)com_buffer[0]);
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_TX_PRBS_ID:
      Radio.TxPrbs();
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case RADIO_SET_RX_GENERIC_CONFIG_ID:
      ret_uint = Radio.RadioSetRxGenericConfig((GenericModems_t) com_buffer[0], (RxConfigGeneric_t *)com_buffer[1], com_buffer[2], com_buffer[3]);
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_uint; /* */
      break;

    case RADIO_SET_TX_GENERIC_CONFIG_ID:
      ret_uint = Radio.RadioSetTxGenericConfig((GenericModems_t) com_buffer[0], (TxConfigGeneric_t *)com_buffer[1], (int8_t) com_buffer[2], (uint32_t) com_buffer[3]);
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_uint; /* */
      break;

    default:
      break;
  }

  /* send Response */
  APP_LOG(TS_ON, VLEVEL_H, "<CM0PLUS(Radio)\r\n");
  MBMUX_ResponseSnd(FEAT_INFO_RADIO_ID);
  /* USER CODE BEGIN Process_Radio_Cmd_2 */

  /* USER CODE END Process_Radio_Cmd_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
static void RadioTxDone_mbwrapper(void)
{
  /* USER CODE BEGIN RadioTxDone_mbwrapper_1 */

  /* USER CODE END RadioTxDone_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;

  com_obj = MBMUXIF_GetRadioFeatureNotifComPtr();
  com_obj->MsgId = RADIO_TX_DONE_CB_ID;
  com_obj->ParamCnt = 0;
  MBMUXIF_RadioSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN RadioTxDone_mbwrapper_2 */

  /* USER CODE END RadioTxDone_mbwrapper_2 */
}

static void RadioTxTimeout_mbwrapper(void)
{
  /* USER CODE BEGIN RadioTxTimeout_mbwrapper_1 */

  /* USER CODE END RadioTxTimeout_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;

  com_obj = MBMUXIF_GetRadioFeatureNotifComPtr();
  com_obj->MsgId = RADIO_TX_TIMEOUT_CB_ID;
  com_obj->ParamCnt = 0;
  MBMUXIF_RadioSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN RadioTxTimeout_mbwrapper_2 */

  /* USER CODE END RadioTxTimeout_mbwrapper_2 */
}

static void RadioRxDone_mbwrapper(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
  /* USER CODE BEGIN RadioRxDone_mbwrapper_1 */

  /* USER CODE END RadioRxDone_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;
  uint16_t i = 0;

  /* copy data from Cm0 private memory to shared memory */
  if (payload != NULL)
  {
    UTIL_MEM_cpy_8(aRadioMbWrapRxBuffer, payload, size);
  }

  com_obj = MBMUXIF_GetRadioFeatureNotifComPtr();
  com_obj->MsgId = RADIO_RX_DONE_CB_ID;
  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  if (payload == NULL)
  {
    com_buffer[i++] = (uint32_t)NULL;
  }
  else
  {
    com_buffer[i++] = (uint32_t) aRadioMbWrapRxBuffer;
  }
  com_buffer[i++] = (uint32_t) size;
  com_buffer[i++] = (uint32_t) rssi;
  com_buffer[i++] = (uint32_t) snr;
  com_obj->ParamCnt = i;
  if (i > MAX_PARAM_OF_RADIO_NOTIF_FUNCTIONS)
  {
    Error_Handler();
  }

  MBMUXIF_RadioSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */

  /* Notice: current needs don't require to copy back data from aRadioMbWrapRxBuffer to payload
     this might be requested just if the buffer should be processed by the application and the stack */
  /* UTIL_MEM_cpy_8(payload, aRadioMbWrapRxBuffer, size); */
  return;
  /* USER CODE BEGIN RadioRxDone_mbwrapper_2 */

  /* USER CODE END RadioRxDone_mbwrapper_2 */
}

static void RadioRxTimeout_mbwrapper(void)
{
  /* USER CODE BEGIN RadioRxTimeout_mbwrapper_1 */

  /* USER CODE END RadioRxTimeout_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;

  com_obj = MBMUXIF_GetRadioFeatureNotifComPtr();
  com_obj->MsgId = RADIO_RX_TIMEOUT_CB_ID;
  com_obj->ParamCnt = 0;

  MBMUXIF_RadioSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN RadioRxTimeout_mbwrapper_2 */

  /* USER CODE END RadioRxTimeout_mbwrapper_2 */
}

static void RadioRxError_mbwrapper(void)
{
  /* USER CODE BEGIN RadioRxError_mbwrapper_1 */

  /* USER CODE END RadioRxError_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;

  com_obj = MBMUXIF_GetRadioFeatureNotifComPtr();
  com_obj->MsgId = RADIO_RX_ERROR_CB_ID;
  com_obj->ParamCnt = 0;
  MBMUXIF_RadioSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN RadioRxError_mbwrapper_2 */

  /* USER CODE END RadioRxError_mbwrapper_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
