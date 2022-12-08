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
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#include "kms.h"
#include "kms_low_level.h"
#include "se_low_level.h"
#if defined(__ARMCC_VERSION)
#include "mapping_sbsfu.h"
#elif defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#endif /* __ARMCC_VERSION */


/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_LL Low Level access
  * @{
  */

/* Private types -------------------------------------------------------------*/
#if defined(KMS_MEM_LOGGING)
typedef enum
{
  KMS_MEM_NONE    = 0,
  KMS_MEM_ALLOC,
  KMS_MEM_FREE
} kms_ll_mem_optype;
typedef struct
{
  kms_ll_mem_optype   optype;
  uint32_t            size;
  void               *ptr;
} kms_ll_mem_operation_t;
#endif /* KMS_MEM_LOGGING */
/* Private define ------------------------------------------------------------*/
#if defined(KMS_MEM_LOGGING)
#define KMS_LL_MEM_LOGGING_MAX    (100UL)
#endif /* KMS_MEM_LOGGING */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined(KMS_MEM_LOGGING)
kms_ll_mem_operation_t kms_ll_mem_logging[KMS_LL_MEM_LOGGING_MAX];
uint32_t kms_ll_mem_logging_idx;
#endif /* KMS_MEM_LOGGING */
/* Private function prototypes -----------------------------------------------*/
/* Private function ----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize resources required for the low level services
  * @retval Operation status
  */
CK_RV KMS_LL_Initialize(void)
{
  return CKR_OK;
}

/**
  * @brief  Deinitialize the resources required for the low level services
  * @retval Operation status
  */
CK_RV KMS_LL_Finalize(void)
{
  return CKR_OK;
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
  SE_ErrorStatus seStatus;
  seStatus = SE_LL_FLASH_Read(pDestination, pSource, Length);
  return (seStatus == SE_SUCCESS) ? CKR_OK : CKR_GENERAL_ERROR;
}

#ifdef KMS_SE_CHECK_PARAMS

/* Remove compilation optimization */
#if defined(__ICCARM__)
#pragma optimize=none
#elif defined(__CC_ARM)
#pragma O0
#elif defined(__ARMCC_VERSION)
__attribute__((optnone))
#elif defined(__GNUC__)
__attribute__((optimize("O0")))
#endif /* __ICCARM__ */

/**
  * @brief  Check if given buffer in inside secure enclave (RAM or Flash, NVM_Storgae)
  *              If it is in Enclave area, then generate a RESET.
  * @param  pBuffer Buffer address
  * @param  ulSize  Buffer size
  * @retval void
  */
void KMS_LL_IsBufferInSecureEnclave(void *pBuffer, uint32_t ulSize)
{
  /* If pBuffer is NULL, simply return FALSE */
  if ((pBuffer == NULL) || (ulSize == 0UL))
  {
    return;
  }
  /* Check if address range is within secure enclave                           */
  /* Called APIs will generate exception in case buffer is in forbidden area   */
  (void)SE_LL_Buffer_part_of_SE_ram(pBuffer, ulSize);
  /*     Checking RAM Need to add test to protect on NVM_STORAGE */
  (void)SE_LL_Buffer_part_of_SE_rom(pBuffer, ulSize);

  /* Double Check to avoid basic fault injection */
  (void)SE_LL_Buffer_part_of_SE_ram(pBuffer, ulSize);
  /*     Checking RAM Need to add test to protect on NVM_STORAGE */
  (void)SE_LL_Buffer_part_of_SE_rom(pBuffer, ulSize);

  return;
}
#endif /* KMS_SE_CHECK_PARAMS */

#if defined(KMS_ENCRYPT_DECRYPT_BLOB)
/**
  * @brief  Generate the KMS data storage key
  * @retval Operation status
  */
CK_RV KMS_LL_DataStorageKey_Init(void)
{
  return CKR_OK;
}

/**
  * @brief  Generate a random number
  * @param  pRandomData buffer to store the random number
  * @retval Operation status
  */
CK_RV KMS_LL_GetRandomData(uint32_t *pRandomData)
{
  return CKR_FUNCTION_NOT_SUPPORTED;
}
#endif /* KMS_ENCRYPT_DECRYPT_BLOB */

/**
  * @brief  An error occurred
  * @note   Used to debug or reset in case of important issue
  * @param  error Error reported by KMS
  */
void KMS_LL_ReportError(uint32_t error)
{
  (void)error;
  while (1 == 1) {;}
}

#if defined(KMS_MEM_LOGGING)
/**
  * @brief  A memory initialization occurred
  */
void KMS_LL_ReportMemInit(void)
{
  for (uint32_t i = 0; i < KMS_LL_MEM_LOGGING_MAX; i++)
  {
    kms_ll_mem_logging[i].optype = KMS_MEM_NONE;
    kms_ll_mem_logging[i].size = 0;
    kms_ll_mem_logging[i].ptr = NULL_PTR;
  }
  kms_ll_mem_logging_idx = 0;
}
#endif /* KMS_MEM_LOGGING */

#if defined(KMS_MEM_LOGGING)
/**
  * @brief  A memory allocation occurred
  * @param  size    Size allocated
  * @param  pmem    Allocated pointer
  */
void KMS_LL_ReportMemAlloc(uint32_t size, void *pmem)
{
  if (kms_ll_mem_logging_idx < KMS_LL_MEM_LOGGING_MAX)
  {
    kms_ll_mem_logging[kms_ll_mem_logging_idx].optype = KMS_MEM_ALLOC;
    kms_ll_mem_logging[kms_ll_mem_logging_idx].size = size;
    kms_ll_mem_logging[kms_ll_mem_logging_idx].ptr = pmem;
    kms_ll_mem_logging_idx++;
  }
}
#endif /* KMS_MEM_LOGGING */

#if defined(KMS_MEM_LOGGING)
/**
  * @brief  A memory free occurred
  * @param  pmem    Freed pointer
  */
void KMS_LL_ReportMemFree(void *pmem)
{
  if (kms_ll_mem_logging_idx < KMS_LL_MEM_LOGGING_MAX)
  {
    kms_ll_mem_logging[kms_ll_mem_logging_idx].optype = KMS_MEM_FREE;
    kms_ll_mem_logging[kms_ll_mem_logging_idx].size = 0;
    kms_ll_mem_logging[kms_ll_mem_logging_idx].ptr = pmem;
    kms_ll_mem_logging_idx++;
  }
}
#endif /* KMS_MEM_LOGGING */
