/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    kms_platf_objects_config.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Key Management Services (KMS)
  *          module platform objects management configuration
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
#ifndef KMS_PLATF_OBJECTS_CONFIG_H
#define KMS_PLATF_OBJECTS_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "kms_platf_objects_interface.h"
/* USER CODE BEGIN KMS_PLATF_OBJECTS_CONFIG_Includes */
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

/* We consider that the ORDER (static = lower ids) will be kept. */
#define KMS_INDEX_MIN_EMBEDDED_OBJECTS                1UL   /*!< Embedded objects min ID. Must be > 0 as '0' is never a
                                                                 valid key index */
#define KMS_INDEX_MAX_EMBEDDED_OBJECTS               19UL   /*!< Embedded objects max ID */
#define KMS_INDEX_MIN_NVM_STATIC_OBJECTS             20UL   /*!< NVM static objects min ID */
#define KMS_INDEX_MAX_NVM_STATIC_OBJECTS             24UL   /*!< NVM static objects max ID */
#define KMS_INDEX_MIN_NVM_DYNAMIC_OBJECTS            25UL   /*!< NVM dynamic objects min ID */
#define KMS_INDEX_MAX_NVM_DYNAMIC_OBJECTS            29UL   /*!< NVM dynamic objects max ID */

/* Blob import key index */
#define KMS_INDEX_BLOBIMPORT_VERIFY       (1U)      /*!< Index in @ref KMS_PlatfObjects_EmbeddedList
                                                         where the blob verification key is stored */
#define KMS_INDEX_BLOBIMPORT_DECRYPT      (2U)      /*!< Index in @ref KMS_PlatfObjects_EmbeddedList
                                                         where the blob decryption key is stored */

/* USER CODE END KMS_PLATF_OBJECTS_CONFIG_Exported_Constants */

/*
 * Embedded objects definition
 *
 */
#ifdef KMS_PLATF_OBJECTS_C
/* USER CODE BEGIN KMS_PLATF_OBJECTS_CONFIG_Embedded_Objects_Definition */
KMS_DECLARE_BLOB_STRUCT(,37)
KMS_DECLARE_BLOB_STRUCT(,25)

/* This object is used for KMS blob header signature                 */
static const kms_obj_keyhead_37_t   KMS_Blob_ECDSA_Verify =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  148,                             /*  uint32_t blobs_size; */
  6,                               /*  uint32_t blobs_count; */
  3,                               /*   uint32_t object_id; */
  {
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_PUBLIC_KEY,
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),     CKK_EC,
    CKA_TOKEN,        sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_LABEL,        8, 0x424F4C42U, 0x49524556U,      /* 'BLOB', 'VERI' */
    CKA_EC_PARAMS,    10, 0x06082a86U, 0x48ce3d03U, 0x0107U,
    CKA_EC_POINT,     67, 0x044104baU, 0xf297f83eU, 0xe307dc16U, 0xc371781dU, 0xf1b03ef0U,
                          0x95b04454U, 0x128148fbU, 0x2c66b954U, 0x3da54ae8U, 0x260476b7U,
                          0x378b3c46U, 0xd8fd6a63U, 0x617c46c3U, 0x7de94644U, 0x316ed7e1U,
                          0x6dba70edU, 0x44ba02U,
  }
};

/* This object is used for KMS blob encryption                    */
static const kms_obj_keyhead_25_t   KMS_Blob_AES_CBC128_Decrypt =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  100,                             /*  uint32_t blobs_size; */
  7,                               /*  uint32_t blobs_count; */
  4,                               /*  uint32_t object_id; */
  {
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),          CKK_AES,
    CKA_VALUE,        16, 0x4f454d5fU, 0x4b45595fU, 0x434f4d50U, 0x414e5931U,
    CKA_DECRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_TOKEN,        sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_LABEL,        8, 0x424F4C42U, 0x50595243U,      /* 'BLOB', 'CRYP' */
    CKA_COPYABLE,     sizeof(CK_BBOOL), (uint8_t)CK_FALSE,
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
  }
};
/* USER CODE END KMS_PLATF_OBJECTS_CONFIG_Embedded_Objects_Definition */

/** @addtogroup KMS_PLATF_Private_Variables Private Variables
  * @{
  */
/**
  * @brief  KMS embedded objects definition
  * @note   Must contains KMS blob verification and decryption keys
  */
const kms_obj_keyhead_t *const KMS_PlatfObjects_EmbeddedList[KMS_INDEX_MAX_EMBEDDED_OBJECTS -
                                                             KMS_INDEX_MIN_EMBEDDED_OBJECTS + 1] =
{
/* USER CODE BEGIN KMS_PlatfObjects_EmbeddedList */
  (kms_obj_keyhead_t *) &KMS_Blob_ECDSA_Verify,       /* Index = 1 */
  (kms_obj_keyhead_t *) &KMS_Blob_AES_CBC128_Decrypt, /* Index = 2 */
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

