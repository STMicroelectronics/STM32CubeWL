/*!
 * \file      secure-element-nvm.h
 *
 * \brief     Secure Element non-volatile data.
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \addtogroup  SECUREELEMENT
 *
 * \{
 *
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    secure-element-nvm.h
  * @author  MCD Application Team
  * @brief   Secure Element non-volatile data.
  ******************************************************************************
  */
#ifndef __SECURE_ELEMENT_NVM_H__
#define __SECURE_ELEMENT_NVM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "LoRaMacTypes.h"
#include "lorawan_conf.h"  /* LORAWAN_KMS */
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
#else /* LORAWAN_KMS == 1 */
#include "kms_if.h"
#endif /* LORAWAN_KMS */

/*!
 * Secure-element keys size in bytes
 */
#define SE_KEY_SIZE             16

/*!
 * Secure-element EUI size in bytes
 */
#define SE_EUI_SIZE             8

/*!
 * Secure-element NVM EUI size in bytes (size of SecureElementNvmDevJoinAddrKey_t)
 */
#define SE_NVM_EUI_SIZE         24

/*!
 * Number of supported crypto keys for the soft-se
 */
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
#define NUM_SESSION_KEY         7
#else /* LORAMAC_VERSION */
#define NUM_SESSION_KEY         3
#endif /* LORAMAC_VERSION */

#define NUM_MC_KEYS             (3 * LORAMAC_MAX_MC_CTX)

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
#define NUM_ID_KEY              0
#else /* LORAWAN_KMS == 1 */
#define NUM_ID_KEY              1
#endif /* LORAWAN_KMS */

#define NUM_OF_KEYS             (5 + NUM_SESSION_KEY + NUM_MC_KEYS + NUM_ID_KEY)

/*!
 * Key structure definition for the soft-se
 */
typedef struct sKey
{
    /*!
     * Key identifier
     */
    KeyIdentifier_t KeyID;
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    /*!
     * Key value
     */
    uint8_t KeyValue[SE_KEY_SIZE];
#else /* LORAWAN_KMS == 1 */
    /*!
     * Key object index in the above list
     */
    CK_OBJECT_HANDLE Object_Index;
#endif /* LORAWAN_KMS */
} Key_t;

/*!
 * Identifiers structure definition for the soft-se
 */
typedef struct SecureElementNvmDevJoinAddrKey
{
    /** OTAA */
    /*!
     * DevEUI storage
     */
    uint8_t DevEui[SE_EUI_SIZE];
    /*!
     * Join EUI storage
     */
    uint8_t JoinEui[SE_EUI_SIZE];
    /*!
     * DevAddr storage for OTA Activation
     */
    uint32_t DevAddrOTAA;
    /** ABP */
    /*!
     * DevAddr storage for ABP Activation
     */
    uint32_t DevAddrABP;
} SecureElementNvmDevJoinAddrKey_t;

/*!
 * Key/Identifiers data which must be stored in the NVM.
 */
typedef struct sSecureElementNvCtx
{
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    /*!
     * The ID list for the soft-se
     */
    SecureElementNvmDevJoinAddrKey_t SeNvmDevJoinKey;
    /*!
     * The key list is required for the soft-se only. All other secure-elements
     * handle the storage on their own.
     */
    Key_t KeyList[NUM_OF_KEYS];
#else
    uint32_t reserved;
#endif /* LORAWAN_KMS */
    /*!
     * CRC32 value of the SecureElement data structure.
     */
    uint32_t Crc32;
} SecureElementNvmData_t;

/*! \} addtogroup SECUREELEMENT */

#ifdef __cplusplus
}
#endif

#endif //  __SECURE_ELEMENT_NVM_H__
