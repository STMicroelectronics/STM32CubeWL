/**
  ******************************************************************************
  * @file    kms_config.h
  * @author  MCD Application Team
  * @brief   This file contains configuration for Key Management Services (KMS)
  *          module functionalities.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef KMS_CONFIG_H
#define KMS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Key_Management_Services Key Management Services (KMS)
  * @{
  */

/** @defgroup KMS_Configuration Global configuration
  * @{
  */

/** @defgroup KMS_HW_Config Hardware
  * @{
  */

/**
  * @brief KMS_EXT_TOKEN_ENABLED Uncomment the below line to support External Token
  */
/*#define KMS_EXT_TOKEN_ENABLED*/


/**
  * @}
  */

/** @defgroup KMS_Storage_Config Storage
  * @{
  */

/**
  * @brief KMS_NVM_ENABLED Uncomment the below line to support Non Volatile
  *        Memory storage
  * @note  Mandatory to handle runtime objects addition through blob import or
  *        creation through specific KMS services
  * @note  Requires @ref KMS_NVM_SLOT_NUMBERS to be defined
  */
#define KMS_NVM_ENABLED

/**
  * @brief KMS_NVM_DYNAMIC_ENABLED Uncomment the below line to support Non
  *        Volatile Memory storage
  * @note  Mandatory to handle runtime objects creation through specific KMS services
  * @note  Requires @ref KMS_NVM_ENABLED to be defined
  */
//#define KMS_NVM_DYNAMIC_ENABLED

/**
  * @brief KMS_NVM_SLOT_NUMBERS Specify here the number of slots to use in the Non Volatile Memory storage
  * @note  NVM slots correspond to storage slots, it should be at least equal to the number of keys you'll store
  *        into NVM. But if you intend to manipulate objects (create, destroy, derivate...) better use a greater
  *        number to avoid too much erase of the flash sectors containing the NVM storage.
  * @note  Memory impact is a 4 bytes per slot to store slot address in NVM.
  * @note  Mandatory to set if @ref KMS_NVM_ENABLED is defined
  */
#define KMS_NVM_SLOT_NUMBERS                    (40UL)

/**
  * @brief KMS_VM_DYNAMIC_ENABLED Uncomment the below line to support
  *        Volatile Memory storage
  * @note  Mandatory to handle runtime objects creation through specific KMS services
  */
#define KMS_VM_DYNAMIC_ENABLED

/**
  * @brief KMS_VM_SLOT_NUMBERS Specify here the number of slots to use in the Volatile Memory storage
  * @note  VM slots correspond to storage slots, it should be at least equal to the number of keys you'll store
  *        into VM.
  * @note  Memory impact is a 4 bytes per slot to store slot address in VM.
  * @note  Mandatory to set if @ref KMS_VM_DYNAMIC_ENABLED is defined
  */
#define KMS_VM_SLOT_NUMBERS                    (5UL)

/**
  * @}
  */

/** @defgroup KMS_Config Services
  * @{
  */
/**
  * @brief KMS_NB_SESSIONS_MAX Defines the number of sessions KMS will be able
  *        to handle in parallel
  * @note  This value has an impact on memory footprint
  */
#define KMS_NB_SESSIONS_MAX                     (1UL)

/**
  * @}
  */

/** @defgroup KMS_PKCS_Config PKCS#11 services
  * @{
  */

/**
  * @brief KMS_ENCRYPT Uncomment the below line to support encryption services
  * @note  Requires a supported encryption algorithm to be enabled too:
  *        @ref KMS_AES_CBC
  *        @ref KMS_AES_CCM
  *        @ref KMS_AES_ECB
  *        @ref KMS_AES_GCM
  */
#define KMS_ENCRYPT

/**
  * @brief KMS_DECRYPT Uncomment the below line to support decryption services
  * @note  Requires a supported decryption algorithm to be enabled too:
  *        @ref KMS_AES_CBC
  *        @ref KMS_AES_CCM
  *        @ref KMS_AES_ECB
  *        @ref KMS_AES_GCM
  */
#define KMS_DECRYPT

/**
  * @brief KMS_DIGEST Uncomment the below line to support digest services
  * @note  Requires a supported digest algorithm to be enabled too:
  *        @ref KMS_SHA1
  *        @ref KMS_SHA256
  */
#define KMS_DIGEST

/**
  * @brief KMS_SIGN Uncomment the below line to support signature services
  * @note  Requires a supported signature algorithm to be enabled too:
  *        @ref KMS_RSA
  *        @ref KMS_AES_CMAC
  */
#define KMS_SIGN

/**
  * @brief KMS_VERIFY Uncomment the below line to support verification services
  * @note  Requires a supported verification algorithm to be enabled too:
  *        @ref KMS_RSA
  *        @ref KMS_ECDSA
  *        @ref KMS_AES_CMAC
  */
#define KMS_VERIFY

/**
  * @brief KMS_DERIVE_KEY Uncomment the below line to support key derivation services
  * @note  Requires a supported derivation algorithm to be enabled too:
  *        @ref KMS_AES_ECB
  *        @ref KMS_ECDSA
  * @note  Requires @ref KMS_NVM_DYNAMIC_ENABLED to be enabled to store derived key into NVM
  */
#define KMS_DERIVE_KEY

/**
  * @brief KMS_SEARCH Uncomment the below line to support key search or mechanism search services
  */
#define KMS_SEARCH

/**
  * @brief KMS_GENERATE_KEYS Uncomment the below line to support key generation services
  * @note  Requires a supported key generation algorithm to be enabled too:
  *        @ref KMS_ECDSA
  * @note  Requires @ref KMS_NVM_DYNAMIC_ENABLED to be enabled to store generated key into NVM
  */
/*#define KMS_GENERATE_KEYS*/

/**
  * @brief KMS_ATTRIBUTES Uncomment the below line to support attributes manipulation services
  * @note  Requires @ref KMS_NVM_DYNAMIC_ENABLED to be enabled in order to modify attributes
  */
#define KMS_ATTRIBUTES

/**
  * @brief KMS_OBJECTS Uncomment the below line to support objects manipulation services
  * @note  Requires @ref KMS_NVM_DYNAMIC_ENABLED to be enabled to create or delete objects into NVM
  */
#define KMS_OBJECTS

/**
  * @brief KMS_PKCS11_GET_FUNCTION_LIST_SUPPORT Uncomment the below line to support C_GetFunctionList API
  * @note  Disabling this feature allows to save some code footprint
  */
/*#define KMS_PKCS11_GET_FUNCTION_LIST_SUPPORT*/

/**
  * @brief KMS_PKCS11_COMPLIANCE Uncomment the below line to support PKCS11 APIs and implementation that are required
  *        to be compliant with PKCS11 standard
  * @note  Disabling this feature allows to save some code footprint
  */
/*#define KMS_PKCS11_COMPLIANCE*/

/**
  * @brief KMS_NIKMS_ROUTER_BYPASS Uncomment the below line to bypass KMS router and perform direct KMS API calls
  * @note  Only applicable when using 'niKMS' access
  * @note  Enabling this feature allows to save some code footprint, but forbid any use of the external token services
  */
/*#define KMS_NIKMS_ROUTER_BYPASS*/

/**
  * @brief KMS_IMPORT_BLOB Uncomment the below line to use vendor defined blob import features
  * @note  Requires @ref KMS_NVM_ENABLED to store imported keys
  * @note  Requires the following services and algorithm to Authenticate, Verify and decrypt the imported blobs:
  *        @ref KMS_DECRYPT
  *        @ref KMS_AES_CBC
  *        @ref KMS_DIGEST
  *        @ref KMS_SHA256
  *        @ref KMS_VERIFY
  *        @ref KMS_ECDSA
  *        @ref KMS_EC_SECP256
  */
#define KMS_IMPORT_BLOB

/**
  * @brief KMS_IMPORT_BLOB_CHUNK_SIZE Configure the blob importation chunk size
  * @note  This size must be a multiple of 4
  * @note  This size must be greater to at least the blob import header size
  * @note  Requires @ref KMS_IMPORT_BLOB to be enabled
  */
#define KMS_IMPORT_BLOB_CHUNK_SIZE      (512U)

/**
  * @brief KMS_SECURE_COUNTERS Uncomment the below line to support KMS secure counters
  * @note  Mandatory to handle counter objects through specific KMS services
  * @note  Requires @ref KMS_NVM_ENABLED or @ref KMS_VM_DYNAMIC_ENABLED to be defined
  */
#define KMS_SECURE_COUNTERS

/**
  * @brief KMS_ENCRYPT_DECRYPT_BLOB Uncomment the below line to enable the encryption of the blobs within the KMS data storage
  * @note  Requires @ref KMS_NVM_ENABLED to store imported keys
  */
/* #define KMS_ENCRYPT_DECRYPT_BLOB */

/**
  * @}
  */


/** @defgroup KMS_Memory_Config Memory management
  * @{
  */

/**
  * @brief KMS_MEM_USE_HEAP_ALLOCATOR Uncomment the below line to perform dynamic allocation in HEAP
  * @note  KMS will use standard malloc and free APIs
  * @note  Requires a dynamic allocator provided by the toolchain like dlmalloc
  * @note  KMS_MEM_USE_HEAP_ALLOCATOR and KMS_MEM_USE_POOL_ALLOCATOR are mutually exclusive
  */
/*#define KMS_MEM_USE_HEAP_ALLOCATOR*/

/**
  * @brief KMS_MEM_USE_POOL_ALLOCATOR Uncomment the below line to perform dynamic allocation using internal pool
  *        allocator
  * @note  Requires a pool definition in a file called kms_mem_pool_def.h (see kms_mem_pool_def_template.h for example)
  */
#define KMS_MEM_USE_POOL_ALLOCATOR

/**
  * @brief KMS_MEM_USE_CUSTOM_ALLOCATOR Uncomment the below line to perform dynamic
  *        allocation using a custom allocator
  * @note  To use this custom allocator, implement the kms_mem_low_level.c
  *        - void KMS_MemInit(void)
  *        - CK_VOID_PTR KMS_Alloc(CK_SESSION_HANDLE session, size_t size)
  *        - void KMS_Free(CK_SESSION_HANDLE session, CK_VOID_PTR ptr)
  */
/*#define KMS_MEM_USE_CUSTOM_ALLOCATOR*/

/**
  * @brief KMS_MEM_DEBUGGING Uncomment the below line to keep memory profiling information
  * @note  Not usable with @ref KMS_MEM_USE_CUSTOM_ALLOCATOR
  */
#define KMS_MEM_DEBUGGING

/**
  * @brief KMS_MEM_CLEANING Uncomment the below line to clean dynamically freed memory
  * @note  The freed memory will be reset to 0
  * @note  If using @ref KMS_MEM_USE_HEAP_ALLOCATOR, @ref KMS_MEM_DEBUGGING required
  * @note  Not usable with @ref KMS_MEM_USE_CUSTOM_ALLOCATOR
  */
#define KMS_MEM_CLEANING

/**
  * @brief KMS_MEM_LOGGING Uncomment the below line to log memory allocation information
  * @note  Not usable with @ref KMS_MEM_USE_CUSTOM_ALLOCATOR
  * @note  Information about allocations will be transmitted to @ref KMS_LL_ReportMemAlloc and @ref KMS_LL_ReportMemFree
  */
/*#define KMS_MEM_LOGGING*/

/**
  * @}
  */


/** @defgroup KMS_Security_Config Security Enforcement
  * @{
  */

/**
  * @brief KMS_SE_CHECK_PARAMS Uncomment the below line to check input & output parameters location vs secure enclave
  * @note  Secure enforcement only applicable when using iKMS and secure enclave
  */
#define KMS_SE_CHECK_PARAMS

/**
  * @brief KMS_SE_LOCK_KEYS Uncomment the below line to use key locking services
  * @note  Secure enforcement only applicable when using iKMS and secure enclave
  */
#define KMS_SE_LOCK_KEYS

/**
  * @brief KMS_SE_LOCK_KEYS_MAXIMUM Specify here the number of keys that can be locked
  * @note  Mandatory to set if @ref KMS_SE_LOCK_KEYS is defined
  */
#define KMS_SE_LOCK_KEYS_MAX                 (10UL)

/**
  * @brief KMS_SE_LOCK_SERVICES Uncomment the below line to use services locking services
  * @note  Secure enforcement only applicable when using iKMS and secure enclave
  */
#define KMS_SE_LOCK_SERVICES

/**
  * @brief KMS_SE_LOCK_SERVICES_MAX Specify here the number of services that can be locked
  * @note  Mandatory to set if @ref KMS_SE_LOCK_SERVICES is defined
  */
#define KMS_SE_LOCK_SERVICES_MAX             (2UL)

/**
  * @}
  */

/** Function flag definition to be used to qualify algorithm usage **/

#if defined(KMS_ENCRYPT)
#define KMS_FCT_ENCRYPT         (1UL << 0)
#else /* KMS_ENCRYPT */
#define KMS_FCT_ENCRYPT         (0UL << 0)
#endif /* KMS_ENCRYPT */
#if defined(KMS_DECRYPT)
#define KMS_FCT_DECRYPT         (1UL << 1)
#else /* KMS_DECRYPT */
#define KMS_FCT_DECRYPT         (0UL << 1)
#endif /* KMS_DECRYPT */
#if defined(KMS_SIGN)
#define KMS_FCT_SIGN            (1UL << 2)
#else /* KMS_SIGN */
#define KMS_FCT_SIGN            (0UL << 2)
#endif /* KMS_SIGN */
#if defined(KMS_VERIFY)
#define KMS_FCT_VERIFY          (1UL << 3)
#else /* KMS_VERIFY */
#define KMS_FCT_VERIFY          (0UL << 3)
#endif /* KMS_VERIFY */
#if defined(KMS_DERIVE_KEY)
#define KMS_FCT_DERIVE_KEY      (1UL << 4)
#else /* KMS_DERIVE_KEY */
#define KMS_FCT_DERIVE_KEY      (0UL << 4)
#endif /* KMS_DERIVE_KEY */
#if defined(KMS_GENERATE_KEYS)
#define KMS_FCT_GENERATE_KEYS   (1UL << 5)
#else /* KMS_GENERATE_KEYS */
#define KMS_FCT_GENERATE_KEYS   (0UL << 5)
#endif /* KMS_GENERATE_KEYS */
#if defined(KMS_DIGEST)
#define KMS_FCT_DIGEST          (1UL << 6)
#else /* KMS_DIGEST */
#define KMS_FCT_DIGEST          (0UL << 6)
#endif /* KMS_DIGEST */

/** @defgroup KMS_Features_Config Features
  * @{
  */

/**
  * @brief KMS_AES_CBC Uncomment the below line to support AES CBC algorithm
  * @note  This defines is a bit mask that include information on the algorithm purpose:
  *        - KMS_FCT_ENCRYPT          This algorithm can be used to perform encryption
  *        - KMS_FCT_DECRYPT          This algorithm can be used to perform decryption
  * @note  Requires at least one of the following PKCS#11 services to be enabled:
  *        @ref KMS_ENCRYPT
  *        @ref KMS_DECRYPT
  */
#define KMS_AES_CBC           (KMS_FCT_ENCRYPT|KMS_FCT_DECRYPT)

/**
  * @brief KMS_AES_CCM Uncomment the below line to support AES CCM algorithm
  * @note  This defines is a bit mask that include information on the algorithm purpose:
  *        - KMS_FCT_ENCRYPT          This algorithm can be used to perform encryption
  *        - KMS_FCT_DECRYPT          This algorithm can be used to perform decryption
  * @note  Requires at least one of the following PKCS#11 services to be enabled:
  *        @ref KMS_ENCRYPT
  *        @ref KMS_DECRYPT
  */
/*#define KMS_AES_CCM           (KMS_FCT_ENCRYPT|KMS_FCT_DECRYPT)*/

/**
  * @brief KMS_AES_ECB Uncomment the below line to support AES ECB algorithm
  * @note  This defines is a bit mask that include information on the algorithm purpose:
  *        - KMS_FCT_ENCRYPT          This algorithm can be used to perform encryption
  *        - KMS_FCT_DECRYPT          This algorithm can be used to perform decryption
  *        - KMS_FCT_DERIVE_KEY       This algorithm can be used to perform key derivation
  * @note  Requires at least one of the following PKCS#11 services to be enabled:
  *        @ref KMS_ENCRYPT
  *        @ref KMS_DECRYPT
  *        @ref KMS_DERIVE_KEY
  */
#define KMS_AES_ECB           (KMS_FCT_ENCRYPT|KMS_FCT_DECRYPT|KMS_FCT_DERIVE_KEY)

/**
  * @brief KMS_AES_GCM Uncomment the below line to support AES GCM algorithm
  * @note  This defines is a bit mask that include information on the algorithm purpose:
  *        - KMS_FCT_ENCRYPT          This algorithm can be used to perform encryption
  *        - KMS_FCT_DECRYPT          This algorithm can be used to perform decryption
  * @note  Requires at least one of the following PKCS#11 services to be enabled:
  *        @ref KMS_ENCRYPT
  *        @ref KMS_DECRYPT
  */
#define KMS_AES_GCM           (KMS_FCT_ENCRYPT|KMS_FCT_DECRYPT)

/**
  * @brief KMS_AES_CMAC Uncomment the below line to support AES CMAC algorithm
  * @note  This defines is a bit mask that include information on the algorithm purpose:
  *        - KMS_FCT_SIGN             This algorithm can be used to perform signature
  *        - KMS_FCT_VERIFY           This algorithm can be used to perform verification
  * @note  Requires at least one of the following PKCS#11 services to be enabled:
  *        @ref KMS_SIGN
  *        @ref KMS_VERIFY
  */
#define KMS_AES_CMAC          (KMS_FCT_SIGN|KMS_FCT_VERIFY)

/**
  * @brief KMS_RSA Uncomment the below line to support RSA algorithm
  * @note  This defines is a bit mask that include information on the algorithm purpose:
  *        - KMS_FCT_SIGN             This algorithm can be used to perform signature
  *        - KMS_FCT_VERIFY           This algorithm can be used to perform verification
  * @note  Requires at least one of the following PKCS#11 services to be enabled:
  *        @ref KMS_SIGN
  *        @ref KMS_VERIFY
  * @note  Requires at least one of the following RSA modulus length to be enabled:
  *        @ref KMS_RSA_1024
  *        @ref KMS_RSA_2048
  */
/* #define KMS_RSA               (KMS_FCT_SIGN|KMS_FCT_VERIFY) */

/**
  * @brief KMS_RSA_1024 Uncomment the below line to support RSA 1024 bits modulus length
  * @note  Requires @ref KMS_RSA to be defined
  */
/* #define KMS_RSA_1024 */

/**
  * @brief KMS_RSA_2048 Uncomment the below line to support RSA 2048 bits modulus length
  * @note  Requires @ref KMS_RSA to be defined
  */
/* #define KMS_RSA_2048 */

/**
  * @brief KMS_ECDSA Uncomment the below line to support ECDSA algorithm
  * @note  This defines is a bit mask that include information on the algorithm purpose:
  *        - KMS_FCT_VERIFY           This algorithm can be used to perform verification
  *        - KMS_FCT_DERIVE_KEY       This algorithm can be used to perform key derivation
  *        - KMS_FCT_GENERATE_KEYS    This algorithm can be used to perform key generation
  * @note  Requires at least one of the following PKCS#11 services to be enabled:
  *        @ref KMS_VERIFY
  *        @ref KMS_DERIVE_KEY
  *        @ref KMS_GENERATE_KEYS
  * @note  Requires at least one of the following elliptic curve to be enabled:
  *        @ref KMS_EC_SECP192
  *        @ref KMS_EC_SECP256
  *        @ref KMS_EC_SECP384
  */
#define KMS_ECDSA             (KMS_FCT_VERIFY)

/**
  * @brief KMS_EC_SECP192 Uncomment the below line to support elliptic curve SECP-192
  * @note  Requires @ref KMS_ECDSA to be defined
  */
/*#define KMS_EC_SECP192*/

/**
  * @brief KMS_EC_SECP256 Uncomment the below line to support elliptic curve SECP-256
  * @note  Requires @ref KMS_ECDSA to be defined
  */
#define KMS_EC_SECP256

/**
  * @brief KMS_EC_SECP384 Uncomment the below line to support elliptic curve SECP-384
  * @note  Requires @ref KMS_ECDSA to be defined
  */
/*#define KMS_EC_SECP384*/

/**
  * @brief KMS_SHA1 Uncomment the below line to support SHA1 digest algorithm
  * @note  This defines is a bit mask that include information on the algorithm purpose:
  *        - KMS_FCT_DIGEST          This algorithm can be used to perform digesting
  * @note  Requires at least one of the following PKCS#11 services to be enabled:
  *        @ref KMS_DIGEST
  */
/*#define KMS_SHA1              (KMS_FCT_DIGEST)*/

/**
  * @brief KMS_SHA256 Uncomment the below line to support SHA256 digest algorithm
  * @note  This defines is a bit mask that include information on the algorithm purpose:
  *        - KMS_FCT_DIGEST          This algorithm can be used to perform digesting
  * @note  Requires at least one of the following PKCS#11 services to be enabled:
  *        @ref KMS_DIGEST
  */
#define KMS_SHA256            (KMS_FCT_DIGEST)



/** @defgroup KMS_Debug_Config Debug
  * @{
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

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* KMS_CONFIG_H */
