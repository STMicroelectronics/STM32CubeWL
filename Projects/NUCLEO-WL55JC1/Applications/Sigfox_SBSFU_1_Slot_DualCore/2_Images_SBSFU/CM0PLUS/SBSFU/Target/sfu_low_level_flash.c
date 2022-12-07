/**
  ******************************************************************************
  * @file    sfu_low_level_flash.c
  * @author  MCD Application Team
  * @brief   SFU Flash Low Level Interface module
  *          This file provides set of firmware functions to manage SFU flash
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

/* Includes ------------------------------------------------------------------*/
#include "sfu_low_level_flash_int.h"
#include "sfu_low_level_flash_ext.h"
#include "sfu_fwimg_regions.h"

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  Initialize internal and external flash interface (OSPI/QSPI)
  * @param  none
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_Init(void)
{
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;

  e_ret_status = SFU_LL_FLASH_INT_Init();
  if (e_ret_status == SFU_SUCCESS)
  {
    e_ret_status = SFU_LL_FLASH_EXT_Init();
  }
  return e_ret_status;
}

/**
  * @brief  Depending on start address, this function will call internal or external (OSPI/QSPI) flash driver
  * @param  pFlashStatus: SFU_FLASH Status pointer
  * @param  pStart: flash address to be erased
  * @param  Length: number of bytes
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_Erase_Size(SFU_FLASH_StatusTypeDef *pFlashStatus, uint8_t *pStart, uint32_t Length)
{

  /* Check Flash start address */
  if ((uint32_t) pStart < EXTERNAL_FLASH_ADDRESS)
  {
    return SFU_LL_FLASH_INT_Erase_Size(pFlashStatus, pStart, Length);
  }
  else
  {
    return SFU_LL_FLASH_EXT_Erase_Size(pFlashStatus, pStart, Length);
  }
}

/**
  * @brief  Depending on destination address, this function will call internal or external (OSPI/QSPI) flash driver
  * @param  pFlashStatus: FLASH_StatusTypeDef
  * @param  pDestination: flash address to write
  * @param  pSource: pointer on buffer with data to write
  * @param  Length: number of bytes
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_Write(SFU_FLASH_StatusTypeDef *pFlashStatus, uint8_t  *pDestination,
                                   const uint8_t *pSource, uint32_t Length)
{
  /* Check Flash destination address */
  if ((uint32_t) pDestination < EXTERNAL_FLASH_ADDRESS)
  {
    return SFU_LL_FLASH_INT_Write(pFlashStatus, pDestination, pSource, Length);
  }
  else
  {
    return SFU_LL_FLASH_EXT_Write(pFlashStatus, pDestination, pSource, Length);
  }
}

/**
  * @brief  Depending on source address, this function will call internal or external (OSPI/QSPI) flash driver
  * @param  pDestination: pointer on buffer to store data
  * @param  pSource: flash address to read
  * @param  Length: number of bytes
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_Read(uint8_t *pDestination, const uint8_t *pSource, uint32_t Length)
{
  /* Check Flash source address */
  if ((uint32_t) pSource < EXTERNAL_FLASH_ADDRESS)
  {
    return SFU_LL_FLASH_INT_Read(pDestination, pSource, Length);
  }
  else
  {
    return SFU_LL_FLASH_EXT_Read(pDestination, pSource, Length);
  }
}

/**
  * @brief  This function compare a buffer with a flash area
  * @note   The flash area should not be located inside the secure area
  * @param  pFlash: address of the flash area
  * @param  Pattern1: first 32 bits pattern to be compared
  * @param  Pattern2: second 32 bits pattern to be compared
  * @param  Length: number of bytes to be compared
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_Compare(const uint8_t *pFlash, const uint32_t Pattern1, const uint32_t Pattern2, uint32_t Length)
{
  /* Check Flash source address */
  if ((uint32_t) pFlash < EXTERNAL_FLASH_ADDRESS)
  {
    return SFU_LL_FLASH_INT_Compare(pFlash, Pattern1, Pattern2, Length);
  }
  else
  {
    return SFU_LL_FLASH_EXT_Compare(pFlash, Pattern1, Pattern2, Length);
  }
}

/**
  * @brief  This function configure the flash to be able to execute code
  * @param  Addr: flash address
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_FLASH_Config_Exe(uint32_t SlotNumber)
{
  /*
   * Internal flash : nothing to do
   * External flash : configure memory mapped mode
   */

  /* Check Flash address */
  if (SlotStartAdd[SlotNumber] < EXTERNAL_FLASH_ADDRESS)
  {
    return SFU_SUCCESS;
  }
  else
  {
    return SFU_LL_FLASH_EXT_Config_Exe(SlotNumber);
  }
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: flash address
  * @retval The page of a given address
  */
uint32_t SFU_LL_FLASH_GetPage(uint32_t Addr)
{
  /* Check Flash address */
  if (Addr < EXTERNAL_FLASH_ADDRESS)
  {
    return SFU_LL_FLASH_INT_GetPage(Addr);
  }
  else
  {
    return INVALID_PAGE;                         /* Page number is not used in SBSFU application for external flash */
  }
}

