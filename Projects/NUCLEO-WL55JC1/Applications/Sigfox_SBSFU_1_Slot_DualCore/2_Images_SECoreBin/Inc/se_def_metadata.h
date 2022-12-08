/**
  ******************************************************************************
  * @file    se_def_metadata.h
  * @author  MCD Application Team
  * @brief   This file contains metadata definitions for SE functionalities.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SE_DEF_METADATA_H
#define SE_DEF_METADATA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "se_crypto_config.h"

/** @addtogroup  SE Secure Engine
  * @{
  */

/** @addtogroup  SE_CORE SE Core
  * @{
  */

/** @addtogroup  SE_CORE_DEF SE Definitions
  * @{
  */

/** @defgroup SE_DEF_METADATA SE Metadata Definitions
  *  @brief definitions related to FW metadata (header).
  * @{
  */

/** @defgroup SE_DEF_METADATA_Exported_Constants Exported Constants
  * @brief  Firmware Image Header (FW metadata) constants
  * @{
  */

#define SE_FW_HEADER_TOT_LEN    (sizeof(SE_FwRawHeaderTypeDef))   /*!< FW INFO header Total Length */
#define SE_FW_HEADER_SIGN_LEN   SE_HEADER_SIGN_LEN                /*!< FW Header signature Len */
#define SE_FW_HEADER_STATE_LEN  (3U * 32U)                        /*!< FW Header state Len */
#define SE_FW_HEADER_FINGERPRINT_LEN  (32U)                       /*!< FW Header finger print Len */
#define SE_FW_AUTH_LEN          (SE_FW_HEADER_TOT_LEN - SE_FW_HEADER_SIGN_LEN  \
                                 - SE_FW_HEADER_STATE_LEN - SE_FW_HEADER_FINGERPRINT_LEN) /*!< Authenticated part of the header */

#define SE_SYMKEY_LEN           (16U)  /*!< SE Symmetric Key length (bytes)*/
#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
/* AES-GCM encryption and FW Tag */
#define SE_NONCE_LEN            (12U)  /*!< Secure Engine Nonce Length (Bytes)*/
#define SE_TAG_LEN              (16U)  /*!< Secure Engine Tag Length (Bytes): AES-GCM for the FW tag */
#define SE_HEADER_SIGN_LEN      (16U)  /*!< Firmware Header signature LEN : AES-GCM for header signature */
#elif ((SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256)\
       || (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256))
/* AES-CBC encryption (or no encryption) and SHA256 for FW tag */
#define SE_IV_LEN               (16U)  /*!< Secure Engine IV Length (Bytes): same size as an AES block*/
#define SE_TAG_LEN              (32U)  /*!< Secure Engine Tag Length (Bytes): SHA-256 for the FW tag */
#define SE_HEADER_SIGN_LEN      (64U)  /*!< Firmware Header signature LEN*/
#define SE_ASYM_PUBKEY_LEN      (64U)  /*!< SE Asymmetric Public Key length (bytes)*/
#else
#error "The current example does not support the selected crypto scheme."
#endif /* SECBOOT_CRYPTO_SCHEME */

/* Image type: complete or partial image */
#define SE_FW_IMAGE_COMPLETE    (0U) /*!< Complete Fw Image */
#define SE_FW_IMAGE_PARTIAL     (1U) /*!< Partial Fw Image */

/*!< FW Magic IDs */
#define SFUM_1 "SFU1"      /*!< Active Slot #1 : first active also referred as master */
#define SFUM_2 "SFU2"      /*!< Active Slot #2 : second active slot */
#define SFUM_3 "SFU3"      /*!< Active Slot #3 : third active slot */

#ifdef ENABLE_IMAGE_STATE_HANDLING
/* SE_FwStateTypeDef - firmware image state type representing the state of the firmware image
 * In the header (SE_FwRawHeaderTypeDef.FwImageState), these states are encoded as follows:
   * FWIMG_STATE_INVALID  : 32 * 0x00, 32 * 0x00, 32 * 0x00
   * FWIMG_STATE_VALID    : 32 * 0xFF, 32 * 0x00, 32 * 0x00
   * FWIMG_STATE_VALID_ALL: 32 * 0xFF, 32 * 0x55, 32 * 0x00
   * FWIMG_STATE_SELFTEST : 32 * 0xFF, 32 * 0xFF, 32 * 0x00
   * FWIMG_STATE_NEW      : 32 * 0xFF, 32 * 0xFF, 32 * 0xFF
 */
typedef enum
{
  FWIMG_STATE_INVALID   = 0U, /* image is not valid (typically selftest has failed) and should not boot */
  FWIMG_STATE_VALID     = 1U, /* image is valid (selftest passed), image OK to boot if signature checks pass */
  FWIMG_STATE_VALID_ALL = 2U, /* master image is valid (selftest passed), all other images are automatically validated */
  FWIMG_STATE_SELFTEST  = 3U, /* self tests to be executed. Image OK to boot one time if signature checks pass */
  FWIMG_STATE_NEW       = 4U, /* new image, has never run on the device */
} SE_FwStateTypeDef;
#endif /* ENABLE_IMAGE_STATE_HANDLING */

/**
  * @}
  */

/** @defgroup SE_DEF_METADATA_Exported_Types Exported Types
  * @{
  */

/**
  * @brief  Firmware Header structure definition
  * @note This structure MUST be called SE_FwRawHeaderTypeDef
  * @note This structure MUST contain a field named 'FwVersion'
  * @note This structure MUST contain a field named 'FwSize'
  * @note This structure MUST contain a field named 'FwTag' (to control integrity of full FW)
  * @note This structure MUST contain a field named 'PartialFwOffset'
  * @note This structure MUST contain a field named 'PartialFwSize'
  * @note This structure MUST contain a field named 'PartialFwTag' (to control integrity of partial FW)
  * @note This structure MUST contain a field named 'HeaderSignature' (to control authentication of the header)
  * @note This structure MUST contain a field named 'FwImageState' (not part of the authenticated header)
  * @note This structure MUST contain a field named 'PrevHeaderFingerprint' (not part of the authenticated header)
  * @note In this example, the header size is always a multiple of 32 to match the FLASH constraint on STM32H7.
  *       We keep this alignment for all platforms (even when the FLASH alignment constraint is another value) to have
  *       one unique header size per crypto scheme.
  * @note In this example, the authenticated header size + the header signature is always 192 bytes (for all crypto
  *       schemes).
  */
#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
typedef struct
{
  uint8_t  SFUMagic[4U];           /*!< SFU Magic 'SFU1' / 'SFU2' / 'SFU3'*/
  uint16_t ProtocolVersion;        /*!< SFU Protocol version*/
  uint16_t FwVersion;              /*!< Firmware version*/
  uint32_t FwSize;                 /*!< Firmware size (bytes)*/
  uint32_t PartialFwOffset;        /*!< Offset (bytes) of partial firmware vs full firmware */
  uint32_t PartialFwSize;          /*!< Size of partial firmware */
  uint8_t  FwTag[SE_TAG_LEN];      /*!< Firmware Tag*/
  uint8_t  PartialFwTag[SE_TAG_LEN];/*!< Partial firmware Tag */
  uint8_t  Nonce[SE_NONCE_LEN];    /*!< Nonce used to encrypt firmware*/
  uint8_t  Reserved[112U];         /*!< Reserved for future use: 112 extra bytes to have a header size of 192 bytes */
  uint8_t  HeaderSignature[SE_HEADER_SIGN_LEN]; /*!< Signature of the full header message */
  uint8_t  FwImageState[3U][32U];  /*!< Firmware image state - see SE_FwStateTypeDef for details */
  uint8_t  PrevHeaderFingerprint[SE_FW_HEADER_FINGERPRINT_LEN]; /*!< Fingerprint of previous FW header (if this is an update, else 32*0x00) */
} SE_FwRawHeaderTypeDef;
#elif (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256)

typedef struct
{
  uint8_t  SFUMagic[4U];           /*!< SFU Magic 'SFU1' / 'SFU2' / 'SFU3'*/
  uint16_t ProtocolVersion;        /*!< SFU Protocol version*/
  uint16_t FwVersion;              /*!< Firmware version*/
  uint32_t FwSize;                 /*!< Firmware size (bytes)*/
  uint32_t PartialFwOffset;        /*!< Offset (bytes) of partial firmware vs full firmware */
  uint32_t PartialFwSize;          /*!< Size of partial firmware */
  uint8_t  FwTag[SE_TAG_LEN];      /*!< Firmware Tag*/
  uint8_t  PartialFwTag[SE_TAG_LEN];/*!< Partial firmware Tag */
  uint8_t  InitVector[SE_IV_LEN];  /*!< IV used to encrypt firmware */
  uint8_t  Reserved[28U];          /*!< Reserved for future use: 28 extra bytes to have a header size of 192 bytes */
  uint8_t  HeaderSignature[SE_HEADER_SIGN_LEN];  /*!< Signature of the full header message */
  uint8_t  FwImageState[3U][32U];  /*!< Firmware image state - see SE_FwStateTypeDef for details */
  uint8_t  PrevHeaderFingerprint[SE_FW_HEADER_FINGERPRINT_LEN]; /*!< Fingerprint of previous FW header (if this is an update, else 32*0x00) */
} SE_FwRawHeaderTypeDef;

#elif (SECBOOT_CRYPTO_SCHEME == SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256)

typedef struct
{
  uint8_t  SFUMagic[4U];           /*!< SFU Magic 'SFU1' / 'SFU2' / 'SFU3'*/
  uint16_t ProtocolVersion;        /*!< SFU Protocol version*/
  uint16_t FwVersion;              /*!< Firmware version*/
  uint32_t FwSize;                 /*!< Firmware size (bytes)*/
  uint32_t PartialFwOffset;        /*!< Offset (bytes) of partial firmware vs full firmware */
  uint32_t PartialFwSize;          /*!< Size of partial firmware */
  uint8_t  FwTag[SE_TAG_LEN];      /*!< Firmware Tag*/
  uint8_t  PartialFwTag[SE_TAG_LEN];/*!< Partial firmware Tag */
  uint8_t  Reserved[44U];          /*!< Reserved for future use: 44 extra bytes to have a header size of 192 bytes */
  uint8_t  HeaderSignature[SE_HEADER_SIGN_LEN];  /*!< Signature of the full header message */
  uint8_t  FwImageState[3U][32U];  /*!< Firmware image state - see SE_FwStateTypeDef for details */
  uint8_t  PrevHeaderFingerprint[SE_FW_HEADER_FINGERPRINT_LEN]; /*!< Fingerprint of previous FW header (if this is an update, else 32*0x00) */
} SE_FwRawHeaderTypeDef;

#else

#error "The current example does not support the selected crypto scheme."

#endif /* SECBOOT_CRYPTO_SCHEME */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* SE_DEF_METADATA_H */
