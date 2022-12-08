/**
  ******************************************************************************
  * @file    mx25l4006.c
  * @Author  MCD Application Team
  * @brief   This file provides the MX25L4006 drivers.
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
  */

/* Includes ------------------------------------------------------------------*/
#include "mx25l4006.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @defgroup MX25L4006 MX25L4006
  * @{
  */
#define MX25L4006_SPI_TIMEOUT 1000U /* 1000ms */


/** @defgroup MX25L4006_Exported_Functions MX25L4006 Exported Functions
  * @{
  */

/**
  * @brief  Get Flash information
  * @param  pInfo pointer to Device Info structure
  * @retval Status
  *    - MX25L4006_OK
  */
int32_t MX25L4006_GetFlashInfo(MX25L4006_Info_t *pInfo)
{
  /* Configure the structure with the memory configuration */
  pInfo->FlashSize              = MX25L4006_FLASH_SIZE;
  pInfo->EraseSectorSize        = MX25L4006_BLOCK_64K;
  pInfo->EraseSectorsNumber     = (MX25L4006_FLASH_SIZE / MX25L4006_BLOCK_64K);
  pInfo->EraseSubSectorSize     = MX25L4006_SECTOR_4K;
  pInfo->EraseSubSectorNumber   = (MX25L4006_FLASH_SIZE / MX25L4006_SECTOR_4K);
  pInfo->EraseSubSector1Size    = MX25L4006_SECTOR_4K;
  pInfo->EraseSubSector1Number  = (MX25L4006_FLASH_SIZE / MX25L4006_SECTOR_4K);
  pInfo->ProgPageSize           = MX25L4006_PAGE_SIZE;
  pInfo->ProgPagesNumber        = (MX25L4006_FLASH_SIZE / MX25L4006_PAGE_SIZE);

  return MX25L4006_OK;
}

/**
  * @brief  Wait until Write In Progress (WIP) bit is equal to 0
  * @param  Ctx Component object pointer
  * @retval Status
  *    - MX25L4006_ERROR_AUTOPOLLING
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_OK
  */
int32_t MX25L4006_AutoPollingMemReady(SPI_HandleTypeDef *Ctx)
{
  int32_t ret = MX25L4006_OK;
  uint8_t statusRegister;
  uint8_t cmd = MX25L4006_READ_STATUS_REG_CMD;

  /* Send the command */
  if (HAL_SPI_Transmit(Ctx, &cmd, 1U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }
  else
  {
    do
    {
      if (HAL_SPI_Receive(Ctx, &statusRegister, 1U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
      {
        ret = MX25L4006_ERROR_AUTOPOLLING;
      }
    } while ((ret == MX25L4006_OK) && ((statusRegister & MX25L4006_SR_WIP) != 0U));
  }

  return ret;
}

/**
  * @brief  Wait until Write Enable Latch (WEL) bit is set to 1
  * @param  Component object pointer
  * @retval Status
  *    - MX25L4006_ERROR_RECEIVE
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_OK
  */
int32_t MX25L4006_AutoPollingMemReadyToWrite(SPI_HandleTypeDef *Ctx)
{
  int32_t ret = MX25L4006_OK;
  uint8_t statusRegister;
  uint8_t cmd = MX25L4006_READ_STATUS_REG_CMD;

  /* Send the command */
  if (HAL_SPI_Transmit(Ctx, &cmd, 1U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }
  else
  {
    do
    {
      if (HAL_SPI_Receive(Ctx, &statusRegister, 1U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
      {
        ret = MX25L4006_ERROR_RECEIVE;
      }
    } while ((ret == MX25L4006_OK) && ((statusRegister & MX25L4006_SR_WREN) != MX25L4006_SR_WREN));
  }

  return ret;
}

/* Read/Write Array Commands (3 Byte Address Command Set) *********************/
/**
  * @brief  Reads an amount of data from the memory.
  * @param  Ctx Component object pointer
  * @param  pData Pointer to data to be read
  * @param  ReadAddr Read start address
  * @param  Size Size of data to read in Byte
  * @retval Status
  *    - MX25L4006_ERROR_RECEIVE
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_OK
  */
int32_t MX25L4006_Read(SPI_HandleTypeDef *Ctx, uint8_t *pData, uint32_t ReadAddr, uint16_t Size)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd[4U];

  /* Send the command */
  cmd[0U] = MX25L4006_READ_CMD;
  cmd[1U] = (uint8_t)((ReadAddr & 0x00FF0000U) >> 16);
  cmd[2U] = (uint8_t)((ReadAddr & 0x0000FF00U) >> 8);
  cmd[3U] = (uint8_t)(ReadAddr & 0x000000FFU);

  if (HAL_SPI_Transmit(Ctx, cmd, 4U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }
  else
  {
    if (HAL_SPI_Receive(Ctx, pData, Size, MX25L4006_SPI_TIMEOUT) != HAL_OK)
    {
      ret = MX25L4006_ERROR_RECEIVE;
    }
  }

  return ret;
}

/**
  * @brief  Reads an amount of data from the memory.
  * @param  Ctx Component object pointer
  * @param  pData Pointer to data to be read
  * @param  ReadAddr Read start address
  * @param  Size Size of data to read in Byte
  * @retval Status
  *    - MX25L4006_ERROR_RECEIVE
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_OK
  */
int32_t MX25L4006_FastRead(SPI_HandleTypeDef *Ctx, uint8_t *pData, uint32_t ReadAddr, uint16_t Size)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd[5U];

  /* Send the command */
  cmd[0U] = MX25L4006_FAST_READ_CMD;
  cmd[1U] = (uint8_t)((ReadAddr & 0x00FF0000U) >> 16);
  cmd[2U] = (uint8_t)((ReadAddr & 0x0000FF00U) >> 8);
  cmd[3U] = (uint8_t)(ReadAddr & 0x000000FFU);
  cmd[4U] = 0xAAU; /* Dummy */

  if (HAL_SPI_Transmit(Ctx, cmd, 5U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }
  else
  {
    if (HAL_SPI_Receive(Ctx, pData, Size, MX25L4006_SPI_TIMEOUT) != HAL_OK)
    {
      ret = MX25L4006_ERROR_RECEIVE;
    }
  }

  return ret;
}

/**
  * @brief  Writes an amount of data to the SPI memory.
  *    For 256 bytes page program, the 8 least significant address bits byte
  *    should be set to 0 this function otherwise returns MX25L4006_ERROR_ADDRESS
  * @param  Ctx Component object pointer
  * @param  pData Pointer to data to be written
  * @param  WriteAddr Write start address
  * @param  Size Size of data to write. Range 1 ~ 256
  * @retval Status
  *    - MX25L4006_ERROR_ADDRESS
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_ERROR_TRANSMIT
  *    - MX25L4006_OK
  */
int32_t MX25L4006_PageProgram(SPI_HandleTypeDef *Ctx, uint8_t *pData, uint32_t WriteAddr, uint16_t Size)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd[4U];

  if ((Size >= MX25L4006_PAGE_SIZE) && ((WriteAddr & 0x000000FFU) != 0U))
  {
    ret = MX25L4006_ERROR_ADDRESS;
  }
  else
  {
    /* 1- Send Page Program (PP) command */
    cmd[0U] = MX25L4006_PAGE_PROG_CMD;
    cmd[1U] = (uint8_t)((WriteAddr & 0x00FF0000U) >> 16);
    cmd[2U] = (uint8_t)((WriteAddr & 0x0000FF00U) >> 8);
    cmd[3U] = (uint8_t)(WriteAddr & 0x000000FFU);
    if (HAL_SPI_Transmit(Ctx, cmd, 4U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
    {
      ret = MX25L4006_ERROR_COMMAND;
    }
    else
    {
      /* 2- Send the data */
      if (HAL_SPI_Transmit(Ctx, pData, Size, MX25L4006_SPI_TIMEOUT) != HAL_OK)
      {
        ret = MX25L4006_ERROR_TRANSMIT;
      }
    }
  }

  return ret;
}

/**
  * @brief  Erases the specified sector of the SPI memory
  *         MX25L4006 support 4K size block erase command.
  * @param  Ctx Component object pointer
  * @param  SectorAddress Sector address to erase
  * @retval Status
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_OK
  */
int32_t MX25L4006_SectorErase(SPI_HandleTypeDef *Ctx, uint32_t SectorAddress)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd[4U];

  /* 1- Send Sector Erase (SE) for erasing the data of chosen block */
  cmd[0U] = MX25L4006_SECTOR_ERASE_4K_CMD;
  cmd[1U] = (uint8_t)((SectorAddress & 0x00FF0000U) >> 16);
  cmd[2U] = (uint8_t)((SectorAddress & 0x0000FF00U) >> 8);
  cmd[3U] = (uint8_t)(SectorAddress & 0x000000FFU);
  if (HAL_SPI_Transmit(Ctx, cmd, 4U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    return MX25L4006_ERROR_COMMAND;
  }

  return ret;
}

/**
  * @brief  Erases the specified block of the SPI memory
  *         MX25L4006 64K size block erase command.
  * @param  Ctx Component object pointer
  * @param  BlockAddress Block address to erase
  * @param  BlockSize Block size to erase
  * @retval Status
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_OK
  */
int32_t MX25L4006_BlockErase(SPI_HandleTypeDef *Ctx, uint32_t BlockAddress, MX25L4006_Erase_t BlockSize)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd[4U];

  /* Setup erase command */
  switch (BlockSize)
  {
    default :
    case MX25L4006_ERASE_4K :
      cmd[0U] = MX25L4006_SECTOR_ERASE_4K_CMD;
      break;

    case MX25L4006_ERASE_64K :
      cmd[0U] = MX25L4006_BLOCK_ERASE_64K_CMD;
      break;

    case MX25L4006_ERASE_CHIP :
      return MX25L4006_ChipErase(Ctx);
      break;
  }

  /* 1- Send Block Erase (BE) for erasing the data of chosen block */
  cmd[1U] = (uint8_t)((BlockAddress & 0x00FF0000U) >> 16);
  cmd[2U] = (uint8_t)((BlockAddress & 0x0000FF00U) >> 8);
  cmd[3U] = (uint8_t)(BlockAddress & 0x000000FFU);
  if (HAL_SPI_Transmit(Ctx, cmd, 4U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }

  return ret;
}

/**
  * @brief  Whole chip erase of the SPI memory
  * @param  Ctx Component object pointer
  * @retval Status
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_OK
  */
int32_t MX25L4006_ChipErase(SPI_HandleTypeDef *Ctx)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd = MX25L4006_CHIP_ERASE_CMD;

  /* 1- Send Chip Erase (CE) command to erase whole chip */
  if (HAL_SPI_Transmit(Ctx, &cmd, 1U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }

  return ret;
}

/* Register/Setting Commands **************************************************/
/**
  * @brief  This function sets the (WEL) Write Enable Latch bit
  * @param  Ctx Component object pointer
  * @retval Status
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_OK
  */
int32_t MX25L4006_WriteEnable(SPI_HandleTypeDef *Ctx)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd = MX25L4006_WRITE_ENABLE_CMD;

  /* Send Write Enable (WREN) command to set Write Enable Latch (WEL) bit in the Status Register */
  if (HAL_SPI_Transmit(Ctx, &cmd, 1U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }

  return ret;
}

/**
  * @brief  This function resets the (WEL) Write Enable Latch bit
  * @param  Ctx Component object pointer
  * @retval Status
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_OK
  */
int32_t MX25L4006_WriteDisable(SPI_HandleTypeDef *Ctx)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd = MX25L4006_WRITE_DISABLE_CMD;

  /* Send Write Disable (WRDI) command to unset Write Enable Latch (WEL) bit in the Status Register */
  if (HAL_SPI_Transmit(Ctx, &cmd, 1U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    return MX25L4006_ERROR_COMMAND;
  }

  return ret;
}

/**
  * @brief  Read Flash Status register
  * @param  Ctx Component object pointer
  * @param  Value pointer to status register value
  * @retval Status
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_ERROR_RECEIVE
  *    - MX25L4006_OK
  */
int32_t MX25L4006_ReadStatusRegister(SPI_HandleTypeDef *Ctx, uint8_t *Value)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd = MX25L4006_READ_STATUS_REG_CMD;

  /* Send the command */
  if (HAL_SPI_Transmit(Ctx, &cmd, 1U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }
  else
  {
    if (HAL_SPI_Receive(Ctx, Value, 1U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
    {
      ret = MX25L4006_ERROR_RECEIVE;
    }
  }

  return ret;
}

/**
  * @brief  Write Flash Status register value
  * @param  Ctx Component object pointer
  * @param  Value Status register value
  * @retval Status
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_OK
  */
int32_t MX25L4006_WriteStatusRegister(SPI_HandleTypeDef *Ctx, uint8_t Value)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd[2U];

  /* Send the command */
  cmd[0U] = MX25L4006_WRITE_STATUS_REG_CMD;
  cmd[1U] = Value;
  if (HAL_SPI_Transmit(Ctx, cmd, 2U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }

  return ret;
}

/**
  * @brief  Deep power down
  *         The device is not active and all Write/Program/Erase instruction are ignored.
  * @param  Ctx Component object pointer
  * @retval Status
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_OK
  */
int32_t MX25L4006_EnterDeepPowerDown(SPI_HandleTypeDef *Ctx)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd = MX25L4006_DEEP_POWER_DOWN_CMD;

  /* Send Deep Powerdown (DP) command to enter deep powerdown mode */
  if (HAL_SPI_Transmit(Ctx, &cmd, 1U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }

  return ret;
}

/**
  * @brief  Release Deep power down
  *         The device is now active and all Write/Program/Erase instruction are available.
  * @param  Ctx Component object pointer
  * @retval Status
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_OK
  */
int32_t MX25L4006_ReleaseDeepPowerDown(SPI_HandleTypeDef *Ctx)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd = MX25L4006_RELEASE_FROM_DEEP_POWER_DOWN_CMD;

  /* Send Release from Deep Powerdown (RDP) command to exit from deep powerdown mode */
  if (HAL_SPI_Transmit(Ctx, &cmd, 1U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }

  return ret;
}

/* ID/Security Commands *******************************************************/
/**
  * @brief  Read Flash 3 Byte IDs
  *         Manufacturer ID, Memory type, Memory density
  * @param  Ctx Component object pointer
  * @param  ID pointer to flash id value
  * @retval Status
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_ERROR_RECEIVE
  *    - MX25L4006_OK
  */
int32_t MX25L4006_ReadID(SPI_HandleTypeDef *Ctx, uint8_t *ID)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd = MX25L4006_READ_ID_CMD;

  /* Send the command */
  if (HAL_SPI_Transmit(Ctx, &cmd, 1U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }
  else
  {
    if (HAL_SPI_Receive(Ctx, ID, 3U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
    {
      ret = MX25L4006_ERROR_RECEIVE;
    }
  }

  return ret;
}

/**
  * @brief  Reads an amount of data from the memory
  * @param  Ctx Component object pointer
  * @param  pData Pointer to data to be read
  * @param  ReadAddr Read start address
  * @param  Size Size of data to read in Byte
  * @retval Status
  *    - MX25L4006_ERROR_COMMAND
  *    - MX25L4006_ERROR_RECEIVE
  *    - MX25L4006_OK
  */
int32_t MX25L4006_ReadSFDP(SPI_HandleTypeDef *Ctx, uint8_t *pData, uint32_t ReadAddr, uint16_t Size)
{
  int32_t ret = MX25L4006_OK;
  uint8_t cmd[5U];

  /* Send the command */
  cmd[0U] = MX25L4006_READ_SERIAL_FLASH_DISCO_PARAM_CMD;
  cmd[1U] = (uint8_t)((ReadAddr & 0x00FF0000U) >> 16);
  cmd[2U] = (uint8_t)(ReadAddr & 0x0000FF00U) >> 8;
  cmd[3U] = (uint8_t)(ReadAddr & 0x000000FFU);
  cmd[4U] = 0xAAU; /* Dummy */
  if (HAL_SPI_Transmit(Ctx, cmd, 5U, MX25L4006_SPI_TIMEOUT) != HAL_OK)
  {
    ret = MX25L4006_ERROR_COMMAND;
  }
  else
  {
    if (HAL_SPI_Receive(Ctx, pData, Size, MX25L4006_SPI_TIMEOUT) != HAL_OK)
    {
      ret = MX25L4006_ERROR_RECEIVE;
    }
  }

  return ret;
}
