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
#include "stm32wlxx.h"
#include "Commissioning.h"
#include "lorawan_conf.h" /* KEY_EXTRACTABLE */
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

#define KMS_INDEX_MIN_VM_DYNAMIC_OBJECTS             50UL   /*!< VM dynamic objects min ID */
#define KMS_INDEX_MAX_VM_DYNAMIC_OBJECTS             69UL   /*!< VM dynamic objects max ID */

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
#define RAW_TO_DEVJOINKEY_STRUCT(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x) \
  0x##a##b##c##d, 0x##e##f##g##h, 0x##i##j##k##l, 0x##m##n##o##p, 0x##t##s##r##q, 0x##x##w##v##u

#define FORMAT_KEY_32(...) RAW_TO_INT32A(__VA_ARGS__)
#define FORMAT_DEVJOINKEY(...) RAW_TO_DEVJOINKEY_STRUCT(__VA_ARGS__)
/**
  * @}
  */

/* USER CODE END KMS_PLATF_OBJECTS_CONFIG_Exported_Constants */

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

KMS_DECLARE_BLOB_STRUCT(, 21);
KMS_DECLARE_BLOB_STRUCT(, 27);
KMS_DECLARE_BLOB_STRUCT(, 29);

/* These objects are used by user tKMS application                 */
#if defined(__GNUC__)
#pragma GCC push_options
#pragma GCC optimize ("O0")
#endif
#if ( LORAMAC_CLASSB_ENABLED == 1 )
#if defined(__GNUC__)
__attribute__((section(".USER_embedded_Keys")))
#endif
static const kms_obj_keyhead_27_t   Lorawan_Zero_Key =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  108,                             /*  uint32_t blobs_size; */
  8,                               /*  uint32_t blobs_count; */
  1,                               /*  uint32_t object_id; */
  {
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),     CKK_AES,
    CKA_VALUE,        16UL,                    0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
    CKA_DERIVE,       sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
    CKA_DECRYPT,      sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
    CKA_COPYABLE,     sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
    CKA_LABEL,        4UL,                     0x5F45524CU /* 'LRE_' */
  }
};
#endif /* LORAMAC_CLASSB_ENABLED == 1 */

#if defined(__GNUC__)
__attribute__((section(".USER_embedded_Keys")))
#endif
static const kms_obj_keyhead_27_t   Lorawan_App_Key =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  108,                             /*  uint32_t blobs_size; */
  8,                               /*  uint32_t blobs_count; */
  2,                               /*  uint32_t object_id; */
  {
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),     CKK_AES,
    CKA_VALUE,        16UL,                    FORMAT_KEY_32(LORAWAN_APP_KEY),
    CKA_ENCRYPT,      sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
    CKA_DECRYPT,      sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
    CKA_COPYABLE,     sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
#if (!defined (KEY_EXTRACTABLE) || (KEY_EXTRACTABLE == 0))
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
#else
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL),        (uint8_t)CK_TRUE,
#endif /* KEY_EXTRACTABLE */
    CKA_LABEL,        4UL,                     0x5F45524CU /* 'LRE_' */
  }
};

#if defined(__GNUC__)
__attribute__((section(".USER_embedded_Keys")))
#endif
static const kms_obj_keyhead_27_t   Lorawan_Nwk_Key =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  108,                             /*  uint32_t blobs_size; */
  8,                               /*  uint32_t blobs_count; */
  3,                               /*  uint32_t object_id; */
  {
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),     CKK_AES,
    CKA_VALUE,        16UL,                    FORMAT_KEY_32(LORAWAN_NWK_KEY),
    CKA_ENCRYPT,      sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
    CKA_DECRYPT,      sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
    CKA_COPYABLE,     sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
#if (!defined (KEY_EXTRACTABLE) || (KEY_EXTRACTABLE == 0))
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
#else
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL),        (uint8_t)CK_TRUE,
#endif /* KEY_EXTRACTABLE */
    CKA_LABEL,        4UL,                     0x5F45524CU /* 'LRE_' */
  }
};

#if defined(__GNUC__)
__attribute__((section(".USER_embedded_Keys")))
#endif
static const kms_obj_keyhead_29_t   Lorawan_DevJoinEuiAddr_Key =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  116,                             /*  uint32_t blobs_size; */
  8,                               /*  uint32_t blobs_count; */
  4,                               /*  uint32_t object_id; */
  {
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),     CKK_AES,
    CKA_VALUE,        24UL,                    FORMAT_DEVJOINKEY(LORAWAN_DEVICE_EUI,LORAWAN_JOIN_EUI,LORAWAN_DEVICE_ADDRESS,LORAWAN_DEVICE_ADDRESS),
    CKA_ENCRYPT,      sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
    CKA_DECRYPT,      sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
    CKA_COPYABLE,     sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL),        (uint8_t)CK_TRUE,
    CKA_LABEL,        4UL,                     0x5F45524CU /* 'LRE_' */
  }
};

#if defined(__GNUC__)
__attribute__((section(".USER_embedded_Keys")))
#endif
static const kms_obj_keyhead_21_t   Lorawan_Nwk_S_Key =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  84,                              /*  uint32_t blobs_size; */
  6,                               /*  uint32_t blobs_count; */
  5,                               /*  uint32_t object_id; */
  {
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),     CKK_AES,
    CKA_VALUE,        16UL,                    FORMAT_KEY_32(LORAWAN_NWK_S_KEY),
    CKA_COPYABLE,     sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
#if (!defined (KEY_EXTRACTABLE) || (KEY_EXTRACTABLE == 0))
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
#else
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL),        (uint8_t)CK_TRUE,
#endif /* KEY_EXTRACTABLE */
    CKA_LABEL,        4UL,                     0x5F45524CU /* 'LRE_' */
  }
};

#if defined(__GNUC__)
__attribute__((section(".USER_embedded_Keys")))
#endif
static const kms_obj_keyhead_21_t   Lorawan_App_S_Key =
{
  KMS_ABI_VERSION_CK_2_40,         /*  uint32_t version; */
  KMS_ABI_CONFIG_KEYHEAD,          /*  uint32_t configuration; */
  84,                              /*  uint32_t blobs_size; */
  6,                               /*  uint32_t blobs_count; */
  6,                               /*  uint32_t object_id; */
  {
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), (uint8_t)CKO_SECRET_KEY,
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE),     CKK_AES,
    CKA_VALUE,        16UL,                    FORMAT_KEY_32(LORAWAN_APP_S_KEY),
    CKA_COPYABLE,     sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
#if (!defined (KEY_EXTRACTABLE) || (KEY_EXTRACTABLE == 0))
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL),        (uint8_t)CK_FALSE,
#else
    CKA_EXTRACTABLE,  sizeof(CK_BBOOL),        (uint8_t)CK_TRUE,
#endif /* KEY_EXTRACTABLE */
    CKA_LABEL,        4UL,                     0x5F45524CU /* 'LRE_' */
  }
};

/* Stop placing data in specified section*/
#if defined(__ICCARM__)
#pragma default_variable_attributes =
#elif defined(__CC_ARM)
#pragma arm section code
#elif defined(__GNUC__)
#pragma GCC pop_options
#endif
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
  /* UserApp example keys */
#if ( LORAMAC_CLASSB_ENABLED == 1 )
  (kms_obj_keyhead_t *) &Lorawan_Zero_Key,           /* Index = 1 */
#else
  (kms_obj_keyhead_t *) NULL,                        /* Index = 1 */
#endif /* LORAMAC_CLASSB_ENABLED == 1 */
  (kms_obj_keyhead_t *) &Lorawan_App_Key,            /* Index = 2 */
  (kms_obj_keyhead_t *) &Lorawan_Nwk_Key,            /* Index = 3 */
  (kms_obj_keyhead_t *) &Lorawan_DevJoinEuiAddr_Key, /* Index = 4 */
  (kms_obj_keyhead_t *) &Lorawan_Nwk_S_Key,          /* Index = 5 */
  (kms_obj_keyhead_t *) &Lorawan_App_S_Key,          /* Index = 6 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 7 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 8 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 9 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 10 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 11 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 12 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 13 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 14 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 15 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 16 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 17 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 18 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 19 */
  (kms_obj_keyhead_t *) NULL,       /* Index = 20 */
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

