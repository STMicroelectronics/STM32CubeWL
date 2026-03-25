/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    kms_low_level.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module Low Level access to Flash, CRC...
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

#ifndef KMS_LOW_LEVEL_H
#define KMS_LOW_LEVEL_H

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_hal.h"
/* USER CODE BEGIN KMS_LOW_LEVEL_Includes */
/* USER CODE END KMS_LOW_LEVEL_Includes */

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_LL Low Level access
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN KMS_LOW_LEVEL_Exported_Types */
/* USER CODE END KMS_LOW_LEVEL_Exported_Types */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN KMS_LOW_LEVEL_Exported_Constants */
/* USER CODE END KMS_LOW_LEVEL_Exported_Constants */

/** @addtogroup KMS_LL_Exported_Constants Exported Constants
  * @{
  */

#define KMS_LL_ERROR_MEM_ALLOC_FAILURE    (0x00000001UL)    /*!< Memory allocation failure error */
#define KMS_LL_ERROR_MEM_FREE_CANARY      (0x00000002UL)    /*!< Memory free detected altered canaries error */
#define KMS_LL_ERROR_MEM_FREE_NULL_PTR    (0x00000003UL)    /*!< Memory free detected a NULL pointer error */
#define KMS_LL_ERROR_MEM_FREE_UNKNOWN     (0x00000004UL)    /*!< Memory free detected an unknown pointer error */

/**
  * @}
  */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN KMS_LOW_LEVEL_Exported_Function_Prototypes */
/* USER CODE END KMS_LOW_LEVEL_Exported_Function_Prototypes */

/** @addtogroup KMS_LL_Exported_Functions Exported Functions
  * @{
  */
CK_RV KMS_LL_Initialize(void);
CK_RV KMS_LL_Finalize(void);

CK_RV KMS_LL_FLASH_Read(void *pDestination, const void *pSource, uint32_t Length);

#ifdef KMS_SE_CHECK_PARAMS
void KMS_LL_IsBufferInSecureEnclave(void *pBuffer, uint32_t Size);
#endif /* KMS_SE_CHECK_PARAMS */

#if defined(KMS_ENCRYPT_DECRYPT_BLOB)
CK_RV KMS_LL_DataStorageKey_Init(void);
#endif /* KMS_ENCRYPT_DECRYPT_BLOB */
#if defined(KMS_ENCRYPT_DECRYPT_BLOB) || defined(KMS_GENERATE_KEYS)
CK_RV KMS_LL_GetRandomData(uint32_t *pRandomData);
#endif /* KMS_ENCRYPT_DECRYPT_BLOB || KMS_GENERATE_KEYS */

void KMS_LL_ReportError(uint32_t Error);

void KMS_LL_ReportMemInit(void);
void KMS_LL_ReportMemAlloc(uint32_t Size, void *pMem);
void KMS_LL_ReportMemFree(void *pMem);

#if defined(KMS_MEM_DEBUGGING)
/**
  * @brief  Inline function that returns link register (LR) value
  * @retval LR value
  */
static inline uint32_t KMS_LL_GetLR(void)
{
  return 0;
}
#endif /* KMS_MEM_DEBUGGING */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#endif /* KMS_LOW_LEVEL_H */
