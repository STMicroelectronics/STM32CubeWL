/**
  ******************************************************************************
  * @file    kms_low_level.c
  * @author  MCD Application Team
  * @brief   This file contains implementations for Key Management Services (KMS)
  *          module Low Level access to Flash, CRC...
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

/**
  * @brief  Check if given buffer in inside secure enclave (RAM or Flash, NVM_Storgae)
  *              If it is in Enclave area, then generate a RESET.
  * @param  pBuffer Buffer address
  * @param  ulSize  Buffer size
  * @retval void
  */
void KMS_LL_IsBufferInSecureEnclave(void *pBuffer, uint32_t Size)
{

}
#endif /* KMS_SE_CHECK_PARAMS */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
