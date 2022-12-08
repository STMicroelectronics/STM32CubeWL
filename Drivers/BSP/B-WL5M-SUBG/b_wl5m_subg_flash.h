/**
  ******************************************************************************
  * @file    b_wl5m_subg_flash.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the b_wl5m_subg_flash.c driver.
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
#ifndef __B_WL5M_SUBG_FLASH_H
#define __B_WL5M_SUBG_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
#include "b_wl5m_subg_errno.h"
#include "mx25l4006.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup B_WL5M_SUBG
  * @{
  */

/** @addtogroup B_WL5M_SUBG_SPI_FLASH
  * @{
  */

/** @defgroup B_WL5M_SUBG_SPI_FLASH_Exported_Types B-WL5M-SUBG SPI FLASH Exported Types
  * @{
  */
#define BSP_SPI_FLASH_Info_t                MX25L4006_Info_t
#define BSP_SPI_FLASH_Erase_t               MX25L4006_Erase_t

typedef enum
{
  SPI_ACCESS_NONE = 0,          /*!<  Instance not initialized,              */
  SPI_ACCESS,                   /*!<  Instance use SPI access mode           */
} SPI_Access_t;

typedef struct
{
  SPI_Access_t              IsInitialized;  /*!<  Instance access Flash method     */
} SPI_FLASH_Ctx_t;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup B_WL5M_SUBG_SPI_FLASH_Exported_Constants B-WL5M-SUBG SPI FLASH Exported Constants
  * @{
  */
#define SPI_FLASH_INSTANCES_NUMBER         1U

/**
  * @brief  FLASH GPIO definition
  */
#define FLASH_CHIP_SELECT_PIN                    GPIO_PIN_6
#define FLASH_CHIP_SELECT_PORT                   GPIOB
#define FLASH_HOLD_PIN                           GPIO_PIN_7
#define FLASH_HOLD_PORT                          GPIOB

#define FLASH_SPI_CLK_PIN                        GPIO_PIN_13
#define FLASH_SPI_CLK_PORT                       GPIOB
#define FLASH_SPI_MOSI_PIN                       GPIO_PIN_10
#define FLASH_SPI_MOSI_PORT                      GPIOA
#define FLASH_SPI_MISO_PIN                       GPIO_PIN_2
#define FLASH_SPI_MISO_PORT                      GPIOC

/**
  * @}
  */

/* Exported variables --------------------------------------------------------*/
/** @defgroup B_WL5M_SUBG_SPI_FLASH_Exported_Variables B-WL5M-SUBG SPI FLASH Exported Variables
  * @{
  */
extern SPI_HandleTypeDef hspi_flash[SPI_FLASH_INSTANCES_NUMBER];
extern SPI_FLASH_Ctx_t Spi_Flash_Ctx[SPI_FLASH_INSTANCES_NUMBER];
/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/

/** @defgroup B_WL5M_SUBG_SPI_FLASH_Exported_Functions B-WL5M-SUBG SPI FLASH Exported Functions
  * @{
  */
int32_t BSP_FLASH_Init(uint32_t Instance);
int32_t BSP_FLASH_DeInit(uint32_t Instance);
int32_t BSP_FLASH_Read(uint32_t Instance, uint8_t *pData, uint32_t StartAddress, uint16_t DataSize);
int32_t BSP_FLASH_FastRead(uint32_t Instance, uint8_t *pData, uint32_t StartAddress, uint16_t DataSize);
int32_t BSP_FLASH_Write(uint32_t Instance, uint8_t *pData, uint32_t StartAddress, uint16_t DataSize);
int32_t BSP_FLASH_Erase_Sector(uint32_t Instance, uint32_t SectorAddress);
int32_t BSP_FLASH_Erase_Block(uint32_t Instance, uint32_t BlockAddress, BSP_SPI_FLASH_Erase_t BlockSize);
int32_t BSP_FLASH_Erase_Chip(uint32_t Instance);

int32_t BSP_FLASH_GetStatus(uint32_t Instance);
int32_t BSP_FLASH_GetInfo(uint32_t Instance, BSP_SPI_FLASH_Info_t *pInfo);

int32_t BSP_FLASH_ReadID(uint32_t Instance, uint8_t *ID);
int32_t BSP_FLASH_ReadSFDP(uint32_t Instance, uint8_t *pData, uint32_t Address, uint16_t DataSize);

int32_t BSP_FLASH_ReadStatusRegister(uint32_t Instance, uint8_t *Status);
int32_t BSP_FLASH_BlockProtectConfig(uint32_t Instance, uint8_t level);

int32_t BSP_FLASH_EnterDeepPowerDown(uint32_t Instance);
int32_t BSP_FLASH_LeaveDeepPowerDown(uint32_t Instance);

/* This function can be modified in case the current settings
   need to be changed for specific application needs */
HAL_StatusTypeDef MX_SPI_FLASH_Init(SPI_HandleTypeDef *hspi);

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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __B_WL5M_SUBG_FLASH_H */
