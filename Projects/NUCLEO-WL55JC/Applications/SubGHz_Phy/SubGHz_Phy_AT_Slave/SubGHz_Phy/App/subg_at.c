/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subg_at.c
  * @author  MCD Application Team
  * @brief   AT command API
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
#include "subg_at.h"
#include "sys_app.h"
#include "stm32_tiny_sscanf.h"
#include "app_version.h"
#include "subghz_phy_version.h"
#include "test_rf.h"
#include "stm32_seq.h"
#include "utilities_def.h"
#include "radio.h"
#include "stm32_timer.h"
#include "stm32_systime.h"

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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Print an unsigned int
  * @param  value to print
  */
static void print_u(uint32_t value);

/**
  * @brief  Check if character in parameter is alphanumeric
  * @param  Char for the alphanumeric check
  */
static int32_t isHex(char Char);

/**
  * @brief  Converts hex string to a nibble ( 0x0X )
  * @param  Char hex string
  * @retval the nibble. Returns 0xF0 in case input was not an hex string (a..f; A..F or 0..9)
  */
static uint8_t Char2Nibble(char Char);

/**
  * @brief  Convert a string into a buffer of data
  * @param  str string to convert
  * @param  data output buffer
  * @param  Size of input string
  */
static int32_t stringToData(const char *str, uint8_t *data, uint32_t Size);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
ATEerror_t AT_return_ok(const char *param)
{
  return AT_OK;
}

ATEerror_t AT_return_error(const char *param)
{
  return AT_ERROR;
}

/* --------------- General commands --------------- */
ATEerror_t AT_version_get(const char *param)
{
  /* USER CODE BEGIN AT_version_get_1 */

  /* USER CODE END AT_version_get_1 */

  /* Get LoRa APP version*/
  AT_PRINTF("APPLICATION_VERSION: V%X.%X.%X\r\n",
            (uint8_t)(APP_VERSION_MAIN),
            (uint8_t)(APP_VERSION_SUB1),
            (uint8_t)(APP_VERSION_SUB2));

  /* Get MW SubGhz_Phy info */
  AT_PRINTF("MW_RADIO_VERSION:    V%X.%X.%X\r\n",
            (uint8_t)(SUBGHZ_PHY_VERSION_MAIN),
            (uint8_t)(SUBGHZ_PHY_VERSION_SUB1),
            (uint8_t)(SUBGHZ_PHY_VERSION_SUB2));

  return AT_OK;
  /* USER CODE BEGIN AT_version_get_2 */

  /* USER CODE END AT_version_get_2 */
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

ATEerror_t AT_reset(const char *param)
{
  /* USER CODE BEGIN AT_reset_1 */

  /* USER CODE END AT_reset_1 */
  NVIC_SystemReset();
  /* USER CODE BEGIN AT_reset_2 */

  /* USER CODE END AT_reset_2 */
}

/* --------------- Radio tests commands --------------- */
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

ATEerror_t AT_test_get_config(const char *param)
{
  /* USER CODE BEGIN AT_test_get_config_1 */

  /* USER CODE END AT_test_get_config_1 */
  testParameter_t testParam;
  uint32_t loraBW[7] = {7812, 15625, 31250, 62500, 125000, 250000, 500000};

  TST_get_config(&testParam);

  AT_PRINTF("1: Freq= %d Hz\r\n", testParam.freq);
  AT_PRINTF("2: Power= %d dBm\r\n", testParam.power);

  if ((testParam.modulation == TEST_FSK) || (testParam.modulation == TEST_MSK))
  {
    /*fsk*/
    AT_PRINTF("3: Bandwidth= %d Hz\r\n", testParam.bandwidth);
    AT_PRINTF("4: FSK datarate= %d bps\r\n", testParam.loraSf_datarate);
    AT_PRINTF("5: Coding Rate not applicable\r\n");
    AT_PRINTF("6: LNA State= %d  \r\n", testParam.lna);
    AT_PRINTF("7: PA Boost State= %d  \r\n", testParam.paBoost);
    if (testParam.modulation == TEST_FSK)
    {
      AT_PRINTF("8: modulation FSK\r\n");
    }
    else
    {
      AT_PRINTF("8: modulation MSK\r\n");
    }
    AT_PRINTF("9: Payload len= %d Bytes\r\n", testParam.payloadLen);
    if (testParam.modulation == TEST_FSK)
    {
      AT_PRINTF("10: FSK deviation= %d Hz\r\n", testParam.fskDev);
    }
    else
    {
      AT_PRINTF("10: FSK deviation forced to FSK datarate/4\r\n");
    }
    AT_PRINTF("11: LowDRopt not applicable\r\n");
    AT_PRINTF("12: FSK gaussian BT product= %d \r\n", testParam.BTproduct);
  }
  else if (testParam.modulation == TEST_LORA)
  {
    /*Lora*/
    AT_PRINTF("3: Bandwidth= %d (=%d Hz)\r\n", testParam.bandwidth, loraBW[testParam.bandwidth]);
    AT_PRINTF("4: SF= %d \r\n", testParam.loraSf_datarate);
    AT_PRINTF("5: CR= %d (=4/%d) \r\n", testParam.codingRate, testParam.codingRate + 4);
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
  return AT_OK;
  /* USER CODE BEGIN AT_test_get_config_2 */

  /* USER CODE END AT_test_get_config_2 */
}

ATEerror_t AT_test_set_config(const char *param)
{
  /* USER CODE BEGIN AT_test_set_config_1 */

  /* USER CODE END AT_test_set_config_1 */
  testParameter_t testParam = {0};
  uint32_t freq;
  int32_t power;
  uint32_t bandwidth;
  uint32_t loraSf_datarate;
  uint32_t codingRate;
  uint32_t lna;
  uint32_t paBoost;
  uint32_t modulation;
  uint32_t payloadLen;
  uint32_t fskDeviation;
  uint32_t lowDrOpt;
  uint32_t BTproduct;
  uint32_t crNum;

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
                        &fskDeviation,
                        &lowDrOpt,
                        &BTproduct))
  {
    /*extend to new format for extended*/
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  /*get current config*/
  TST_get_config(&testParam);

  /* 8: modulation check and set */
  /* first check because required for others */
  if (modulation == TEST_FSK)
  {
    testParam.modulation = TEST_FSK;
  }
  else if (modulation == TEST_LORA)
  {
    testParam.modulation = TEST_LORA;
  }
  else if (modulation == TEST_BPSK)
  {
    testParam.modulation = TEST_BPSK;
  }
  else if (modulation == TEST_MSK)
  {
    testParam.modulation = TEST_MSK;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  /* 1: frequency check and set */
  if (freq < 1000)
  {
    /*given in MHz*/
    testParam.freq = freq * 1000000;
  }
  else
  {
    testParam.freq = freq;
  }

  /* 2: power check and set */
  if ((power >= -9) && (power <= 22))
  {
    testParam.power = power;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  /* 3: bandwidth check and set */
  if ((testParam.modulation == TEST_FSK) && (bandwidth >= 4800) && (bandwidth <= 467000))
  {
    testParam.bandwidth = bandwidth;
  }
  else if ((testParam.modulation == TEST_MSK) && (bandwidth >= 4800) && (bandwidth <= 467000))
  {
    testParam.bandwidth = bandwidth;
  }
  else if ((testParam.modulation == TEST_LORA) && (bandwidth <= BW_500kHz))
  {
    testParam.bandwidth = bandwidth;
  }
  else if (testParam.modulation == TEST_BPSK)
  {
    /* Not used */
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  /* 4: datarate/spreading factor check and set */
  if ((testParam.modulation == TEST_FSK) && (loraSf_datarate >= 600) && (loraSf_datarate <= 300000))
  {
    testParam.loraSf_datarate = loraSf_datarate;
  }
  else if ((testParam.modulation == TEST_MSK) && (loraSf_datarate >= 100) && (loraSf_datarate <= 300000))
  {
    testParam.loraSf_datarate = loraSf_datarate;
  }
  else if ((testParam.modulation == TEST_LORA) && (loraSf_datarate >= 5) && (loraSf_datarate <= 12))
  {
    testParam.loraSf_datarate = loraSf_datarate;
  }
  else if ((testParam.modulation == TEST_BPSK) && (loraSf_datarate <= 1000))
  {
    testParam.loraSf_datarate = loraSf_datarate;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  /* 5: coding rate check and set */
  if ((testParam.modulation == TEST_FSK) || (testParam.modulation == TEST_MSK) || (testParam.modulation == TEST_BPSK))
  {
    /* Not used */
  }
  else if ((testParam.modulation == TEST_LORA) && ((codingRate >= 5) && (codingRate <= 8)))
  {
    testParam.codingRate = codingRate - 4;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  /* 6: lna state check and set */
  if (lna <= 1)
  {
    testParam.lna = lna;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  /* 7: pa boost check and set */
  if (paBoost <= 1)
  {
    /* Not used */
    testParam.paBoost = paBoost;
  }

  /* 9: payloadLen check and set */
  if ((payloadLen != 0) && (payloadLen < 256))
  {
    testParam.payloadLen = payloadLen;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  /* 10: fsk Deviation check and set */
  if ((testParam.modulation == TEST_LORA) || (testParam.modulation == TEST_BPSK) || (testParam.modulation == TEST_MSK))
  {
    /* Not used */
  }
  else if ((testParam.modulation == TEST_FSK) && ((fskDeviation >= 600) && (fskDeviation <= 200000)))
  {
    /*given in MHz*/
    testParam.fskDev = fskDeviation;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  /* 11: low datarate optimization check and set */
  if ((testParam.modulation == TEST_FSK) || (testParam.modulation == TEST_BPSK) || (testParam.modulation == TEST_MSK))
  {
    /* Not used */
  }
  else if ((testParam.modulation == TEST_LORA) && (lowDrOpt <= 2))
  {
    testParam.lowDrOpt = lowDrOpt;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  /* 12: FSK gaussian BT product check and set */
  if ((testParam.modulation == TEST_LORA) || (testParam.modulation == TEST_BPSK))
  {
    /* Not used */
  }
  else if (((testParam.modulation == TEST_FSK) || (testParam.modulation == TEST_MSK)) && (BTproduct <= 4))
  {
    /*given in MHz*/
    testParam.BTproduct = BTproduct;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  TST_set_config(&testParam);

  return AT_OK;
  /* USER CODE BEGIN AT_test_set_config_2 */

  /* USER CODE END AT_test_set_config_2 */
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
    return AT_ERROR;
  }
  /* USER CODE BEGIN AT_test_tx_2 */

  /* USER CODE END AT_test_tx_2 */
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
    return AT_ERROR;
  }
  /* USER CODE BEGIN AT_test_rx_2 */

  /* USER CODE END AT_test_rx_2 */
}

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

  for (int32_t i = 0; i < nb_tx; i++)
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

/* --------------- Radio access commands --------------- */
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

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
static void print_u(uint32_t value)
{
  /* USER CODE BEGIN print_u_1 */

  /* USER CODE END print_u_1 */
  AT_PRINTF("%u\r\n", value);
  /* USER CODE BEGIN print_u_2 */

  /* USER CODE END print_u_2 */
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

    /*check if input is hex */
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
