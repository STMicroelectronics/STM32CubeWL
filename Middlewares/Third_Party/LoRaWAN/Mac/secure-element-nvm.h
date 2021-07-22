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

#include <stdint.h>
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

/* ST_WORKAROUND_BEGIN: Dynamic number of keys definition */
/*!
 * Number of supported crypto keys for the soft-se
 */
#if ( USE_LRWAN_1_1_X_CRYPTO == 1 )
#if ( LORAMAC_MAX_MC_CTX > 3 )
#define NUM_OF_KEYS             23UL
#elif ( LORAMAC_MAX_MC_CTX > 2 )
#define NUM_OF_KEYS             20UL
#elif ( LORAMAC_MAX_MC_CTX > 1 )
#define NUM_OF_KEYS             17UL
#else /* LORAMAC_MAX_MC_CTX == 0 */
#define NUM_OF_KEYS             14UL
#endif /* LORAMAC_MAX_MC_CTX */
#else /* USE_LRWAN_1_1_X_CRYPTO == 0 */
#if ( LORAMAC_MAX_MC_CTX > 3 )
#define NUM_OF_KEYS             19UL
#elif ( LORAMAC_MAX_MC_CTX > 2 )
#define NUM_OF_KEYS             16UL
#elif ( LORAMAC_MAX_MC_CTX > 1 )
#define NUM_OF_KEYS             13UL
#else /* LORAMAC_MAX_MC_CTX == 0 */
#define NUM_OF_KEYS             10UL
#endif /* LORAMAC_MAX_MC_CTX */
#endif /* USE_LRWAN_1_1_X_CRYPTO */
/* ST_WORKAROUND_END */

/*!
 * Key structure definition for the soft-se
 */
typedef struct sKey
{
    /*!
     * Key identifier
     */
    KeyIdentifier_t KeyID;
    /*!
     * Key value
     */
    uint8_t KeyValue[SE_KEY_SIZE];
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
#else /* LORAWAN_KMS == 1 */
    /*!
     * Key object index in the above list
     */
    CK_OBJECT_HANDLE Object_Index;
#endif /* LORAWAN_KMS */
} Key_t;

typedef struct sSecureElementNvCtx
{
    /*!
     * DevEUI storage
     */
    uint8_t DevEui[SE_EUI_SIZE];
    /*!
     * Join EUI storage
     */
    uint8_t JoinEui[SE_EUI_SIZE];
    /*!
     * The key list is required for the soft-se only. All other secure-elements
     * handle the storage on their own.
     */
    Key_t KeyList[NUM_OF_KEYS];
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
