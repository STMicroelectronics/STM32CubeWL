/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_radio_utils.c
  * @author  MCD Application Team
  * @brief   Module for various utilities common to Concentrator and Sensor of STM32WL demo.
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
#include "demo_radio_utils.h"

#include "stm32_timer.h"
#include "utilities_conf.h"


/**
  * @brief Wait until the channel is free for Scan long.
  *
  * Functions Radio.SetModem(); Radio.SetChannel(f); need to be called before this.
  *
  * @param Modem LoRa or FSK
  * @param RssiThreshold threshold of free channel [dBm]
  * @param Scan time for which the channel needs to be free [ms]
  * @param MaxWait Wait maximally this long before giving up [ms]
  * @return true if the channel is free, false if the channel is busy
  */
bool DEMO_WaitForFreeChannel(RadioModems_t Modem, int16_t RssiThreshold, uint32_t Scan, uint32_t MaxWait)
{
  UTIL_TIMER_Time_t start = UTIL_TIMER_GetCurrentTime();    /*Time when this business started*/
  UTIL_TIMER_Time_t now = start;      /*Current time, to save on reading from RTC calendar*/
  UTIL_TIMER_Time_t free_from;        /*Last time when the channel was busy*/

  Radio.Rx(0);
  RADIO_DELAY_MS(1);

  free_from = UTIL_TIMER_GetCurrentTime();    /*Do not count radio init to the free time*/

  while ((now - start) < MaxWait)       /*Check maximal time limit*/
  {
    UTILS_ENTER_CRITICAL_SECTION();
    now = UTIL_TIMER_GetCurrentTime();
    if (Radio.Rssi(Modem) > RssiThreshold)     /*Channel is currently busy*/
    {
      UTILS_EXIT_CRITICAL_SECTION();
      free_from = now;  /*Reset time when the channel became free*/
    }
    else        /*Channel is currently free*/
    {
      UTILS_EXIT_CRITICAL_SECTION();
      if ((now - free_from) > Scan) /*Channel is free for needed time*/
      {
        return true;
      }
    }
  }

  return false;     /*Channel is busy and time is up*/
}

/**
  * @brief Convert coding BT parameter into proper symbol shaping.
  * @param coding FSH coding
  * @return shaping
  */
RadioModShapings_t DEMO_GetShaping(const DEMO_coding_fsk_t *coding)
{
  switch (coding->bt)
  {
    default:    /*Off*/
      return MOD_SHAPING_OFF;
    case 1:     /*0.3*/
      return MOD_SHAPING_G_BT_03;
    case 2:     /*0.5*/
      return MOD_SHAPING_G_BT_05;
    case 3:     /*0.7*/
      return MOD_SHAPING_G_BT_07;
    case 4:     /*1*/
      return MOD_SHAPING_G_BT_1;

  }
}

/**
  * @brief Convert coding rise parameter into proper power ramp.
  * @param coding FSK coding
  * @return ramp up
  */
RadioRampTimes_t DEMO_GetRampUp(const DEMO_coding_fsk_t *coding)
{
  return (RadioRampTimes_t)coding->rise;        /*Values fit*/
}

/**
  * @brief Convert coding sf parameter into proper spreading factor.
  * @param coding LoRa coding
  * @return spreading factor
  */
RadioLoRaSpreadingFactors_t DEMO_GetSpreadingFactor(const DEMO_coding_lora_t *coding)
{
  return (RadioLoRaSpreadingFactors_t)coding->sf;       /*Values fit*/
}

/**
  * @brief Convert coding bw parameter into proper bandwidth.
  * @param coding LoRa coding
  * @return spreading factor
  */
RadioLoRaBandwidths_t DEMO_GetBandwidth(const DEMO_coding_lora_t *coding)
{
  switch (coding->bw)
  {
    default:     /*7.8 kHz*/
      return LORA_BW_007;
    case 1:     /*10.4 kHz*/
      return LORA_BW_010;
    case 2:     /*15.6 kHz*/
      return LORA_BW_015;
    case 3:     /*20.8 kHz*/
      return LORA_BW_020;
    case 4:     /*31.25 kHz*/
      return LORA_BW_031;
    case 5:     /*41.7 kHz*/
      return LORA_BW_041;
    case 6:     /*62.5 kHz*/
      return LORA_BW_062;
    case 7:     /*125 kHz*/
      return LORA_BW_125;
    case 8:     /*250 kHz*/
      return LORA_BW_250;
    case 9:     /*500 kHz*/
      return LORA_BW_500;
  }
}

/**
  * @brief Convert coding cr parameter into proper coding rate.
  * @param coding LoRa coding
  * @return spreading factor
  */
RadioLoRaCodingRates_t DEMO_GetCoderate(const DEMO_coding_lora_t *coding)
{
  return (RadioLoRaCodingRates_t)coding->cr;    /*Values fit*/
}

/**
  * @brief Convert FSK bandwidth to a constant of RX filter settings.
  * @param bandwidth bandwidth in Hz
  * @return bandwidth setting
  */
RadioRxBandwidth_t DEMO_ConvertFSKBandwidth(uint32_t bandwidth)
{
  if (bandwidth < 4800)
  {
    return RX_BW_4800  ;
  }
  else if (bandwidth < 5800)
  {
    return RX_BW_5800  ;
  }
  else if (bandwidth < 7300)
  {
    return RX_BW_7300  ;
  }
  else if (bandwidth < 9700)
  {
    return RX_BW_9700  ;
  }
  else if (bandwidth < 11700)
  {
    return RX_BW_11700 ;
  }
  else if (bandwidth < 14600)
  {
    return RX_BW_14600 ;
  }
  else if (bandwidth < 19500)
  {
    return RX_BW_19500 ;
  }
  else if (bandwidth < 23400)
  {
    return RX_BW_23400 ;
  }
  else if (bandwidth < 29300)
  {
    return RX_BW_29300 ;
  }
  else if (bandwidth < 39000)
  {
    return RX_BW_39000 ;
  }
  else if (bandwidth < 46900)
  {
    return RX_BW_46900 ;
  }
  else if (bandwidth < 58600)
  {
    return RX_BW_58600 ;
  }
  else if (bandwidth < 78200)
  {
    return RX_BW_78200 ;
  }
  else if (bandwidth < 93800)
  {
    return RX_BW_93800 ;
  }
  else if (bandwidth < 117300)
  {
    return RX_BW_117300;
  }
  else if (bandwidth < 156200)
  {
    return RX_BW_156200;
  }
  else if (bandwidth < 187200)
  {
    return RX_BW_187200;
  }
  else if (bandwidth < 234300)
  {
    return RX_BW_234300;
  }
  else if (bandwidth < 312000)
  {
    return RX_BW_312000;
  }
  else if (bandwidth < 373600)
  {
    return RX_BW_373600;
  }
  else if (bandwidth < 467000)
  {
    return RX_BW_467000;
  }

  return RX_BW_4800;     /*Invalid bandwidth*/
}
