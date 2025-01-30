/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    kms_low_level.c
  * @author  MCD Application Team
  * @brief   This file contains implementations for Key Management Services (KMS)
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
/* Define to prevent recursive inclusion -------------------------------------*/
#define KMS_LOW_LEVEL_C

/* Includes ------------------------------------------------------------------*/
#include "kms.h"
#include "kms_low_level.h"
/* USER CODE BEGIN KMS_LOW_LEVEL_Includes */
/* USER CODE END KMS_LOW_LEVEL_Includes */

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_LL Low Level access
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* USER CODE BEGIN KMS_LOW_LEVEL_Private_Types */
/* USER CODE END KMS_LOW_LEVEL_Private_Types */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN KMS_LOW_LEVEL_Private_Defines */
/* USER CODE END KMS_LOW_LEVEL_Private_Defines */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN KMS_LOW_LEVEL_Private_Macros */
/* USER CODE END KMS_LOW_LEVEL_Private_Macros */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN KMS_LOW_LEVEL_Private_Variables */
/* USER CODE END KMS_LOW_LEVEL_Private_Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN KMS_LOW_LEVEL_Private_Function_Prototypes */
/* USER CODE END KMS_LOW_LEVEL_Private_Function_Prototypes */

/* Private function ----------------------------------------------------------*/
/* USER CODE BEGIN KMS_LOW_LEVEL_Private_Functions */
/* USER CODE END KMS_LOW_LEVEL_Private_Functions */

/* Exported functions --------------------------------------------------------*/
/* USER CODE BEGIN KMS_LOW_LEVEL_Private_Exported_Functions */
/* USER CODE END KMS_LOW_LEVEL_Private_Exported_Functions */

/** @addtogroup KMS_LL_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief  Initialize resources required for the low level services
  * @retval Operation status
  */
CK_RV KMS_LL_Initialize(void)
{
  /* USER CODE BEGIN KMS_LL_Initialize */
  return CKR_OK;
  /* USER CODE END KMS_LL_Initialize */
}

/**
  * @brief  Deinitialize the resources required for the low level services
  * @retval Operation status
  */
CK_RV KMS_LL_Finalize(void)
{
  /* USER CODE BEGIN KMS_LL_Finalize */
  return CKR_OK;
  /* USER CODE END KMS_LL_Finalize */
}

/**
  * @brief  Read data from flash and store into buffer
  * @note   Used to access encrypted blob in flash.
  * @param  pDestination buffer to store red data
  * @param  pSource flash aread to read from
  * @param  Length amount of data to read from flash
  * @retval Operation status
  */
CK_RV KMS_LL_FLASH_Read(void *pDestination, const void *pSource, uint32_t Length)
{
  /* USER CODE BEGIN KMS_LL_FLASH_Read */
  if ((pDestination == NULL) || (pSource == NULL) || (Length == 0UL))
  {
    return CKR_FUNCTION_FAILED;
  }
  else
  {
    (void)memcpy(pDestination, pSource, Length);
    return CKR_OK;
  }
  /* USER CODE END KMS_LL_FLASH_Read */
}

#ifdef KMS_SE_CHECK_PARAMS

/* Remove compilation optimization */
#if defined(__ICCARM__)
#pragma optimize=none
#elif defined(__CC_ARM)
#pragma O0
#else
__attribute__((optimize("O0")))
#endif /* __ICCARM__ */

/**
  * @brief  Check if given buffer in inside secure enclave (RAM or Flash, NVM_Storgae)
  *              If it is in Enclave area, then generate a RESET.
  * @param  pBuffer Buffer address
  * @param  ulSize  Buffer size
  * @retval void
  */
void KMS_LL_IsBufferInSecureEnclave(void *pBuffer, uint32_t Size)
{
  /* USER CODE BEGIN KMS_LL_IsBufferInSecureEnclave */
  /* USER CODE END KMS_LL_IsBufferInSecureEnclave */
}
#endif /* KMS_SE_CHECK_PARAMS */

#if defined(KMS_ENCRYPT_DECRYPT_BLOB)
/**
  * @brief  Generate the KMS data storage key
  * @retval Operation status
  */
CK_RV KMS_LL_DataStorageKey_Init(void)
{
  /* USER CODE BEGIN KMS_LL_DataStorageKey_Init */
  return CKR_OK;
  /* USER CODE END KMS_LL_DataStorageKey_Init */
}

/**
  * @brief  Generate a random number
  * @param  pRandomData buffer to store the random number
  * @retval Operation status
  */
CK_RV KMS_LL_GetRandomData(uint32_t *pRandomData)
{
  /* USER CODE BEGIN KMS_LL_GetRandomData */
  return CKR_FUNCTION_NOT_SUPPORTED;
  /* USER CODE END KMS_LL_GetRandomData */
}
#endif /* KMS_ENCRYPT_DECRYPT_BLOB */

/**
  * @brief  An error occurred
  * @note   Used to debug or reset in case of important issue
  * @param  error Error reported by KMS
  */
void KMS_LL_ReportError(uint32_t Error)
{
  /* USER CODE BEGIN KMS_LL_ReportError */

  /* USER CODE END KMS_LL_ReportError */
}

/**
  * @brief  A memory initialization occurred
  */
void KMS_LL_ReportMemInit(void)
{
  /* USER CODE BEGIN KMS_LL_ReportMemInit */

  /* USER CODE END KMS_LL_ReportMemInit */
}

/**
  * @brief  A memory allocation occurred
  * @param  size    Size allocated
  * @param  pMem    Allocated pointer
  */
void KMS_LL_ReportMemAlloc(uint32_t Size, void *pMem)
{
  /* USER CODE BEGIN KMS_LL_ReportMemAlloc */

  /* USER CODE END KMS_LL_ReportMemAlloc */
}

/**
  * @brief  A memory free occurred
  * @param  pMem    Freed pointer
  */
void KMS_LL_ReportMemFree(void *pMem)
{
  /* USER CODE BEGIN KMS_LL_ReportMemFree */

  /* USER CODE END KMS_LL_ReportMemFree */
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
