/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_report.h
  * @author  MCD Application Team
  * @brief   Report various things to printf console.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEMO_REPORT_H__
#define __DEMO_REPORT_H__

#include "demo_packet_format.h"

/**
  * @brief Report lost sensor to console.
  * @param Eui sensor's address
  */
void CONC_Report_LOST(uint32_t Eui);

/**
  * @brief Report successful modulation change to console.
  * @param Eui sensor's address
  */
void CONC_Report_MOD_OK(uint32_t Eui);

/**
  * @brief Report received packet to console.
  * @param Header sensor packet header
  * @param Rssi RSSI value computed while receiving the frame [dBm]
  * @param Snr raw SNR value given by the radio hardware [dB]
  * @param Data sensor's measured sensor data
  * @param DataLen length of data [byte]
  */
void CONC_Report_RCV(const DEMO_packet_sensor_header_t *Header,
                     int16_t Rssi, int8_t Snr,
                     const uint8_t *Data, uint32_t DataLen);


#endif /* __DEMO_REPORT_H__ */
