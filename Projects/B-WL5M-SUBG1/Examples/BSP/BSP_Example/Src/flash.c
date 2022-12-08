/**
  ******************************************************************************
  * @file    flash.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the Flash supported by the
  *          B-WL5M-SUBG board
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
#include "main.h"
#include "b_wl5m_subg_bus.h"
#include "string.h"

/** @addtogroup B_WL5M_SUBG_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void printSector(uint8_t idxSector);
void printSector2(uint32_t address, uint16_t size);
void printBlock(uint8_t idxBlock);
int32_t checkSectorErased(uint32_t sectorAddress);
int32_t checkBlockErased(uint32_t blockAddress);

/**
  * @brief  demo of External Flash.
  */
void Flash_demo(void)
{
  /* Init the LEDs */
  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_GREEN);

  /* Initialize the External Flash */
  if (BSP_FLASH_Init(0) != BSP_ERROR_NONE)
  {
    printf("Error to initialize BSP Flash !!\n");
    return;
  }

  /* 1 - Read Identification (RDID) */
  uint8_t RDID[3] = {0U, 0U, 0U};
  printf("1 - Read Device IDs: ");
  if (BSP_FLASH_ReadID(0, RDID) != BSP_ERROR_NONE)
  {
    printf("Failed\n");
    BSP_LED_On(LED_RED);
  }
  else
  {
    printf("0x%02x | 0x%02x | 0x%02x ", RDID[0], RDID[1], RDID[2]);
    if ((RDID[0] != 0xC2) || (RDID[1] != 0x20) || (RDID[2] != 0x13))
    {
      printf("Failed");
      BSP_LED_On(LED_RED);
    }
    printf("\n");
  }
  
  /* 2 - Read SFDP parameters */
  uint8_t SFDP[4] = {0U, 0U, 0U, 0U};
  printf("2 - Read SFDP Parameters\n");
  printf("\t2.1 - Read SFDP Signature: ");
  if (BSP_FLASH_ReadSFDP(0, SFDP, 0x00, 4) != BSP_ERROR_NONE)
  {
    printf("BSP_FLASH_ReadSFDP error\n");
  }
  else
  {
    printf("\t%x%x%x%x\n", SFDP[3], SFDP[2], SFDP[1], SFDP[0]);
    if ((SFDP[3] != 0x50) || (SFDP[2] != 0x44) || (SFDP[1] != 0x46) || (SFDP[0] != 0x53))
    {
      printf("Failed\n");
      BSP_LED_On(LED_RED);
    }
  }
  
  printf("\t2.2 - Read SFDP revision: ");
  if (BSP_FLASH_ReadSFDP(0, SFDP, 0x04, 2) != BSP_ERROR_NONE)
  {
    printf("BSP_FLASH_ReadSFDP error\n");
  }
  else
  {
    printf("\tv%x.%x\n", SFDP[1], SFDP[0]);
    if ((SFDP[1] != 0x01) || (SFDP[0] != 0x00))
    {
      printf("Failed\n");
      BSP_LED_On(LED_RED);
    }
  }
  
  /* 3 - Read Status */
  printf("3 - Read Status");
  if (BSP_FLASH_GetStatus(0) == BSP_ERROR_NONE)
  {
    printf("\tGet Status is OK\n");
  }
  else
  {
    printf("\tGet Status error\n");
    BSP_LED_On(LED_RED);
  }
  
  /* 4 - Check Memory configuration */
  uint8_t statusRegister = 0xFF;
  printf("4 - Test to configure Memory Block Protection: ");
  printf("\t- Initial Status Register value: ");
  if (BSP_FLASH_ReadStatusRegister(0, &statusRegister) != BSP_ERROR_NONE)
  {
    printf("Failed\n");
    BSP_LED_On(LED_RED);
  }
  else
  {
    printf("0x%02x\n", statusRegister);
  }

  if (BSP_FLASH_GetStatus(0) == BSP_ERROR_NONE)
  {
    printf("Get Status is OK\n");
  }
  else
  {
    printf("Get Status error\n");
  }
  
  printf("\t- Tests all Block Protection configurations:\n");
  for (uint8_t i = 1; i < 8; i++)
  {
    uint8_t BPbits =  i << 2;
    printf("\t\t-> BP bits: 0x%02x", BPbits);
    if (BSP_FLASH_BlockProtectConfig(0U, BPbits) != BSP_ERROR_NONE)
    {
      printf("Failed\n");
      BSP_LED_On(LED_RED);
    }
    else
    {
      if (BSP_FLASH_ReadStatusRegister(0U, &statusRegister) != BSP_ERROR_NONE)
      {
        printf("Failed\n");
        BSP_LED_On(LED_RED);
      }
      else if ((statusRegister & MX25L4006_SR_BLOCKPR) == BPbits)
      {
        printf(" 0x%02x\n", statusRegister);
      }
      else
      {
        printf("Failed\n");
        BSP_LED_On(LED_RED);
      }
    }
  }

  /* Reset Block Protection bits */
  if (BSP_FLASH_BlockProtectConfig(0U, 0U) != BSP_ERROR_NONE)
  {
    printf("Failed to reset Block Protection Configuration !!\n");
    BSP_LED_On(LED_RED);
  }

  /* Erase Block */
  if (BSP_FLASH_Erase_Chip(0) != BSP_ERROR_NONE)
  {
    printf("Failed to erase whole chip !!\n");
    BSP_LED_On(LED_RED);
  }

  printf("5 - Read/Write to Memory\n");

  /* Write data in block 0 with granularity of 64B */
  uint8_t data[256] = {0};
  uint8_t data2[256] = {0};
  uint32_t sizeBlock = 256;

  for (uint32_t i = 0; i < sizeBlock; i++)
  {
    data[i] = (uint8_t)(0xFE - i);
  }

  uint32_t nbBlockPerSector = MX25L4006_SECTOR_SIZE / sizeBlock;
  for (uint32_t idSector = 0; idSector < 128; idSector++)
  {
    uint32_t sectorAddress = idSector * MX25L4006_SECTOR_SIZE;

    for (uint32_t idxBlock = 0; idxBlock < nbBlockPerSector; idxBlock++)
    {
      if (BSP_FLASH_Write(0, data, sectorAddress, sizeBlock) != BSP_ERROR_NONE)
      {
        printf("Write error at sector[0x%06x]\n", sectorAddress);
        BSP_LED_On(LED_RED);
      }

      sectorAddress += sizeBlock;
      BSP_LED_Toggle(LED_BLUE);
    }

    /* Read memory with classic function BSP_FLASH_Read */
    sectorAddress = idSector * MX25L4006_SECTOR_SIZE;

    for (uint32_t idxBlock = 0; idxBlock < nbBlockPerSector; idxBlock++)
    {
      memset(data2, 0xAA, sizeBlock);
      if (BSP_FLASH_Read(0, data2, sectorAddress, sizeBlock) != BSP_ERROR_NONE)
      {
        printf("Read error at sector[0x%06x]\n", sectorAddress);
        BSP_LED_On(LED_RED);
      }
      else
      {
        if (memcmp(data, data2, sizeBlock) != 0)
        {
          printf("Error diff at sector[0x%06x]\n", sectorAddress);
          BSP_LED_On(LED_RED);
        }
      }
      sectorAddress += sizeBlock;
      BSP_LED_Toggle(LED_GREEN);
    }
    
    /* Quickly Read memory with specific function BSP_FLASH_FastRead */
    sectorAddress = idSector * MX25L4006_SECTOR_SIZE;

    for (uint32_t idxBlock = 0; idxBlock < nbBlockPerSector; idxBlock++)
    {
      memset(data2, 0xAA, sizeBlock);
      if (BSP_FLASH_FastRead(0, data2, sectorAddress, sizeBlock) != BSP_ERROR_NONE)
      {
        printf("Read error at sector[0x%06x]\n", sectorAddress);
        BSP_LED_On(LED_RED);
      }
      else
      {
        if (memcmp(data, data2, sizeBlock) != 0)
        {
          printf("Error diff at sector[0x%06x]\n", sectorAddress);
          BSP_LED_On(LED_RED);
        }
      }
      sectorAddress += sizeBlock;
      BSP_LED_Toggle(LED_GREEN);
    }
  }

  printf("6 - Checking Erase sector at 0x000000\n");
  if (BSP_FLASH_Erase_Sector(0, 0x000000) != BSP_ERROR_NONE)
  {
    printf("failed\n");
    BSP_LED_On(LED_RED);
  }

  if (checkSectorErased(0) != 0)
  {
    BSP_LED_On(LED_RED);
  }

  printf("7 - Checking Erase block at 0x01CA32\n");
  uint32_t blockAddress = 0x01CA32;
  if (BSP_FLASH_Erase_Block(0, blockAddress, MX25L4006_ERASE_64K) != BSP_ERROR_NONE)
  {
    printf("failed\n");
    BSP_LED_On(LED_RED);
  }

  if (checkBlockErased(blockAddress & ~0xFFFF) != 0)
  {
    printf("failed\n");
    BSP_LED_On(LED_RED);
  }

  printf("8 - Checking Erase whole Chip\n");

  /* Erase full chip */
  if (BSP_FLASH_Erase_Chip(0) != BSP_ERROR_NONE)
  {
    printf("failed\n");
    BSP_LED_On(LED_RED);
  }

  for (uint32_t blockAddress = 0x00000000; blockAddress < 0x00080000; blockAddress += MX25L4006_BLOCK_SIZE)
  {
    if (checkBlockErased(blockAddress) != 0)
    {
      printf("failed\n");
      BSP_LED_On(LED_RED);
      break;
    }

    BSP_LED_Toggle(LED_BLUE);
    HAL_Delay(1);
  }

  /* DeInit the Flash */
  BSP_FLASH_DeInit(0);

  /* DeInit the LEDs */
  BSP_LED_DeInit(LED_RED);
  BSP_LED_DeInit(LED_GREEN);
}

void printSector(uint8_t idxSector)
{
  uint8_t datasRead[64];
  uint32_t idx = 0;
  uint32_t addressSector = idxSector * MX25L4006_SECTOR_SIZE; /* Sector of 4KB */
  memset(datasRead, 0, 64 * sizeof(uint8_t));

  printf("\t- Sector %02i [0x%06x] ", idxSector, addressSector);
  fflush(stdout);
  for (uint32_t idxBlock64 = 0; idxBlock64 < 64; idxBlock64++)
  {
    if (BSP_FLASH_Read(0, datasRead, addressSector, 64) != BSP_ERROR_NONE)
    {
      BSP_LED_On(LED_RED);
    }

    for (idx = 0; idx < 64; idx++)
    {
      printf("%02x ", datasRead[idx]);
      fflush(stdout);
    }
    printf("\n\t\t\t\t");

    addressSector += 64;
  }
  printf("\n");
}

void printSector2(uint32_t address, uint16_t size)
{
  uint8_t datasRead[64];
  uint32_t idx = 0;
  uint32_t addressSector = address; /* Sector of 4KB */
  memset(datasRead, 0, 64 * sizeof(uint8_t));

  printf("\t- Sector [0x%06x] %02i ", address, size);
  fflush(stdout);

  if (BSP_FLASH_Read(0, datasRead, addressSector, size) != BSP_ERROR_NONE)
  {
    BSP_LED_On(LED_RED);
  }

  for (idx = 0; idx < size; idx++)
  {
    printf("%02x ", datasRead[idx]);
    fflush(stdout);
  }

  printf("\n");
}

void printBlock(uint8_t idxBlock)
{
  uint32_t addressBlock = idxBlock * MX25L4006_BLOCK_SIZE; /* Block of 64KB */

  printf("\t- Block %i [0x%06x\n", idxBlock, addressBlock);

  for (uint8_t idxSector = 0; idxSector < 16; idxSector++)
  {
    printSector(idxBlock * 16 + idxSector);
  }
}

int32_t checkSectorErased(uint32_t sectorAddress)
{
  int32_t ret = 0;
  uint8_t dataRead[MX25L4006_PAGE_SIZE];

  uint32_t endSectorAddress = sectorAddress + MX25L4006_SECTOR_SIZE;
  for (uint32_t memoryAddress = sectorAddress; (ret == 0) && (memoryAddress < endSectorAddress); memoryAddress += MX25L4006_PAGE_SIZE)
  {
    memset(dataRead, 0x00, MX25L4006_PAGE_SIZE);
    if (BSP_FLASH_Read(0, dataRead, memoryAddress, MX25L4006_PAGE_SIZE) != BSP_ERROR_NONE)
    {
      ret = -1;
      break;
    }

    for (uint32_t i = 0; i < MX25L4006_PAGE_SIZE; i++)
    {
      if (dataRead[ i ] != 0xFF)
      {
        ret = -2;
        break;
      }
    }
  }

  return ret;
}

int32_t checkBlockErased(uint32_t blockAddress)
{
  int32_t ret = 0;

  uint32_t endBlockAddress = blockAddress + MX25L4006_BLOCK_SIZE;
  for (uint32_t memoryAddress = blockAddress; (ret == 0) && (memoryAddress < endBlockAddress); memoryAddress += MX25L4006_SECTOR_SIZE)
  {
    ret = checkSectorErased(memoryAddress);
  }

  return ret;
}

/**
  * @}
  */

/**
  * @}
  */
