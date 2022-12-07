/**
  ******************************************************************************
  * @file    nvms_low_level.c
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module NVM Low Level access to physical storage (Flash...)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "stm32wlxx_hal.h"
#include "stm32wlxx_hal_flash.h"
#include "nvms_low_level.h"

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_NVMLL NVM Low Level access
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static bool page_write(const uint8_t **sourcep,
                       uint8_t **destinationp,
                       size_t *sizep);
static uint32_t get_page(uint32_t Addr);
/* Private function ----------------------------------------------------------*/
static bool page_write(const uint8_t **sourcep,
                       uint8_t **destinationp,
                       size_t *sizep)
{
  uint8_t *p;
  uint32_t maxsize;
  HAL_StatusTypeDef ret = HAL_OK;

  /* Calculating the maximum writable chunk size.*/
  maxsize = (size_t)(NVMS_LL_PAGE_SIZE -
                     ((uint32_t) * destinationp & (NVMS_LL_PAGE_SIZE - 1UL)));
  if (maxsize > *sizep)
  {
    maxsize = *sizep;
  }

  p = *destinationp;

  /* Updating size and pointers.*/
  *sizep        -= maxsize;
  *destinationp += maxsize;

  /* Programming, 64 bits granularity  */
  (void)HAL_FLASH_Unlock();
  while ((maxsize >= 8UL) && (ret == HAL_OK))
  {
    ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)p, (uint64_t) * ((uint64_t *)(uint32_t) * sourcep));
    p = &p[8];
    *sourcep = &((*sourcep)[8]);
    maxsize -= 8UL;
  }

  if (maxsize != 0UL)
  {
    uint8_t block[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    /* Problem, there are remaining bytes to write, but not a full 64bit word --> we can't handle it */
    /* Let's write a full 64bit word */
    (void)memcpy(block, *sourcep, maxsize);
    ret = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)p, (uint64_t) * ((uint64_t *)(uint32_t) * sourcep));
  }
  (void)HAL_FLASH_Lock();

  if (ret == HAL_OK)
  {
    return false;
  }
  return true;
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
static uint32_t get_page(uint32_t Addr)
{
  uint32_t page;

  page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;

  return page;
}

/* Exported functions --------------------------------------------------------*/

/** @addtogroup KMS_NVMLL_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief   Flash low level driver initialization.
  */
void NVMS_LL_Init(void)
{

}

/**
  * @brief   Determines if a flash block is in erased state.
  *
  * @param   block         the block identifier
  * @return                  The block state.
  * @retval false            if the block is not in erased state.
  * @retval true             if the block is in erased state.
  */
bool NVMS_LL_IsBlockErased(nvms_block_t block)
{
  uint32_t i;
  uint32_t *p = (uint32_t *)((block == NVMS_BLOCK0) ? NVMS_LL_BLOCK0_ADDRESS :
                             NVMS_LL_BLOCK1_ADDRESS);

  for (i = 0; i < (NVMS_LL_BLOCK_SIZE / sizeof(uint32_t)); i++)
  {
    if (*p != NVMS_LL_ERASED)
    {
      return false;
    }
    p++;
  }
  return true;
}



/**
  * @brief   Erases a block.
  * @note    The erase operation is verified internally.
  *
  * @param   block         the block identifier
  * @return                  The operation status.
  * @retval false            if the operation is successful.
  * @retval true             if the erase operation failed.
  */
bool NVMS_LL_BlockErase(nvms_block_t block)
{
  /* Erase First Flash sector */
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t SectorError = 0;

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Page        = get_page((block == NVMS_BLOCK0) ? NVMS_LL_BLOCK0_ADDRESS : NVMS_LL_BLOCK1_ADDRESS);
  EraseInitStruct.NbPages     = NVMS_LL_NB_PAGE_PER_BLOCK;

  /* Unlock the Flash to enable the flash control register access *************/
  (void)HAL_FLASH_Unlock();

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
  {
    /* Error occurred while sector erase.
      User can add here some code to deal with this error.
      SectorError will contain the faulty sector and then to know the code error on this sector,
      user can call function 'HAL_FLASH_GetError()'
      FLASH_ErrorTypeDef errorcode = HAL_FLASH_GetError(); */
  }

  /* Lock the Flash to disable the flash control register access (recommended
  to protect the FLASH memory against possible unwanted operation) *********/
  (void)HAL_FLASH_Lock();

  /* Operation verification.*/
  return !NVMS_LL_IsBlockErased(block);
}

/**
  * @brief   Writes data.
  * @note    The write operation is verified internally.
  * @note    If the write operation partially writes flash pages then the
  *          unwritten bytes are left to the filler value.
  *
  * @param   source        pointer to the data to be written
  * @param   destination   pointer to the flash position
  * @param   size          size of data
  * @return                  The operation status.
  * @retval false            if the operation is successful.
  * @retval true             if the write operation failed.
  */
bool NVMS_LL_Write(const uint8_t *source, uint8_t *destination, size_t size)
{
  const uint8_t *s = source;
  uint8_t *d = destination;
  size_t sz = size;

  while (sz > 0UL)
  {
    /* Writes as much as a single operation allows.*/
    if (page_write(&s, &d, &sz))
    {
      return true;
    }
  }

  /* Operation verification.*/
  return (bool)(memcmp(source, destination, size) != 0);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
