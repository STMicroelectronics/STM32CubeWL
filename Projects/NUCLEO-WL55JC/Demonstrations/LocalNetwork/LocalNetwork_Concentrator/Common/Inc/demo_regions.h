/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_regions.h
  * @author  MCD Application Team
  * @brief   Region specific definitions for STM32WL Sensor Concentrator Demo.
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
#ifndef __DEMO_REGIONS_H__
#define __DEMO_REGIONS_H__

#include "platform.h"

/* BEACON modulation definition for FCC in DTS mode*/
#define BEACON_FCC_BW_500               9
#define BEACON_FCC_SF_12               12

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief Structure defining one of sensor channels.
  */
typedef struct
{
  int32_t power;                        /**<Power used on this channel [dBm]*/
  uint32_t max_bw;                      /**<Maximal bandwidth used [Hz]*/
  uint32_t freq;                        /**<Channel frequency [Hz]*/
  int duty_cycle;                       /**<Channel duty cycle multiple [1 = no limit, 100 = 1% duty cycle]*/
} DEMO_Channel_t;

/**
  * @brief Structure defining subregion.
  */
typedef struct
{
  const char *name;                     /**<String with subregion name*/
  int32_t beacon_power;                 /**<Beacon power [dBm]*/
  uint16_t max_t;                       /**<Maximal transmission time [ms]*/
  uint8_t hopping:1;                    /**<Choose channels pseudorandomly if set*/
  uint8_t lbt:1;                        /**<Use listen before talk*/
  uint8_t :6;
  uint8_t lbt_time;                     /**<Duration of LBT [ms]*/
  int16_t lbt_rssi;                     /**<RSSI threshold for LBT [dBm]*/

  int channels_n;                       /**<Number of valid channels*/
  const DEMO_Channel_t *channels;       /**<List of available channels*/
} DEMO_Subregion_t;

/**
  * @brief Structure defining region.
  */
typedef struct
{
  const char *name;                     /**<String with region name*/
  uint32_t beacon_freq;                 /**<Frequency of the beacon [Hz]*/
  uint8_t beacon_bw;                    /**<Bandwidth used for beacon [same as in DEMO_coding_lora_t::bw]*/
  uint8_t beacon_sf;                    /**<Spreading used for beacon [same as in DEMO_coding_lora_t::sf]*/
  uint16_t beacon_length;               /**<TimeOnAir for beacon [ms]*/

  int subregions_n;                     /**<Number of valid subregions*/
  const DEMO_Subregion_t *subregions;   /**<Subregions for this region*/
} DEMO_Region_t;


/**
  * @brief Definition of all Regions.
  */
extern const DEMO_Region_t DEMO_Regions[];
extern const int DEMO_Regions_n;

#ifdef __cplusplus
}
#endif

#endif /* __DEMO_REGIONS_H__ */
