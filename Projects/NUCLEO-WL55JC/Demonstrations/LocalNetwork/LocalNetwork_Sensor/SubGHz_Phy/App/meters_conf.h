/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    meters_conf.h
  * @author  MCD Application Team
  * @brief   Configuration for metering module.
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
#ifndef __METERS_CONF_H__
#define __METERS_CONF_H__

/**
  * @brief Maximal period of measurement for slow sensors [ms].
  * This is the maximum period between two measurements if sensor is not connected.
  * If sensor is connected, the measurement is done just before transmit.
  * Slow sensors are: Humidity, Temperature, Pressure, Battery Voltage.
  */
#define METERS_SLOW_MAX_DELAY   (30*1000)

/**
  * @brief Enable sensors this long before reading out the measurement data [ms].
  */
#define METERS_SLOW_ENABLE_DELAY      150

/**
  * @brief Read out measurement data this long before NextGet from function void Meters_GetData() [ms].
  */
#define METERS_SLOW_PREMEASURE         10

/**
  * @brief Period of measurement of the ranging sensor [ms].
  * Measurement itself takes 33 ms.
  * Higher value will cripple the swipe detection, lower value increases consumption.
  */
#define METERS_RANGE_PERIOD           250

/**
  * @brief Time that the range sensor is blocked after out of range [ms].
  * When the sensor is out of range, it can sometimes throw values on the range limit which confuse
  * the swipe detection algorithm. This constant will block the sensor and swipe detection for a limited time.
  * Set to 0 to not block the swipe detection algorithm when sensor is out of range.
  */
#define METERS_RANGE_BLOCK_PERIOD    1000

/**
  * @brief Period of measurement for fast sensors [ms].
  * Fast sensors, magnetometer and accelerometer, are measured continuously.
  * This value affects multiple MotionXX libraries and some of them can misbehave for other frequencies than 50 Hz.
  */
#define METERS_FAST_PERIOD             20


#endif /* __METERS_CONF_H__ */
