/**
  ******************************************************************************
  * @file    flash_if.c
  * @author  MCD Application Team
  * @brief   FLASH Interface module.
  *          This file provides set of firmware functions to manage Flash
  *          Interface functionalities.
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

/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup USER_APP_COMMON Common
  * @{
  */
/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
#include "string.h"
#include <stdio.h>

/* Uncomment the line below if you want some debug logs */
#define FLASH_IF_DBG
#ifdef FLASH_IF_DBG
#define FLASH_IF_TRACE printf
#else
#define FLASH_IF_TRACE(...)
#endif /* FLASH_IF_DBG */


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NB_PAGE_SECTOR_PER_ERASE  2U    /*!< Nb page erased per erase */
#define EXTERNAL_FLASH_ADDRESS    0x90000000U


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
static uint32_t GetPage(uint32_t uAddr);
static HAL_StatusTypeDef FLASH_INT_If_Clear_Error(void);

/* Public functions : wrapper ---------------------------------------------------------*/

/**
  * @brief  Initialize internal and external flash interface (OSPI/QSPI)
  * @param  none
  * @retval HAL status.
  */
HAL_StatusTypeDef FLASH_If_Init(void)
{
  HAL_StatusTypeDef e_ret_status = HAL_OK;

  e_ret_status = FLASH_INT_Init();
  if (e_ret_status == HAL_OK)
  {
    e_ret_status = FLASH_EXT_Init();
  }
  return e_ret_status;
}
/**
  * @brief  Depending on start address, this function will call internal or external (OSPI/QSPI) flash driver
  * @param  pStart: flash address to be erased
  * @param  uLength: number of bytes
  * @retval HAL status.
  */
HAL_StatusTypeDef FLASH_If_Erase_Size(void *pStart, uint32_t uLength)
{
  /* Check Flash start address */
  if ((uint32_t) pStart < EXTERNAL_FLASH_ADDRESS)
  {
    return FLASH_INT_If_Erase_Size(pStart, uLength);
  }
  else
  {
    return FLASH_EXT_If_Erase_Size(pStart, uLength);
  }
}

/**
  * @brief  Depending on destination address, this function will call internal or external (OSPI/QSPI) flash driver
  * @param  pDestination: flash address to write
  * @param  pSource: pointer on buffer with data to write
  * @param  uLength: number of bytes
  * @retval HAL Status.
  */
HAL_StatusTypeDef FLASH_If_Write(void *pDestination, const void *pSource, uint32_t uLength)
{
  /* Check Flash destination address */
  if ((uint32_t) pDestination < EXTERNAL_FLASH_ADDRESS)
  {
    return FLASH_INT_If_Write(pDestination, pSource, uLength);
  }
  else
  {
    return FLASH_EXT_If_Write(pDestination, pSource, uLength);
  }
}

/**
  * @brief  Depending on destination address, this function will call internal or external (OSPI/QSPI) flash driver
  * @brief  Depending on source address, this function will call internal or external (OSPI/QSPI) flash driver
  * @param  pDestination: pointer on buffer to store data
  * @param  pSource: flash address to read
  * @param  uLength: number of bytes
  * @retval HAL Status.
  */
HAL_StatusTypeDef FLASH_If_Read(void *pDestination, const void *pSource, uint32_t uLength)
{
  /* Check Flash source address */
  if ((uint32_t) pSource < EXTERNAL_FLASH_ADDRESS)
  {
    return FLASH_INT_If_Read(pDestination, pSource, uLength);
  }
  else
  {
    return FLASH_EXT_If_Read(pDestination, pSource, uLength);
  }
}

/* Public functions : internal flash --------------------------------------------------------- */
/**
  * @brief  This function initialize the internal flash interface if required
  * @param  none
  * @retval HAL status.
  */
HAL_StatusTypeDef FLASH_INT_Init(void)
{
  return HAL_OK;
}

/**
  * @brief  This function does an erase of n (depends on Length) pages in user flash area
  * @param  pStart: Start of user flash area
  * @param  uLength: number of bytes.
  * @retval HAL status.
  */
HAL_StatusTypeDef FLASH_INT_If_Erase_Size(void *pStart, uint32_t uLength)
{
  uint32_t page_error = 0U;
  uint32_t uStart = (uint32_t)pStart;
  FLASH_EraseInitTypeDef x_erase_init;
  HAL_StatusTypeDef e_ret_status = HAL_ERROR;
  uint32_t first_page = 0U;
  uint32_t nb_pages = 0U;
  uint32_t chunk_nb_pages;

  /* Clear error flags raised during previous operation */
  e_ret_status = FLASH_INT_If_Clear_Error();

  if (e_ret_status == HAL_OK)
  {
    /* Unlock the Flash to enable the flash control register access *************/
    if (HAL_FLASH_Unlock() == HAL_OK)
    {
      first_page = GetPage(uStart);
      /* Get the number of pages to erase from 1st page */
      nb_pages = GetPage(uStart + uLength - 1U) - first_page + 1U;
      x_erase_init.TypeErase   = FLASH_TYPEERASE_PAGES;
      /* Erase flash per NB_PAGE_SECTOR_PER_ERASE to avoid watch-dog */
      do
      {
        chunk_nb_pages = (nb_pages >= NB_PAGE_SECTOR_PER_ERASE) ? NB_PAGE_SECTOR_PER_ERASE : nb_pages;
        x_erase_init.Page = first_page;
        x_erase_init.NbPages = chunk_nb_pages;
        first_page += chunk_nb_pages;
        nb_pages -= chunk_nb_pages;
        if (HAL_FLASHEx_Erase(&x_erase_init, &page_error) != HAL_OK)
        {
          HAL_FLASH_GetError();
          e_ret_status = HAL_ERROR;
        }
        /* Refresh Watchdog */
        WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);
      } while (nb_pages > 0);
      /* Lock the Flash to disable the flash control register access (recommended
      to protect the FLASH memory against possible unwanted operation) *********/
      HAL_FLASH_Lock();

    }
    else
    {
      e_ret_status = HAL_ERROR;
    }
  }

  return e_ret_status;
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  pDestination: Start address for target location. It has to be 8 bytes aligned.
  * @param  pSource: pointer on buffer with data to write
  * @param  uLength: Length of data buffer in bytes. It has to be 8 bytes aligned.
  * @retval HAL Status.
  */
HAL_StatusTypeDef FLASH_INT_If_Write(void *pDestination, const void *pSource, uint32_t uLength)
{
  HAL_StatusTypeDef e_ret_status = HAL_ERROR;
  uint32_t i = 0U;
  uint32_t pdata = (uint32_t)pSource;

  /* Clear error flags raised during previous operation */
  e_ret_status = FLASH_INT_If_Clear_Error();

  if (e_ret_status == HAL_OK)
  {
    /* Unlock the Flash to enable the flash control register access *************/
    if (HAL_FLASH_Unlock() != HAL_OK)
    {
      return HAL_ERROR;

    }
    else
    {
      /* DataLength must be a multiple of 64 bit */
      for (i = 0U; i < uLength; i += 8U)
      {
        /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
        be done by word */
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)pDestination,  *((uint64_t *)(pdata + i)))
            == HAL_OK)
        {
          /* Check the written value */
          if (*(uint64_t *)pDestination != *(uint64_t *)(pdata + i))
          {
            /* Flash content doesn't match SRAM content */
            e_ret_status = HAL_ERROR;
            break;
          }
          /* Increment FLASH Destination address */
          pDestination = (void *)((uint32_t)pDestination + 8U);
        }
        else
        {
          /* Error occurred while writing data in Flash memory */
          e_ret_status = HAL_ERROR;
          break;
        }
      }
      /* Lock the Flash to disable the flash control register access (recommended
      to protect the FLASH memory against possible unwanted operation) *********/
      HAL_FLASH_Lock();
    }
  }
  return e_ret_status;
}

/**
  * @brief  This function reads flash
  * @param  pDestination: Start address for target location
  * @param  pSource: flash address to read
  * @param  uLength: number of bytes
  * @retval HAL Status.
  */
HAL_StatusTypeDef FLASH_INT_If_Read(void *pDestination, const void *pSource, uint32_t uLength)
{
  memcpy(pDestination, pSource, uLength);
  return HAL_OK;
}

/* Public functions : external flash ---------------------------------------------------------*/

HAL_StatusTypeDef FLASH_EXT_Init(void)
{
  return HAL_OK;
}

/* No external flash available on this product
   ==> return SFU_ERROR */

HAL_StatusTypeDef FLASH_EXT_If_Erase_Size(void *pStart, uint32_t uLength)
{
  return HAL_ERROR;
}

HAL_StatusTypeDef FLASH_EXT_If_Write(void  *pDestination, const void *pSource, uint32_t uLength)
{
  return HAL_ERROR;
}

HAL_StatusTypeDef FLASH_EXT_If_Read(void *pDestination, const void *pSource, uint32_t uLength)
{
  return HAL_ERROR;
}

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Clear error flags raised during previous operation
  * @param  None
  * @retval HAL Status.
  */
HAL_StatusTypeDef FLASH_INT_If_Clear_Error(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;

  /* Unlock the Program memory */
  if (HAL_FLASH_Unlock() == HAL_OK)
  {

    /* Clear all FLASH flags */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
    /* Unlock the Program memory */
    if (HAL_FLASH_Lock() == HAL_OK)
    {
      ret = HAL_OK;
    }
#ifdef FLASH_IF_DBG
    else
    {
      FLASH_IF_TRACE("[FLASH_IF] Lock failure\r\n");
    }
#endif /* FLASH_IF_DBG */
  }
#ifdef FLASH_IF_DBG
  else
  {
    FLASH_IF_TRACE("[FLASH_IF] Unlock failure\r\n");
  }
#endif /* FLASH_IF_DBG */
  return ret;
}

/**
  * @brief  Gets the page of a given address
  * @param  Addr: Address of the FLASH Memory
  * @retval The page of a given address
  */
uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0U;

  page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;

  return page;
}


/**
  * @}
  */

/**
  * @}
  */
