/**
  ******************************************************************************
  * @file    sfu_low_level_flash_ext.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update low level
  *          interface for external flash.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SFU_LOW_LEVEL_FLASH_EXT_H
#define SFU_LOW_LEVEL_FLASH_EXT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sfu_low_level_flash.h"
#include "main.h"
#include "sfu_def.h"

/* Exported constants --------------------------------------------------------*/
#define EXTERNAL_FLASH_ADDRESS  0x90000000U

/* Exported functions ------------------------------------------------------- */
SFU_ErrorStatus SFU_LL_FLASH_EXT_Init(void);
SFU_ErrorStatus SFU_LL_FLASH_EXT_Erase_Size(SFU_FLASH_StatusTypeDef *pxFlashStatus, uint8_t *pStart, uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_EXT_Write(SFU_FLASH_StatusTypeDef *pxFlashStatus, uint8_t *pDestination,
                                       const uint8_t *pSource, uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_EXT_Read(uint8_t *pDestination, const uint8_t *pSource, uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_EXT_Compare(const uint8_t *pFlash, const uint32_t Pattern1, const uint32_t Pattern2, uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_EXT_Config_Exe(uint32_t SlotNumber);

#ifdef __cplusplus
}
#endif

#endif /* SFU_LOW_LEVEL_FLASH_EXT_H */