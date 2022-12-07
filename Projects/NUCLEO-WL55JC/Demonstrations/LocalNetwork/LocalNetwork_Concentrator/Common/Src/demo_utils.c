/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_utils.c
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
/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "demo_utils.h"


#define DEMO_MAX(a, b)          (((a) >= (b)) ? (a) : (b))      /*Larger of two*/
#define DEMO_CEIL_DIV(a, b)     (((a) + (b) - 1) / (b))         /*Ceil after division*/

/*Convert bw settings [0~9] into bandwidth [Hz]*/
static const int DEMO_BandwidthTable[9] = { 7800, 10400, 15600, 20800, 31250, 41700, 62500, 125000, 250000, };  /*[Hz]*/
#define DEMO_BW_TO_HZ(bw)       (((uint32_t)(bw) < 9) ? DEMO_BandwidthTable[(uint32_t)(bw)] : 500000)


static int CheckLimits(int input, int min, int max);

/**
  * @brief Calculate sensor's packet length.
  * @param Coding packet coding
  * @param Payload number of bytes in payload
  * @return packet length [ms]
  */
uint32_t DEMO_TimeOnAirLora(const DEMO_coding_lora_t *Coding, uint32_t Payload)
{
  uint32_t t_s; /*Symbol duration [us]*/
  int32_t n_payload; /*Number of symbols of payload*/

  t_s = (1000000 << Coding->sf) / DEMO_BW_TO_HZ(Coding->bw); /*Symbol time [us]*/

  n_payload = 8 + DEMO_MAX(
                DEMO_CEIL_DIV(((8 * (int32_t)Payload) - (4 * (int32_t)Coding->sf) + 28 + 16),
                              (4 * ((int32_t)Coding->sf - (2 * (int32_t)Coding->de))))
                * ((int32_t)Coding->cr + 4), 0);   /*Number of symbols of payload*/

  /*Return packet length [ms]*/
  return ((32 + 17 + (4 * n_payload)) * t_s / 4000); /* ((8 + 4.25 + n_payload) * t_s / 1000) */
}

/**
  * @brief Calculate sensor's packet length.
  * @param Coding packet coding
  * @param Payload number of bytes in payload
  * @return packet length [ms]
  */
uint32_t DEMO_TimeOnAirFsk(const DEMO_coding_fsk_t *Coding, uint32_t Payload)
{
  return (1000 * (4 + 4 + Payload + 1 + 2) * 8) / Coding->br; /*Return packet length [ms]*/
}

/**
  * @brief Get the longest payload that can be sent in the given subregion.
  * @param Coding packet coding
  * @param Subregion local subregion
  * @param packet_time if not NULL, length of the packet will be written there [ms]
  * @return number of bytes which can be sent
  */
uint32_t DEMO_MaxPayloadLora(const DEMO_coding_lora_t *Coding,
                             const DEMO_Subregion_t *Subregion, uint32_t *packet_time)
{
  uint32_t payload;
  uint32_t temp_time;

  for (payload = DEMO_SENSOR_MAX_PAYLOAD; payload > 0; payload--)
  {
    temp_time = DEMO_TimeOnAirLora(Coding, payload);
    if (temp_time < Subregion->max_t)
    {
      break;
    }
  }

  if (packet_time != NULL)
  {
    *packet_time = temp_time;   /*Store TimeOnAir of the packet*/
  }
  return payload; /*First value which makes packet shorter than limit*/
}

/**
  * @brief Get the longest payload that can be sent in the given subregion.
  * @param Coding packet coding
  * @param Subregion local subregion
  * @param packet_time if not NULL, length of the packet will be written there [ms]
  * @return number of bytes which can be sent
  */
uint32_t DEMO_MaxPayloadFsk(const DEMO_coding_fsk_t *Coding,
                            const DEMO_Subregion_t *Subregion, uint32_t *packet_time)
{
  uint32_t payload;
  uint32_t temp_time;

  for (payload = DEMO_SENSOR_MAX_PAYLOAD; payload > 0; payload--)
  {
    temp_time = DEMO_TimeOnAirFsk(Coding, payload);
    if (temp_time < Subregion->max_t)
    {
      break;
    }
  }

  if (packet_time != NULL)
  {
    *packet_time = temp_time;   /*Store TimeOnAir of the packet*/
  }
  return payload; /*First value which makes packet shorter than limit*/
}

/**
  * @brief Set coding to default.
  * @param Codings structure to clear
  * @param SlotNr clear structure for this slot
  * @param Region default coding is the same as beacon
  */
void DEMO_DefaultCoding(DEMO_codings_t *Codings, uint32_t SlotNr, const DEMO_Region_t *Region)
{
  if ((SlotNr < 2) || (SlotNr >= DEMO_SLOT_NUMBER))
  {
    SlotNr = 0;
  }

  Codings->hdr.data_lim = DEMO_SENSOR_MAX_PAYLOAD;
  Codings->hdr.mode = DEMO_DEFAULT_LORA;
  Codings->hdr.period = 0;
  Codings->hdr.slot = SlotNr;
  Codings->hdr.chan_nr = 0;

  Codings->lora.bw = Region->beacon_bw;
  Codings->lora.cr = DEMO_DEFAULT_CR;
  Codings->lora.de = DEMO_DEFAULT_DE;
  Codings->lora.sf = Region->beacon_sf;

  Codings->fsk.br = DEMO_DEFAULT_BR;
  Codings->fsk.bt = DEMO_DEFAULT_BT;
  Codings->fsk.fdev = DEMO_DEFAULT_FDEV;
  Codings->fsk.rise = DEMO_DEFAULT_RISE;
}

/**
  * @brief Check if the sensor coding is the default coding.
  * @param Sensor structure for Sensor
  * @return true if the coding is default
  */
bool DEMO_IsCodingDefault(const DEMO_codings_t *Codings, const DEMO_Region_t *Region)
{
  if ((Codings->hdr.data_lim != DEMO_SENSOR_MAX_PAYLOAD)
      || (Codings->hdr.mode != DEMO_DEFAULT_LORA)
      || (Codings->hdr.period != 0)
      || (Codings->hdr.chan_nr != 0)
      || (Codings->lora.bw != Region->beacon_bw)
      || (Codings->lora.cr != DEMO_DEFAULT_CR)
      || (Codings->lora.de != DEMO_DEFAULT_DE)
      || (Codings->lora.sf != Region->beacon_sf)
      || (Codings->fsk.br != DEMO_DEFAULT_BR)
      || (Codings->fsk.bt != DEMO_DEFAULT_BT)
      || (Codings->fsk.fdev != DEMO_DEFAULT_FDEV)
      || (Codings->fsk.rise != DEMO_DEFAULT_RISE))
  {
    return false;       /*There is at least one difference*/
  }

  return true;  /*Equal*/
}

/**
  * @brief Get modulation bandwidth.
  * @param Coding modulation
  * @return bandwidth [Hz]
  */
uint32_t DEMO_BandwidthLora(const DEMO_coding_lora_t *Coding)
{
  return ((16 * DEMO_BW_TO_HZ(Coding->bw)) / 10); /*Bandwidth = bw * 1.6, (guessed from LoRaWAN documentation)*/
}

/**
  * @brief Get modulation bandwidth.
  * @param Coding modulation
  * @return bandwidth [Hz]
  */
uint32_t DEMO_BandwidthFsk(const DEMO_coding_fsk_t *Coding)
{
  uint32_t bt10;        /*10*BT factor*/
  switch (Coding->bt)
  {
    default:
      bt10 = 10;
      break;
    case 1:
      bt10 = 3;
      break;
    case 2:
      bt10 = 5;
      break;
    case 3:
      bt10 = 7;
      break;
  }

  return (2 * (((bt10 * Coding->br) / 10) + Coding->fdev)); /*Carson's rule to guess bandwidth*/
}

/**
  * @brief Check and limit input.
  * @param input input variable
  * @param min lower limit
  * @param max upper limit
  * @return output limited variable
  */
static int CheckLimits(int input, int min, int max)
{
  if (input < min)
  {
    return min;
  }
  else if (input > max)
  {
    return max;
  }

  return input;
}

/**
  * @brief Validate and copy coding header.
  * @param Hdr structure to validate
  * @param Subregion pointer to subregion used to validate
  */
void DEMO_ValidateCodingHdr(DEMO_coding_hdr_t *Hdr, const DEMO_Subregion_t *Subregion)
{
  Hdr->data_lim = CheckLimits(Hdr->data_lim, 5, DEMO_SENSOR_MAX_PAYLOAD);

  if (Hdr->chan_nr >= Subregion->channels_n)
  {
    Hdr->chan_nr = 0;
  }
}

/**
  * @brief Validate and copy coding parameters.
  * @param Coding structure to validate
  */
void DEMO_ValidateCodingLora(DEMO_coding_lora_t *Coding)
{
  Coding->bw = CheckLimits(Coding->bw, DEMO_LORA_PARAM_BW_MIN, DEMO_LORA_PARAM_BW_MAX);
  Coding->cr = CheckLimits(Coding->cr, DEMO_LORA_PARAM_CR_MIN, DEMO_LORA_PARAM_CR_MAX);
  Coding->sf = CheckLimits(Coding->sf, DEMO_LORA_PARAM_SF_MIN, DEMO_LORA_PARAM_SF_MAX);
}

/**
  * @brief Validate and copy coding parameters.
  * @param Coding structure to validate
  */
void DEMO_ValidateCodingFsk(DEMO_coding_fsk_t *Coding)
{
  Coding->br = CheckLimits(Coding->br, DEMO_FSK_PARAM_BR_MIN, DEMO_FSK_PARAM_BR_MAX);
  Coding->bt = CheckLimits(Coding->bt, DEMO_FSK_PARAM_BT_MIN, DEMO_FSK_PARAM_BT_MAX);
  Coding->fdev = CheckLimits(Coding->fdev, DEMO_FSK_PARAM_FDEV_MIN, DEMO_FSK_PARAM_FDEV_MAX);
  Coding->rise = CheckLimits(Coding->rise, DEMO_FSK_PARAM_RISE_MIN, DEMO_FSK_PARAM_RISE_MAX);
}

/**
  * @brief Get number of free slots.
  * @param Occupied mask of occupied slots
  * @return number of free slots
  */
uint32_t DEMO_CountFreeSlots(uint16_t Occupied)
{
  uint32_t free_slots = DEMO_SLOT_NUMBER - 2;   /*Slots 0 and 1 are taken by Beacon and Sync*/
  for (uint16_t oc_copy = (Occupied & ~0x3); (free_slots > 0) && (oc_copy != 0x0000); free_slots--)
  {
    oc_copy &= oc_copy - 1; /*Remove least significant 1*/
  }
  return free_slots;
}

/**
  * @brief Pick one slot from the slots marked as free in Occupied mask.
  * @param Occupied mask of occupied slots, true if occupied, slot 0 and slot 1 are reserved
  * @param Random random input for the random selection (should be much larger than number of channels)
  * @return selected slot number
  */
uint32_t DEMO_ChooseRandomSlot(uint16_t Occupied, uint32_t Random)
{
  uint32_t random_nr = DEMO_CountFreeSlots(Occupied); /*Get number of free slots*/
  if (random_nr == 0)
  {
    return 0;   /*No slot left*/
  }
  random_nr = (Random % random_nr) + 1;   /*Get random number between 1 and number of free slots*/

  /*Offset for used slots*/
  uint32_t slot;
  for (slot = 1; (slot < DEMO_SLOT_NUMBER) && (random_nr > 0); random_nr--)
  {
    do
    {
      slot++; /*Find next free slot*/
    } while ((Occupied & (1 << slot)) != 0);
  }

  if (slot == DEMO_SLOT_NUMBER) /*Error, this shouldn't happen*/
  {
    return 0;
  }

  return slot;
}

