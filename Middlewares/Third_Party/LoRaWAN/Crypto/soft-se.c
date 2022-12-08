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
#include "LoRaMacVersion.h"

#include "secure-element.h"
#include "secure-element-nvm.h"
#include "se-identity.h"

/* Private constants ---------------------------------------------------------*/
#ifndef LORAWAN_KMS
#define LORAWAN_KMS 0
#endif /* LORAWAN_KMS */

#ifndef KEY_EXTRACTABLE
#define KEY_EXTRACTABLE 0
#endif /* KEY_EXTRACTABLE */
/*!
 * MIC computation offset
 * \remark required for 1.1.x support
 */
#define CRYPTO_MIC_COMPUTATION_OFFSET ( JOIN_REQ_TYPE_SIZE\
                                        + LORAMAC_JOIN_EUI_FIELD_SIZE + DEV_NONCE_SIZE + LORAMAC_MHDR_FIELD_SIZE )

#if (LORAWAN_KMS == 0)
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

/*Can be overloaded in lorawan_conf.h*/
#ifndef SOFT_SE_PLACE_IN_NVM_START
#define SOFT_SE_PLACE_IN_NVM_START
#endif /* SOFT_SE_PLACE_IN_NVM_START */
#ifndef SOFT_SE_PLACE_IN_NVM_STOP
#define SOFT_SE_PLACE_IN_NVM_STOP
#endif /* SOFT_SE_PLACE_IN_NVM_STOP */

/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(b,i)                        \
    (   (uint32_t) (b)[(i)    ] << 24 )           \
    | ( (uint32_t) (b)[(i) + 1] << 16 )           \
    | ( (uint32_t) (b)[(i) + 2] <<  8 )           \
    | ( (uint32_t) (b)[(i) + 3]       )
#endif /* GET_UINT32_BE */

/*
 * 32-bit integer manipulation macros (little endian)
 */
#ifndef GET_UINT32_LE
#define GET_UINT32_LE(b,i)                        \
    (   (uint32_t) (b)[(i)    ]       )           \
    | ( (uint32_t) (b)[(i) + 1] <<  8 )           \
    | ( (uint32_t) (b)[(i) + 2] << 16 )           \
    | ( (uint32_t) (b)[(i) + 3] << 24 )
#endif /* GET_UINT32_LE */

/*
 * Get the number of attributes in the structure
 */
#ifndef GET_NB_ATTRIBUTE
#define GET_NB_ATTRIBUTE(s) (sizeof(s) / sizeof(CK_ATTRIBUTE))
#endif /* GET_NB_ATTRIBUTE */

/* Private Types -------------------------------------------------------------*/
typedef struct SecureElementKeyLabel
{
    KeyIdentifier_t keyID;
    uint32_t keyLabel;
    char *keyStr;
} SecureElementKeyLabel_t;

/* Private variables ---------------------------------------------------------*/
/*!
 * Secure element context
 */
static SecureElementNvmData_t *SeNvm;

#if ((LORAWAN_KMS == 1) || (KEY_EXTRACTABLE == 1))
static const SecureElementKeyLabel_t KeyLabel[NUM_OF_KEYS] =
{
    {APP_KEY, 0x5F505041U, "AppKey:     "},
    {NWK_KEY, 0x5F4B574EU, "NwkKey:     "},
#if (LORAWAN_KMS == 1)
    {DEV_JOIN_EUI_ADDR_KEY, 0x5F564544U, ""},
#endif /* LORAWAN_KMS */
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    {J_S_INT_KEY, 0x314B574EU, "JSIntKey:   "},
    {J_S_ENC_KEY, 0x324B574EU, "JSEncKey:   "},
    {F_NWK_S_INT_KEY, 0x334B574EU, "FNwkSIntKey:"},
    {S_NWK_S_INT_KEY, 0x344B574EU, "SNwkSIntKey:"},
    {NWK_S_ENC_KEY, 0x354B574EU, "NwkSEncKey: "},
#else
    {NWK_S_KEY, 0x534B574EU, "NwkSKey:    "},
#endif /* LORAMAC_VERSION */
    {APP_S_KEY, 0x53505041U, "AppSKey:    "},
    {DATABLOCK_INT_KEY, 0x5F494244U, "DBIntKey:   "},
    {MC_ROOT_KEY, 0x5452434DU, "MCRootKey:  "},
    {MC_KE_KEY, 0x454B434DU, "MCKEKey:    "},
#if ( LORAMAC_MAX_MC_CTX > 0 )
    {MC_KEY_0, 0x304B434DU, "MCAppSKey_0:"},
    {MC_APP_S_KEY_0, 0x3053414DU, "MCAppSKey_0:"},
    {MC_NWK_S_KEY_0, 0x30534E4DU, "MCNwkSKey_0:"},
#endif /* LORAMAC_MAX_MC_CTX > 0 */
#if ( LORAMAC_MAX_MC_CTX > 1 )
    {MC_KEY_1, 0x314B434DU, "MCAppSKey_1:"},
    {MC_APP_S_KEY_1, 0x3153414DU, "MCAppSKey_1:"},
    {MC_NWK_S_KEY_1, 0x31534E4DU, "MCNwkSKey_1:"},
#endif /* LORAMAC_MAX_MC_CTX > 1 */
#if ( LORAMAC_MAX_MC_CTX > 2 )
    {MC_KEY_2, 0x324B434DU, "MCAppSKey_2:"},
    {MC_APP_S_KEY_2, 0x3253414DU, "MCAppSKey_2:"},
    {MC_NWK_S_KEY_2, 0x32534E4DU, "MCNwkSKey_2:"},
#endif /* LORAMAC_MAX_MC_CTX > 2 */
#if ( LORAMAC_MAX_MC_CTX > 3 )
    {MC_KEY_3, 0x334B434DU, "MCAppSKey_3:"},
    {MC_APP_S_KEY_3, 0x3353414DU, "MCAppSKey_3:"},
    {MC_NWK_S_KEY_3, 0x33534E4DU, "MCNwkSKey_3:"},
#endif /* LORAMAC_MAX_MC_CTX > 3 */
    {SLOT_RAND_ZERO_KEY, 0x4F52455AU, ""}
};
#endif /* (LORAWAN_KMS == 1) || (KEY_EXTRACTABLE == 1) */

#if (LORAWAN_KMS == 0)
/*
 * if defined, place seNvmInit in  memory
 * this allows device commissioning
 */
SOFT_SE_PLACE_IN_NVM_START
static const SecureElementNvmData_t seNvmInit =
{
    SOFT_SE_ID_LIST,
    .KeyList = SOFT_SE_KEY_LIST,
};
SOFT_SE_PLACE_IN_NVM_STOP
#else /* LORAWAN_KMS == 1 */
static Key_t KeyList[NUM_OF_KEYS] =
{
    {APP_KEY, KMS_APP_KEY_OBJECT_HANDLE},
    {NWK_KEY, KMS_NWK_KEY_OBJECT_HANDLE},
    {DEV_JOIN_EUI_ADDR_KEY, KMS_DEVJOINEUIADDR_KEY_OBJECT_HANDLE},
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    {J_S_INT_KEY, ( CK_OBJECT_HANDLE )( ~0UL )},
    {J_S_ENC_KEY, ( CK_OBJECT_HANDLE )( ~0UL )},
    {F_NWK_S_INT_KEY, ( CK_OBJECT_HANDLE )( ~0UL )},
    {S_NWK_S_INT_KEY, ( CK_OBJECT_HANDLE )( ~0UL )},
    {NWK_S_ENC_KEY, ( CK_OBJECT_HANDLE )( ~0UL )},
#else
    {NWK_S_KEY, KMS_NWK_S_KEY_OBJECT_HANDLE},
#endif /* LORAMAC_VERSION */
    {APP_S_KEY, KMS_APP_S_KEY_OBJECT_HANDLE},
    {DATABLOCK_INT_KEY, ( CK_OBJECT_HANDLE )( ~0UL )},
    {MC_ROOT_KEY, ( CK_OBJECT_HANDLE )( ~0UL )},
    {MC_KE_KEY, ( CK_OBJECT_HANDLE )( ~0UL )},
#if ( LORAMAC_MAX_MC_CTX > 0 )
    {MC_KEY_0, ( CK_OBJECT_HANDLE )( ~0UL )},
    {MC_APP_S_KEY_0, ( CK_OBJECT_HANDLE )( ~0UL )},
    {MC_NWK_S_KEY_0, ( CK_OBJECT_HANDLE )( ~0UL )},
#endif /* LORAMAC_MAX_MC_CTX > 0 */
#if ( LORAMAC_MAX_MC_CTX > 1 )
    {MC_KEY_1, ( CK_OBJECT_HANDLE )( ~0UL )},
    {MC_APP_S_KEY_1, ( CK_OBJECT_HANDLE )( ~0UL )},
    {MC_NWK_S_KEY_1, ( CK_OBJECT_HANDLE )( ~0UL )},
#endif /* LORAMAC_MAX_MC_CTX > 1 */
#if ( LORAMAC_MAX_MC_CTX > 2 )
    {MC_KEY_2, ( CK_OBJECT_HANDLE )( ~0UL )},
    {MC_APP_S_KEY_2, ( CK_OBJECT_HANDLE )( ~0UL )},
    {MC_NWK_S_KEY_2, ( CK_OBJECT_HANDLE )( ~0UL )},
#endif /* LORAMAC_MAX_MC_CTX > 2 */
#if ( LORAMAC_MAX_MC_CTX > 3 )
    {MC_KEY_3, ( CK_OBJECT_HANDLE )( ~0UL )},
    {MC_APP_S_KEY_3, ( CK_OBJECT_HANDLE )( ~0UL )},
    {MC_NWK_S_KEY_3, ( CK_OBJECT_HANDLE )( ~0UL )},
#endif /*LORAMAC_MAX_MC_CTX > 3 */
    {SLOT_RAND_ZERO_KEY, KMS_ZERO_KEY_OBJECT_HANDLE}
};

/* WARNING: Should be modified at the end of product development */
static const CK_ULONG GlobalTemplateLabel = 0x444E524CU;

/*
 * Intermediate buffer used for two reasons:
 * - align to 32 bits and
 * - for Cmac combine InitVector + input buff
 */
static uint8_t input_align_combined_buf[PAYLOAD_MAX_SIZE + SE_KEY_SIZE] ALIGN( 4 );

static uint8_t output_align[PAYLOAD_MAX_SIZE] ALIGN( 4 );

static uint8_t tag[SE_KEY_SIZE] ALIGN( 4 ) = {0};
#endif /* LORAWAN_KMS */

/* Private functions prototypes ---------------------------------------------------*/
#if (LORAWAN_KMS == 0)
/*
 * Gets key item from key list.
 *
 * \param [in] keyID          - Key identifier
 * \param [out] keyItem       - Key item reference
 * \retval                    - Status of the operation
 */
static SecureElementStatus_t GetKeyByID( KeyIdentifier_t keyID, Key_t **keyItem );
#else /* LORAWAN_KMS == 1 */
/*
 * Gets key index from key list in KMS table
 *
 * \param [in] keyID          - Key identifier
 * \param [out] keyIndex      - Key item reference
 * \retval                    - Status of the operation
 */
static SecureElementStatus_t GetKeyIndexByID( KeyIdentifier_t keyID, CK_OBJECT_HANDLE *keyIndex );

/*
 * Gets key label from key list in KMS table
 *
 * \param [in] keyID          - Key identifier
 * \param [out] keyLabel      - Key label reference
 * \retval                    - Status of the operation
 */
static SecureElementStatus_t GetSpecificLabelByID( KeyIdentifier_t keyID, uint32_t *keyLabel );
#endif /* LORAWAN_KMS */

/*
 * Extract and print the key content
 *
 * \param [in] keyID          - Key identifier
 */
static void PrintKey( KeyIdentifier_t keyID );

/*
 * Extract and print the IDs information
 *
 * \param [in] mode           - Mode used to get the associated DevAddr value
 */
static void PrintIds( ActivationType_t mode );

/*
 * Computes a CMAC of a message using provided initial Bx block
 *
 *  cmac = aes128_cmac(keyID, blocks[i].Buffer)
 *
 * \param [in] micBxBuffer    - Buffer containing the initial Bx block
 * \param [in] buffer         - Data buffer
 * \param [in] size           - Data buffer size
 * \param [in] keyID          - Key identifier to determine the AES key to be used
 * \param [out] cmac          - Computed cmac
 * \retval                    - Status of the operation
 */
static SecureElementStatus_t ComputeCmac( uint8_t *micBxBuffer, uint8_t *buffer, uint32_t size, KeyIdentifier_t keyID,
                                          uint32_t *cmac );

/* Private functions ---------------------------------------------------------*/
static void PrintKey( KeyIdentifier_t keyID )
{
#if (KEY_EXTRACTABLE == 1)
#if (LORAWAN_KMS == 0)
    Key_t *keyItem;
    if( SECURE_ELEMENT_SUCCESS == SecureElementGetKeyByID( keyID, &keyItem ) )
#else
    uint8_t extractable_key[SE_KEY_SIZE] = {0};
    if( SECURE_ELEMENT_SUCCESS == SecureElementGetKeyByID( keyID, ( uint8_t * )extractable_key ) )
#endif /* LORAWAN_KMS */
    {
        for( uint8_t i = 0; i < NUM_OF_KEYS; i++ )
        {
            if( KeyLabel[i].keyID == keyID )
            {
#if (LORAWAN_KMS == 0)
                MW_LOG( TS_OFF, VLEVEL_M,
                        "###### %s %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                        KeyLabel[i].keyStr,
                        HEX16( keyItem->KeyValue ) );
#else
                MW_LOG( TS_OFF, VLEVEL_M,
                        "###### %s %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n",
                        KeyLabel[i].keyStr,
                        HEX16( extractable_key ) );
#endif /* LORAWAN_KMS */
                return;
            }
        }
    }
#endif /* KEY_EXTRACTABLE */
}

static void PrintIds( ActivationType_t mode )
{
    uint8_t joinEui[SE_EUI_SIZE];
    uint8_t devEui[SE_EUI_SIZE];
    uint32_t devAddr = 0;

    SecureElementGetDevEui( devEui );
    MW_LOG( TS_OFF, VLEVEL_M, "###### DevEUI:      %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n", HEX8( devEui ) );

    SecureElementGetJoinEui( joinEui );
    MW_LOG( TS_OFF, VLEVEL_M, "###### AppEUI:      %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X\r\n", HEX8( joinEui ) );

    SecureElementGetDevAddr( mode, &devAddr );
    MW_LOG( TS_OFF, VLEVEL_M, "###### DevAddr:     %02X:%02X:%02X:%02X\r\n",
            ( unsigned )( ( unsigned char * )( &devAddr ) )[3],
            ( unsigned )( ( unsigned char * )( &devAddr ) )[2],
            ( unsigned )( ( unsigned char * )( &devAddr ) )[1],
            ( unsigned )( ( unsigned char * )( &devAddr ) )[0] );
}

#if (LORAWAN_KMS == 0)
static SecureElementStatus_t GetKeyByID( KeyIdentifier_t keyID, Key_t **keyItem )
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
static SecureElementStatus_t GetKeyIndexByID( KeyIdentifier_t keyID, CK_OBJECT_HANDLE *keyIndex )
{
    if( keyIndex == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    for( uint8_t i = 0; i < NUM_OF_KEYS; i++ )
    {
        if( KeyList[i].KeyID == keyID )
        {
            *keyIndex = KeyList[i].Object_Index;
            return SECURE_ELEMENT_SUCCESS;
        }
    }
    return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
}

static SecureElementStatus_t GetSpecificLabelByID( KeyIdentifier_t keyID, uint32_t *keyLabel )
{
    if( keyLabel == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    for( uint8_t i = 0; i < NUM_OF_KEYS; i++ )
    {
        if( KeyLabel[i].keyID == keyID )
        {
            *keyLabel = KeyLabel[i].keyLabel;
            return SECURE_ELEMENT_SUCCESS;
        }
    }
    return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
}
#endif /* LORAWAN_KMS */

static SecureElementStatus_t ComputeCmac( uint8_t *micBxBuffer, uint8_t *buffer, uint32_t size, KeyIdentifier_t keyID,
                                          uint32_t *cmac )
{
    if( ( buffer == NULL ) || ( cmac == NULL ) )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

#if (LORAWAN_KMS == 0)
    uint8_t Cmac[16];
    AES_CMAC_CTX aesCmacCtx[1];

    AES_CMAC_Init( aesCmacCtx );

    Key_t                *keyItem;
    SecureElementStatus_t retval = GetKeyByID( keyID, &keyItem );

    if( retval == SECURE_ELEMENT_SUCCESS )
    {
        AES_CMAC_SetKey( aesCmacCtx, keyItem->KeyValue );

        if( micBxBuffer != NULL )
        {
            AES_CMAC_Update( aesCmacCtx, micBxBuffer, MIC_BLOCK_BX_SIZE );
        }

        AES_CMAC_Update( aesCmacCtx, buffer, size );

        AES_CMAC_Final( Cmac, aesCmacCtx );

        /* Bring into the required format */
        *cmac = GET_UINT32_LE( Cmac, 0 );
    }
#else /* LORAWAN_KMS == 1 */
    CK_RV rv;
    CK_SESSION_HANDLE session;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;    /* Read ONLY session */
    uint32_t tag_length = sizeof( tag );
    CK_OBJECT_HANDLE key_handle;
#if 0 /* require C_SignUpdate and C_SignFinal KMS implementation */
    uint32_t max_allocated_size = 0;
    uint32_t buffer_addr = ( uint32_t )buffer;
#endif /* 0 */

    /* AES CMAC Authentication variables */
    CK_MECHANISM aes_cmac_mechanism = { CKM_AES_CMAC, ( CK_VOID_PTR )NULL, 0 };

    SecureElementStatus_t retval = GetKeyIndexByID( keyID, &key_handle );
    if( retval != SECURE_ELEMENT_SUCCESS )
    {
        return retval;
    }

    /* Open session with KMS */
    rv = C_OpenSession( 0,    session_flags, NULL, 0, &session );

    /* Configure session to Authentication message in AES CMAC with settings included into the mechanism */
    if( rv == CKR_OK )
    {
        rv = C_SignInit( session, &aes_cmac_mechanism, key_handle );
    }

#if 1 /* require C_SignUpdate and C_SignFinal KMS implementation */
#if (LORAWAN_PACKAGES_VERSION == 2)
#warning the current implementation of ComputeCmac is not functional for LoRaMacProcessMicForDatablock method called by LmhpFragmentation due to memcpy overflow. \
need to replace this code as below with C_SignUpdate and C_SignFinal methods usage.
#endif /* LORAWAN_PACKAGES_VERSION */
    /* Encrypt clear message */
    if( rv == CKR_OK )
    {
        /* work around : need to double-check if possible to use micBxBuffer as IV for Sign */
        if( micBxBuffer != NULL )
        {
            memcpy1( ( uint8_t * ) &input_align_combined_buf[0], ( uint8_t * ) micBxBuffer, SE_KEY_SIZE );
            memcpy1( ( uint8_t * ) &input_align_combined_buf[SE_KEY_SIZE], ( uint8_t * ) buffer, size );
        }
        else
        {
            memcpy1( ( uint8_t * ) &input_align_combined_buf[0], ( uint8_t * ) buffer, size );
        }
    }

    if( rv == CKR_OK )
    {
        if( micBxBuffer != NULL )
        {
            rv = C_Sign( session, ( CK_BYTE_PTR )&input_align_combined_buf[0], size + SE_KEY_SIZE, &tag[0],
                         ( CK_ULONG_PTR )&tag_length );
        }
        else
        {
            rv = C_Sign( session, ( CK_BYTE_PTR )&input_align_combined_buf[0], size, &tag[0],
                         ( CK_ULONG_PTR )&tag_length );
        }
    }
#else
    /* Sign the partial start message if exists */
    if( rv == CKR_OK )
    {
        if( micBxBuffer != NULL )
        {
            rv = C_SignUpdate( session, ( CK_BYTE_PTR )micBxBuffer, MIC_BLOCK_BX_SIZE );
        }
    }

    if( ( buffer_addr % 4 ) == 0 ) /* buffer address is aligned */
    {
        /* Sign the full message */
        if( rv == CKR_OK )
        {
            rv = C_SignUpdate( session, ( CK_BYTE_PTR )buffer, size );
        }
    }
    else
    {
        /* Sign the message by block */
        while( ( size != 0 ) && ( rv == CKR_OK ) )
        {
            if( size > sizeof( input_align_combined_buf ) )
            {
                max_allocated_size = sizeof( input_align_combined_buf );
            }
            else
            {
                max_allocated_size = size;
            }

            memcpy1( ( uint8_t * ) input_align_combined_buf, ( uint8_t * ) buffer, max_allocated_size );
            rv = C_SignUpdate( session, ( CK_BYTE_PTR )input_align_combined_buf, max_allocated_size );
            size -= max_allocated_size;
        }
    }

    /* Finishes a multiple-part signature operation */
    if( rv == CKR_OK )
    {
        rv = C_SignFinal( session, tag, ( CK_ULONG_PTR )&tag_length );
    }
#endif /* 0 */

    /* Close session with KMS */
    ( void )C_CloseSession( session );

    /* combine to a 32bit authentication word (MIC) */
    *cmac = GET_UINT32_LE( tag, 0 );

    if( rv != CKR_OK )
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
SecureElementStatus_t SecureElementInit( SecureElementNvmData_t *nvm )
{
    if( nvm == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    /* Initialize nvm pointer */
    SeNvm = nvm;

#if (LORAWAN_KMS == 0)
    /* Initialize data */
    memcpy1( ( uint8_t * )SeNvm, ( uint8_t * )&seNvmInit, sizeof( seNvmInit ) );
#else /* LORAWAN_KMS == 1 */
    SeNvm->reserved = 0;
    CK_RV rv;
    CK_SESSION_HANDLE session;
    uint32_t ulCount;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
    CK_OBJECT_HANDLE hObject[NUM_OF_KEYS];
    CK_ULONG local_template_label[] = {GlobalTemplateLabel, 0UL};
    CK_ATTRIBUTE key_template = {CKA_LABEL, ( CK_VOID_PTR )local_template_label, sizeof( local_template_label )};

    /* Open session with KMS */
    rv = C_OpenSession( 0, session_flags, NULL, 0, &session );

    for( uint8_t itr = 0; itr < NUM_OF_KEYS; itr++ )
    {
        if( SECURE_ELEMENT_SUCCESS == GetSpecificLabelByID( KeyList[itr].KeyID, &local_template_label[1] ) )
        {
            ulCount = 0;
            /* Search from Template pattern */
            if( rv == CKR_OK )
            {
                rv = C_FindObjectsInit( session, &key_template, GET_NB_ATTRIBUTE( key_template ) );
            }

            /* Find all existing keys handle Template pattern */
            if( rv == CKR_OK )
            {
                rv = C_FindObjects( session, hObject, NUM_OF_KEYS, ( CK_ULONG * ) &ulCount );
            }

            if( rv == CKR_OK )
            {
                rv = C_FindObjectsFinal( session );
            }

            if( ( rv == CKR_OK ) && ( ulCount > 0 ) && ( ulCount <= NUM_OF_KEYS ) )
            {
                KeyList[itr].Object_Index = hObject[ulCount - 1];
            }
        }
    }
    /* Close sessions */
    if( session > 0 )
    {
        ( void )C_CloseSession( session );
    }

#endif /* LORAWAN_KMS */

    return SECURE_ELEMENT_SUCCESS;
}

SecureElementStatus_t SecureElementInitMcuID( SecureElementGetUniqueId_t seGetUniqueId,
                                              SecureElementGetDevAddr_t seGetDevAddr )
{
    uint8_t devEui[SE_EUI_SIZE];
    uint32_t devAddrABP = 0;

    SecureElementGetDevEui( devEui );
    SecureElementGetDevAddr( ACTIVATION_TYPE_ABP, &devAddrABP );

    if( seGetUniqueId != NULL )
    {
        bool id_init = false;
        for( uint8_t index = 0; index < SE_EUI_SIZE; index++ )
        {
            if( devEui[index] != 0 )
            {
                id_init = true;
                break;
            }
        }
        if( id_init == false )
        {
            /* Get a DevEUI from MCU unique ID */
            seGetUniqueId( devEui );
            SecureElementSetDevEui( devEui );
        }
    }

    if( ( seGetDevAddr != NULL ) && ( devAddrABP == 0 ) )
    {
        /* callback to dynamic DevAddr generation */
        seGetDevAddr( &devAddrABP );
        SecureElementSetDevAddr( ACTIVATION_TYPE_ABP, devAddrABP );
    }

    return SECURE_ELEMENT_SUCCESS;
}

#if (LORAWAN_KMS == 0)
SecureElementStatus_t SecureElementGetKeyByID( KeyIdentifier_t keyID, Key_t **keyItem )
{
#if (KEY_EXTRACTABLE == 1)
    for( uint8_t i = 0; i < NUM_OF_KEYS; i++ )
    {
        if( SeNvm->KeyList[i].KeyID == keyID )
        {
            *keyItem = &( SeNvm->KeyList[i] );
            return SECURE_ELEMENT_SUCCESS;
        }
    }
#endif /* KEY_EXTRACTABLE */
    return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
}
#else /* LORAWAN_KMS == 1 */
SecureElementStatus_t SecureElementGetKeyByID( KeyIdentifier_t keyID, uint8_t *extractable_key )
{
    CK_RV rv;
    CK_SESSION_HANDLE session;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
    CK_OBJECT_HANDLE key_handle = ( CK_OBJECT_HANDLE )( ~0UL );
    CK_ULONG derive_key_template_class = CKO_SECRET_KEY;
    uint32_t size = SE_KEY_SIZE;

    if( extractable_key == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    if( keyID == DEV_JOIN_EUI_ADDR_KEY )
    {
        size = SE_NVM_EUI_SIZE;
    }
    CK_ATTRIBUTE key_attribute_template = {CKA_VALUE, ( CK_VOID_PTR ) &derive_key_template_class, size};

    if( GetKeyIndexByID( keyID, &key_handle ) == SECURE_ELEMENT_ERROR_INVALID_KEY_ID )
    {
        return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
    }

    /* Open session with KMS */
    rv = C_OpenSession( 0, session_flags, NULL, 0, &session );

    /* Get key to display */
    if( rv == CKR_OK )
    {
        key_attribute_template.pValue = extractable_key;
        rv = C_GetAttributeValue( session, key_handle, &key_attribute_template, 1UL );
    }

    /* Close sessions */
    ( void )C_CloseSession( session );

    if( rv != CKR_OK )
    {
        return SECURE_ELEMENT_ERROR;
    }
    return SECURE_ELEMENT_SUCCESS;
}
#endif /* LORAWAN_KMS */

SecureElementStatus_t SecureElementPrintKeys( void )
{
    PrintKey( APP_KEY );
    PrintKey( NWK_KEY );
    PrintKey( APP_S_KEY );
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    PrintKey( F_NWK_S_INT_KEY );
    PrintKey( S_NWK_S_INT_KEY );
    PrintKey( NWK_S_ENC_KEY );
#else
    PrintKey( NWK_S_KEY );
#endif /* LORAMAC_VERSION */
    PrintIds( ACTIVATION_TYPE_NONE );

    return SECURE_ELEMENT_SUCCESS;
}

SecureElementStatus_t SecureElementPrintSessionKeys( ActivationType_t mode )
{
    PrintKey( MC_ROOT_KEY );
    PrintKey( MC_KE_KEY );
    PrintKey( APP_S_KEY );
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    PrintKey( F_NWK_S_INT_KEY );
    PrintKey( S_NWK_S_INT_KEY );
    PrintKey( NWK_S_ENC_KEY );
#else
    PrintKey( NWK_S_KEY );
#endif /* LORAMAC_VERSION */
    PrintKey( DATABLOCK_INT_KEY );
    PrintIds( mode );

    return SECURE_ELEMENT_SUCCESS;
}

SecureElementStatus_t SecureElementDeleteDynamicKey( KeyIdentifier_t keyID, uint32_t *key_label )
{
#if (LORAWAN_KMS == 0)
    return SECURE_ELEMENT_ERROR;
#else /* LORAWAN_KMS == 1 */
    CK_RV rv;
    CK_SESSION_HANDLE session;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;    /* Read ONLY session */
    CK_OBJECT_HANDLE hObject[NUM_OF_KEYS];
    CK_ULONG local_template_label[] = {GlobalTemplateLabel, 0UL};
    CK_ATTRIBUTE dynamic_key_template =
    {
        CKA_LABEL, ( CK_VOID_PTR )local_template_label, sizeof( local_template_label )
    };
    uint32_t ulCount = 0;

    if( key_label == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    if( SECURE_ELEMENT_SUCCESS != GetSpecificLabelByID( keyID, &local_template_label[1] ) )
    {
        return SECURE_ELEMENT_ERROR;
    }
    *key_label = local_template_label[1];

    /* Open session with KMS */
    rv = C_OpenSession( 0,    session_flags, NULL, 0, &session );

    /* Search from Template pattern */
    if( rv == CKR_OK )
    {
        rv = C_FindObjectsInit( session, &dynamic_key_template, GET_NB_ATTRIBUTE( dynamic_key_template ) );
    }

    /* Find all existing keys handle Template pattern */
    if( rv == CKR_OK )
    {
        rv = C_FindObjects( session, hObject, NUM_OF_KEYS, ( CK_ULONG * ) &ulCount );
    }

    if( rv == CKR_OK )
    {
        rv = C_FindObjectsFinal( session );
    }

    if( ulCount <= NUM_OF_KEYS )
    {
        for( uint8_t i = 0; i < ulCount; i++ )
        {
            if( rv == CKR_OK )
            {
                rv = C_DestroyObject( session, hObject[i] );
            }
        }
    }

    /* Close sessions */
    if( session > 0 )
    {
        ( void )C_CloseSession( session );
    }

    if( rv != CKR_OK )
    {
        return SECURE_ELEMENT_ERROR;
    }
    return SECURE_ELEMENT_SUCCESS;
#endif /* LORAWAN_KMS */
}

SecureElementStatus_t SecureElementSetObjHandler( KeyIdentifier_t keyID, uint32_t keyIndex )
{
#if (LORAWAN_KMS == 0)
    return SECURE_ELEMENT_ERROR;
#else /* LORAWAN_KMS == 1 */
    for( uint8_t i = 0; i < NUM_OF_KEYS; i++ )
    {
        if( KeyList[i].KeyID == keyID )
        {
            KeyList[i].Object_Index = ( CK_OBJECT_HANDLE ) keyIndex;
            return SECURE_ELEMENT_SUCCESS;
        }
    }
    return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
#endif /* LORAWAN_KMS */
}

SecureElementStatus_t SecureElementSetKey( KeyIdentifier_t keyID, uint8_t *key )
{
    if( key == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

#if (LORAWAN_KMS == 0)
    for( uint8_t i = 0; i < NUM_OF_KEYS; i++ )
    {
        if( SeNvm->KeyList[i].KeyID == keyID )
        {
#if ( LORAMAC_MAX_MC_CTX == 1 )
            if( keyID == MC_KEY_0 )
#else /* LORAMAC_MAX_MC_CTX > 1 */
            if( ( keyID == MC_KEY_0 ) || ( keyID == MC_KEY_1 ) || ( keyID == MC_KEY_2 ) || ( keyID == MC_KEY_3 ) )
#endif /* LORAMAC_MAX_MC_CTX */
            {
                /* Decrypt the key if its a Mckey */
                SecureElementStatus_t retval = SECURE_ELEMENT_ERROR;
                uint8_t decryptedKey[SE_KEY_SIZE] = { 0 };

                retval = SecureElementAesEncrypt( key, SE_KEY_SIZE, MC_KE_KEY, decryptedKey );

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
#if (KEY_EXTRACTABLE == 1)
    CK_ULONG template_true = CK_TRUE;
#else
    CK_ULONG template_false = CK_FALSE;
#endif /* KEY_EXTRACTABLE */
    uint32_t key_ui32[] =
    {
        GET_UINT32_BE( key, 0 ),
        GET_UINT32_BE( key, 4 ),
        GET_UINT32_BE( key, 8 ),
        GET_UINT32_BE( key, 12 ),
    };

    uint32_t specific_label[] = {GlobalTemplateLabel, 0UL};
    CK_ATTRIBUTE key_attribute_template[] =
    {
        { CKA_CLASS, ( CK_VOID_PTR ) &template_class, sizeof( CK_BBOOL ) },
        { CKA_KEY_TYPE, ( CK_VOID_PTR ) &template_type, sizeof( CK_BBOOL ) },
        { CKA_VALUE, ( CK_VOID_PTR ) key_ui32, sizeof( key_ui32 )},
#if (KEY_EXTRACTABLE == 1)
        { CKA_EXTRACTABLE, ( CK_VOID_PTR ) &template_true, sizeof( CK_BBOOL ) },
#else
        { CKA_EXTRACTABLE, ( CK_VOID_PTR ) &template_false, sizeof( CK_BBOOL ) },
#endif /* KEY_EXTRACTABLE */
        { CKA_LABEL, ( CK_VOID_PTR ) specific_label, sizeof( specific_label ) },
    };

#if ( LORAMAC_MAX_MC_CTX == 1 )
    if( keyID == MC_KEY_0 )
#else /* LORAMAC_MAX_MC_CTX > 1 */
    if( ( keyID == MC_KEY_0 ) || ( keyID == MC_KEY_1 ) || ( keyID == MC_KEY_2 ) || ( keyID == MC_KEY_3 ) )
#endif /* LORAMAC_MAX_MC_CTX */
    {
        /* Decrypt the key if its a Mckey */
        uint8_t decryptedKey[SE_KEY_SIZE] = { 0 };
        if( SECURE_ELEMENT_SUCCESS != SecureElementAesEncrypt( key, SE_KEY_SIZE, MC_KE_KEY, decryptedKey ) )
        {
            return SECURE_ELEMENT_ERROR;
        }

        key_ui32[0] = GET_UINT32_BE( decryptedKey, 0 );
        key_ui32[1] = GET_UINT32_BE( decryptedKey, 4 );
        key_ui32[2] = GET_UINT32_BE( decryptedKey, 8 );
        key_ui32[3] = GET_UINT32_BE( decryptedKey, 12 );
    }

    if( SECURE_ELEMENT_SUCCESS != GetKeyIndexByID( keyID, &key_handle ) )
    {
        return SECURE_ELEMENT_ERROR;
    }

    if( SECURE_ELEMENT_SUCCESS != SecureElementDeleteDynamicKey( keyID, &specific_label[1] ) )
    {
        return SECURE_ELEMENT_ERROR;
    }

    /* Open session with KMS */
    rv = C_OpenSession( 0, session_flags, NULL, 0, &session );

    /* Get key to display */
    if( rv == CKR_OK )
    {
        rv = C_CreateObject( session, key_attribute_template, GET_NB_ATTRIBUTE( key_attribute_template ),
                             &key_handle );
    }

    if( rv == CKR_OK )
    {
        retval = SecureElementSetObjHandler( keyID, key_handle );
    }

    /* Close sessions */
    ( void )C_CloseSession( session );

    if( rv != CKR_OK )
    {
        retval = SECURE_ELEMENT_ERROR;
    }
    return retval;
#endif /* LORAWAN_KMS */
}

SecureElementStatus_t SecureElementSetID( SecureElementNvmDevJoinAddrKey_t *KMSKeyBlob )
{
#if (LORAWAN_KMS == 0)
    return SECURE_ELEMENT_SUCCESS;
#else /* LORAWAN_KMS == 1 */
    SecureElementStatus_t retval = SECURE_ELEMENT_ERROR;
    CK_RV rv;
    CK_SESSION_HANDLE session;
    KeyIdentifier_t keyID = DEV_JOIN_EUI_ADDR_KEY;
    CK_FLAGS session_flags = CKF_SERIAL_SESSION;  /* Read ONLY session */
    CK_OBJECT_HANDLE key_handle;
    CK_ULONG template_class = CKO_SECRET_KEY;
    CK_ULONG template_type = CKK_AES;
    CK_ULONG template_true = CK_TRUE;
    uint8_t *buf = ( uint8_t * )KMSKeyBlob;
    uint32_t buf_ui32[] =
    {
        GET_UINT32_BE( buf, 0 ),
        GET_UINT32_BE( buf, 4 ),
        GET_UINT32_BE( buf, 8 ),
        GET_UINT32_BE( buf, 12 ),
        GET_UINT32_BE( buf, 16 ),
        GET_UINT32_BE( buf, 20 ),
    };

    uint32_t specific_label[] = {GlobalTemplateLabel, 0UL};
    CK_ATTRIBUTE attribute_template[] =
    {
        { CKA_CLASS, ( CK_VOID_PTR ) &template_class,  sizeof( CK_BBOOL )      },
        { CKA_KEY_TYPE, ( CK_VOID_PTR ) &template_type,   sizeof( CK_BBOOL )      },
        { CKA_VALUE, ( CK_VOID_PTR ) buf_ui32,         sizeof( buf_ui32 )      },
        { CKA_EXTRACTABLE, ( CK_VOID_PTR ) &template_true,   sizeof( CK_BBOOL )      },
        { CKA_LABEL, ( CK_VOID_PTR ) specific_label,   sizeof( specific_label )},
    };

    if( KMSKeyBlob == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    if( SECURE_ELEMENT_SUCCESS != GetKeyIndexByID( keyID, &key_handle ) )
    {
        return SECURE_ELEMENT_ERROR;
    }

    if( SECURE_ELEMENT_SUCCESS != SecureElementDeleteDynamicKey( keyID, &specific_label[1] ) )
    {
        return SECURE_ELEMENT_ERROR;
    }

    /* Open session with KMS */
    rv = C_OpenSession( 0, session_flags, NULL, 0, &session );

    /* Get key to display */
    if( rv == CKR_OK )
    {
        rv = C_CreateObject( session, attribute_template, GET_NB_ATTRIBUTE( attribute_template ),
                             &key_handle );
    }

    if( rv == CKR_OK )
    {
        retval = SecureElementSetObjHandler( keyID, key_handle );
    }

    /* Close sessions */
    ( void )C_CloseSession( session );

    if( rv != CKR_OK )
    {
        retval = SECURE_ELEMENT_ERROR;
    }
    return retval;
#endif /* LORAWAN_KMS */
}

SecureElementStatus_t SecureElementComputeAesCmac( uint8_t *micBxBuffer, uint8_t *buffer, uint32_t size,
                                                   KeyIdentifier_t keyID, uint32_t *cmac )
{
    if( keyID >= MC_KE_KEY )
    {
        /* Never accept multicast key identifier for cmac computation */
        return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
    }

    return ComputeCmac( micBxBuffer, buffer, size, keyID, cmac );
}

SecureElementStatus_t SecureElementVerifyAesCmac( uint8_t *buffer, uint32_t size, uint32_t expectedCmac,
                                                  KeyIdentifier_t keyID )
{
    if( buffer == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    SecureElementStatus_t retval   = SECURE_ELEMENT_ERROR;
#if (LORAWAN_KMS == 0)
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

    if( buffer == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    /* AES CMAC Authentication variables */
    CK_MECHANISM aes_cmac_mechanism = { CKM_AES_CMAC, ( CK_VOID_PTR )NULL, 0 };

    retval = GetKeyIndexByID( keyID, &object_handle );
    if( retval != SECURE_ELEMENT_SUCCESS )
    {
        return retval;
    }

    /* Open session with KMS */
    rv = C_OpenSession( 0,    session_flags, NULL, 0, &session );

    /* Configure session to Verify the message in AES CMAC with settings included into the mechanism */
    if( rv == CKR_OK )
    {
        rv = C_VerifyInit( session, &aes_cmac_mechanism, object_handle );
    }

    /* Verify the message */
    if( rv == CKR_OK )
    {
        memcpy1( input_align_combined_buf, buffer, size );
        rv = C_Verify( session, ( CK_BYTE_PTR )input_align_combined_buf, size, ( CK_BYTE_PTR )&expectedCmac, 4 );
    }

    ( void )C_CloseSession( session );

    if( rv != CKR_OK )
    {
        retval = SECURE_ELEMENT_ERROR;
    }

#endif /* LORAWAN_KMS */

    return retval;
}

SecureElementStatus_t SecureElementAesEncrypt( uint8_t *buffer, uint32_t size, KeyIdentifier_t keyID,
                                               uint8_t *encBuffer )
{
    if( ( buffer == NULL ) || ( encBuffer == NULL ) )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    /* Check if the size is divisible by 16 */
    if( ( size % 16 ) != 0 )
    {
        return SECURE_ELEMENT_ERROR_BUF_SIZE;
    }

#if (LORAWAN_KMS == 0)
    lorawan_aes_context aesContext;
    memset1( aesContext.ksch, '\0', 240 );

    Key_t                *pItem;
    SecureElementStatus_t retval = GetKeyByID( keyID, &pItem );

    if( retval == SECURE_ELEMENT_SUCCESS )
    {
        lorawan_aes_set_key( pItem->KeyValue, SE_KEY_SIZE, &aesContext );

        uint8_t block = 0;

        while( size != 0 )
        {
            lorawan_aes_encrypt( &buffer[block], &encBuffer[block], &aesContext );
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
    uint32_t dummy_tag_length = 0;

    CK_MECHANISM aes_ecb_mechanism = { CKM_AES_ECB, ( CK_VOID_PTR * ) NULL, 0 };

    SecureElementStatus_t retval = GetKeyIndexByID( keyID, &object_handle );
    if( retval != SECURE_ELEMENT_SUCCESS )
    {
        return retval;
    }

    /* Open session with KMS */
    rv = C_OpenSession( 0,    session_flags, NULL, 0, &session );

    /* Configure session to encrypt message in AES ECB with settings included into the mechanism */
    if( rv == CKR_OK )
    {
        rv = C_EncryptInit( session, &aes_ecb_mechanism, object_handle );
    }

    /* Encrypt clear message */
    if( rv == CKR_OK )
    {
        memcpy1( input_align_combined_buf, buffer, size );
        encrypted_length = sizeof( output_align );
        rv = C_EncryptUpdate( session, ( CK_BYTE_PTR )input_align_combined_buf, size,
                              output_align, ( CK_ULONG_PTR )&encrypted_length );
        memcpy1( encBuffer, output_align, size );
    }

    /* In this case C_EncryptFinal is just called to Free the Alloc mem */
    if( rv == CKR_OK )
    {
        dummy_tag_length = sizeof( tag );
        rv = C_EncryptFinal( session, &dummy_tag[0], ( CK_ULONG_PTR )&dummy_tag_length );
    }

    /* Close session with KMS */
    ( void )C_CloseSession( session );

    if( rv != CKR_OK )
    {
        retval = SECURE_ELEMENT_ERROR;
    }
#endif /* LORAWAN_KMS */

    return retval;
}

SecureElementStatus_t SecureElementDeriveAndStoreKey( uint8_t *input, KeyIdentifier_t rootKeyID,
                                                      KeyIdentifier_t targetKeyID )
{
    if( input == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    SecureElementStatus_t retval  = SECURE_ELEMENT_ERROR;

    /* In case of MC_KE_KEY, only McRootKey can be used as root key */
    if( targetKeyID == MC_KE_KEY )
    {
        if( rootKeyID != MC_ROOT_KEY )
        {
            return SECURE_ELEMENT_ERROR_INVALID_KEY_ID;
        }
    }

#if (LORAWAN_KMS == 0)
    uint8_t key[SE_KEY_SIZE] = { 0 };
    /* Derive key */
    retval = SecureElementAesEncrypt( input, SE_KEY_SIZE, rootKeyID, key );
    if( retval != SECURE_ELEMENT_SUCCESS )
    {
        return retval;
    }

    /* Store key */
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
    CK_ATTRIBUTE DeriveKey_template = {CKA_LABEL, ( CK_VOID_PTR )specific_label, sizeof( specific_label )};

    /* Derive key */
    if( SECURE_ELEMENT_SUCCESS != GetKeyIndexByID( rootKeyID, &rootkey_object_handle ) )
    {
        return SECURE_ELEMENT_ERROR;
    }

    if( SECURE_ELEMENT_SUCCESS != GetKeyIndexByID( targetKeyID, &derived_object_handle ) )
    {
        return SECURE_ELEMENT_ERROR;
    }

    if( SECURE_ELEMENT_SUCCESS != SecureElementDeleteDynamicKey( targetKeyID, &specific_label[1] ) )
    {
        return SECURE_ELEMENT_ERROR;
    }

    /* Open session with KMS */
    rv = C_OpenSession( 0,    session_flags, NULL, 0, &session );

    /* Derive key with pass phrase */
    if( rv == CKR_OK )
    {
        rv = C_DeriveKey( session, &( mech ), rootkey_object_handle,
                          &DeriveKey_template, GET_NB_ATTRIBUTE( DeriveKey_template ), &derived_object_handle );
    }

    if( rv == CKR_OK )
    {
        /* Store Derived Index in table */
        retval = SecureElementSetObjHandler( targetKeyID, derived_object_handle );
    }

    /* Close session with KMS */
    ( void )C_CloseSession( session );

    if( rv != CKR_OK )
    {
        retval = SECURE_ELEMENT_ERROR;
    }
    return retval;
#endif /* LORAWAN_KMS */
}

SecureElementStatus_t SecureElementProcessJoinAccept( JoinReqIdentifier_t joinReqType, uint8_t *joinEui,
                                                      uint16_t devNonce, uint8_t *encJoinAccept,
                                                      uint8_t encJoinAcceptSize, uint8_t *decJoinAccept,
                                                      uint8_t *versionMinor )
{
    if( ( encJoinAccept == NULL ) || ( decJoinAccept == NULL ) || ( versionMinor == NULL ) )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

    /* Check that frame size isn't bigger than a JoinAccept with CFList size */
    if( encJoinAcceptSize > LORAMAC_JOIN_ACCEPT_FRAME_MAX_SIZE )
    {
        return SECURE_ELEMENT_ERROR_BUF_SIZE;
    }

    /* Determine decryption key */
    KeyIdentifier_t encKeyID = NWK_KEY;

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    if( joinReqType != JOIN_REQ )
    {
        encKeyID = J_S_ENC_KEY;
    }
#endif /* LORAMAC_VERSION */

    memcpy1( decJoinAccept, encJoinAccept, encJoinAcceptSize );

    /* Decrypt JoinAccept, skip MHDR */
    if( SecureElementAesEncrypt( encJoinAccept + LORAMAC_MHDR_FIELD_SIZE, encJoinAcceptSize - LORAMAC_MHDR_FIELD_SIZE,
                                 encKeyID, decJoinAccept + LORAMAC_MHDR_FIELD_SIZE ) != SECURE_ELEMENT_SUCCESS )
    {
        return SECURE_ELEMENT_FAIL_ENCRYPT;
    }

    *versionMinor = ( ( decJoinAccept[11] & 0x80 ) == 0x80 ) ? 1 : 0;

    uint32_t mic = GET_UINT32_LE( decJoinAccept, encJoinAcceptSize - LORAMAC_MIC_FIELD_SIZE );

    /*  - Header buffer to be used for MIC computation
     *        - LoRaWAN 1.0.x : micHeader = [MHDR(1)]
     *        - LoRaWAN 1.1.x : micHeader = [JoinReqType(1), JoinEUI(8), DevNonce(2), MHDR(1)]
     */

    /* Verify mic */
    if( *versionMinor == 0 )
    {
        /* For LoRaWAN 1.0.x
         *   cmac = aes128_cmac(NwkKey, MHDR |  JoinNonce | NetID | DevAddr | DLSettings | RxDelay | CFList |
         *   CFListType)
         */
        if( SecureElementVerifyAesCmac( decJoinAccept, ( encJoinAcceptSize - LORAMAC_MIC_FIELD_SIZE ), mic, NWK_KEY ) !=
            SECURE_ELEMENT_SUCCESS )
        {
            return SECURE_ELEMENT_FAIL_CMAC;
        }
    }
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    else if( *versionMinor == 1 )
    {
        uint8_t  micHeader11[JOIN_ACCEPT_MIC_COMPUTATION_OFFSET] = { 0 };
        uint16_t bufItr                                          = 0;

        micHeader11[bufItr++] = ( uint8_t ) joinReqType;

        memcpyr( micHeader11 + bufItr, joinEui, LORAMAC_JOIN_EUI_FIELD_SIZE );
        bufItr += LORAMAC_JOIN_EUI_FIELD_SIZE;

        micHeader11[bufItr++] = devNonce & 0xFF;
        micHeader11[bufItr++] = ( devNonce >> 8 ) & 0xFF;

        /* For LoRaWAN 1.1.x and later:
         *   cmac = aes128_cmac(JSIntKey, JoinReqType | JoinEUI | DevNonce | MHDR | JoinNonce | NetID | DevAddr |
         *   DLSettings | RxDelay | CFList | CFListType)
         *   Prepare the msg for integrity check (adding JoinReqType, JoinEUI and DevNonce)
         */
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
#endif /* LORAMAC_VERSION */
    else
    {
        return SECURE_ELEMENT_ERROR_INVALID_LORAWAM_SPEC_VERSION;
    }

    return SECURE_ELEMENT_SUCCESS;
}

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
SecureElementStatus_t SecureElementRandomNumber( uint32_t *randomNum )
{
    if( randomNum == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }
    *randomNum = Radio.Random();
    return SECURE_ELEMENT_SUCCESS;
}
#endif /* LORAMAC_VERSION */

SecureElementStatus_t SecureElementSetDevEui( uint8_t *devEui )
{
    if( devEui == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

#if (LORAWAN_KMS == 0)
    memcpy1( SeNvm->SeNvmDevJoinKey.DevEui, devEui, SE_EUI_SIZE );
    return SECURE_ELEMENT_SUCCESS;
#else
    SecureElementStatus_t status;
    SecureElementNvmDevJoinAddrKey_t KMSKeyBlob ALIGN( 8 );

    /* recover whole KMSKeyBlob handle */
    status = SecureElementGetKeyByID( DEV_JOIN_EUI_ADDR_KEY, ( uint8_t * )&KMSKeyBlob );
    if( ( status == SECURE_ELEMENT_SUCCESS ) && ( memcmp( KMSKeyBlob.DevEui, devEui, SE_EUI_SIZE ) != 0 ) )
    {
        memcpy1( KMSKeyBlob.DevEui, devEui, SE_EUI_SIZE );
        status = SecureElementSetID( &KMSKeyBlob );
    }
    return status;
#endif /* LORAWAN_KMS */
}

SecureElementStatus_t SecureElementGetDevEui( uint8_t *devEui )
{
    if( devEui == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

#if (LORAWAN_KMS == 0)
    memcpy1( devEui, SeNvm->SeNvmDevJoinKey.DevEui, SE_EUI_SIZE );
    return SECURE_ELEMENT_SUCCESS;
#else
    SecureElementStatus_t status;
    SecureElementNvmDevJoinAddrKey_t KMSKeyBlob ALIGN( 8 );

    status = SecureElementGetKeyByID( DEV_JOIN_EUI_ADDR_KEY, ( uint8_t * )&KMSKeyBlob );
    if( status == SECURE_ELEMENT_SUCCESS )
    {
        /* get DevEui field in KMSKeyBlob handle */
        memcpy1( devEui, KMSKeyBlob.DevEui, SE_EUI_SIZE );
    }
    return status;
#endif /* LORAWAN_KMS */
}

SecureElementStatus_t SecureElementSetJoinEui( uint8_t *joinEui )
{
    if( joinEui == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

#if (LORAWAN_KMS == 0)
    memcpy1( SeNvm->SeNvmDevJoinKey.JoinEui, joinEui, SE_EUI_SIZE );
    return SECURE_ELEMENT_SUCCESS;
#else
    SecureElementStatus_t status;
    SecureElementNvmDevJoinAddrKey_t KMSKeyBlob ALIGN( 8 );

    /* recover whole KMSKeyBlob handle */
    status = SecureElementGetKeyByID( DEV_JOIN_EUI_ADDR_KEY, ( uint8_t * )&KMSKeyBlob );
    if( ( status == SECURE_ELEMENT_SUCCESS ) && ( memcmp( KMSKeyBlob.JoinEui, joinEui, SE_EUI_SIZE ) != 0 ) )
    {
        memcpy1( KMSKeyBlob.JoinEui, joinEui, SE_EUI_SIZE );
        status = SecureElementSetID( &KMSKeyBlob );
    }
    return status;
#endif /* LORAWAN_KMS */
}

SecureElementStatus_t SecureElementGetJoinEui( uint8_t *joinEui )
{
    if( joinEui == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }

#if (LORAWAN_KMS == 0)
    memcpy1( joinEui, SeNvm->SeNvmDevJoinKey.JoinEui, SE_EUI_SIZE );
    return SECURE_ELEMENT_SUCCESS;
#else
    SecureElementStatus_t status;
    SecureElementNvmDevJoinAddrKey_t KMSKeyBlob ALIGN( 8 );

    status = SecureElementGetKeyByID( DEV_JOIN_EUI_ADDR_KEY, ( uint8_t * )&KMSKeyBlob );
    if( status == SECURE_ELEMENT_SUCCESS )
    {
        /* get JoinEui field from KMSKeyBlob handle */
        memcpy1( joinEui, KMSKeyBlob.JoinEui, SE_EUI_SIZE );
    }
    return status;
#endif /* LORAWAN_KMS */
}

SecureElementStatus_t SecureElementSetDevAddr( ActivationType_t mode, uint32_t devAddr )
{
#if (LORAWAN_KMS == 0)
    if( mode == ACTIVATION_TYPE_OTAA )
    {
        SeNvm->SeNvmDevJoinKey.DevAddrOTAA = devAddr;
    }
    else
    {
        SeNvm->SeNvmDevJoinKey.DevAddrABP = devAddr;
    }

    return SECURE_ELEMENT_SUCCESS;
#else
    SecureElementStatus_t status;
    SecureElementNvmDevJoinAddrKey_t KMSKeyBlob ALIGN( 8 );

    /* recover whole KMSKeyBlob handle */
    status = SecureElementGetKeyByID( DEV_JOIN_EUI_ADDR_KEY, ( uint8_t * )&KMSKeyBlob );
    if( status == SECURE_ELEMENT_SUCCESS )
    {
        /* Recover DevAddrABP or DevAddrOTAA depending on mode */
        if( mode == ACTIVATION_TYPE_OTAA )
        {
            if( KMSKeyBlob.DevAddrOTAA != devAddr )
            {
                KMSKeyBlob.DevAddrOTAA = devAddr;
                status = SecureElementSetID( &KMSKeyBlob );
            }
        }
        else
        {
            /* ABP or Not yet joined: set devAddrABP */
            if( KMSKeyBlob.DevAddrABP != devAddr )
            {
                KMSKeyBlob.DevAddrABP = devAddr;
                status = SecureElementSetID( &KMSKeyBlob );
            }
        }
    }

    return status;
#endif /* LORAWAN_KMS */
}

SecureElementStatus_t SecureElementGetDevAddr( ActivationType_t mode, uint32_t *devAddr )
{
    if( devAddr == NULL )
    {
        return SECURE_ELEMENT_ERROR_NPE;
    }
#if (LORAWAN_KMS == 0)
    /* Recover DevAddrABP or DevAddrOTAA depending on mode */
    if( mode == ACTIVATION_TYPE_OTAA )
    {
        *devAddr = SeNvm->SeNvmDevJoinKey.DevAddrOTAA;
    }
    else
    {
        *devAddr = SeNvm->SeNvmDevJoinKey.DevAddrABP;
    }
    return SECURE_ELEMENT_SUCCESS;
#else
    SecureElementStatus_t status;
    SecureElementNvmDevJoinAddrKey_t KMSKeyBlob ALIGN( 8 );

    /* recover whole KMSKeyBlob handle */
    status = SecureElementGetKeyByID( DEV_JOIN_EUI_ADDR_KEY, ( uint8_t * )&KMSKeyBlob );
    if( status == SECURE_ELEMENT_SUCCESS )
    {
        /* Recover DevAddrABP or DevAddrOTAA depending on mode */
        if( mode == ACTIVATION_TYPE_OTAA )
        {
            *devAddr = KMSKeyBlob.DevAddrOTAA;
        }
        else
        {
            *devAddr = KMSKeyBlob.DevAddrABP;
        }
    }

    return status;
#endif /* LORAWAN_KMS */
}
