/**
  ******************************************************************************
  * @file    mx25l4006.h
  * @modify  MCD Application Team
  * @brief   This file contains all the description of the
  *          MX25L4006 memory.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MX25L4006_H
#define MX25L4006_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "mx25l4006_conf.h"


/** @defgroup MX25L4006_Exported_Constants MX25L4006 Exported Constants
  * @{
  */
/* Device ID & Secure OTP length *********************************************/
#define MX25L4006_MANUFACTURER_ID             ((uint8_t)0xC2)
#define MX25L4006_DEVICE_MEM_TYPE             ((uint8_t)0x20)
#define MX25L4006_DEVICE_MEM_CAPACITY         ((uint8_t)0x1A)
#define MX25L4006_SECURE_ID                   ((uint8_t)0x12)

#define MX25L4006_BLOCK_64K                   (uint32_t) (64 * 1024)        /*!< 8 blocks of 64KBytes */
#define MX25L4006_BLOCK_32K                   (uint32_t) (32 * 1024)        /*!< 16 blocks of 32KBytes */
#define MX25L4006_SECTOR_4K                   (uint32_t) (4  * 1024)        /*!< 4096 sectors of 4KBytes */

#define MX25L4006_FLASH_SIZE                  (uint32_t) (4*1024*1024/8)    /*!< 4 MBits => 512KBytes */
#define MX25L4006_SECTOR_SIZE                 MX25L4006_SECTOR_4K
#define MX25L4006_BLOCK_SIZE                  MX25L4006_BLOCK_64K
#define MX25L4006_PAGE_SIZE                   256U                          /*!< 2048 pages of 256 Bytes */

/**
  * @brief  MX25L4006 Timing configuration
  */

#define MX25L4006_CHIP_ERASE_MAX_TIME                  4000U /* time in millisecond to erase the full flash */
#define MX25L4006_BLOCK_ERASE_MAX_TIME                 2000U /* time in millisecond to erase a block of 64K */
#define MX25L4006_SECTOR_ERASE_MAX_TIME                200U  /* time in millisecond to erase a sector of 4K */
#define MX25L4006_WRITE_REG_MAX_TIME                   40U   /* time in millisecond to write to Status Register */
#define MX25L4006_PAGE_PROGRAM_MAX_TIME                3U    /* time in millisecond to write a page of 256 bytes */

/* MX25L4006 Component Error codes *********************************************/
#define MX25L4006_OK                           0
#define MX25L4006_ERROR_INIT                  -1
#define MX25L4006_ERROR_COMMAND               -2
#define MX25L4006_ERROR_TRANSMIT              -3
#define MX25L4006_ERROR_RECEIVE               -4
#define MX25L4006_ERROR_AUTOPOLLING           -5
#define MX25L4006_ERROR_MEMORYMAPPED          -6
#define MX25L4006_ERROR_ADDRESS               -7

/******************************************************************************
  * @brief  MX25L4006 Commands
  ****************************************************************************/
/***** Read/Write Array Commands (3 Byte Address Command Set) **************/
/* Read Operations */
#define MX25L4006_READ_CMD                             0x03U   /*!< READ, Normal Read 3 Byte Address; SPI 1-1-1                    */
#define MX25L4006_FAST_READ_CMD                        0x0BU   /*!< FAST READ, Fast Read 3 Byte Address; SPI 1-1-1                 */

/* Program Operations */
#define MX25L4006_PAGE_PROG_CMD                        0x02U   /*!< PP, Page Program 3 Byte Address; SPI 1-1-1           */

/* Erase Operations */
#define MX25L4006_SECTOR_ERASE_4K_CMD                  0x20U   /*!< SE, Sector Erase 4KB 3 Byte Address; SPI 1-1-0       */
#define MX25L4006_BLOCK_ERASE_64K_CMD                  0xD8U   /*!< BE, Block Erase 64KB 3 Byte Address; SPI 1-1-0       */
#define MX25L4006_CHIP_ERASE_CMD                       0xC7U   /*!< CE, Chip Erase 0 Byte Address; SPI 1-0-0             */

/***** Register/Setting Commands *********************************************/
#define MX25L4006_WRITE_ENABLE_CMD                     0x06U   /*!< WREN, Write Enable; SPI                        */
#define MX25L4006_WRITE_DISABLE_CMD                    0x04U   /*!< WRDI, Write Disable; SPI                       */

#define MX25L4006_READ_STATUS_REG_CMD                  0x05U   /*!< RDSR, Read Status Register; SPI                */
#define MX25L4006_WRITE_STATUS_REG_CMD                 0x01U   /*!< WRSR, Write Status Register; SPI               */

#define MX25L4006_DEEP_POWER_DOWN_CMD                  0xB9U   /*!< DP, Deep power down;               SPI */
#define MX25L4006_RELEASE_FROM_DEEP_POWER_DOWN_CMD     0xABU   /*!< RDP, Release from Deep Power down; SPI */

/***** ID/Security Commands **************************************************/
/* Identification Operations */
#define MX25L4006_READ_ID_CMD                          0x9FU   /*!< RDID, Read IDentification; SPI                         */
#define MX25L4006_READ_ELECTRONIC_ID_CMD               0xABU   /*!< RES, Read Electronic ID; SPI                           */
#define MX25L4006_READ_ELECTRONIC_MANFACTURER_ID_CMD   0x90U   /*!< REMS, Read Electronic Manufacturer ID & Device ID; SPI */

#define MX25L4006_READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5AU   /*!< RDSFDP, Read Serial Flash Discoverable Parameter; SPI  */

#define MX25L4006_DOUBLE_OUTPUT_MODE_CMD               0x3BU   /*!< DREAD, Double Output Mode; SPI                         */

/******************************************************************************
  * @brief  MX25L4006 Registers
  ****************************************************************************/
/* Status Register */
#define MX25L4006_SR_WIP                      ((uint8_t)0x01)    /*!< Write in progress */
#define MX25L4006_SR_WREN                     ((uint8_t)0x02)    /*!< Write enable latch */
#define MX25L4006_SR_BLOCKPR                  ((uint8_t)0x1C)    /*!< Block protected against program and erase operations */
#define MX25L4006_SR_SRWD                     ((uint8_t)0x80)    /*!< Status register write enable/disable */


/**
  * @}
  */

/** @defgroup MX25L4006_Exported_Types MX25L4006 Exported Types
  * @{
  */

typedef struct
{
  uint32_t FlashSize;                        /*!< Size of the flash                             */
  uint32_t EraseSectorSize;                  /*!< Size of sectors for the erase operation       */
  uint32_t EraseSectorsNumber;               /*!< Number of sectors for the erase operation     */
  uint32_t EraseSubSectorSize;               /*!< Size of subsector for the erase operation     */
  uint32_t EraseSubSectorNumber;             /*!< Number of subsector for the erase operation   */
  uint32_t EraseSubSector1Size;              /*!< Size of subsector 1 for the erase operation   */
  uint32_t EraseSubSector1Number;            /*!< Number of subsector 1 for the erase operation */
  uint32_t ProgPageSize;                     /*!< Size of pages for the program operation       */
  uint32_t ProgPagesNumber;                  /*!< Number of pages for the program operation     */
} MX25L4006_Info_t;

typedef enum
{
  MX25L4006_ERASE_4K = 0,                 /*!< 4K size Sector erase */
  MX25L4006_ERASE_64K,                    /*!< 64K size Block erase */
  MX25L4006_ERASE_CHIP                    /*!< Whole chip erase     */
} MX25L4006_Erase_t;

/**
  * @}
  */

/** @defgroup MX25L4006_Exported_Functions MX25L4006 Exported Functions
  * @{
  */
/* Function by commands combined */
int32_t MX25L4006_GetFlashInfo(MX25L4006_Info_t *pInfo);
int32_t MX25L4006_AutoPollingMemReady(SPI_HandleTypeDef *Ctx);
int32_t MX25L4006_AutoPollingMemReadyToWrite(SPI_HandleTypeDef *Ctx);

/* Read/Write Array Commands (3 Bytes Address Command Set) ********************/
int32_t MX25L4006_Read(SPI_HandleTypeDef *Ctx, uint8_t *pData, uint32_t ReadAddr, uint16_t Size);
int32_t MX25L4006_FastRead(SPI_HandleTypeDef *Ctx, uint8_t *pData, uint32_t ReadAddr, uint16_t Size);
int32_t MX25L4006_PageProgram(SPI_HandleTypeDef *Ctx, uint8_t *pData, uint32_t WriteAddr, uint16_t Size);
int32_t MX25L4006_SectorErase(SPI_HandleTypeDef *Ctx, uint32_t SectorAddress);
int32_t MX25L4006_BlockErase(SPI_HandleTypeDef *Ctx, uint32_t BlockAddress, MX25L4006_Erase_t BlockSize);
int32_t MX25L4006_ChipErase(SPI_HandleTypeDef *Ctx);

/* Register/Setting Commands *************************************************/
int32_t MX25L4006_WriteEnable(SPI_HandleTypeDef *Ctx);
int32_t MX25L4006_WriteDisable(SPI_HandleTypeDef *Ctx);
int32_t MX25L4006_ReadStatusRegister(SPI_HandleTypeDef *Ctx, uint8_t *Value);
int32_t MX25L4006_WriteStatusRegister(SPI_HandleTypeDef *Ctx, uint8_t Value);
int32_t MX25L4006_EnterDeepPowerDown(SPI_HandleTypeDef *Ctx);
int32_t MX25L4006_ReleaseDeepPowerDown(SPI_HandleTypeDef *Ctx);

/* ID/Security Commands ******************************************************/
int32_t MX25L4006_ReadID(SPI_HandleTypeDef *Ctx, uint8_t *ID);
int32_t MX25L4006_ReadSFDP(SPI_HandleTypeDef *Ctx, uint8_t *pData, uint32_t ReadAddr, uint16_t Size);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MX25L4006_H */
