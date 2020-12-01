/**
  ******************************************************************************
  * @file    lora_at.c
  * @author  MCD Application Team
  * @brief   AT command API
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
#include "lora_at.h"
#include "sys_app.h"
#include "stm32_tiny_sscanf.h"
#include "lora_app_version.h"
#include "lorawan_version.h"
#include "subghz_phy_version.h"
#include "test_rf.h"
#include "adc_if.h"
#include "stm32_seq.h"
#include "utilities_def.h"
#include "radio.h"
#include "lora_info.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/*!
 * User application data buffer size
 */
#define LORAWAN_APP_DATA_BUFFER_MAX_SIZE            242

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static bool ClassBEnableRequest = false;

/*!
 * User application buffer
 */
static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];

/*!
 * User application data structure
 */
static LmHandlerAppData_t AppData = { 0, 0, AppDataBuffer };

/* Dummy data sent periodically to let the tester respond with start test command*/
static UTIL_TIMER_Object_t TxCertifTimer;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Get 4 bytes values in hexa
  * @param  The string containing the 16 bytes, something like ab:cd:01:21
  * @param  The buffer that will contain the bytes read
  * @retval The number of bytes read
  */
static int32_t sscanf_uint32_as_hhx(const char *from, uint32_t *value);

/**
  * @brief  Print 4 bytes as %02x
  * @param  the value containing the 4 bytes to print
  * @retval None
  */
static void print_uint32_as_02x(uint32_t value);

/**
  * @brief  Print 8 bytes as %02x
  * @param  the pointer containing the 8 bytes to print
  * @retval None
  */
static void print_8_02x(uint8_t *pt);

/**
  * @brief  Print an int
  * @param  the value to print
  * @retval None
  */
static void print_d(int32_t value);

/**
  * @brief  Print an unsigned int
  * @param  the value to print
  * @retval None
  */
static void print_u(uint32_t value);

static void OnCertifTimer(void *context);

static void CertifSend(void);

static int32_t isHex(char Char);

static int32_t stringToData(const char *str, uint8_t *data, uint32_t Size);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
void AT_event_join(LmHandlerJoinParams_t *params)
{
  /* USER CODE BEGIN AT_event_join_1 */

  /* USER CODE END AT_event_join_1 */
  if ((params != NULL) && (params->Status == LORAMAC_HANDLER_SUCCESS))
  {
    AT_PRINTF("+EVT:JOINED\r\n");
  }
  else
  {
    AT_PRINTF("+EVT:JOIN FAILED\r\n");
  }
  /* USER CODE BEGIN AT_event_join_2 */

  /* USER CODE END AT_event_join_2 */
}

void AT_event_receive(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
  /* USER CODE BEGIN AT_event_receive_1 */

  /* USER CODE END AT_event_receive_1 */
  const char *slotStrings[] = { "1", "2", "C", "C Multicast", "B Ping-Slot", "B Multicast Ping-Slot" };
  uint8_t ReceivedDataSize = 0;

  if ((appData != NULL) && (appData->BufferSize > 0))
  {
    /* Received data to be copied*/
    if (LORAWAN_APP_DATA_BUFFER_MAX_SIZE <= appData->BufferSize)
    {
      ReceivedDataSize = LORAWAN_APP_DATA_BUFFER_MAX_SIZE;
    }
    else
    {
      ReceivedDataSize = appData->BufferSize;
    }

    /*aynchronous notification to the host*/
    AT_PRINTF("+EVT:%d:%02x:", appData->Port, ReceivedDataSize);

    for (uint8_t i = 0; i < ReceivedDataSize; i++)
    {
      AT_PRINTF("%02x", appData->Buffer[i]);
    }
    AT_PRINTF("\r\n");
    if (params != NULL)
    {
      AT_PRINTF("+EVT:RX_%s, DR %d, RSSI %d, SNR %d\r\n", slotStrings[params->RxSlot], params->Datarate, params->Rssi, params->Snr);
    }
  }
  /* USER CODE BEGIN AT_event_receive_2 */

  /* USER CODE END AT_event_receive_2 */
}

void AT_event_confirm(LmHandlerTxParams_t *params)
{
  /* USER CODE BEGIN AT_event_confirm_1 */

  /* USER CODE END AT_event_confirm_1 */
  if ((params != NULL) && (params->MsgType == LORAMAC_HANDLER_CONFIRMED_MSG) && (params->AckReceived != 0))
  {
    AT_PRINTF("+EVT:SEND_CONFIRMED\r\n");
  }
  /* USER CODE BEGIN AT_event_confirm_2 */

  /* USER CODE END AT_event_confirm_2 */
}

ATEerror_t AT_return_ok(const char *param)
{
  return AT_OK;
}

ATEerror_t AT_return_error(const char *param)
{
  return AT_ERROR;
}

ATEerror_t AT_reset(const char *param)
{
  /* USER CODE BEGIN AT_reset_1 */

  /* USER CODE END AT_reset_1 */
  NVIC_SystemReset();
  /* USER CODE BEGIN AT_reset_2 */

  /* USER CODE END AT_reset_2 */
}

ATEerror_t AT_Region_get(const char *param)
{
  /* USER CODE BEGIN AT_Region_get_1 */

  /* USER CODE END AT_Region_get_1 */
  const char *regionStrings[] = { "AS923", "AU915", "CN470", "CN779", "EU433", "EU868", "KR920", "IN865", "US915", "RU864" };
  LoRaMacRegion_t region;
  if (LmHandlerGetActiveRegion(&region) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  if (region > LORAMAC_REGION_RU864)
  {
    return AT_PARAM_ERROR;
  }

  AT_PRINTF("%d:%s\r\n", region, regionStrings[region]);
  return AT_OK;
  /* USER CODE BEGIN AT_Region_get_2 */

  /* USER CODE END AT_Region_get_2 */
}

ATEerror_t AT_Region_set(const char *param)
{
  /* USER CODE BEGIN AT_Region_set_1 */

  /* USER CODE END AT_Region_set_1 */
  LoRaMacRegion_t region;
  if (tiny_sscanf(param, "%hhu", &region) != 1)
  {
    return AT_PARAM_ERROR;
  }
  if (region > LORAMAC_REGION_RU864)
  {
    return AT_PARAM_ERROR;
  }

  if (LmHandlerSetActiveRegion(region) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_Region_set_2 */

  /* USER CODE END AT_Region_set_2 */
}

ATEerror_t AT_DevEUI_get(const char *param)
{
  /* USER CODE BEGIN AT_DevEUI_get_1 */

  /* USER CODE END AT_DevEUI_get_1 */
  uint8_t devEUI[8];
  if (LmHandlerGetDevEUI(devEUI) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_8_02x(devEUI);
  return AT_OK;
  /* USER CODE BEGIN AT_DevEUI_get_2 */

  /* USER CODE END AT_DevEUI_get_2 */
}

ATEerror_t AT_DevEUI_set(const char *param)
{
  /* USER CODE BEGIN AT_DevEUI_set_1 */

  /* USER CODE END AT_DevEUI_set_1 */
  uint8_t devEui[8];
  if (tiny_sscanf(param, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                  &devEui[0], &devEui[1], &devEui[2], &devEui[3],
                  &devEui[4], &devEui[5], &devEui[6], &devEui[7]) != 8)
  {
    return AT_PARAM_ERROR;
  }

  if (LORAMAC_HANDLER_SUCCESS != LmHandlerSetDevEUI(devEui))
  {
    return AT_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_DevEUI_set_2 */

  /* USER CODE END AT_DevEUI_set_2 */
}

ATEerror_t AT_JoinEUI_get(const char *param)
{
  /* USER CODE BEGIN AT_JoinEUI_get_1 */

  /* USER CODE END AT_JoinEUI_get_1 */
  uint8_t appEUI[8];
  if (LmHandlerGetAppEUI(appEUI) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_8_02x(appEUI);
  return AT_OK;
  /* USER CODE BEGIN AT_JoinEUI_get_2 */

  /* USER CODE END AT_JoinEUI_get_2 */
}

ATEerror_t AT_JoinEUI_set(const char *param)
{
  /* USER CODE BEGIN AT_JoinEUI_set_1 */

  /* USER CODE END AT_JoinEUI_set_1 */
  uint8_t JoinEui[8];
  if (tiny_sscanf(param, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                  &JoinEui[0], &JoinEui[1], &JoinEui[2], &JoinEui[3],
                  &JoinEui[4], &JoinEui[5], &JoinEui[6], &JoinEui[7]) != 8)
  {
    return AT_PARAM_ERROR;
  }

  if (LORAMAC_HANDLER_SUCCESS != LmHandlerSetAppEUI(JoinEui))
  {
    return AT_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_JoinEUI_set_2 */

  /* USER CODE END AT_JoinEUI_set_2 */
}

ATEerror_t AT_DevAddr_get(const char *param)
{
  /* USER CODE BEGIN AT_DevAddr_get_1 */

  /* USER CODE END AT_DevAddr_get_1 */
  uint32_t devAddr;
  if (LmHandlerGetDevAddr(&devAddr) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_uint32_as_02x(devAddr);
  return AT_OK;
  /* USER CODE BEGIN AT_DevAddr_get_2 */

  /* USER CODE END AT_DevAddr_get_2 */
}

ATEerror_t AT_DevAddr_set(const char *param)
{
  /* USER CODE BEGIN AT_DevAddr_set_1 */

  /* USER CODE END AT_DevAddr_set_1 */
  uint32_t devAddr;
  if (sscanf_uint32_as_hhx(param, &devAddr) != 4)
  {
    return AT_PARAM_ERROR;
  }

  if (LORAMAC_HANDLER_SUCCESS != LmHandlerSetDevAddr(devAddr))
  {
    return AT_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_DevAddr_set_2 */

  /* USER CODE END AT_DevAddr_set_2 */
}

ATEerror_t AT_ADR_get(const char *param)
{
  /* USER CODE BEGIN AT_ADR_get_1 */

  /* USER CODE END AT_ADR_get_1 */
  bool adrEnable;
  if (LmHandlerGetAdrEnable(&adrEnable) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_d(adrEnable);
  return AT_OK;
  /* USER CODE BEGIN AT_ADR_get_2 */

  /* USER CODE END AT_ADR_get_2 */
}

ATEerror_t AT_ADR_set(const char *param)
{
  /* USER CODE BEGIN AT_ADR_set_1 */

  /* USER CODE END AT_ADR_set_1 */
  switch (param[0])
  {
    case '0':
    case '1':
      LmHandlerSetAdrEnable(param[0] - '0');
      break;
    default:
      return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_ADR_set_2 */

  /* USER CODE END AT_ADR_set_2 */
}

ATEerror_t AT_TransmitPower_get(const char *param)
{
  /* USER CODE BEGIN AT_TransmitPower_get_1 */

  /* USER CODE END AT_TransmitPower_get_1 */
  int8_t txPower;
  if (LmHandlerGetTxPower(&txPower) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_d(txPower);
  return AT_OK;
  /* USER CODE BEGIN AT_TransmitPower_get_2 */

  /* USER CODE END AT_TransmitPower_get_2 */
}

ATEerror_t AT_TransmitPower_set(const char *param)
{
  /* USER CODE BEGIN AT_TransmitPower_set_1 */

  /* USER CODE END AT_TransmitPower_set_1 */
  int8_t txPower;
  if (tiny_sscanf(param, "%hhu", &txPower) != 1)
  {
    return AT_PARAM_ERROR;
  }

  if (LmHandlerSetTxPower(txPower) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_TransmitPower_set_2 */

  /* USER CODE END AT_TransmitPower_set_2 */
}

ATEerror_t AT_DataRate_get(const char *param)
{
  /* USER CODE BEGIN AT_DataRate_get_1 */

  /* USER CODE END AT_DataRate_get_1 */
  int8_t txDatarate;
  if (LmHandlerGetTxDatarate(&txDatarate) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_d(txDatarate);
  return AT_OK;
  /* USER CODE BEGIN AT_DataRate_get_2 */

  /* USER CODE END AT_DataRate_get_2 */
}

ATEerror_t AT_DataRate_set(const char *param)
{
  /* USER CODE BEGIN AT_DataRate_set_1 */

  /* USER CODE END AT_DataRate_set_1 */
  int8_t datarate;

  if (tiny_sscanf(param, "%hhu", &datarate) != 1)
  {
    return AT_PARAM_ERROR;
  }
  if ((datarate < 0) || (datarate > 15))
  {
    return AT_PARAM_ERROR;
  }

  if (LmHandlerSetTxDatarate(datarate) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_DataRate_set_2 */

  /* USER CODE END AT_DataRate_set_2 */
}

ATEerror_t AT_DutyCycle_get(const char *param)
{
  /* USER CODE BEGIN AT_DutyCycle_get_1 */

  /* USER CODE END AT_DutyCycle_get_1 */
  bool dutyCycleEnable;
  if (LmHandlerGetDutyCycleEnable(&dutyCycleEnable) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_d(dutyCycleEnable);
  return AT_OK;
  /* USER CODE BEGIN AT_DutyCycle_get_2 */

  /* USER CODE END AT_DutyCycle_get_2 */
}

ATEerror_t AT_DutyCycle_set(const char *param)
{
  /* USER CODE BEGIN AT_DutyCycle_set_1 */

  /* USER CODE END AT_DutyCycle_set_1 */
  switch (param[0])
  {
    case '0':
    case '1':
      LmHandlerSetDutyCycleEnable(param[0] - '0');
      break;
    default:
      return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_DutyCycle_set_2 */

  /* USER CODE END AT_DutyCycle_set_2 */
}

ATEerror_t AT_Rx2Frequency_get(const char *param)
{
  /* USER CODE BEGIN AT_Rx2Frequency_get_1 */

  /* USER CODE END AT_Rx2Frequency_get_1 */
  RxChannelParams_t rx2Params;
  LmHandlerGetRX2Params(&rx2Params);
  print_d(rx2Params.Frequency);
  return AT_OK;
  /* USER CODE BEGIN AT_Rx2Frequency_get_2 */

  /* USER CODE END AT_Rx2Frequency_get_2 */
}

ATEerror_t AT_Rx2DataRate_get(const char *param)
{
  /* USER CODE BEGIN AT_Rx2DataRate_get_1 */

  /* USER CODE END AT_Rx2DataRate_get_1 */
  RxChannelParams_t rx2Params;
  LmHandlerGetRX2Params(&rx2Params);
  print_d(rx2Params.Datarate);
  return AT_OK;
  /* USER CODE BEGIN AT_Rx2DataRate_get_2 */

  /* USER CODE END AT_Rx2DataRate_get_2 */
}

ATEerror_t AT_Rx1Delay_get(const char *param)
{
  /* USER CODE BEGIN AT_Rx1Delay_get_1 */

  /* USER CODE END AT_Rx1Delay_get_1 */
  uint32_t rxDelay;
  if (LmHandlerGetRx1Delay(&rxDelay) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_u(rxDelay);
  return AT_OK;
  /* USER CODE BEGIN AT_Rx1Delay_get_2 */

  /* USER CODE END AT_Rx1Delay_get_2 */
}

ATEerror_t AT_Rx2Delay_get(const char *param)
{
  /* USER CODE BEGIN AT_Rx2Delay_get_1 */

  /* USER CODE END AT_Rx2Delay_get_1 */
  uint32_t rxDelay;
  if (LmHandlerGetRx2Delay(&rxDelay) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_u(rxDelay);
  return AT_OK;
  /* USER CODE BEGIN AT_Rx2Delay_get_2 */

  /* USER CODE END AT_Rx2Delay_get_2 */
}

ATEerror_t AT_JoinAcceptDelay1_get(const char *param)
{
  /* USER CODE BEGIN AT_JoinAcceptDelay1_get_1 */

  /* USER CODE END AT_JoinAcceptDelay1_get_1 */
  uint32_t rxDelay;
  if (LmHandlerGetJoinRx1Delay(&rxDelay) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_u(rxDelay);
  return AT_OK;
  /* USER CODE BEGIN AT_JoinAcceptDelay1_get_2 */

  /* USER CODE END AT_JoinAcceptDelay1_get_2 */
}

ATEerror_t AT_JoinAcceptDelay2_get(const char *param)
{
  /* USER CODE BEGIN AT_JoinAcceptDelay2_get_1 */

  /* USER CODE END AT_JoinAcceptDelay2_get_1 */
  uint32_t rxDelay;
  if (LmHandlerGetJoinRx2Delay(&rxDelay) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_u(rxDelay);
  return AT_OK;
  /* USER CODE BEGIN AT_JoinAcceptDelay2_get_2 */

  /* USER CODE END AT_JoinAcceptDelay2_get_2 */
}

ATEerror_t AT_Rx2Frequency_set(const char *param)
{
  /* USER CODE BEGIN AT_Rx2Frequency_set_1 */

  /* USER CODE END AT_Rx2Frequency_set_1 */
  RxChannelParams_t rx2Params;

  /* Get the current configuration of RX2 */
  LmHandlerGetRX2Params(&rx2Params);

  /* Update the frequency with scanf */
  if (tiny_sscanf(param, "%lu", &(rx2Params.Frequency)) != 1)
  {
    return AT_PARAM_ERROR;
  }
  else if (LmHandlerSetRX2Params(&rx2Params) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_Rx2Frequency_set_2 */

  /* USER CODE END AT_Rx2Frequency_set_2 */
}

ATEerror_t AT_Rx2DataRate_set(const char *param)
{
  /* USER CODE BEGIN AT_Rx2DataRate_set_1 */

  /* USER CODE END AT_Rx2DataRate_set_1 */
  RxChannelParams_t rx2Params;

  /* Get the current configuration of RX2 */
  LmHandlerGetRX2Params(&rx2Params);

  /* Update the Datarate with scanf */
  if (tiny_sscanf(param, "%hhu", &(rx2Params.Datarate)) != 1)
  {
    return AT_PARAM_ERROR;
  }
  else if (rx2Params.Datarate > 15)
  {
    return AT_PARAM_ERROR;
  }
  else if (LmHandlerSetRX2Params(&rx2Params) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_Rx2DataRate_set_2 */

  /* USER CODE END AT_Rx2DataRate_set_2 */
}

ATEerror_t AT_Rx1Delay_set(const char *param)
{
  /* USER CODE BEGIN AT_Rx1Delay_set_1 */

  /* USER CODE END AT_Rx1Delay_set_1 */
  uint32_t rxDelay;
  if (tiny_sscanf(param, "%lu", &rxDelay) != 1)
  {
    return AT_PARAM_ERROR;
  }
  else if (LmHandlerSetRx1Delay(rxDelay) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_Rx1Delay_set_2 */

  /* USER CODE END AT_Rx1Delay_set_2 */
}

ATEerror_t AT_Rx2Delay_set(const char *param)
{
  /* USER CODE BEGIN AT_Rx2Delay_set_1 */

  /* USER CODE END AT_Rx2Delay_set_1 */
  uint32_t rxDelay;
  if (tiny_sscanf(param, "%lu", &rxDelay) != 1)
  {
    return AT_PARAM_ERROR;
  }
  else if (LmHandlerSetRx2Delay(rxDelay) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_Rx2Delay_set_2 */

  /* USER CODE END AT_Rx2Delay_set_2 */
}

ATEerror_t AT_JoinAcceptDelay1_set(const char *param)
{
  /* USER CODE BEGIN AT_JoinAcceptDelay1_set_1 */

  /* USER CODE END AT_JoinAcceptDelay1_set_1 */
  uint32_t rxDelay;
  if (tiny_sscanf(param, "%lu", &rxDelay) != 1)
  {
    return AT_PARAM_ERROR;
  }
  else if (LmHandlerSetJoinRx1Delay(rxDelay) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_JoinAcceptDelay1_set_2 */

  /* USER CODE END AT_JoinAcceptDelay1_set_2 */
}

ATEerror_t AT_JoinAcceptDelay2_set(const char *param)
{
  /* USER CODE BEGIN AT_JoinAcceptDelay2_set_1 */

  /* USER CODE END AT_JoinAcceptDelay2_set_1 */
  uint32_t rxDelay;
  if (tiny_sscanf(param, "%lu", &rxDelay) != 1)
  {
    return AT_PARAM_ERROR;
  }
  else if (LmHandlerSetJoinRx2Delay(rxDelay) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_JoinAcceptDelay2_set_2 */

  /* USER CODE END AT_JoinAcceptDelay2_set_2 */
}

ATEerror_t AT_NetworkID_get(const char *param)
{
  /* USER CODE BEGIN AT_NetworkID_get_1 */

  /* USER CODE END AT_NetworkID_get_1 */
  uint32_t networkId;
  if (LmHandlerGetNetworkID(&networkId) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_uint32_as_02x(networkId);
  return AT_OK;
  /* USER CODE BEGIN AT_NetworkID_get_2 */

  /* USER CODE END AT_NetworkID_get_2 */
}

ATEerror_t AT_NetworkID_set(const char *param)
{
  /* USER CODE BEGIN AT_NetworkID_set_1 */

  /* USER CODE END AT_NetworkID_set_1 */
  uint32_t networkId;
  if (sscanf_uint32_as_hhx(param, &networkId) != 4)
  {
    return AT_PARAM_ERROR;
  }

  LmHandlerSetNetworkID(networkId);
  return AT_OK;
  /* USER CODE BEGIN AT_NetworkID_set_2 */

  /* USER CODE END AT_NetworkID_set_2 */
}

ATEerror_t AT_DeviceClass_get(const char *param)
{
  /* USER CODE BEGIN AT_DeviceClass_get_1 */

  /* USER CODE END AT_DeviceClass_get_1 */
  DeviceClass_t currentClass;
  LoraInfo_t *loraInfo = LoraInfo_GetPtr();
  if (loraInfo == NULL)
  {
    return AT_ERROR;
  }

  if (LmHandlerGetCurrentClass(&currentClass) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  if ((loraInfo->ClassB == 1) && (ClassBEnableRequest == true) && (currentClass == CLASS_A))
  {
    BeaconState_t beaconState;

    if (LmHandlerGetBeaconState(&beaconState) != LORAMAC_HANDLER_SUCCESS)
    {
      return AT_PARAM_ERROR;
    }

    if ((beaconState == BEACON_STATE_ACQUISITION) ||
        (beaconState == BEACON_STATE_ACQUISITION_BY_TIME) ||
        (beaconState == BEACON_STATE_REACQUISITION)) /*Beacon_Searching on Class B request*/
    {
      AT_PRINTF("B,S0\r\n");
    }
    else if ((beaconState == BEACON_STATE_LOCKED) || /*Beacon locked on Gateway*/
             (beaconState == BEACON_STATE_IDLE)   ||
             (beaconState == BEACON_STATE_GUARD)  ||
             (beaconState == BEACON_STATE_RX))
    {
      AT_PRINTF("B,S1\r\n");
    }
    else
    {
      AT_PRINTF("B,S2\r\n");
    }
  }
  else /* we are now either in Class B enable or Class C enable*/
  {
    AT_PRINTF("%c\r\n", 'A' + currentClass);
  }

  return AT_OK;
  /* USER CODE BEGIN AT_DeviceClass_get_2 */

  /* USER CODE END AT_DeviceClass_get_2 */
}

ATEerror_t AT_DeviceClass_set(const char *param)
{
  /* USER CODE BEGIN AT_DeviceClass_set_1 */

  /* USER CODE END AT_DeviceClass_set_1 */
  LmHandlerErrorStatus_t errorStatus = LORAMAC_HANDLER_SUCCESS;
  LoraInfo_t *loraInfo = LoraInfo_GetPtr();
  if (loraInfo == NULL)
  {
    return AT_ERROR;
  }

  switch (param[0])
  {
    case 'A':
      if (loraInfo->ClassB == 1)
      {
        ClassBEnableRequest = false;
      }
      errorStatus = LmHandlerRequestClass(CLASS_A);
      break;
    case 'B':
      if (loraInfo->ClassB == 1)
      {
        ClassBEnableRequest = true;
        errorStatus = LmHandlerRequestClass(CLASS_B);  /*Class B AT cmd switch Class B not supported cf.[UM2073]*/
      }
      else
      {
        return AT_NO_CLASS_B_ENABLE;
      }
      break;
    case 'C':
      errorStatus = LmHandlerRequestClass(CLASS_C);
      break;
    default:
      return AT_PARAM_ERROR;
  }

  if (errorStatus == LORAMAC_HANDLER_NO_NETWORK_JOINED)
  {
    return AT_NO_NET_JOINED;
  }
  else if (errorStatus != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_DeviceClass_set_2 */

  /* USER CODE END AT_DeviceClass_set_2 */
}

ATEerror_t AT_PingSlot_get(const char *param)
{
  /* USER CODE BEGIN AT_PingSlot_get_1 */

  /* USER CODE END AT_PingSlot_get_1 */
  uint8_t periodicity;

  if (LmHandlerGetPingPeriodicity(&periodicity) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  print_d(periodicity);
  return AT_OK;
  /* USER CODE BEGIN AT_PingSlot_get_2 */

  /* USER CODE END AT_PingSlot_get_2 */
}

ATEerror_t AT_PingSlot_set(const char *param)
{
  /* USER CODE BEGIN AT_PingSlot_set_1 */

  /* USER CODE END AT_PingSlot_set_1 */
  uint8_t periodicity;

  if (tiny_sscanf(param, "%hhu", &periodicity) != 1)
  {
    return AT_PARAM_ERROR;
  }
  else if (periodicity > 7)
  {
    return AT_PARAM_ERROR;
  }
  else if (LmHandlerSetPingPeriodicity(periodicity) != LORAMAC_HANDLER_SUCCESS)
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_PingSlot_set_2 */

  /* USER CODE END AT_PingSlot_set_2 */
}

ATEerror_t AT_LocalTime_get(const char *param)
{
  /* USER CODE BEGIN AT_LocalTime_get_1 */

  /* USER CODE END AT_LocalTime_get_1 */
  struct tm localtime;
  SysTime_t UnixEpoch = SysTimeGet();
  UnixEpoch.Seconds -= 18; /*removing leap seconds*/

  UnixEpoch.Seconds += 3600 * 2; /*adding 2 hours*/

  SysTimeLocalTime(UnixEpoch.Seconds,  & localtime);

  AT_PRINTF("LTIME:%02dh%02dm%02ds on %02d/%02d/%04d\r\n",
            localtime.tm_hour, localtime.tm_min, localtime.tm_sec,
            localtime.tm_mday, localtime.tm_mon + 1, localtime.tm_year + 1900);

  return AT_OK;
  /* USER CODE BEGIN AT_LocalTime_get_2 */

  /* USER CODE END AT_LocalTime_get_2 */
}

ATEerror_t AT_Join(const char *param)
{
  /* USER CODE BEGIN AT_Join_1 */

  /* USER CODE END AT_Join_1 */
  switch (param[0])
  {
    case '0':
      LmHandlerJoin(ACTIVATION_TYPE_ABP);
      break;
    case '1':
      LmHandlerJoin(ACTIVATION_TYPE_OTAA);
      break;
    default:
      return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_Join_2 */

  /* USER CODE END AT_Join_2 */
}

ATEerror_t AT_Send(const char *param)
{
  /* USER CODE BEGIN AT_Send_1 */

  /* USER CODE END AT_Send_1 */
  const char *buf = param;
  uint16_t bufSize = strlen(param);
  uint32_t appPort;
  LmHandlerMsgTypes_t isTxConfirmed;
  unsigned size = 0;
  char hex[3] = {0, 0, 0};
  UTIL_TIMER_Time_t nextTxIn = 0;
  LmHandlerErrorStatus_t lmhStatus = LORAMAC_HANDLER_ERROR;
  ATEerror_t status = AT_ERROR;

  /* read and set the application port */
  if (1 != tiny_sscanf(buf, "%u:", &appPort))
  {
    AT_PRINTF("AT+SEND without the application port\r\n");
    return AT_PARAM_ERROR;
  }

  /* skip the application port */
  while (('0' <= buf[0]) && (buf[0] <= '9') && bufSize > 1)
  {
    buf ++;
    bufSize --;
  };

  if ((bufSize == 0) || (':' != buf[0]))
  {
    AT_PRINTF("AT+SEND missing : character after app port\r\n");
    return AT_PARAM_ERROR;
  }
  else
  {
    /* skip the char ':' */
    buf ++;
    bufSize --;
  }

  switch (buf[0])
  {
    case '0':
      isTxConfirmed = LORAMAC_HANDLER_UNCONFIRMED_MSG;
      break;
    case '1':
      isTxConfirmed = LORAMAC_HANDLER_CONFIRMED_MSG;
      break;
    default:
      AT_PRINTF("AT+SEND without the acknowledge flag\r\n");
      return AT_PARAM_ERROR;
  }

  if (bufSize > 0)
  {
    /* skip the acknowledge flag */
    buf ++;
    bufSize --;
  }

  if ((bufSize == 0) || (':' != buf[0]))
  {
    AT_PRINTF("AT+SEND missing : character after ack flag\r\n");
    return AT_PARAM_ERROR;
  }
  else
  {
    /* skip the char ':' */
    buf ++;
    bufSize --;
  }

  while ((size < LORAWAN_APP_DATA_BUFFER_MAX_SIZE) && (bufSize > 1))
  {
    hex[0] = buf[size * 2];
    hex[1] = buf[size * 2 + 1];
    if (tiny_sscanf(hex, "%hhx", &AppData.Buffer[size]) != 1)
    {
      return AT_PARAM_ERROR;
    }
    size++;
    bufSize -= 2;
  }
  if (bufSize != 0)
  {
    return AT_PARAM_ERROR;
  }

  AppData.BufferSize = size;
  AppData.Port = appPort;

  lmhStatus = LmHandlerSend(&AppData, isTxConfirmed, &nextTxIn, false);

  switch (lmhStatus)
  {
  case LORAMAC_HANDLER_SUCCESS:
    status = (nextTxIn > 0)?AT_DUTYCYLE_RESTRICTED:AT_OK;
    break;
  case LORAMAC_HANDLER_BUSY_ERROR:
  case LORAMAC_HANDLER_COMPLIANCE_RUNNING:
    status = (LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)?AT_NO_NET_JOINED:AT_BUSY_ERROR;
    break;
  case LORAMAC_HANDLER_NO_NETWORK_JOINED:
    status = AT_NO_NET_JOINED;
    break;
  case LORAMAC_HANDLER_DUTYCYCLE_RESTRICTED:
    status = AT_DUTYCYLE_RESTRICTED;
    break;
  case LORAMAC_HANDLER_CRYPTO_ERROR:
    status = AT_CRYPTO_ERROR;
    break;
  case LORAMAC_HANDLER_ERROR:
  default:
    status = AT_ERROR;
    break;
  }

  return status;
  /* USER CODE BEGIN AT_Send_2 */

  /* USER CODE END AT_Send_2 */
}

ATEerror_t AT_version_get(const char *param)
{
  /* USER CODE BEGIN AT_version_get_1 */

  /* USER CODE END AT_version_get_1 */
  /* Get LoRa APP version*/
  AT_PRINTF("APP_VERSION:        V%X.%X.%X\r\n",
            (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_MAIN_SHIFT),
            (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_SUB1_SHIFT),
            (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_SUB2_SHIFT));

  /* Get MW LoraWAN info */
  AT_PRINTF("MW_LORAWAN_VERSION: V%X.%X.%X\r\n",
            (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_MAIN_SHIFT),
            (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_SUB1_SHIFT),
            (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_SUB2_SHIFT));

  /* Get MW SubGhz_Phy info */
  AT_PRINTF("MW_RADIO_VERSION:   V%X.%X.%X\r\n",
            (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_MAIN_SHIFT),
            (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_SUB1_SHIFT),
            (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_SUB2_SHIFT));

  return AT_OK;
  /* USER CODE BEGIN AT_version_get_2 */

  /* USER CODE END AT_version_get_2 */
}

ATEerror_t AT_bat_get(const char *param)
{
  /* USER CODE BEGIN AT_bat_get_1 */

  /* USER CODE END AT_bat_get_1 */
  print_d(SYS_GetBatteryLevel());

  return AT_OK;
  /* USER CODE BEGIN AT_bat_get_2 */

  /* USER CODE END AT_bat_get_2 */
}

ATEerror_t AT_test_txTone(const char *param)
{
  /* USER CODE BEGIN AT_test_txTone_1 */

  /* USER CODE END AT_test_txTone_1 */
  if (0U == TST_TxTone())
  {
    return AT_OK;
  }
  else
  {
    return AT_BUSY_ERROR;
  }
  /* USER CODE BEGIN AT_test_txTone_2 */

  /* USER CODE END AT_test_txTone_2 */
}

ATEerror_t AT_test_rxRssi(const char *param)
{
  /* USER CODE BEGIN AT_test_rxRssi_1 */

  /* USER CODE END AT_test_rxRssi_1 */
  if (0U == TST_RxRssi())
  {
    return AT_OK;
  }
  else
  {
    return AT_BUSY_ERROR;
  }
  /* USER CODE BEGIN AT_test_rxRssi_2 */

  /* USER CODE END AT_test_rxRssi_2 */
}

ATEerror_t AT_test_tx(const char *param)
{
  /* USER CODE BEGIN AT_test_tx_1 */

  /* USER CODE END AT_test_tx_1 */
  const char *buf = param;
  uint32_t nb_packet;

  if (1 != tiny_sscanf(buf, "%u", &nb_packet))
  {
    AT_PRINTF("AT+TTX: nb packets sent is missing\r\n");
    return AT_PARAM_ERROR;
  }

  if (0U == TST_TX_Start(nb_packet))
  {
    return AT_OK;
  }
  else
  {
    return AT_BUSY_ERROR;
  }
  /* USER CODE BEGIN AT_test_tx_2 */

  /* USER CODE END AT_test_tx_2 */
}

/**
  * @brief  Start Tx hopping
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_tx_hopping(const char *param)
{
  /* USER CODE BEGIN AT_test_tx_hopping_1 */

  /* USER CODE END AT_test_tx_hopping_1 */
  const char *buf = param;
  uint32_t freq_start;
  uint32_t freq_stop;
  uint32_t delta_f;
  uint32_t nb_tx;

  testParameter_t test_param;
  uint32_t hop_freq;

  if (4 != tiny_sscanf(buf, "%u,%u,%u,%u", &freq_start, &freq_stop, &delta_f, &nb_tx))
  {
    return AT_PARAM_ERROR;
  }

  /*if freq is set in MHz, convert to Hz*/
  if (freq_start < 1000)
  {
    freq_start *= 1000000;
  }
  if (freq_stop < 1000)
  {
    freq_stop *= 1000000;
  }
  /**/
  hop_freq = freq_start;

  for (int i = 0; i < nb_tx; i++)
  {
    /*get current config*/
    TST_get_config(&test_param);

    /*increment frequency*/
    test_param.freq = hop_freq;
    /*Set new config*/
    TST_set_config(&test_param);

    APP_TPRINTF("Tx Hop at %dHz. %d of %d\r\n", hop_freq, i, nb_tx);

    if (0U != TST_TX_Start(1))
    {
      return AT_BUSY_ERROR;
    }

    hop_freq += delta_f;

    if (hop_freq > freq_stop)
    {
      hop_freq = freq_start;
    }
  }

  return AT_OK;
  /* USER CODE BEGIN AT_test_tx_hopping_2 */

  /* USER CODE END AT_test_tx_hopping_2 */
}

ATEerror_t AT_test_rx(const char *param)
{
  /* USER CODE BEGIN AT_test_rx_1 */

  /* USER CODE END AT_test_rx_1 */
  const char *buf = param;
  uint32_t nb_packet;

  if (1 != tiny_sscanf(buf, "%u", &nb_packet))
  {
    AT_PRINTF("AT+TRX: nb expected packets is missing\r\n");
    return AT_PARAM_ERROR;
  }

  if (0U == TST_RX_Start(nb_packet))
  {
    return AT_OK;
  }
  else
  {
    return AT_BUSY_ERROR;
  }
  /* USER CODE BEGIN AT_test_rx_2 */

  /* USER CODE END AT_test_rx_2 */
}

ATEerror_t AT_write_register(const char *param)
{
  /* USER CODE BEGIN AT_write_register_1 */

  /* USER CODE END AT_write_register_1 */
  uint8_t add[2];
  uint16_t add16;
  uint8_t data;

  if (strlen(param) != 7)
  {
    return AT_PARAM_ERROR;
  }

  if (stringToData(param, add, 2) != 0)
  {
    return AT_PARAM_ERROR;
  }
  param += 5;
  if (stringToData(param, &data, 1) != 0)
  {
    return AT_PARAM_ERROR;
  }
  add16 = (((uint16_t)add[0]) << 8) + (uint16_t)add[1];
  Radio.Write(add16, data);

  return AT_OK;
  /* USER CODE BEGIN AT_write_register_2 */

  /* USER CODE END AT_write_register_2 */
}

ATEerror_t AT_read_register(const char *param)
{
  /* USER CODE BEGIN AT_read_register_1 */

  /* USER CODE END AT_read_register_1 */
  uint8_t add[2];
  uint16_t add16;
  uint8_t data;

  if (strlen(param) != 4)
  {
    return AT_PARAM_ERROR;
  }

  if (stringToData(param, add, 2) != 0)
  {
    return AT_PARAM_ERROR;
  }

  add16 = (((uint16_t)add[0]) << 8) + (uint16_t)add[1];
  data = Radio.Read(add16);
  AT_PRINTF("REG 0x%04X=0x%02X", add16, data);

  return AT_OK;
  /* USER CODE BEGIN AT_read_register_2 */

  /* USER CODE END AT_read_register_2 */
}

ATEerror_t AT_verbose_set(const char *param)
{
  /* USER CODE BEGIN AT_verbose_set_1 */

  /* USER CODE END AT_verbose_set_1 */
  const char *buf = param;
  int32_t lvl_nb;

  /* read and set the verbose level */
  if (1 != tiny_sscanf(buf, "%u", &lvl_nb))
  {
    AT_PRINTF("AT+VL: verbose level is not well set\r\n");
    return AT_PARAM_ERROR;
  }
  if ((lvl_nb > VLEVEL_H) || (lvl_nb < VLEVEL_OFF))
  {
    AT_PRINTF("AT+VL: verbose level out of range => 0(VLEVEL_OFF) to 3(VLEVEL_H)\r\n");
    return AT_PARAM_ERROR;
  }

  UTIL_ADV_TRACE_SetVerboseLevel(lvl_nb);

  return AT_OK;
  /* USER CODE BEGIN AT_verbose_set_2 */

  /* USER CODE END AT_verbose_set_2 */
}

ATEerror_t AT_verbose_get(const char *param)
{
  /* USER CODE BEGIN AT_verbose_get_1 */

  /* USER CODE END AT_verbose_get_1 */
  print_u(UTIL_ADV_TRACE_GetVerboseLevel());
  return AT_OK;
  /* USER CODE BEGIN AT_verbose_get_2 */

  /* USER CODE END AT_verbose_get_2 */
}

ATEerror_t AT_test_get_config(const char *param)
{
  /* USER CODE BEGIN AT_test_get_config_1 */

  /* USER CODE END AT_test_get_config_1 */
  testParameter_t testParam;
  uint32_t loraBW[7] = {7812, 15625, 31250, 62500, 125000, 250000, 500000};

  TST_get_config(&testParam);

  AT_PRINTF("1: Freq= %d Hz\r\n", testParam.freq);
  AT_PRINTF("2: Power= %d dBm\r\n", testParam.power);

  if (testParam.modulation == 0)
  {
    /*fsk*/
    AT_PRINTF("3: Bandwidth= %d kHz\r\n", testParam.bandwidth);
    AT_PRINTF("4: FSK datarate= %d bps\r\n", testParam.loraSf_datarate);
    AT_PRINTF("5: Coding Rate not applicable\r\n");
    AT_PRINTF("6: LNA State= %d  \r\n", testParam.lna);
    AT_PRINTF("7: PA Boost State= %d  \r\n", testParam.paBoost);
    AT_PRINTF("8: modulation FSK\r\n");
    AT_PRINTF("9: Payload len= %d Bytes\r\n", testParam.payloadLen);
    AT_PRINTF("10: FSK deviation= %d \r\n", testParam.fskDev);
    AT_PRINTF("11: LowDRopt not applicable\r\n");
    AT_PRINTF("12: FSK gaussian BT product= %d \r\n", testParam.BTproduct);
  }
  else if (testParam.modulation == 1)
  {
    /*Lora*/
    AT_PRINTF("3: Bandwidth= %d Hz\r\n", loraBW[testParam.bandwidth]);
    AT_PRINTF("4: SF= %d \r\n", testParam.loraSf_datarate);
    AT_PRINTF("5: CR= 4/%d \r\n", testParam.codingRate + 4);
    AT_PRINTF("6: LNA State= %d  \r\n", testParam.lna);
    AT_PRINTF("7: PA Boost State= %d  \r\n", testParam.paBoost);
    AT_PRINTF("8: modulation LORA\r\n");
    AT_PRINTF("9: Payload len= %d Bytes\r\n", testParam.payloadLen);
    AT_PRINTF("10: Frequency deviation not applicable\r\n");
    AT_PRINTF("11: LowDRopt[0 to 2]= %d \r\n", testParam.lowDrOpt);
    AT_PRINTF("12 BT product not applicable\r\n");
  }
  else
  {
    AT_PRINTF("4: BPSK datarate= %d bps\r\n", testParam.loraSf_datarate);
  }

  AT_PRINTF("can be copy/paste in set cmd: AT+TCONF=%d:%d:%d:%d:4/%d:%d:%d:%d:%d:%d:%d:%d\r\n", testParam.freq,
            testParam.power,
            testParam.bandwidth, testParam.loraSf_datarate, testParam.codingRate + 4, \
            testParam.lna, testParam.paBoost, testParam.modulation, testParam.payloadLen, testParam.fskDev, testParam.lowDrOpt,
            testParam.BTproduct);
  return AT_OK;
  /* USER CODE BEGIN AT_test_get_config_2 */

  /* USER CODE END AT_test_get_config_2 */
}

ATEerror_t AT_test_set_config(const char *param)
{
  /* USER CODE BEGIN AT_test_set_config_1 */

  /* USER CODE END AT_test_set_config_1 */
  testParameter_t testParam;
  int32_t power = 14;
  uint32_t crNum;
  uint32_t freq;
  uint32_t loraSf_datarate;
  uint32_t bandwidth;
  uint32_t codingRate;
  uint32_t lna;
  uint32_t paBoost;
  uint32_t modulation = 1; /*Lora default in legacy*/
  uint32_t payloadLen = 16; /*Lora default in legacy*/
  uint32_t fskdDeviation = 25000; /*Lora default in legacy*/
  uint32_t lowDrOpt = 2; /*Lora default in legacy*/
  uint32_t BTproduct = 3; /*Lora default in legacy*/

  if (13 == tiny_sscanf(param, "%d:%d:%d:%d:%d/%d:%d:%d:%d:%d:%d:%d:%d",
                        &freq,
                        &power,
                        &bandwidth,
                        &loraSf_datarate,
                        &crNum,
                        &codingRate,
                        &lna,
                        &paBoost,
                        &modulation,
                        &payloadLen,
                        &fskdDeviation,
                        &lowDrOpt,
                        &BTproduct))
  {
    /*extend to new format for extended*/
  }
  else if (8 == tiny_sscanf(param, "%d:%d:%d:%d:%d/%d:%d:%d",
                            &freq,
                            &power,
                            &bandwidth,
                            &loraSf_datarate,
                            &crNum,
                            &codingRate,
                            &lna,
                            &paBoost))
  {
    /*legacy*/
    if (bandwidth == 500)
    {
      bandwidth = BW_500kHz;
    }
    else if (bandwidth == 250)
    {
      bandwidth = BW_250kHz;
    }
    else if (bandwidth == 125)
    {
      bandwidth = BW_125kHz;
    }
    else
    {
      return AT_PARAM_ERROR;
    }
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  /* Power  check and set*/
  if ((power >= -9) && (power <= 22))
  {
    testParam.power = power;
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  /* payloadLen check and set*/
  if ((payloadLen != 0) && (payloadLen <= 256))
  {
    testParam.payloadLen = payloadLen;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  /*modulation check and set*/
  if (modulation == 0)
  {
    /*modulation fsk=0 or lora=1*/
    testParam.modulation = TEST_FSK;
  }
  else if (modulation == 1)
  {
    /*modulation fsk=0 or lora=1*/
    testParam.modulation = TEST_LORA;
  }
  else if (modulation == 2)
  {
    /*modulation fsk=0 or lora=1*/
    testParam.modulation = TEST_BPSK;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  /* frequency check and set*/
  if (freq < 1000)
  {
    /*given in MHz*/
    testParam.freq = freq * 1000000;
  }
  else
  {
    testParam.freq = freq;
  }

  /* bandwidth check and set*/
  if (testParam.modulation == 0)
  {
    /*fsk bandwidth check and set*/
    if ((bandwidth >= 4800) && (bandwidth <= 467000))
    {
      testParam.bandwidth = bandwidth;
    }
    else
    {

      return AT_PARAM_ERROR;
    }
  }
  else if (testParam.modulation == 1)
  {
    /*Lora modulation check and set*/
    if (bandwidth <= BW_500kHz)
    {
      testParam.bandwidth = bandwidth;
    }
    else
    {
      return AT_PARAM_ERROR;
    }
  }
  else
  {
  }

  if (lowDrOpt <= 2)
  {
    testParam.lowDrOpt = lowDrOpt;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  if (testParam.modulation == 0)
  {
    /*fsk modulation check and set*/
    if ((loraSf_datarate >= 600) && (loraSf_datarate <= 300000))
    {
      testParam.loraSf_datarate = loraSf_datarate;
    }
    else
    {
      return AT_PARAM_ERROR;
    }
  }
  else if (testParam.modulation == 1)
  {
    /*Lora modulation*/
    /*spreading factor check and set*/
    if ((loraSf_datarate >= 5) && (loraSf_datarate <= 12))
    {
      testParam.loraSf_datarate = loraSf_datarate;
    }
    else
    {
      return AT_PARAM_ERROR;
    }
  }
  else if (testParam.modulation == 2)
  {
    /*BPSK modulation*/
    /*spreading factor check and set*/
    if (loraSf_datarate <= 1000)
    {
      testParam.loraSf_datarate = loraSf_datarate;
    }
    else
    {
      return AT_PARAM_ERROR;
    }
  }

  /* coding rate numerator check and set*/
  if (crNum != 4)
  {
    return AT_PARAM_ERROR;
  }
  /* check coding rate denominator is ok*/
  if ((codingRate >= 5) && (codingRate <= 8))
  {
    testParam.codingRate = codingRate - 4;
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  /* fsk BT product check and set*/
  if (BTproduct <= 4)
  {
    /*given in MHz*/
    testParam.BTproduct = BTproduct;
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  /* fskdDeviation check and se*/
  if ((fskdDeviation >= 600) && (fskdDeviation <= 200000))
  {
    /*given in MHz*/
    testParam.fskDev = fskdDeviation;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  testParam.lna = lna;
  testParam.paBoost = paBoost;

  TST_set_config(&testParam);

  return AT_OK;
  /* USER CODE BEGIN AT_test_set_config_2 */

  /* USER CODE END AT_test_set_config_2 */
}

ATEerror_t AT_test_stop(const char *param)
{
  /* USER CODE BEGIN AT_test_stop_1 */

  /* USER CODE END AT_test_stop_1 */
  TST_stop();
  AT_PRINTF("Test Stop\r\n");
  return AT_OK;
  /* USER CODE BEGIN AT_test_stop_2 */

  /* USER CODE END AT_test_stop_2 */
}

ATEerror_t AT_Certif(const char *param)
{
  /* USER CODE BEGIN AT_Certif_1 */

  /* USER CODE END AT_Certif_1 */
  switch (param[0])
  {
    case '0':
      LmHandlerJoin(ACTIVATION_TYPE_ABP);
    case '1':
      LmHandlerJoin(ACTIVATION_TYPE_OTAA);
      break;
    default:
      return AT_PARAM_ERROR;
  }

  UTIL_TIMER_Create(&TxCertifTimer,  0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnCertifTimer, NULL);  /* 8s */
  UTIL_TIMER_SetPeriod(&TxCertifTimer,  8000);  /* 8s */
  UTIL_TIMER_Start(&TxCertifTimer);
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_LoRaCertifTx), UTIL_SEQ_RFU, CertifSend);

  return AT_OK;
  /* USER CODE BEGIN AT_Certif_2 */

  /* USER CODE END AT_Certif_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
static int32_t sscanf_uint32_as_hhx(const char *from, uint32_t *value)
{
  /* USER CODE BEGIN sscanf_uint32_as_hhx_1 */

  /* USER CODE END sscanf_uint32_as_hhx_1 */
  return tiny_sscanf(from, "%hhx:%hhx:%hhx:%hhx",
                     &((unsigned char *)(value))[3],
                     &((unsigned char *)(value))[2],
                     &((unsigned char *)(value))[1],
                     &((unsigned char *)(value))[0]);
  /* USER CODE BEGIN sscanf_uint32_as_hhx_2 */

  /* USER CODE END sscanf_uint32_as_hhx_2 */
}

static void print_uint32_as_02x(uint32_t value)
{
  /* USER CODE BEGIN print_uint32_as_02x_1 */

  /* USER CODE END print_uint32_as_02x_1 */
  AT_PRINTF("%02x:%02x:%02x:%02x\r\n",
            (unsigned)((unsigned char *)(&value))[3],
            (unsigned)((unsigned char *)(&value))[2],
            (unsigned)((unsigned char *)(&value))[1],
            (unsigned)((unsigned char *)(&value))[0]);
  /* USER CODE BEGIN print_uint32_as_02x_2 */

  /* USER CODE END print_uint32_as_02x_2 */
}

static void print_8_02x(uint8_t *pt)
{
  /* USER CODE BEGIN print_8_02x_1 */

  /* USER CODE END print_8_02x_1 */
  AT_PRINTF("%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
            pt[0], pt[1], pt[2], pt[3], pt[4], pt[5], pt[6], pt[7]);
  /* USER CODE BEGIN print_8_02x_2 */

  /* USER CODE END print_8_02x_2 */
}

static void print_d(int32_t value)
{
  /* USER CODE BEGIN print_d_1 */

  /* USER CODE END print_d_1 */
  AT_PRINTF("%d\r\n", value);
  /* USER CODE BEGIN print_d_2 */

  /* USER CODE END print_d_2 */
}

static void print_u(uint32_t value)
{
  /* USER CODE BEGIN print_u_1 */

  /* USER CODE END print_u_1 */
  AT_PRINTF("%u\r\n", value);
  /* USER CODE BEGIN print_u_2 */

  /* USER CODE END print_u_2 */
}

static void OnCertifTimer(void *context)
{
  /* USER CODE BEGIN OnCertifTimer_1 */

  /* USER CODE END OnCertifTimer_1 */
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_LoRaCertifTx), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN OnCertifTimer_2 */

  /* USER CODE END OnCertifTimer_2 */
}

static void CertifSend(void)
{
  /* USER CODE BEGIN CertifSend_1 */

  /* USER CODE END CertifSend_1 */
  AppData.Buffer[0] = 0x43;
  AppData.BufferSize = 1;
  AppData.Port = 99;

  /* Restart Tx to prevent a previous Join Failed */
  if(LmHandlerJoinStatus() != LORAMAC_HANDLER_SET)
  {
    UTIL_TIMER_Start(&TxCertifTimer);
  }
  LmHandlerSend(&AppData, LORAMAC_HANDLER_UNCONFIRMED_MSG, NULL, false);
}

static uint8_t Char2Nibble(char Char)
{
  if (((Char >= '0') && (Char <= '9')))
  {
    return Char - '0';
  }
  else if (((Char >= 'a') && (Char <= 'f')))
  {
    return Char - 'a' + 10;
  }
  else if ((Char >= 'A') && (Char <= 'F'))
  {
    return Char - 'A' + 10;
  }
  else
  {
    return 0xF0;
  }
  /* USER CODE BEGIN CertifSend_2 */

  /* USER CODE END CertifSend_2 */
}

static int32_t stringToData(const char *str, uint8_t *data, uint32_t Size)
{
  /* USER CODE BEGIN stringToData_1 */

  /* USER CODE END stringToData_1 */
  char hex[3];
  hex[2] = 0;
  int32_t ii = 0;
  while (Size-- > 0)
  {
    hex[0] = *str++;
    hex[1] = *str++;

    /*check if input is hexa */
    if ((isHex(hex[0]) == -1) || (isHex(hex[1]) == -1))
    {
      return -1;
    }
    /*check if input is even nb of character*/
    if ((hex[1] == '\0') || (hex[1] == ','))
    {
      return -1;
    }
    data[ii] = (Char2Nibble(hex[0]) << 4) + Char2Nibble(hex[1]);

    ii++;
  }

  return 0;
  /* USER CODE BEGIN stringToData_2 */

  /* USER CODE END stringToData_2 */
}

static int32_t isHex(char Char)
{
  /* USER CODE BEGIN isHex_1 */

  /* USER CODE END isHex_1 */
  if (((Char >= '0') && (Char <= '9')) ||
      ((Char >= 'a') && (Char <= 'f')) ||
      ((Char >= 'A') && (Char <= 'F')))
  {
    return 0;
  }
  else
  {
    return -1;
  }
  /* USER CODE BEGIN isHex_2 */

  /* USER CODE END isHex_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
