/**
  ******************************************************************************
  * @file    sfu_low_level_flash_int.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update low level
  *          interface for internal flash.
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
#ifndef SFU_LOW_LEVEL_FLASH_INT_H
#define SFU_LOW_LEVEL_FLASH_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sfu_low_level_flash.h"
#include "main.h"
#include "sfu_def.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief Flash Write Access Constraints (size and alignment)
  *
  * For instance, on L4, it is only possible to program double word (2 x 32-bit data).
  * See http://www.st.com/content/ccc/resource/technical/document/reference_manual/02/35/09/0c/4f/f7/40/03/DM00083560.pdf/files/DM00083560.pdf/jcr:content/translations/en.DM00083560.pdf
  *
  * @note This type is very important for the FWIMG module (see @ref SFU_IMG).
  * \li This is the type to be used for an atomic write in FLASH: see @ref AtomicWrite.
  * \li The size of this type changes the size of the TRAILER area at the end of dwl slot,
  *     as it is used to tag if a Firmware Image chunk has been swapped or not (see @ref SFU_IMG_FirmwareToResume).
  */

/* double-word is the default setting for most platforms */
typedef uint64_t SFU_LL_FLASH_write_t;

/* Exported constants --------------------------------------------------------*/
/**
  * Length of a MAGIC tag (32 bytes).
  * This must be a multiple of @ref SFU_LL_FLASH_write_t with a minimum value of 32.
  */
#define MAGIC_LENGTH ((uint32_t)32U)

/* External variables --------------------------------------------------------*/
#define IS_ALIGNED(address) (0U == ((address) % FLASH_PAGE_SIZE))

/* Exported functions ------------------------------------------------------- */
SFU_ErrorStatus SFU_LL_FLASH_INT_Init(void);
SFU_ErrorStatus SFU_LL_FLASH_INT_Erase_Size(SFU_FLASH_StatusTypeDef *pxFlashStatus, uint8_t *pStart, uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_INT_Write(SFU_FLASH_StatusTypeDef *pxFlashStatus, uint8_t *pDestination,
                                       const uint8_t *pSource, uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_INT_Read(uint8_t *pDestination, const uint8_t *pSource, uint32_t Length);
SFU_ErrorStatus SFU_LL_FLASH_INT_Compare(const uint8_t *pFlash, const uint32_t Pattern1, const uint32_t Pattern2, uint32_t Length);
uint32_t SFU_LL_FLASH_INT_GetPage(uint32_t Addr);
void NMI_Handler(void);
void HardFault_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* SFU_LOW_LEVEL_FLASH_INT_H */