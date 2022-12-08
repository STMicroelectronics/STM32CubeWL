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
#include "app_version.h"

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
#ifndef ALIGN_X
#define ALIGN_X( operand, alignment ) ( ((operand + (alignment - 1)) & ~(alignment - 1)) )
#endif /* ALIGN_X */
#define LMH_ON_JOIN_SIZE (ALIGN_X(sizeof(LmHandlerJoinParams_t), 8) + (sizeof(CommissioningParams_t)))
#define LMH_ON_TX_SIZE   (ALIGN_X(sizeof(LmHandlerTxParams_t), 8) + (sizeof(CommissioningParams_t)))
#define LMH_ON_RX_SIZE   (ALIGN_X(sizeof(LmHandlerAppData_t), 8)\
                          + 256 + ALIGN_X(sizeof(LmHandlerRxParams_t), 8) + (sizeof(CommissioningParams_t)))
#define LORA_MBWRAP_SHBUF_SIZE MAX(LMH_ON_JOIN_SIZE, MAX(LMH_ON_TX_SIZE, LMH_ON_RX_SIZE))

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
static uint8_t GetBatteryLevel_mbwrapper(void);

static int16_t GetTemperatureLevel_mbwrapper(void);

static void OnRestoreContextRequest_mbwrapper(void *nvm, uint32_t nvm_size);

static void OnStoreContextRequest_mbwrapper(void *nvm, uint32_t nvm_size);

static void OnNvmDataChange_mbwrapper(LmHandlerNvmContextStates_t state);

static void OnNetworkParametersChange_mbwrapper(CommissioningParams_t *params);

static void OnJoinRequest_mbwrapper(LmHandlerJoinParams_t *joinParams);

static void OnTxData_mbwrapper(LmHandlerTxParams_t *params);

static void OnRxData_mbwrapper(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);

static void OnClassChange_mbwrapper(DeviceClass_t deviceClass);

static void OnBeaconStatusChange_mbwrapper(LmHandlerBeaconParams_t *params);

static void OnSysTimeUpdate_mbwrapper(void);

static void OnTxPeriodicityChanged_mbwrapper(uint32_t periodicity);

static void OnTxFrameCtrlChanged_mbwrapper(LmHandlerMsgTypes_t isTxConfirmed);

static void OnPingSlotPeriodicityChanged_mbwrapper(uint8_t pingSlotPeriodicity);

static void OnSystemReset_mbwrapper(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief LmHandler buffer to exchange data between from CM0+ to CM4
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM3") uint8_t aLoraMbWrapShare2Buffer[LORA_MBWRAP_SHBUF_SIZE];

/**
  * @brief local structure of LmHandler callbacks for notification processes
  */
static LmHandlerCallbacks_t callback_mbwrapper =
{
  .GetBatteryLevel =              GetBatteryLevel_mbwrapper,
  .GetTemperature =               GetTemperatureLevel_mbwrapper,
  .GetUniqueId =                  GetUniqueId,
  .GetDevAddr =                   GetDevAddr,
  .OnRestoreContextRequest =      OnRestoreContextRequest_mbwrapper,
  .OnStoreContextRequest =        OnStoreContextRequest_mbwrapper,
  .OnMacProcess =                 OnMacProcessNotify,
  .OnNvmDataChange =              OnNvmDataChange_mbwrapper,
  .OnNetworkParametersChange =    OnNetworkParametersChange_mbwrapper,
  .OnJoinRequest =                OnJoinRequest_mbwrapper,
  .OnTxData =                     OnTxData_mbwrapper,
  .OnRxData =                     OnRxData_mbwrapper,
  .OnClassChange =                OnClassChange_mbwrapper,
  .OnBeaconStatusChange =         OnBeaconStatusChange_mbwrapper,
  .OnSysTimeUpdate =              OnSysTimeUpdate_mbwrapper,
  .OnTxPeriodicityChanged =       OnTxPeriodicityChanged_mbwrapper,
  .OnTxFrameCtrlChanged =         OnTxFrameCtrlChanged_mbwrapper,
  .OnPingSlotPeriodicityChanged = OnPingSlotPeriodicityChanged_mbwrapper,
  .OnSystemReset =                OnSystemReset_mbwrapper
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
  bool busyStatus;

  APP_LOG(TS_ON, VLEVEL_H, ">CM0PLUS(LoRa)\r\n");

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(ComObj->ParamBuf, ComObj->BufSize);

  /* process Command */
  switch (ComObj->MsgId)
  {
    case   LMHANDLER_INIT_ID:
      errorStatus = LmHandlerInit(&callback_mbwrapper, APP_VERSION);

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      InitPackageProcess();
      break;

    case   LMHANDLER_DEINIT_ID:
      errorStatus = LmHandlerDeInit();

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;

    case   LMHANDLER_CONFIGURE_ID:
      errorStatus = LmHandlerConfigure((LmHandlerParams_t *) com_buffer[0]);

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;

    case   LMHANDLER_JOIN_STATUS_ID:
    {
      LmHandlerFlagStatus_t flagStatus = LmHandlerJoinStatus();

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) flagStatus; /* */
    }
    break;

    case   LMHANDLER_JOIN_ID:
      LmHandlerJoin((ActivationType_t) com_buffer[0], (bool) com_buffer[1]);

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

    case   LMHANDLER_HALT_ID:
      errorStatus = LmHandlerHalt();

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;

    case   LMHANDLER_IS_BUSY_ID:
      busyStatus = LmHandlerIsBusy();

      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (bool) busyStatus; /* */
      break;

    case   LMHANDLER_SEND_ID:
    {
      LmHandlerAppData_t appData;
      appData.Port = (uint8_t)com_buffer[0];
      appData.BufferSize = (uint8_t)com_buffer[1];
      appData.Buffer = (uint8_t *)com_buffer[2];

      errorStatus = LmHandlerSend(&appData, (LmHandlerMsgTypes_t) com_buffer[3], (bool)com_buffer[4]);

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
    case   LMHANDLER_GET_DUTY_CYCLE_TIME_ID:
    {
      TimerTime_t dutyCycleWaitTime;
      dutyCycleWaitTime = LmHandlerGetDutyCycleWaitTime();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) dutyCycleWaitTime; /* */
    }
    break;

    case   LMHANDLER_REQUEST_CLASS_ID:
      errorStatus = LmHandlerRequestClass((DeviceClass_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_CLASS_ID:
      errorStatus = LmHandlerGetCurrentClass((DeviceClass_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_DEVEUI_ID:
      errorStatus = LmHandlerGetDevEUI((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_DEVEUI_ID:
      errorStatus = LmHandlerSetDevEUI((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_APPEUI_ID:
      errorStatus = LmHandlerGetAppEUI((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_APPEUI_ID:
      errorStatus = LmHandlerSetAppEUI((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_KEY_ID:
      errorStatus = LmHandlerGetKey((KeyIdentifier_t) com_buffer[0], (uint8_t *) com_buffer[1]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_KEY_ID:
      errorStatus = LmHandlerSetKey((KeyIdentifier_t) com_buffer[0], (uint8_t *) com_buffer[1]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_NWKID_ID:
      errorStatus = LmHandlerGetNetworkID((uint32_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_NWKID_ID:
      errorStatus = LmHandlerSetNetworkID((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_DEVADDR_ID:
      errorStatus = LmHandlerGetDevAddr((uint32_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_DEVADDR_ID:
      errorStatus = LmHandlerSetDevAddr((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_ACTIVE_REGION_ID:
      errorStatus = LmHandlerGetActiveRegion((LoRaMacRegion_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_ACTIVE_REGION_ID:
      errorStatus = LmHandlerSetActiveRegion((LoRaMacRegion_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_ADR_ENABLE_ID:
      errorStatus = LmHandlerGetAdrEnable((bool *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_ADR_ENABLE_ID:
      errorStatus = LmHandlerSetAdrEnable((bool) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_TX_DATARATE_ID:
      errorStatus = LmHandlerGetTxDatarate((int8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_TX_DATARATE_ID:
      errorStatus = LmHandlerSetTxDatarate((int8_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_DUTY_CYCLE_ID:
      errorStatus = LmHandlerGetDutyCycleEnable((bool *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_DUTY_CYCLE_ID:
      errorStatus = LmHandlerSetDutyCycleEnable((bool) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_RX2_PARAMS_ID:
      errorStatus = LmHandlerGetRX2Params((RxChannelParams_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_TX_PWR_ID:
      errorStatus = LmHandlerGetTxPower((int8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_RX1_DELAY_ID:
      errorStatus = LmHandlerGetRx1Delay((uint32_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_RX2_DELAY_ID:
      errorStatus = LmHandlerGetRx2Delay((uint32_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_JOIN_RX1_DELAY_ID:
      errorStatus = LmHandlerGetJoinRx1Delay((uint32_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_JOIN_RX2_DELAY_ID:
      errorStatus = LmHandlerGetJoinRx2Delay((uint32_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_RX2_PARAMS_ID:
      errorStatus = LmHandlerSetRX2Params((RxChannelParams_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_TX_PWR_ID:
      errorStatus = LmHandlerSetTxPower((int8_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_RX1_DELAY_ID:
      errorStatus = LmHandlerSetRx1Delay((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_RX2_DELAY_ID:
      errorStatus = LmHandlerSetRx2Delay((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_JOIN_RX1_DELAY_ID:
      errorStatus = LmHandlerSetJoinRx1Delay((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_JOIN_RX2_DELAY_ID:
      errorStatus = LmHandlerSetJoinRx2Delay((uint32_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_PING_PERIODICITY_ID:
      errorStatus = LmHandlerGetPingPeriodicity((uint8_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_SET_PING_PERIODICITY_ID:
      errorStatus = LmHandlerSetPingPeriodicity((uint8_t) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case   LMHANDLER_GET_BEACON_STATE_ID:
      errorStatus = LmHandlerGetBeaconState((BeaconState_t *) com_buffer[0]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case LMHANDLER_LINK_CHECK_REQ_ID:
      errorStatus = LmHandlerLinkCheckReq();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case LMHANDLER_DEVICE_TIME_REQ_ID:
      errorStatus = LmHandlerDeviceTimeReq();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case LMHANDLER_GET_VERSION_ID:
      errorStatus = LmHandlerGetVersion((LmHandlerVersionType_t)com_buffer[0], (uint32_t *) com_buffer[1]);
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
      break;
    case LMHANDLER_NVM_DATA_STORE_ID:
      errorStatus = LmHandlerNvmDataStore();
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal = (uint32_t) errorStatus; /* */
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
static int16_t GetTemperatureLevel_mbwrapper(void)
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
  return (int16_t) com_obj->ReturnVal;
  /* USER CODE BEGIN GetTemperatureLevel_mbwrapper_2 */

  /* USER CODE END GetTemperatureLevel_mbwrapper_2 */
}

static void OnRestoreContextRequest_mbwrapper(void *nvm, uint32_t nvm_size)
{
  /* USER CODE BEGIN OnRestoreContextRequest_mbwrapper_1 */

  /* USER CODE END OnRestoreContextRequest_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_RESTORE_CONTEXT_REQ_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  com_buffer[0] = (uint32_t)nvm;
  com_buffer[1] = nvm_size;
  com_obj->ParamCnt = 2;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnRestoreContextRequest_mbwrapper_2 */

  /* USER CODE END OnRestoreContextRequest_mbwrapper_2 */
}

static void OnStoreContextRequest_mbwrapper(void *nvm, uint32_t nvm_size)
{
  /* USER CODE BEGIN OnStoreContextRequest_mbwrapper_1 */

  /* USER CODE END OnStoreContextRequest_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_STORE_CONTEXT_REQ_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  com_buffer[0] = (uint32_t)nvm;
  com_buffer[1] = nvm_size;
  com_obj->ParamCnt = 2;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnStoreContextRequest_mbwrapper_2 */

  /* USER CODE END OnStoreContextRequest_mbwrapper_2 */
}

static void OnNvmDataChange_mbwrapper(LmHandlerNvmContextStates_t state)
{
  /* USER CODE BEGIN OnNvmDataChange_mbwrapper_1 */

  /* USER CODE END OnNvmDataChange_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_NVM_DATA_CHANGE_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  com_buffer[0] = (uint32_t)state;
  com_obj->ParamCnt = 1;

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
  uint32_t next_addr = 0;

  if (joinParams != NULL)
  {
    UTIL_MEM_cpy_8(aLoraMbWrapShare2Buffer, joinParams, sizeof(LmHandlerJoinParams_t));
    /* need to 8-align the next address */
    next_addr += ALIGN_X(sizeof(LmHandlerJoinParams_t), 8);
    if (joinParams->CommissioningParams != NULL)
    {
      UTIL_MEM_cpy_8(&aLoraMbWrapShare2Buffer[next_addr], joinParams->CommissioningParams, sizeof(CommissioningParams_t));
      ((LmHandlerJoinParams_t *)aLoraMbWrapShare2Buffer)->CommissioningParams = (CommissioningParams_t *)&aLoraMbWrapShare2Buffer[next_addr];
    }
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
  uint32_t next_addr = 0;

  if (params != NULL)
  {
    UTIL_MEM_cpy_8(aLoraMbWrapShare2Buffer, params, sizeof(LmHandlerTxParams_t));
    /* need to 8-align the next address */
    next_addr += ALIGN_X(sizeof(LmHandlerTxParams_t), 8);
    if (params->CommissioningParams != NULL)
    {
      UTIL_MEM_cpy_8(&aLoraMbWrapShare2Buffer[next_addr], params->CommissioningParams, sizeof(CommissioningParams_t));
      ((LmHandlerTxParams_t *)aLoraMbWrapShare2Buffer)->CommissioningParams = (CommissioningParams_t *)&aLoraMbWrapShare2Buffer[next_addr];
    }
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
  uint32_t next_addr = 0;
  uint32_t param_addr = 0;

  if (appData != NULL)
  {
    UTIL_MEM_cpy_8(aLoraMbWrapShare2Buffer, appData, sizeof(LmHandlerAppData_t));
    /* need to 8-align the next address */
    next_addr += ALIGN_X(sizeof(LmHandlerAppData_t), 8);

    if (appData->BufferSize > 0)
    {
      UTIL_MEM_cpy_8(&aLoraMbWrapShare2Buffer[next_addr], appData->Buffer, appData->BufferSize);
      ((LmHandlerAppData_t *)aLoraMbWrapShare2Buffer)->Buffer = &aLoraMbWrapShare2Buffer[next_addr];
      /* need to 8-align the next address */
      next_addr += ALIGN_X(appData->BufferSize, 8);
    }
  }

  if (params != NULL)
  {
    UTIL_MEM_cpy_8(&aLoraMbWrapShare2Buffer[next_addr], params, sizeof(LmHandlerRxParams_t));
    param_addr = next_addr;
    /* need to 8-align the next address */
    next_addr += ALIGN_X(sizeof(CommissioningParams_t), 8);
    if (params->CommissioningParams != NULL)
    {
      UTIL_MEM_cpy_8(&aLoraMbWrapShare2Buffer[next_addr], params->CommissioningParams, sizeof(CommissioningParams_t));
      ((LmHandlerRxParams_t *)&aLoraMbWrapShare2Buffer[param_addr])->CommissioningParams = (CommissioningParams_t *)&aLoraMbWrapShare2Buffer[next_addr];
    }
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
    com_buffer[1] = (uint32_t) &aLoraMbWrapShare2Buffer[param_addr];
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
  /* USER CODE BEGIN OnSysTimeUpdate_mbwrapper_1 */

  /* USER CODE END OnSysTimeUpdate_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_SYS_TIME_UPDATE_CB_ID;
  com_obj->ParamCnt = 0;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnSysTimeUpdate_mbwrapper_2 */

  /* USER CODE END OnSysTimeUpdate_mbwrapper_2 */
}

static void OnTxPeriodicityChanged_mbwrapper(uint32_t periodicity)
{
  /* USER CODE BEGIN OnTxPeriodicityChanged_mbwrapper_1 */

  /* USER CODE END OnTxPeriodicityChanged_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_TX_PERIOD_CHANGED_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  com_buffer[0] = periodicity;
  com_obj->ParamCnt = 1;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnTxPeriodicityChanged_mbwrapper_2 */

  /* USER CODE END OnTxPeriodicityChanged_mbwrapper_2 */
}

static void OnTxFrameCtrlChanged_mbwrapper(LmHandlerMsgTypes_t isTxConfirmed)
{
  /* USER CODE BEGIN OnTxFrameCtrlChanged_mbwrapper_1 */

  /* USER CODE END OnTxFrameCtrlChanged_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_TX_CTRL_CHANGED_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  com_buffer[0] = (uint32_t)isTxConfirmed;
  com_obj->ParamCnt = 1;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnTxFrameCtrlChanged_mbwrapper_2 */

  /* USER CODE END OnTxFrameCtrlChanged_mbwrapper_2 */
}
static void OnPingSlotPeriodicityChanged_mbwrapper(uint8_t pingSlotPeriodicity)
{
  /* USER CODE BEGIN OnPingSlotPeriodicityChanged_mbwrapper_1 */

  /* USER CODE END OnPingSlotPeriodicityChanged_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer = NULL;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_PING_SLOT_PERIOD_CHANGED_CB_ID;

  com_buffer = MBMUX_SEC_VerifySramBufferPtr(com_obj->ParamBuf, com_obj->BufSize);
  com_buffer[0] = (uint32_t)pingSlotPeriodicity;
  com_obj->ParamCnt = 1;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnPingSlotPeriodicityChanged_mbwrapper_2 */

  /* USER CODE END OnPingSlotPeriodicityChanged_mbwrapper_2 */
}

static void OnSystemReset_mbwrapper(void)
{
  /* USER CODE BEGIN OnSystemReset_mbwrapper_1 */

  /* USER CODE END OnSystemReset_mbwrapper_1 */
  MBMUX_ComParam_t *com_obj;

  com_obj = MBMUXIF_GetLoraFeatureNotifComPtr();
  com_obj->MsgId = LMHANDLER_ON_SYSTEM_RESET_CB_ID;
  com_obj->ParamCnt = 0;

  MBMUXIF_LoraSendNotif();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN OnSystemReset_mbwrapper_2 */

  /* USER CODE END OnSystemReset_mbwrapper_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
