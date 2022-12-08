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
/* Define to prevent recursive inclusion -------------------------------------*/
#define KMS_LOW_LEVEL_C

/* Includes ------------------------------------------------------------------*/
#include "kms.h"
#include "kms_low_level.h"
#include "se_low_level.h"
#if defined(__ARMCC_VERSION)
#include "mapping_sbsfu.h"
#elif defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#endif /* __ARMCC_VERSION */
#ifdef KMS_ENCRYPT_DECRYPT_BLOB
#include "kms_platf_objects_config.h"     /* KMS embedded objects definitions */
#include "CryptoApi/ca.h"                 /* Crypto API services */
#endif /* KMS_ENCRYPT_DECRYPT_BLOB */


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
#if defined(KMS_ENCRYPT_DECRYPT_BLOB)
#define LENGTH_INDEX                     1
#define VALUE_INDEX                      2
#define KMS_DATASTORAGE_KEY_SIZE        16
#define ENCRYPT_DECRYPT_BLOB_TAG_LENGTH 16
#define UID_USED_LENGTH                 12
#endif /* KMS_ENCRYPT_DECRYPT_BLOB */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined(KMS_ENCRYPT_DECRYPT_BLOB)
RNG_HandleTypeDef hrng;
#endif /* KMS_ENCRYPT_DECRYPT_BLOB */
#if defined(KMS_MEM_LOGGING)
kms_ll_mem_operation_t kms_ll_mem_logging[KMS_LL_MEM_LOGGING_MAX];
uint32_t kms_ll_mem_logging_idx;
#endif /* KMS_MEM_LOGGING */
/* Private function prototypes -----------------------------------------------*/
#if defined(KMS_ENCRYPT_DECRYPT_BLOB)
CK_RV KMS_LL_BlobU32_2_u8ptr(uint32_t *pU32, uint32_t u32Size, uint8_t *pU8);
#endif /* KMS_ENCRYPT_DECRYPT_BLOB */
/* Private function ----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Initialize resources required for the low level services
  * @retval Operation status
  */
CK_RV KMS_LL_Initialize(void)
{
#if defined(KMS_ENCRYPT_DECRYPT_BLOB)
  /* Initialize RNG */
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    return CKR_FUNCTION_FAILED;
  }
#endif /* KMS_ENCRYPT_DECRYPT_BLOB */
  return CKR_OK;
}

/**
  * @brief  Deinitialize the resources required for the low level services
  * @retval Operation status
  */
CK_RV KMS_LL_Finalize(void)
{
#if defined(KMS_ENCRYPT_DECRYPT_BLOB)
  /* Deinitialize RNG */
  if (HAL_RNG_DeInit(&hrng) != HAL_OK)
  {
    return CKR_FUNCTION_FAILED;
  }
#endif /* KMS_ENCRYPT_DECRYPT_BLOB */
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
  * @brief  Check if given buffer in inside secure enclave (RAM or Flash, NVM_Storage)
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
  * @brief  This function transposes a value from u32[] to an u8[] value
  * @note   Storage is as follow:
  *         u8[]  = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08}
  *         u32[] = {0x01020304, 0x05060708}
  * @param  pU32 u32 buffer containing the value to translate (in bytes)
  * @param  u32Size u32 buffer size (in bytes)
  * @param  pU8 translated buffer
  * @retval Operation status
  */
CK_RV KMS_LL_BlobU32_2_u8ptr(uint32_t *pU32, uint32_t u32Size, uint8_t *pU8)
{
  uint32_t index_key;

  /* Check the size of the buffer */
  if (u32Size % sizeof(uint32_t) != 0U)
  {
    return CKR_FUNCTION_FAILED;
  }

  for (index_key = 0; index_key < (u32Size / sizeof(uint32_t)); index_key++)
  {
    pU8[(index_key * sizeof(uint32_t))]       = (uint8_t)(pU32[index_key] >> 24);
    pU8[(index_key * sizeof(uint32_t)) + 1UL] = (uint8_t)(pU32[index_key] >> 16);
    pU8[(index_key * sizeof(uint32_t)) + 2UL] = (uint8_t)(pU32[index_key] >> 8);
    pU8[(index_key * sizeof(uint32_t)) + 3UL] = (uint8_t)(pU32[index_key]);
  }

  return CKR_OK;
}

/**
  * @brief  Generate the KMS data storage key
  * @retval Operation status
  */
CK_RV KMS_LL_DataStorageKey_Init(void)
{
  int32_t aes_status;
  CK_RV rv;
  CA_AESCMACctx_stt ca_ctx;
  uint8_t aes_key[KMS_DATASTORAGE_KEY_SIZE];
  uint8_t aes_data[KMS_DATASTORAGE_KEY_SIZE];
  int32_t tag_length;

  /* Check the size of the input data */
  if ((KMS_DATASTORAGE_KEY_SIZE % sizeof(uint32_t)        != 0U) ||
      (KMS_Data_Storage_AES128_Encrypt_Key[LENGTH_INDEX]  != KMS_DATASTORAGE_KEY_SIZE) ||
      (KMS_Data_Storage_AES128_Encrypt_Data[LENGTH_INDEX] <= UID_USED_LENGTH) ||
      (KMS_Data_Storage_AES128_Encrypt_Data[LENGTH_INDEX] != KMS_DATASTORAGE_KEY_SIZE))
  {
    return CKR_FUNCTION_FAILED;
  }

  /* Check the output buffer */
  if (KMS_DATASTORAGE_KEY_END - KMS_DATASTORAGE_KEY_START + 1 < KMS_DATASTORAGE_KEY_SIZE)
  {
    return CKR_FUNCTION_FAILED;
  }

  /* Initialize the context */
  /* Set flag field to default value */
  ca_ctx.mFlags = CA_E_SK_DEFAULT;
  /* Set key size */
  ca_ctx.mKeySize = KMS_DATASTORAGE_KEY_SIZE;

  /* Read value from the flash. */
  rv = KMS_LL_BlobU32_2_u8ptr((uint32_t *)&(KMS_Data_Storage_AES128_Encrypt_Key[VALUE_INDEX]),
                              KMS_Data_Storage_AES128_Encrypt_Key[LENGTH_INDEX],
                              aes_key);
  ca_ctx.pmKey  = (uint8_t*) aes_key;
  if (rv != CKR_OK)
  {
    return rv;
  }

  /* Size of the returned authentication TAG */
  ca_ctx.mTagSize = KMS_DATASTORAGE_KEY_SIZE;
  ca_ctx.pmTag = (uint8_t *)KMS_DATASTORAGE_KEY_START;

  /* Get the data to encypt: UID + data */
  memcpy(aes_data, (void*)(UID_BASE), UID_USED_LENGTH);
  rv = KMS_LL_BlobU32_2_u8ptr((uint32_t *)&(KMS_Data_Storage_AES128_Encrypt_Data[VALUE_INDEX]),
                              KMS_Data_Storage_AES128_Encrypt_Data[LENGTH_INDEX] - UID_USED_LENGTH,
                              aes_data + UID_USED_LENGTH);
  if (rv != CKR_OK)
  {
    return rv;
  }

  /* Initialize the encryption process */
  aes_status = CA_AES_CMAC_Encrypt_Init(&ca_ctx);

  if (aes_status == CA_AES_SUCCESS)
  {
    /* Specify last packet used */
    ca_ctx.mFlags |= CA_E_SK_FINAL_APPEND;

    /* Perform the encryption */
    aes_status = CA_AES_CMAC_Encrypt_Append(&ca_ctx,
                                            (uint8_t *)aes_data,
                                            (int32_t)KMS_DATASTORAGE_KEY_SIZE);
  }

  if (aes_status == CA_AES_SUCCESS)
  {
    tag_length = ENCRYPT_DECRYPT_BLOB_TAG_LENGTH;
    aes_status = CA_AES_CMAC_Encrypt_Finish(&ca_ctx,
                                            (uint8_t *)KMS_DATASTORAGE_KEY_START,
                                            &tag_length);
  }

  if (aes_status != CA_AES_SUCCESS)
  {
    return CKR_FUNCTION_FAILED;
  }

  return CKR_OK;
}

/**
  * @brief  Generate a random number
  * @param  pRandomData buffer to store the random number
  * @retval Operation status
  */
CK_RV KMS_LL_GetRandomData(uint32_t *pRandomData)
{
  HAL_StatusTypeDef result;

  result = HAL_RNG_GenerateRandomNumber(&hrng, pRandomData);

  /* If the random number isn't well generated */
  if (result != HAL_OK)
  {
    /* Do cold reset as we can have seed error */
    SET_BIT(hrng.Instance->CR, RNG_CR_CONDRST);
    CLEAR_BIT(hrng.Instance->CR, RNG_CR_CONDRST);
    while((hrng.Instance->CR & RNG_CR_CONDRST) != RESET);

    result = HAL_RNG_GenerateRandomNumber(&hrng, pRandomData);

    /* If the random number isn't well generated (second trial) */
    if (result != HAL_OK)
    {
      /* Do cold reset as we can have seed error */
      SET_BIT(hrng.Instance->CR, RNG_CR_CONDRST);
      CLEAR_BIT(hrng.Instance->CR, RNG_CR_CONDRST);
      while((hrng.Instance->CR & RNG_CR_CONDRST) != RESET);

      result = HAL_RNG_GenerateRandomNumber(&hrng, pRandomData);

      /* If the random number isn't well generated (third trial) */
      if (result != HAL_OK)
      {
        /* Return an error as third trial failed */
        return CKR_FUNCTION_FAILED;
      }
    }
  }

  return CKR_OK;
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
