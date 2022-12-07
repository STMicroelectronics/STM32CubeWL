/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_concentrator.h
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
#ifndef __DEMO_CONCENTRATOR_H__
#define __DEMO_CONCENTRATOR_H__

#include <demo_regions.h>
#include <demo_packet_format.h>

#ifdef __cplusplus
extern "C" {
#endif



/**
  * @brief Initialize Concentrator but don't send anything yet.
  * @return zero on success, 1 - LSE fault, 2 - ARR set fault
  */
int CONC_Init(void);

/**
  * @brief Start sending beacons.
  * @param Region region number as index to CONC_Regions
  * @param Subregion subregion number as index to CONC_Regions[].subregions
  * @return zero on success
  */
int CONC_StartBeacon(uint32_t Region, uint32_t Subregion);

/**
  * @brief Stop sending beacons and forget all sensors.
  */
void CONC_StopBeacon(void);

/**
  * @brief Get whether beacon sending is enabled.
  * @return true when beacons are sent
  */
bool CONC_IsEnabled(void);

/**
  * @brief Return values from CONC_SetModXxxx().
  */
typedef enum
{
  CONC_SETMOD_Ok = 0,       /**< CONC_SETMOD_Ok modulation accepted*/
  CONC_SETMOD_Fail,         /**< CONC_SETMOD_Fail unspecified error*/
  CONC_SETMOD_EuiWrong,     /**< CONC_SETMOD_EuiWrong eui wasn't found*/
  CONC_SETMOD_ModNotAllowed,/**< CONC_SETMOD_ModNotAllowed modulation is not allowed in the region*/
  CONC_SETMOD_ModWrong,     /**< CONC_SETMOD_ModWrong modulation doesn't work*/
} CONC_SetModReturn_t;

/**
  * @brief Change modulation for one sensor to LoRa.
  * @param eui device whose modulation is to change
  * @param param LoRa modulation parameters
  * @param test_only if true, only respond and do not use the modulation
  * @return CONC_SETMOD_Ok when successfully added the modulation, the modulation will change after a while
  */
CONC_SetModReturn_t CONC_SetModLora(uint32_t eui, const DEMO_coding_lora_t *param, bool test_only);

/**
  * @brief Change modulation for one sensor to FSK.
  * @param eui device whose modulation is to change
  * @param param FSK modulation parameters
  * @param test_only if true, only respond and do not use the modulation
  * @return CONC_SETMOD_Ok when successfully added the modulation, the modulation will change after a while
  */
CONC_SetModReturn_t CONC_SetModFSK(uint32_t eui, const DEMO_coding_fsk_t *param, bool test_only);


#ifdef __cplusplus
}
#endif

#endif /* __DEMO_CONCENTRATOR_H__ */
