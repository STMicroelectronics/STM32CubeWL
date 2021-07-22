/**
  ******************************************************************************
  * @file    nvms_low_level.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module NVM Low Level access to physical storage (Flash...)
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef NVMS_LOW_LEVEL_H
#define NVMS_LOW_LEVEL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if defined(__ARMCC_VERSION)
#include "mapping_sbsfu.h"
#elif defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#endif /* __ARMCC_VERSION */

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_NVMLL NVM Low Level access
  * @{
  */

/* Exported constants --------------------------------------------------------*/

/** @addtogroup KMS_NVMLL_Exported_Constants Exported Constants
  * @{
  */
#define NVMS_LL_BLOCK0_ADDRESS   KMS_DATASTORAGE_START
#define NVMS_LL_BLOCK1_ADDRESS   (KMS_DATASTORAGE_START + NVMS_LL_BLOCK_SIZE)

#define NVMS_LL_NB_PAGE_PER_BLOCK  (NVMS_LL_BLOCK_SIZE / FLASH_PAGE_SIZE)
#define NVMS_LL_BLOCK_SIZE         ((KMS_DATASTORAGE_END- KMS_DATASTORAGE_START + 1U) / 2U)

#define NVMS_LL_ERASED           0xFFFFFFFFU

/* not physical flash page size, but writing granularity size -> 8 bytes */
#define NVMS_LL_PAGE_SIZE    8UL

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup KMS_NVMLL_Exported_Types Exported Types
  * @{
  */

/**
  * @brief   Block identifiers.
  */
typedef enum
{
  NVMS_BLOCK0 = 0,
  NVMS_BLOCK1 = 1,
} nvms_block_t;


/**
  * @}
  */


/* Exported functions prototypes ---------------------------------------------*/

/** @addtogroup KMS_NVMLL_Exported_Functions Exported Functions
  * @{
  */
void NVMS_LL_Init(void);
bool NVMS_LL_IsBlockErased(nvms_block_t block);
bool NVMS_LL_BlockErase(nvms_block_t block);
bool NVMS_LL_Write(const uint8_t *source, uint8_t *destination, size_t size);

/* Exported in line functions ------------------------------------------------*/

/**
  * @brief   Returns the base address of the specified flash block
  * @param   block block identifier
  * @retval  pointer to the block base
  */
static inline uint32_t NVMS_LL_GetBlockAddress(nvms_block_t block)
{

  if (block == NVMS_BLOCK0)
  {
    return (uint32_t)NVMS_LL_BLOCK0_ADDRESS;
  }
  return (uint32_t)NVMS_LL_BLOCK1_ADDRESS;
}


/**
  * @brief   Returns the size of the flash blocks
  * @note    Blocks are required to be of equal size
  * @retval  Block size
  */
static inline size_t NVMS_LL_GetBlockSize(void)
{

  return NVMS_LL_BLOCK_SIZE;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
