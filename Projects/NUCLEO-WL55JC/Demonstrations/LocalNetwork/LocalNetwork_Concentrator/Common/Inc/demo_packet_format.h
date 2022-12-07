/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_packet_format.h
  * @author  MCD Application Team
  * @brief   Header with packet format definitions for STM32WL Concentrator Demo.
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
#ifndef __DEMO_PACKET_FORMAT_H__
#define __DEMO_PACKET_FORMAT_H__

#include "demo_data_format.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DEMO_SLOT_NUMBER           16   /*Number of slots used*/


/**
  * @brief Beacon data structure.
  * Always 4 B.
  * Beacon packet doesn't have length or CRC.
  */
typedef __PACKED_STRUCT
{
  uint8_t version_major :3;   /**<Major version of concentrator*/
  uint8_t subregion :5; /**<Subregion number*/
  uint8_t seed;         /**<Seed for hopping*/
  uint16_t offset :10;  /**<Time offset from the exact start of slot [ms]*/
  uint16_t pattern :2;  /**<Constant pattern to lower risk of catching different LoRa system*/
  uint16_t sum :4;      /**<Checksum of previous nibbles (negative value, sum of all nibbles = 0)*/
} DEMO_packet_beacon_t;

#define DEMO_BEACON_PATTERN        0x2     /*Inserted into DEMO_packet_beacon_t::pattern*/

typedef __PACKED_STRUCT
{
  uint8_t mark :4;     /**<Is zero for the connect header*/
  uint8_t slot :4;     /**<Slot which is connected*/
  /**Following are 4 B of eui that is connected*/
} DEMO_coding_connect_t;   /**<Header for sensor connecting and slot control*/

#define DEMO_HDR_CONNECT_MARK           0x0 /*This value marks connect header*/

/**
  * @brief Slot coding and connect header.
  */
typedef struct
{
  uint8_t slot :4;     /**<Slot number to change, [2 .. DEMO_SLOT_NUMBER]*/
  uint8_t mode :1;     /**<true for LoRa, false for FSK*/
  uint8_t period :3;   /**<Number of beacons skipped before another transmission*/
  uint8_t data_lim :5; /**<Limit length of sensor's packet [byte]*/
  uint8_t chan_nr :3;  /**<Set different channel due to different channel constrains (not used when hopping)*/
  /**Following are DEMO_coding_lora_t or DEMO_coding_fsk_t*/
} DEMO_coding_hdr_t;    /**<Header for a coding change*/

#define DEMO_HDR_PARAM_PERIOD_MAX    7  /*Maximum period*/
#define DEMO_HDR_CHAN_NR_MAX         7  /*Static channel maximum*/

typedef __PACKED_UNION
{
  DEMO_coding_hdr_t hdr;                /**<2 B header to change coding*/
  DEMO_coding_connect_t connect;        /**<1 B to connect sensor to proper slot*/
} DEMO_coding_combined_t;               /**<Combined header put into Sync*/

/**
  * @brief Slot coding specific for LoRa.
  */
typedef __PACKED_STRUCT
{
  uint8_t de :1;        /**<LowDataRateOptimize, used when symbol length > 16 ms*/

  /**
    * Error coding rate
    *   001 - 4/5
    *   010 - 4/6
    *   011 - 4/7
    *   100 - 4/8
    */
  uint8_t cr :3;

  /**
    * Signal bandwidth
    *   0000 - 7.8 kHz
    *   0001 - 10.4 kHz
    *   0010 - 15.6 kHz
    *   0011 - 20.8kHz
    *   0100 - 31.25 kHz
    *   0101 - 41.7 kHz
    *   0110 - 62.5 kHz
    *   0111 - 125 kHz
    *   1000 - 250 kHz
    *   1001 - 500 kHz
    */
  uint8_t bw :4;

  /**
    * SF rate (expressed as a base-2 logarithm)
    *   6  - 64 chips / symbol
    *   7  - 128 chips / symbol
    *   8  - 256 chips / symbol
    *   9  - 512 chips / symbol
    *   10 - 1024 chips / symbol
    *   11 - 2048 chips / symbol
    *   12 - 4096 chips / symbol
    */
  uint8_t sf :4;

  uint8_t :4;
} DEMO_coding_lora_t;

/**
  * @brief LoRa parameters limits.
  */
#define DEMO_LORA_PARAM_CR_MIN      1
#define DEMO_LORA_PARAM_SF_MIN      6
#define DEMO_LORA_PARAM_BW_MIN      0

#define DEMO_LORA_PARAM_CR_MAX      4
#define DEMO_LORA_PARAM_SF_MAX      12
#define DEMO_LORA_PARAM_BW_MAX      9

/**
  * @brief Change of FSK coding.
  */
typedef __PACKED_STRUCT
{
  uint32_t br :24;      /**<Data rate [600..300000 bits/s]*/

  /**
    * Power Ramp Time
    * 0 -   10 us
    * 1 -   20 us
    * 2 -   40 us
    * 3 -   80 us
    * 4 -  200 us
    * 5 -  800 us
    * 6 - 1700 us
    * 7 - 3400 us
    */
  uint32_t rise :3;
  uint32_t :5;
  uint32_t fdev :24;    /**<Frequency Deviation [Hz]*/

  /**
    * Gaussian Shaping BT
    * Lower number means wider spreading.
    * 0 - Off
    * 1 - 0.3
    * 2 - 0.5
    * 3 - 0.7
    * 4 -   1
    **/
  uint32_t bt :3;
  uint32_t :5;
} DEMO_coding_fsk_t;

/**
  * @brief FSK parameters limits.
  */
#define DEMO_FSK_PARAM_RISE_MIN      0
#define DEMO_FSK_PARAM_BR_MIN        600
#define DEMO_FSK_PARAM_FDEV_MIN      0
#define DEMO_FSK_PARAM_BT_MIN        0

#define DEMO_FSK_PARAM_RISE_MAX      7
#define DEMO_FSK_PARAM_BR_MAX        300000
#define DEMO_FSK_PARAM_FDEV_MAX      300000
#define DEMO_FSK_PARAM_BT_MAX        4

#define DEMO_SYNC_CODINGS_LEN        23 /*This many bytes are for coding changes*/

/**
  * @brief Sync data structure. Maximally 27 B.
  */
typedef __PACKED_STRUCT
{
  /**
    * @brief Mask of occupied frequencies for 16 slots.
    * Each slot is true if a node is connected, false if the slot is free to take.
    * False also means that default coding is used.
    * Bits 0 and 1 are not used by sensors, but by concentrator.
    */
  uint16_t occupied;
  uint8_t version_major:3;    /**<Major version of concentrator*/
  uint8_t subregion:5;  /**<Subregion number*/
  uint8_t seed;         /**<Seed for hopping*/

  /**
    * @brief List of slot coding changes or connect accept structures.
    * Contains list of DEMO_coding_combined_t
    * each followed by DEMO_coding_lora_t, DEMO_coding_gfsk_t or 4 B eui to connect.
    */
  uint8_t codings[DEMO_SYNC_CODINGS_LEN];
} DEMO_packet_sync_t;


#define DEMO_SENSOR_MAX_PAYLOAD    26   /*Maximal payload for sensor*/

/**
  * @brief Sensor packet data structure.
  */
typedef __PACKED_STRUCT
{
  uint32_t eui;         /**<Lower part of EUI read from HW*/
  uint8_t packet_cnt;   /**<Counts packets*/
  uint8_t version_major :3;     /**<Major version of a sensor*/
  uint8_t version_minor :5;     /**<Minor version of a sensor*/

  /*Following are DEMO_data_t. Together up to DEMO_SENSOR_MAX_PAYLOAD bytes*/
} DEMO_packet_sensor_header_t;


#ifdef __cplusplus
}
#endif


#endif /* __DEMO_PACKET_FORMAT_H__ */
