/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    LmHandler_mbwrapper.c
  * @author  MCD Application Team
  * @brief   This file implements the CM4 side wrapper of the LoraMac interface
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
#include "platform.h"
#include "LmHandler_mbwrapper.h"
#include "lora_info.h"
#include "LmHandler.h"
#include "mbmuxif_sys.h"
#include "mbmux.h"
#include "msg_id.h"
#include "mbmuxif_lora.h"
#include "stm32_mem.h"
#include "sys_app.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#ifndef MAX
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif /* MAX */
#define LMH_CONFIG_SIZE     (sizeof(LmHandlerParams_t))
#define LMH_SEND_SIZE       (256 + sizeof(UTIL_TIMER_Time_t))
#define LMH_GET_CLASS_SIZE  (sizeof(DeviceClass_t))
#define LMH_EUI_SIZE        (SE_EUI_SIZE)
#define LMH_KEY_SIZE        (SE_KEY_SIZE)
#define LMH_REGION_SIZE     (sizeof(LoRaMacRegion_t))
#define LMH_RX_PARAM_SIZE   (sizeof(RxChannelParams_t))
#define LMH_BEACON_SIZE     (sizeof(BeaconState_t))
#define LORA_MBWRAP_SHBUF_SIZE MAX(MAX(MAX(LMH_CONFIG_SIZE, LMH_SEND_SIZE), MAX(LMH_GET_CLASS_SIZE, LMH_EUI_SIZE)), MAX(MAX(LMH_KEY_SIZE, LMH_REGION_SIZE), MAX(LMH_RX_PARAM_SIZE, LMH_BEACON_SIZE)))

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief LmHandler buffer to exchange data between from CM4 to CM0+
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint8_t aLoraMbWrapShareBuffer[LORA_MBWRAP_SHBUF_SIZE];

/**
  * @brief local structure of LmHandler callbacks for notification processes
  */
static LmHandlerCallbacks_t callback_mbwrapper;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
/**
  * @addtogroup LoRaWan  LoRaWan Interface
  *
  * @{
  *
  */
LmHandlerErrorStatus_t LmHandlerInit(LmHandlerCallbacks_t *handlerCallbacks)
{
  /* USER CODE BEGIN LmHandlerInit_1 */

  /* USER CODE END LmHandlerInit_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer ;
  uint16_t i = 0;
  uint32_t ret;

  if (handlerCallbacks == NULL)
  {
    return LORAMAC_HANDLER_ERROR;
  }

  callback_mbwrapper.GetBatteryLevel = handlerCallbacks->GetBatteryLevel;
  callback_mbwrapper.GetTemperature = handlerCallbacks->GetTemperature;
  callback_mbwrapper.OnNvmDataChange = handlerCallbacks->OnNvmDataChange;
  callback_mbwrapper.OnNetworkParametersChange = handlerCallbacks->OnNetworkParametersChange;
  callback_mbwrapper.OnJoinRequest = handlerCallbacks->OnJoinRequest;
  callback_mbwrapper.OnTxData = handlerCallbacks->OnTxData;
  callback_mbwrapper.OnRxData = handlerCallbacks->OnRxData;
  callback_mbwrapper.OnClassChange = handlerCallbacks->OnClassChange;
  callback_mbwrapper.OnBeaconStatusChange = handlerCallbacks->OnBeaconStatusChange;
  callback_mbwrapper.OnSysTimeUpdate = handlerCallbacks->OnSysTimeUpdate;

  com_obj = MBMUXIF_GetLoraFeatureCmdComPtr();
  com_obj->MsgId = LMHANDLER_INITIALIZATION_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) aLoraMbWrapShareBuffer;
  com_obj->ParamCnt = i;

  MBMUXIF_LoraSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */
  ret = com_obj->ReturnVal;
  return (LmHandlerErrorStatus_t) ret;
  /* USER CODE BEGIN LmHandlerInit_2 */

  /* USER CODE END LmHandlerInit_2 */
}

LmHandlerErrorStatus_t LmHandlerConfigure(LmHandlerParams_t *handlerParams)
{
  /* USER CODE BEGIN LmHandlerConfigure_1 */

  /* USER CODE END LmHandlerConfigure_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer ;
  uint16_t i = 0;
  uint32_t ret;

  if (handlerParams == NULL)
  {
    return LORAMAC_HANDLER_ERROR;
  }

  /* copy data from Cm4 stack memory to shared memory */
  UTIL_MEM_cpy_8(aLoraMbWrapShareBuffer, handlerParams, sizeof(LmHandlerParams_t));

  com_obj = MBMUXIF_GetLoraFeatureCmdComPtr();
  com_obj->MsgId = LMHANDLER_CONFIGURE_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) aLoraMbWrapShareBuffer;
  com_obj->ParamCnt = i;

  MBMUXIF_LoraSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */
  ret = com_obj->ReturnVal;
  return (LmHandlerErrorStatus_t) ret;
  /* USER CODE BEGIN LmHandlerConfigure_2 */

  /* USER CODE END LmHandlerConfigure_2 */
}

LmHandlerFlagStatus_t LmHandlerJoinStatus(void)
{
  /* USER CODE BEGIN LmHandlerJoinStatus_1 */

  /* USER CODE END LmHandlerJoinStatus_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t ret;

  com_obj = MBMUXIF_GetLoraFeatureCmdComPtr();
  com_obj->MsgId = LMHANDLER_JOIN_STATUS_ID;
  com_obj->ParamCnt = 0;

  MBMUXIF_LoraSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */
  ret = com_obj->ReturnVal;
  return (LmHandlerFlagStatus_t) ret;
  /* USER CODE BEGIN LmHandlerJoinStatus_2 */

  /* USER CODE END LmHandlerJoinStatus_2 */
}

void LmHandlerJoin(ActivationType_t mode)
{
  /* USER CODE BEGIN LmHandlerJoin_1 */

  /* USER CODE END LmHandlerJoin_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer ;
  uint16_t i = 0;

  com_obj = MBMUXIF_GetLoraFeatureCmdComPtr();
  com_obj->MsgId = LMHANDLER_JOIN_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) mode;
  com_obj->ParamCnt = i;

  MBMUXIF_LoraSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN LmHandlerJoin_2 */

  /* USER CODE END LmHandlerJoin_2 */
}

LmHandlerErrorStatus_t LmHandlerStop(void)
{
  /* USER CODE BEGIN LmHandlerStop_1 */

  /* USER CODE END LmHandlerStop_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t ret;

  com_obj = MBMUXIF_GetLoraFeatureCmdComPtr();
  com_obj->MsgId = LMHANDLER_STOP_ID;
  com_obj->ParamCnt = 0;

  MBMUXIF_LoraSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */
  ret = com_obj->ReturnVal;
  return (LmHandlerErrorStatus_t) ret;
  /* USER CODE BEGIN LmHandlerStop_2 */

  /* USER CODE END LmHandlerStop_2 */
}

LmHandlerErrorStatus_t LmHandlerSend(LmHandlerAppData_t *appData, LmHandlerMsgTypes_t isTxConfirmed,
                                     UTIL_TIMER_Time_t *nextTxIn, bool allowDelayedTx)
{
  /* USER CODE BEGIN LmHandlerSend_1 */

  /* USER CODE END LmHandlerSend_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer ;
  uint16_t i = 0;
  uint32_t ret;
  uint8_t next_addr = 0;

  if ((appData == NULL) || (appData->Buffer == NULL))
  {
    return LORAMAC_HANDLER_ERROR;
  }

  /* copy data from Cm4 stack memory to shared memory */
  if (appData->BufferSize > 0)
  {
    UTIL_MEM_cpy_8(aLoraMbWrapShareBuffer, appData->Buffer, appData->BufferSize);
    /* need to 8-align the next address */
    next_addr = (appData->BufferSize + 7) & ~7;
  }

  if (nextTxIn != NULL)
  {
    UTIL_MEM_cpy_8(&aLoraMbWrapShareBuffer[next_addr], nextTxIn, sizeof(UTIL_TIMER_Time_t));
  }

  com_obj = MBMUXIF_GetLoraFeatureCmdComPtr();
  com_obj->MsgId = LMHANDLER_SEND_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) appData->Port;
  com_buffer[i++] = (uint32_t) appData->BufferSize;
  com_buffer[i++] = (uint32_t) aLoraMbWrapShareBuffer;
  com_buffer[i++] = (uint32_t) isTxConfirmed;
  if (nextTxIn == NULL)
  {
    com_buffer[i++] = (uint32_t)NULL;
  }
  else
  {
    com_buffer[i++] = (uint32_t) &aLoraMbWrapShareBuffer[next_addr];
  }
  com_buffer[i++] = (uint32_t) allowDelayedTx;
  com_obj->ParamCnt = i;

  MBMUXIF_LoraSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */
  if (nextTxIn != NULL)
  {
    UTIL_MEM_cpy_8(nextTxIn, &aLoraMbWrapShareBuffer[next_addr], sizeof(UTIL_TIMER_Time_t));
  }

  ret = com_obj->ReturnVal;
  return (LmHandlerErrorStatus_t) ret;
  /* USER CODE BEGIN LmHandlerSend_2 */

  /* USER CODE END LmHandlerSend_2 */
}

void LmHandlerProcess(void)
{
  /* USER CODE BEGIN LmHandlerProcess_1 */

  /* USER CODE END LmHandlerProcess_1 */
  MBMUX_ComParam_t *com_obj;

  com_obj = MBMUXIF_GetLoraFeatureCmdComPtr();
  com_obj->MsgId = LMHANDLER_PROCESS_ID;

  MBMUXIF_LoraSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN LmHandlerProcess_2 */

  /* USER CODE END LmHandlerProcess_2 */
}

LmHandlerErrorStatus_t LmHandlerRequestClass(DeviceClass_t newClass)
{
  /* USER CODE BEGIN LmHandlerRequestClass_1 */

  /* USER CODE END LmHandlerRequestClass_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerRequestClass_2 */

  /* USER CODE END LmHandlerRequestClass_2 */
}

LmHandlerErrorStatus_t LmHandlerGetCurrentClass(DeviceClass_t *deviceClass)
{
  /* USER CODE BEGIN LmHandlerGetCurrentClass_1 */

  /* USER CODE END LmHandlerGetCurrentClass_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetCurrentClass_2 */

  /* USER CODE END LmHandlerGetCurrentClass_2 */
}

LmHandlerErrorStatus_t LmHandlerGetDevEUI(uint8_t *devEUI)
{
  /* USER CODE BEGIN LmHandlerGetDevEUI_1 */

  /* USER CODE END LmHandlerGetDevEUI_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetDevEUI_2 */

  /* USER CODE END LmHandlerGetDevEUI_2 */
}

LmHandlerErrorStatus_t LmHandlerSetDevEUI(uint8_t *devEUI)
{
  /* USER CODE BEGIN LmHandlerSetDevEUI_1 */

  /* USER CODE END LmHandlerSetDevEUI_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetDevEUI_2 */

  /* USER CODE END LmHandlerSetDevEUI_2 */
}

LmHandlerErrorStatus_t LmHandlerGetAppEUI(uint8_t *appEUI)
{
  /* USER CODE BEGIN LmHandlerGetAppEUI_1 */

  /* USER CODE END LmHandlerGetAppEUI_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetAppEUI_2 */

  /* USER CODE END LmHandlerGetAppEUI_2 */
}

LmHandlerErrorStatus_t LmHandlerSetAppEUI(uint8_t *appEUI)
{
  /* USER CODE BEGIN LmHandlerSetAppEUI_1 */

  /* USER CODE END LmHandlerSetAppEUI_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetAppEUI_2 */

  /* USER CODE END LmHandlerSetAppEUI_2 */
}

LmHandlerErrorStatus_t LmHandlerGetNwkKey(uint8_t *nwkKey)
{
  /* USER CODE BEGIN LmHandlerGetNwkKey_1 */

  /* USER CODE END LmHandlerGetNwkKey_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetNwkKey_2 */

  /* USER CODE END LmHandlerGetNwkKey_2 */
}

LmHandlerErrorStatus_t LmHandlerSetNwkKey(uint8_t *nwkKey)
{
  /* USER CODE BEGIN LmHandlerSetNwkKey_1 */

  /* USER CODE END LmHandlerSetNwkKey_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetNwkKey_2 */

  /* USER CODE END LmHandlerSetNwkKey_2 */
}

LmHandlerErrorStatus_t LmHandlerGetAppKey(uint8_t *appKey)
{
  /* USER CODE BEGIN LmHandlerGetAppKey_1 */

  /* USER CODE END LmHandlerGetAppKey_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetAppKey_2 */

  /* USER CODE END LmHandlerGetAppKey_2 */
}

LmHandlerErrorStatus_t LmHandlerSetAppKey(uint8_t *appKey)
{
  /* USER CODE BEGIN LmHandlerSetAppKey_1 */

  /* USER CODE END LmHandlerSetAppKey_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetAppKey_2 */

  /* USER CODE END LmHandlerSetAppKey_2 */
}

LmHandlerErrorStatus_t LmHandlerGetNwkSKey(uint8_t *nwkSKey)
{
  /* USER CODE BEGIN LmHandlerGetNwkSKey_1 */

  /* USER CODE END LmHandlerGetNwkSKey_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetNwkSKey_2 */

  /* USER CODE END LmHandlerGetNwkSKey_2 */
}

LmHandlerErrorStatus_t LmHandlerSetNwkSKey(uint8_t *nwkSKey)
{
  /* USER CODE BEGIN LmHandlerSetNwkSKey_1 */

  /* USER CODE END LmHandlerSetNwkSKey_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetNwkSKey_2 */

  /* USER CODE END LmHandlerSetNwkSKey_2 */
}

LmHandlerErrorStatus_t LmHandlerGetAppSKey(uint8_t *appSKey)
{
  /* USER CODE BEGIN LmHandlerGetAppSKey_1 */

  /* USER CODE END LmHandlerGetAppSKey_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetAppSKey_2 */

  /* USER CODE END LmHandlerGetAppSKey_2 */
}

LmHandlerErrorStatus_t LmHandlerSetAppSKey(uint8_t *appSKey)
{
  /* USER CODE BEGIN LmHandlerSetAppSKey_1 */

  /* USER CODE END LmHandlerSetAppSKey_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetAppSKey_2 */

  /* USER CODE END LmHandlerSetAppSKey_2 */
}

LmHandlerErrorStatus_t LmHandlerGetNetworkID(uint32_t *networkId)
{
  /* USER CODE BEGIN LmHandlerGetNetworkID_1 */

  /* USER CODE END LmHandlerGetNetworkID_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetNetworkID_2 */

  /* USER CODE END LmHandlerGetNetworkID_2 */
}

LmHandlerErrorStatus_t LmHandlerSetNetworkID(uint32_t networkId)
{
  /* USER CODE BEGIN LmHandlerSetNetworkID_1 */

  /* USER CODE END LmHandlerSetNetworkID_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetNetworkID_2 */

  /* USER CODE END LmHandlerSetNetworkID_2 */
}

LmHandlerErrorStatus_t LmHandlerGetDevAddr(uint32_t *devAddr)
{
  /* USER CODE BEGIN LmHandlerGetDevAddr_1 */

  /* USER CODE END LmHandlerGetDevAddr_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetDevAddr_2 */

  /* USER CODE END LmHandlerGetDevAddr_2 */
}

LmHandlerErrorStatus_t LmHandlerSetDevAddr(uint32_t devAddr)
{
  /* USER CODE BEGIN LmHandlerSetDevAddr_1 */

  /* USER CODE END LmHandlerSetDevAddr_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetDevAddr_2 */

  /* USER CODE END LmHandlerSetDevAddr_2 */
}

LmHandlerErrorStatus_t LmHandlerGetActiveRegion(LoRaMacRegion_t *region)
{
  /* USER CODE BEGIN LmHandlerGetActiveRegion_1 */

  /* USER CODE END LmHandlerGetActiveRegion_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetActiveRegion_2 */

  /* USER CODE END LmHandlerGetActiveRegion_2 */
}

LmHandlerErrorStatus_t LmHandlerSetActiveRegion(LoRaMacRegion_t region)
{
  /* USER CODE BEGIN LmHandlerSetActiveRegion_1 */

  /* USER CODE END LmHandlerSetActiveRegion_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetActiveRegion_2 */

  /* USER CODE END LmHandlerSetActiveRegion_2 */
}

LmHandlerErrorStatus_t LmHandlerGetAdrEnable(bool *adrEnable)
{
  /* USER CODE BEGIN LmHandlerGetAdrEnable_1 */

  /* USER CODE END LmHandlerGetAdrEnable_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetAdrEnable_2 */

  /* USER CODE END LmHandlerGetAdrEnable_2 */
}

LmHandlerErrorStatus_t LmHandlerSetAdrEnable(bool adrEnable)
{
  /* USER CODE BEGIN LmHandlerSetAdrEnable_1 */

  /* USER CODE END LmHandlerSetAdrEnable_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetAdrEnable_2 */

  /* USER CODE END LmHandlerSetAdrEnable_2 */
}

LmHandlerErrorStatus_t LmHandlerGetTxDatarate(int8_t *txDatarate)
{
  /* USER CODE BEGIN LmHandlerGetTxDatarate_1 */

  /* USER CODE END LmHandlerGetTxDatarate_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetTxDatarate_2 */

  /* USER CODE END LmHandlerGetTxDatarate_2 */
}

LmHandlerErrorStatus_t LmHandlerSetTxDatarate(int8_t txDatarate)
{
  /* USER CODE BEGIN LmHandlerSetTxDatarate_1 */

  /* USER CODE END LmHandlerSetTxDatarate_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetTxDatarate_2 */

  /* USER CODE END LmHandlerSetTxDatarate_2 */
}

LmHandlerErrorStatus_t LmHandlerGetDutyCycleEnable(bool *dutyCycleEnable)
{
  /* USER CODE BEGIN LmHandlerGetDutyCycleEnable_1 */

  /* USER CODE END LmHandlerGetDutyCycleEnable_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetDutyCycleEnable_2 */

  /* USER CODE END LmHandlerGetDutyCycleEnable_2 */
}

LmHandlerErrorStatus_t LmHandlerSetDutyCycleEnable(bool dutyCycleEnable)
{
  /* USER CODE BEGIN LmHandlerSetDutyCycleEnable_1 */

  /* USER CODE END LmHandlerSetDutyCycleEnable_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetDutyCycleEnable_2 */

  /* USER CODE END LmHandlerSetDutyCycleEnable_2 */
}

LmHandlerErrorStatus_t LmHandlerGetRX2Params(RxChannelParams_t *rxParams)
{
  /* USER CODE BEGIN LmHandlerGetRX2Params_1 */

  /* USER CODE END LmHandlerGetRX2Params_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetRX2Params_2 */

  /* USER CODE END LmHandlerGetRX2Params_2 */
}

LmHandlerErrorStatus_t LmHandlerGetTxPower(int8_t *txPower)
{
  /* USER CODE BEGIN LmHandlerGetTxPower_1 */

  /* USER CODE END LmHandlerGetTxPower_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetTxPower_2 */

  /* USER CODE END LmHandlerGetTxPower_2 */
}

LmHandlerErrorStatus_t LmHandlerGetRx1Delay(uint32_t *rxDelay)
{
  /* USER CODE BEGIN LmHandlerGetRx1Delay_1 */

  /* USER CODE END LmHandlerGetRx1Delay_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetRx1Delay_2 */

  /* USER CODE END LmHandlerGetRx1Delay_2 */
}

LmHandlerErrorStatus_t LmHandlerGetRx2Delay(uint32_t *rxDelay)
{
  /* USER CODE BEGIN LmHandlerGetRx2Delay_1 */

  /* USER CODE END LmHandlerGetRx2Delay_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetRx2Delay_2 */

  /* USER CODE END LmHandlerGetRx2Delay_2 */
}

LmHandlerErrorStatus_t LmHandlerGetJoinRx1Delay(uint32_t *rxDelay)
{
  /* USER CODE BEGIN LmHandlerGetJoinRx1Delay_1 */

  /* USER CODE END LmHandlerGetJoinRx1Delay_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetJoinRx1Delay_2 */

  /* USER CODE END LmHandlerGetJoinRx1Delay_2 */
}

LmHandlerErrorStatus_t LmHandlerGetJoinRx2Delay(uint32_t *rxDelay)
{
  /* USER CODE BEGIN LmHandlerGetJoinRx2Delay_1 */

  /* USER CODE END LmHandlerGetJoinRx2Delay_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetJoinRx2Delay_2 */

  /* USER CODE END LmHandlerGetJoinRx2Delay_2 */
}

LmHandlerErrorStatus_t LmHandlerSetRX2Params(RxChannelParams_t *rxParams)
{
  /* USER CODE BEGIN LmHandlerSetRX2Params_1 */

  /* USER CODE END LmHandlerSetRX2Params_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetRX2Params_2 */

  /* USER CODE END LmHandlerSetRX2Params_2 */
}

LmHandlerErrorStatus_t LmHandlerSetTxPower(int8_t txPower)
{
  /* USER CODE BEGIN LmHandlerSetTxPower_1 */

  /* USER CODE END LmHandlerSetTxPower_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetTxPower_2 */

  /* USER CODE END LmHandlerSetTxPower_2 */
}

LmHandlerErrorStatus_t LmHandlerSetRx1Delay(uint32_t rxDelay)
{
  /* USER CODE BEGIN LmHandlerSetRx1Delay_1 */

  /* USER CODE END LmHandlerSetRx1Delay_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetRx1Delay_2 */

  /* USER CODE END LmHandlerSetRx1Delay_2 */
}

LmHandlerErrorStatus_t LmHandlerSetRx2Delay(uint32_t rxDelay)
{
  /* USER CODE BEGIN LmHandlerSetRx2Delay_1 */

  /* USER CODE END LmHandlerSetRx2Delay_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetRx2Delay_2 */

  /* USER CODE END LmHandlerSetRx2Delay_2 */
}

LmHandlerErrorStatus_t LmHandlerSetJoinRx1Delay(uint32_t rxDelay)
{
  /* USER CODE BEGIN LmHandlerSetJoinRx1Delay_1 */

  /* USER CODE END LmHandlerSetJoinRx1Delay_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetJoinRx1Delay_2 */

  /* USER CODE END LmHandlerSetJoinRx1Delay_2 */
}

LmHandlerErrorStatus_t LmHandlerSetJoinRx2Delay(uint32_t rxDelay)
{
  /* USER CODE BEGIN LmHandlerSetJoinRx2Delay_1 */

  /* USER CODE END LmHandlerSetJoinRx2Delay_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetJoinRx2Delay_2 */

  /* USER CODE END LmHandlerSetJoinRx2Delay_2 */
}

LmHandlerErrorStatus_t LmHandlerGetPingPeriodicity(uint8_t *pingPeriodicity)
{
  /* USER CODE BEGIN LmHandlerGetPingPeriodicity_1 */

  /* USER CODE END LmHandlerGetPingPeriodicity_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetPingPeriodicity_2 */

  /* USER CODE END LmHandlerGetPingPeriodicity_2 */
}

LmHandlerErrorStatus_t LmHandlerSetPingPeriodicity(uint8_t pingPeriodicity)
{
  /* USER CODE BEGIN LmHandlerSetPingPeriodicity_1 */

  /* USER CODE END LmHandlerSetPingPeriodicity_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerSetPingPeriodicity_2 */

  /* USER CODE END LmHandlerSetPingPeriodicity_2 */
}

LmHandlerErrorStatus_t LmHandlerGetBeaconState(BeaconState_t *beaconState)
{
  /* USER CODE BEGIN LmHandlerGetBeaconState_1 */

  /* USER CODE END LmHandlerGetBeaconState_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerGetBeaconState_2 */

  /* USER CODE END LmHandlerGetBeaconState_2 */
}

LmHandlerErrorStatus_t LmHandlerLinkCheckReq(void)
{
  /* USER CODE BEGIN LmHandlerLinkCheckReq_1 */

  /* USER CODE END LmHandlerLinkCheckReq_1 */
  return (LmHandlerErrorStatus_t) 0UL;
  /* USER CODE BEGIN LmHandlerLinkCheckReq_2 */

  /* USER CODE END LmHandlerLinkCheckReq_2 */
}

LmHandlerErrorStatus_t LmHandlerDeviceTimeReq(void)
{
  /* USER CODE BEGIN LmHandlerDeviceTimeReq_1 */

  /* USER CODE END LmHandlerDeviceTimeReq_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t ret;

  com_obj = MBMUXIF_GetLoraFeatureCmdComPtr();
  com_obj->MsgId = LMHANDLER_DEVICE_TIME_REQ_ID;
  com_obj->ParamCnt = 0;

  MBMUXIF_LoraSendCmd();
  /* waiting for event */

  ret = com_obj->ReturnVal;
  return (LmHandlerErrorStatus_t) ret;
  /* USER CODE BEGIN LmHandlerDeviceTimeReq_2 */

  /* USER CODE END LmHandlerDeviceTimeReq_2 */
}

void LoraInfo_Init(void)
{
  /* USER CODE BEGIN LoraInfo_Init_1 */

  /* USER CODE END LoraInfo_Init_1 */
  MBMUX_ComParam_t *com_obj;

  com_obj = MBMUXIF_GetLoraFeatureCmdComPtr();
  com_obj->MsgId = LMHANDLER_INFO_INIT_ID;
  com_obj->ParamCnt = 0;

  MBMUXIF_LoraSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN LoraInfo_Init_2 */

  /* USER CODE END LoraInfo_Init_2 */
}

LoraInfo_t *LoraInfo_GetPtr(void)
{
  /* USER CODE BEGIN LoraInfo_GetPtr_1 */

  /* USER CODE END LoraInfo_GetPtr_1 */
  FEAT_INFO_Param_t *p_feature;

  p_feature = MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_LORAWAN_ID);
  return (LoraInfo_t *) p_feature->Feat_Info_Config_Ptr;
  /* USER CODE BEGIN LoraInfo_GetPtr_2 */

  /* USER CODE END LoraInfo_GetPtr_2 */
}

void Process_Lora_Notif(MBMUX_ComParam_t *ComObj)
{
  /* USER CODE BEGIN Process_Lora_Notif_1 */

  /* USER CODE END Process_Lora_Notif_1 */
  uint32_t *com_buffer = ComObj->ParamBuf;
  uint32_t cb_ret = 0;

  /* Lora Notif received */
  APP_LOG(TS_ON, VLEVEL_H, "CM4<(LoRa)\r\n");

  /* process Command */
  switch (ComObj->MsgId)
  {
    /* callbacks */
    case LMHANDLER_GET_BATTERY_LEVEL_CB_ID:
      if (callback_mbwrapper.GetBatteryLevel != NULL)
      {
        cb_ret = (uint32_t) callback_mbwrapper.GetBatteryLevel();
      }
      else
      {
        Error_Handler();
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  cb_ret; /* */
      break;

    case LMHANDLER_GET_TEMPERATURE_CB_ID:
      if (callback_mbwrapper.GetTemperature != NULL)
      {
        cb_ret = (uint32_t) callback_mbwrapper.GetTemperature();
      }
      else
      {
        Error_Handler();
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  cb_ret; /* */
      break;

    case LMHANDLER_ON_NVM_DATA_CHANGE_CB_ID:
      if (callback_mbwrapper.OnNvmDataChange != NULL)
      {
        callback_mbwrapper.OnNvmDataChange((LmHandlerNvmContextStates_t) com_buffer[0], (uint16_t) com_buffer[1]);
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  0; /* */
      break;

    case LMHANDLER_ON_NETWORK_PARAMS_CHANGE_CB_ID:
      if (callback_mbwrapper.OnNetworkParametersChange != NULL)
      {
        callback_mbwrapper.OnNetworkParametersChange((CommissioningParams_t *) com_buffer[0]);
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  0; /* */
      break;

    case LMHANDLER_ON_JOIN_REQ_CB_ID:
      if (callback_mbwrapper.OnJoinRequest != NULL)
      {
        callback_mbwrapper.OnJoinRequest((LmHandlerJoinParams_t *) com_buffer[0]);
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  0; /* */
      break;

    case LMHANDLER_ON_TX_DATA_CB_ID:
      if (callback_mbwrapper.OnTxData != NULL)
      {
        callback_mbwrapper.OnTxData((LmHandlerTxParams_t *) com_buffer[0]);
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  0; /* */
      break;

    case LMHANDLER_ON_RX_DATA_CB_ID:
      if (callback_mbwrapper.OnRxData != NULL)
      {
        callback_mbwrapper.OnRxData((LmHandlerAppData_t *) com_buffer[0], (LmHandlerRxParams_t *) com_buffer[1]);
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  0; /* */
      break;

    case LMHANDLER_ON_CLASS_CHANGE_CB_ID:
      if (callback_mbwrapper.OnClassChange != NULL)
      {
        callback_mbwrapper.OnClassChange((DeviceClass_t) com_buffer[0]);
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  0; /* */
      break;

    case LMHANDLER_ON_BEACON_STATUS_CHANGE_CB_ID:
      if (callback_mbwrapper.OnBeaconStatusChange != NULL)
      {
        callback_mbwrapper.OnBeaconStatusChange((LmHandlerBeaconParams_t *) com_buffer[0]);
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  0; /* */
      break;

    case LMHANDLER_ON_ON_SYS_TIME_UPDATE_CB_ID:
      if (callback_mbwrapper.OnSysTimeUpdate != NULL)
      {
        callback_mbwrapper.OnSysTimeUpdate();
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  0; /* */
      break;
    default:
      break;
  }

  /* Lora sending ack */
  APP_LOG(TS_ON, VLEVEL_H, "CM4>(LoRa)\r\n");
  MBMUX_AcknowledgeSnd(FEAT_INFO_LORAWAN_ID);
  /* USER CODE BEGIN Process_Lora_Notif_2 */

  /* USER CODE END Process_Lora_Notif_2 */
}

/**
  * @}
  *
  */
/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
