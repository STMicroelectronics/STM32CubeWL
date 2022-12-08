/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    flash_if.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage Flash
  *          Interface functionalities.
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
#include "flash_if.h"
#include "stm32_mem.h"
#include "b_wl5m_subg_flash.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/**
  * @brief Flash empty status enumeration
  */
enum
{
  FLASH_IF_MEM_EMPTY     = 0,
  FLASH_IF_MEM_NOT_EMPTY = 1
};

#define FLASH_IF_EXT_FLASH_ADDRESS    0x90000000U

#define FLASH_IF_EXT_INSTANCE         0

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/**
  * @brief Get internal flash page index from page address
  */
#define PAGE_INDEX(__ADDRESS__)     (uint32_t)((((__ADDRESS__) - FLASH_BASE) % FLASH_BANK_SIZE) / FLASH_PAGE_SIZE)

/**
  * @brief Get external flash sector index from page address
  */
#define SECTOR_INDEX(__ADDRESS__)   (uint32_t)(((__ADDRESS__) - FLASH_IF_EXT_FLASH_ADDRESS) / MX25L4006_SECTOR_SIZE)

/**
  * @brief Check if value is in external flash memory range
  */
#define IS_FLASH_EXT_MEM_ADDRESS(__VALUE__) (((__VALUE__) >= FLASH_IF_EXT_FLASH_ADDRESS) &&\
                                             ((__VALUE__) <= (FLASH_IF_EXT_FLASH_ADDRESS + MX25L4006_FLASH_SIZE - 1UL)))

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static uint8_t *pAllocatedBuffer = NULL;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* Private Functions prototypes: internal flash ------------------------------*/
/**
  * @brief  This function writes a data buffer in flash (data are 64-bit aligned).
  *
  * @note   After writing data buffer, the flash content is checked.
  * @param  pDestination: Start address for target location. It has to be 8 bytes aligned.
  * @param  pSource: pointer on buffer with data to write
  * @param  uLength: Length of data buffer in bytes. It has to be 8 bytes aligned.
  * @return FLASH_IF_StatusTypedef status
  */
static FLASH_IF_StatusTypedef FLASH_IF_INT_Write(void *pDestination, const void *pSource, uint32_t uLength);

/**
  * @brief  This function reads flash
  *
  * @param  pDestination: Start address for target location
  * @param  pSource: flash address to read
  * @param  uLength: number of bytes
  * @return FLASH_IF_StatusTypedef status
  */
static FLASH_IF_StatusTypedef FLASH_IF_INT_Read(void *pDestination, const void *pSource, uint32_t uLength);

/**
  * @brief This function does an erase of n (depends on Length) pages in user flash area
  *
  * @param pStart pointer of flash address to be erased
  * @param uLength number of bytes
  * @return FLASH_IF_StatusTypedef status
  */
static FLASH_IF_StatusTypedef FLASH_IF_INT_Erase(void *pStart, uint32_t uLength);

/**
  * @brief This function checks if part of Flash is empty
  *
  * @param pStart flash address to check
  * @param uLength number of bytes to check. It has to be 8 bytes aligned.
  * @return int32_t FLASH_IF_MEM_EMPTY or FLASH_IF_MEM_NOT_EMPTY
  */
static int32_t FLASH_IF_INT_IsEmpty(void *pStart, uint32_t uLength);

/**
  * @brief  Clear error flags raised during previous operation
  *
  * @retval FLASH_IF_StatusTypedef status
  */
static FLASH_IF_StatusTypedef FLASH_IF_INT_Clear_Error(void);

/* Private Functions prototypes: external flash ------------------------------*/
/**
  * @brief  This function writes a data buffer in external OSPI flash.
  *
  * @param  pDestination: flash address to write
  * @param  pSource: pointer on buffer with data to write
  * @param  uLength: number of bytes
  * @retval FLASH_IF_StatusTypedef status
  */
static FLASH_IF_StatusTypedef FLASH_IF_EXT_Write(void *pDestination, const void *pSource, uint32_t uLength);

/**
  * @brief  This function reads external OSPI flash
  *
  * @param  pDestination: pointer on buffer to store data
  * @param  pSource: flash address to read
  * @param  uLength: number of bytes
  * @retval FLASH_IF_StatusTypedef status
  */
static FLASH_IF_StatusTypedef FLASH_IF_EXT_Read(void *pDestination, const void *pSource, uint32_t uLength);

/**
  * @brief This function does an erase of n (depends on Length) pages in external OSPI flash
  *
  * @param pStart flash address to be erased
  * @param uLength number of bytes
  * @return FLASH_IF_StatusTypedef status
  */
static FLASH_IF_StatusTypedef FLASH_IF_EXT_Erase(void *pStart, uint32_t uLength);

/**
  * @brief This function checks if part of Flash is empty
  *
  * @param pStart flash address to check
  * @param uLength number of bytes to check. It has to be 8 bytes aligned.
  * @return int32_t FLASH_IF_MEM_EMPTY or FLASH_IF_MEM_NOT_EMPTY
  */
static int32_t FLASH_IF_EXT_IsEmpty(void *pStart, uint32_t uLength);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
FLASH_IF_StatusTypedef FLASH_IF_Init(void *pAllocRamBuffer)
{
  FLASH_IF_StatusTypedef ret_status = FLASH_IF_OK;
  /* USER CODE BEGIN FLASH_IF_Init_1 */

  /* USER CODE END FLASH_IF_Init_1 */
  pAllocatedBuffer = (uint8_t *)pAllocRamBuffer;

  if (BSP_FLASH_Init(FLASH_IF_EXT_INSTANCE) != BSP_ERROR_NONE)
  {
    ret_status = FLASH_IF_ERROR;
  }
  /* USER CODE BEGIN FLASH_IF_Init_2 */

  /* USER CODE END FLASH_IF_Init_2 */
  return ret_status;
}

FLASH_IF_StatusTypedef FLASH_IF_DeInit(void)
{
  FLASH_IF_StatusTypedef ret_status = FLASH_IF_OK;
  /* USER CODE BEGIN FLASH_IF_DeInit_1 */

  /* USER CODE END FLASH_IF_DeInit_1 */
  pAllocatedBuffer = NULL;

  if (BSP_FLASH_DeInit(FLASH_IF_EXT_INSTANCE) != BSP_ERROR_NONE)
  {
    ret_status = FLASH_IF_ERROR;
  }
  /* USER CODE BEGIN FLASH_IF_DeInit_2 */

  /* USER CODE END FLASH_IF_DeInit_2 */
  return ret_status;
}

FLASH_IF_StatusTypedef FLASH_IF_Write(void *pDestination, const void *pSource, uint32_t uLength)
{
  FLASH_IF_StatusTypedef ret_status = FLASH_IF_ERROR;
  /* USER CODE BEGIN FLASH_IF_Write_1 */

  /* USER CODE END FLASH_IF_Write_1 */
  if (IS_FLASH_MAIN_MEM_ADDRESS((uint32_t)pDestination))
  {
    ret_status = FLASH_IF_INT_Write(pDestination, pSource, uLength);
  }
  else if (IS_FLASH_EXT_MEM_ADDRESS((uint32_t)pDestination))
  {
    ret_status = FLASH_IF_EXT_Write(pDestination, pSource, uLength);
  }
  /* USER CODE BEGIN FLASH_IF_Write_2 */

  /* USER CODE END FLASH_IF_Write_2 */
  return ret_status;
}

FLASH_IF_StatusTypedef FLASH_IF_Read(void *pDestination, const void *pSource, uint32_t uLength)
{
  FLASH_IF_StatusTypedef ret_status = FLASH_IF_ERROR;
  /* USER CODE BEGIN FLASH_IF_Read_1 */

  /* USER CODE END FLASH_IF_Read_1 */
  if (IS_FLASH_MAIN_MEM_ADDRESS((uint32_t)pSource))
  {
    ret_status = FLASH_IF_INT_Read(pDestination, pSource, uLength);
  }
  else if (IS_FLASH_EXT_MEM_ADDRESS((uint32_t)pSource))
  {
    ret_status = FLASH_IF_EXT_Read(pDestination, pSource, uLength);
  }
  /* USER CODE BEGIN FLASH_IF_Read_2 */

  /* USER CODE END FLASH_IF_Read_2 */
  return ret_status;
}

FLASH_IF_StatusTypedef FLASH_IF_Erase(void *pStart, uint32_t uLength)
{
  FLASH_IF_StatusTypedef ret_status = FLASH_IF_ERROR;
  /* USER CODE BEGIN FLASH_IF_Erase_1 */

  /* USER CODE END FLASH_IF_Erase_1 */
  /* Check Flash start address */
  if (IS_FLASH_MAIN_MEM_ADDRESS((uint32_t)pStart))
  {
    ret_status = FLASH_IF_INT_Erase(pStart, uLength);
  }
  else if (IS_FLASH_EXT_MEM_ADDRESS((uint32_t)pStart))
  {
    ret_status = FLASH_IF_EXT_Erase(pStart, uLength);
  }
  /* USER CODE BEGIN FLASH_IF_Erase_2 */

  /* USER CODE END FLASH_IF_Erase_2 */
  return ret_status;
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

/* Private Functions : internal flash -----------------------------------------*/
static FLASH_IF_StatusTypedef FLASH_IF_INT_Write(void *pDestination, const void *pSource, uint32_t uLength)
{
  FLASH_IF_StatusTypedef ret_status = FLASH_IF_OK;
  /* USER CODE BEGIN FLASH_IF_INT_Write_1 */

  /* USER CODE END FLASH_IF_INT_Write_1 */
  uint32_t uDest = (uint32_t)pDestination;
  uint32_t uSource = (uint32_t)pSource;
  uint32_t length = uLength;
  uint32_t page_index;
  uint32_t address_offset;
  uint32_t start_page_index;
  uint32_t page_address;
  uint32_t number_pages;
  uint32_t current_dest;
  uint32_t current_source;
  uint32_t current_length;

  if ((pDestination == NULL) || (pSource == NULL) || !IS_ADDR_ALIGNED_64BITS(uLength)
      || !IS_ADDR_ALIGNED_64BITS((uint32_t)pDestination))
  {
    return FLASH_IF_PARAM_ERROR;
  }

  /* Clear error flags raised during previous operation */
  ret_status = FLASH_IF_INT_Clear_Error();

  if (ret_status == FLASH_IF_OK)
  {
    /* Unlock the Flash to enable the flash control register access */
    if (HAL_FLASH_Unlock() == HAL_OK)
    {
      start_page_index = PAGE_INDEX(uDest);
      number_pages = PAGE_INDEX(uDest + uLength - 1U) - start_page_index + 1U;

      if (number_pages > 1)
      {
        length = FLASH_PAGE_SIZE - (uDest % FLASH_PAGE_SIZE);
      }

      for (page_index = start_page_index; page_index < (start_page_index + number_pages); page_index++)
      {
        page_address = page_index * FLASH_PAGE_SIZE + FLASH_BASE;
        if (FLASH_IF_INT_IsEmpty(pDestination, length) != FLASH_IF_MEM_EMPTY)
        {
          if (pAllocatedBuffer == NULL)
          {
            ret_status = FLASH_IF_PARAM_ERROR;
            break; /* exit for loop */
          }

          /* backup initial Flash page data in RAM area */
          FLASH_IF_INT_Read(pAllocatedBuffer, (const void *)page_address, FLASH_PAGE_SIZE);
          /* copy fragment into RAM area */
          UTIL_MEM_cpy_8(&pAllocatedBuffer[uDest % FLASH_PAGE_SIZE], (const void *)uSource, length);

          /*  erase the Flash sector, to avoid writing twice in RAM */
          if (FLASH_IF_INT_Erase((void *)page_address, FLASH_PAGE_SIZE) != FLASH_IF_OK)
          {
            ret_status = FLASH_IF_ERASE_ERROR;
            break; /* exit for loop */
          }

          /* copy the whole flash sector including fragment from RAM to Flash */
          current_dest = page_address;
          current_source = (uint32_t)pAllocatedBuffer;
          current_length = FLASH_PAGE_SIZE;
        }
        else
        {
          /* write a part of flash page from selected source data */
          current_dest = uDest;
          current_source = uSource;
          current_length = length;
        }

        for (address_offset = 0U; address_offset < current_length; address_offset += 8U)
        {
          /* Device voltage range supposed to be [2.7V to 3.6V], the operation will be done by word */
          if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, current_dest,
                                *((uint64_t *)(current_source + address_offset))) == HAL_OK)
          {
            /* Check the written value */
            if (*(uint64_t *)current_dest != *(uint64_t *)(current_source + address_offset))
            {
              /* Flash content doesn't match SRAM content */
              ret_status = FLASH_IF_WRITE_ERROR;
              break;
            }
            /* Increment FLASH Destination address */
            current_dest = current_dest + 8U;
          }
          else
          {
            /* Error occurred while writing data in Flash memory */
            ret_status = FLASH_IF_WRITE_ERROR;
            break;
          }
        }

        if (ret_status != FLASH_IF_OK)
        {
          /* Error occurred while writing data in Flash memory */
          break;
        }

        /* Increment FLASH destination address, source address, and decrease remaining length */
        uDest += length;
        uSource += length;
        length = ((uLength - length) > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : uLength - length;
      }

      /* Lock the Flash to disable the flash control register access (recommended
       * to protect the FLASH memory against possible unwanted operation) */
      HAL_FLASH_Lock();
    }
    else
    {
      ret_status = FLASH_IF_LOCK_ERROR;
    }
  }
  /* USER CODE BEGIN FLASH_IF_INT_Write_2 */

  /* USER CODE END FLASH_IF_INT_Write_2 */
  return ret_status;
}

static FLASH_IF_StatusTypedef FLASH_IF_INT_Read(void *pDestination, const void *pSource, uint32_t uLength)
{
  FLASH_IF_StatusTypedef ret_status = FLASH_IF_OK;
  /* USER CODE BEGIN FLASH_IF_INT_Read_1 */

  /* USER CODE END FLASH_IF_INT_Read_1 */
  if ((pDestination == NULL) || (pSource == NULL))
  {
    return FLASH_IF_PARAM_ERROR;
  }

  UTIL_MEM_cpy_8(pDestination, pSource, uLength);
  /* USER CODE BEGIN FLASH_IF_INT_Read_2 */

  /* USER CODE END FLASH_IF_INT_Read_2 */
  return ret_status;
}

static FLASH_IF_StatusTypedef FLASH_IF_INT_Erase(void *pStart, uint32_t uLength)
{
  FLASH_IF_StatusTypedef ret_status = FLASH_IF_OK;
  /* USER CODE BEGIN FLASH_IF_INT_Erase_1 */

  /* USER CODE END FLASH_IF_INT_Erase_1 */
  HAL_StatusTypeDef hal_status = HAL_ERROR;
  uint32_t page_error = 0U;
  uint32_t uStart = (uint32_t)pStart;
  FLASH_EraseInitTypeDef erase_init;

  if (pStart == NULL)
  {
    return FLASH_IF_PARAM_ERROR;
  }

  /* Clear error flags raised during previous operation */
  ret_status = FLASH_IF_INT_Clear_Error();

  if (ret_status == FLASH_IF_OK)
  {
    /* Unlock the Flash to enable the flash control register access */
    if (HAL_FLASH_Unlock() == HAL_OK)
    {
      erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
      erase_init.Page = PAGE_INDEX(uStart);
      /* Get the number of pages to erase from 1st page */
      erase_init.NbPages = PAGE_INDEX(uStart + uLength - 1U) - erase_init.Page + 1U;

      /* Erase the Page */
      hal_status = HAL_FLASHEx_Erase(&erase_init, &page_error);

      if (hal_status != HAL_OK)
      {
        ret_status = (hal_status == HAL_BUSY) ? FLASH_IF_BUSY : FLASH_IF_ERASE_ERROR;
      }

      /* Lock the Flash to disable the flash control register access (recommended
       * to protect the FLASH memory against possible unwanted operation) */
      HAL_FLASH_Lock();
    }
    else
    {
      ret_status = FLASH_IF_LOCK_ERROR;
    }
  }
  /* USER CODE BEGIN FLASH_IF_INT_Erase_2 */

  /* USER CODE END FLASH_IF_INT_Erase_2 */
  return ret_status;
}

static int32_t FLASH_IF_INT_IsEmpty(void *pStart, uint32_t uLength)
{
  int32_t status = FLASH_IF_MEM_EMPTY;
  /* USER CODE BEGIN FLASH_IF_INT_IsEmpty_1 */

  /* USER CODE END FLASH_IF_INT_IsEmpty_1 */
  uint32_t index;
  for (index = 0; index < uLength; index += 8)
  {
    if (*(uint64_t *)pStart != UINT64_MAX)
    {
      status = FLASH_IF_MEM_NOT_EMPTY;
      break;
    }
    pStart = (void *)((uint32_t)pStart + 8U);
  }
  /* USER CODE BEGIN FLASH_IF_INT_IsEmpty_2 */

  /* USER CODE END FLASH_IF_INT_IsEmpty_2 */
  return status;
}

static FLASH_IF_StatusTypedef FLASH_IF_INT_Clear_Error(void)
{
  FLASH_IF_StatusTypedef ret_status = FLASH_IF_LOCK_ERROR;
  /* USER CODE BEGIN FLASH_IF_INT_Clear_Error_1 */

  /* USER CODE END FLASH_IF_INT_Clear_Error_1 */
  /* Unlock the Program memory */
  if (HAL_FLASH_Unlock() == HAL_OK)
  {
    /* Clear all FLASH flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    /* Unlock the Program memory */
    if (HAL_FLASH_Lock() == HAL_OK)
    {
      ret_status = FLASH_IF_OK;
    }
  }
  /* USER CODE BEGIN FLASH_IF_INT_Clear_Error_2 */

  /* USER CODE END FLASH_IF_INT_Clear_Error_2 */
  return ret_status;
}

/* Private Functions : external flash -----------------------------------------*/
static FLASH_IF_StatusTypedef FLASH_IF_EXT_Write(void *pDestination, const void *pSource, uint32_t uLength)
{
  FLASH_IF_StatusTypedef ret_status = FLASH_IF_OK;
  /* USER CODE BEGIN FLASH_IF_EXT_Write_1 */

  /* USER CODE END FLASH_IF_EXT_Write_1 */
  uint32_t uDest = (uint32_t)pDestination;
  uint32_t uSource = (uint32_t)pSource;
  uint32_t length = uLength;
  uint32_t sector_index;
  uint32_t start_sector_index;
  uint32_t sector_address;
  uint32_t number_sectors;
  uint32_t current_dest;
  uint32_t current_source;
  uint32_t current_length;

  if ((pDestination == NULL) || (pSource == NULL) || !IS_ADDR_ALIGNED_64BITS(uLength)
      || !IS_ADDR_ALIGNED_64BITS((uint32_t)pDestination))
  {
    return FLASH_IF_PARAM_ERROR;
  }

  start_sector_index = SECTOR_INDEX(uDest);
  number_sectors = SECTOR_INDEX(uDest + uLength - 1U) - start_sector_index + 1U;

  if (number_sectors > 1)
  {
    length = MX25L4006_SECTOR_SIZE - (uDest % MX25L4006_SECTOR_SIZE);
  }

  for (sector_index = start_sector_index; sector_index < (start_sector_index + number_sectors); sector_index++)
  {
    sector_address = sector_index * MX25L4006_SECTOR_SIZE + FLASH_IF_EXT_FLASH_ADDRESS;
    if (FLASH_IF_EXT_IsEmpty(pDestination, length) != FLASH_IF_MEM_EMPTY)
    {
      if (pAllocatedBuffer == NULL)
      {
        ret_status = FLASH_IF_PARAM_ERROR;
        break; /* exit for loop */
      }

      /* backup initial Flash page data in RAM area */
      FLASH_IF_EXT_Read(pAllocatedBuffer, (const void *)sector_address, MX25L4006_SECTOR_SIZE);
      /* copy fragment into RAM area */
      UTIL_MEM_cpy_8(&pAllocatedBuffer[uDest % MX25L4006_SECTOR_SIZE], (const void *)uSource, length);

      /*  erase the Flash sector, to avoid writing twice in RAM */
      if (FLASH_IF_EXT_Erase((void *)sector_address, MX25L4006_SECTOR_SIZE) != FLASH_IF_OK)
      {
        ret_status = FLASH_IF_ERASE_ERROR;
        break; /* exit for loop */
      }

      /* copy the whole flash sector including fragment from RAM to Flash */
      current_dest = sector_address;
      current_source = (uint32_t)pAllocatedBuffer;
      current_length = MX25L4006_SECTOR_SIZE;
    }
    else
    {
      /* write a part of flash page from selected source data */
      current_dest = uDest;
      current_source = uSource;
      current_length = length;
    }

    if (BSP_FLASH_Write(FLASH_IF_EXT_INSTANCE, (uint8_t *)current_source, current_dest - FLASH_IF_EXT_FLASH_ADDRESS,
                        current_length) != BSP_ERROR_NONE)
    {
      /* Error occurred while writing data in Flash memory */
      ret_status = FLASH_IF_WRITE_ERROR;
      break;
    }

    if (ret_status != FLASH_IF_OK)
    {
      /* Error occurred while writing data in Flash memory */
      break;
    }

    /* Increment FLASH destination address, source address, and decrease remaining length */
    uDest += length;
    uSource += length;
    length = ((uLength - length) > MX25L4006_SECTOR_SIZE) ? MX25L4006_SECTOR_SIZE : uLength - length;
  }
  /* USER CODE BEGIN FLASH_IF_EXT_Write_2 */

  /* USER CODE END FLASH_IF_EXT_Write_2 */
  return ret_status;
}

static FLASH_IF_StatusTypedef FLASH_IF_EXT_Read(void *pDestination, const void *pSource, uint32_t uLength)
{
  FLASH_IF_StatusTypedef ret_status = FLASH_IF_OK;
  /* USER CODE BEGIN FLASH_IF_EXT_Read_1 */

  /* USER CODE END FLASH_IF_EXT_Read_1 */
  if ((pDestination == NULL) || (pSource == NULL))
  {
    return FLASH_IF_PARAM_ERROR;
  }

  if (BSP_FLASH_Read(FLASH_IF_EXT_INSTANCE, (uint8_t *) pDestination, (uint32_t) pSource - FLASH_IF_EXT_FLASH_ADDRESS,
                     uLength) != BSP_ERROR_NONE)
  {
    ret_status = FLASH_IF_READ_ERROR;
  }
  /* USER CODE BEGIN FLASH_IF_EXT_Read_2 */

  /* USER CODE END FLASH_IF_EXT_Read_2 */
  return ret_status;
}

static FLASH_IF_StatusTypedef FLASH_IF_EXT_Erase(void *pStart, uint32_t uLength)
{
  FLASH_IF_StatusTypedef ret_status = FLASH_IF_OK;
  /* USER CODE BEGIN FLASH_IF_EXT_Erase_1 */

  /* USER CODE END FLASH_IF_EXT_Erase_1 */
  uint32_t uStart = (uint32_t)pStart;
  uint32_t index = 0;
  uint32_t sector_start = SECTOR_INDEX(uStart);
  uint32_t number_sectors = SECTOR_INDEX(uStart + uLength - 1U) - sector_start + 1U;

  if (pStart == NULL)
  {
    return FLASH_IF_PARAM_ERROR;
  }

  /* Loop on 4 KBytes block */
  for (index = sector_start ; index < (sector_start + number_sectors); index++)
  {
    if (BSP_FLASH_Erase_Sector(FLASH_IF_EXT_INSTANCE, index * MX25L4006_SECTOR_SIZE) != BSP_ERROR_NONE)
    {
      ret_status = FLASH_IF_ERASE_ERROR;
      break;
    }
  }
  /* USER CODE BEGIN FLASH_IF_EXT_Erase_2 */

  /* USER CODE END FLASH_IF_EXT_Erase_2 */
  return ret_status;
}

static int32_t FLASH_IF_EXT_IsEmpty(void *pStart, uint32_t uLength)
{
  int32_t status = FLASH_IF_MEM_EMPTY;
  /* USER CODE BEGIN FLASH_IF_EXT_IsEmpty_1 */

  /* USER CODE END FLASH_IF_EXT_IsEmpty_1 */
  uint32_t index;
  uint64_t buffer;
  for (index = 0; index < uLength; index += 8)
  {
    if (FLASH_IF_EXT_Read((void *)&buffer, pStart, 8) != FLASH_IF_OK)
    {
      status = FLASH_IF_ERROR;
      break;
    }
    if (buffer != UINT64_MAX)
    {
      status = FLASH_IF_MEM_NOT_EMPTY;
      break;
    }
    pStart = (void *)((uint32_t)pStart + 8U);
  }
  /* USER CODE BEGIN FLASH_IF_EXT_IsEmpty_2 */

  /* USER CODE END FLASH_IF_EXT_IsEmpty_2 */
  return status;
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/* HAL overload functions ---------------------------------------------------------*/
/**
  * @note This function overwrites the __weak one from HAL
  */
void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
  /* USER CODE BEGIN HAL_FLASH_EndOfOperationCallback_1 */

  /* USER CODE END HAL_FLASH_EndOfOperationCallback_1 */
  if (ReturnValue == 0xFFFFFFFFUL)
  {
    /* Call when all requested pages have been erased */
  }
  /* USER CODE BEGIN HAL_FLASH_EndOfOperationCallback_2 */

  /* USER CODE END HAL_FLASH_EndOfOperationCallback_2 */
}

/**
  * @note This function overwrites the __weak one from HAL
  */
void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue)
{
  /* USER CODE BEGIN HAL_FLASH_OperationErrorCallback_1 */

  /* USER CODE END HAL_FLASH_OperationErrorCallback_1 */
}

/* USER CODE BEGIN Overload_HAL_weaks */

/* USER CODE END Overload_HAL_weaks */
