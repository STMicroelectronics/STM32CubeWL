/*!
 * \file      LmhpRemoteMcastSetup.c
 *
 * \brief     Implements the LoRa-Alliance remote multicast setup package
 *            Specification V1.0.0: https://resources.lora-alliance.org/technical-specifications/lorawan-remote-multicast-setup-specification-v1-0-0
 *            Specification V2.0.0: https://resources.lora-alliance.org/technical-specifications/ts005-2-0-0-remote-multicast-setup
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
  * @file    LmhpRemoteMcastSetup.c
  * @author  MCD Application Team
  * @brief   Remote Multicast Package definition
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "LoRaMac.h"
#include "LmHandler.h"
#include "LmhpRemoteMcastSetup.h"
#include "mw_log_conf.h"  /* needed for MW_LOG */

/*!
 * LoRaWAN Application Layer Remote multicast setup Specification
 */
#define REMOTE_MCAST_SETUP_PORT                     200

#define REMOTE_MCAST_SETUP_ID                       2

#if (LORAWAN_PACKAGES_VERSION == 1)
#define REMOTE_MCAST_SETUP_VERSION                  1
#elif (LORAWAN_PACKAGES_VERSION == 2)
#define REMOTE_MCAST_SETUP_VERSION                  2
#endif /* LORAWAN_PACKAGES_VERSION */

typedef enum LmhpRemoteMcastSetupSessionStates_e
{
    REMOTE_MCAST_SETUP_SESSION_STATE_IDLE,
    REMOTE_MCAST_SETUP_SESSION_STATE_START,
    REMOTE_MCAST_SETUP_SESSION_STATE_STOP,
} LmhpRemoteMcastSetupSessionStates_t;

/*!
 * Package current context
 */
typedef struct LmhpRemoteMcastSetupState_s
{
    bool Initialized;
    bool IsTxPending;
    LmhpRemoteMcastSetupSessionStates_t SessionState;
    uint8_t ID;
    uint8_t DataBufferMaxSize;
    uint8_t *DataBuffer;
} LmhpRemoteMcastSetupState_t;

typedef enum LmhpRemoteMcastSetupMoteCmd_e
{
    REMOTE_MCAST_SETUP_PKG_VERSION_ANS              = 0x00,
    REMOTE_MCAST_SETUP_MC_GROUP_STATUS_ANS          = 0x01,
    REMOTE_MCAST_SETUP_MC_GROUP_SETUP_ANS           = 0x02,
    REMOTE_MCAST_SETUP_MC_GROUP_DELETE_ANS          = 0x03,
    REMOTE_MCAST_SETUP_MC_GROUP_CLASS_C_SESSION_ANS = 0x04,
    REMOTE_MCAST_SETUP_MC_GROUP_CLASS_B_SESSION_ANS = 0x05,
} LmhpRemoteMcastSetupMoteCmd_t;

typedef enum LmhpRemoteMcastSetupSrvCmd_e
{
    REMOTE_MCAST_SETUP_PKG_VERSION_REQ              = 0x00,
    REMOTE_MCAST_SETUP_MC_GROUP_STATUS_REQ          = 0x01,
    REMOTE_MCAST_SETUP_MC_GROUP_SETUP_REQ           = 0x02,
    REMOTE_MCAST_SETUP_MC_GROUP_DELETE_REQ          = 0x03,
    REMOTE_MCAST_SETUP_MC_GROUP_CLASS_C_SESSION_REQ = 0x04,
    REMOTE_MCAST_SETUP_MC_GROUP_CLASS_B_SESSION_REQ = 0x05,
} LmhpRemoteMcastSetupSrvCmd_t;

/*!
 * Initializes the package with provided parameters
 *
 * \param [in] params            Pointer to the package parameters
 * \param [in] dataBuffer        Pointer to main application buffer
 * \param [in] dataBufferMaxSize Main application buffer maximum size
 */
static void LmhpRemoteMcastSetupInit( void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize );

/*!
 * Returns the current package initialization status.
 *
 * \retval status Package initialization status
 *                [true: Initialized, false: Not initialized]
 */
static bool LmhpRemoteMcastSetupIsInitialized( void );

/*!
 * Returns if a package transmission is pending or not.
 *
 * \retval status Package transmission status
 *                [true: pending, false: Not pending]
 */
static bool LmhpRemoteMcastSetupIsTxPending( void );

/*!
 * Processes the internal package events.
 */
static void LmhpRemoteMcastSetupProcess( void );

/*!
 * Processes the MCPS Indication
 *
 * \param [in] mcpsIndication     MCPS indication primitive data
 */
static void LmhpRemoteMcastSetupOnMcpsIndication( McpsIndication_t *mcpsIndication );

#if ( LORAMAC_MAX_MC_CTX > 0 )
static void OnSessionStartTimer0( void *context );
static void OnSessionStopTimer0( void *context );
#endif /* LORAMAC_MAX_MC_CTX > 0 */
#if ( LORAMAC_MAX_MC_CTX > 1 )
static void OnSessionStartTimer1( void *context );
static void OnSessionStopTimer1( void *context );
#endif /* LORAMAC_MAX_MC_CTX > 1 */
#if ( LORAMAC_MAX_MC_CTX > 2 )
static void OnSessionStartTimer2( void *context );
static void OnSessionStopTimer2( void *context );
#endif /* LORAMAC_MAX_MC_CTX > 2 */
#if ( LORAMAC_MAX_MC_CTX > 3 )
static void OnSessionStartTimer3( void *context );
static void OnSessionStopTimer3( void *context );
#endif /* LORAMAC_MAX_MC_CTX > 3 */

static LmhpRemoteMcastSetupState_t LmhpRemoteMcastSetupState =
{
    .Initialized = false,
    .IsTxPending = false,
    .SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_IDLE,
    .ID = 0xFF,
};

typedef struct McGroupData_s
{
    uint8_t McGroupEnabled;
    union
    {
        uint8_t Value;
        struct
        {
            uint8_t McGroupId:   2;
            uint8_t RFU:         6;
        } Fields;
    } IdHeader;
    uint32_t McAddr;
    uint8_t McKeyEncrypted[16];
    uint32_t McFCountMin;
    uint32_t McFCountMax;
} McGroupData_t;

typedef enum eSessionState
{
    SESSION_STOPPED,
    SESSION_STARTED
} SessionState_t;

typedef struct McSessionData_s
{
    McGroupData_t McGroupData;
    SessionState_t SessionState;
    uint32_t SessionTime;
    uint8_t SessionTimeout;
    McRxParams_t RxParams;
} McSessionData_t;

static McSessionData_t McSessionData[LORAMAC_MAX_MC_CTX];

/*!
 * Session start timer
 */
static TimerEvent_t SessionStartTimer[LORAMAC_MAX_MC_CTX];

/*!
 * Session start timer
 */
static TimerEvent_t SessionStopTimer[LORAMAC_MAX_MC_CTX];

static LmhPackage_t LmhpRemoteMcastSetupPackage =
{
    .Port = REMOTE_MCAST_SETUP_PORT,
    .Init = LmhpRemoteMcastSetupInit,
    .IsInitialized = LmhpRemoteMcastSetupIsInitialized,
    .IsTxPending = LmhpRemoteMcastSetupIsTxPending,
    .Process = LmhpRemoteMcastSetupProcess,
    .OnPackageProcessEvent = NULL,                             /* To be initialized by LmHandler */
    .OnMcpsConfirmProcess = NULL,                              /* Not used in this package */
    .OnMcpsIndicationProcess = LmhpRemoteMcastSetupOnMcpsIndication,
    .OnMlmeConfirmProcess = NULL,                              /* Not used in this package */
    .OnMlmeIndicationProcess = NULL,                           /* Not used in this package */
    .OnJoinRequest = NULL,                                     /* To be initialized by LmHandler */
    .OnDeviceTimeRequest = NULL,                               /* To be initialized by LmHandler */
    .OnSysTimeUpdate = NULL,                                   /* To be initialized by LmHandler */
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    .OnSystemReset = NULL,                                     /* To be initialized by LmHandler */
#endif /* LORAMAC_VERSION */
};

LmhPackage_t *LmhpRemoteMcastSetupPackageFactory( void )
{
    return &LmhpRemoteMcastSetupPackage;
}

static void LmhpRemoteMcastSetupInit( void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize )
{
    if( dataBuffer != NULL )
    {
        LmhpRemoteMcastSetupState.DataBuffer = dataBuffer;
        LmhpRemoteMcastSetupState.DataBufferMaxSize = dataBufferMaxSize;
        LmhpRemoteMcastSetupState.Initialized = true;
#if ( LORAMAC_MAX_MC_CTX > 0 )
        TimerInit( &SessionStartTimer[0], OnSessionStartTimer0 );
        TimerInit( &SessionStopTimer[0], OnSessionStopTimer0 );
#endif /* LORAMAC_MAX_MC_CTX > 0 */
#if ( LORAMAC_MAX_MC_CTX > 1 )
        TimerInit( &SessionStartTimer[1], OnSessionStartTimer1 );
        TimerInit( &SessionStopTimer[1], OnSessionStopTimer1 );
#endif /* LORAMAC_MAX_MC_CTX > 1 */
#if ( LORAMAC_MAX_MC_CTX > 2 )
        TimerInit( &SessionStartTimer[2], OnSessionStartTimer2 );
        TimerInit( &SessionStopTimer[2], OnSessionStopTimer2 );
#endif /* LORAMAC_MAX_MC_CTX > 2 */
#if ( LORAMAC_MAX_MC_CTX > 3 )
        TimerInit( &SessionStartTimer[3], OnSessionStartTimer3 );
        TimerInit( &SessionStopTimer[3], OnSessionStopTimer3 );
#endif /* LORAMAC_MAX_MC_CTX > 3 */
    }
    else
    {
        LmhpRemoteMcastSetupState.Initialized = false;
    }
    LmhpRemoteMcastSetupState.IsTxPending = false;

    for( uint8_t id = 0; id < LORAMAC_MAX_MC_CTX; id++ )
    {
        McSessionData[id].McGroupData.McGroupEnabled = false;
    }
}

static bool LmhpRemoteMcastSetupIsInitialized( void )
{
    return LmhpRemoteMcastSetupState.Initialized;
}

static bool LmhpRemoteMcastSetupIsTxPending( void )
{
    return LmhpRemoteMcastSetupState.IsTxPending;
}

static void LmhpRemoteMcastSetupProcess( void )
{
    LmhpRemoteMcastSetupSessionStates_t state;
    uint8_t id;
    bool active_session = false;
    DeviceClass_t deviceClass = CLASS_A;

    CRITICAL_SECTION_BEGIN( );
    state = LmhpRemoteMcastSetupState.SessionState;
    id = LmhpRemoteMcastSetupState.ID;
    CRITICAL_SECTION_END( );

    switch( state )
    {
        case REMOTE_MCAST_SETUP_SESSION_STATE_START:

            LmHandlerGetCurrentClass( &deviceClass );
            if( ( ( McSessionData[id].RxParams.Class == CLASS_B ) && ( deviceClass == CLASS_C ) ) ||
                ( ( McSessionData[id].RxParams.Class == CLASS_C ) && ( deviceClass == CLASS_B ) ) )
            {
                LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_IDLE;
            }
            else
            {
                /* Switch to Class B or C */
                if( LmHandlerRequestClass( McSessionData[id].RxParams.Class ) == LORAMAC_HANDLER_SUCCESS )
                {
                    LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_IDLE;

                    if( McSessionData[id].RxParams.Class == CLASS_B )
                    {
                        TimerSetValue( &SessionStopTimer[id], ( 1 << McSessionData[id].SessionTimeout ) * 1000 * 128 );
                    }
                    else /* CLASS_C */
                    {
                        TimerSetValue( &SessionStopTimer[id], ( 1 << McSessionData[id].SessionTimeout ) * 1000 );
                    }
                    TimerStart( &SessionStopTimer[id] );
                }
                else
                {
                    TimerSetValue( &SessionStartTimer[id], 1000 );
                    TimerStart( &SessionStartTimer[id] );
                }
            }
            break;
        case REMOTE_MCAST_SETUP_SESSION_STATE_STOP:
            for( uint8_t id_index = 0; id_index < LORAMAC_MAX_MC_CTX; id_index++ )
            {
                if( McSessionData[id_index].SessionState == SESSION_STARTED )
                {
                    active_session = true;
                    break;
                }
            }

            if( active_session == false )
            {
                /* Switch back to Class A */
                if( LmHandlerRequestClass( CLASS_A ) == LORAMAC_HANDLER_SUCCESS )
                {
                    LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_IDLE;
                }
                else
                {
                    TimerSetValue( &SessionStopTimer[id], 1000 );
                    TimerStart( &SessionStopTimer[id] );
                }
            }
            break;
        case REMOTE_MCAST_SETUP_SESSION_STATE_IDLE:
            break;
        default:
            break;
    }
}

static void LmhpRemoteMcastSetupOnMcpsIndication( McpsIndication_t *mcpsIndication )
{
    uint8_t cmdIndex = 0;
    uint8_t dataBufferIndex = 0;
    uint8_t id = 0xFF;

    if( mcpsIndication->Port != REMOTE_MCAST_SETUP_PORT )
    {
        return;
    }

    while( cmdIndex < mcpsIndication->BufferSize )
    {
        switch( mcpsIndication->Buffer[cmdIndex++] )
        {
            case REMOTE_MCAST_SETUP_PKG_VERSION_REQ:
                {
                    LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_PKG_VERSION_ANS;
                    LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_ID;
                    LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_VERSION;
                    break;
                }
            case REMOTE_MCAST_SETUP_MC_GROUP_STATUS_REQ:
                {
                    uint8_t nbAvailableGroups = 0;
                    uint8_t reqGroupMask = mcpsIndication->Buffer[cmdIndex++] & 0x0F;
                    uint8_t AnsGroupMask = 0x00;

                    LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_MC_GROUP_STATUS_ANS;

                    /* move index to the next first optional list */
                    dataBufferIndex++;
                    for( id = 0; id < LORAMAC_MAX_MC_CTX; id++ )
                    {
                        if( McSessionData[id].McGroupData.McGroupEnabled )
                        {
                            nbAvailableGroups++;

                            /* If multicast group defined in the input bit mask */
                            if( ( reqGroupMask & ( 1 << id ) ) != 0 )
                            {
                                AnsGroupMask |= ( 1 << id );
                                LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = id;
                                LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = ( McSessionData[id].McGroupData.McAddr >> 0 ) & 0xFF;
                                LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = ( McSessionData[id].McGroupData.McAddr >> 8 ) & 0xFF;
                                LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = ( McSessionData[id].McGroupData.McAddr >> 16 ) & 0xFF;
                                LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = ( McSessionData[id].McGroupData.McAddr >> 24 ) & 0xFF;
                            }
                        }
                    }
                    /* set the status bit */
                    LmhpRemoteMcastSetupState.DataBuffer[1] = ( nbAvailableGroups & 0x07 ) << 4 | ( AnsGroupMask & 0x0F );

                    break;
                }
            case REMOTE_MCAST_SETUP_MC_GROUP_SETUP_REQ:
                {
                    uint8_t idError = 0x01; /* One bit value */
                    id = mcpsIndication->Buffer[cmdIndex++] & 0x03;

                    McSessionData[id].McGroupData.IdHeader.Value = id;

                    if( id < LORAMAC_MAX_MC_CTX )
                    {
                        McSessionData[id].McGroupData.McAddr = ( mcpsIndication->Buffer[cmdIndex++] << 0 ) & 0x000000FF;
                        McSessionData[id].McGroupData.McAddr += ( mcpsIndication->Buffer[cmdIndex++] << 8 ) & 0x0000FF00;
                        McSessionData[id].McGroupData.McAddr += ( mcpsIndication->Buffer[cmdIndex++] << 16 ) & 0x00FF0000;
                        McSessionData[id].McGroupData.McAddr += ( mcpsIndication->Buffer[cmdIndex++] << 24 ) & 0xFF000000;

                        for( int8_t i = 0; i < 16; i++ )
                        {
                            McSessionData[id].McGroupData.McKeyEncrypted[i] = mcpsIndication->Buffer[cmdIndex++];
                        }

                        McSessionData[id].McGroupData.McFCountMin = ( mcpsIndication->Buffer[cmdIndex++] << 0 ) & 0x000000FF;
                        McSessionData[id].McGroupData.McFCountMin += ( mcpsIndication->Buffer[cmdIndex++] << 8 ) & 0x0000FF00;
                        McSessionData[id].McGroupData.McFCountMin += ( mcpsIndication->Buffer[cmdIndex++] << 16 ) & 0x00FF0000;
                        McSessionData[id].McGroupData.McFCountMin += ( mcpsIndication->Buffer[cmdIndex++] << 24 ) & 0xFF000000;

                        McSessionData[id].McGroupData.McFCountMax = ( mcpsIndication->Buffer[cmdIndex++] << 0 ) & 0x000000FF;
                        McSessionData[id].McGroupData.McFCountMax += ( mcpsIndication->Buffer[cmdIndex++] << 8 ) & 0x0000FF00;
                        McSessionData[id].McGroupData.McFCountMax += ( mcpsIndication->Buffer[cmdIndex++] << 16 ) & 0x00FF0000;
                        McSessionData[id].McGroupData.McFCountMax += ( mcpsIndication->Buffer[cmdIndex++] << 24 ) & 0xFF000000;

                        McChannelParams_t channel =
                        {
                            .IsRemotelySetup = true,
                            .IsEnabled = true,
                            .GroupID = ( AddressIdentifier_t )McSessionData[id].McGroupData.IdHeader.Fields.McGroupId,
                            .Address = McSessionData[id].McGroupData.McAddr,
                            .McKeys.McKeyE = McSessionData[id].McGroupData.McKeyEncrypted,
                            .FCountMin = McSessionData[id].McGroupData.McFCountMin,
                            .FCountMax = McSessionData[id].McGroupData.McFCountMax,
                            .RxParams.Params.ClassC = /* Field not used for multicast channel setup. Must be initialized to something */
                            {
                                .Frequency = 0,
                                .Datarate = 0
                            }
                        };

                        if( LoRaMacMcChannelSetup( &channel ) == LORAMAC_STATUS_OK )
                        {
                            idError = 0x00;
                            McSessionData[id].McGroupData.McGroupEnabled = true;
                        }
                    }
                    LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_MC_GROUP_SETUP_ANS;
                    LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = ( idError << 2 ) | McSessionData[id].McGroupData.IdHeader.Fields.McGroupId;
                    break;
                }
            case REMOTE_MCAST_SETUP_MC_GROUP_DELETE_REQ:
                {
                    uint8_t status = 0x00;
                    id = mcpsIndication->Buffer[cmdIndex++] & 0x03;

                    status = id;
                    McSessionData[id].McGroupData.IdHeader.Value = 0;
                    McSessionData[id].McGroupData.McAddr = 0;
                    memset1( McSessionData[id].McGroupData.McKeyEncrypted, 0x00, 16 );
                    McSessionData[id].McGroupData.McFCountMin = 0;
                    McSessionData[id].McGroupData.McFCountMax = 0;

                    LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_MC_GROUP_DELETE_ANS;

                    if( LoRaMacMcChannelDelete( ( AddressIdentifier_t )id ) != LORAMAC_STATUS_OK )
                    {
                        status |= 0x04; /* McGroupUndefined bit set */
                    }
                    else
                    {
                        McSessionData[id].McGroupData.McGroupEnabled = false;
                    }
                    LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = status;
                    break;
                }
            case REMOTE_MCAST_SETUP_MC_GROUP_CLASS_C_SESSION_REQ:
                {
                    bool isTimerSet = false;
                    int32_t timeToSessionStart = 0;
                    uint8_t status = 0x00;
                    id = mcpsIndication->Buffer[cmdIndex++] & 0x03;

                    if( id < LORAMAC_MAX_MC_CTX )
                    {
                        McSessionData[id].RxParams.Class = CLASS_C;

                        McSessionData[id].SessionTime = ( mcpsIndication->Buffer[cmdIndex++] << 0 ) & 0x000000FF;
                        McSessionData[id].SessionTime += ( mcpsIndication->Buffer[cmdIndex++] << 8 ) & 0x0000FF00;
                        McSessionData[id].SessionTime += ( mcpsIndication->Buffer[cmdIndex++] << 16 ) & 0x00FF0000;
                        McSessionData[id].SessionTime += ( mcpsIndication->Buffer[cmdIndex++] << 24 ) & 0xFF000000;

                        /* Add Unix to Gps epoch offset. The system time is based on Unix time. */
                        McSessionData[id].SessionTime += UNIX_GPS_EPOCH_OFFSET;

                        McSessionData[id].SessionTimeout =  mcpsIndication->Buffer[cmdIndex++] & 0x0F;

                        McSessionData[id].RxParams.Params.ClassC.Frequency = ( mcpsIndication->Buffer[cmdIndex++] << 0 ) & 0x000000FF;
                        McSessionData[id].RxParams.Params.ClassC.Frequency |= ( mcpsIndication->Buffer[cmdIndex++] << 8 ) & 0x0000FF00;
                        McSessionData[id].RxParams.Params.ClassC.Frequency |= ( mcpsIndication->Buffer[cmdIndex++] << 16 ) & 0x00FF0000;
                        McSessionData[id].RxParams.Params.ClassC.Frequency *= 100;
                        McSessionData[id].RxParams.Params.ClassC.Datarate = mcpsIndication->Buffer[cmdIndex++];

                        if( LoRaMacMcChannelSetupRxParams( ( AddressIdentifier_t )id, &McSessionData[id].RxParams,
                                                           &status ) == LORAMAC_STATUS_OK )
                        {
                            SysTime_t curTime = { .Seconds = 0, .SubSeconds = 0 };
                            curTime = SysTimeGet( );

                            timeToSessionStart = McSessionData[id].SessionTime - curTime.Seconds;
                            if( timeToSessionStart > 0 )
                            {
                                /* Start session start timer */
                                TimerSetValue( &SessionStartTimer[id], timeToSessionStart * 1000 );
                                TimerStart( &SessionStartTimer[id] );

                                isTimerSet = true;

                                MW_LOG( TS_OFF, VLEVEL_M, "Time2SessionStart: %d ms\r\n", timeToSessionStart * 1000 );
                            }
                            else
                            {
                                /* Session start time before current device time */
                                status |= 0x10; /* McGroupUndefined bit set */
                            }
                        }
                    }
                    else
                    {
                        status |= 0x10; /* McGroupUndefined bit set */
                    }

                    LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_MC_GROUP_CLASS_C_SESSION_ANS;
                    LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = status;
                    if( isTimerSet == true )
                    {
                        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = ( timeToSessionStart >> 0 ) & 0xFF;
                        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = ( timeToSessionStart >> 8 ) & 0xFF;
                        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = ( timeToSessionStart >> 16 ) & 0xFF;
                    }
                    break;
                }
            case REMOTE_MCAST_SETUP_MC_GROUP_CLASS_B_SESSION_REQ:
                {
                    bool isTimerSet = false;
                    int32_t timeToSessionStart = 0;
                    uint8_t status = 0x00;
                    id = mcpsIndication->Buffer[cmdIndex++] & 0x03;

                    if( id < LORAMAC_MAX_MC_CTX )
                    {
                        McSessionData[id].RxParams.Class = CLASS_B;

                        McSessionData[id].SessionTime = ( mcpsIndication->Buffer[cmdIndex++] << 0 ) & 0x000000FF;
                        McSessionData[id].SessionTime += ( mcpsIndication->Buffer[cmdIndex++] << 8 ) & 0x0000FF00;
                        McSessionData[id].SessionTime += ( mcpsIndication->Buffer[cmdIndex++] << 16 ) & 0x00FF0000;
                        McSessionData[id].SessionTime += ( mcpsIndication->Buffer[cmdIndex++] << 24 ) & 0xFF000000;

                        /* Add Unix to Gps epoch offset. The system time is based on Unix time. */
                        McSessionData[id].SessionTime += UNIX_GPS_EPOCH_OFFSET;

                        McSessionData[id].RxParams.Params.ClassB.Periodicity = ( mcpsIndication->Buffer[cmdIndex] >> 4 ) & 0x07;
                        McSessionData[id].SessionTimeout =  mcpsIndication->Buffer[cmdIndex++] & 0x0F;

                        McSessionData[id].RxParams.Params.ClassB.Frequency = ( mcpsIndication->Buffer[cmdIndex++] << 0 ) & 0x000000FF;
                        McSessionData[id].RxParams.Params.ClassB.Frequency |= ( mcpsIndication->Buffer[cmdIndex++] << 8 ) & 0x0000FF00;
                        McSessionData[id].RxParams.Params.ClassB.Frequency |= ( mcpsIndication->Buffer[cmdIndex++] << 16 ) & 0x00FF0000;
                        McSessionData[id].RxParams.Params.ClassB.Frequency *= 100;
                        McSessionData[id].RxParams.Params.ClassB.Datarate = mcpsIndication->Buffer[cmdIndex++];

                        if( LoRaMacMcChannelSetupRxParams( ( AddressIdentifier_t )id, &McSessionData[id].RxParams,
                                                           &status ) == LORAMAC_STATUS_OK )
                        {
                            SysTime_t curTime = { .Seconds = 0, .SubSeconds = 0 };
                            curTime = SysTimeGet( );

                            timeToSessionStart = McSessionData[id].SessionTime - curTime.Seconds;
                            if( timeToSessionStart > 0 )
                            {
                                /* Start session start timer */
                                TimerSetValue( &SessionStartTimer[id], timeToSessionStart * 1000 );
                                TimerStart( &SessionStartTimer[id] );

                                isTimerSet = true;

                                MW_LOG( TS_OFF, VLEVEL_M, "Time2SessionStart: %d ms\r\n", timeToSessionStart * 1000 );
                            }
                            else
                            {
                                /* Session start time before current device time */
                                status |= 0x10; /* McGroupUndefined bit set */
                            }
                        }
                    }
                    else
                    {
                        status |= 0x10; /* McGroupUndefined bit set */
                    }

                    LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = REMOTE_MCAST_SETUP_MC_GROUP_CLASS_B_SESSION_ANS;
                    LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = status;
                    if( isTimerSet == true )
                    {
                        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = ( timeToSessionStart >> 0 ) & 0xFF;
                        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = ( timeToSessionStart >> 8 ) & 0xFF;
                        LmhpRemoteMcastSetupState.DataBuffer[dataBufferIndex++] = ( timeToSessionStart >> 16 ) & 0xFF;
                    }
                    break;
                }
            default:
                {
                    break;
                }
        }
    }

    if( dataBufferIndex != 0 )
    {
        /* Answer commands */
        LmHandlerAppData_t appData =
        {
            .Buffer = LmhpRemoteMcastSetupState.DataBuffer,
            .BufferSize = dataBufferIndex,
            .Port = REMOTE_MCAST_SETUP_PORT
        };

        bool current_dutycycle;
        LmHandlerGetDutyCycleEnable( &current_dutycycle );

        /* force Duty Cycle OFF to this Send */
        LmHandlerSetDutyCycleEnable( false );
        LmHandlerSend( &appData, LORAMAC_HANDLER_UNCONFIRMED_MSG, true );

        /* restore initial Duty Cycle */
        LmHandlerSetDutyCycleEnable( current_dutycycle );

        if( id != 0xFF && id < LORAMAC_MAX_MC_CTX )
        {
            MW_LOG( TS_OFF, VLEVEL_M, "ID          : %d\r\n", McSessionData[id].McGroupData.IdHeader.Fields.McGroupId );
            MW_LOG( TS_OFF, VLEVEL_M, "McAddr      : %08X\r\n", McSessionData[id].McGroupData.McAddr );
            MW_LOG( TS_OFF, VLEVEL_M, "McKey       : %02X", McSessionData[id].McGroupData.McKeyEncrypted[0] );
            for( int32_t i = 1; i < 16; i++ )
            {
                MW_LOG( TS_OFF, VLEVEL_M, "-%02X",  McSessionData[id].McGroupData.McKeyEncrypted[i] );
            }
            MW_LOG( TS_OFF, VLEVEL_M, "\r\n" );
            MW_LOG( TS_OFF, VLEVEL_M, "McFCountMin : %u\r\n",  McSessionData[id].McGroupData.McFCountMin );
            MW_LOG( TS_OFF, VLEVEL_M, "McFCountMax : %u\r\n",  McSessionData[id].McGroupData.McFCountMax );
            MW_LOG( TS_OFF, VLEVEL_M, "SessionTime : %u\r\n",  McSessionData[id].SessionTime );
            MW_LOG( TS_OFF, VLEVEL_M, "SessionTimeT: %d s\r\n", ( 1 << McSessionData[id].SessionTimeout ) );
            if( McSessionData[id].RxParams.Class == CLASS_B )
            {
                MW_LOG( TS_OFF, VLEVEL_M, "Rx Freq     : %u\r\n", McSessionData[id].RxParams.Params.ClassB.Frequency );
                MW_LOG( TS_OFF, VLEVEL_M, "Rx DR       : DR_%d\r\n", McSessionData[id].RxParams.Params.ClassB.Datarate );
                MW_LOG( TS_OFF, VLEVEL_M, "Periodicity : %u\r\n", McSessionData[id].RxParams.Params.ClassB.Periodicity );
            }
            else
            {
                MW_LOG( TS_OFF, VLEVEL_M, "Rx Freq     : %u\r\n", McSessionData[id].RxParams.Params.ClassC.Frequency );
                MW_LOG( TS_OFF, VLEVEL_M, "Rx DR       : DR_%d\r\n", McSessionData[id].RxParams.Params.ClassC.Datarate );
            }
        }
    }
}

#if ( LORAMAC_MAX_MC_CTX > 0 )
static void OnSessionStartTimer0( void *context )
{
    TimerStop( &SessionStartTimer[0] );

    McSessionData[0].SessionState = SESSION_STARTED;
    LmhpRemoteMcastSetupState.ID = 0;
    LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_START;
    if( LmhpRemoteMcastSetupPackage.OnPackageProcessEvent != NULL )
    {
        LmhpRemoteMcastSetupPackage.OnPackageProcessEvent();
    }
}

static void OnSessionStopTimer0( void *context )
{
    TimerStop( &SessionStopTimer[0] );

    McSessionData[0].SessionState = SESSION_STOPPED;
    LmhpRemoteMcastSetupState.ID = 0;
    LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_STOP;
    if( LmhpRemoteMcastSetupPackage.OnPackageProcessEvent != NULL )
    {
        LmhpRemoteMcastSetupPackage.OnPackageProcessEvent();
    }
}
#endif /* LORAMAC_MAX_MC_CTX > 0 */
#if ( LORAMAC_MAX_MC_CTX > 1 )
static void OnSessionStartTimer1( void *context )
{
    TimerStop( &SessionStartTimer[1] );

    McSessionData[1].SessionState = SESSION_STARTED;
    LmhpRemoteMcastSetupState.ID = 1;
    LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_START;
    if( LmhpRemoteMcastSetupPackage.OnPackageProcessEvent != NULL )
    {
        LmhpRemoteMcastSetupPackage.OnPackageProcessEvent();
    }
}

static void OnSessionStopTimer1( void *context )
{
    TimerStop( &SessionStopTimer[1] );

    McSessionData[1].SessionState = SESSION_STOPPED;
    LmhpRemoteMcastSetupState.ID = 1;
    LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_STOP;
    if( LmhpRemoteMcastSetupPackage.OnPackageProcessEvent != NULL )
    {
        LmhpRemoteMcastSetupPackage.OnPackageProcessEvent();
    }
}
#endif /* LORAMAC_MAX_MC_CTX > 1 */
#if ( LORAMAC_MAX_MC_CTX > 2 )
static void OnSessionStartTimer2( void *context )
{
    TimerStop( &SessionStartTimer[2] );

    McSessionData[2].SessionState = SESSION_STARTED;
    LmhpRemoteMcastSetupState.ID = 2;
    LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_START;
    if( LmhpRemoteMcastSetupPackage.OnPackageProcessEvent != NULL )
    {
        LmhpRemoteMcastSetupPackage.OnPackageProcessEvent();
    }
}

static void OnSessionStopTimer2( void *context )
{
    TimerStop( &SessionStopTimer[2] );

    McSessionData[2].SessionState = SESSION_STOPPED;
    LmhpRemoteMcastSetupState.ID = 2;
    LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_STOP;
    if( LmhpRemoteMcastSetupPackage.OnPackageProcessEvent != NULL )
    {
        LmhpRemoteMcastSetupPackage.OnPackageProcessEvent();
    }
}
#endif /* LORAMAC_MAX_MC_CTX > 2 */
#if ( LORAMAC_MAX_MC_CTX > 3 )
static void OnSessionStartTimer3( void *context )
{
    TimerStop( &SessionStartTimer[3] );

    McSessionData[3].SessionState = SESSION_STARTED;
    LmhpRemoteMcastSetupState.ID = 3;
    LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_START;
    if( LmhpRemoteMcastSetupPackage.OnPackageProcessEvent != NULL )
    {
        LmhpRemoteMcastSetupPackage.OnPackageProcessEvent();
    }
}

static void OnSessionStopTimer3( void *context )
{
    TimerStop( &SessionStopTimer[3] );

    McSessionData[3].SessionState = SESSION_STOPPED;
    LmhpRemoteMcastSetupState.ID = 3;
    LmhpRemoteMcastSetupState.SessionState = REMOTE_MCAST_SETUP_SESSION_STATE_STOP;
    if( LmhpRemoteMcastSetupPackage.OnPackageProcessEvent != NULL )
    {
        LmhpRemoteMcastSetupPackage.OnPackageProcessEvent();
    }
}
#endif /* LORAMAC_MAX_MC_CTX > 3 */
