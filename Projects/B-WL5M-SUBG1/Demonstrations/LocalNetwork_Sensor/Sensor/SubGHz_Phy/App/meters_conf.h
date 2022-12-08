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
  * @brief Period of measurement for fast sensors [ms].
  * Fast sensors, magnetometer and accelerometer, are measured continuously.
  * This value affects multiple MotionXX libraries and some of them can misbehave for other frequencies than 50 Hz.
  */
#define METERS_FAST_PERIOD             20


#endif /* __METERS_CONF_H__ */
