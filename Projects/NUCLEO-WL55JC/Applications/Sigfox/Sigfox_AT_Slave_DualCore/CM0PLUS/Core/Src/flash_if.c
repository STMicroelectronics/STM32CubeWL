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
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
#include "stm32_mem.h"

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
  FLASH_EMPTY      = 0,
  FLASH_NOT_EMPTY  = 1
};

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  This function writes a data buffer in flash (data are 64-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  pDestination: Start address for target location
  * @param  pSource: pointer on buffer with data to write
  * @param  uLength: Length of data buffer in byte. It has to be 64-bit aligned.
  * @retval HAL Status.
  */
static int32_t FLASH_IF_Write_Buffer(uint32_t pDestination, uint8_t *pSource, uint32_t uLength);

/**
  * @brief  This function checks if part of Flash is empty
            It handles 32b unaligned address
  * @param  addr: Start of user flash area
  * @param  size: number of bytes.
  * @retval FLASH_EMPTY or FLASH_NOT_EMPTY.
  */
static int32_t FLASH_IF_IsEmpty(uint8_t *addr, uint32_t size);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int32_t FLASH_IF_Write(uint32_t address, uint8_t *data, uint32_t size, uint8_t *dataTempPage)
{
  /* USER CODE BEGIN FLASH_IF_Write_1 */

  /* USER CODE END FLASH_IF_Write_1 */
  int32_t status = FLASH_OK;
  uint32_t page_start_index = PAGE(address);
  uint32_t page_end_index = PAGE(address + size - 1);
  uint32_t curr_size = size;
  uint32_t curr_dest_addr = address;
  uint32_t curr_src_addr = (uint32_t)data;

  if ((data == NULL) || ((size % sizeof(uint64_t)) != 0) || ((address % sizeof(uint64_t)) != 0))
  {
    return FLASH_PARAM_ERROR;
  }

  if (READ_BIT(FLASH->CR, FLASH_CR_LOCK) != 0U)
  {
    return FLASH_LOCK_ERROR;
  }

  if (page_start_index != page_end_index)
  {
    curr_size = FLASH_PAGE_SIZE - (address % FLASH_PAGE_SIZE);
  }

  for (uint32_t idx = page_start_index; idx <= page_end_index; idx++)
  {
    if (FLASH_IF_IsEmpty((uint8_t *)curr_dest_addr, curr_size) != FLASH_EMPTY)
    {
      if (dataTempPage == NULL)
      {
        return FLASH_PARAM_ERROR;
      }
      /* backup initial Flash page data in RAM area */
      UTIL_MEM_cpy_8(dataTempPage, (uint8_t *)(idx * FLASH_PAGE_SIZE + FLASH_BASE), FLASH_PAGE_SIZE);
      /* copy fragment into RAM area */
      UTIL_MEM_cpy_8(&dataTempPage[((uint32_t)curr_dest_addr) % FLASH_PAGE_SIZE], (uint8_t *)curr_src_addr, curr_size);

      /*  erase the Flash sector, to avoid writing twice in RAM */
      if (FLASH_IF_EraseByPages(idx, 1, 0) != FLASH_OK)
      {
        status = FLASH_ERASE_ERROR;
        break; /* exit for loop */
      }
      else
      {
        /* copy the whole flash sector including fragment from RAM to Flash*/
        if (FLASH_IF_Write_Buffer(idx * FLASH_PAGE_SIZE + FLASH_BASE, dataTempPage, FLASH_PAGE_SIZE) != FLASH_OK)
        {
          status = FLASH_WRITE_ERROR;
          break; /* exit for loop */
        }
      }
    }
    else
    {
      if (FLASH_IF_Write_Buffer(curr_dest_addr, (uint8_t *)curr_src_addr, curr_size) != FLASH_OK)
      {
        status = FLASH_WRITE_ERROR;
        break; /* exit for loop */
      }
    }

    /* 2nd part of memory overlapped on 2nd flash sector */
    curr_dest_addr += curr_size;
    curr_src_addr += curr_size;
    curr_size = size - curr_size;
  }

  return status;
  /* USER CODE BEGIN FLASH_IF_Write_2 */

  /* USER CODE END FLASH_IF_Write_2 */
}

int32_t FLASH_IF_Write64(uint32_t address, uint64_t data)
{
  /* USER CODE BEGIN FLASH_IF_Write64_1 */

  /* USER CODE END FLASH_IF_Write64_1 */
  while (*(uint64_t *)address != data)
  {
    while (LL_FLASH_IsActiveFlag_OperationSuspended());
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, data);
  }

  return FLASH_OK;
  /* USER CODE BEGIN HW_FLASH_Write_2 */

  /* USER CODE END HW_FLASH_Write_2 */
}

int32_t FLASH_IF_EraseByPages(uint32_t page, uint16_t n, int32_t interrupt)
{
  /* USER CODE BEGIN FLASH_IF_EraseByPages_1 */

  /* USER CODE END FLASH_IF_EraseByPages_1 */
  HAL_StatusTypeDef hal_status;
  FLASH_EraseInitTypeDef erase_str;
  uint32_t page_error;

  erase_str.TypeErase = FLASH_TYPEERASE_PAGES;
  erase_str.Page = page;
  erase_str.NbPages = n;

  /* Erase the Page */
  if (interrupt)
  {
    hal_status = HAL_FLASHEx_Erase_IT(&erase_str);
  }
  else
  {
    hal_status = HAL_FLASHEx_Erase(&erase_str, &page_error);
  }

  return ((hal_status == HAL_OK) ? FLASH_OK : ((hal_status == HAL_BUSY) ? FLASH_BUSY : FLASH_ERASE_ERROR));
  /* USER CODE BEGIN FLASH_IF_EraseByPages_2 */

  /* USER CODE END FLASH_IF_EraseByPages_2 */
}

void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
  /* USER CODE BEGIN HAL_FLASH_EndOfOperationCallback_1 */

  /* USER CODE END HAL_FLASH_EndOfOperationCallback_1 */
  /* Call CleanUp callback when all requested pages have been erased */
  if (ReturnValue == 0xFFFFFFFFUL)
  {
    HWCB_FLASH_EndOfCleanup();
  }
  /* USER CODE BEGIN HAL_FLASH_EndOfOperationCallback_2 */

  /* USER CODE END HAL_FLASH_EndOfOperationCallback_2 */
}

void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue)
{
  /* USER CODE BEGIN HAL_FLASH_OperationErrorCallback_1 */

  /* USER CODE END HAL_FLASH_OperationErrorCallback_1 */
}

void HWCB_FLASH_EndOfCleanup(void)
{
  /* USER CODE BEGIN HWCB_FLASH_EndOfCleanup_1 */

  /* USER CODE END HWCB_FLASH_EndOfCleanup_1 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
static int32_t FLASH_IF_Write_Buffer(uint32_t pDestination, uint8_t *pSource, uint32_t uLength)
{
  /* USER CODE BEGIN FLASH_IF_Write_Buffer_1 */

  /* USER CODE END FLASH_IF_Write_Buffer_1 */
  uint8_t *pSrc = pSource;
  uint64_t src_value;
  int32_t status = FLASH_OK;

  for (uint32_t i = 0; i < (uLength / sizeof(uint64_t)); i++)
  {
    UTIL_MEM_cpy_8(&src_value, pSrc, sizeof(uint64_t));

    /* Avoid writing 0xFFFFFFFFFFFFFFFFLL on erased Flash */
    if (src_value != UINT64_MAX)
    {
      status = FLASH_IF_Write64(pDestination, src_value);
    }

    pDestination += sizeof(uint64_t);
    pSrc += sizeof(uint64_t);

    if (status != FLASH_OK)
    {
      /* exit the for loop*/
      break;
    }
  }

  return status;
  /* USER CODE BEGIN FLASH_IF_Write_Buffer_2 */

  /* USER CODE END FLASH_IF_Write_Buffer_2 */
}

static int32_t FLASH_IF_IsEmpty(uint8_t *addr, uint32_t size)
{
  /* USER CODE BEGIN FLASH_IF_IsEmpty_1 */

  /* USER CODE END FLASH_IF_IsEmpty_1 */
  uint64_t *addr64;
  uint32_t i;

  /* start memory NOT 64bits aligned */
  while ((((uint32_t)addr) % sizeof(uint64_t)) != 0)
  {
    if (*addr++ != UINT8_MAX)
    {
      return FLASH_NOT_EMPTY;
    }
    size--;
  }

  /* addr64 is 64 bits aligned */
  addr64 = (uint64_t *)addr;
  for (i = 0; i < (size / sizeof(uint64_t)); i++)
  {
    if (*addr64++ != UINT64_MAX)
    {
      return FLASH_NOT_EMPTY;
    }
  }
  size -= sizeof(uint64_t) * i;

  /* end memory NOT 64 bits aligned */
  addr = (uint8_t *)addr64;
  while (size != 0)
  {
    if (*addr++ != UINT8_MAX)
    {
      return FLASH_NOT_EMPTY;
    }
    size--;
  }
  return FLASH_EMPTY;
  /* USER CODE BEGIN FLASH_IF_IsEmpty_2 */

  /* USER CODE END FLASH_IF_IsEmpty_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE***/
