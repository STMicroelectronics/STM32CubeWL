/**
  ******************************************************************************
  * @file    ca_conf.h
  * @author  MCD Application Team
  * @brief   This file contains configuration for Cryptographic API (CA)
  *          module functionalities.
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
#ifndef CA_CONF_H
#define CA_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Cryptographic_API Cryptographic API (CA)
  * @{
  */

/** @defgroup CA_Configuration Global configuration
  * @{
  */

/** @defgroup CA_Crypto_Libraries Cryptographic Libraries
  * @{
  */

/**
  * @brief Define CA_ST_CRYPTOLIB_SUPP to use services from ST Cryptographic firmware Library
  * @note  More details on this library can be found here www.st.com/en/embedded-software/x-cube-cryptolib.html
  */
/*#define CA_ST_CRYPTOLIB_SUPP*/

/**
  * @brief Define CA_MBED_CRYPTOLIB_SUPP to use services from the Mbed Cryptography Library
  * @note  More details on this library can be found here github.com/ARMmbed/mbed-crypto
  */
#define CA_MBED_CRYPTOLIB_SUPP

/**
  * @brief Define CA_HAL_CRYPTOLIB_SUPP to use services from the STM32Cube HAL drivers
  * @note  HAL drivers can be used to access cryptographic services of specific IP peripherals
  *        Supported IP peripherals are CRYP & PKA
  */
#define CA_HAL_CRYPTOLIB_SUPP


/**
  * @}
  */

/** @defgroup CA_Features_Config Supported Features
  * @{
  */

/**
  * @brief Define CA_FEAT_RNG to support RNG cryptographic algorithm
  */
/* #define CA_FEAT_RNG */

/**
  * @}
  */

/**
  * @brief Define the below line to support AES cryptographic algorithm
  */
#define CA_FEAT_AES

#if defined(CA_FEAT_AES)
/** @defgroup CA_AES_Features_Config AES Feature Configuration
  * @{
  */

/**
  * @brief Define CA_FEAT_AES_ENCRYPT to support AES cryptographic algorithm encryption
  */
#define CA_FEAT_AES_ENCRYPT

/**
  * @brief Define CA_FEAT_AES_DECRYPT to support AES cryptographic algorithm decryption
  */
#define CA_FEAT_AES_DECRYPT

/**
  * @brief Define CA_FEAT_AES_128BITS to support AES cryptographic algorithm on 128 bits length keys
  */
#define CA_FEAT_AES_128BITS

/**
  * @brief Define CA_FEAT_AES_192BITS to support AES cryptographic algorithm on 192 bits length keys
  */
/* #define CA_FEAT_AES_192BITS */

/**
  * @brief Define CA_FEAT_AES_256BITS to support AES cryptographic algorithm on 256 bits length keys
  */
#define CA_FEAT_AES_256BITS

/**
  * @}
  */
#endif /* CA_FEAT_AES */
/**
  * @brief Define CA_FEAT_HASH to support HASH cryptographic algorithm
  */
#define CA_FEAT_HASH

/**
  * @}
  */

/**
  * @brief Define CA_FEAT_ECC to support ECC cryptographic algorithm
  */
#define CA_FEAT_ECC

/**
  * @}
  */

#if defined(CA_FEAT_ECC)
/** @defgroup CA_ECC_Features_Config ECC Feature Configuration
  * @{
  */

/**
  * @brief Define CA_FEAT_ECC_VERIFY to support ECC cryptographic algorithm verification
  */
#define CA_FEAT_ECC_VERIFY


/**
  * @}
  */
#endif /* CA_FEAT_ECC */

/**
  * @brief Define CA_FEAT_RSA to support RSA cryptographic algorithm
  */
/* #define CA_FEAT_RSA */

/**
  * @}
  */

#if defined(CA_FEAT_RSA)
/** @defgroup CA_ECC_Features_Config ECC Feature Configuration
  * @{
  */

/**
  * @brief Define CA_FEAT_RSA_SIGN to support RSA cryptographic algorithm signature
  */
#define CA_FEAT_RSA_SIGN

/**
  * @brief Define CA_FEAT_RSA_VERIFY to support RSA cryptographic algorithm verification
  */
#define CA_FEAT_RSA_VERIFY

/**
  * @brief Define CA_FEAT_RSA_1024BITS to support RSA cryptographic algorithm on 1024 bits length modulus
  */
#define CA_FEAT_RSA_1024BITS

/**
  * @brief Define CA_FEAT_RSA_2048BITS to support RSA cryptographic algorithm on 2048 bits length modulus
  */
#define CA_FEAT_RSA_2048BITS

/**
  * @}
  */
#endif /* CA_FEAT_RSA */

/** @defgroup CA_Router_Config Algorithms Router & Configuration
  * @{
  */

/** @defgroup CA_Router_Config_Def Router defines
  * @note     Masks & Defines to use to configure algorithm routing options
  * @{
  */

/**
  * @brief Use CA_ROUTE_MASK to extract configured route from algorithm configuration definition
  */
#define CA_ROUTE_MASK   (0xFUL)

/**
  * @brief Use CA_ROUTE_ST to make the configured algorithm using ST Cryptographic firmware Library
  * @note  See @ref CA_ST_CRYPTOLIB_SUPP for more details
  */
#if defined(CA_ST_CRYPTOLIB_SUPP)
#define CA_ROUTE_ST     (1UL)
#else /* CA_ST_CRYPTOLIB_SUPP */
#define CA_ROUTE_ST     (0UL)
#endif /* CA_ST_CRYPTOLIB_SUPP */

/**
  * @brief Use CA_ROUTE_MBED to make the configured algorithm using Mbed Cryptography Library
  * @note  See @ref CA_MBED_CRYPTOLIB_SUPP for more details
  */
#if defined(CA_MBED_CRYPTOLIB_SUPP)
#define CA_ROUTE_MBED   (2UL)
#else /* CA_MBED_CRYPTOLIB_SUPP */
#define CA_ROUTE_MBED   (0UL)
#endif /* CA_MBED_CRYPTOLIB_SUPP */

/**
  * @brief Use CA_ROUTE_HAL to make the configured algorithm using STM32Cube HAL drivers
  * @note  See @ref CA_HAL_CRYPTOLIB_SUPP for more details
  */
#if defined(CA_HAL_CRYPTOLIB_SUPP)
#define CA_ROUTE_HAL    (3UL)
#else /* CA_HAL_CRYPTOLIB_SUPP */
#define CA_ROUTE_HAL    (0UL)
#endif /* CA_HAL_CRYPTOLIB_SUPP */

#if !defined(CA_ROUTE_DEFAULT)
/**
  * @brief Use CA_ROUTE_DEFAULT to choose default route for the configured algorithm
  * @note  Default is chosen as below described:
  *        - If CA_ST_CRYPTOLIB_SUPP is defined, default route is CA_ROUTE_ST
  *        - else if CA_MBED_CRYPTOLIB_SUPP is defined, default route is CA_ROUTE_MBED
  *        - else if CA_HAL_CRYPTOLIB_SUPP is defined, default route is CA_ROUTE_HAL
  */
#if defined(CA_ST_CRYPTOLIB_SUPP)
#define CA_ROUTE_DEFAULT  CA_ROUTE_ST
#elif defined(CA_MBED_CRYPTOLIB_SUPP)
#define CA_ROUTE_DEFAULT  CA_ROUTE_MBED
#elif defined(CA_HAL_CRYPTOLIB_SUPP)
#define CA_ROUTE_DEFAULT  CA_ROUTE_HAL
#else /* CA_ST_CRYPTOLIB_SUPP */
#endif /* CA_ST_CRYPTOLIB_SUPP */
#endif /* CA_ROUTE_DEFAULT */

/**
  * @}
  */
#if defined(CA_FEAT_RNG)
/** @defgroup CA_Router_Config_RNG_Def RNG Defines
  * @note     Masks & Defines to use to configure RNG algorithm supported features
  * @{
  */

/**
  * @brief RNG configuration
  * @note  Define CA_ROUTE_RNG to enable RNG algorithm
  * @note  This define is a bit mask that includes 1 kind of information
  *        - The cryptographic library to use to serve this algorithm (choose one of the below elements)
  *          * @ref CA_ROUTE_ST      Use ST cryptographic library
  *          * @ref CA_ROUTE_MBED    Use MBED cryptographic library
  *          * @ref CA_ROUTE_HAL     Use HAL drivers
  * @note  You can use @ref CA_ROUTE_DEFAULT to use default route
  */
#define CA_ROUTE_RNG  (CA_ROUTE_DEFAULT)

/**
  * @}
  */
#endif /* CA_FEAT_RNG */

#if defined(CA_FEAT_AES)
/** @defgroup CA_Router_Config_AES_Def AES Defines
  * @note     Masks & Defines to use to configure AES algorithm supported features
  * @{
  */

/**
  * @brief Use CA_ROUTE_AES_CFG_ENCRYPT_ENABLE to enable encryption for the configured AES algorithm
  * @note  Requires @ref CA_FEAT_AES_ENCRYPT to be defined
  */
#if defined(CA_FEAT_AES_ENCRYPT)
#define CA_ROUTE_AES_CFG_ENCRYPT_ENABLE     (1UL << 8)
#else /* CA_FEAT_AES_ENCRYPT */
#define CA_ROUTE_AES_CFG_ENCRYPT_ENABLE     (0UL)
#endif /* CA_FEAT_AES_ENCRYPT */
/**
  * @brief Use CA_ROUTE_AES_CFG_DECRYPT_ENABLE to enable decryption for the configured AES algorithm
  * @note  Requires @ref CA_FEAT_AES_DECRYPT to be defined
  */
#if defined(CA_FEAT_AES_DECRYPT)
#define CA_ROUTE_AES_CFG_DECRYPT_ENABLE     (1UL << 9)
#else /* CA_FEAT_AES_DECRYPT */
#define CA_ROUTE_AES_CFG_DECRYPT_ENABLE     (0UL)
#endif /* CA_FEAT_AES_DECRYPT */

/**
  * @brief Use CA_ROUTE_AES_CFG_128BITS_ENABLE to enable 128 bits key length for the configured AES algorithm
  * @note  Requires @ref CA_FEAT_AES_128BITS to be defined
  */
#if defined(CA_FEAT_AES_128BITS)
#define CA_ROUTE_AES_CFG_128BITS_ENABLE     (1UL << 10)
#else /* CA_FEAT_AES_128BITS */
#define CA_ROUTE_AES_CFG_128BITS_ENABLE     (0UL)
#endif /* CA_FEAT_AES_128BITS */
/**
  * @brief Use CA_ROUTE_AES_CFG_192BITS_ENABLE to enable 128 bits key length for the configured AES algorithm
  * @note  Requires @ref CA_FEAT_AES_192BITS to be defined
  */
#if defined(CA_FEAT_AES_192BITS)
#define CA_ROUTE_AES_CFG_192BITS_ENABLE     (1UL << 11)
#else /* CA_FEAT_AES_192BITS */
#define CA_ROUTE_AES_CFG_192BITS_ENABLE     (0UL)
#endif /* CA_FEAT_AES_192BITS */
/**
  * @brief Use CA_ROUTE_AES_CFG_256BITS_ENABLE to enable 128 bits key length for the configured AES algorithm
  * @note  Requires @ref CA_FEAT_AES_256BITS to be defined
  */
#if defined(CA_FEAT_AES_256BITS)
#define CA_ROUTE_AES_CFG_256BITS_ENABLE     (1UL << 12)
#else /* CA_FEAT_AES_256BITS */
#define CA_ROUTE_AES_CFG_256BITS_ENABLE     (0UL)
#endif /* CA_FEAT_AES_256BITS */

#if !defined(CA_ROUTE_AES_CFG_DEFAULT)
/**
  * @brief Use CA_ROUTE_AES_CFG_DEFAULT to choose default settings for the configured AES algorithm
  * @note  Default is: encryption & decryption using 128, 192 & 256 bits lengths keys enabled
  */
#define CA_ROUTE_AES_CFG_DEFAULT  (CA_ROUTE_AES_CFG_ENCRYPT_ENABLE  \
                                   | CA_ROUTE_AES_CFG_DECRYPT_ENABLE \
                                   | CA_ROUTE_AES_CFG_128BITS_ENABLE \
                                   | CA_ROUTE_AES_CFG_192BITS_ENABLE \
                                   | CA_ROUTE_AES_CFG_256BITS_ENABLE)
#endif /* CA_ROUTE_AES_CFG_DEFAULT */

/**
  * @}
  */

/** @defgroup CA_Router_Config_AES AES Algorithms configurations
  * @{
  */

/**
  * @brief AES CBC configuration
  * @note  Define CA_ROUTE_AES_CBC to enable AES CBC algorithm
  * @note  This define is a bit mask that includes 2 kinds of information
  *        - The cryptographic library to use to serve this algorithm (choose one of the below elements)
  *          * @ref CA_ROUTE_ST      Use ST cryptographic library
  *          * @ref CA_ROUTE_MBED    Use MBED cryptographic library
  *          * @ref CA_ROUTE_HAL     Use HAL drivers
  *        - The algorithm specific configuration (use a combination of the below elements)
  *          * @ref CA_ROUTE_AES_CFG_ENCRYPT_ENABLE    Encryption is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_DECRYPT_ENABLE    Decryption is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_128BITS_ENABLE    128 bits key length is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_192BITS_ENABLE    192 bits key length is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_256BITS_ENABLE    256 bits key length is enabled for this algorithm
  * @note  You can use @ref CA_ROUTE_DEFAULT to use default route
  * @note  You can use @ref CA_ROUTE_AES_CFG_DEFAULT to use default AES algorithm configuration
  */
#define CA_ROUTE_AES_CBC  (CA_ROUTE_HAL | CA_ROUTE_AES_CFG_DEFAULT)

/**
  * @brief AES CCM configuration
  * @note  Define CA_ROUTE_AES_CCM to enable AES CCM algorithm
  * @note  This define is a bit mask that includes 2 kinds of information
  *        - The cryptographic library to use to serve this algorithm (choose one of the below elements)
  *          * @ref CA_ROUTE_ST      Use ST cryptographic library
  *          * @ref CA_ROUTE_MBED    Use MBED cryptographic library
  *          * @ref CA_ROUTE_HAL     Use HAL drivers
  *        - The algorithm specific configuration (use a combination of the below elements)
  *          * @ref CA_ROUTE_AES_CFG_ENCRYPT_ENABLE    Encryption is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_DECRYPT_ENABLE    Decryption is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_128BITS_ENABLE    128 bits key length is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_192BITS_ENABLE    192 bits key length is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_256BITS_ENABLE    256 bits key length is enabled for this algorithm
  * @note  You can use @ref CA_ROUTE_DEFAULT to use default route
  * @note  You can use @ref CA_ROUTE_AES_CFG_DEFAULT to use default AES algorithm configuration
  */
#define CA_ROUTE_AES_CCM  (CA_ROUTE_HAL | CA_ROUTE_AES_CFG_DEFAULT)

/**
  * @brief AES CMAC configuration
  * @note  Define CA_ROUTE_AES_CMAC to enable AES CMAC algorithm
  * @note  This define is a bit mask that includes 2 kinds of information
  *        - The cryptographic library to use to serve this algorithm (choose one of the below elements)
  *          * @ref CA_ROUTE_ST      Use ST cryptographic library
  *          * @ref CA_ROUTE_MBED    Use MBED cryptographic library
  *          * @ref CA_ROUTE_HAL     Use HAL drivers
  *        - The algorithm specific configuration (use a combination of the below elements)
  *          * @ref CA_ROUTE_AES_CFG_ENCRYPT_ENABLE    Encryption is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_DECRYPT_ENABLE    Decryption is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_128BITS_ENABLE    128 bits key length is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_192BITS_ENABLE    192 bits key length is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_256BITS_ENABLE    256 bits key length is enabled for this algorithm
  * @note  You can use @ref CA_ROUTE_DEFAULT to use default route
  * @note  You can use @ref CA_ROUTE_AES_CFG_DEFAULT to use default AES algorithm configuration
  */
#define CA_ROUTE_AES_CMAC  (CA_ROUTE_HAL | CA_ROUTE_AES_CFG_DEFAULT)

/**
  * @brief AES ECB configuration
  * @note  Define CA_ROUTE_AES_ECB to enable AES ECB algorithm
  * @note  This define is a bit mask that includes 2 kinds of information
  *        - The cryptographic library to use to serve this algorithm (choose one of the below elements)
  *          * @ref CA_ROUTE_ST      Use ST cryptographic library
  *          * @ref CA_ROUTE_MBED    Use MBED cryptographic library
  *          * @ref CA_ROUTE_HAL     Use HAL drivers
  *        - The algorithm specific configuration (use a combination of the below elements)
  *          * @ref CA_ROUTE_AES_CFG_ENCRYPT_ENABLE    Encryption is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_DECRYPT_ENABLE    Decryption is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_128BITS_ENABLE    128 bits key length is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_192BITS_ENABLE    192 bits key length is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_256BITS_ENABLE    256 bits key length is enabled for this algorithm
  * @note  You can use @ref CA_ROUTE_DEFAULT to use default route
  * @note  You can use @ref CA_ROUTE_AES_CFG_DEFAULT to use default AES algorithm configuration
  */
#define CA_ROUTE_AES_ECB  (CA_ROUTE_HAL | CA_ROUTE_AES_CFG_DEFAULT)

/**
  * @brief AES GCM configuration
  * @note  Define CA_ROUTE_AES_GCM to enable AES GCM algorithm
  * @note  This define is a bit mask that includes 2 kinds of information
  *        - The cryptographic library to use to serve this algorithm (choose one of the below elements)
  *          * @ref CA_ROUTE_ST      Use ST cryptographic library
  *          * @ref CA_ROUTE_MBED    Use MBED cryptographic library
  *          * @ref CA_ROUTE_HAL     Use HAL drivers
  *        - The algorithm specific configuration (use a combination of the below elements)
  *          * @ref CA_ROUTE_AES_CFG_ENCRYPT_ENABLE    Encryption is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_DECRYPT_ENABLE    Decryption is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_128BITS_ENABLE    128 bits key length is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_192BITS_ENABLE    192 bits key length is enabled for this algorithm
  *          * @ref CA_ROUTE_AES_CFG_256BITS_ENABLE    256 bits key length is enabled for this algorithm
  * @note  You can use @ref CA_ROUTE_DEFAULT to use default route
  * @note  You can use @ref CA_ROUTE_AES_CFG_DEFAULT to use default AES algorithm configuration
  */
#define CA_ROUTE_AES_GCM  (CA_ROUTE_HAL | CA_ROUTE_AES_CFG_DEFAULT)

/**
  * @}
  */
#endif /* CA_FEAT_AES */

#if defined(CA_FEAT_HASH)
/** @defgroup CA_Router_Config_HASH_Def HASH Defines
  * @note     Masks & Defines to use to configure HASH algorithm supported features
  * @{
  */

/**
  * @brief HASH SHA1 configuration
  * @note  Define CA_ROUTE_HASH_SHA1 to enable HASH SHA1 algorithm
  * @note  This define is a bit mask that includes 1 kind of information
  *        - The cryptographic library to use to serve this algorithm (choose one of the below elements)
  *          * @ref CA_ROUTE_ST      Use ST cryptographic library
  *          * @ref CA_ROUTE_MBED    Use MBED cryptographic library
  *          * @ref CA_ROUTE_HAL     Use HAL drivers
  * @note  You can use @ref CA_ROUTE_DEFAULT to use default route
  */
/* #define CA_ROUTE_HASH_SHA1  (CA_ROUTE_DEFAULT) */

/**
  * @brief HASH SHA256 configuration
  * @note  Define CA_ROUTE_HASH_SHA256 to enable HASH SHA256 algorithm
  * @note  This define is a bit mask that includes 1 kind of information
  *        - The cryptographic library to use to serve this algorithm (choose one of the below elements)
  *          * @ref CA_ROUTE_ST      Use ST cryptographic library
  *          * @ref CA_ROUTE_MBED    Use MBED cryptographic library
  *          * @ref CA_ROUTE_HAL     Use HAL drivers
  * @note  You can use @ref CA_ROUTE_DEFAULT to use default route
  */
#define CA_ROUTE_HASH_SHA256  (CA_ROUTE_MBED)

/**
  * @}
  */
#endif /* CA_FEAT_HASH */

#if defined(CA_FEAT_ECC)
/** @defgroup CA_Router_Config_ECC_Def ECC Defines
  * @note     Masks & Defines to use to configure ECC algorithm supported features
  * @{
  */

/**
  * @brief Use CA_ROUTE_ECC_CFG_VERIFY_ENABLE to enable verification for the configured ECC algorithm
  * @note  Requires @ref CA_FEAT_ECC_VERIFY to be defined
  */
#if defined(CA_FEAT_ECC_VERIFY)
#define CA_ROUTE_ECC_CFG_VERIFY_ENABLE     (1UL << 9)
#else /* CA_FEAT_ECC_VERIFY */
#define CA_ROUTE_ECC_CFG_VERIFY_ENABLE     (0UL)
#endif /* CA_FEAT_ECC_VERIFY */


#if !defined(CA_ROUTE_ECC_CFG_DEFAULT)
/**
  * @brief Use CA_ROUTE_ECC_CFG_DEFAULT to choose default settings for the configured ECC algorithm
  * @note  Default is: signature & verification enabled
  */
#define CA_ROUTE_ECC_CFG_DEFAULT  (CA_ROUTE_ECC_CFG_VERIFY_ENABLE)
#endif /* CA_ROUTE_ECC_CFG_DEFAULT */

/**
  * @brief ECC ECDSA configuration
  * @note  Define CA_ROUTE_ECC_ECDSA to enable ECC ECDSA algorithm
  * @note  Requires CA_ROUTE_RNG to be defined
  * @note  This define is a bit mask that includes 2 kinds of information
  *        - The cryptographic library to use to serve this algorithm (choose one of the below elements)
  *          * @ref CA_ROUTE_ST      Use ST cryptographic library
  *          * @ref CA_ROUTE_MBED    Use MBED cryptographic library
  *          * @ref CA_ROUTE_HAL     Use HAL drivers

  *        - The algorithm specific configuration (use a combination of the below elements)
  *          * @ref CA_ROUTE_ECC_CFG_VERIFY_ENABLE    Verification is enabled for this algorithm
  * @note  You can use @ref CA_ROUTE_DEFAULT to use default route
  * @note  You can use @ref CA_ROUTE_ECC_CFG_DEFAULT to use default ECC algorithm configuration
  */
#define CA_ROUTE_ECC_ECDSA  (CA_ROUTE_HAL | CA_ROUTE_ECC_CFG_DEFAULT)


/**
  * @}
  */
#endif /* CA_FEAT_ECC */


#if defined(CA_FEAT_RSA)
/** @defgroup CA_Router_Config_RSA_Def RSA Defines
  * @note     Masks & Defines to use to configure RSA algorithm supported features
  * @{
  */

/**
  * @brief Use CA_ROUTE_RSA_CFG_SIGN_ENABLE to enable signature for the configured RSA algorithm
  * @note  Requires @ref CA_FEAT_RSA_SIGN to be defined
  */
#if defined(CA_FEAT_RSA_SIGN)
#define CA_ROUTE_RSA_CFG_SIGN_ENABLE     (1UL << 8)
#else /* CA_FEAT_RSA_SIGN */
#define CA_ROUTE_RSA_CFG_SIGN_ENABLE     (0UL)
#endif /* CA_FEAT_RSA_SIGN */

/**
  * @brief Use CA_ROUTE_RSA_CFG_VERIFY_ENABLE to enable verification for the configured RSA algorithm
  * @note  Requires @ref CA_FEAT_RSA_VERIFY to be defined
  */
#if defined(CA_FEAT_RSA_VERIFY)
#define CA_ROUTE_RSA_CFG_VERIFY_ENABLE     (1UL << 9)
#else /* CA_FEAT_RSA_VERIFY */
#define CA_ROUTE_RSA_CFG_VERIFY_ENABLE     (0UL)
#endif /* CA_FEAT_RSA_VERIFY */

/**
  * @brief Use CA_ROUTE_RSA_CFG_1024BITS_ENABLE to enable support of 1024 bits length modulus for the configured
  *        RSA algorithm
  * @note  Requires @ref CA_FEAT_RSA_1024BITS to be defined
  */
#if defined(CA_FEAT_RSA_1024BITS)
#define CA_ROUTE_RSA_CFG_1024BITS_ENABLE     (1UL << 12)
#else /* CA_FEAT_RSA_1024BITS */
#define CA_ROUTE_RSA_CFG_1024BITS_ENABLE     (0UL)
#endif /* CA_FEAT_RSA_1024BITS */

/**
  * @brief Use CA_ROUTE_RSA_CFG_2048BITS_ENABLE to enable support of 2048 bits length modulus for the configured
  *        RSA algorithm
  * @note  Requires @ref CA_FEAT_RSA_2048BITS to be defined
  */
#if defined(CA_FEAT_RSA_2048BITS)
#define CA_ROUTE_RSA_CFG_2048BITS_ENABLE     (1UL << 13)
#else /* CA_FEAT_RSA_2048BITS */
#define CA_ROUTE_RSA_CFG_2048BITS_ENABLE     (0UL)
#endif /* CA_FEAT_RSA_2048BITS */


#if !defined(CA_ROUTE_RSA_CFG_DEFAULT)
/**
  * @brief Use CA_ROUTE_RSA_CFG_DEFAULT to choose default settings for the configured RSA algorithm
  * @note  Default is: signature & verification enabled
  */
#define CA_ROUTE_RSA_CFG_DEFAULT  (CA_ROUTE_RSA_CFG_SIGN_ENABLE\
                                   | CA_ROUTE_RSA_CFG_VERIFY_ENABLE\
                                   | CA_ROUTE_RSA_CFG_1024BITS_ENABLE\
                                   | CA_ROUTE_RSA_CFG_2048BITS_ENABLE)
#endif /* CA_ROUTE_RSA_CFG_DEFAULT */

/**
  * @brief RSA configuration
  * @note  Define CA_ROUTE_RSA to enable RSA algorithm
  * @note  This define is a bit mask that includes 2 kinds of information
  *        - The cryptographic library to use to serve this algorithm (choose one of the below elements)
  *          * @ref CA_ROUTE_ST      Use ST cryptographic library
  *          * @ref CA_ROUTE_MBED    Use MBED cryptographic library
  *          * @ref CA_ROUTE_HAL     Use HAL drivers

  *        - The algorithm specific configuration (use a combination of the below elements)
  *          * @ref CA_ROUTE_RSA_CFG_SIGN_ENABLE      Signature is enabled for this algorithm
  *          * @ref CA_ROUTE_RSA_CFG_VERIFY_ENABLE    Verification is enabled for this algorithm
  *          * @ref CA_ROUTE_RSA_CFG_1024BITS_ENABLE  1024 bits length modulus is enabled for this algorithm
  *          * @ref CA_ROUTE_RSA_CFG_2048BITS_ENABLE  2048 bits length modulus is enabled for this algorithm
  * @note  You can use @ref CA_ROUTE_DEFAULT to use default route
  * @note  You can use @ref CA_ROUTE_RSA_CFG_DEFAULT to use default RSA algorithm configuration
  */
#define CA_ROUTE_RSA  (CA_ROUTE_HAL | CA_ROUTE_RSA_CFG_DEFAULT)


/**
  * @}
  */
#endif /* CA_FEAT_RSA */

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

#endif /* CA_CONF_H */
