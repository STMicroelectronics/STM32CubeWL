/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subghz_phy_app.c
  * @author  MCD Application Team
  * @brief   Application of the SubGHz_Phy Middleware
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
#include "sys_app.h"
#include "subghz_phy_app.h"
#include "radio.h"
#include "app_version.h"

/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "stm32_seq.h"
#include "utilities_def.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Configurations */
/*Timeout*/
#define RX_TIMEOUT_VALUE              2000
#define TX_TIMEOUT_VALUE              3000
/* Definitions */
#define RX_CONTINUOUS_ON              1
#define RADIO_TX                      0 /* do not change*/
#define RADIO_RX                      1 /* do not change*/
#define PRBS9_INIT                    ( ( uint16_t) 2 )

/* Test Configurations */
/*if TEST_MODE is RADIO_TX, board will send packet indefinitely*/
/*if TEST_MODE is RADIO_RX, board will receive packet indefinitely*/
#define TEST_MODE                     RADIO_TX

/* 0: Tx Long packet disable*/
/* 1: Tx Long packet enable(payload can be greater than 255bytes. Available on stm32wl revision Y)*/
#define APP_LONG_PACKET               0
/* Application buffer, can be increased further*/
#define MAX_APP_BUFFER_SIZE 1000
#if (PAYLOAD_LEN>MAX_APP_BUFFER_SIZE)
#error increase MAX_APP_BUFFER_SIZE
#endif /* (PAYLOAD_LEN>MAX_APP_BUFFER_SIZE) */

#if ((APP_LONG_PACKET==0) && PAYLOAD_LEN>255)
#error in case PAYLOAD_LEN>255, APP_LONG_PACKET shall be defined to 1
#endif /* ((APP_LONG_PACKET==0) && PAYLOAD_LEN>255) */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;
/* USER CODE BEGIN PV */

static __IO uint32_t RadioTxDone_flag = 0;
static __IO uint32_t RadioTxTimeout_flag = 0;
static __IO uint32_t RadioRxDone_flag = 0;
static __IO uint32_t RadioRxTimeout_flag = 0;
static __IO uint32_t RadioError_flag = 0;
static __IO int16_t last_rx_rssi = 0;
static __IO int8_t last_rx_cfo = 0;

uint8_t data_buffer[MAX_APP_BUFFER_SIZE] UTIL_MEM_ALIGN(4);
uint16_t data_offset = 0;

static __IO uint16_t payloadLen = PAYLOAD_LEN;

static uint8_t syncword[] = { 0xC1, 0x94, 0xC1};

uint32_t count_RxOk = 0;
uint32_t count_RxKo = 0;
uint32_t PER = 0;

static int packetCnt = 0;

/* TxPayloadMode
 * 0: byte Inc e.g payload=0x00, 0x01, ..,payloadLen-1
 * 1: prbs9  */
static __IO uint8_t TxPayloadMode = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
 * @brief Function to be executed on Radio Tx Done event
 */
static void OnTxDone(void);

/**
  * @brief Function to be executed on Radio Rx Done event
  * @param  payload ptr of buffer received
  * @param  size buffer size
  * @param  rssi
  * @param  LoraSnr_FskCfo
  */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);

/**
  * @brief Function executed on Radio Tx Timeout event
  */
static void OnTxTimeout(void);

/**
  * @brief Function executed on Radio Rx Timeout event
  */
static void OnRxTimeout(void);

/**
  * @brief Function executed on Radio Rx Error event
  */
static void OnRxError(void);

/* USER CODE BEGIN PFP */
/**
  * @brief Packet Error Rate state machine implementation
  */
static void Per_Process(void);

#if (TEST_MODE == RADIO_TX)
/**
  * @brief Generates a PRBS9 sequence
  * @retval 0
  */
static int32_t tx_payload_generator(void);
#endif /* TEST_MODE == RADIO_TX */

#if (APP_LONG_PACKET != 0)
/**
  * @brief Process next Tx chunk of payload
  * @param buffer
  * @param buffer_size
  */
void TxLongPacketGetNextChunk(uint8_t **buffer, uint8_t buffer_size);

/**
  * @brief Process next Rx chunk of payload
  * @param buffer
  * @param chunk_size
  */
void RxLongPacketChunk(uint8_t *buffer, uint8_t chunk_size);
#endif /* APP_LONG_PACKET != 0 */
/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */
#if (TEST_MODE == RADIO_RX)
  RxConfigGeneric_t RxConfig = {0};
  /* RX LEDs*/
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);
#elif (TEST_MODE == RADIO_TX)
  TxConfigGeneric_t TxConfig;
  BSP_PB_Init(BUTTON_SW1, BUTTON_MODE_EXTI);
  BSP_PB_Init(BUTTON_SW2, BUTTON_MODE_EXTI);
  BSP_PB_Init(BUTTON_SW3, BUTTON_MODE_EXTI);
  /* TX LED*/
  BSP_LED_Init(LED_BLUE);
#else
#endif /* TEST_MODE */
  /* Print APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "APP_VERSION= V%X.%X.%X\r\n",
          (uint8_t)(__APP_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__APP_VERSION >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(__APP_VERSION >> __APP_VERSION_SUB2_SHIFT));

  /* Init leds*/
  BSP_LED_Init(LED_GREEN);

  APP_LOG(TS_OFF, VLEVEL_M, "---------------\n\r");
  APP_LOG(TS_OFF, VLEVEL_M, "FSK_MODULATION\n\r");
  APP_LOG(TS_OFF, VLEVEL_M, "FSK_BW=%d Hz\n\r", FSK_BANDWIDTH);
  APP_LOG(TS_OFF, VLEVEL_M, "FSK_DR=%d bits/s\n\r", FSK_DATARATE);
#if (TEST_MODE == RADIO_RX)
  APP_LOG(TS_OFF, VLEVEL_M, "Rx Mode\n\r", FSK_DATARATE);
#elif (TEST_MODE == RADIO_TX)
  APP_LOG(TS_OFF, VLEVEL_M, "Tx Mode\n\r", FSK_DATARATE);
#endif /* TEST_MODE */
  /* USER CODE END SubghzApp_Init_1 */

  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

  /* USER CODE BEGIN SubghzApp_Init_2 */
  /* Radio Set frequency */
  Radio.SetChannel(RF_FREQUENCY);

  data_offset = 0;
#if (TEST_MODE == RADIO_RX)
  APP_TPRINTF("Rx FSK Test\r\n");
  /* RX Continuous */
  RxConfig.fsk.ModulationShaping = RADIO_FSK_MOD_SHAPING_G_BT_05;
  RxConfig.fsk.Bandwidth = FSK_BANDWIDTH;
  RxConfig.fsk.BitRate = FSK_DATARATE; /*BitRate*/
  RxConfig.fsk.PreambleLen = 4; /*in Byte*/
  RxConfig.fsk.SyncWordLength = sizeof(syncword); /*in Byte*/
  RxConfig.fsk.PreambleMinDetect = RADIO_FSK_PREAMBLE_DETECTOR_08_BITS;
  RxConfig.fsk.SyncWord = syncword; /*SyncWord Buffer*/
  RxConfig.fsk.whiteSeed = 0x01FF ; /*WhiteningSeed*/
#if (APP_LONG_PACKET==0)
  RxConfig.fsk.LengthMode  = RADIO_FSK_PACKET_VARIABLE_LENGTH; /* legacy: payload length field is 1 byte long*/
#else
  RxConfig.fsk.LengthMode  = RADIO_FSK_PACKET_2BYTES_LENGTH;  /* payload length field is 2 bytes long */
#endif /* APP_LONG_PACKET */
  RxConfig.fsk.CrcLength = RADIO_FSK_CRC_2_BYTES_IBM;       /* Size of the CRC block in the GFSK packet*/
  RxConfig.fsk.CrcPolynomial = 0x8005;
  RxConfig.fsk.CrcSeed = 0xFFFF;
  RxConfig.fsk.Whitening = RADIO_FSK_DC_FREEWHITENING;
  RxConfig.fsk.MaxPayloadLength = MAX_APP_BUFFER_SIZE;
  RxConfig.fsk.StopTimerOnPreambleDetect = 0;
  RxConfig.fsk.AddrComp = RADIO_FSK_ADDRESSCOMP_FILT_OFF;
  if (0UL != Radio.RadioSetRxGenericConfig(GENERIC_FSK, &RxConfig, RX_CONTINUOUS_ON, 0))
  {
    while (1);
  }
#if (APP_LONG_PACKET==0)
  Radio.Rx(RX_TIMEOUT_VALUE);
#else
  (void) Radio.ReceiveLongPacket(0, RX_TIMEOUT_VALUE, RxLongPacketChunk);
#endif /* APP_LONG_PACKET */

#elif (TEST_MODE == RADIO_TX)
  tx_payload_generator();

  /*fsk modulation*/
  TxConfig.fsk.ModulationShaping = RADIO_FSK_MOD_SHAPING_G_BT_05;
  TxConfig.fsk.Bandwidth = FSK_BANDWIDTH;
  TxConfig.fsk.FrequencyDeviation = FSK_FDEV;
  TxConfig.fsk.BitRate = FSK_DATARATE; /*BitRate*/
  TxConfig.fsk.PreambleLen = 4;   /*in Byte        */
  TxConfig.fsk.SyncWordLength = sizeof(syncword); /*in Byte        */
  TxConfig.fsk.SyncWord = syncword; /*SyncWord Buffer*/
  TxConfig.fsk.whiteSeed =  0x01FF ; /*WhiteningSeed  */
#if (APP_LONG_PACKET==0)
  TxConfig.fsk.HeaderType  = RADIO_FSK_PACKET_VARIABLE_LENGTH; /*legacy: payload length field is 1 byte long*/
#else
  TxConfig.fsk.HeaderType  = RADIO_FSK_PACKET_2BYTES_LENGTH;  /* payload length field is 2 bytes long */
#endif /* APP_LONG_PACKET */
  TxConfig.fsk.CrcLength = RADIO_FSK_CRC_2_BYTES_IBM;       /* Size of the CRC block in the GFSK packet*/
  TxConfig.fsk.CrcPolynomial = 0x8005;
  TxConfig.fsk.CrcSeed = 0xFFFF;
  TxConfig.fsk.Whitening = RADIO_FSK_DC_FREEWHITENING;
  if (0UL != Radio.RadioSetTxGenericConfig(GENERIC_FSK, &TxConfig, TX_OUTPUT_POWER, TX_TIMEOUT_VALUE))
  {
    while (1);
  }
#if (APP_LONG_PACKET==0)
  Radio.Send(data_buffer, payloadLen);
#else
  if (0UL != Radio.TransmitLongPacket(payloadLen, TX_TIMEOUT_VALUE, TxLongPacketGetNextChunk))
  {
    while (1);
  }
#endif /* APP_LONG_PACKET */
#else
#error should be either Tx or Rx
#endif /* TEST_MODE */

  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), UTIL_SEQ_RFU, Per_Process);
  /* USER CODE END SubghzApp_Init_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/

static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
  RadioTxDone_flag = 1;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxDone */
}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone */
  last_rx_rssi = rssi;
  last_rx_cfo = LoraSnr_FskCfo;

  /* Set Rxdone flag */
  RadioRxDone_flag = 1;
  /* Run Per process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
#if (APP_LONG_PACKET==0)
  memcpy(data_buffer, payload, size);
  payloadLen = size;
#else
  /*from chunk*/
  payloadLen = data_offset;
  /*payload data are not relevant in long packet mode*/
#endif /* APP_LONG_PACKET */
  /* USER CODE END OnRxDone */
}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
  RadioTxTimeout_flag = 1;
  /* Run Per process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxTimeout */
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout */
  RadioRxTimeout_flag = 1;
  /* Run Per process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxTimeout */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */
  RadioError_flag = 1;
  /* Run Per process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxError */
}

/* USER CODE BEGIN PrFD */
#if (APP_LONG_PACKET!=0)
void RxLongPacketChunk(uint8_t *buffer, uint8_t chunk_size)
{
  uint8_t *rxdata = &data_buffer[data_offset];
  uint8_t *rxbuffer = buffer;

  if (data_offset + chunk_size > MAX_APP_BUFFER_SIZE)
  {
    __NOP();
    return;
  }
  for (int i = 0; i < chunk_size; i++)
  {
    *rxdata++ = *rxbuffer++;
  }
  data_offset += chunk_size;
}

void TxLongPacketGetNextChunk(uint8_t **buffer, uint8_t chunk_size)
{
  *buffer = &data_buffer[data_offset];
  data_offset += chunk_size;
  /* APP_TPRINTF("Tx chunk: chunk_size=%d, data_offset=%d\r\n",chunk_size, data_offset); */
}
#endif /* APP_LONG_PACKET */
uint8_t buffer_error = 0;
static void Per_Process(void)
{
  packetCnt++;
  data_offset = 0;
#if (TEST_MODE == RADIO_RX)
  if (RadioRxDone_flag == 1)
  {
    int16_t rssi = last_rx_rssi;
    int8_t cfo = last_rx_cfo;
    BSP_LED_On(LED_GREEN) ;
    APP_TPRINTF("OnRxDone\r\n");
    APP_TPRINTF("RssiValue=%d dBm, cfo=%d kHz\r\n", rssi, cfo);
    APP_TPRINTF("payloadLen=%d bytes\r\n", payloadLen);
#if 0
    /* warning, delay between 2 consecutive Tx may be increased to allow DMA to empty printf queue*/
    APP_PPRINTF("data=0x \n\r");
    for (int i = 0; i < payloadLen; i++)
    {
      APP_PRINTF("%02X ", data_buffer[i]);
      if ((i % 16) == 15)
      {
        APP_PPRINTF("\n\r");
      }
    }
    APP_PPRINTF("\n\r");
#endif /* 0 */
  }
  else
  {
    BSP_LED_On(LED_RED);
  }

  if (RadioRxTimeout_flag == 1)
  {
    APP_TPRINTF("OnRxTimeout\r\n");
  }

  if (RadioError_flag == 1)
  {
    APP_TPRINTF("OnRxError\r\n");
  }

  /*check flag*/
  if ((RadioRxTimeout_flag == 1) || (RadioError_flag == 1))
  {
    count_RxKo++;
  }
  if (RadioRxDone_flag == 1)
  {
    count_RxOk++;
  }
  /* Reset timeout flag */
  RadioRxDone_flag = 0;
  RadioRxTimeout_flag = 0;
  RadioError_flag = 0;

  /* Compute PER */
  PER = (100 * (count_RxKo)) / (count_RxKo + count_RxOk);
  APP_TPRINTF("Rx %d>>> PER= %d %%\r\n", packetCnt, PER);
#if (APP_LONG_PACKET==0)
  Radio.Rx(RX_TIMEOUT_VALUE);
#else
  (void) Radio.ReceiveLongPacket(0, RX_TIMEOUT_VALUE, RxLongPacketChunk);
#endif /* APP_LONG_PACKET */
  HAL_Delay(10);
  BSP_LED_Off(LED_GREEN) ;
  BSP_LED_Off(LED_RED) ;
#elif (TEST_MODE == RADIO_TX)
  BSP_LED_Off(LED_BLUE) ;
  if (RadioTxDone_flag == 1)
  {
    APP_TPRINTF("OnTxDone\r\n");
  }

  if (RadioTxTimeout_flag == 1)
  {
    APP_TPRINTF("OnTxTimeout\r\n");
  }

  if (RadioError_flag == 1)
  {
    APP_TPRINTF("OnRxError\r\n");
  }
  /* this delay is only to give enough time to allow DMA to empty printf queue*/
  HAL_Delay(500);
  /* Reset TX Done or timeout flags */
  RadioTxDone_flag = 0;
  RadioTxTimeout_flag = 0;
  RadioError_flag = 0;

  tx_payload_generator();
#if (APP_LONG_PACKET==0)
  Radio.Send(data_buffer, payloadLen);
#else
  if (0UL != Radio.TransmitLongPacket(payloadLen, TX_TIMEOUT_VALUE, TxLongPacketGetNextChunk))
  {
    while (1);
  }
#endif /* APP_LONG_PACKET */
  APP_TPRINTF("Tx %d \r\n", packetCnt);
  BSP_LED_On(LED_BLUE) ;
#endif /* TEST_MODE */
}

#if (TEST_MODE == RADIO_TX)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case  BUTTON_SW1_PIN:
      /* Note: when "EventType == TX_ON_TIMER" this GPIO is not initialized */
      /*increment by 16*/
      payloadLen += 16;
      if (payloadLen > MAX_APP_BUFFER_SIZE)
      {
        payloadLen = 1;
      }
      APP_TPRINTF("New Tx Payload Length= %d\r\n", payloadLen);

      break;
    case  BUTTON_SW2_PIN:

      /*increment by 1*/
      payloadLen += 1;
      if (payloadLen > MAX_APP_BUFFER_SIZE)
      {
        payloadLen = 1;
      }
      APP_TPRINTF("New Tx Payload Length= %d\r\n", payloadLen);

      break;

    case  BUTTON_SW3_PIN:

      TxPayloadMode = (TxPayloadMode + 1) % 2;
      if (TxPayloadMode == 1)
      {
        APP_TPRINTF("Payload PRBS9 mode\r\n");
      }
      else
      {
        APP_TPRINTF("Payload Inc mode\r\n");
      }

      break;

    default:
      break;
  }
}

static int32_t tx_payload_generator(void)
{
  if (TxPayloadMode == 1)
  {
    uint16_t prbs9_val = PRBS9_INIT;
    for (int32_t i = 0; i < payloadLen; i++)
    {
      data_buffer[i] = 0;
    }
    for (int32_t i = 0; i < payloadLen * 8; i++)
    {
      /*fill buffer with prbs9 sequence*/
      int32_t newbit = (((prbs9_val >> 8) ^ (prbs9_val >> 4)) & 1);
      prbs9_val = ((prbs9_val << 1) | newbit) & 0x01ff;
      data_buffer[i / 8] |= ((prbs9_val & 0x1) << (i % 8));
    }
  }
  else
  {
    for (int32_t i = 0; i < payloadLen; i++)
    {
      data_buffer[i] = i;
    }
  }
  return 0;
}
#endif /* TEST_MODE */
/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
