/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    vms_low_level.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module VM Low Level access to RAM storage
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
#ifndef VMS_LOW_LEVEL_H
#define VMS_LOW_LEVEL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN VMS_LOW_LEVEL_Includes */
/* USER CODE END VMS_LOW_LEVEL_Includes */

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_VMLL VM Low Level access
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN VMS_LOW_LEVEL_Exported_Types */
/* USER CODE END VMS_LOW_LEVEL_Exported_Types */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN VMS_LOW_LEVEL_Exported_Constants */
/*
 * KMS VM data storage
 * -------------------
 * Reserved section in the mapping
 * Storage has been defined in the mapping from 0x2000A000 to 0x2000A7FF to reserve area
 */
#define VMS_KMS_DATA_STORAGE_START (0x2000A000UL)
#define VMS_KMS_DATA_STORAGE_END   (0x2000A800UL)
#define VMS_KMS_DATA_STORAGE_SIZE  (VMS_KMS_DATA_STORAGE_END-VMS_KMS_DATA_STORAGE_START)
/* USER CODE END VMS_LOW_LEVEL_Exported_Constants */

/** @addtogroup KMS_VMLL_Exported_Constants Exported Constants
  * @{
  */

#define VMS_LL_ERASED           0xFFFFFFFFU    /*!< Value used to notifies data has been erased */

/**
  * @brief VM writing granularity size
  * @note  Not physical page size, but writing granularity size -> 8 bytes
  */
#define VMS_LL_PAGE_SIZE    8

/**
  * @}
  */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN VMS_LOW_LEVEL_Exported_Function_Prototypes */
/* USER CODE END VMS_LOW_LEVEL_Exported_Function_Prototypes */

/** @addtogroup KMS_VMLL_Exported_Functions Exported Functions
  * @{
  */

/* Exported in line functions ------------------------------------------------*/
/* USER CODE BEGIN VMS_LOW_LEVEL_Exported_In_Line_Function */
/* USER CODE END VMS_LOW_LEVEL_Exported_In_Line_Function */

/**
  * @brief   Returns the base address of the KMS VM data storage
  * @retval  pointer to the base of the KMS VM data storage
  */
/* Template version */
static inline uint32_t VMS_LL_GetDataStorageAddress(void)
{
  /* USER CODE BEGIN VMS_LL_GetDataStorageAddress */
  return (uint32_t)VMS_KMS_DATA_STORAGE_START;
  /* USER CODE END VMS_LL_GetDataStorageAddress */
}

/**
  * @brief   Returns the size of the KMS VM data storage
  * @retval  KMS VM data storage size
  */
/* Template version */
static inline size_t VMS_LL_GetDataStorageSize(void)
{
  /* USER CODE BEGIN VMS_LL_GetDataStorageSize */
  return VMS_KMS_DATA_STORAGE_SIZE;
  /* USER CODE END VMS_LL_GetDataStorageSize */
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

#endif /* VMS_LOW_LEVEL_H */
