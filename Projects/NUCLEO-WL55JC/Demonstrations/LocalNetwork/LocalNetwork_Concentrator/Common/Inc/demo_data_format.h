/**
  ******************************************************************************
  * @file    demo_data_format.h
  * @author  MCD Application Team
  * @brief   Header with data format definitions for STM32WL Concentrator Demo.
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

#ifndef __DEMO_DATA_FORMAT_H__
#define __DEMO_DATA_FORMAT_H__

#include <stdint.h>
#include "stm32wlxx_hal.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief Structure with measurement data.
  * Sent in DEMO_packet_sensor_t.
  * This data structure is for version 1.0.
  */
typedef  struct
{
  int16_t temperature;  /**<Temperature [0.01 `C]*/

  uint8_t voltage;      /**<Voltage on the MCU [0.05 V]*/
} __packed DEMO_data_1_0_t;

/**Error values for DEMO_data_1_0_t*/
#define DEMO_DATA_1_0_ERROR_TEMPERATURE       INT16_MIN /*-3k `C*/
#define DEMO_DATA_1_0_ERROR_VOLTAGE           UINT8_MAX /*12.75 V*/


/**
  * @brief Structure with measurement data.
  * Sent in DEMO_packet_sensor_t.
  * This data structure is for version 1.1.
  */
typedef  struct
{
  int16_t temperature;  /**<Temperature [0.01 `C]*/
  uint16_t pressure;    /**<Pressure [0.1 hPa]*/

  int32_t yaw :10;      /**<Yaw angle [deg]*/
  int32_t pitch :10;    /**<Pitch angle [deg]*/
  int32_t roll :10;     /**<Roll angle [deg]*/
  int32_t :2;

  uint8_t humidity;     /**<Humidity [0.5 %]*/

  uint8_t voltage;      /**<Voltage on the MCU [0.05 V]*/

  uint16_t distance:12; /**<Distance from the ranging sensor [mm]*/

  /**
    * Activity detected by the accelerometer and MotionAR
    * 0 - No Activity
    * 1 - Stationary
    * 2 - Walking
    * 3 - Fast Walking
    * 4 - Jogging
    * 5 - Biking
    * 6 - Driving
    */
  uint16_t activity:4;

  uint16_t step_count;  /**<Number of steps (acceleration bumps) [1]*/
  uint16_t swipe_count; /**<Number of swipes over the ranging sensor [1]*/

  /**Average acceleration, gravity excluded, over past 16 s [mg]*/
  uint16_t acceleration_avg;
  /**Maximal acceleration, gravity excluded, during past 16 s [mg]*/
  uint16_t acceleration_max;

} __packed DEMO_data_1_1_t;

/**Error values for DEMO_data_1_0_t*/
#define DEMO_DATA_1_1_ERROR_TEMPERATURE       DEMO_DATA_1_0_ERROR_TEMPERATURE
#define DEMO_DATA_1_1_ERROR_PRESSURE          UINT16_MAX /*6k hpA*/
#define DEMO_DATA_1_1_ERROR_YAW               (-512) /*-512 deg = 0x200*/
#define DEMO_DATA_1_1_ERROR_PITCH             (-512)
#define DEMO_DATA_1_1_ERROR_ROLL              (-512)
#define DEMO_DATA_1_1_ERROR_HUMIDITY          UINT8_MAX /*127.5 %*/
#define DEMO_DATA_1_1_ERROR_ACTIVITY          UINT8_MAX /*Not in enum*/
#define DEMO_DATA_1_1_ERROR_DISTANCE          UINT16_MAX /*65 m*/
#define DEMO_DATA_1_1_ERROR_ACCELERATION      UINT16_MAX /*65 g*/
#define DEMO_DATA_1_1_ERROR_VOLTAGE           DEMO_DATA_1_0_ERROR_VOLTAGE

#ifdef __cplusplus
}
#endif

#endif /* __DEMO_DATA_FORMAT_H__ */
