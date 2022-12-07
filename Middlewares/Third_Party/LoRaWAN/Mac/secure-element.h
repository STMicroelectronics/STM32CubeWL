/*!
 * \file      secure-element.h
 *
 * \brief     Secure Element driver API
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
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 *
 * \defgroup  SECUREELEMENT Secure Element API Definition
 *
 * \{
 *
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    secure-element.h
  * @author  MCD Application Team
  * @brief   Secure Element driver API
  ******************************************************************************
  */
#ifndef __SECURE_ELEMENT_H__
#define __SECURE_ELEMENT_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "LoRaMacCrypto.h"
#include "secure-element-nvm.h"
#include "LoRaMacVersion.h"

/*!
 * Return values.
 */
typedef enum eSecureElementStatus
{
    /*!
     * No error occurred
     */
    SECURE_ELEMENT_SUCCESS = 0,
    /*!
     * CMAC does not match
     */
    SECURE_ELEMENT_FAIL_CMAC,
    /*!
     * Null pointer exception
     */
    SECURE_ELEMENT_ERROR_NPE,
    /*!
     * Invalid key identifier exception
     */
    SECURE_ELEMENT_ERROR_INVALID_KEY_ID,
    /*!
     * Invalid LoRaWAN specification version
     */
    SECURE_ELEMENT_ERROR_INVALID_LORAWAM_SPEC_VERSION,
    /*!
     * Incompatible buffer size
     */
    SECURE_ELEMENT_ERROR_BUF_SIZE,
    /*!
     * Undefined Error occurred
     */
    SECURE_ELEMENT_ERROR,
    /*!
     * Failed to encrypt
     */
    SECURE_ELEMENT_FAIL_ENCRYPT,
}SecureElementStatus_t;

/* ST_WORKAROUND_BEGIN: Add unique ID callback as input parameter */
/*!
 * \brief   get the board 64 bits unique ID
 *
 * \param   [out] id unique
 */
typedef void ( *SecureElementGetUniqueId )(uint8_t *id);

/*!
 * Initialization of Secure Element driver
 *
 * \param [in]    nvm              - Pointer to the non-volatile memory data
 *                                   structure.
 * \param [in]    seGetUniqueId    - Get unique ID callback
 * \retval                         - Status of the operation
 */
SecureElementStatus_t SecureElementInit( SecureElementNvmData_t* nvm, SecureElementGetUniqueId seGetUniqueId );
/* ST_WORKAROUND_END */

/*!
 * Print Root and Session keys if available
 *
 * \retval                         - Status of the operation
 */
SecureElementStatus_t SecureElementPrintKeys( void );

/*!
 * Print OTAA Session keys if available
 *
 * \retval                         - Status of the operation
 */
SecureElementStatus_t SecureElementPrintSessionKeys( void );

/* ST_WORKAROUND_BEGIN: Add KMS specific functions */
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
/*!
 * Gets key item from key list.
 *
 * \param [in] keyID          - Key identifier
 * \param [out] keyItem       - Key item reference
 * \retval                    - Status of the operation
 */
#else
/*!
 * Gets key item from key list.
 *
 * \param [in] keyID          - Key identifier
 * \param [out] extractable_key - Key item pointer
 * \retval                    - Status of the operation
 */
#endif /* LORAWAN_KMS */
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
SecureElementStatus_t SecureElementGetKeyByID( KeyIdentifier_t keyID, Key_t **keyItem);
#else
SecureElementStatus_t SecureElementGetKeyByID( KeyIdentifier_t keyID, uint8_t* extractable_key );
#endif /* LORAWAN_KMS */

/*!
 * Remove previously generated dynamic keys with "label" from memory
 *
 * \param [in]    keyID               - Key identifier
 * \param [out]   key_label           - string of char to identifying targetKeyID label
 * \retval                            - Status of the operation
 */
SecureElementStatus_t SecureElementDeleteDynamicKeys( KeyIdentifier_t keyID, uint32_t *key_label );

/*!
 * Sets a the KMS object handler for a given keyID (reserved to Kms)
 *
 * \param [in] keyID          - Key identifier
 * \param [in] keyIndex       - Key index value
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementSetObjHandler( KeyIdentifier_t keyID, uint32_t keyIndex );
/* ST_WORKAROUND_END */

/*!
 * Sets a key
 *
 * \param [in] keyID          - Key identifier
 * \param [in] key            - Key value
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementSetKey( KeyIdentifier_t keyID, uint8_t* key );

/*!
 * Computes a CMAC of a message using provided initial Bx block
 *
 * \param [in] micBxBuffer    - Buffer containing the initial Bx block
 * \param [in] buffer         - Data buffer
 * \param [in] size           - Data buffer size
 * \param [in] keyID          - Key identifier to determine the AES key to be used
 * \param [out] cmac          - Computed cmac
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementComputeAesCmac( uint8_t* micBxBuffer, uint8_t* buffer, uint16_t size, KeyIdentifier_t keyID, uint32_t* cmac );

/*!
 * Verifies a CMAC (computes and compare with expected cmac)
 *
 * \param [in] buffer         - Data buffer
 * \param [in] size           - Data buffer size
 * \param [in] expectedCmac   - Expected cmac
 * \param [in] keyID          - Key identifier to determine the AES key to be used
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementVerifyAesCmac( uint8_t* buffer, uint16_t size, uint32_t expectedCmac, KeyIdentifier_t keyID );

/*!
 * Encrypt a buffer
 *
 * \param [in] buffer         - Data buffer
 * \param [in] size           - Data buffer size
 * \param [in] keyID          - Key identifier to determine the AES key to be used
 * \param [out] encBuffer     - Encrypted buffer
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementAesEncrypt( uint8_t* buffer, uint16_t size, KeyIdentifier_t keyID, uint8_t* encBuffer );

/*!
 * Derives and store a key
 *
 * \param [in] input          - Input data from which the key is derived ( 16 byte )
 * \param [in] rootKeyID      - Key identifier of the root key to use to perform the derivation
 * \param [in] targetKeyID    - Key identifier of the key which will be derived
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementDeriveAndStoreKey( uint8_t* input, KeyIdentifier_t rootKeyID, KeyIdentifier_t targetKeyID );

/*!
 * Process JoinAccept message.
 *
 * \param [in] joinReqType       - Type of last join-request or rejoin which triggered the join-accept response
 * \param [in] joinEui           - Join server EUI (8 byte)
 * \param [in] devNonce          - Random value generated by Network Server to prevent replay attacks with previous Join session params
 * \param [in] encJoinAccept     - Received encrypted JoinAccept message
 * \param [in] encJoinAcceptSize - Received encrypted JoinAccept message Size
 * \param [out] decJoinAccept    - Decrypted and validated JoinAccept message
 * \param [out] versionMinor     - Detected LoRaWAN specification version minor field.
 *                                     - 0 -> LoRaWAN 1.0.x
 *                                     - 1 -> LoRaWAN 1.1.x
 * \retval                       - Status of the operation
 */
SecureElementStatus_t SecureElementProcessJoinAccept( JoinReqIdentifier_t joinReqType, uint8_t* joinEui,
                                                      uint16_t devNonce, uint8_t* encJoinAccept,
                                                      uint8_t encJoinAcceptSize, uint8_t* decJoinAccept,
                                                      uint8_t* versionMinor );

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
/*!
 * Generates a random number
 *
 * \param[OUT] randomNum      - 32 bit random number
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementRandomNumber( uint32_t* randomNum );
#endif /* LORAMAC_VERSION */

/*!
 * Sets the DevEUI
 *
 * \param [in] devEui         - Pointer to the 8-byte devEUI
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementSetDevEui( uint8_t* devEui );

/*!
 * Gets the DevEUI
 *
 * \retval                    - Pointer to the 8-byte devEUI
 */
uint8_t* SecureElementGetDevEui( void );

/*!
 * Sets the JoinEUI
 *
 * \param [in] joinEui        - Pointer to the 8-byte joinEui
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementSetJoinEui( uint8_t* joinEui );

/*!
 * Gets the DevEUI
 *
 * \retval                    - Pointer to the 8-byte joinEui
 */
uint8_t* SecureElementGetJoinEui( void );

/*!
 * Sets the pin
 *
 * \param [in] pin            - Pointer to the 4-byte pin
 * \retval                    - Status of the operation
 */
SecureElementStatus_t SecureElementSetPin( uint8_t* pin );

/*!
 * Gets the Pin
 *
 * \retval                    - Pointer to the 4-byte pin
 */
uint8_t* SecureElementGetPin( void );

/*! \} defgroup SECUREELEMENT */

#ifdef __cplusplus
}
#endif

#endif //  __SECURE_ELEMENT_H__
