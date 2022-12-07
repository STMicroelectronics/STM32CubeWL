/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_sensor.h
  * @author  MCD Application Team
  * @brief   Header for concentrator behavior module for STM32WL Concentrator Demo.
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
#ifndef __DEMO_SENSOR_H__
#define __DEMO_SENSOR_H__

#include <stdint.h>
#include <stdbool.h>
#include "demo_packet_format.h"

#ifdef __cplusplus
extern "C" {
#endif

/*One side of tolerance added to beacon receiving window for each missed beacon [ms]*/
#define SENS_TIMING_TOLERANCE       50

/*Time of receiving before expected packet [ms]*/
#define SENS_TIMING_PRERECEIVE      10

/*Miss this many beacons and be lost*/
#define SENS_MAX_MISSED_TO_LOST     (DEMO_SENSOR_MISS_LIMIT - 2) /*Less than Concentrator value to try to reconnect*/

/*When lost, switch between RX and sleep [ms]*/
#define SENS_LOST_DUTY_RX           1000*DEMO_SLOT_NUMBER*1
#define SENS_LOST_DUTY_SLEEP        1000*DEMO_SLOT_NUMBER*32

/*Source of eui address, 0 - UDN, 1 - 3*UID xored, 2 - random,*/
#define SENS_EUI_SOURCE             0

/*When true, successful beacon receive is followed by another scanning*/
#define SENS_SCAN_TESTING           0

/*Timeout of CAD while scanning [ms]*/
#define SENS_CAD_TIMEOUT            (50 + Radio.GetWakeupTime())

/*CAD parameters for scanning*/
#define SENS_SCAN_CadDetPeak        50
#define SENS_SCAN_CadDetMin         20

/**
  * @brief Initialize Sensor and start scanning for Beacon.
  * @param GetDataCallback called before transmission, quickly add data to transmit, do block in the callback
  * @param ActivityCallback called on every activity to blink LED or something
  */
int SENS_Init(void (*GetDataCallback)(void), void (*ActivityCallback)(void));

/**
  * @brief Store data to send in next packet.
  * @param Data structure
  */
void SENS_WriteSensorData(const DEMO_data_1_0_t *Data);

typedef enum
{
  SENS_STATE_Dead = 0,  /**<Error state*/
  SENS_STATE_Scan,      /**<Scanning for Beacon of all regions*/
  SENS_STATE_Sync,      /**<Synchronized to Beacons, but not connected*/
  SENS_STATE_Lost,      /**<Not synchronized, lost all communication*/
  SENS_STATE_Connect,   /**<Connected to the Concentrator and synchronized to Beacons*/
} SENS_State_t;

/**
  * @brief Get state of the sensor connection.
  * @return one of SENS_State_t
  */
SENS_State_t SENS_GetState(void);


#ifdef __cplusplus
}
#endif

#endif /* __DEMO_SENSOR_H__ */
