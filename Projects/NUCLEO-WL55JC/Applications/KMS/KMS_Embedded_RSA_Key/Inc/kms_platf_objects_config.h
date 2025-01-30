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
KMS_DECLARE_BLOB_STRUCT(,256)

/* These objects are used by user tKMS application                 */
const kms_obj_keyhead_256_t   Test_Key_RSA_2048 =
{
  KMS_ABI_VERSION_CK_2_40,         /*   uint32_t version;           */
  KMS_ABI_CONFIG_KEYHEAD,          /*   uint32_t configuration;     */
  900,                             /*   uint32_t blobs_size;        */
  11,                              /*   uint32_t blobs_count;       */
  1,                               /*   uint32_t object_id;         */
  {
    CKA_CLASS,        sizeof(CK_OBJECT_CLASS), CKO_PRIVATE_KEY,
    CKA_KEY_TYPE,     sizeof(CK_KEY_TYPE), CKK_RSA,
    CKA_TOKEN,        4, CK_TRUE,
    CKA_LABEL,        16, 0x52455355U, 0x424F4C42U, 0x20415352U, 0x38343032U, /* 'USER', 'BLOB', 'RSA ', '2048' */
    CKA_ID,           4, 123,
    CKA_SENSITIVE,    4, CK_TRUE,
    CKA_VERIFY,       4, CK_TRUE,
    CKA_SIGN,         4, CK_TRUE,

    CKA_MODULUS,      256,
                      0xcea80475U, 0x324c1dc8U, 0x34782781U, 0x8da58bacU,
                      0x069d3419U, 0xc614a6eaU, 0x1ac6a3b5U, 0x10dcd72cU,
                      0xc5169549U, 0x05e9fef9U, 0x08d45e13U, 0x006adf27U,
                      0xd467a7d8U, 0x3c111d1aU, 0x5df15ef2U, 0x93771aefU,
                      0xb920032aU, 0x5bb989f8U, 0xe4f5e1b0U, 0x5093d3f1U,
                      0x30f984c0U, 0x7a772a36U, 0x83f4dc6fU, 0xb28a9681U,
                      0x5b32123cU, 0xcdd13954U, 0xf19d5b8bU, 0x24a103e7U,
                      0x71a34c32U, 0x8755c65eU, 0xd64e1924U, 0xffd04d30U,
                      0xb2142cc2U, 0x62f6e004U, 0x8fef6dbcU, 0x652f2147U,
                      0x9ea1c4b1U, 0xd66d28f4U, 0xd46ef718U, 0x5e390cbfU,
                      0xa2e02380U, 0x582f3188U, 0xbb94ebbfU, 0x05d31487U,
                      0xa09aff01U, 0xfcbb4cd4U, 0xbfd1f0a8U, 0x33b38c11U,
                      0x813c8436U, 0x0bb53c7dU, 0x4481031cU, 0x40bad871U,
                      0x3bb6b835U, 0xcb08098eU, 0xd15ba31eU, 0xe4ba728aU,
                      0x8c8e10f7U, 0x294e1b41U, 0x63b7aee5U, 0x7277bfd8U,
                      0x81a6f9d4U, 0x3e02c692u, 0x5aa3a043U, 0xfb7fb78dU,

    CKA_PUBLIC_EXPONENT, 256,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U,
                      0x00000000U, 0x00000000U, 0x00000000U, 0x00260445U,


    CKA_PRIVATE_EXPONENT, 256,
                      0x0997634cU, 0x477c1a03U, 0x9d44c810U, 0xb2aaa3c7U,
                      0x862b0b88U, 0xd3708272U, 0xe1e15f66U, 0xfc938970U,
                      0x9f8a11f3U, 0xea6a5af7U, 0xeffa2d01U, 0xc189c50fU,
                      0x0d5bcbe3U, 0xfa272e56U, 0xcfc4a4e1U, 0xd388a9dcU,
                      0xd65df862U, 0x8902556cU, 0x8b6bb6a6U, 0x41709b5aU,
                      0x35dd2622U, 0xc73d4640U, 0xbfa1359dU, 0x0e76e1f2U,
                      0x19f8e33eU, 0xb9bd0b59U, 0xec198eb2U, 0xfccaae03U,
                      0x46bd8b40U, 0x1e12e3c6U, 0x7cb62956U, 0x9c185a2eU,
                      0x0f35a2f7U, 0x41644c1cU, 0xca5ebb13U, 0x9d77a89aU,
                      0x2953fc5eU, 0x30048c0eU, 0x619f07c8U, 0xd21d1e56U,
                      0xb8af0719U, 0x3d0fdf3fU, 0x49cd49f2U, 0xef3138b5U,
                      0x138862f1U, 0x470bd2d1U, 0x6e34a2b9U, 0xe7777a6cU,
                      0x8c8d4cb9U, 0x4b4e8b5dU, 0x616cd539U, 0x3753e7b0U,
                      0xf31cc7daU, 0x559ba8e9U, 0x8d888914U, 0xe334773bU,
                      0xaf498ad8U, 0x8d9631ebU, 0x5fe32e53U, 0xa4145bf0U,
                      0xba548bf2U, 0xb0a50c63U, 0xf67b14e3U, 0x98a34b0dU,
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
  (kms_obj_keyhead_t *) &Test_Key_RSA_2048,             /* Index = 1 */
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

