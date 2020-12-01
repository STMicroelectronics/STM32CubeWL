/**
  ******************************************************************************
  * @file    kms_platf_objects_config.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module platform objects management configuration
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef KMS_PLATF_OBJECTS_CONFIG_H
#define KMS_PLATF_OBJECTS_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "kms_platf_objects_interface.h"
/* USER CODE BEGIN KMS_PLATF_OBJECTS_CONFIG_Includes */
#include "stm32wlxx.h"
#include "Commissioning.h"
/* USER CODE END KMS_PLATF_OBJECTS_CONFIG_Includes */

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @addtogroup KMS_PLATF Platform Objects
  * @{
  */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN KMS_PLATF_OBJECTS_CONFIG_Exported_Constants */

/** @addtogroup KMS_PLATF_Exported_Constants Exported Constants
  * @note KMS support different type of objects, their respective ranges are
  *       defined here
  * @{
  */

/* We consider that the ORDER (static = lower ids) will be kept  */
#define KMS_INDEX_MIN_EMBEDDED_OBJECTS                1UL   /*!< Embedded objects min ID. Must be > 0 as '0' is never a
                                                                 valid key index */
#define KMS_INDEX_MAX_EMBEDDED_OBJECTS               29UL   /*!< Embedded objects max ID */
#define KMS_INDEX_MIN_NVM_STATIC_OBJECTS             30UL   /*!< NVM static objects min ID */
#define KMS_INDEX_MAX_NVM_STATIC_OBJECTS             49UL   /*!< NVM static objects max ID */
#define KMS_INDEX_MIN_NVM_DYNAMIC_OBJECTS            50UL   /*!< NVM dynamic objects min ID */
#define KMS_INDEX_MAX_NVM_DYNAMIC_OBJECTS            69UL   /*!< NVM dynamic objects max ID */

/* When EXTERNAL TOKEN is not supported the below values can be commented */
#define KMS_INDEX_MIN_EXT_TOKEN_STATIC_OBJECTS        70UL  /*!< External token static objects min ID */
#define KMS_INDEX_MAX_EXT_TOKEN_STATIC_OBJECTS        89UL  /*!< External token static objects max ID */
#define KMS_INDEX_MIN_EXT_TOKEN_DYNAMIC_OBJECTS       90UL  /*!< External token dynamic objects min ID */
#define KMS_INDEX_MAX_EXT_TOKEN_DYNAMIC_OBJECTS      110UL  /*!< External token dynamic objects max ID */

/* Blob import key index */
#define KMS_INDEX_BLOBIMPORT_VERIFY       (1U)      /*!< Index in @ref KMS_PlatfObjects_EmbeddedList
                                                         where the blob verification key is stored */
#define KMS_INDEX_BLOBIMPORT_DECRYPT      (2U)      /*!< Index in @ref KMS_PlatfObjects_EmbeddedList
                                                         where the blob decryption key is stored */

#define RAW_TO_INT32A(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) \
  0x##a##b##c##d, 0x##e##f##g##h, 0x##i##j##k##l, 0x##m##n##o##p

#define FORMAT_KEY_32(...) RAW_TO_INT32A(__VA_ARGS__)

/* USER CODE END KMS_PLATF_OBJECTS_CONFIG_Exported_Constants */

/**
  * @}
  */

/*
 * Embedded objects definition
 *
 */
#ifdef KMS_PLATF_OBJECTS_C
/* USER CODE BEGIN KMS_PLATF_OBJECTS_CONFIG_Embedded_Objects_Definition */
/* Place code in a specific section*/
#if defined(__ICCARM__)
#pragma default_variable_attributes = @ ".USER_embedded_Keys"
#elif defined(__CC_ARM)
#pragma arm section rodata = ".USER_embedded_Keys"
#endif

KMS_DECLARE_BLOB_STRUCT(, 32)

#if defined(__GNUC__)
__attribute__((section(".USER_embedded_Keys")))
#endif
/* These objects are used by user tKMS application                 */
static const kms_obj_keyhead_32_t   Lorawan_Zero_Key =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  128,                             /*  uint32_t blobs_size; */
  9,                               /*  uint32_t blobs_count; */
  1,                               /*  uint32_t object_id; */
  {
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),          CKK_AES,
    CKA_VALUE,        16, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
    CKA_ENCRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_DECRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_TOKEN,        sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_COPYABLE,     sizeof(CK_BBOOL), (uint8_t)CK_FALSE,
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL), (uint8_t)CK_FALSE,
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
    CKA_LABEL,        12, 0x41524F4CU, 0x50595243U, 0x00383231U      /* 'LORA', 'CRYP', '128' */
  }
};

#if defined(__GNUC__)
__attribute__((section(".USER_embedded_Keys")))
#endif
static const kms_obj_keyhead_32_t   Lorawan_App_Key =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  128,                             /*  uint32_t blobs_size; */
  9,                               /*  uint32_t blobs_count; */
  2,                               /*  uint32_t object_id; */
  {
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),          CKK_AES,
    CKA_VALUE,        16, FORMAT_KEY_32(LORAWAN_APP_KEY),
    CKA_ENCRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_DECRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_TOKEN,        sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_COPYABLE,     sizeof(CK_BBOOL), (uint8_t)CK_FALSE,
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL), (uint8_t)CK_FALSE,
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
    CKA_LABEL,        12, 0x41524F4CU, 0x50595243U, 0x00383231U      /* 'LORA', 'CRYP', '128' */
  }
};

#if defined(__GNUC__)
__attribute__((section(".USER_embedded_Keys")))
#endif
static const kms_obj_keyhead_32_t   Lorawan_Nwk_Key =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  128,                             /*  uint32_t blobs_size; */
  9,                               /*  uint32_t blobs_count; */
  3,                               /*  uint32_t object_id; */
  {
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),          CKK_AES,
    CKA_VALUE,        16, FORMAT_KEY_32(LORAWAN_NWK_KEY),
    CKA_ENCRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_DECRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_TOKEN,        sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_COPYABLE,     sizeof(CK_BBOOL), (uint8_t)CK_FALSE,
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL), (uint8_t)CK_FALSE,
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
    CKA_LABEL,        12, 0x41524F4CU, 0x50595243U, 0x00383231U      /* 'LORA', 'CRYP', '128' */
  }
};

#if defined(__GNUC__)
__attribute__((section(".USER_embedded_Keys")))
#endif
static const kms_obj_keyhead_32_t   Lorawan_Nwk_S_Key =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  128,                             /*  uint32_t blobs_size; */
  9,                               /*  uint32_t blobs_count; */
  6,                               /*  uint32_t object_id; */
  {
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),          CKK_AES,
    CKA_VALUE,        16, FORMAT_KEY_32(LORAWAN_NWK_S_KEY),
    CKA_ENCRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_DECRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_TOKEN,        sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_COPYABLE,     sizeof(CK_BBOOL), (uint8_t)CK_FALSE,
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL), (uint8_t)CK_FALSE,
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
    CKA_LABEL,        12, 0x41524F4CU, 0x50595243U, 0x00383231U      /* 'LORA', 'CRYP', '128' */
  }
};

#if defined(__GNUC__)
__attribute__((section(".USER_embedded_Keys")))
#endif
static const kms_obj_keyhead_32_t   Lorawan_App_S_Key =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  128,                             /*  uint32_t blobs_size; */
  9,                               /*  uint32_t blobs_count; */
  7,                               /*  uint32_t object_id; */
  {
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),          CKK_AES,
    CKA_VALUE,        16, FORMAT_KEY_32(LORAWAN_APP_S_KEY),
    CKA_ENCRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_DECRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_TOKEN,        sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_COPYABLE,     sizeof(CK_BBOOL), (uint8_t)CK_FALSE,
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL), (uint8_t)CK_FALSE,
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
    CKA_LABEL,        12, 0x41524F4CU, 0x50595243U, 0x00383231U      /* 'LORA', 'CRYP', '128' */
  }
};

/* Stop placing data in specified section*/
#if defined(__ICCARM__)
#pragma default_variable_attributes =
#elif defined(__CC_ARM)
#pragma arm section code
#endif
/* USER CODE END KMS_PLATF_OBJECTS_CONFIG_Embedded_Objects_Definition */

/** @addtogroup KMS_PLATF_Private_Variables Private Variables
  * @{
  */
/**
  * @brief  KMS embedded objects definition
  * @note   Must contains KMS blob verification and decryption keys
  */
const kms_obj_keyhead_t * const KMS_PlatfObjects_EmbeddedList[KMS_INDEX_MAX_EMBEDDED_OBJECTS - KMS_INDEX_MIN_EMBEDDED_OBJECTS + 1] =
{
/* USER CODE BEGIN KMS_PlatfObjects_EmbeddedList */
  /* UserApp example keys */
  (kms_obj_keyhead_t *) &Lorawan_Zero_Key,            /* Index = 1 */
  (kms_obj_keyhead_t *) &Lorawan_App_Key,             /* Index = 2 */
  (kms_obj_keyhead_t *) &Lorawan_Nwk_Key,             /* Index = 3 */
  (kms_obj_keyhead_t *) &Lorawan_Nwk_S_Key,           /* Index = 4 */
  (kms_obj_keyhead_t *) &Lorawan_App_S_Key,           /* Index = 5 */
/* USER CODE END KMS_PlatfObjects_EmbeddedList */
};

/**
  * @}
  */
#endif /* KMS_PLATF_OBJECTS_C */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* KMS_PLATF_OBJECTS_CONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

