/**
  ******************************************************************************
  * @file    b_wl5m_subg_flash.c
  * @author  MCD Application Team
  * @brief   This file includes a standard driver for the M25L4006 CMOS SERIAL
  *          FLASH memory mounted on B-WL5M-SUBG board.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @verbatim
  ==============================================================================
                     ##### How to use this driver #####
  ==============================================================================
  [..]
   (#) This driver is used to drive the M25L4006 CMOS SERIAL flash external
       memory mounted on B-WL5M-SUBG board.

   (#) This driver does not need a specific component driver for the NOR device
       to be included with.

   (#) Initialization steps:
       (++) Initialize the NOR external memory using the BSP_FLASH_Init() function. This
            function includes the MSP layer hardware resources initialization and the
            FMC controller configuration to interface with the external NOR memory.

   (#) NOR flash operations
       (++) NOR external memory can be accessed with read/write operations once it is
            initialized.
            Read/write operation can be performed with SPI access using the functions
            BSP_FLASH_Read()/BSP_FLASH_FastRead()/BSP_FLASH_Write().
            The BSP_FLASH_Write() performs write operation of an amount of data
            by unit (byte).
            To write more than 255 bytes, the 8 least significant address bits should be
            set to 0.
       (++) The function BSP_FLASH_ReadID() returns the chip IDs.
            (see the NOR IDs in the memory data sheet)
       (++) The function BSP_FLASH_ReadSFDP() returns the parameter value from JEDEC table
            (see the NOR JEDEC table in the memory data sheet)
       (++) Perform erase sector operation using the function BSP_FLASH_Erase_Sector() and by
            specifying the sector address.
       (++) Perform erase block operation using the function BSP_FLASH_Erase_Block() and by
            specifying the block address.
       (++) Perform erase of the whole chip by calling the function BSP_FLASH_Erase_Chip().

  @endverbatim
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "b_wl5m_subg_flash.h"
#include <string.h>
#include <stdio.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup B_WL5M_SUBG
  * @{
  */

/** @defgroup B_WL5M_SUBG_SPI_FLASH B-WL5M-SUBG SPI FLASH
  * @{
  */

/* Exported variables --------------------------------------------------------*/
/** @addtogroup B_WL5M_SUBG_SPI_FLASH_Exported_Variables
  * @{
  */
SPI_HandleTypeDef hspi_flash[SPI_FLASH_INSTANCES_NUMBER] = {0};
SPI_FLASH_Ctx_t Spi_Flash_Ctx[SPI_FLASH_INSTANCES_NUMBER] = {SPI_ACCESS_NONE};

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup B_WL5M_SUBG_SPI_FLASH_Private_Variables B-WL5M-SUBG SPI FLASH Private Variables
  * @{
  */

/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/

/** @defgroup B_WL5M_SUBG_SPI_FLASH_Private_Functions B-WL5M-SUBG SPI FLASH Private Functions
  * @{
  */
static void    SPI_FLASH_MspInit(SPI_HandleTypeDef *hspi);
static void    SPI_FLASH_MspDeInit(SPI_HandleTypeDef *hspi);
static int32_t SPI_FLASH_ResetMemory(uint32_t Instance);

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/

/** @addtogroup B_WL5M_SUBG_FLASH_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  Initializes the FLASH device.
  * @param  Instance SPI Instance
  * @retval BSP status
  */
int32_t BSP_FLASH_Init(uint32_t Instance)
{
  int32_t ret;
  BSP_SPI_FLASH_Info_t pInfo;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check if the instance is already initialized */
    if (Spi_Flash_Ctx[Instance].IsInitialized == SPI_ACCESS_NONE)
    {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
      /* Msp OSPI initialization */
      SPI_FLASH_MspInit(&hspi_flash[Instance]);
#else
      /* Register the OSPI MSP Callbacks */
      if (OspiNor_IsMspCbValid[Instance] == 0UL)
      {
        if (BSP_OSPI_NOR_RegisterDefaultMspCallbacks(Instance) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_PERIPH_FAILURE;
        }
      }
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */

      /* Get Flash information of one memory */
      (void)MX25L4006_GetFlashInfo(&pInfo);

      /* STM32 SPI interface initialization */
      if (MX_SPI_FLASH_Init(&hspi_flash[Instance]) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      /* SPI memory reset */
      else if (SPI_FLASH_ResetMemory(Instance) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      /* Check if memory is ready */
      else if (MX25L4006_AutoPollingMemReady(&hspi_flash[Instance]) != MX25L4006_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  DeInitialization the flash device
  * @param  Instance SPI Instance
  * @retval BSP status
  */
int32_t BSP_FLASH_DeInit(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check if the instance is already initialized */
    if (Spi_Flash_Ctx[Instance].IsInitialized != SPI_ACCESS_NONE)
    {
      /* Set default Spi_Flash_Ctx values */
      Spi_Flash_Ctx[Instance].IsInitialized = SPI_ACCESS_NONE;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
      SPI_FLASH_MspDeInit(&hspi_flash[Instance]);
#endif /* (USE_HAL_SPI_REGISTER_CALLBACKS == 0) */

      /* Call the DeInit function to reset the driver */
      if (HAL_SPI_DeInit(&hspi_flash[Instance]) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Read an amount of data from the External FLash device
  * @param  Instance SPI Instance
  * @param  pData Pointer to data to be read
  * @param  StartAddress Read start address
  * @param  DataSize Size of data to read
  * @retval BSP status
  */
int32_t BSP_FLASH_Read(uint32_t Instance, uint8_t *pData, uint32_t StartAddress, uint16_t DataSize)
{
  int32_t ret;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* 1- Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2- Send the command to read the flash */
    if (MX25L4006_Read(&hspi_flash[Instance], pData, StartAddress, DataSize) != MX25L4006_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }

    /* 3- Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Quickly read an amount of data from the External FLash device
  *         While Program/Erase/Write Status Register cycle is in progress,
  *         Fast Read will be rejected without any impact on the current cycle.
  * @param  Instance SPI Instance
  * @param  pData Pointer to data to be read
  * @param  StartAddress Read start address
  * @param  DataSize Size of data to read
  * @retval BSP status
  */
int32_t BSP_FLASH_FastRead(uint32_t Instance, uint8_t *pData, uint32_t StartAddress, uint16_t DataSize)
{
  int32_t ret;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* 1- Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2- Send the command to read the flash */
    if (MX25L4006_FastRead(&hspi_flash[Instance], pData, StartAddress, DataSize) != MX25L4006_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }

    /* 3- Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Write an amount of data to the Flash device
  * @param  Instance SPI Instance
  * @param  pData Pointer to data to be written
  * @param  WriteAddr Write start address
  * @param  DataSize Size of data to write
  * @retval BSP status
  */
int32_t BSP_FLASH_Write(uint32_t Instance, uint8_t *pData, uint32_t WriteAddr, uint16_t DataSize)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t end_addr, current_addr;
  uint32_t data_addr;
  uint16_t current_size;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Calculation of the size between the write address and the end of the page */
    current_size = MX25L4006_PAGE_SIZE - (WriteAddr % MX25L4006_PAGE_SIZE);

    /* Check if the size of the data is less than the remaining place in the page */
    if (current_size > DataSize)
    {
      current_size = DataSize;
    }

    /* Initialize the address variables */
    current_addr = WriteAddr;
    end_addr = WriteAddr + DataSize;
    data_addr = (uint32_t)pData;

    /* Check if Flash busy ? */
    /* 1- Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2- AutoPolling until Memory is ready */
    if (MX25L4006_AutoPollingMemReady(&hspi_flash[Instance]) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }

    /* 3- Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */

    /* Perform the write page by page */
    while ((current_addr < end_addr) && (ret == BSP_ERROR_NONE))
    {
      /* Enable write operations */
      /* 1 - Send Write Enable (WREN) command to set Write Enable Latch (WEL) bit
             before sending the command Block Erase (BE) */
      /* 1.1 - Select the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

      /* 1.2 - Send the command */
      if (MX25L4006_WriteEnable(&hspi_flash[Instance]) != MX25L4006_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        /* 1.3 - Unselect the Chip to terminate the WriteEnable action */
        HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
        HAL_Delay(1); /* Delay to ensure a nice rising edge */

        /* 2 - Check WEL flag is set to 1 */
        /* 2.1 - Select the Chip */
        HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

        /* 2.2 - Send the command to read the Status Register */
        if (MX25L4006_AutoPollingMemReadyToWrite(&hspi_flash[Instance]) != MX25L4006_OK)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
      }

      /* 1.3 or 2.3 - Unselect the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
      HAL_Delay(1); /* Delay to ensure a nice rising edge */

      if (ret == BSP_ERROR_NONE)
      {
        /* 3 - Send Page Program (PP) command */
        /* 3.1 - Select the Chip */
        HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

        /* 3.2 - Send the command */
        if (MX25L4006_PageProgram(&hspi_flash[Instance], (uint8_t *)data_addr, current_addr, current_size) != MX25L4006_OK)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }

        /* 3.3 - Unselect the Chip */
        HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
        HAL_Delay(1); /* Delay to ensure a nice rising edge */

        /* 4.1 - Select the Chip */
        HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

        /* 4.2 - Send the command to read the Status Register */
        if (MX25L4006_AutoPollingMemReady(&hspi_flash[Instance]) != MX25L4006_OK)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }

        /* 4.3 - Unselect the Chip */
        HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
        HAL_Delay(1); /* Delay to ensure a nice rising edge */

        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        data_addr += current_size;
        current_size = ((current_addr + MX25L4006_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : MX25L4006_PAGE_SIZE;
      }
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Erase the specified sector of the FLASH device
  * @param  Instance SPI Instance
  * @param  SectorAddress Sector address to erase
  * @retval BSP status
  */
int32_t BSP_FLASH_Erase_Sector(uint32_t Instance, uint32_t SectorAddress)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* 1 - Send Write Enable (WREN) command to set Write Enable Latch (WEL) bit
         before sending the command Sector Erase (SE) */
    /* 1.1 - Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 1.2 - Send Write Enable (WREN) command */
    if (MX25L4006_WriteEnable(&hspi_flash[Instance]) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }

    /* 1.3 - Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */

    /* 2 - Wait until WEL bit is set to 1 */
    /* 2.1 - Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2.2 - Send the command to read the Status Register */
    if (MX25L4006_AutoPollingMemReadyToWrite(&hspi_flash[Instance]) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }

    /* 2.3 - Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */

    if (ret == BSP_ERROR_NONE)
    {
      /* 3 - Send Sector Erase (SE) command */
      /* 3.1 - Select the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

      /* 3.2 - Send the command */
      if (MX25L4006_SectorErase(&hspi_flash[Instance], SectorAddress) != MX25L4006_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }

      /* 3.3 - Unselect the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
      HAL_Delay(1); /* Delay to ensure a nice rising edge */

      /* 4 - Wait Until WIP flag (Write In Progress) is set to 0 */
      /* 4.1 - Select the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

      /* 4.2 - Send the command to read the Status Register */
      if (MX25L4006_AutoPollingMemReady(&hspi_flash[Instance]) != MX25L4006_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }

      /* 4.3 - Unselect the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
      HAL_Delay(1); /* Delay to ensure a nice rising edge */
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Erase the specified block of the FLASH device
  * @param  Instance SPI Instance
  * @param  BlockAddress Block address to erase
  * @param  BlockSize Size of block to erase
  *         This parameter can be one of following parameters:
  *             @arg MX25L4006_ERASE_4K
  *             @arg MX25L4006_ERASE_64K
  *             @arg MX25L4006_ERASE_CHIP
  * @retval BSP status
  */
int32_t BSP_FLASH_Erase_Block(uint32_t Instance, uint32_t BlockAddress, BSP_SPI_FLASH_Erase_t BlockSize)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* 1 - Send Write Enable (WREN) command to set Write Enable Latch (WEL) bit
         before sending the command Sector Erase (SE) */
    /* 1.1 - Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 1.2 - Send Write Enable (WREN) command */
    if (MX25L4006_WriteEnable(&hspi_flash[Instance]) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }

    /* 1.3 - Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */

    /* 2 - Wait until WEL bit is set to 1 */
    /* 2.1 - Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2.2 - Send the command to read the Status Register */
    if (MX25L4006_AutoPollingMemReadyToWrite(&hspi_flash[Instance]) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }

    /* 2.3 - Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */

    if (ret == BSP_ERROR_NONE)
    {
      /* 3 - Send Sector Erase (SE) command */
      /* 3.1 - Select the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

      /* 3.2 - Send the command */
      if (MX25L4006_BlockErase(&hspi_flash[Instance], BlockAddress, BlockSize) != MX25L4006_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }

      /* 3.3 - Unselect the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
      HAL_Delay(1); /* Delay to ensure a nice rising edge */

      /* 4 - Wait Until WIP flag (Write In Progress) is set to 0 */
      /* 4.1 - Select the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

      /* 4.2 - Send the command to read the Status Register */
      if (MX25L4006_AutoPollingMemReady(&hspi_flash[Instance]) != MX25L4006_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }

      /* 4.3 - Unselect the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
      HAL_Delay(1); /* Delay to ensure a nice rising edge */
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Erase the entire FLASH chip
  * @param  Instance SPI Instance
  * @retval BSP status
  */
int32_t BSP_FLASH_Erase_Chip(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* 1 - Send Write Enable (WREN) command to set Write Enable Latch (WEL) bit
         before sending the command Sector Erase (SE) */
    /* 1.1 - Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 1.2 - Send Write Enable (WREN) command */
    if (MX25L4006_WriteEnable(&hspi_flash[Instance]) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }

    /* 1.3 - Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */

    /* 2 - Wait until WEL bit is set to 1 */
    /* 2.1 - Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2.2 - Send the command to read the Status Register */
    if (MX25L4006_AutoPollingMemReadyToWrite(&hspi_flash[Instance]) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }

    /* 2.3 - Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */

    if (ret == BSP_ERROR_NONE)
    {
      /* 3 - Send Sector Erase (SE) command */
      /* 3.1 - Select the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

      /* 3.2 - Send the command */
      if (MX25L4006_ChipErase(&hspi_flash[Instance]) != MX25L4006_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }

      /* 3.3 - Unselect the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
      HAL_Delay(1); /* Delay to ensure a nice rising edge */

      /* 4 - Wait Until WIP flag (Write In Progress) is set to 0 */
      /* 4.1 - Select the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

      /* 4.2 - Send the command to read the Status Register */
      if (MX25L4006_AutoPollingMemReady(&hspi_flash[Instance]) != MX25L4006_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }

      /* 4.3 - Unselect the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
      HAL_Delay(1); /* Delay to ensure a nice rising edge */
    }
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Reads current status of the SPI memory.
  * @param  Instance SPI instance
  * @retval SPI memory status: whether busy or not
  */
int32_t BSP_FLASH_GetStatus(uint32_t Instance)
{
  static uint8_t reg;
  int32_t ret;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* 1- Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2- Send the command */
    if (MX25L4006_ReadStatusRegister(&hspi_flash[Instance], &reg) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }/* Check the value of the register */
    else if ((reg & MX25L4006_SR_WIP) != 0U)
    {
      ret = BSP_ERROR_BUSY;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }

    /* 3- Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Return the configuration of the SPI memory.
  * @param  Instance SPI instance
  * @param  pInfo    pointer on the configuration structure
  * @retval BSP status
  */
int32_t BSP_FLASH_GetInfo(uint32_t Instance, BSP_SPI_FLASH_Info_t *pInfo)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    (void)MX25L4006_GetFlashInfo(pInfo);
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Reads flash IDs.
  * @param  Instance SPI Instance
  * @param  ID Pointer to Device IDs
  * @retval BSP status
  */
int32_t BSP_FLASH_ReadID(uint32_t Instance, uint8_t *ID)
{
  int32_t ret;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* 1- Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2- Send the 1st command to read the Manufacturer IDs */
    if (MX25L4006_ReadID(&hspi_flash[Instance], ID) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }

    /* 3- Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */
  }

  return ret;
}

/**
  * @brief  Read parameter value from SFDP table
  * @param  Instance SPI Instance
  * @param  pData Pointer to data to be read
  * @param  Address Parameter address in SFDP table
  * @param  DataSize Size of data to read
  * @retval BSP status
  */
int32_t BSP_FLASH_ReadSFDP(uint32_t Instance, uint8_t *pData, uint32_t Address, uint16_t DataSize)
{
  int32_t ret;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* 1- Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2- Send the 1st command to read the Manufacturer IDs */
    if (MX25L4006_ReadSFDP(&hspi_flash[Instance], pData, Address, DataSize) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }

    /* 3- Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */
  }

  return ret;
}

/**
  * @brief  Erase the specified block of the FLASH device
  * @param  Instance SPI Instance
  * @param  Status pointer to status register value
  * @retval BSP status
  */
int32_t BSP_FLASH_ReadStatusRegister(uint32_t Instance, uint8_t *Status)
{
  int32_t ret;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* 1- Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2- Send the 1st command to read the Manufacturer IDs */
    if (MX25L4006_ReadStatusRegister(&hspi_flash[Instance], Status) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }

    /* 3- Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */
  }

  return ret;
}

/**
  * @brief  This function configures the Block Protection
  * @param  Instance SPI instance
  * @param  level Level of protection (See MX25L4406 Datasheet)
  * @retval BSP status
  */
int32_t BSP_FLASH_BlockProtectConfig(uint32_t Instance, uint8_t level)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* 1 - Send Write Enable (WREN) command to set Write Enable Latch (WEL) bit
         before configure the Status Register */
    /* 1.1 - Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 1.2 - Send Write Enable (WREN) command */
    if (MX25L4006_WriteEnable(&hspi_flash[Instance]) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }

    /* 1.3 - Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */

    /* 2 - Wait until WEL bit is set to 1 */
    /* 2.1 - Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2.2 - Send the command to read the Status Register */
    if (MX25L4006_AutoPollingMemReadyToWrite(&hspi_flash[Instance]) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }

    /* 2.3 - Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */

    if (ret == BSP_ERROR_NONE)
    {
      /* 1- Select the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

      /* 2- Send the command to write to the Status Register */
      if (MX25L4006_WriteStatusRegister(&hspi_flash[Instance], level & MX25L4006_SR_BLOCKPR) != MX25L4006_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }

      /* 3- Unselect the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
      HAL_Delay(1); /* Delay to ensure a nice rising edge */

      /* 4 - Wait Until WIP flag (Write In Progress) is set to 0 */
      /* 4.1 - Select the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

      /* 4.2 - Send the command to read the Status Register */
      if (MX25L4006_AutoPollingMemReady(&hspi_flash[Instance]) != MX25L4006_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }

      /* 4.3 - Unselect the Chip */
      HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
      HAL_Delay(1); /* Delay to ensure a nice rising edge */
    }
  }

  return ret;
}

/**
  * @brief  This function enters the SPI memory in deep power down mode.
  * @param  Instance  SPI instance
  * @retval BSP status
  */
int32_t BSP_FLASH_EnterDeepPowerDown(uint32_t Instance)
{
  int32_t ret;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* 1- Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2- Send the command */
    if (MX25L4006_EnterDeepPowerDown(&hspi_flash[Instance]) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }

    /* 3- Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */
  }

  /* ---          Memory takes 10us max to enter deep power down          --- */

  /* Return BSP status */
  return ret;
}

/**
  * @brief  This function leaves the SPI memory from deep power down mode.
  * @param  Instance SPI instance
  * @retval BSP status
  */
int32_t BSP_FLASH_LeaveDeepPowerDown(uint32_t Instance)
{
  int32_t ret;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* 1- Select the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_RESET);

    /* 2- Send the command */
    if (MX25L4006_ReleaseDeepPowerDown(&hspi_flash[Instance]) != MX25L4006_OK)
    {
      ret = BSP_ERROR_COMPONENT_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }

    /* 3- Unselect the Chip */
    HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);
    HAL_Delay(1); /* Delay to ensure a nice rising edge */
  }

  /* --- A NOP command is sent to the memory, as the nCS should be low for at least 20 ns --- */
  /* ---                  Memory takes 30us min to leave deep power down                  --- */

  /* Return BSP status */
  return ret;
}

/**
  * @brief  Initializes the SPI interface.
  * @param  hspi          SPI handle
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_SPI_FLASH_Init(SPI_HandleTypeDef *hspi)
{
  /* SPI initialization */
  hspi->Instance = SPI2;
  hspi->Init.Mode = SPI_MODE_MASTER;
  hspi->Init.Direction = SPI_DIRECTION_2LINES;
  hspi->Init.DataSize = SPI_DATASIZE_8BIT;
  hspi->Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi->Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi->Init.NSS = SPI_NSS_SOFT;
  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi->Init.TIMode = SPI_TIMODE_DISABLE;
  hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial = 7;
  hspi->Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;

  return HAL_SPI_Init(hspi);
}

/**
  * @}
  */

/** @addtogroup B_WL5M_SUBG_SPI_FLASH_Private_Functions
  * @{
  */
/**
  * @brief  This function reset the SPI memory.
  * @param  Instance SPI instance
  * @retval BSP status
  */
static int32_t SPI_FLASH_ResetMemory(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= SPI_FLASH_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (BSP_FLASH_LeaveDeepPowerDown(Instance) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    Spi_Flash_Ctx[Instance].IsInitialized = SPI_ACCESS;
  }

  /* Return BSP status */
  return ret;
}

/**
  * @brief SPI MSP Initialization
  *    This function configures the hardware resources used for SPI
  * @param hspi SPI handle pointer
  * @retval None
  */
void SPI_FLASH_MspInit(SPI_HandleTypeDef *hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  __HAL_RCC_SPI2_CLK_ENABLE();

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /** SPI2 GPIO Configuration
      PC2     ------> SPI2_MISO
      PA10    ------> SPI2_MOSI
      PB13    ------> SPI2_SCK
      PB6     ------> CS (Chip Select)
      PB7     ------> Hold
    **/
  /* Flash SPI MISO Pin configuration */
  GPIO_InitStruct.Pin = FLASH_SPI_MISO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(FLASH_SPI_MISO_PORT, &GPIO_InitStruct);

  /* Flash SPI MOSI Pin configuration */
  GPIO_InitStruct.Pin = FLASH_SPI_MOSI_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(FLASH_SPI_MOSI_PORT, &GPIO_InitStruct);

  /* Flash SPI Clock Pin configuration */
  GPIO_InitStruct.Pin = FLASH_SPI_CLK_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(FLASH_SPI_CLK_PORT, &GPIO_InitStruct);

  /* Flash Chip Select Pin configuration */
  GPIO_InitStruct.Pin = FLASH_CHIP_SELECT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(FLASH_CHIP_SELECT_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN, GPIO_PIN_SET);

  /* Flash Hold Pin configuration */
  GPIO_InitStruct.Pin = FLASH_HOLD_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(FLASH_HOLD_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(FLASH_HOLD_PORT, FLASH_HOLD_PIN, GPIO_PIN_SET);
}

/**
  * @brief  De-Initializes the SPI MSP.
  * @param  hspi SPI handle
  * @retval None
  */
static void SPI_FLASH_MspDeInit(SPI_HandleTypeDef *hspi)
{
  if (hspi->Instance == SPI2)
  {
    /* SPI GPIO pins de-configuration  */
    HAL_GPIO_DeInit(FLASH_HOLD_PORT, FLASH_HOLD_PIN);
    HAL_GPIO_DeInit(FLASH_CHIP_SELECT_PORT, FLASH_CHIP_SELECT_PIN);
    HAL_GPIO_DeInit(FLASH_SPI_CLK_PORT, FLASH_SPI_CLK_PIN);
    HAL_GPIO_DeInit(FLASH_SPI_MOSI_PORT, FLASH_SPI_MOSI_PIN);
    HAL_GPIO_DeInit(FLASH_SPI_MISO_PORT, FLASH_SPI_MISO_PIN);

    /* Disable the SPI memory interface clock */
    __HAL_RCC_SPI2_CLK_DISABLE();
  }
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

/**
  * @}
  */
