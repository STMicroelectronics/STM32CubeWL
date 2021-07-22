/**
  ******************************************************************************
  * @file    demo_report.h
  * @author  MCD Application Team
  * @brief   Report various things to printf console.
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
