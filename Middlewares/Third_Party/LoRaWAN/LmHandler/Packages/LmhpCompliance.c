/*!
 * \file      LmhpCompliance.c
 *
 * \brief     Implements the LoRa-Alliance certification protocol handling
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
  * @file    LmhpCompliance.c
  * @author  MCD Application Team
  * @brief   Certification Protocol Handling definition
  ******************************************************************************
  */
#include "platform.h"
#include "NvmDataMgmt.h"
#include "LoRaMac.h"
#include "LoRaMacTest.h"
#include "LmHandler.h"
#include "LmhpCompliance.h"

/*!
 * LoRaWAN compliance certification protocol port number.
 *
 * LoRaWAN Specification V1.x.x, chapter 4.3.2
 */
#define COMPLIANCE_PORT 224

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
/*!
 * Defines the compliance mode data transmission duty cycle.
 * An uplink will be transmitted ever \ref COMPLIANCE_TX_DUTYCYCLE [ms].
 */
#define COMPLIANCE_TX_DUTYCYCLE                     5000

/*!
 * LoRaWAN compliance tests support data
 */
typedef struct ComplianceTestState_s
{
    bool Initialized;
    bool IsRunning;
    uint8_t State;
    bool IsTxConfirmed;
    uint8_t Port;
    uint8_t DataBufferMaxSize;
    uint8_t DataBufferSize;
    uint8_t *DataBuffer;
    uint16_t DownLinkCounter;
    bool LinkCheck;
    uint8_t DemodMargin;
    uint8_t NbGateways;
} ComplianceTestState_t;

#ifndef MIN
#define MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#endif /* MIN */

/*!
 * Timer to handle the application data transmission duty cycle
 */
static TimerEvent_t ComplianceTxNextPacketTimer;

/*!
 * Holds the compliance test current context
 */
static ComplianceTestState_t ComplianceTestState =
{
    .Initialized = false,
    .IsRunning = false,
    .State = 0,
    .IsTxConfirmed = false,
    .Port = 0,
    .DataBufferMaxSize = 0,
    .DataBufferSize = 0,
    .DataBuffer = NULL,
    .DownLinkCounter = 0,
    .LinkCheck = false,
    .DemodMargin = 0,
    .NbGateways = 0
};

/*!
 * LoRaWAN compliance tests protocol handler parameters
 */
static LmhpComplianceParams_t *LmhpComplianceParams;

/*!
 * Initializes the compliance tests with provided parameters
 *
 * \param [IN] params Structure containing the initial compliance
 *                    tests parameters.
 * \param [IN] dataBuffer        Pointer to main application buffer
 * \param [IN] dataBufferMaxSize Application buffer maximum size
 */
static void LmhpComplianceInit( void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize );

/*!
 * Returns the current compliance certification protocol initialization status.
 *
 * \retval status Compliance certification protocol initialization status
 *                [true: Initialized, false: Not initialized]
 */
static bool LmhpComplianceIsInitialized( void );

/*!
 * Returns the current compliance certification protocol handling status.
 *
 * \retval status Compliance certification protocol handling status
 *                [true: Running, false: Not running]
 */
static bool LmhpComplianceIsRunning( void );

/*!
 * Processes the LoRaMac Compliance events.
 */
static void LmhpComplianceProcess( void );

/*!
 * Processes the MCPS Confirm
 *
 * \param [in] mcpsConfirm MCPS confirmation primitive data
 */
static void LmhpComplianceOnMcpsConfirm( McpsConfirm_t *mcpsConfirm );

/*!
 * Processes the MCPS Indication
 *
 * \param [IN] mcpsIndication     MCPS indication primitive data
 */
static void LmhpComplianceOnMcpsIndication( McpsIndication_t *mcpsIndication );

/*!
 * Processes the MLME Confirm
 *
 * \param [IN] mlmeConfirm MLME confirmation primitive data
 */
static void LmhpComplianceOnMlmeConfirm( MlmeConfirm_t *mlmeConfirm );

/*!
 * Function executed on TxNextPacket Timeout event
 */
static void OnComplianceTxNextPacketTimerEvent( void *context );

/*!
 * Processes the data to transmit on port \ref COMPLIANCE_PORT
 * Handles the compliance certification protocol data transmission
 *
 * \retval status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                processed else \ref LORAMAC_HANDLER_ERROR
 */
static LmHandlerErrorStatus_t LmhpComplianceTxProcess( void );

static LmhPackage_t LmhpCompliancePackage =
{
    .Port = COMPLIANCE_PORT,
    .Init = LmhpComplianceInit,
    .IsInitialized = LmhpComplianceIsInitialized,
    .IsRunning = LmhpComplianceIsRunning,
    .Process = LmhpComplianceProcess,
    .OnMcpsConfirmProcess =       LmhpComplianceOnMcpsConfirm,
    .OnMcpsIndicationProcess = LmhpComplianceOnMcpsIndication,
    .OnMlmeConfirmProcess = LmhpComplianceOnMlmeConfirm,
    .OnJoinRequest = NULL,                                     /* To be initialized by LmHandler */
    .OnSendRequest = NULL,                                     /* To be initialized by LmHandler */
    .OnDeviceTimeRequest = NULL,                               /* To be initialized by LmHandler */
    .OnSysTimeUpdate = NULL,                                   /* To be initialized by LmHandler */
    .OnPackageProcessEvent = NULL,                             /* To be initialized by LmHandler */
};

LmhPackage_t *LmhpCompliancePackageFactory( void )
{
    return &LmhpCompliancePackage;
}

static void LmhpComplianceInit( void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize )
{
    if( ( params != NULL ) && ( dataBuffer != NULL ) )
    {
        LmhpComplianceParams = ( LmhpComplianceParams_t * )params;
        ComplianceTestState.DataBuffer = dataBuffer;
        ComplianceTestState.DataBufferMaxSize = dataBufferMaxSize;
        ComplianceTestState.Initialized = true;
    }
    else
    {
        LmhpComplianceParams = NULL;
        ComplianceTestState.Initialized = false;
    }
}

static bool LmhpComplianceIsInitialized( void )
{
    return ComplianceTestState.Initialized;
}

static bool LmhpComplianceIsRunning( void )
{
    if( ComplianceTestState.Initialized == false )
    {
        return false;
    }

    return ComplianceTestState.IsRunning;
}

static void LmhpComplianceOnMcpsConfirm( McpsConfirm_t *mcpsConfirm )
{
    if( ComplianceTestState.Initialized == false )
    {
        return;
    }

    if( ( ComplianceTestState.IsRunning == true ) &&
        ( mcpsConfirm->McpsRequest == MCPS_CONFIRMED ) &&
        ( mcpsConfirm->AckReceived != 0 ) )
    {
        /* Increment the compliance certification protocol downlink counter */
        ComplianceTestState.DownLinkCounter++;
    }
}

static void LmhpComplianceOnMlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    if( ComplianceTestState.Initialized == false )
    {
        return;
    }

    if( ComplianceTestState.IsRunning == false )
    {
        return;
    }

    if( mlmeConfirm->MlmeRequest == MLME_LINK_CHECK )
    {
        ComplianceTestState.LinkCheck = true;
        ComplianceTestState.DemodMargin = mlmeConfirm->DemodMargin;
        ComplianceTestState.NbGateways = mlmeConfirm->NbGateways;
    }
}

static LmHandlerErrorStatus_t LmhpComplianceTxProcess( void )
{
    if( ComplianceTestState.Initialized == false )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    if( ComplianceTestState.IsRunning == false )
    {
        return LORAMAC_HANDLER_SUCCESS;
    }

    if( ComplianceTestState.LinkCheck == true )
    {
        ComplianceTestState.LinkCheck = false;
        ComplianceTestState.DataBufferSize = 3;
        ComplianceTestState.DataBuffer[0] = 5;
        ComplianceTestState.DataBuffer[1] = ComplianceTestState.DemodMargin;
        ComplianceTestState.DataBuffer[2] = ComplianceTestState.NbGateways;
        ComplianceTestState.State = 1;
    }
    else
    {
        switch( ComplianceTestState.State )
        {
            case 4:
                ComplianceTestState.State = 1;
                break;
            case 1:
                ComplianceTestState.DataBufferSize = 2;
                ComplianceTestState.DataBuffer[0] = ComplianceTestState.DownLinkCounter >> 8;
                ComplianceTestState.DataBuffer[1] = ComplianceTestState.DownLinkCounter;
                break;
        }
    }

    LmHandlerAppData_t appData =
    {
        .Buffer = ComplianceTestState.DataBuffer,
        .BufferSize = ComplianceTestState.DataBufferSize,
        .Port = COMPLIANCE_PORT
    };

    /* Schedule next transmission */
    TimerStart( &ComplianceTxNextPacketTimer );

    if( LmhpCompliancePackage.OnSendRequest == NULL)
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LmhpCompliancePackage.OnSendRequest( &appData, ( LmHandlerMsgTypes_t )ComplianceTestState.IsTxConfirmed, true );
}

static void LmhpComplianceOnMcpsIndication( McpsIndication_t *mcpsIndication )
{
    if( ComplianceTestState.Initialized == false )
    {
        return;
    }

    if( mcpsIndication->RxData == false )
    {
        return;
    }

    if( ( ComplianceTestState.IsRunning == true ) &&
        ( mcpsIndication->AckReceived == 0 ) )
    {
        /* Increment the compliance certification protocol downlink counter */
        ComplianceTestState.DownLinkCounter++;
    }

    if( mcpsIndication->Port != COMPLIANCE_PORT )
    {
        return;
    }

    if( ComplianceTestState.IsRunning == false )
    {
        /* Check compliance test enable command (i) */
        if( ( mcpsIndication->BufferSize == 4 ) &&
            ( mcpsIndication->Buffer[0] == 0x01 ) &&
            ( mcpsIndication->Buffer[1] == 0x01 ) &&
            ( mcpsIndication->Buffer[2] == 0x01 ) &&
            ( mcpsIndication->Buffer[3] == 0x01 ) )
        {
            MibRequestConfirm_t mibReq;

            /* Initialize compliance test mode context */
            ComplianceTestState.IsTxConfirmed = false;
            ComplianceTestState.Port = 224;
            ComplianceTestState.DataBufferSize = 2;
            ComplianceTestState.DownLinkCounter = 0;
            ComplianceTestState.LinkCheck = false;
            ComplianceTestState.DemodMargin = 0;
            ComplianceTestState.NbGateways = 0;
            ComplianceTestState.IsRunning = true;
            ComplianceTestState.State = 1;

            /* Enable ADR while in compliance test mode */
            mibReq.Type = MIB_ADR;
            mibReq.Param.AdrEnable = true;
            LoRaMacMibSetRequestConfirm( &mibReq );

            /* Disable duty cycle enforcement while in compliance test mode */
            LoRaMacTestSetDutyCycleOn( false );

            /* Stop peripherals */
            if( LmhpComplianceParams->StopPeripherals != NULL )
            {
                LmhpComplianceParams->StopPeripherals( );
            }
            /* Initialize compliance protocol transmission timer */
            TimerInit( &ComplianceTxNextPacketTimer, OnComplianceTxNextPacketTimerEvent );
            TimerSetValue( &ComplianceTxNextPacketTimer, COMPLIANCE_TX_DUTYCYCLE );

            /* Confirm compliance test protocol activation */
            LmhpComplianceTxProcess( );
        }
    }
    else
    {

        /* Parse compliance test protocol */
        ComplianceTestState.State = mcpsIndication->Buffer[0];
        switch( ComplianceTestState.State )
        {
            case 0: /* Check compliance test disable command (ii) */
                {
                    MibRequestConfirm_t mibReq;

                    TimerStop( &ComplianceTxNextPacketTimer );

                    /* Disable compliance test mode and reset the downlink counter. */
                    ComplianceTestState.DownLinkCounter = 0;
                    ComplianceTestState.IsRunning = false;

                    /* Restore previous ADR setting */
                    mibReq.Type = MIB_ADR;
                    mibReq.Param.AdrEnable = LmhpComplianceParams->AdrEnabled;
                    LoRaMacMibSetRequestConfirm( &mibReq );

                    /* Enable duty cycle enforcement */
                    LoRaMacTestSetDutyCycleOn( LmhpComplianceParams->DutyCycleEnabled );

                    /* Restart peripherals */
                    if( LmhpComplianceParams->StartPeripherals != NULL )
                    {
                        LmhpComplianceParams->StartPeripherals( );
                    }
                }
                break;
            case 1: /* (iii, iv) */
                ComplianceTestState.DataBufferSize = 2;
                break;
            case 2: /* Enable confirmed messages (v) */
                ComplianceTestState.IsTxConfirmed = true;
                ComplianceTestState.State = 1;
                break;
            case 3:  /* Disable confirmed messages (vi) */
                ComplianceTestState.IsTxConfirmed = false;
                ComplianceTestState.State = 1;
                break;
            case 4: /* (vii) */
                ComplianceTestState.DataBufferSize = mcpsIndication->BufferSize;

                ComplianceTestState.DataBuffer[0] = 4;
                for( uint8_t i = 1; i < MIN( ComplianceTestState.DataBufferSize, ComplianceTestState.DataBufferMaxSize ); i++ )
                {
                    ComplianceTestState.DataBuffer[i] = mcpsIndication->Buffer[i] + 1;
                }
                break;
            case 5: /* (viii) */
                {
                    MlmeReq_t mlmeReq;

                    mlmeReq.Type = MLME_LINK_CHECK;

                    LoRaMacMlmeRequest( &mlmeReq );
                }
                break;
            case 6: /* (ix) */
                {
                    MibRequestConfirm_t mibReq;

                    TimerStop( &ComplianceTxNextPacketTimer );

                    /* Disable TestMode and revert back to normal operation */
                    /* Disable compliance test mode and reset the downlink counter. */
                    ComplianceTestState.DownLinkCounter = 0;
                    ComplianceTestState.IsRunning = false;

                    /* Restore previous ADR setting */
                    mibReq.Type = MIB_ADR;
                    mibReq.Param.AdrEnable = LmhpComplianceParams->AdrEnabled;
                    LoRaMacMibSetRequestConfirm( &mibReq );

                    /* Enable duty cycle enforcement */
                    LoRaMacTestSetDutyCycleOn( LmhpComplianceParams->DutyCycleEnabled );

                    /* Restart peripherals */
                    if( LmhpComplianceParams->StartPeripherals != NULL )
                    {
                        LmhpComplianceParams->StartPeripherals( );
                    }

                    if( LmhpCompliancePackage.OnJoinRequest != NULL )
                    {
                        LmhpCompliancePackage.OnJoinRequest( ACTIVATION_TYPE_OTAA, true );
                    }
                }
                break;
            case 7: /* (x) */
                {
                    MlmeReq_t mlmeReq;
                    if( mcpsIndication->BufferSize == 3 )
                    {
                        mlmeReq.Type = MLME_TXCW;
                        mlmeReq.Req.TxCw.Timeout = ( uint16_t )( ( mcpsIndication->Buffer[1] << 8 ) | mcpsIndication->Buffer[2] );
                    }
                    else if( mcpsIndication->BufferSize == 7 )
                    {
                        mlmeReq.Type = MLME_TXCW_1;
                        mlmeReq.Req.TxCw.Timeout = ( uint16_t )( ( mcpsIndication->Buffer[1] << 8 ) | mcpsIndication->Buffer[2] );
                        mlmeReq.Req.TxCw.Frequency = ( uint32_t )( ( mcpsIndication->Buffer[3] << 16 ) | ( mcpsIndication->Buffer[4] << 8 ) | mcpsIndication->Buffer[5] ) * 100;
                        mlmeReq.Req.TxCw.Power = mcpsIndication->Buffer[6];
                    }
                    LoRaMacMlmeRequest( &mlmeReq );
                    ComplianceTestState.State = 1;
                }
                break;
            case 8: /* Send DeviceTimeReq */
                {
                    MlmeReq_t mlmeReq;

                    mlmeReq.Type = MLME_DEVICE_TIME;

                    LoRaMacMlmeRequest( &mlmeReq );
                }
                break;
            case 9: /* Switch end device Class */
                {
                    MibRequestConfirm_t mibReq;

                    mibReq.Type = MIB_DEVICE_CLASS;
                    /* CLASS_A = 0, CLASS_B = 1, CLASS_C = 2 */
                    mibReq.Param.Class = ( DeviceClass_t )mcpsIndication->Buffer[1];;
                    LoRaMacMibSetRequestConfirm( &mibReq );
                }
                break;
            case 10: /* Send PingSlotInfoReq */
                {
                    MlmeReq_t mlmeReq;

                    mlmeReq.Type = MLME_PING_SLOT_INFO;
                    mlmeReq.Req.PingSlotInfo.PingSlot.Value = mcpsIndication->Buffer[1];

                    LoRaMacMlmeRequest( &mlmeReq );
                }
                break;
            default:
                break;
        }
    }
}

static void LmhpComplianceProcess( void )
{
    /* Nothing to process */
}

static void OnComplianceTxNextPacketTimerEvent( void *context )
{
    LmhpComplianceTxProcess( );
}
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
#define COMPLIANCE_ID 6
#define COMPLIANCE_VERSION 1

typedef struct ClassBStatus_s
{
    bool         IsBeaconRxOn;
    uint8_t      PingSlotPeriodicity;
    uint16_t     BeaconCnt;
    BeaconInfo_t Info;
} ClassBStatus_t;

/*!
 * LoRaWAN compliance tests support data
 */
typedef struct ComplianceTestState_s
{
    bool                Initialized;
    bool                IsTxPending;
    TimerTime_t         TxPendingTimestamp;
    LmHandlerMsgTypes_t IsTxConfirmed;
    uint8_t             DataBufferMaxSize;
    uint8_t             DataBufferSize;
    uint8_t            *DataBuffer;
    uint16_t            RxAppCnt;
    bool                IsBeaconRxStatusIndOn;
    ClassBStatus_t      ClassBStatus;
    bool                IsResetCmdPending;
    bool                IsClassReqCmdPending;
    DeviceClass_t       NewClass;
} ComplianceTestState_t;

typedef enum ComplianceMoteCmd_e
{
    COMPLIANCE_PKG_VERSION_ANS      = 0x00,
    COMPLIANCE_ECHO_PAYLOAD_ANS     = 0x08,
    COMPLIANCE_RX_APP_CNT_ANS       = 0x09,
#if 0
    COMPLIANCE_BEACON_RX_STATUS_IND = 0x41,
    COMPLIANCE_BEACON_CNT_ANS       = 0x42,
#endif /* CLASS_B not available */
    COMPLIANCE_DUT_VERSION_ANS      = 0x7F,
} ComplianceMoteCmd_t;

typedef enum ComplianceSrvCmd_e
{
    COMPLIANCE_PKG_VERSION_REQ              = 0x00,
    COMPLIANCE_DUT_RESET_REQ                = 0x01,
    COMPLIANCE_DUT_JOIN_REQ                 = 0x02,
    COMPLIANCE_SWITCH_CLASS_REQ             = 0x03,
    COMPLIANCE_ADR_BIT_CHANGE_REQ           = 0x04,
    COMPLIANCE_REGIONAL_DUTY_CYCLE_CTRL_REQ = 0x05,
    COMPLIANCE_TX_PERIODICITY_CHANGE_REQ    = 0x06,
    COMPLIANCE_TX_FRAMES_CTRL_REQ           = 0x07,
    COMPLIANCE_ECHO_PAYLOAD_REQ             = 0x08,
    COMPLIANCE_RX_APP_CNT_REQ               = 0x09,
    COMPLIANCE_RX_APP_CNT_RESET_REQ         = 0x0A,
    COMPLIANCE_LINK_CHECK_REQ               = 0x20,
    COMPLIANCE_DEVICE_TIME_REQ              = 0x21,
    COMPLIANCE_PING_SLOT_INFO_REQ           = 0x22,
#if 0
    COMPLIANCE_BEACON_RX_STATUS_IND_CTRL    = 0x40,
    COMPLIANCE_BEACON_CNT_REQ               = 0x42,
    COMPLIANCE_BEACON_CNT_RESET_REQ         = 0x43,
#endif /* CLASS_B not available */
    COMPLIANCE_TX_CW_REQ                    = 0x7D,
    COMPLIANCE_DUT_FPORT_224_DISABLE_REQ    = 0x7E,
    COMPLIANCE_DUT_VERSION_REQ              = 0x7F,
} ComplianceSrvCmd_t;

/*!
 * Holds the compliance test current context
 */
static ComplianceTestState_t ComplianceTestState =
{
    .Initialized           = false,
    .IsTxPending           = false,
    .TxPendingTimestamp    = 0,
    .IsTxConfirmed         = LORAMAC_HANDLER_UNCONFIRMED_MSG,
    .DataBufferMaxSize     = 0,
    .DataBufferSize        = 0,
    .DataBuffer            = NULL,
    .RxAppCnt              = 0,
    .ClassBStatus          = { 0 },
    .IsBeaconRxStatusIndOn = false,
    .IsResetCmdPending     = false,
    .IsClassReqCmdPending  = false,
    .NewClass              = CLASS_A,
};

/*!
 * LoRaWAN compliance tests protocol handler parameters
 */
static LmhpComplianceParams_t *ComplianceParams;

/*!
 * Reset Beacon status structure
 */
static inline void ClassBStatusReset( void )
{
    memset1( ( uint8_t * ) &ComplianceTestState.ClassBStatus, 0, sizeof( ClassBStatus_t ) / sizeof( uint8_t ) );
}

/*!
 * Initializes the compliance tests with provided parameters
 *
 * \param [in] params Structure containing the initial compliance
 *                    tests parameters.
 * \param [in] dataBuffer        Pointer to main application buffer
 * \param [in] dataBufferMaxSize Application buffer maximum size
 */
static void LmhpComplianceInit( void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize );

/*!
 * Returns the current compliance certification protocol initialization status.
 *
 * \retval status Compliance certification protocol initialization status
 *                [true: Initialized, false: Not initialized]
 */
static bool LmhpComplianceIsInitialized( void );

/*!
 * Returns if a package transmission is pending or not.
 *
 * \retval status Package transmission status
 *                [true: pending, false: Not pending]
 */
static bool LmhpComplianceIsTxPending( void );

/*!
 * Processes the LoRaMac Compliance events.
 */
static void LmhpComplianceProcess( void );

/*!
 * Processes the MCPS Indication
 *
 * \param [in] mcpsIndication     MCPS indication primitive data
 */
static void LmhpComplianceOnMcpsIndication( McpsIndication_t *mcpsIndication );

/*!
 * Processes the MLME Confirm
 *
 * \param [in] mlmeConfirm MLME confirmation primitive data
 */
static void LmhpComplianceOnMlmeConfirm( MlmeConfirm_t *mlmeConfirm );

/*!
 * Processes the MLME Indication
 *
 * \param [in] mlmeIndication     MLME indication primitive data
 */
static void LmhpComplianceOnMlmeIndication( MlmeIndication_t *mlmeIndication );

#if 0
*Helper function to send the BeaconRxStatusInd message
*
* \param [IN] isBeaconRxStatusIndOn Indicates if the beacon status info is sent or not
* /
static void SendBeaconRxStatusInd( bool isBeaconRxStatusIndOn );
#endif /* CLASS_B not available */

static void OnProcessTimer( void *context );

/*!
 * Process timer
 */
static TimerEvent_t ProcessTimer;

LmhPackage_t CompliancePackage =
{
    .Port                    = COMPLIANCE_PORT,
    .Init                    = LmhpComplianceInit,
    .IsInitialized           = LmhpComplianceIsInitialized,
    .IsTxPending             = LmhpComplianceIsTxPending,
    .Process                 = LmhpComplianceProcess,
    .OnPackageProcessEvent   = NULL,  /* To be initialized by LmHandler */
    .OnMcpsConfirmProcess    = NULL,  /* Not used in this package */
    .OnMcpsIndicationProcess = LmhpComplianceOnMcpsIndication,
    .OnMlmeConfirmProcess    = LmhpComplianceOnMlmeConfirm,
    .OnMlmeIndicationProcess = LmhpComplianceOnMlmeIndication,
    .OnJoinRequest           = NULL,  /* To be initialized by LmHandler */
    .OnDeviceTimeRequest     = NULL,  /* To be initialized by LmHandler */
    .OnSysTimeUpdate         = NULL,  /* To be initialized by LmHandler */
    .OnSystemReset           = NULL,  /* To be initialized by LmHandler */
};

LmhPackage_t *LmhpCompliancePackageFactory( void )
{
    return &CompliancePackage;
}

static void LmhpComplianceInit( void *params, uint8_t *dataBuffer, uint8_t dataBufferMaxSize )
{
    if( ( params != NULL ) && ( dataBuffer != NULL ) )
    {
        ComplianceParams                      = ( LmhpComplianceParams_t * ) params;
        ComplianceTestState.DataBuffer        = dataBuffer;
        ComplianceTestState.DataBufferMaxSize = dataBufferMaxSize;
        ComplianceTestState.Initialized       = true;
        TimerInit( &ProcessTimer, OnProcessTimer );
    }
    else
    {
        ComplianceParams                = NULL;
        ComplianceTestState.Initialized = false;
    }
    ComplianceTestState.RxAppCnt = 0;
    ClassBStatusReset( );
    ComplianceTestState.IsTxPending = false;
    ComplianceTestState.IsBeaconRxStatusIndOn = false;
    ComplianceTestState.IsResetCmdPending = false;
    ComplianceTestState.IsClassReqCmdPending = false;
}

static bool LmhpComplianceIsInitialized( void )
{
    return ComplianceTestState.Initialized;
}

static bool LmhpComplianceIsTxPending( void )
{
    return ComplianceTestState.IsTxPending;
}

static void LmhpComplianceProcess( void )
{
    if( ComplianceTestState.IsTxPending == true )
    {
        TimerTime_t now = TimerGetCurrentTime( );
        if( now > ( ComplianceTestState.TxPendingTimestamp + LmHandlerGetDutyCycleWaitTime( ) ) )
        {
            if( ComplianceTestState.DataBufferSize != 0 )
            {
                /* Answer commands */
                LmHandlerAppData_t appData =
                {
                    .Buffer     = ComplianceTestState.DataBuffer,
                    .BufferSize = ComplianceTestState.DataBufferSize,
                    .Port       = COMPLIANCE_PORT,
                };

                LmHandlerErrorStatus_t lmhStatus = LORAMAC_HANDLER_ERROR;
                lmhStatus = LmHandlerSend( &appData, ComplianceTestState.IsTxConfirmed, true );
                if( ( lmhStatus == LORAMAC_HANDLER_SUCCESS ) || ( lmhStatus == LORAMAC_HANDLER_PAYLOAD_LENGTH_RESTRICTED ) )
                {
                    ComplianceTestState.IsTxPending = false;
                    ComplianceTestState.DataBufferSize = 0;
                }
                else
                {
                    /* try to send the message again */
                    TimerSetValue( &ProcessTimer, 1500 );
                    TimerStart( &ProcessTimer );
                }

                ComplianceTestState.TxPendingTimestamp = now;
            }
        }
    }
    else
    {
        /* If no Tx is pending process other commands */
        if( ComplianceTestState.IsClassReqCmdPending == true )
        {
            ComplianceTestState.IsClassReqCmdPending = false;
            LmHandlerRequestClass( ComplianceTestState.NewClass );
        }
    }

    if( ComplianceTestState.IsResetCmdPending == true )
    {
        ComplianceTestState.IsResetCmdPending = false;

        /* Call platform MCU reset API */
        if( CompliancePackage.OnSystemReset != NULL )
        {
            CompliancePackage.OnSystemReset( );
        }
    }
}

static void LmhpComplianceOnMcpsIndication( McpsIndication_t *mcpsIndication )
{
    uint8_t cmdIndex        = 0;
    MibRequestConfirm_t mibReq;

    if( ComplianceTestState.Initialized == false )
    {
        return;
    }

    /* Increment the compliance certification protocol downlink counter */
    /* Not counting downlinks on FPort 0 */
    if( ( mcpsIndication->Port > 0 ) || ( mcpsIndication->AckReceived == true ) )
    {
        ComplianceTestState.RxAppCnt++;
    }

    if( mcpsIndication->RxData == false )
    {
        return;
    }

    if( mcpsIndication->Port != COMPLIANCE_PORT )
    {
        return;
    }

    ComplianceTestState.DataBufferSize = 0;

    switch( mcpsIndication->Buffer[cmdIndex++] )
    {
        case COMPLIANCE_PKG_VERSION_REQ:
            {
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = COMPLIANCE_PKG_VERSION_ANS;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = COMPLIANCE_ID;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = COMPLIANCE_VERSION;
                break;
            }
        case COMPLIANCE_DUT_RESET_REQ:
            {
                ComplianceTestState.IsResetCmdPending = true;
                break;
            }
        case COMPLIANCE_DUT_JOIN_REQ:
            {
                if( CompliancePackage.OnJoinRequest != NULL )
                {
                    CompliancePackage.OnJoinRequest( ACTIVATION_TYPE_OTAA, true );
                }
                break;
            }
        case COMPLIANCE_SWITCH_CLASS_REQ:
            {
                /* CLASS_A = 0, CLASS_B = 1, CLASS_C = 2 */
                ComplianceTestState.NewClass = ( DeviceClass_t ) mcpsIndication->Buffer[cmdIndex++];
                ComplianceTestState.IsClassReqCmdPending = true;
                break;
            }
        case COMPLIANCE_ADR_BIT_CHANGE_REQ:
            {
                MibRequestConfirm_t mibReq;
                mibReq.Type            = MIB_ADR;
                mibReq.Param.AdrEnable = mcpsIndication->Buffer[cmdIndex++];

                LoRaMacMibSetRequestConfirm( &mibReq );
                break;
            }
        case COMPLIANCE_REGIONAL_DUTY_CYCLE_CTRL_REQ:
            {
                LoRaMacTestSetDutyCycleOn( mcpsIndication->Buffer[cmdIndex++] );
                break;
            }
        case COMPLIANCE_TX_PERIODICITY_CHANGE_REQ:
            {
                /* Periodicity in milli-seconds */
                uint32_t periodicity[] = { 0, 5000, 10000, 20000, 30000, 40000, 50000, 60000, 120000, 240000, 480000 };
                uint8_t  index         = mcpsIndication->Buffer[cmdIndex++];

                if( index < ( sizeof( periodicity ) / sizeof( uint32_t ) ) )
                {
                    if( ComplianceParams->OnTxPeriodicityChanged != NULL )
                    {
                        ComplianceParams->OnTxPeriodicityChanged( periodicity[index] );
                    }
                }
                break;
            }
        case COMPLIANCE_TX_FRAMES_CTRL_REQ:
            {
                uint8_t frameType = mcpsIndication->Buffer[cmdIndex++];

                if( ( frameType == 1 ) || ( frameType == 2 ) )
                {
                    ComplianceTestState.IsTxConfirmed = ( frameType != 1 ) ? LORAMAC_HANDLER_CONFIRMED_MSG : LORAMAC_HANDLER_UNCONFIRMED_MSG;

                    if( ComplianceParams->OnTxFrameCtrlChanged != NULL )
                    {
                        ComplianceParams->OnTxFrameCtrlChanged( ComplianceTestState.IsTxConfirmed );
                    }
                }
                break;
            }
        case COMPLIANCE_ECHO_PAYLOAD_REQ:
            {
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = COMPLIANCE_ECHO_PAYLOAD_ANS;

                for( uint8_t i = 1; i < MIN( mcpsIndication->BufferSize, ComplianceTestState.DataBufferMaxSize );
                     i++ )
                {
                    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = mcpsIndication->Buffer[cmdIndex++] + 1;
                }
                break;
            }
        case COMPLIANCE_RX_APP_CNT_REQ:
            {
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = COMPLIANCE_RX_APP_CNT_ANS;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ComplianceTestState.RxAppCnt;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ComplianceTestState.RxAppCnt >> 8;
                break;
            }
        case COMPLIANCE_RX_APP_CNT_RESET_REQ:
            {
                ComplianceTestState.RxAppCnt = 0;
                break;
            }
        case COMPLIANCE_LINK_CHECK_REQ:
            {
                MlmeReq_t mlmeReq;
                mlmeReq.Type = MLME_LINK_CHECK;

                LoRaMacMlmeRequest( &mlmeReq );
                break;
            }
        case COMPLIANCE_DEVICE_TIME_REQ:
            {
                CompliancePackage.OnDeviceTimeRequest( );
                break;
            }
        case COMPLIANCE_PING_SLOT_INFO_REQ:
            {
                ComplianceTestState.ClassBStatus.PingSlotPeriodicity = mcpsIndication->Buffer[cmdIndex++];
                if( ComplianceParams->OnPingSlotPeriodicityChanged != NULL )
                {
                    ComplianceParams->OnPingSlotPeriodicityChanged( ComplianceTestState.ClassBStatus.PingSlotPeriodicity );
                }
                break;
            }
#if 0
        case COMPLIANCE_BEACON_RX_STATUS_IND_CTRL:
            {
                ComplianceTestState.IsBeaconRxStatusIndOn = ( bool ) mcpsIndication->Buffer[cmdIndex++];
                break;
            }
        case COMPLIANCE_BEACON_CNT_REQ:
            {
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = COMPLIANCE_BEACON_CNT_ANS;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ComplianceTestState.ClassBStatus.BeaconCnt;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ComplianceTestState.ClassBStatus.BeaconCnt >> 8;
                break;
            }
        case COMPLIANCE_BEACON_CNT_RESET_REQ:
            {
                ComplianceTestState.ClassBStatus.BeaconCnt = 0;
                break;
            }
#endif /* CLASS_B not available */
        case COMPLIANCE_TX_CW_REQ:
            {
                MlmeReq_t mlmeReq;
                if( mcpsIndication->BufferSize == 7 )
                {
                    mlmeReq.Type = MLME_TXCW;
                    mlmeReq.Req.TxCw.Timeout =
                        ( uint16_t )( mcpsIndication->Buffer[cmdIndex] | ( mcpsIndication->Buffer[cmdIndex + 1] << 8 ) );
                    cmdIndex += 2;
                    mlmeReq.Req.TxCw.Frequency =
                        ( uint32_t )( mcpsIndication->Buffer[cmdIndex] | ( mcpsIndication->Buffer[cmdIndex + 1] << 8 ) |
                                      ( mcpsIndication->Buffer[cmdIndex + 2] << 16 ) ) *
                        100;
                    cmdIndex += 3;
                    mlmeReq.Req.TxCw.Power = mcpsIndication->Buffer[cmdIndex++];

                    LoRaMacMlmeRequest( &mlmeReq );
                }
                break;
            }
        case COMPLIANCE_DUT_FPORT_224_DISABLE_REQ:
            {
                mibReq.Type = MIB_IS_CERT_FPORT_ON;
                mibReq.Param.IsCertPortOn = false;
                LoRaMacMibSetRequestConfirm( &mibReq );

                ComplianceTestState.IsResetCmdPending = true;
                break;
            }
        case COMPLIANCE_DUT_VERSION_REQ:
            {
                Version_t           lrwanVersion;
                Version_t           lrwanRpVersion;
                MibRequestConfirm_t mibReq;

                mibReq.Type = MIB_LORAWAN_VERSION;

                LoRaMacMibGetRequestConfirm( &mibReq );
                lrwanVersion   = mibReq.Param.LrWanVersion.LoRaWan;
                lrwanRpVersion = mibReq.Param.LrWanVersion.LoRaWanRegion;

                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = COMPLIANCE_DUT_VERSION_ANS;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ComplianceParams->FwVersion.Fields.Major;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ComplianceParams->FwVersion.Fields.Minor;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ComplianceParams->FwVersion.Fields.Patch;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ComplianceParams->FwVersion.Fields.Revision;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = lrwanVersion.Fields.Major;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = lrwanVersion.Fields.Minor;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = lrwanVersion.Fields.Patch;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = lrwanVersion.Fields.Revision;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = lrwanRpVersion.Fields.Major;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = lrwanRpVersion.Fields.Minor;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = lrwanRpVersion.Fields.Patch;
                ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = lrwanRpVersion.Fields.Revision;
                break;
            }
        default:
            {
                break;
            }
    }

    if( ComplianceTestState.DataBufferSize != 0 )
    {
        if( ProcessTimer.IsRunning == 0U)
        {
            TimerSetValue( &ProcessTimer, 1000 );
            TimerStart( &ProcessTimer );
        }
    }
    else
    {
        /* Abort any pending Tx as a new command has been processed */
        TimerStop( &ProcessTimer );
        ComplianceTestState.IsTxPending = false;
    }
}

static void LmhpComplianceOnMlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    switch( mlmeConfirm->MlmeRequest )
    {
#if 0
        case MLME_BEACON_ACQUISITION:
            {
                if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
                {
                    ClassBStatusReset( );
                    ComplianceTestState.ClassBStatus.IsBeaconRxOn = true;
                }
                else
                {
                    ComplianceTestState.ClassBStatus.IsBeaconRxOn = false;
                }
                break;
            }
#endif /* CLASS_B not available */
        default:
            break;
    }
}

static void LmhpComplianceOnMlmeIndication( MlmeIndication_t *mlmeIndication )
{
    if( ComplianceTestState.Initialized == false )
    {
        return;
    }

    switch( mlmeIndication->MlmeIndication )
    {
#if 0
        case MLME_BEACON_LOST:
            {
                ClassBStatusReset( );
                SendBeaconRxStatusInd( ComplianceTestState.IsBeaconRxStatusIndOn );
                break;
            }
        case MLME_BEACON:
            {
                if( mlmeIndication->Status == LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED )
                {
                    /* As we received a beacon ensure that IsBeaconRxOn is set to true */
                    if( ComplianceTestState.ClassBStatus.IsBeaconRxOn == false )
                    {
                        ComplianceTestState.ClassBStatus.IsBeaconRxOn = true;
                    }
                    ComplianceTestState.ClassBStatus.BeaconCnt++;
                }
                ComplianceTestState.ClassBStatus.Info = mlmeIndication->BeaconInfo;
                SendBeaconRxStatusInd( ComplianceTestState.IsBeaconRxStatusIndOn );
                break;
            }
#endif /* CLASS_B not available */
        default:
            break;
    }
}

#if 0
static void SendBeaconRxStatusInd( bool isBeaconRxStatusIndOn )
{
    if( isBeaconRxStatusIndOn == false )
    {
        return;
    }
    uint32_t frequency = ComplianceTestState.ClassBStatus.Info.Frequency / 100;

    ComplianceTestState.DataBufferSize = 0;
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = COMPLIANCE_BEACON_RX_STATUS_IND;
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( ComplianceTestState.ClassBStatus.IsBeaconRxOn == true ) ? 1 : 0;
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.BeaconCnt );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.BeaconCnt >> 8 );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( frequency );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( frequency >> 8 );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( frequency >> 16 );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ComplianceTestState.ClassBStatus.Info.Datarate;
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.Rssi );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.Rssi >> 8 );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.Snr );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.Param );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.Time.Seconds );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.Time.Seconds >> 8 );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.Time.Seconds >> 16 );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.Time.Seconds >> 24 );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.GwSpecific.InfoDesc );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.GwSpecific.Info[0] );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.GwSpecific.Info[1] );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.GwSpecific.Info[2] );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.GwSpecific.Info[3] );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.GwSpecific.Info[4] );
    ComplianceTestState.DataBuffer[ComplianceTestState.DataBufferSize++] = ( uint8_t )( ComplianceTestState.ClassBStatus.Info.GwSpecific.Info[5] );

    if( ProcessTimer.IsRunning == 0U)
    {
        TimerSetValue( &ProcessTimer, 1000 );
        TimerStart( &ProcessTimer );
    }
}
#endif /* CLASS_B not available */

static void OnProcessTimer( void *context )
{
    if( ComplianceTestState.DataBufferSize != 0 )
    {
        ComplianceTestState.IsTxPending = true;
    }
    if( CompliancePackage.OnPackageProcessEvent != NULL )
    {
        CompliancePackage.OnPackageProcessEvent();
    }
}
#endif /* LORAMAC_VERSION */
