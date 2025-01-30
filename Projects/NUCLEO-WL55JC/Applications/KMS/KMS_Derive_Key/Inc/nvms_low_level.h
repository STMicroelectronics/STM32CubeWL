/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    nvms_low_level.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module NVM Low Level access to physical storage (Flash...)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef NVMS_LOW_LEVEL_H
#define NVMS_LOW_LEVEL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN NVMS_LOW_LEVEL_Includes */
/* USER CODE END NVMS_LOW_LEVEL_Includes */

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_NVMLL NVM Low Level access
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN NVMS_LOW_LEVEL_Exported_Types */
/* USER CODE END NVMS_LOW_LEVEL_Exported_Types */

/** @addtogroup KMS_NVMLL_Exported_Types Exported Types
  * @{
  */

/**
  * @brief   Block identifiers.
  */
typedef enum
{
  NVMS_BLOCK0 = 0,    /*!< Block ID 0 */
  NVMS_BLOCK1 = 1,    /*!< Block ID 1 */
} nvms_block_t;

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN NVMS_LOW_LEVEL_Exported_Constants */
/*
 * KMS NVM data storage
 * --------------------
 * Reserved section in the mapping that is composed of 2 blocks of the same size
 * Blocks are contiguous and each of them must be erasable independently
 * Storage has been defined into mapping from 0x0803F000 to 0x0803FFFF to reserve area
 */
#define NVMS_LL_BLOCK0_ADDRESS   (0x0803F000UL)
#define NVMS_LL_BLOCK1_ADDRESS   (0x0803F800UL)

#define NVMS_LL_NB_PAGE_PER_BLOCK  (NVMS_LL_BLOCK_SIZE / FLASH_PAGE_SIZE)
#define NVMS_LL_BLOCK_SIZE         (NVMS_LL_BLOCK1_ADDRESS-NVMS_LL_BLOCK0_ADDRESS)
/* USER CODE END NVMS_LOW_LEVEL_Exported_Constants */

/** @addtogroup KMS_NVMLL_Exported_Constants Exported Constants
  * @{
  */

#define NVMS_LL_ERASED           0xFFFFFFFFU    /*!< Value used to notifies data has been erased */

/**
  * @brief NVM writing granularity size
  * @note  Not physical flash page size, but writing granularity size -> 8 bytes
  */
#define NVMS_LL_PAGE_SIZE    8

/**
  * @}
  */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN NVMS_LOW_LEVEL_Exported_Function_Prototypes */
/* USER CODE END NVMS_LOW_LEVEL_Exported_Function_Prototypes */

/** @addtogroup KMS_NVMLL_Exported_Functions Exported Functions
  * @{
  */
void NVMS_LL_Init(void);
bool NVMS_LL_IsBlockErased(nvms_block_t block);
bool NVMS_LL_BlockErase(nvms_block_t block);
bool NVMS_LL_Write(const uint8_t *source, uint8_t *destination, size_t size);

/* Exported in line functions ------------------------------------------------*/
/* USER CODE BEGIN NVMS_LOW_LEVEL_Exported_In_Line_Function */
/* USER CODE END NVMS_LOW_LEVEL_Exported_In_Line_Function */

/**
  * @brief   Returns the base address of the specified flash block
  * @param   block block identifier
  * @retval  pointer to the block base
  */
/* Template version */
static inline uint32_t NVMS_LL_GetBlockAddress(nvms_block_t block)
{
  /* USER CODE BEGIN NVMS_LL_GetBlockAddress */
  if (block == NVMS_BLOCK0)
  {
    return (uint32_t)NVMS_LL_BLOCK0_ADDRESS;
  }
  return (uint32_t)NVMS_LL_BLOCK1_ADDRESS;
  /* USER CODE END NVMS_LL_GetBlockAddress */
}

/**
  * @brief   Returns the size of the flash blocks
  * @note    Blocks are required to be of equal size
  * @retval  Block size
  */
/* Template version */
static inline size_t NVMS_LL_GetBlockSize(void)
{
  /* USER CODE BEGIN NVMS_LL_GetBlockSize */
  return NVMS_LL_BLOCK_SIZE;
  /* USER CODE END NVMS_LL_GetBlockSize */
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

#ifdef __cplusplus
}
#endif

#endif /* NVMS_LOW_LEVEL_H */
