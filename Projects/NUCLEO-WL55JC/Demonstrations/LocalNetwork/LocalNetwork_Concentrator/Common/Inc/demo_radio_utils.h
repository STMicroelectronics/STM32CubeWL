/**
  ******************************************************************************
  * @file    demo_radio_utils.h
  * @author  MCD Application Team
  * @brief   Module for various utilities common to Concentrator and Sensor of STM32WL demo.
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

#ifndef __DEMO_RADIO_UTILS_H__
#define __DEMO_RADIO_UTILS_H__

#include "radio_conf.h"
#include "radio_driver.h"
#include "radio.h"
#include "demo_utils.h"

#ifdef __cplusplus
extern "C" {
#endif


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
bool DEMO_WaitForFreeChannel(RadioModems_t Modem, int16_t RssiThreshold, uint32_t Scan, uint32_t MaxWait);

/**
  * @brief Convert coding bt parameter into proper symbol shaping.
  * @param coding FSK coding
  * @return shaping
  */
RadioModShapings_t DEMO_GetShaping(const DEMO_coding_fsk_t *coding);

/**
  * @brief Convert coding rise parameter into proper power ramp.
  * @param coding FSK coding
  * @return ramp up
  */
RadioRampTimes_t DEMO_GetRampUp(const DEMO_coding_fsk_t *coding);

/**
  * @brief Convert coding sf parameter into proper spreading factor.
  * @param coding LoRa coding
  * @return spreading factor
  */
RadioLoRaSpreadingFactors_t DEMO_GetSpreadingFactor(const DEMO_coding_lora_t *coding);

/**
  * @brief Convert coding bw parameter into proper bandwidth.
  * @param coding LoRa coding
  * @return spreading factor
  */
RadioLoRaBandwidths_t DEMO_GetBandwidth(const DEMO_coding_lora_t *coding);

/**
  * @brief Convert coding cr parameter into proper coding rate.
  * @param coding LoRa coding
  * @return spreading factor
  */
RadioLoRaCodingRates_t DEMO_GetCoderate(const DEMO_coding_lora_t *coding);

/**
  * @brief Convert FSK bandwidth to a constant of RX filter settings.
  * @param bandwidth bandwidth in Hz
  * @return bandwidth setting
  */
RadioRxBandwidth_t DEMO_ConvertFSKBandwidth(uint32_t bandwidth);

#ifdef __cplusplus
}
#endif

#endif /* __DEMO_RADIO_UTILS_H__ */
