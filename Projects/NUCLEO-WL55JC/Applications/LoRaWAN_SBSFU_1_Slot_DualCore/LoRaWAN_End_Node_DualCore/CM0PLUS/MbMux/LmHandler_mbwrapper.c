/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    LmHandler_mbwrapper.c
  * @author  MCD Application Team
  * @brief   This file implements the CM0PLUS side wrapper of the LoraMac interface
  *          shared between M0 and M4.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32_mem.h"
#include "mbmux_table.h"
#include "msg_id.h"
#include "mbmux.h"
#include "mbmuxif_lora.h"
#include "sys_app.h"
#include "lora_app.h"

#include "LmHandler_mbwrapper.h"
#include "LmHandler.h"
#include "lora_info.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/*!
 * LoRa State Machine states
 */
typedef enum TxEventType_e
{
  /*!
   * @brief Appdata Transmission issue based on timer every TxDutyCycleTime
   */
  TX_ON_TIMER,
  /*!
   * @brief Appdata Transmission external event plugged on OnSendEvent( )
   */
  TX_ON_EVENT
} TxEventType_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#ifndef MAX
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif /* MAX */
#define LMH_ON_JOIN_SIZE (sizeof(LmHandlerJoinParams_t))
#define LMH_ON_TX_SIZE   (sizeof(LmHandlerTxParams_t))
#define LMH_ON_RX_SIZE   (((sizeof(LmHandlerAppData_t) + 7) & ~7) + 256 + (sizeof(LmHandlerRxParams_t)))
#define LORA_MBWRAP_SHBUF_SIZE MAX(LMH_ON_JOIN_SIZE, MAX(LMH_ON_TX_SIZE, LMH_ON_RX_SIZE))

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
static uint8_t GetBatteryLevel_mbwrapper(void);

static uint16_t GetTemperatureLevel_mbwrapper(void);

static void OnNvmDataChange_mbwrapper(LmHandlerNvmContextStates_t state, uint16_t size);

static void OnNetworkParametersChange_mbwrapper(CommissioningParams_t *params);

static void OnJoinRequest_mbwrapper(LmHandlerJoinParams_t *joinParams);

static void OnTxData_mbwrapper(LmHandlerTxParams_t *params);

static void OnRxData_mbwrapper(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);

static void OnClassChange_mbwrapper(DeviceClass_t deviceClass);

static void OnBeaconStatusChange_mbwrapper(LmHandlerBeaconParams_t *params);

static void OnSysTimeUpdate_mbwrapper(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief LmHandler buffer to exchange data between from CM0+ to CM4
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM2") uint8_t aLoraMbWrapShare2Buffer[LORA_MBWRAP_SHBUF_SIZE];

/**
  * @brief local structure of LmHandler callbacks for notification processes
  */
static LmHandlerCallbacks_t callback_mbwrapper =
{
  .GetBatteryLevel =           GetBatteryLevel_mbwrapper,
  .GetTemperature =            GetTemperatureLevel_mbwrapper,
  .GetUniqueId =               GetUniqueId,
  .GetDevAddr =                GetDevAddr,
  .OnMacProcess =              OnMacProcessNotify,
  .OnNvmDataChange =           OnNvmDataChange_mbwrapper,
  .OnNetworkParametersChange = OnNetworkParametersChange_mbwrapper,
  .OnJoinRequest =             OnJoinRequest_mbwrapper,
  .OnTxData =                  OnTxData_mbwrapper,
  .OnRxData =                  OnRxData_mbwrapper,
  .OnClassChange =             OnClassChange_mbwrapper,
  .OnBeaconStatusChange =      OnBeaconStatusChange_mbwrapper,
  .OnSysTimeUpdate =           OnSysTimeUpdate_mbwrapper
};

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Exported functions ---------------------------------------------------------*/
void Process_Lora_Cmd(MBMUX_ComParam_t *ComObj)
{
  /* USER CODE BEGIN Process_Lora_Cmd_1 */

  /* USER CODE END Process_Lora_Cmd_1 */
  uint32_t *com_buffer = NULL;
  LmHandlerErrorStatus_t errorStatus;
  LmHandlerFlagStatus_t flagStatus;
  int32_t ret_code;

  APP_LOG(TS_ON, VLEVEL_H, ">CM0PLUS(LoRa)\r\n");

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(ComObj->ParamBuf, ComObj->BufSize);

  /* process Command */
  switch (ComObj->MsgId)
  {
    case   LMHANDLER_INITIALIZATION_ID:
      errorStatus = LmHandlerInit(&callback_mbwrapper);

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      InitPackageProcess();
      break;

    case   LMHANDLER_CONFIGURE_ID:
      errorStatus = LmHandlerConfigure((LmHandlerParams_t *) com_buffer[0]);

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;

    case   LMHANDLER_JOIN_STATUS_ID:
      flagStatus = LmHandlerJoinStatus();

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) flagStatus; /* */
      break;

    case   LMHANDLER_JOIN_ID:
      LmHandlerJoin((ActivationType_t) com_buffer[0]);

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    case   LMHANDLER_STOP_ID:
      errorStatus = LmHandlerStop();

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;

    case   LMHANDLER_SEND_ID:
    {
      LmHandlerAppData_t appData;
      appData.Port = (uint8_t)com_buffer[0];
      appData.BufferSize = (uint8_t)com_buffer[1];
      appData.Buffer = (uint8_t *)com_buffer[2];

      errorStatus = LmHandlerSend(&appData, (LmHandlerMsgTypes_t) com_buffer[3], (UTIL_TIMER_Time_t *)com_buffer[4], (bool)com_buffer[5]);

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
    }
    break;
    case   LMHANDLER_PROCESS_ID:
      LmHandlerProcess();

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;
    case   LMHANDLER_REQUEST_CLASS_ID:
      errorStatus = LmHandlerRequestClass((DeviceClass_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_CLASS_ID:
      ret_code = LmHandlerGetCurrentClass((DeviceClass_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_DEVEUI_ID:
      ret_code = LmHandlerGetDevEUI((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_DEVEUI_ID:
      ret_code = LmHandlerSetDevEUI((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_APPEUI_ID:
      ret_code = LmHandlerGetAppEUI((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_APPEUI_ID:
      ret_code = LmHandlerSetAppEUI((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_NWKKEY_ID:
      ret_code = LmHandlerGetNwkKey((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_NWKKEY_ID:
      ret_code = LmHandlerSetNwkKey((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_APPKEY_ID:
      ret_code = LmHandlerGetAppKey((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_APPKEY_ID:
      ret_code = LmHandlerSetAppKey((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_NWKSKEY_ID:
      ret_code = LmHandlerGetNwkSKey((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_NWKSKEY_ID:
      ret_code = LmHandlerSetNwkSKey((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_APPSKEY_ID:
      ret_code = LmHandlerGetAppSKey((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_APPSKEY_ID:
      ret_code = LmHandlerSetAppSKey((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_NWKID_ID:
      ret_code = LmHandlerGetNetworkID((uint32_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_NWKID_ID:
      ret_code = LmHandlerSetNetworkID((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_DEVADDR_ID:
      ret_code = LmHandlerGetDevAddr((uint32_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_DEVADDR_ID:
      ret_code = LmHandlerSetDevAddr((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_ACTIVE_REGION_ID:
      ret_code = LmHandlerGetActiveRegion((LoRaMacRegion_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_ACTIVE_REGION_ID:
      ret_code = LmHandlerSetActiveRegion((LoRaMacRegion_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_ADR_ENABLE_ID:
      ret_code = LmHandlerGetAdrEnable((bool *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_ADR_ENABLE_ID:
      ret_code = LmHandlerSetAdrEnable((bool) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_TX_DATARATE_ID:
      ret_code = LmHandlerGetTxDatarate((int8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_TX_DATARATE_ID:
      ret_code = LmHandlerSetTxDatarate((int8_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_DUTY_CYCLE_ID:
      ret_code = LmHandlerGetDutyCycleEnable((bool *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_DUTY_CYCLE_ID:
      ret_code = LmHandlerSetDutyCycleEnable((bool) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_RX2_PARAMS_ID:
      ret_code = LmHandlerGetRX2Params((RxChannelParams_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_TX_PWR_ID:
      ret_code = LmHandlerGetTxPower((int8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_RX1_DELAY_ID:
      ret_code = LmHandlerGetRx1Delay((uint32_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_RX2_DELAY_ID:
      ret_code = LmHandlerGetRx2Delay((uint32_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_JOIN_RX1_DELAY_ID:
      ret_code = LmHandlerGetJoinRx1Delay((uint32_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_JOIN_RX2_DELAY_ID:
      ret_code = LmHandlerGetJoinRx2Delay((uint32_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_RX2_PARAMS_ID:
      ret_code = LmHandlerSetRX2Params((RxChannelParams_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_TX_PWR_ID:
      ret_code = LmHandlerSetTxPower((int8_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_RX1_DELAY_ID:
      ret_code = LmHandlerSetRx1Delay((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_RX2_DELAY_ID:
      ret_code = LmHandlerSetRx2Delay((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_JOIN_RX1_DELAY_ID:
      ret_code = LmHandlerSetJoinRx1Delay((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_JOIN_RX2_DELAY_ID:
      ret_code = LmHandlerSetJoinRx2Delay((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_PING_PERIODICITY_ID:
      ret_code = LmHandlerGetPingPeriodicity((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_SET_PING_PERIODICITY_ID:
      ret_code = LmHandlerSetPingPeriodicity((uint8_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_GET_BEACON_STATE_ID:
      ret_code = LmHandlerGetBeaconState((BeaconState_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case LMHANDLER_LINK_CHECK_REQ_ID:
      ret_code = LmHandlerLinkCheckReq();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case LMHANDLER_DEVICE_TIME_REQ_ID:
      ret_code = LmHandlerDeviceTimeReq();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) ret_code; /* */
      break;
    case   LMHANDLER_INFO_INIT_ID:
      LoraInfo_Init();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = 0; /* */
      break;

    default:
      break;
  }

  /* send Response */
  APP_LOG(TS_ON, VLEVEL_H, "<CM0PLUS(LoRa)\r\n");
  MBMUX_ResponseSnd(FEAT_INFO_LORAWAN_ID);
  /* USER CODE BEGIN Process_Lora_Cmd_2 */

  /* USER CODE END Process_Lora_Cmd_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/*!
 * \brief   Measures the battery level
 *
 * \retval  Battery level [0: node is connected to an external
 *          power source, 1..254: battery level, where 1 is the minimum
 *          and 254 is the maximum value, 255: the node was not able
 *          to measure the battery level]
 */

static uint8_t GetBatteryLevel_mbwrapper(void)
{
  /* USER CODE BEGIN GetBatteryLevel_mbwrapper_1 */

  /* USER CODE END GetBatteryLevel_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_GET_BATTERY_LEVEL_CB_ID;
  com_obj->ParamCnt = 0;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return (uint8_t) com_obj->ReturnVal;
  /* USER CODE BEGIN GetBatteryLevel_mbwrapper_2 */

  /* USER CODE END GetBatteryLevel_mbwrapper_2 */
}

/*!
 * \brief   Measures the temperature level
 *
 * \retval  Temperature level
 */
static uint16_t GetTemperatureLevel_mbwrapper(void)
{
  /* USER CODE BEGIN GetTemperatureLevel_mbwrapper_1 */

  /* USER CODE END GetTemperatureLevel_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_GET_TEMPERATURE_CB_ID;
  com_obj->ParamCnt = 0;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return (uint16_t) com_obj->ReturnVal;
  /* USER CODE BEGIN GetTemperatureLevel_mbwrapper_2 */

  /* USER CODE END GetTemperatureLevel_mbwrapper_2 */
}

static void OnNvmDataChange_mbwrapper(LmHandlerNvmContextStates_t state, uint16_t size)
{
  /* USER CODE BEGIN OnNvmDataChange_mbwrapper_1 */

  /* USER CODE END OnNvmDataChange_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_NVM_DATA_CHANGE_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  com_buffer[0] = (uint32_t)state;
  com_buffer[1] = (uint32_t)size;
  com_obj->ParamCnt = 2;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnNvmDataChange_mbwrapper_2 */

  /* USER CODE END OnNvmDataChange_mbwrapper_2 */
}

static void OnNetworkParametersChange_mbwrapper(CommissioningParams_t *params)
{
  /* USER CODE BEGIN OnNetworkParametersChange_mbwrapper_1 */

  /* USER CODE END OnNetworkParametersChange_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;

  if (params != NULL)
  {
    UTIL_MEM_cpy_8(aLoraMbWrapShare2Buffer, params, sizeof(CommissioningParams_t));
  }

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_NETWORK_PARAMS_CHANGE_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  if (params == NULL)
  {
    com_buffer[0] = (uint32_t)NULL;
  }
  else
  {
    com_buffer[0] = (uint32_t) aLoraMbWrapShare2Buffer;
  }
  com_obj->ParamCnt = 1;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnNetworkParametersChange_mbwrapper_2 */

  /* USER CODE END OnNetworkParametersChange_mbwrapper_2 */
}

static void OnJoinRequest_mbwrapper(LmHandlerJoinParams_t *joinParams)
{
  /* USER CODE BEGIN OnJoinRequest_mbwrapper_1 */

  /* USER CODE END OnJoinRequest_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;

  if (joinParams != NULL)
  {
    UTIL_MEM_cpy_8(aLoraMbWrapShare2Buffer, joinParams, sizeof(LmHandlerJoinParams_t));
  }

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_JOIN_REQ_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  if (joinParams == NULL)
  {
    com_buffer[0] = (uint32_t)NULL;
  }
  else
  {
    com_buffer[0] = (uint32_t) aLoraMbWrapShare2Buffer;
  }
  com_obj->ParamCnt = 1;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnJoinRequest_mbwrapper_2 */

  /* USER CODE END OnJoinRequest_mbwrapper_2 */
}

static void OnTxData_mbwrapper(LmHandlerTxParams_t *params)
{
  /* USER CODE BEGIN OnTxData_mbwrapper_1 */

  /* USER CODE END OnTxData_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;

  if (params != NULL)
  {
    UTIL_MEM_cpy_8(aLoraMbWrapShare2Buffer, params, sizeof(LmHandlerTxParams_t));
  }

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_TX_DATA_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);

  if (params == NULL)
  {
    com_buffer[0] = (uint32_t)NULL;
  }
  else
  {
    com_buffer[0] = (uint32_t) aLoraMbWrapShare2Buffer;
  }

  com_obj->ParamCnt = 1;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnTxData_mbwrapper_2 */

  /* USER CODE END OnTxData_mbwrapper_2 */
}

static void OnRxData_mbwrapper(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
  /* USER CODE BEGIN OnRxData_mbwrapper_1 */

  /* USER CODE END OnRxData_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;
  uint8_t next_addr = 0;

  if (appData != NULL)
  {
    UTIL_MEM_cpy_8(aLoraMbWrapShare2Buffer, appData, sizeof(LmHandlerAppData_t));
    /* need to 8-align the next address */
    next_addr = (sizeof(LmHandlerAppData_t) + 7) & ~7;

    if (appData->BufferSize > 0)
    {
      UTIL_MEM_cpy_8(&aLoraMbWrapShare2Buffer[next_addr], appData->Buffer, appData->BufferSize);
      ((LmHandlerAppData_t *)aLoraMbWrapShare2Buffer)->Buffer = &aLoraMbWrapShare2Buffer[next_addr];
      next_addr = (appData->BufferSize + 7) & ~7;
    }
  }

  if (params != NULL)
  {
    UTIL_MEM_cpy_8(&aLoraMbWrapShare2Buffer[next_addr], params, sizeof(LmHandlerRxParams_t));
  }

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_RX_DATA_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  if (appData == NULL)
  {
    com_buffer[0] = (uint32_t)NULL;
  }
  else
  {
    com_buffer[0] = (uint32_t) aLoraMbWrapShare2Buffer;
  }

  if (params == NULL)
  {
    com_buffer[1] = (uint32_t)NULL;
  }
  else
  {
    com_buffer[1] = (uint32_t) &aLoraMbWrapShare2Buffer[next_addr];
  }

  com_obj->ParamCnt = 2;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnRxData_mbwrapper_2 */

  /* USER CODE END OnRxData_mbwrapper_2 */
}

static void OnClassChange_mbwrapper(DeviceClass_t deviceClass)
{
  /* USER CODE BEGIN OnClassChange_mbwrapper_1 */

  /* USER CODE END OnClassChange_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_CLASS_CHANGE_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  com_buffer[0] = (uint32_t)deviceClass;
  com_obj->ParamCnt = 1;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnClassChange_mbwrapper_2 */

  /* USER CODE END OnClassChange_mbwrapper_2 */
}

static void OnBeaconStatusChange_mbwrapper(LmHandlerBeaconParams_t *params)
{
  /* USER CODE BEGIN OnBeaconStatusChange_mbwrapper_1 */

  /* USER CODE END OnBeaconStatusChange_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;

  if (params != NULL)
  {
    UTIL_MEM_cpy_8(aLoraMbWrapShare2Buffer, params, sizeof(LmHandlerBeaconParams_t));
  }

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_BEACON_STATUS_CHANGE_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  if (params == NULL)
  {
    com_buffer[0] = (uint32_t)NULL;
  }
  else
  {
    com_buffer[0] = (uint32_t) aLoraMbWrapShare2Buffer;
  }
  com_obj->ParamCnt = 1;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnBeaconStatusChange_mbwrapper_2 */

  /* USER CODE END OnBeaconStatusChange_mbwrapper_2 */
}

static void OnSysTimeUpdate_mbwrapper(void)
{
  /* USER CODE BEGIN OnBeaconStatusChange_mbwrapper_1 */

  /* USER CODE END OnBeaconStatusChange_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_ON_SYS_TIME_UPDATE_CB_ID;
  com_obj->ParamCnt = 0;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnBeaconStatusChange_mbwrapper_2 */

  /* USER CODE END OnBeaconStatusChange_mbwrapper_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
