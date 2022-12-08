/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    test_rf.c
  * @author  MCD Application Team
  * @brief   manages tx tests
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
#include "sys_app.h"
#include "test_rf.h"
#include "radio.h"
#include "stm32_seq.h"
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
#define F_868MHz                      868000000
#define P_14dBm                       14
#define P_22dBm                       22
#define SF12                          12
#define CR4o5                         1
#define EMISSION_POWER                P_14dBm
#define CONTINUOUS_TIMEOUT           0xFFFF
#define LORA_PREAMBLE_LENGTH          8         /* Same for Tx and Rx */
#define LORA_SYMBOL_TIMEOUT           30        /* Symbols */
#define TX_TIMEOUT_VALUE              3000
#define LORA_FIX_LENGTH_PAYLOAD_OFF   false
#define LORA_IQ_INVERSION_OFF         false
#define TX_TEST_TONE                  (1<<0)
#define RX_TEST_RSSI                  (1<<1)
#define TX_TEST_MODU                  (1<<2)
#define RX_TEST_MODU                  (1<<3)
#define RX_TIMEOUT_VALUE              5000
#define RX_CONTINUOUS_ON              1
#define PRBS9_INIT                    ( ( uint16_t) 2 )
#define DEFAULT_PAYLOAD_LEN           16
#define DEFAULT_LDR_OPT               2
#define DEFAULT_FSK_DEVIATION         25000
#define DEFAULT_GAUSS_BT              3 /*Lora default in legacy*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static uint8_t TestState = 0;

static testParameter_t testParam = { TEST_LORA, F_868MHz, EMISSION_POWER, BW_125kHz, SF12, CR4o5, 0, 0, DEFAULT_PAYLOAD_LEN, DEFAULT_FSK_DEVIATION, DEFAULT_LDR_OPT, DEFAULT_GAUSS_BT};

static __IO uint32_t RadioTxDone_flag = 0;
static __IO uint32_t RadioTxTimeout_flag = 0;
static __IO uint32_t RadioRxDone_flag = 0;
static __IO uint32_t RadioRxTimeout_flag = 0;
static __IO uint32_t RadioError_flag = 0;
static __IO int16_t last_rx_rssi = 0;
static __IO int8_t last_rx_LoraSnr_FskCfo = 0;

/*!
 * Radio events function pointer
 */
static RadioEvents_t RadioEvents;

/*!
 * Radio test payload pointer
 */
static uint8_t payload[256] = {0};

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/*!
 * \brief Generates a PRBS9 sequence
 */
static int32_t Prbs9_generator(uint8_t *payload, uint8_t len);
/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone(void);

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout(void);

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout(void);

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int32_t TST_TxTone(void)
{
  /* USER CODE BEGIN TST_TxTone_1 */

  /* USER CODE END TST_TxTone_1 */
  if ((TestState & TX_TEST_TONE) != TX_TEST_TONE)
  {
    TestState |= TX_TEST_TONE;

    APP_TPRINTF("Tx FSK Test\r\n");

    Radio.SetTxContinuousWave(testParam.freq, testParam.power, CONTINUOUS_TIMEOUT);

    return 0;
  }
  else
  {
    return -1;
  }
  /* USER CODE BEGIN TST_TxTone_2 */

  /* USER CODE END TST_TxTone_2 */
}

int32_t TST_RxRssi(void)
{
  /* USER CODE BEGIN TST_RxRssi_1 */

  /* USER CODE END TST_RxRssi_1 */
  uint32_t timeout = 0;
  int16_t rssiVal = 0;
  RxConfigGeneric_t RxConfig;
  /* Test with LNA */
  /* check that test is not already started*/
  if ((TestState & RX_TEST_RSSI) != RX_TEST_RSSI)
  {
    TestState |= RX_TEST_RSSI;

    APP_TPRINTF("Rx FSK Test\r\n");

    Radio.SetChannel(testParam.freq);

    /* RX Continuous */
    uint8_t syncword[] = { 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 };
    RxConfig.fsk.ModulationShaping = (RADIO_FSK_ModShapings_t)((testParam.BTproduct == 0) ? 0 : testParam.BTproduct + 8);
    RxConfig.fsk.Bandwidth = testParam.bandwidth;
    RxConfig.fsk.BitRate = testParam.loraSf_datarate; /*BitRate*/
    RxConfig.fsk.PreambleLen = 3;   /*in Byte*/
    RxConfig.fsk.SyncWordLength = 3; /*in Byte*/
    RxConfig.fsk.SyncWord = syncword; /*SyncWord Buffer*/
    RxConfig.fsk.whiteSeed = 0x01FF; /*WhiteningSeed*/
    RxConfig.fsk.LengthMode = RADIO_FSK_PACKET_VARIABLE_LENGTH; /* If the header is explicit, it will be transmitted in the GFSK packet. If the header is implicit, it will not be transmitted*/
    RxConfig.fsk.CrcLength = RADIO_FSK_CRC_2_BYTES_CCIT;       /* Size of the CRC block in the GFSK packet*/
    RxConfig.fsk.CrcPolynomial = 0x1021;
    RxConfig.fsk.Whitening = RADIO_FSK_DC_FREEWHITENING;
    Radio.RadioSetRxGenericConfig(GENERIC_FSK, &RxConfig, RX_CONTINUOUS_ON, 0);

    timeout = 0xFFFFFF; /* continuous Rx */
    if (testParam.lna == 0)
    {
      Radio.Rx(timeout);
    }
    else
    {
      Radio.RxBoosted(timeout);
    }

    HAL_Delay(Radio.GetWakeupTime());   /* Wait for 50ms */

    rssiVal = Radio.Rssi(MODEM_FSK);
    APP_TPRINTF(">>> RSSI Value= %d dBm\r\n", rssiVal);

    Radio.Sleep();
    TestState &= ~RX_TEST_RSSI;
    return 0;
  }
  else
  {
    return -1;
  }
  /* USER CODE BEGIN TST_RxRssi_2 */

  /* USER CODE END TST_RxRssi_2 */
}

int32_t  TST_set_config(testParameter_t *Param)
{
  /* USER CODE BEGIN TST_set_config_1 */

  /* USER CODE END TST_set_config_1 */
  UTIL_MEM_cpy_8(&testParam, Param, sizeof(testParameter_t));

  return 0;
  /* USER CODE BEGIN TST_set_config_2 */

  /* USER CODE END TST_set_config_2 */
}

int32_t TST_get_config(testParameter_t *Param)
{
  /* USER CODE BEGIN TST_get_config_1 */

  /* USER CODE END TST_get_config_1 */
  UTIL_MEM_cpy_8(Param, &testParam, sizeof(testParameter_t));
  return 0;
  /* USER CODE BEGIN TST_get_config_2 */

  /* USER CODE END TST_get_config_2 */
}

int32_t TST_stop(void)
{
  /* USER CODE BEGIN TST_stop_1 */

  /* USER CODE END TST_stop_1 */
  TestState = 0;

  /* Set the radio in Sleep*/
  Radio.Sleep();

  return 0;
  /* USER CODE BEGIN TST_stop_2 */

  /* USER CODE END TST_stop_2 */
}

int32_t TST_TX_Start(int32_t nb_packet)
{
  /* USER CODE BEGIN TST_TX_Start_1 */

  /* USER CODE END TST_TX_Start_1 */
  int32_t i;
  TxConfigGeneric_t TxConfig;

  if ((TestState & TX_TEST_MODU) != TX_TEST_MODU)
  {
    TestState |= TX_TEST_MODU;

    APP_TPRINTF("Tx Test\r\n");

    /* Radio initialization */
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    Radio.Init(&RadioEvents);
    /*Fill payload with PRBS9 data*/
    Prbs9_generator(payload, testParam.payloadLen);

    /* Launch several times payload: nb times given by user */
    for (i = 1; i <= nb_packet; i++)
    {
      APP_TPRINTF("Tx %d of %d\r\n", i, nb_packet);
      Radio.SetChannel(testParam.freq);

      if (testParam.modulation == TEST_FSK)
      {
        /*fsk modulation*/
        uint8_t syncword[] = { 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 };
        TxConfig.fsk.ModulationShaping = (RADIO_FSK_ModShapings_t)((testParam.BTproduct == 0) ? 0 : testParam.BTproduct + 7);
        TxConfig.fsk.FrequencyDeviation = testParam.fskDev;
        TxConfig.fsk.BitRate = testParam.loraSf_datarate; /*BitRate*/
        TxConfig.fsk.PreambleLen = 3;   /*in Byte        */
        TxConfig.fsk.SyncWordLength = 3; /*in Byte        */
        TxConfig.fsk.SyncWord = syncword; /*SyncWord Buffer*/
        TxConfig.fsk.whiteSeed = 0x01FF; /*WhiteningSeed  */
        TxConfig.fsk.HeaderType = RADIO_FSK_PACKET_VARIABLE_LENGTH; /* If the header is explicit, it will be transmitted in the GFSK packet. If the header is implicit, it will not be transmitted*/
        TxConfig.fsk.CrcLength = RADIO_FSK_CRC_2_BYTES_CCIT;       /* Size of the CRC block in the GFSK packet*/
        TxConfig.fsk.CrcPolynomial = 0x1021;
        TxConfig.fsk.Whitening = RADIO_FSK_DC_FREE_OFF;
        Radio.RadioSetTxGenericConfig(GENERIC_FSK, &TxConfig, testParam.power, TX_TIMEOUT_VALUE);
      }
      else if (testParam.modulation == TEST_MSK)
      {
        /*fsk modulation*/
        uint8_t syncword[] = { 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 };
        TxConfig.msk.ModulationShaping = (RADIO_FSK_ModShapings_t)((testParam.BTproduct == 0) ? 0 : testParam.BTproduct + 7);
        TxConfig.msk.BitRate = testParam.loraSf_datarate; /*BitRate*/
        TxConfig.msk.PreambleLen = 3;   /*in Byte        */
        TxConfig.msk.SyncWordLength = 3; /*in Byte        */
        TxConfig.msk.SyncWord = syncword; /*SyncWord Buffer*/
        TxConfig.msk.whiteSeed = 0x01FF; /*WhiteningSeed  */
        TxConfig.msk.HeaderType = RADIO_FSK_PACKET_VARIABLE_LENGTH; /* If the header is explicit, it will be transmitted in the GFSK packet. If the header is implicit, it will not be transmitted*/
        TxConfig.msk.CrcLength = RADIO_FSK_CRC_2_BYTES_CCIT;       /* Size of the CRC block in the GFSK packet*/
        TxConfig.msk.CrcPolynomial = 0x1021;
        TxConfig.msk.Whitening = RADIO_FSK_DC_FREE_OFF;
        Radio.RadioSetTxGenericConfig(GENERIC_MSK, &TxConfig, testParam.power, TX_TIMEOUT_VALUE);
      }
      else if (testParam.modulation == TEST_LORA)
      {
        /*lora modulation*/
        TxConfig.lora.Bandwidth = (RADIO_LoRaBandwidths_t) testParam.bandwidth;
        TxConfig.lora.SpreadingFactor = (RADIO_LoRaSpreadingFactors_t) testParam.loraSf_datarate; /*BitRate*/
        TxConfig.lora.Coderate = (RADIO_LoRaCodingRates_t)testParam.codingRate;
        TxConfig.lora.LowDatarateOptimize = (RADIO_Ld_Opt_t)testParam.lowDrOpt; /*0 inactive, 1 active, 2: auto*/
        TxConfig.lora.PreambleLen = LORA_PREAMBLE_LENGTH;
        TxConfig.lora.LengthMode = RADIO_LORA_PACKET_VARIABLE_LENGTH;
        TxConfig.lora.CrcMode = RADIO_LORA_CRC_ON;
        TxConfig.lora.IqInverted = RADIO_LORA_IQ_NORMAL;
        Radio.RadioSetTxGenericConfig(GENERIC_LORA, &TxConfig, testParam.power, TX_TIMEOUT_VALUE);
        Radio.SetPublicNetwork(false); /*set private syncword*/
      }
      else if (testParam.modulation == TEST_BPSK)
      {
        TxConfig.bpsk.BitRate = testParam.loraSf_datarate; /*BitRate*/
        Radio.RadioSetTxGenericConfig(GENERIC_BPSK, &TxConfig, testParam.power, TX_TIMEOUT_VALUE);
      }
      else
      {
        return -1; /*error*/
      }
      /* Send payload once*/
      Radio.Send(payload, testParam.payloadLen);
      /* Wait Tx done/timeout */
      UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
      Radio.Sleep();

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

      /*Delay between 2 consecutive Tx*/
      HAL_Delay(500);
      /* Reset TX Done or timeout flags */
      RadioTxDone_flag = 0;
      RadioTxTimeout_flag = 0;
      RadioError_flag = 0;
    }
    TestState &= ~TX_TEST_MODU;
    return 0;
  }
  else
  {
    return -1;
  }
  /* USER CODE BEGIN TST_TX_Start_2 */

  /* USER CODE END TST_TX_Start_2 */
}

int32_t TST_RX_Start(int32_t nb_packet)
{
  /* USER CODE BEGIN TST_RX_Start_1 */

  /* USER CODE END TST_RX_Start_1 */
  int32_t i;
  /* init of PER counter */
  uint32_t count_RxOk = 0;
  uint32_t count_RxKo = 0;
  uint32_t PER = 0;
  RxConfigGeneric_t RxConfig = {0};

  if (((TestState & RX_TEST_MODU) != RX_TEST_MODU) && (nb_packet > 0))
  {
    TestState |= RX_TEST_MODU;

    /* Radio initialization */
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;
    Radio.Init(&RadioEvents);

    for (i = 1; i <= nb_packet; i++)
    {
      /* Rx config */
      Radio.SetChannel(testParam.freq);

      if (testParam.modulation == TEST_FSK)
      {
        /*fsk modulation*/
        uint8_t syncword[] = { 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 };
        RxConfig.fsk.ModulationShaping = (RADIO_FSK_ModShapings_t)((testParam.BTproduct == 0) ? 0 : testParam.BTproduct + 8);
        RxConfig.fsk.Bandwidth = testParam.bandwidth;
        RxConfig.fsk.BitRate = testParam.loraSf_datarate; /*BitRate*/
        RxConfig.fsk.PreambleLen = 3; /*in Byte*/
        RxConfig.fsk.SyncWordLength = 3; /*in Byte*/
        RxConfig.fsk.SyncWord = syncword; /*SyncWord Buffer*/
        RxConfig.fsk.PreambleMinDetect = RADIO_FSK_PREAMBLE_DETECTOR_08_BITS;
        RxConfig.fsk.whiteSeed = 0x01FF; /*WhiteningSeed*/
        RxConfig.fsk.LengthMode = RADIO_FSK_PACKET_VARIABLE_LENGTH; /* If the header is explicit, it will be transmitted in the GFSK packet. If the header is implicit, it will not be transmitted*/
        RxConfig.fsk.CrcLength = RADIO_FSK_CRC_2_BYTES_CCIT;       /* Size of the CRC block in the GFSK packet*/
        RxConfig.fsk.CrcPolynomial = 0x1021;
        RxConfig.fsk.Whitening = RADIO_FSK_DC_FREE_OFF;
        RxConfig.fsk.MaxPayloadLength = 255;
        RxConfig.fsk.AddrComp = RADIO_FSK_ADDRESSCOMP_FILT_OFF;
        Radio.RadioSetRxGenericConfig(GENERIC_FSK, &RxConfig, RX_CONTINUOUS_ON, 0);
      }
      else if (testParam.modulation == TEST_LORA)
      {
        /*Lora*/
        RxConfig.lora.Bandwidth = (RADIO_LoRaBandwidths_t) testParam.bandwidth;
        RxConfig.lora.SpreadingFactor = (RADIO_LoRaSpreadingFactors_t) testParam.loraSf_datarate; /*BitRate*/
        RxConfig.lora.Coderate = (RADIO_LoRaCodingRates_t)testParam.codingRate;
        RxConfig.lora.LowDatarateOptimize = (RADIO_Ld_Opt_t)testParam.lowDrOpt; /*0 inactive, 1 active, 2: auto*/
        RxConfig.lora.PreambleLen = LORA_PREAMBLE_LENGTH;
        RxConfig.lora.LengthMode = RADIO_LORA_PACKET_VARIABLE_LENGTH;
        RxConfig.lora.CrcMode = RADIO_LORA_CRC_ON;
        RxConfig.lora.IqInverted = RADIO_LORA_IQ_NORMAL;
        Radio.RadioSetRxGenericConfig(GENERIC_LORA, &RxConfig, RX_CONTINUOUS_ON, LORA_SYMBOL_TIMEOUT);
        Radio.SetPublicNetwork(false); /*set private syncword*/
      }
      else
      {
        /* excluding MSK Rx */
        return -1; /* error */
      }

      if (testParam.lna == 0)
      {
        Radio.Rx(RX_TIMEOUT_VALUE);
      }
      else
      {
        Radio.RxBoosted(RX_TIMEOUT_VALUE);
      }

      /* Wait Rx done/timeout */
      UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
      Radio.Sleep();

      if (RadioRxDone_flag == 1)
      {
        int16_t rssi = last_rx_rssi;
        int8_t LoraSnr_FskCfo = last_rx_LoraSnr_FskCfo;
        APP_TPRINTF("OnRxDone\r\n");
        if (testParam.modulation == TEST_FSK)
        {
          APP_TPRINTF("RssiValue=%d dBm, cfo=%dkHz\r\n", rssi, LoraSnr_FskCfo);
        }
        else
        {
          APP_TPRINTF("RssiValue=%d dBm, SnrValue=%ddB\r\n", rssi, LoraSnr_FskCfo);
        }
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
      APP_TPRINTF("Rx %d of %d  >>> PER= %d %%\r\n", i, nb_packet, PER);
    }
    TestState &= ~RX_TEST_MODU;
    return 0;
  }
  else
  {
    return -1;
  }
  /* USER CODE BEGIN TST_RX_Start_2 */

  /* USER CODE END TST_RX_Start_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone_1 */

  /* USER CODE END OnTxDone_1 */
  /* Set TxDone flag */
  RadioTxDone_flag = 1;
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
  /* USER CODE BEGIN OnTxDone_2 */

  /* USER CODE END OnTxDone_2 */
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone_1 */

  /* USER CODE END OnRxDone_1 */
  last_rx_rssi = rssi;
  last_rx_LoraSnr_FskCfo = LoraSnr_FskCfo;

  /* Set Rxdone flag */
  RadioRxDone_flag = 1;
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
  /* USER CODE BEGIN OnRxDone_2 */

  /* USER CODE END OnRxDone_2 */
}

void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout_1 */

  /* USER CODE END OnTxTimeout_1 */
  /* Set timeout flag */
  RadioTxTimeout_flag = 1;
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
  /* USER CODE BEGIN OnTxTimeout_2 */

  /* USER CODE END OnTxTimeout_2 */
}

void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout_1 */

  /* USER CODE END OnRxTimeout_1 */
  /* Set timeout flag */
  RadioRxTimeout_flag = 1;
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
  /* USER CODE BEGIN OnRxTimeout_2 */

  /* USER CODE END OnRxTimeout_2 */
}

void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError_1 */

  /* USER CODE END OnRxError_1 */
  /* Set error flag */
  RadioError_flag = 1;
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_RadioOnTstRF);
  /* USER CODE BEGIN OnRxError_2 */

  /* USER CODE END OnRxError_2 */
}

static int32_t Prbs9_generator(uint8_t *payload, uint8_t len)
{
  /* USER CODE BEGIN Prbs9_generator_1 */

  /* USER CODE END Prbs9_generator_1 */
  uint16_t prbs9_val = PRBS9_INIT;
  /*init payload to 0*/
  UTIL_MEM_set_8(payload, 0, len);

  for (int32_t i = 0; i < len * 8; i++)
  {
    /*fill buffer with prbs9 sequence*/
    int32_t newbit = (((prbs9_val >> 8) ^ (prbs9_val >> 4)) & 1);
    prbs9_val = ((prbs9_val << 1) | newbit) & 0x01ff;
    payload[i / 8] |= ((prbs9_val & 0x1) << (i % 8));
  }
  return 0;
  /* USER CODE BEGIN Prbs9_generator_2 */

  /* USER CODE END Prbs9_generator_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
