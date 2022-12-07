/**
  ******************************************************************************
  * @file    se_crypto_config.h
  * @author  MCD Application Team
  *          This file provides a template to configure the crypto scheme used
  *          for the services used by the bootloader (crypto operations for the
  *          Firmware and Metadata).
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
#ifndef SE_CRYPTO_CONFIG_H
#define SE_CRYPTO_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SE Secure Engine
  * @{
  */

/** @addtogroup SE_CORE SE Core
  * @{
  */

/** @addtogroup SE_CRYPTO SE Crypto
  * @{
  */

/** @defgroup SE_CRYPTO_CONFIG SE Crypto scheme configuration
  * @brief Choose the cryptographic scheme you want the bootloader to use.
  *        These settings apply to the services used by the bootloader only, not to the services called by the User
  *        Application.
  * @{
  */

/**
  * SE_CoreBin instantiates the crypto scheme selected thanks to "SECBOOT_CRYPTO_SCHEME".
  * "SECBOOT_CRYPTO_SCHEME" is composed as indicated below:
  *        SECBOOT_authenticationalgo_encryptionalgo_MACalgo
  *
  * Set "SECBOOT_CRYPTO_SCHEME" to one of the values listed below:
  *
  * SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256: no FW encryption, only Authentication and Integrity are ensured.
  *                           Authenticate Firmware Metadata using SHA256 signed with ECC DSA.
  *                           No Firmware encryption.
  *                           Firmware Image Integrity using SHA256 on clear Firmware (SHA256 stored in the
  *                           authenticated metadata).
  *
  * SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256: Authentication, Integrity, Confidentiality are ensured.
  *                           Authenticate Firmware Metadata using SHA256 signed with ECC DSA.
  *                           Decrypt Firmware Image with AES128-CBC.
  *                           Firmware Image Integrity using SHA256 on clear Firmware (SHA256 stored in the
  *                           authenticated metadata).
  *
  * SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM: Authentication, Integrity, Confidentiality are ensured.
  *                           Authenticate Firmware Metadata using AES128-GCM tag.
  *                           Decrypt Firmware image using AES128-GCM algo.
  *                           Firmware Image Integrity using AES128-GCM tag.
  */
#define SECBOOT_CRYPTO_SCHEME SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256     /*!< Selected Crypto Scheme */

#define SECBOOT_ECCDSA_WITHOUT_ENCRYPT_SHA256        (1U) /*!< asymmetric crypto, no FW encryption           */
#define SECBOOT_ECCDSA_WITH_AES128_CBC_SHA256        (2U) /*!< asymmetric crypto with encrypted Firmware     */
#define SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM     (3U) /*!< symmetric crypto                              */
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

#endif /* SE_CRYPTO_CONFIG_H */
