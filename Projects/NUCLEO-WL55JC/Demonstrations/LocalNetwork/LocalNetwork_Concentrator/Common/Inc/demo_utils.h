/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_utils.h
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEMO_UTILS_H__
#define __DEMO_UTILS_H__

#include "demo_packet_format.h"
#include "demo_regions.h"
#include "demo_prnd.h"

#ifdef __cplusplus
extern "C" {
#endif


#define DEMO_MAX_END_OF_PACKET_TX     940  /*All packets should end at least this time of a slot*/
#define DEMO_MAX_END_OF_PACKET_RX     970  /*All packets should end at least this time of a slot*/

/*Set to 1 to use LORA as a default modulation, 0 - FSK*/
#define DEMO_DEFAULT_LORA          1

/*Number of periods sensor can miss without being lost*/
#define DEMO_SENSOR_MISS_LIMIT     5

/*Default coding and modulation parameters*/
#define DEMO_DEFAULT_CR            1
#define DEMO_DEFAULT_DE            0
#define DEMO_DEFAULT_BR            50000
#define DEMO_DEFAULT_BT            2
#define DEMO_DEFAULT_FDEV          50000
#define DEMO_DEFAULT_RISE          2

/*Syncwords*/
/*Syncword for LoRa for Concentrator is set automatically to LORA_MAC_PUBLIC_SYNCWORD*/
#define DEMO_SYNCWORD_LORA_SENS  0x1424
#define DEMO_SYNCWORD_FSK_SENS   {0x60, 0x6D, 0xC9, 0xE2, 0x00, 0x00, 0x00, 0x00}

/**
  * @brief Convert bw used in regional settings to input for Radio.SetTxConfig()
  * This allows only higher bandwidth.
  * For proper bandwidth use DEMO_GetBandwidth().
  */
#define DEMO_BW_TO_SETTXCONFIG(bw)      (((((uint32_t)(bw)) - 7) < 3) ? ((bw) - 7) : 0) /*0 = 125 kHz ... 2 = 500 kHz*/

/*Generate next pseudorandom value*/
#define DEMO_PSEUDORANDOM_NEXT(last)    (DEMO_PRND_Next(((last) == 0 ? 0x32b87 : (last))))   /*Prevent 0*/
/*Get 8 bits of seed, rnd is a source of true random*/
#define DEMO_PSEUDORANDOM_FIRST(rnd)    ((rnd) & 0xff)

/**
  * Pack of all codings.
  */
typedef struct
{
  DEMO_coding_hdr_t hdr;    /**<Modulation header, indicates whether lora or fsk is used*/
  DEMO_coding_lora_t lora;  /**<LoRa modulation parameters*/
  DEMO_coding_fsk_t fsk;    /**<FSK modulation parameters*/
} DEMO_codings_t;

/**
  * @brief Calculate sensor's packet length.
  * @param Coding packet coding
  * @param Payload number of bytes in payload
  * @return packet length [ms]
  */
uint32_t DEMO_TimeOnAirLora(const DEMO_coding_lora_t *Coding, uint32_t Payload);

/**
  * @brief Calculate sensor's packet length.
  * @param Coding packet coding
  * @param Payload number of bytes in payload
  * @return packet length [ms]
  */
uint32_t DEMO_TimeOnAirFsk(const DEMO_coding_fsk_t *Coding, uint32_t payload);

/**
  * @brief Get the longest payload that can be sent in the given subregion.
  * @param Coding packet coding
  * @param Subregion local subregion
  * @param packet_time if not NULL, length of the packet will be written there [ms]
  * @return number of bytes which can be sent
  */
uint32_t DEMO_MaxPayloadLora(const DEMO_coding_lora_t *Coding,
                             const DEMO_Subregion_t *Subregion, uint32_t *packet_time);

/**
  * @brief Get the longest payload that can be sent in the given subregion.
  * @param Coding packet coding
  * @param Subregion local subregion
  * @param packet_time if not NULL, length of the packet will be written there [ms]
  * @return number of bytes which can be sent
  */
uint32_t DEMO_MaxPayloadFsk(const DEMO_coding_fsk_t *coding,
                            const DEMO_Subregion_t *Subregion, uint32_t *packet_time);

/**
  * @brief Set coding to default.
  * @param Codings structure to clear
  * @param SlotNr clear structure for this slot
  * @param Region default coding is the same as beacon
  */
void DEMO_DefaultCoding(DEMO_codings_t *Codings, uint32_t SlotNr, const DEMO_Region_t *Region);

/**
  * @brief Check if the sensor coding is the default coding.
  * @param Sensor structure for Sensor
  * @return true if the coding is default
  */
bool DEMO_IsCodingDefault(const DEMO_codings_t *Codings, const DEMO_Region_t *Region);

/**
  * @brief Get modulation bandwidth.
  * @param Coding modulation
  * @return bandwidth [Hz]
  */
uint32_t DEMO_BandwidthLora(const DEMO_coding_lora_t *Coding);

/**
  * @brief Get modulation bandwidth.
  * @param Coding modulation
  * @return bandwidth [Hz]
  */
uint32_t DEMO_BandwidthFsk(const DEMO_coding_fsk_t *Coding);

/**
  * @brief Validate and copy coding header.
  * @param Hdr structure to validate
  * @param Subregion pointer to subregion used to validate
  */
void DEMO_ValidateCodingHdr(DEMO_coding_hdr_t *Hdr, const DEMO_Subregion_t *Subregion);

/**
  * @brief Validate and copy coding parameters.
  * @param Coding structure to validate
  */
void DEMO_ValidateCodingLora(DEMO_coding_lora_t *Coding);

/**
  * @brief Validate and copy coding parameters.
  * @param Coding structure to validate
  */
void DEMO_ValidateCodingFsk(DEMO_coding_fsk_t *Coding);

/**
  * @brief Get number of free slots.
  * @param Occupied mask of occupied slots
  * @return number of free slots
  */
uint32_t DEMO_CountFreeSlots(uint16_t Occupied);

/**
  * @brief Pick one slot from the slots marked as free in Occupied mask.
  * @param Occupied mask of occupied slots, true if occupied, slot 0 and slot 1 are reserved
  * @param Random random input for the random selection (should be much larger than number of channels)
  * @return selected slot number
  */
uint32_t DEMO_ChooseRandomSlot(uint16_t Occupied, uint32_t Random);

#ifdef __cplusplus
}
#endif

#endif /* __DEMO_UTILS_H__ */
