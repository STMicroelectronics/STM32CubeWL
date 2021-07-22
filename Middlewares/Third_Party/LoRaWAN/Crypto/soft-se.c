/*!
 * \file      soft-se.c
 *
 * \brief     Secure Element software implementation
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
 *              (C)2020 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 */

/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    soft-se.c
  * @author  MCD Application Team
  * @brief   Secure Element software implementation
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>

#include "lorawan_conf.h"  /* LORAWAN_KMS */
#include "radio.h"         /* needed for Random */
#include "utilities.h"
#include "mw_log_conf.h"   /* needed for MW_LOG */
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
#include "lorawan_aes.h"
#include "cmac.h"
#else /* LORAWAN_KMS == 1 */
#include "kms_if.h"
#endif /* LORAWAN_KMS */

#include "LoRaMacHeaderTypes.h"

#include "secure-element.h"
#include "secure-element-nvm.h"
#include "se-identity.h"

/* Private constants ---------------------------------------------------------*/
/*!
 * MIC computation offset
 * \remark required for 1.1.x support
 */
#define CRYPTO_MIC_COMPUTATION_OFFSET ( JOIN_REQ_TYPE_SIZE\
                                        + LORAMAC_JOIN_EUI_FIELD_SIZE + DEV_NONCE_SIZE + LORAMAC_MHDR_FIELD_SIZE )

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
#else /* LORAWAN_KMS == 1 */
#define DERIVED_OBJECT_HANDLE_RESET_VAL      0x0UL
#define PAYLOAD_MAX_SIZE     270UL  /* 270 PHYPayload: 1+(22+1+242)+4 */
#endif /* LORAWAN_KMS */

/* Private macro -------------------------------------------------------------*/
/*!
 * Hex 8 split buffer
 */
#define HEX8(X)   X[0], X[1], X[2], X[3], X[4], X[5], X[6], X[7]

/*!
 * Hex 16 split buffer
 */
#define HEX16(X)  HEX8(X), X[8], X[9], X[10], X[11], X[12], X[13], X[14], X[15]

/* Private Types ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*!
 * Secure element context
 */
static SecureElementNvmData_t* SeNvm;

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
#else /* LORAWAN_KMS == 1 */
/* WARNING: Should be modified at the end of product development */
static const CK_ULONG GlobalTemplateLabel = 0x444E524CU;

/*
 * Intermediate buffer used for two reasons:
 * - align to 32 bits and
 * - for Cmac combine InitVector + input buff
 */
static uint8_t input_align_combined_buf[PAYLOAD_MAX_SIZE + SE_KEY_SIZE] ALIGN(4);

static uint8_t output_align[PAYLOAD_MAX_SIZE] ALIGN(4);

static uint8_t tag[SE_KEY_SIZE] ALIGN(4) = {0};
#endif /* LORAWAN_KMS */

/* Private functions prototypes ---------------------------------------------------*/
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
static SecureElementStatus_t GetKeyByID( KeyIdentifier_t keyID, Key_t **keyItem );
#else /* LORAWAN_KMS == 1 */
static SecureElementStatus_t GetKeyIndexByID( KeyIdentifier_t keyID, CK_OBJECT_HANDLE *keyIndex );

static SecureElementStatus_t GetSpecificLabelByID( KeyIdentifier_t keyID, uint32_t *specificLabel );

static SecureElementStatus_t DeleteAllDynamicKeys( void );
#endif /* LORAWAN_KMS */

#if (defined (KEY_EXTRACTABLE) && (KEY_EXTRACTABLE == 1))
static void PrintKey( KeyIdentifier_t key );
#endif /* KEY_EXTRACTABLE */

static SecureElementStatus_t ComputeCmac(uint8_t *micBxBuffer, uint8_t *buffer, uint16_t size, KeyIdentifier_t keyID,
                                         uint32_t *cmac);


/* Private functions ---------------------------------------------------------*/
#if (defined (KEY_EXTRACTABLE) && (KEY_EXTRACTABLE == 1))
static void PrintKey( KeyIdentifier_t key )
{
    SecureElementStatus_t retval = SECURE_ELEMENT_ERROR;
    Key_t *keyItem;
    retval = SecureElementGetKeyByID(key, &keyItem);
    if (retval == SECURE_ELEMENT_SUCCESS)
    {
        if (key == APP_KEY)
        {
            MW_LOG(TS_OFF, VLEVEL_M, "###### AppKey:      ");
        }
        else if (key == NWK_KEY)
        {
            MW_LOG(TS_OFF, VLEVEL_M, "###### NwkKey:      ");
        }
        else if (key == APP_S_KEY)
        {
            MW_LOG(TS_OFF, VLEVEL_M, "###### AppSKey:     ");
        }
        else if (key == NWK_S_KEY)
        {
            MW_LOG(TS_OFF, VLEVEL_M, "###### NwkSKey:     ");
        }
        else if (key == MC_ROOT_KEY)
        {
            MW_LOG(TS_OFF, VLEVEL_M, "###### MCRootKey:   ");
        }
        else if (key == MC_KE_KEY)
        {
            MW_LOG(TS_OFF, VLEVEL_M, "###### MCKEKey:     ");
        }
        else if (key == MC_KEY_0)
        {
            MW_LOG(TS_OFF, VLEVEL_M, "###### MCKey_0:     ");
        }
        else if (key == MC_APP_S_KEY_0)
        {
            MW_LOG(TS_OFF, VLEVEL_M, "###### MCAppSKey_0: ");
        }
        else if (key == MC_NWK_S_KEY_0)
        {
            MW_LOG(TS_OFF, VLEVEL_M, "###### MCNwkSKey_0: ");
        }
        MW_LOG(TS_OFF, VLEVEL_M, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
               HEX16(keyItem->KeyValue));
    }
}
#endif /* KEY_EXTRACTABLE */

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
/*
 * Gets key item from key list.
 *
 * \param[IN]  keyID          - Key identifier
 * \param[OUT] keyItem        - Key item reference
 * \retval                    - Status of the operation
 */
static SecureElementStatus_t GetKeyByID( KeyIdentifier_t keyID, Key_t** keyItem )
{
    for( uint8_t i = 0; i < NUM_OF_KEYS; i++ )
    {
        if( SeNvm->KeyList[i].KeyID == keyID )
        {
            *keyItem = &( SeNvm->KeyList[i] );
            return SECURE_ELEMENT_SUCCESS;
        }
    }
    return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
}

#else /* LORAWAN_KMS == 1 */

/*
 * Gets key Index from key list in KMS table
 *
 * \param[IN]    keyID                    - Key identifier
 * \param[OUT] keyIndex             - Key item reference
 * \retval                                        - Status of the operation
 */
static SecureElementStatus_t GetKeyIndexByID( KeyIdentifier_t keyID, CK_OBJECT_HANDLE *keyIndex )
{
    for (uint8_t i = 0; i < NUM_OF_KEYS; i++)
    {
        if (SeNvm->KeyList[i].KeyID == keyID)
        {
            *keyIndex = SeNvm->KeyList[i].Object_Index;
            return SECURE_ELEMENT_SUCCESS;
        }
    }
    return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
}

static SecureElementStatus_t GetSpecificLabelByID( KeyIdentifier_t keyID, uint32_t *specificLabel )
{
    SecureElementStatus_t retval = SECURE_ELEMENT_SUCCESS;
    switch (keyID)
    {
        case APP_KEY:
            *specificLabel = 0x5F505041U;
            break;
        case NWK_KEY:
            *specificLabel = 0x5F4B574EU;
            break;
#if ( USE_LRWAN_1_1_X_CRYPTO == 1 )
        case J_S_INT_KEY:
            *specificLabel = 0x314B574EU;
            break;
        case J_S_ENC_KEY:
            *specificLabel = 0x324B574EU;
            break;
        case F_NWK_S_INT_KEY:
            *specificLabel = 0x334B574EU;
            break;
        case S_NWK_S_INT_KEY:
            *specificLabel = 0x344B574EU;
            break;
        case NWK_S_ENC_KEY:
            *specificLabel = 0x354B574EU;
            break;
#else /* USE_LRWAN_1_1_X_CRYPTO == 0 */
        case NWK_S_KEY:
            *specificLabel = 0x534B574EU;
            break;
#endif /* USE_LRWAN_1_1_X_CRYPTO */
        case APP_S_KEY:
            *specificLabel = 0x53505041U;
            break;
        case MC_ROOT_KEY:
            *specificLabel = 0x5452434DU;
            break;
        case MC_KE_KEY:
            *specificLabel = 0x454B434DU;
            break;
        case MC_KEY_0:
            *specificLabel = 0x304B434DU;
            break;
        case MC_APP_S_KEY_0:
            *specificLabel = 0x3053414DU;
            break;
        case MC_NWK_S_KEY_0:
            *specificLabel = 0x30534E4DU;
            break;
#if ( LORAMAC_MAX_MC_CTX > 1 )
        case MC_KEY_1:
            *specificLabel = 0x314B434DU;
            break;
        case MC_APP_S_KEY_1:
            *specificLabel = 0x3153414DU;
            break;
        case MC_NWK_S_KEY_1:
            *specificLabel = 0x31534E4DU;
            break;
        case MC_KEY_2:
            *specificLabel = 0x324B434DU;
            break;
        case MC_APP_S_KEY_2:
            *specificLabel = 0x3253414DU;
            break;
        case MC_NWK_S_KEY_2:
            *specificLabel = 0x32534E4DU;
            break;
        case MC_KEY_3:
            *specificLabel = 0x334B434DU;
            break;
        case MC_APP_S_KEY_3:
            *specificLabel = 0x3353414DU;
            break;
        case MC_NWK_S_KEY_3:
            *specificLabel = 0x33534E4DU;
            break;
#endif /* LORAMAC_MAX_MC_CTX > 1 */
        default:
            retval = SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
            break;
    }
    return retval;
}

static SecureElementStatus_t DeleteAllDynamicKeys( void )
{
    CK_RV rv;
    CK_SESSION_HANDLE session;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
    CK_OBJECT_HANDLE hObject[NUM_OF_KEYS];
    uint32_t ulCount = 0;

    /* Open session with KMS */
    rv = C_OpenSession(0,  session_flags, NULL, 0, &session);

    /* Get all keys handle */
    if (rv == CKR_OK)
    {
        rv = C_FindObjectsInit(session, NULL, 0);
    }

    /* Find all existing keys handle */
    if (rv == CKR_OK)
    {
        rv = C_FindObjects(session, hObject, NUM_OF_KEYS, (CK_ULONG *) &ulCount);
    }

    if (rv == CKR_OK)
    {
        rv = C_FindObjectsFinal(session);
    }

    if (ulCount <= NUM_OF_KEYS)
    {
        for (uint8_t i = 0; i < ulCount; i++)
        {
            /* Exclude all Embedded keys */
            if (hObject[i] > LAST_KMS_KEY_OBJECT_HANDLE)
            {
                if (rv == CKR_OK)
                {
                    rv = C_DestroyObject(session, hObject[i]);
                }
            }
        }
    }

    /* Close sessions */
    if (session > 0)
    {
        (void)C_CloseSession(session);
    }

    if (rv != CKR_OK)
    {
        return SECURE_ELEMENT_ERROR;
    }
    return SECURE_ELEMENT_SUCCESS;
}
#endif /* LORAWAN_KMS */

/*
 * Computes a CMAC of a message using provided initial Bx block
 *
 *  cmac = aes128_cmac(keyID, blocks[i].Buffer)
 *
 * \param[IN]  micBxBuffer    - Buffer containing the initial Bx block
 * \param[IN]  buffer         - Data buffer
 * \param[IN]  size           - Data buffer size
 * \param[IN]  keyID          - Key identifier to determine the AES key to be used
 * \param[OUT] cmac           - Computed cmac
 * \retval                    - Status of the operation
 */
static SecureElementStatus_t ComputeCmac( uint8_t* micBxBuffer, uint8_t* buffer, uint16_t size, KeyIdentifier_t keyID,
                                          uint32_t* cmac )
{
    if( ( buffer == NULL ) || ( cmac == NULL ) )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    uint8_t Cmac[16];
    AES_CMAC_CTX aesCmacCtx[1];

    AES_CMAC_Init( aesCmacCtx );

    Key_t*                keyItem;
    SecureElementStatus_t retval = GetKeyByID( keyID, &keyItem );

    if( retval == SECURE_ELEMENT_SUCCESS )
    {
        AES_CMAC_SetKey( aesCmacCtx, keyItem->KeyValue );

        if( micBxBuffer != NULL )
        {
            AES_CMAC_Update( aesCmacCtx, micBxBuffer, 16 );
        }

        AES_CMAC_Update( aesCmacCtx, buffer, size );

        AES_CMAC_Final( Cmac, aesCmacCtx );

        // Bring into the required format
        *cmac = ( uint32_t )( ( uint32_t ) Cmac[3] << 24 | ( uint32_t ) Cmac[2] << 16 | ( uint32_t ) Cmac[1] << 8 |
                              ( uint32_t ) Cmac[0] );
    }
#else /* LORAWAN_KMS == 1 */
    CK_RV rv;
    CK_SESSION_HANDLE session;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;    /* Read ONLY session */
    uint32_t tag_lenth = 0;
    CK_OBJECT_HANDLE key_handle ;

    /* AES CMAC Authentication variables */
    CK_MECHANISM aes_cmac_mechanism = { CKM_AES_CMAC, (CK_VOID_PTR)NULL, 0 };

    SecureElementStatus_t retval = GetKeyIndexByID(keyID, &key_handle);
    if (retval != SECURE_ELEMENT_SUCCESS)
    {
        return retval;
    }

    /* Open session with KMS */
    rv = C_OpenSession(0,    session_flags, NULL, 0, &session);

    /* Configure session to Authentication message in AES CMAC with settings included into the mechanism */
    if (rv == CKR_OK)
    {
        rv = C_SignInit(session, &aes_cmac_mechanism, key_handle);
    }

    /* Encrypt clear message */
    if (rv == CKR_OK)
    {
        /* work around : need to double-check if possible to use micBxBuffer as IV for Sign */
        if (micBxBuffer != NULL)
        {
            memcpy1((uint8_t *) &input_align_combined_buf[0], (uint8_t *) micBxBuffer, SE_KEY_SIZE);
            memcpy1((uint8_t *) &input_align_combined_buf[SE_KEY_SIZE], (uint8_t *) buffer, size);
        }
        else
        {
            memcpy1((uint8_t *) &input_align_combined_buf[0], (uint8_t *) buffer, size);
        }
    }

    if (rv == CKR_OK)
    {
        if (micBxBuffer != NULL)
        {
            rv = C_Sign(session, (CK_BYTE_PTR)&input_align_combined_buf[0], size + SE_KEY_SIZE, &tag[0],
                        (CK_ULONG_PTR)&tag_lenth);
        }
        else
        {
            rv = C_Sign(session, (CK_BYTE_PTR)&input_align_combined_buf[0], size, &tag[0],
                        (CK_ULONG_PTR)&tag_lenth);
        }
    }

    /* Close session with KMS */
    (void)C_CloseSession(session);

    /* combine to a 32bit authentication word (MIC) */
    *cmac = (uint32_t)((uint32_t) tag[3] << 24 | (uint32_t) tag[2] << 16 | (uint32_t) tag[1] << 8 |
                       (uint32_t) tag[0]);

    if (rv != CKR_OK)
    {
        retval = SECURE_ELEMENT_ERROR;
    }
#endif /* LORAWAN_KMS */
    return retval;
}


/* Exported functions ---------------------------------------------------------*/

/*
 * API functions
 */
/* ST_WORKAROUND: Add unique ID callback as input parameter */
SecureElementStatus_t SecureElementInit( SecureElementNvmData_t *nvm, SecureElementGetUniqueId seGetUniqueId )
{
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    SecureElementNvmData_t seNvmInit =
    {
        /*!
        * end-device IEEE EUI (big endian)
        *
        * \remark In this application the value is automatically generated by
        *         calling BoardGetUniqueId function
        */
        .DevEui = LORAWAN_DEVICE_EUI,
        /*!
        * App/Join server IEEE EUI (big endian)
        */
        .JoinEui = LORAWAN_JOIN_EUI,
        /*!
        * LoRaWAN key list
        */
        .KeyList = SOFT_SE_KEY_LIST
    };


    if( nvm == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    // Initialize nvm pointer
    SeNvm = nvm;

    // Initialize data
    memcpy1( ( uint8_t* )SeNvm, ( uint8_t* )&seNvmInit, sizeof( seNvmInit ) );
#else /* LORAWAN_KMS == 1 */
    uint8_t itr = 0;
    uint8_t devEUI[SE_EUI_SIZE] = LORAWAN_DEVICE_EUI;
    uint8_t joinEUI[SE_EUI_SIZE] = LORAWAN_JOIN_EUI;

    if (nvm == NULL)
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    /* Initialize nvm pointer */
    SeNvm = nvm;

    /* Initialize with defaults */
    memcpy1((uint8_t *)SeNvm->DevEui, devEUI, SE_EUI_SIZE);
    memcpy1((uint8_t *)SeNvm->JoinEui, joinEUI, SE_EUI_SIZE);

    SeNvm->KeyList[itr++].KeyID = APP_KEY;
    SeNvm->KeyList[itr++].KeyID = NWK_KEY;
#if ( USE_LRWAN_1_1_X_CRYPTO == 1 )
    SeNvm->KeyList[itr++].KeyID = J_S_INT_KEY;
    SeNvm->KeyList[itr++].KeyID = J_S_ENC_KEY;
    SeNvm->KeyList[itr++].KeyID = F_NWK_S_INT_KEY;
    SeNvm->KeyList[itr++].KeyID = S_NWK_S_INT_KEY;
    SeNvm->KeyList[itr++].KeyID = NWK_S_ENC_KEY;
#else /* USE_LRWAN_1_1_X_CRYPTO == 0 */
    SeNvm->KeyList[itr++].KeyID = NWK_S_KEY;
#endif /* USE_LRWAN_1_1_X_CRYPTO */
    SeNvm->KeyList[itr++].KeyID = APP_S_KEY;
    SeNvm->KeyList[itr++].KeyID = MC_ROOT_KEY;
    SeNvm->KeyList[itr++].KeyID = MC_KE_KEY;
    SeNvm->KeyList[itr++].KeyID = MC_KEY_0;
    SeNvm->KeyList[itr++].KeyID = MC_APP_S_KEY_0;
    SeNvm->KeyList[itr++].KeyID = MC_NWK_S_KEY_0;
#if ( LORAMAC_MAX_MC_CTX > 1 )
    SeNvm->KeyList[itr++].KeyID = MC_KEY_1;
    SeNvm->KeyList[itr++].KeyID = MC_APP_S_KEY_1;
    SeNvm->KeyList[itr++].KeyID = MC_NWK_S_KEY_1;
    SeNvm->KeyList[itr++].KeyID = MC_KEY_2;
    SeNvm->KeyList[itr++].KeyID = MC_APP_S_KEY_2;
    SeNvm->KeyList[itr++].KeyID = MC_NWK_S_KEY_2;
    SeNvm->KeyList[itr++].KeyID = MC_KEY_3;
    SeNvm->KeyList[itr++].KeyID = MC_APP_S_KEY_3;
    SeNvm->KeyList[itr++].KeyID = MC_NWK_S_KEY_3;
#endif /*LORAMAC_MAX_MC_CTX > 1 */
    SeNvm->KeyList[itr].KeyID = SLOT_RAND_ZERO_KEY;

    /* Delete all obsolete keys in NVM  */
    DeleteAllDynamicKeys();

    SecureElementSetObjHandler(APP_KEY, KMS_APP_KEY_OBJECT_HANDLE);
    SecureElementSetObjHandler(NWK_KEY, KMS_NWK_KEY_OBJECT_HANDLE);
    SecureElementSetObjHandler(APP_S_KEY, KMS_APP_S_KEY_OBJECT_HANDLE);
    SecureElementSetObjHandler(NWK_S_KEY, KMS_NWK_S_KEY_OBJECT_HANDLE);
#if ( LORAMAC_CLASSB_ENABLED == 1 )
    SecureElementSetObjHandler(SLOT_RAND_ZERO_KEY, KMS_ZERO_KEY_OBJECT_HANDLE);
#endif /* LORAMAC_CLASSB_ENABLED */
#endif /* LORAWAN_KMS */

#if (defined (KEY_EXTRACTABLE) && (KEY_EXTRACTABLE == 1))
    MW_LOG(TS_OFF, VLEVEL_M, "###### OTAA ######\r\n");
    PrintKey(APP_KEY);
    PrintKey(NWK_KEY);
    MW_LOG(TS_OFF, VLEVEL_M, "###### ABP  ######\r\n");
    PrintKey(APP_S_KEY);
    PrintKey(NWK_S_KEY);
#endif /* KEY_EXTRACTABLE */

#if !defined( SECURE_ELEMENT_PRE_PROVISIONED )
#if( STATIC_DEVICE_EUI == 0 )
    if (seGetUniqueId != NULL)
    {
        // Get a DevEUI from MCU unique ID
        seGetUniqueId(SeNvm->DevEui);
    }
#endif /* STATIC_DEVICE_EUI */
#endif /* !SECURE_ELEMENT_PRE_PROVISIONED */
    return SECURE_ELEMENT_SUCCESS;
}

/* ST_WORKAROUND_BEGIN: Add KMS specific functions */
SecureElementStatus_t SecureElementGetKeyByID( KeyIdentifier_t keyID, Key_t **keyItem )
{
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
#if (defined (KEY_EXTRACTABLE) && (KEY_EXTRACTABLE == 1))
    for (uint8_t i = 0; i < NUM_OF_KEYS; i++)
    {
        if (SeNvm->KeyList[i].KeyID == keyID)
        {
            *keyItem = &(SeNvm->KeyList[i]);
            return SECURE_ELEMENT_SUCCESS;
        }
    }
#endif /* KEY_EXTRACTABLE */
    return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
#else /* LORAWAN_KMS == 1 */
    CK_RV rv;
    CK_SESSION_HANDLE session;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
    CK_OBJECT_HANDLE key_handle = (CK_OBJECT_HANDLE)(~0UL);
    CK_ULONG derive_key_template_class = CKO_SECRET_KEY;
    uint8_t extractable_key[16] = {0};
    CK_ATTRIBUTE key_attribute_template = {CKA_VALUE, (CK_VOID_PTR) &derive_key_template_class, 16UL};
    uint8_t index_keylist = 0;
    for (index_keylist = 0; index_keylist < NUM_OF_KEYS; index_keylist++)
    {
        if (SeNvm->KeyList[index_keylist].KeyID == keyID)
        {
            key_handle = SeNvm->KeyList[index_keylist].Object_Index;
            break;
        }
    }
    if (key_handle == (CK_OBJECT_HANDLE)(~0UL))
    {
        return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
    }

    /* Open session with KMS */
    rv = C_OpenSession(0, session_flags, NULL, 0, &session);

    /* Get key to display */
    if (rv == CKR_OK)
    {
        key_attribute_template.pValue = extractable_key;
        rv = C_GetAttributeValue(session, key_handle, &key_attribute_template, 1UL);
    }
    if (rv == CKR_OK)
    {
        memcpy1(SeNvm->KeyList[index_keylist].KeyValue, extractable_key, sizeof(extractable_key));
        *keyItem = &(SeNvm->KeyList[index_keylist]);
    }

    /* Close sessions */
    (void)C_CloseSession(session);

    if (rv != CKR_OK)
    {
        return SECURE_ELEMENT_ERROR;
    }
    return SECURE_ELEMENT_SUCCESS;

#endif /* LORAWAN_KMS == 1 */
}

SecureElementStatus_t SecureElementDeleteDynamicKeys( KeyIdentifier_t keyID, uint32_t *key_label )
{
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    return SECURE_ELEMENT_ERROR;
#else /* LORAWAN_KMS == 1 */
    CK_RV rv;
    CK_SESSION_HANDLE session;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;    /* Read ONLY session */
    CK_OBJECT_HANDLE hObject[NUM_OF_KEYS];
    CK_ULONG local_template_label[] = {GlobalTemplateLabel, 0UL};
    CK_ATTRIBUTE dynamic_key_template = {CKA_LABEL, (CK_VOID_PTR)local_template_label, sizeof(local_template_label)};
    uint32_t ulCount = 0;

    if (SECURE_ELEMENT_SUCCESS != GetSpecificLabelByID(keyID, &local_template_label[1]))
    {
        return SECURE_ELEMENT_ERROR;
    }
    *key_label = local_template_label[1];

    /* Open session with KMS */
    rv = C_OpenSession(0,    session_flags, NULL, 0, &session);

    /* Search from Template pattern */
    if (rv == CKR_OK)
    {
        rv = C_FindObjectsInit(session, &dynamic_key_template, sizeof(dynamic_key_template) / sizeof(CK_ATTRIBUTE));
    }

    /* Find all existing keys handle Template pattern */
    if (rv == CKR_OK)
    {
        rv = C_FindObjects(session, hObject, NUM_OF_KEYS, (CK_ULONG *) &ulCount);
    }

    if (rv == CKR_OK)
    {
        rv = C_FindObjectsFinal(session);
    }

    if (ulCount <= NUM_OF_KEYS)
    {
        for (uint8_t i = 0; i < ulCount; i++)
        {
            if (rv == CKR_OK)
            {
                rv = C_DestroyObject(session, hObject[i]);
            }
        }
    }

    /* Close sessions */
    if (session > 0)
    {
        (void)C_CloseSession(session);
    }

    if (rv != CKR_OK)
    {
        return SECURE_ELEMENT_ERROR;
    }
    return SECURE_ELEMENT_SUCCESS;
#endif /* LORAWAN_KMS == 1 */
}

SecureElementStatus_t SecureElementSetObjHandler( KeyIdentifier_t keyID, uint32_t keyIndex )
{
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    return SECURE_ELEMENT_ERROR;
#else /* LORAWAN_KMS == 1 */
    for (uint8_t i = 0; i < NUM_OF_KEYS; i++)
    {
        if (SeNvm->KeyList[i].KeyID == keyID)
        {
            SeNvm->KeyList[i].Object_Index = (CK_OBJECT_HANDLE) keyIndex;
            return SECURE_ELEMENT_SUCCESS;
        }
    }
    return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
#endif /* LORAWAN_KMS */
}
/* ST_WORKAROUND_END */

SecureElementStatus_t SecureElementSetKey( KeyIdentifier_t keyID, uint8_t* key )
{
    if( key == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    for( uint8_t i = 0; i < NUM_OF_KEYS; i++ )
    {
        if( SeNvm->KeyList[i].KeyID == keyID )
        {
            /* ST_WORKAROUND_BEGIN: reduced LORAMAC_MAX_MC_CTX */
#if ( LORAMAC_MAX_MC_CTX == 1 )
            if ( keyID == MC_KEY_0 )
#else /* LORAMAC_MAX_MC_CTX > 1 */
            if( ( keyID == MC_KEY_0 ) || ( keyID == MC_KEY_1 ) || ( keyID == MC_KEY_2 ) || ( keyID == MC_KEY_3 ) )
#endif /* LORAMAC_MAX_MC_CTX */
            /* ST_WORKAROUND_END */
            {  // Decrypt the key if its a Mckey
                SecureElementStatus_t retval           = SECURE_ELEMENT_ERROR;
                uint8_t               decryptedKey[16] = { 0 };

                retval = SecureElementAesEncrypt( key, 16, MC_KE_KEY, decryptedKey );

                memcpy1( SeNvm->KeyList[i].KeyValue, decryptedKey, SE_KEY_SIZE );
                return retval;
            }
            else
            {
                memcpy1( SeNvm->KeyList[i].KeyValue, key, SE_KEY_SIZE );
                return SECURE_ELEMENT_SUCCESS;
            }
        }
    }

    return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
#else /* LORAWAN_KMS == 1 */
    SecureElementStatus_t retval = SECURE_ELEMENT_ERROR;
    CK_RV rv;
    CK_SESSION_HANDLE session;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
    CK_OBJECT_HANDLE key_handle;
    CK_ULONG template_class = CKO_SECRET_KEY;
    CK_ULONG template_type = CKK_AES;
#if (defined (KEY_EXTRACTABLE) && (KEY_EXTRACTABLE == 1))
    CK_ULONG template_true = CK_TRUE;
#else
    CK_ULONG template_false = CK_TRUE;
#endif /* KEY_EXTRACTABLE */
    uint32_t key_ui32[] =
    {
        key[3]  | (key[2] << 8)  | (key[1] << 16)  | (key[0] << 24),
        key[7]  | (key[6] << 8)  | (key[5] << 16)  | (key[4] << 24),
        key[11] | (key[10] << 8) | (key[9] << 16)  | (key[8] << 24),
        key[15] | (key[14] << 8) | (key[13] << 16) | (key[12] << 24),
    };

    uint32_t specific_label[] = {GlobalTemplateLabel, 0UL};
    CK_ATTRIBUTE key_attribute_template[] =
    {
        { CKA_CLASS, (CK_VOID_PTR) &template_class,    sizeof(CK_BBOOL)      },
        { CKA_KEY_TYPE, (CK_VOID_PTR) &template_type,     sizeof(CK_BBOOL)      },
        { CKA_VALUE, (CK_VOID_PTR)key_ui32,           sizeof(key_ui32)      },
#if (defined (KEY_EXTRACTABLE) && (KEY_EXTRACTABLE == 1))
        { CKA_EXTRACTABLE, (CK_VOID_PTR) &template_true,     sizeof(CK_BBOOL)      },
#else
        { CKA_EXTRACTABLE, (CK_VOID_PTR) &template_false,    sizeof(CK_BBOOL)      },
#endif /* KEY_EXTRACTABLE */
        { CKA_LABEL, (CK_VOID_PTR)specific_label,     sizeof(specific_label)},
    };

    /* ST_WORKAROUND_BEGIN: reduced LORAMAC_MAX_MC_CTX */
#if ( LORAMAC_MAX_MC_CTX == 1 )
    if ( keyID == MC_KEY_0 )
#else /* LORAMAC_MAX_MC_CTX > 1 */
    if( ( keyID == MC_KEY_0 ) || ( keyID == MC_KEY_1 ) || ( keyID == MC_KEY_2 ) || ( keyID == MC_KEY_3 ) )
#endif /* LORAMAC_MAX_MC_CTX */
    /* ST_WORKAROUND_END */
    {  // Decrypt the key if its a Mckey
        uint8_t decryptedKey[16] = { 0 };
        if (SECURE_ELEMENT_SUCCESS != SecureElementAesEncrypt( key, 16, MC_KE_KEY, decryptedKey ))
        {
            return SECURE_ELEMENT_ERROR;
        }

        key_ui32[0] = decryptedKey[3]  | (decryptedKey[2] << 8)  | (decryptedKey[1] << 16)  | (decryptedKey[0] << 24);
        key_ui32[1] = decryptedKey[7]  | (decryptedKey[6] << 8)  | (decryptedKey[5] << 16)  | (decryptedKey[4] << 24);
        key_ui32[2] = decryptedKey[11] | (decryptedKey[10] << 8) | (decryptedKey[9] << 16)  | (decryptedKey[8] << 24);
        key_ui32[3] = decryptedKey[15] | (decryptedKey[14] << 8) | (decryptedKey[13] << 16) | (decryptedKey[12] << 24);
    }

    if (SECURE_ELEMENT_SUCCESS != GetKeyIndexByID(keyID, &key_handle))
    {
        return SECURE_ELEMENT_ERROR;
    }

    if (SECURE_ELEMENT_SUCCESS != SecureElementDeleteDynamicKeys(keyID, &specific_label[1]))
    {
        return SECURE_ELEMENT_ERROR;
    }

    /* Open session with KMS */
    rv = C_OpenSession(0, session_flags, NULL, 0, &session);

    /* Get key to display */
    if (rv == CKR_OK)
    {
        rv = C_CreateObject(session, key_attribute_template, sizeof(key_attribute_template) / sizeof(CK_ATTRIBUTE),
                            &key_handle);
    }

    if (rv == CKR_OK)
    {
        retval = SecureElementSetObjHandler(keyID, key_handle);
    }

    PrintKey(keyID);

    /* Close sessions */
    (void)C_CloseSession(session);

    if (rv != CKR_OK)
    {
        retval = SECURE_ELEMENT_ERROR;
    }
    return retval;
#endif /* LORAWAN_KMS */
}

SecureElementStatus_t SecureElementComputeAesCmac( uint8_t* micBxBuffer, uint8_t* buffer, uint16_t size,
                                                   KeyIdentifier_t keyID, uint32_t* cmac )
{
    if( keyID >= LORAMAC_CRYPTO_MULTICAST_KEYS )
    {
        // Never accept multicast key identifier for cmac computation
        return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
    }

    return ComputeCmac( micBxBuffer, buffer, size, keyID, cmac );
}

SecureElementStatus_t SecureElementVerifyAesCmac( uint8_t* buffer, uint16_t size, uint32_t expectedCmac,
                                                  KeyIdentifier_t keyID )
{
    if( buffer == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    SecureElementStatus_t retval   = SECURE_ELEMENT_ERROR;
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    uint32_t              compCmac = 0;
    retval                         = ComputeCmac( NULL, buffer, size, keyID, &compCmac );
    if( retval != SECURE_ELEMENT_SUCCESS )
    {
        return retval;
    }

    if( expectedCmac != compCmac )
    {
        retval = SECURE_ELEMENT_FAIL_CMAC;
    }
#else /* LORAWAN_KMS == 1 */
    CK_RV rv;
    CK_SESSION_HANDLE session;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;    /* Read ONLY session */
    CK_OBJECT_HANDLE object_handle;

    if (buffer == NULL)
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    /* AES CMAC Authentication variables */
    CK_MECHANISM aes_cmac_mechanism = { CKM_AES_CMAC, (CK_VOID_PTR)NULL, 0 };

    retval = GetKeyIndexByID(keyID, &object_handle);
    if (retval != SECURE_ELEMENT_SUCCESS)
    {
        return retval;
    }

    /* Open session with KMS */
    rv = C_OpenSession(0,    session_flags, NULL, 0, &session);

    /* Configure session to Verify the message in AES CMAC with settings included into the mechanism */
    if (rv == CKR_OK)
    {
        rv = C_VerifyInit(session, &aes_cmac_mechanism, object_handle);
    }

    /* Verify the message */
    if (rv == CKR_OK)
    {
        memcpy1(input_align_combined_buf, buffer, size);
        rv = C_Verify(session, (CK_BYTE_PTR)input_align_combined_buf, size, (CK_BYTE_PTR)&expectedCmac, 4);
    }

    (void)C_CloseSession(session);

    if (rv != CKR_OK)
    {
        retval = SECURE_ELEMENT_ERROR;
    }

#endif /* LORAWAN_KMS */

    return retval;
}

SecureElementStatus_t SecureElementAesEncrypt( uint8_t* buffer, uint16_t size, KeyIdentifier_t keyID,
                                               uint8_t* encBuffer )
{
    if( buffer == NULL || encBuffer == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    // Check if the size is divisible by 16,
    if( ( size % 16 ) != 0 )
    {
        return SECURE_ELEMENT_ERROR_BUF_SIZE;
    }

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    lorawan_aes_context aesContext;
    memset1( aesContext.ksch, '\0', 240 );

    Key_t*                pItem;
    SecureElementStatus_t retval = GetKeyByID( keyID, &pItem );

    if( retval == SECURE_ELEMENT_SUCCESS )
    {
        lorawan_aes_set_key(pItem->KeyValue, 16, &aesContext);

        uint8_t block = 0;

        while( size != 0 )
        {
            lorawan_aes_encrypt(&buffer[block], &encBuffer[block], &aesContext);
            block = block + 16;
            size  = size - 16;
        }
    }
#else /* LORAWAN_KMS == 1 */
    CK_RV rv;
    CK_SESSION_HANDLE session;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;    /* Read ONLY session */
    uint32_t encrypted_length = 0;
    CK_OBJECT_HANDLE object_handle;
    uint8_t dummy_tag[SE_KEY_SIZE] = {0};
    uint32_t dummy_tag_lenth = 0;

    CK_MECHANISM aes_ecb_mechanism = { CKM_AES_ECB, (CK_VOID_PTR *) NULL, 0 };

    SecureElementStatus_t retval = GetKeyIndexByID(keyID, &object_handle);
    if (retval != SECURE_ELEMENT_SUCCESS)
    {
        return retval;
    }

    /* Open session with KMS */
    rv = C_OpenSession(0,    session_flags, NULL, 0, &session);

    /* Configure session to encrypt message in AES ECB with settings included into the mechanism */
    if (rv == CKR_OK)
    {
        rv = C_EncryptInit(session, &aes_ecb_mechanism, object_handle);
    }

    /* Encrypt clear message */
    if (rv == CKR_OK)
    {
        memcpy1(input_align_combined_buf, buffer, size);
        encrypted_length = sizeof(output_align);
        rv = C_EncryptUpdate(session, (CK_BYTE_PTR)input_align_combined_buf, size,
                             output_align, (CK_ULONG_PTR)&encrypted_length);
        memcpy1(encBuffer, output_align, size);
    }

    /* In this case C_EncryptFinal is just called to Free the Alloc mem */
    if (rv == CKR_OK)
    {
        dummy_tag_lenth = sizeof(tag);
        rv = C_EncryptFinal(session, &dummy_tag[0], (CK_ULONG_PTR)&dummy_tag_lenth);
    }

    /* Close session with KMS */
    (void)C_CloseSession(session);

    if (rv != CKR_OK)
    {
        retval = SECURE_ELEMENT_ERROR;
    }
#endif /* LORAWAN_KMS */

    return retval;
}

SecureElementStatus_t SecureElementDeriveAndStoreKey( uint8_t* input, KeyIdentifier_t rootKeyID,
                                                      KeyIdentifier_t targetKeyID )
{
    if( input == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    SecureElementStatus_t retval  = SECURE_ELEMENT_ERROR;

    // In case of MC_KE_KEY, only McRootKey can be used as root key
    if( targetKeyID == MC_KE_KEY )
    {
        if( rootKeyID != MC_ROOT_KEY )
        {
            return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
        }
    }

#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    uint8_t key[16] = { 0 };
    // Derive key
    retval = SecureElementAesEncrypt( input, 16, rootKeyID, key );
    if( retval != SECURE_ELEMENT_SUCCESS )
    {
        return retval;
    }

    // Store key
    retval = SecureElementSetKey( targetKeyID, key );
    if( retval != SECURE_ELEMENT_SUCCESS )
    {
        return retval;
    }

    return SECURE_ELEMENT_SUCCESS;
#else /* LORAWAN_KMS == 1 */
    CK_RV rv;
    CK_SESSION_HANDLE session;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;    /* Read ONLY session */
    /* Key derivation */
    CK_MECHANISM            mech = {CKM_AES_ECB_ENCRYPT_DATA, input, SE_KEY_SIZE};
    CK_OBJECT_HANDLE  derived_object_handle;
    CK_OBJECT_HANDLE    rootkey_object_handle;
    uint32_t specific_label[] = {GlobalTemplateLabel, 0UL};
    CK_ATTRIBUTE DeriveKey_template = {CKA_LABEL, (CK_VOID_PTR)specific_label, sizeof(specific_label)};

    /* Derive key */
    if (SECURE_ELEMENT_SUCCESS != GetKeyIndexByID(rootKeyID, &rootkey_object_handle))
    {
        return SECURE_ELEMENT_ERROR;
    }

    if (SECURE_ELEMENT_SUCCESS != GetKeyIndexByID(targetKeyID, &derived_object_handle))
    {
        return SECURE_ELEMENT_ERROR;
    }

    if (SECURE_ELEMENT_SUCCESS != SecureElementDeleteDynamicKeys(targetKeyID, &specific_label[1]))
    {
        return SECURE_ELEMENT_ERROR;
    }

    /* Open session with KMS */
    rv = C_OpenSession(0,    session_flags, NULL, 0, &session);

    /* Derive key with pass phrase */
    if (rv == CKR_OK)
    {
        rv = C_DeriveKey(session, &(mech), rootkey_object_handle,
                         &DeriveKey_template, sizeof(DeriveKey_template) / sizeof(CK_ATTRIBUTE), &derived_object_handle);
    }

    if (rv == CKR_OK)
    {
        /* Store Derived Index in table */
        retval = SecureElementSetObjHandler(targetKeyID, derived_object_handle);
    }

    PrintKey(targetKeyID);

    /* Close session with KMS */
    (void)C_CloseSession(session);

    if (rv != CKR_OK)
    {
        retval = SECURE_ELEMENT_ERROR;
    }
    return retval;
#endif /* LORAWAN_KMS */
}

SecureElementStatus_t SecureElementProcessJoinAccept( JoinReqIdentifier_t joinReqType, uint8_t* joinEui,
                                                      uint16_t devNonce, uint8_t* encJoinAccept,
                                                      uint8_t encJoinAcceptSize, uint8_t* decJoinAccept,
                                                      uint8_t* versionMinor )
{
    if( ( encJoinAccept == NULL ) || ( decJoinAccept == NULL ) || ( versionMinor == NULL ) )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    // Check that frame size isn't bigger than a JoinAccept with CFList size
    if( encJoinAcceptSize > LORAMAC_JOIN_ACCEPT_FRAME_MAX_SIZE )
    {
        return SECURE_ELEMENT_ERROR_BUF_SIZE;
    }

    // Determine decryption key
    KeyIdentifier_t encKeyID = NWK_KEY;

    /* ST_WORKAROUND_BEGIN: Keep NWK_KEY if 1.1.x keys are not defined */
#if ( USE_LRWAN_1_1_X_CRYPTO == 1 )
    if( joinReqType != JOIN_REQ )
    {
        encKeyID = J_S_ENC_KEY;
    }
#endif /* USE_LRWAN_1_1_X_CRYPTO == 1 */

    memcpy1( decJoinAccept, encJoinAccept, encJoinAcceptSize );

    // Decrypt JoinAccept, skip MHDR
    if( SecureElementAesEncrypt( encJoinAccept + LORAMAC_MHDR_FIELD_SIZE, encJoinAcceptSize - LORAMAC_MHDR_FIELD_SIZE,
                                 encKeyID, decJoinAccept + LORAMAC_MHDR_FIELD_SIZE ) != SECURE_ELEMENT_SUCCESS )
    {
        return SECURE_ELEMENT_FAIL_ENCRYPT;
    }

    *versionMinor = ( ( decJoinAccept[11] & 0x80 ) == 0x80 ) ? 1 : 0;

    uint32_t mic = 0;

    mic = ( ( uint32_t ) decJoinAccept[encJoinAcceptSize - LORAMAC_MIC_FIELD_SIZE] << 0 );
    mic |= ( ( uint32_t ) decJoinAccept[encJoinAcceptSize - LORAMAC_MIC_FIELD_SIZE + 1] << 8 );
    mic |= ( ( uint32_t ) decJoinAccept[encJoinAcceptSize - LORAMAC_MIC_FIELD_SIZE + 2] << 16 );
    mic |= ( ( uint32_t ) decJoinAccept[encJoinAcceptSize - LORAMAC_MIC_FIELD_SIZE + 3] << 24 );

    //  - Header buffer to be used for MIC computation
    //        - LoRaWAN 1.0.x : micHeader = [MHDR(1)]
    //        - LoRaWAN 1.1.x : micHeader = [JoinReqType(1), JoinEUI(8), DevNonce(2), MHDR(1)]

    // Verify mic
    if( *versionMinor == 0 )
    {
        // For LoRaWAN 1.0.x
        //   cmac = aes128_cmac(NwkKey, MHDR |  JoinNonce | NetID | DevAddr | DLSettings | RxDelay | CFList |
        //   CFListType)
        if( SecureElementVerifyAesCmac( decJoinAccept, ( encJoinAcceptSize - LORAMAC_MIC_FIELD_SIZE ), mic, NWK_KEY ) !=
            SECURE_ELEMENT_SUCCESS )
        {
            return SECURE_ELEMENT_FAIL_CMAC;
        }
    }
#if( USE_LRWAN_1_1_X_CRYPTO == 1 )
    else if( *versionMinor == 1 )
    {
        uint8_t  micHeader11[JOIN_ACCEPT_MIC_COMPUTATION_OFFSET] = { 0 };
        uint16_t bufItr                                          = 0;

        micHeader11[bufItr++] = ( uint8_t ) joinReqType;

        memcpyr( micHeader11 + bufItr, joinEui, LORAMAC_JOIN_EUI_FIELD_SIZE );
        bufItr += LORAMAC_JOIN_EUI_FIELD_SIZE;

        micHeader11[bufItr++] = devNonce & 0xFF;
        micHeader11[bufItr++] = ( devNonce >> 8 ) & 0xFF;

        // For LoRaWAN 1.1.x and later:
        //   cmac = aes128_cmac(JSIntKey, JoinReqType | JoinEUI | DevNonce | MHDR | JoinNonce | NetID | DevAddr |
        //   DLSettings | RxDelay | CFList | CFListType)
        // Prepare the msg for integrity check (adding JoinReqType, JoinEUI and DevNonce)
        uint8_t localBuffer[LORAMAC_JOIN_ACCEPT_FRAME_MAX_SIZE + JOIN_ACCEPT_MIC_COMPUTATION_OFFSET] = { 0 };

        memcpy1( localBuffer, micHeader11, JOIN_ACCEPT_MIC_COMPUTATION_OFFSET );
        memcpy1( localBuffer + JOIN_ACCEPT_MIC_COMPUTATION_OFFSET - 1, decJoinAccept, encJoinAcceptSize );

        if( SecureElementVerifyAesCmac( localBuffer,
                                        encJoinAcceptSize + JOIN_ACCEPT_MIC_COMPUTATION_OFFSET -
                                            LORAMAC_MHDR_FIELD_SIZE - LORAMAC_MIC_FIELD_SIZE,
                                        mic, J_S_INT_KEY ) != SECURE_ELEMENT_SUCCESS )
        {
            return SECURE_ELEMENT_FAIL_CMAC;
        }
    }
#endif /* USE_LRWAN_1_1_X_CRYPTO == 1 */
    else
    {
        return SECURE_ELEMENT_ERROR_INVALID_LORAWAM_SPEC_VERSION;
    }

    return SECURE_ELEMENT_SUCCESS;
}

SecureElementStatus_t SecureElementRandomNumber( uint32_t* randomNum )
{
    if( randomNum == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }
    *randomNum = Radio.Random();
    return SECURE_ELEMENT_SUCCESS;
}

SecureElementStatus_t SecureElementSetDevEui( uint8_t* devEui )
{
    if( devEui == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }
    memcpy1( SeNvm->DevEui, devEui, SE_EUI_SIZE );
    return SECURE_ELEMENT_SUCCESS;
}

uint8_t* SecureElementGetDevEui( void )
{
    return SeNvm->DevEui;
}

SecureElementStatus_t SecureElementSetJoinEui( uint8_t* joinEui )
{
    if( joinEui == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }
    memcpy1( SeNvm->JoinEui, joinEui, SE_EUI_SIZE );
    return SECURE_ELEMENT_SUCCESS;
}

uint8_t* SecureElementGetJoinEui( void )
{
    return SeNvm->JoinEui;
}

