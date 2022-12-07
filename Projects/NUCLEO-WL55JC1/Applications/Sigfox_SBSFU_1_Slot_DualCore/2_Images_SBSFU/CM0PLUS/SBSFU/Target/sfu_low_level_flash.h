/**
  ******************************************************************************
  * @file    sfu_low_level_flash.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update flash
  *          low level interface.
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
#ifndef SFU_LOW_LEVEL_FLASH_H
#define SFU_LOW_LEVEL_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sfu_def.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  SFU_FLASH_ERROR = 0U,       /*!< Error Flash generic*/
  SFU_FLASH_ERR_HAL,          /*!< Error Flash HAL init */
  SFU_FLASH_ERR_ERASE,        /*!< Error Flash erase */
  SFU_FLASH_ERR_WRITING,      /*!< Error writing data in Flash */
  SFU_FLASH_ERR_WRITINGCTRL,  /*!< Error checking data written in Flash */
  SFU_FLASH_SUCCESS           /*!< Flash Success */
} SFU_FLASH_StatusTypeDef;

/* Exported constants --------------------------------------------------------*/
#define INVALID_SECTOR 0xFFFFFFFFU
#define INVALID_PAGE   0xFFFFFFFFU
#define INVALID_BANK   0xFFFFFFFFU

/* Exported functions ------------------------------------------------------- */
SFU_ErrorStatus SFU_LL_FLASH_Init(void);
SFU_ErrorStatus SFU_LL_FLASH_Erase_Size(SFU_FLASH_StatusTypeDef *pxFlashStatus, uint8_t *pStart, uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_Write(SFU_FLASH_StatusTypeDef *pxFlashStatus, uint8_t *pDestination,
                                   const uint8_t *pSource, uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_Read(uint8_t *pDestination, const uint8_t *pSource, uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_Compare(const uint8_t *pFlash, const uint32_t Pattern1, const uint32_t Pattern2, uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_Config_Exe(uint32_t SlotNumber);

uint32_t SFU_LL_FLASH_GetPage(uint32_t Addr);

#ifdef __cplusplus
}
#endif

#endif /* SFU_LOW_LEVEL_FLASH_H */