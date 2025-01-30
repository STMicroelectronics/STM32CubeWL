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
/* USER CODE END KMS_PLATF_OBJECTS_CONFIG_Exported_Constants */

/*
 * Embedded objects definition
 *
 */
#ifdef KMS_PLATF_OBJECTS_C
/* USER CODE BEGIN KMS_PLATF_OBJECTS_CONFIG_Embedded_Objects_Definition */
KMS_DECLARE_BLOB_STRUCT(,36)

/* These objects are used by user tKMS application                 */
static const kms_obj_keyhead_36_t   Test_Key_AES128 =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  144,                             /*  uint32_t blobs_size; */
  10,                              /*  uint32_t blobs_count; */
  1,                               /*  uint32_t object_id; */
  {
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),          CKK_AES,
    CKA_VALUE,        16, 0xfeffe992U, 0x8665731cU, 0x6d6a8f94U, 0x67308308U,
    CKA_ENCRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_DECRYPT,      sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_TOKEN,        sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_COPYABLE,     sizeof(CK_BBOOL), (uint8_t)CK_FALSE,
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_DERIVE,       sizeof(CK_BBOOL), (uint8_t)CK_TRUE,
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
    CKA_LABEL,        12, 0x52455355U, 0x50595243U, 0x00383231U,     /* 'USER', 'CRYP', '128' */
    0UL /* Fill end of table */
  }
};

static const kms_obj_keyhead_36_t   Test_Key_AES256 =
{
  KMS_ABI_VERSION_CK_2_40,         /* uint32_t version;  */
  KMS_ABI_CONFIG_KEYHEAD,          /* uint32_t configuration; */
  144,                             /* uint32_t blobs_size; */
  9,                               /* uint32_t blobs_count; */
  2,                               /* uint32_t object_id; */
  {
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE), CKK_AES,
    CKA_VALUE,        32, 0x03030303U, 0x03030303U, 0x03030303U, 0x03030303U,
    0x03030303U, 0x03030303U, 0x03030303U, 0x03030303U,
    CKA_ENCRYPT,      sizeof(CK_BBOOL), CK_TRUE,
    CKA_DECRYPT,      sizeof(CK_BBOOL), CK_TRUE,
    CKA_TOKEN,        sizeof(CK_BBOOL), CK_TRUE,
    CKA_COPYABLE,     sizeof(CK_BBOOL), CK_FALSE,
    CKA_MODIFIABLE,   sizeof(CK_BBOOL), CK_FALSE,
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), CKO_SECRET_KEY,
    CKA_LABEL,        12, 0x52455355U, 0x50595243U, 0x00363532U,      /* 'USER', 'CRYP', '256' */
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
  (kms_obj_keyhead_t *) &Test_Key_AES128,             /* Index = 1 */
  (kms_obj_keyhead_t *) &Test_Key_AES256,             /* Index = 2 */
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

