/*!
 * \file      LmhpFragmentation.c
 *
 * \brief     Implements the LoRa-Alliance fragmented data block transport package
 *            Specification V1.0.0: https://resources.lora-alliance.org/technical-specifications/lorawan-fragmented-data-block-transport-specification-v1-0-0
 *            Specification V2.0.0: https://resources.lora-alliance.org/technical-specifications/ts004-2-0-0-fragmented-data-block-transport
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
 *              (C)2013-2018 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    LmhpFragmentation.c
  * @author  MCD Application Team
  * @brief   Implements the LoRa-Alliance fragmented data block transport package
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "LoRaMac.h"
#include "LmHandler.h"
#include "LmhpFragmentation.h"
#include "frag_decoder_if.h"
#include "utilities.h"
#include "mw_log_conf.h"  /* needed for MW_LOG */

/*!
 * LoRaWAN Application Layer Fragmented Data Block Transport Specification
 */
#define FRAGMENTATION_PORT                          201

#define FRAGMENTATION_ID                            3

#if (LORAWAN_PACKAGES_VERSION == 1)
#define FRAGMENTATION_VERSION                       1
#elif (LORAWAN_PACKAGES_VERSION == 2)
#define FRAGMENTATION_VERSION                       2
#endif /* LORAWAN_PACKAGES_VERSION */

#define FRAGMENTATION_MAX_SESSIONS                  4

/*!
 * Package current context
 */
typedef struct LmhpFragmentationState_s
{
    bool Initialized;
    bool IsTxPending;
#if ( FRAGMENTATION_VERSION == 2 )
    bool FragDataBlockAnsRequired;
#endif /* FRAGMENTATION_VERSION */
    uint8_t DataBufferMaxSize;
    uint8_t DataBufferSize;
    uint8_t *DataBuffer;
} LmhpFragmentationState_t;

typedef enum LmhpFragmentationMoteCmd_e
{
    FRAGMENTATION_PKG_VERSION_ANS               = 0x00,
    FRAGMENTATION_FRAG_STATUS_ANS               = 0x01,
    FRAGMENTATION_FRAG_SESSION_SETUP_ANS        = 0x02,
    FRAGMENTATION_FRAG_SESSION_DELETE_ANS       = 0x03,
#if ( FRAGMENTATION_VERSION == 2 )
    FRAGMENTATION_FRAG_DATA_BLOCK_RECEIVED_REQ  = 0x04,
#endif /* FRAGMENTATION_VERSION */
} LmhpFragmentationMoteCmd_t;

typedef enum LmhpFragmentationSrvCmd_e
{
    FRAGMENTATION_PKG_VERSION_REQ               = 0x00,
    FRAGMENTATION_FRAG_STATUS_REQ               = 0x01,
    FRAGMENTATION_FRAG_SESSION_SETUP_REQ        = 0x02,
    FRAGMENTATION_FRAG_SESSION_DELETE_REQ       = 0x03,
#if ( FRAGMENTATION_VERSION == 2 )
    FRAGMENTATION_FRAG_DATA_BLOCK_RECEIVED_ANS  = 0x04,
#endif /* FRAGMENTATION_VERSION */
    FRAGMENTATION_DATA_FRAGMENT                 = 0x08,
} LmhpFragmentationSrvCmd_t;

/*!
 * LoRaWAN fragmented data block transport handler parameters
 */
static LmhpFragmentationParams_t *LmhpFragmentationParams;

/*!
 * Initializes the package with provided parameters
 *
 * \param [in] params            Pointer to the package parameters
 * \param [in] dataBuffer        Pointer to main application buffer
 * \param [in] dataBufferMaxSize Main application buffer maximum size
 */
static void LmhpFragmentationInit( void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize );

/*!
 * Returns the current package initialization status.
 *
 * \retval status Package initialization status
 *                [true: Initialized, false: Not initialized]
 */
static bool LmhpFragmentationIsInitialized( void );

/*!
 * Returns if a package transmission is pending or not.
 *
 * \retval status Package transmission status
 *                [true: pending, false: Not pending]
 */
static bool LmhpFragmentationIsTxPending( void );

/*!
 * Processes the internal package events.
 */
static void LmhpFragmentationProcess( void );

/*!
 * Processes the MCPS Indication
 *
 * \param [in] mcpsIndication     MCPS indication primitive data
 */
static void LmhpFragmentationOnMcpsIndication( McpsIndication_t *mcpsIndication );

/*!
 * \brief Callback function for Fragment delay timer.
 */
static void OnFragmentProcessTimer( void *context );

static LmhpFragmentationState_t LmhpFragmentationState =
{
    .Initialized = false,
    .IsTxPending = false,
#if ( FRAGMENTATION_VERSION == 2 )
    .FragDataBlockAnsRequired = false,
#endif /* FRAGMENTATION_VERSION */
    .DataBufferMaxSize = 0,
    .DataBufferSize = 0,
    .DataBuffer = NULL,
};

typedef struct FragGroupData_s
{
    bool IsActive;
    union
    {
        uint8_t Value;
        struct
        {
            uint8_t McGroupBitMask:   4;
            uint8_t FragIndex:        2;
            uint8_t RFU:              2;
        } Fields;
    } FragSession;
    uint16_t FragNb;
    uint8_t FragSize;
    union
    {
        uint8_t Value;
        struct
        {
            uint8_t BlockAckDelay:    3;
            uint8_t FragAlgo:         3;
#if ( FRAGMENTATION_VERSION == 1 )
            uint8_t RFU:              2;
#elif ( FRAGMENTATION_VERSION == 2 )
            uint8_t AckReception:     1;
            uint8_t RFU:              1;
#endif /* FRAGMENTATION_VERSION */
        } Fields;
    } Control;
    uint8_t Padding;
    uint32_t Descriptor;
#if ( FRAGMENTATION_VERSION == 2 )
    uint16_t SessionCnt;
    uint32_t Mic;
#endif /* FRAGMENTATION_VERSION */
} FragGroupData_t;

typedef struct FragSessionData_s
{
    FragGroupData_t FragGroupData;
    FragDecoderStatus_t FragDecoderStatus;
    int32_t FragDecoderProcessStatus;
} FragSessionData_t;

static FragSessionData_t FragSessionData[FRAGMENTATION_MAX_SESSIONS];

static LmhPackage_t LmhpFragmentationPackage =
{
    .Port = FRAGMENTATION_PORT,
    .Init = LmhpFragmentationInit,
    .IsInitialized = LmhpFragmentationIsInitialized,
    .IsTxPending = LmhpFragmentationIsTxPending,
    .Process = LmhpFragmentationProcess,
    .OnPackageProcessEvent = NULL,                             /* To be initialized by LmHandler */
    .OnMcpsConfirmProcess = NULL,                              /* Not used in this package */
    .OnMcpsIndicationProcess = LmhpFragmentationOnMcpsIndication,
    .OnMlmeConfirmProcess = NULL,                              /* Not used in this package */
    .OnMlmeIndicationProcess = NULL,                           /* Not used in this package */
    .OnJoinRequest = NULL,                                     /* To be initialized by LmHandler */
    .OnDeviceTimeRequest = NULL,                               /* To be initialized by LmHandler */
    .OnSysTimeUpdate = NULL,                                   /* To be initialized by LmHandler */
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    .OnSystemReset = NULL,                                     /* To be initialized by LmHandler */
#endif /* LORAMAC_VERSION */
};

/* Delay value. */
static uint32_t TxDelayTime;

/* Fragment Delay Timer struct */
static TimerEvent_t FragmentProcessTimer;

/* Co-efficient used to calculate delay. */
static uint8_t BlockAckDelay = 0;

#if ( FRAGMENTATION_VERSION == 2 )
/* fragmentation counter session */
static int32_t SessionCntPrev[FRAGMENTATION_MAX_SESSIONS] = {-1, -1, -1, -1};
#endif /* FRAGMENTATION_VERSION */

/*!
 * \brief Callback function for Fragment delay timer.
 */
static void OnFragmentProcessTimer( void *context )
{
    if( LmhpFragmentationState.DataBufferSize != 0 )
    {
        LmhpFragmentationState.IsTxPending = true;
    }
    if( LmhpFragmentationPackage.OnPackageProcessEvent != NULL )
    {
        LmhpFragmentationPackage.OnPackageProcessEvent();
    }
}

LmhPackage_t *LmhpFragmentationPackageFactory( void )
{
    return &LmhpFragmentationPackage;
}

static void LmhpFragmentationInit( void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize )
{
    if( ( params != NULL ) && ( dataBuffer != NULL ) )
    {
        LmhpFragmentationParams = ( LmhpFragmentationParams_t * )params;
        LmhpFragmentationState.DataBuffer = dataBuffer;
        LmhpFragmentationState.DataBufferMaxSize = dataBufferMaxSize;
        LmhpFragmentationState.Initialized = true;
        /* Initialize Fragmentation delay time. */
        TxDelayTime = 0;
        /* Initialize Fragmentation delay timer. */
        TimerInit( &FragmentProcessTimer, OnFragmentProcessTimer );
    }
    else
    {
        LmhpFragmentationParams = NULL;
        LmhpFragmentationState.Initialized = false;
    }
    LmhpFragmentationState.IsTxPending = false;

    /* initialize the global fragmentation session buffer */
    memset1( ( uint8_t * )FragSessionData, 0, sizeof( FragSessionData ) );
}

static bool LmhpFragmentationIsInitialized( void )
{
    return LmhpFragmentationState.Initialized;
}

static bool LmhpFragmentationIsTxPending( void )
{
    return LmhpFragmentationState.IsTxPending;
}

static void LmhpFragmentationProcess( void )
{
    if( LmhpFragmentationState.IsTxPending == true )
    {
        /* Send the reply. */
        LmHandlerAppData_t appData =
        {
            .Buffer     = LmhpFragmentationState.DataBuffer,
            .BufferSize = LmhpFragmentationState.DataBufferSize,
            .Port       = FRAGMENTATION_PORT,
        };

        LmHandlerErrorStatus_t lmhStatus = LORAMAC_HANDLER_ERROR;
        lmhStatus = LmHandlerSend( &appData, LORAMAC_HANDLER_UNCONFIRMED_MSG, true );
#if ( FRAGMENTATION_VERSION == 2 )
        if( ( lmhStatus != LORAMAC_HANDLER_SUCCESS ) || ( LmhpFragmentationState.FragDataBlockAnsRequired == true ) )
#else
        if( lmhStatus != LORAMAC_HANDLER_SUCCESS )
#endif /* FRAGMENTATION_VERSION */
        {
            /* try to send the message again */
            TimerSetValue( &FragmentProcessTimer, 1500 );
            TimerStart( &FragmentProcessTimer );
        }
        else
        {
            LmhpFragmentationState.IsTxPending = false;
            LmhpFragmentationState.DataBufferSize = 0;
        }
    }
}


static uint8_t buffer[FRAG_MAX_SIZE];

static void LmhpFragmentationOnMcpsIndication( McpsIndication_t *mcpsIndication )
{
    uint8_t cmdIndex = 0;
    uint8_t dataBufferIndex = 0;
    bool isAnswerDelayed = false;

    if( mcpsIndication->Port != FRAGMENTATION_PORT )
    {
        return;
    }
 
    LmhpFragmentationState.DataBufferSize = 0;

    while( cmdIndex < mcpsIndication->BufferSize )
    {
        switch( mcpsIndication->Buffer[cmdIndex++] )
        {
            case FRAGMENTATION_PKG_VERSION_REQ:
                {
                    if( mcpsIndication->Multicast == 1 )
                    {
                        /* Multicast channel. Don't process command. */
                        break;
                    }
                    LmhpFragmentationState.DataBuffer[dataBufferIndex++] = FRAGMENTATION_PKG_VERSION_ANS;
                    LmhpFragmentationState.DataBuffer[dataBufferIndex++] = FRAGMENTATION_ID;
                    LmhpFragmentationState.DataBuffer[dataBufferIndex++] = FRAGMENTATION_VERSION;
                    break;
                }
            case FRAGMENTATION_FRAG_STATUS_REQ:
                {
                    uint8_t fragIndex = mcpsIndication->Buffer[cmdIndex++];
                    uint8_t participants = fragIndex & 0x01;

                    fragIndex = ( fragIndex >> 1 ) & 0x03;
                    FragSessionData[fragIndex].FragDecoderStatus = FragDecoderGetStatus( );

                    if( ( participants == 1 ) ||
                        ( ( participants == 0 ) && ( FragSessionData[fragIndex].FragDecoderStatus.FragNbLost > 0 ) ) )
                    {
                        LmhpFragmentationState.DataBuffer[dataBufferIndex++] = FRAGMENTATION_FRAG_STATUS_ANS;
                        LmhpFragmentationState.DataBuffer[dataBufferIndex++] = FragSessionData[fragIndex].FragDecoderStatus.FragNbRx & 0xFF;
                        LmhpFragmentationState.DataBuffer[dataBufferIndex++] = ( fragIndex << 6 ) |
                                                                               ( ( FragSessionData[fragIndex].FragDecoderStatus.FragNbRx >> 8 ) & 0x3F );
                        LmhpFragmentationState.DataBuffer[dataBufferIndex++] = FragSessionData[fragIndex].FragDecoderStatus.FragNbLost;
                        LmhpFragmentationState.DataBuffer[dataBufferIndex++] = FragSessionData[fragIndex].FragDecoderStatus.MatrixError & 0x01;

                        /* Fetch the co-efficient value required to calculate delay of that respective session. */
                        BlockAckDelay = FragSessionData[fragIndex].FragGroupData.Control.Fields.BlockAckDelay;
                        isAnswerDelayed = true;
                    }
                    break;
                }
            case FRAGMENTATION_FRAG_SESSION_SETUP_REQ:
                {
                    if( mcpsIndication->Multicast == 1 )
                    {
                        /* Multicast channel. Don't process command. */
                        break;
                    }
                    FragSessionData_t fragSessionData;
                    uint8_t status = 0x00;

                    fragSessionData.FragGroupData.FragSession.Value = mcpsIndication->Buffer[cmdIndex++];

                    fragSessionData.FragGroupData.FragNb = ( mcpsIndication->Buffer[cmdIndex++] << 0 ) & 0x00FF;
                    fragSessionData.FragGroupData.FragNb |= ( mcpsIndication->Buffer[cmdIndex++] << 8 ) & 0xFF00;

                    fragSessionData.FragGroupData.FragSize = mcpsIndication->Buffer[cmdIndex++];

                    fragSessionData.FragGroupData.Control.Value = mcpsIndication->Buffer[cmdIndex++];

                    fragSessionData.FragGroupData.Padding = mcpsIndication->Buffer[cmdIndex++];

                    fragSessionData.FragGroupData.Descriptor = ( mcpsIndication->Buffer[cmdIndex++] << 0 ) & 0x000000FF;
                    fragSessionData.FragGroupData.Descriptor += ( mcpsIndication->Buffer[cmdIndex++] << 8 ) & 0x0000FF00;
                    fragSessionData.FragGroupData.Descriptor += ( mcpsIndication->Buffer[cmdIndex++] << 16 ) & 0x00FF0000;
                    fragSessionData.FragGroupData.Descriptor += ( mcpsIndication->Buffer[cmdIndex++] << 24 ) & 0xFF000000;

#if ( FRAGMENTATION_VERSION == 2 )
                    /* SessionCnt and MIC filed added for V2*/
                    fragSessionData.FragGroupData.SessionCnt = ( mcpsIndication->Buffer[cmdIndex++] << 0 ) & 0x00FF;
                    fragSessionData.FragGroupData.SessionCnt += ( mcpsIndication->Buffer[cmdIndex++] << 8 ) & 0xFF00;

                    fragSessionData.FragGroupData.Mic = ( mcpsIndication->Buffer[cmdIndex++] << 0 ) & 0x000000FF;
                    fragSessionData.FragGroupData.Mic += ( mcpsIndication->Buffer[cmdIndex++] << 8 ) & 0x0000FF00;
                    fragSessionData.FragGroupData.Mic += ( mcpsIndication->Buffer[cmdIndex++] << 16 ) & 0x00FF0000;
                    fragSessionData.FragGroupData.Mic += ( mcpsIndication->Buffer[cmdIndex++] << 24 ) & 0xFF000000;
#endif /* FRAGMENTATION_VERSION */

                    if( fragSessionData.FragGroupData.Control.Fields.FragAlgo > 0 )
                    {
                        status |= 0x01; /* Encoding unsupported */
                    }

                    if( ( fragSessionData.FragGroupData.FragNb > FRAG_MAX_NB ) ||
                        ( fragSessionData.FragGroupData.FragSize > FRAG_MAX_SIZE ) ||
                        ( fragSessionData.FragGroupData.FragSize < FRAG_MIN_SIZE ) ||
                        ( ( fragSessionData.FragGroupData.FragNb * fragSessionData.FragGroupData.FragSize ) > FRAG_DECODER_DWL_REGION_SIZE ) )
                    {
                        status |= 0x02; /* Not enough Memory */
                    }
                    status |= ( fragSessionData.FragGroupData.FragSession.Fields.FragIndex << 6 ) & 0xC0;
                    if( fragSessionData.FragGroupData.FragSession.Fields.FragIndex >= FRAGMENTATION_MAX_SESSIONS )
                    {
                        status |= 0x04; /* FragSession index not supported */
                    }

                    /* Descriptor is not really defined in the specification */
                    /* Not clear how to handle this. */
                    /* Currently the descriptor is always correct */
                    if( fragSessionData.FragGroupData.Descriptor != 0x01020304 )
                    {
                        /* status |= 0x08; */ /* Wrong Descriptor */
                    }

#if ( FRAGMENTATION_VERSION == 1 )
                    if( ( status & 0x0F ) == 0 )
                    {
#elif ( FRAGMENTATION_VERSION == 2 )
                    if( SessionCntPrev[fragSessionData.FragGroupData.FragSession.Fields.FragIndex] >=
                        fragSessionData.FragGroupData.SessionCnt )
                    {
                        status |= 0x10; /* SessionCnt Replay */
                    }

                    if( ( status & 0x1F ) == 0 )
                    {
#endif /* FRAGMENTATION_VERSION */
                        /* The FragSessionSetup is accepted */
                        fragSessionData.FragGroupData.IsActive = true;
                        fragSessionData.FragDecoderProcessStatus = FRAG_SESSION_ONGOING;
                        FragSessionData[fragSessionData.FragGroupData.FragSession.Fields.FragIndex] = fragSessionData;
                        FragDecoderInit( fragSessionData.FragGroupData.FragNb,
                                         fragSessionData.FragGroupData.FragSize,
                                         &LmhpFragmentationParams->DecoderCallbacks,
                                         FRAGMENTATION_VERSION );
                    }
                    LmhpFragmentationState.DataBuffer[dataBufferIndex++] = FRAGMENTATION_FRAG_SESSION_SETUP_ANS;
                    LmhpFragmentationState.DataBuffer[dataBufferIndex++] = status;
                    isAnswerDelayed = false;
                    break;
                }
            case FRAGMENTATION_FRAG_SESSION_DELETE_REQ:
                {
                    if( mcpsIndication->Multicast == 1 )
                    {
                        /* Multicast channel. Don't process command. */
                        break;
                    }
                    uint8_t status = 0x00;
                    uint8_t id = mcpsIndication->Buffer[cmdIndex++] & 0x03;

                    status |= id;
                    if( ( id >= FRAGMENTATION_MAX_SESSIONS ) || ( FragSessionData[id].FragGroupData.IsActive == false ) )
                    {
                        status |= 0x04; /* Session does not exist */
                    }
                    else
                    {
                        /* Delete session */
                        FragSessionData[id].FragGroupData.IsActive = false;
                    }
                    LmhpFragmentationState.DataBuffer[dataBufferIndex++] = FRAGMENTATION_FRAG_SESSION_DELETE_ANS;
                    LmhpFragmentationState.DataBuffer[dataBufferIndex++] = status;
                    isAnswerDelayed = false;
                    break;
                }
#if ( FRAGMENTATION_VERSION == 2 )
            case FRAGMENTATION_FRAG_DATA_BLOCK_RECEIVED_ANS:
                {
                    if( mcpsIndication->Multicast == 1 )
                    {
                        /* Multicast channel. Don't process command. */
                        break;
                    }
                    uint8_t fragIndex = mcpsIndication->Buffer[cmdIndex++] & 0x03;

                    if( ( FragSessionData[fragIndex].FragGroupData.FragSession.Fields.FragIndex == fragIndex )
                        && ( LmhpFragmentationState.FragDataBlockAnsRequired == true ) )
                    {
                        TimerStop( &FragmentProcessTimer );
                        LmhpFragmentationState.IsTxPending = false;
                        LmhpFragmentationState.DataBufferSize = 0;
                        LmhpFragmentationState.FragDataBlockAnsRequired = false;
                    }
                    break;
                }
#endif /* FRAGMENTATION_VERSION */
            case FRAGMENTATION_DATA_FRAGMENT:
                {
                    uint8_t fragIndex = 0;
                    uint16_t fragCounter = 0;

                    fragCounter = ( mcpsIndication->Buffer[cmdIndex++] << 0 ) & 0x00FF;
                    fragCounter |= ( mcpsIndication->Buffer[cmdIndex++] << 8 ) & 0xFF00;

                    fragIndex = ( fragCounter >> 14 ) & 0x03;
                    fragCounter &= 0x3FFF;
                    if( FragSessionData[fragIndex].FragGroupData.IsActive == false )
                    {
                        cmdIndex = mcpsIndication->BufferSize;
                        break;
                    }
                    if( mcpsIndication->Multicast == 1 )
                    {
                        /* Message received on a multicast address */
                        /* Check McGroupBitMask */
                        uint8_t groupId = LoRaMacMcChannelGetGroupId( mcpsIndication->DevAddress );
                        if( ( groupId == 0xFF ) ||
                            ( ( FragSessionData[fragIndex].FragGroupData.FragSession.Fields.McGroupBitMask & ( 1 << groupId ) ) == 0 ) )
                        {
                            /* Ignore message */
                            cmdIndex = mcpsIndication->BufferSize;
                            break;
                        }
                    }

                    if( FragSessionData[fragIndex].FragDecoderProcessStatus == FRAG_SESSION_ONGOING )
                    {
                        memcpy(buffer, (void *)&mcpsIndication->Buffer[cmdIndex], FragSessionData[fragIndex].FragGroupData.FragSize);
                        FragSessionData[fragIndex].FragDecoderProcessStatus = FragDecoderProcess( fragCounter, buffer );
                        FragSessionData[fragIndex].FragDecoderStatus = FragDecoderGetStatus( );
                        if( LmhpFragmentationParams->OnProgress != NULL )
                        {
                            LmhpFragmentationParams->OnProgress( FragSessionData[fragIndex].FragDecoderStatus.FragNbRx,
                                                                 FragSessionData[fragIndex].FragGroupData.FragNb,
                                                                 FragSessionData[fragIndex].FragGroupData.FragSize,
                                                                 FragSessionData[fragIndex].FragDecoderStatus.FragNbLost );
                        }

                        if( FragSessionData[fragIndex].FragDecoderProcessStatus >= 0 )
                        {
                            uint32_t UnfragmentedBufferAddr;
                            /* Fragmentation successfully done */
                            if( LmhpFragmentationParams->OnDone != NULL )
                            {
                                LmhpFragmentationParams->OnDone( FragSessionData[fragIndex].FragDecoderProcessStatus,
                                                                 ( FragSessionData[fragIndex].FragGroupData.FragNb * FragSessionData[fragIndex].FragGroupData.FragSize ) -
                                                                 FragSessionData[fragIndex].FragGroupData.Padding,
                                                                 &UnfragmentedBufferAddr );
                            }

#if ( FRAGMENTATION_VERSION == 2 )
                            /*If AckReception = 0, the end-device SHALL do nothing */
                            if( FragSessionData[fragIndex].FragGroupData.Control.Fields.AckReception == 1 )
                            {
                                uint8_t status = 0x00;
                                uint32_t micComputed = 0;

                                status = fragIndex;

                                /* Compute MIC */
                                LoRaMacProcessMicForDatablock( ( uint8_t * )UnfragmentedBufferAddr,
                                                               ( FragSessionData[fragIndex].FragGroupData.FragNb * FragSessionData[fragIndex].FragGroupData.FragSize ) -
                                                               FragSessionData[fragIndex].FragGroupData.Padding,
                                                               FragSessionData[fragIndex].FragGroupData.SessionCnt,
                                                               fragIndex,
                                                               FragSessionData[fragIndex].FragGroupData.Descriptor,
                                                               &micComputed );
                                MW_LOG( TS_OFF, VLEVEL_M, "MIC         : %08X\r\n", micComputed );

                                /* check if the MIC computed is equal to the MIC received */
                                if( micComputed != FragSessionData[fragIndex].FragGroupData.Mic )
                                {
                                    status |= 0x04;
                                }

                                LmhpFragmentationState.DataBuffer[dataBufferIndex++] = FRAGMENTATION_FRAG_DATA_BLOCK_RECEIVED_REQ;
                                LmhpFragmentationState.DataBuffer[dataBufferIndex++] = status;
                                BlockAckDelay = FragSessionData[fragIndex].FragGroupData.Control.Fields.BlockAckDelay;
                                isAnswerDelayed = true;
                                LmhpFragmentationState.FragDataBlockAnsRequired = true;
                            }
#endif /* FRAGMENTATION_VERSION */

                            FragSessionData[fragIndex].FragDecoderProcessStatus = FRAG_SESSION_NOT_STARTED;
                        }
                    }
                    cmdIndex += FragSessionData[fragIndex].FragGroupData.FragSize;
#if ( FRAGMENTATION_VERSION == 2 )
                    /* Store the previous session counter*/
                    SessionCntPrev[fragIndex] = FragSessionData[fragIndex].FragGroupData.SessionCnt;
#endif /* FRAGMENTATION_VERSION */
                    break;
                }
            default:
                {
                    break;
                }
        }
    }

    /* After processing the commands, if the end-node has to reply back then a flag is checked if the */
    /* reply is to be sent immediately or with a delay. */
    /* In some scenarios it is not desired that multiple end-notes send uplinks at the same time to */
    /* the same server. (Example: Fragment status during a multicast FUOTA) */
    if( dataBufferIndex != 0 )
    {
        /* Prepare Answer that is to be transmitted */
        LmhpFragmentationState.DataBufferSize = dataBufferIndex;

        if( isAnswerDelayed == true )
        {
            /* Delay value is calculated using BlockAckDelay which is communicated by server during the FragSessionSetupReq
             * Pseudo Random Delay = rand(0:1) * 2^(BlockAckDelay + 4) Seconds.
             * Delay = Pseudo Random Delay * 1000 milli seconds.
             * Eg: BlockAckDelay = 7
             *     Pseudo Random Delay = rand(0:1) * 2^11
             *     rand(0:1) seconds = rand(0:1000) milliseconds
             *     Delay = rand(0:1000) * 2048 => 2048000ms = 34 minutes
             * To prevent too early execution, a minimum delay of 3s is added
             */
            TxDelayTime = 3000 + ( randr( 0, 1000 ) * ( 1 << ( BlockAckDelay + 4 ) ) );
        }
        else
        {
            TxDelayTime = 3000;
        }
        TimerSetValue( &FragmentProcessTimer, TxDelayTime );
        TimerStart( &FragmentProcessTimer );
    }
}
