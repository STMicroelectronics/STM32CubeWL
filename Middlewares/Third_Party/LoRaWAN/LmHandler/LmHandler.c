/*!
 * \file      LmHandler.c
 *
 * \brief     Implements the LoRaMac layer handling.
 *            Provides the possibility to register applicative packages.
 *
 * \remark    Inspired by the examples provided on the en.i-cube_lrwan fork.
 *            MCD Application Team ( STMicroelectronics International )
 *
 * \copyright Revised BSD License, see section \ref LICENSE.txt.
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
  * @file    LmHandler.c
  * @author  MCD Application Team
  * @brief   LoRaMAC Layer handling definition
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "utilities.h"
#include "timer.h"
#include "Commissioning.h"
#include "NvmDataMgmt.h"
#include "radio.h"
#include "Region.h"
#include "LoRaMac.h"
#include "LoRaMacVersion.h"
#include "LoRaMacTest.h"
#include "LmHandler.h"
#include "LmhPackage.h"
#include "LmhpCompliance.h"
#include "secure-element.h"
#include "mw_log_conf.h"  /* needed for MW_LOG */
#include "lorawan_version.h"
#include "lora_info.h"
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
#else /* LORAWAN_KMS == 1 */
#include "kms.h"
#include "kms_platf_objects_interface.h"
#endif /* LORAWAN_KMS */
#include "LmhpPackagesRegistration.h"

/* Private typedef -----------------------------------------------------------*/
/*!
 * MAC notification type
 */
typedef enum PackageNotifyTypes_e
{
    PACKAGE_MCPS_CONFIRM,
    PACKAGE_MCPS_INDICATION,
    PACKAGE_MLME_CONFIRM,
    PACKAGE_MLME_INDICATION,
} PackageNotifyTypes_t;

/* Private define ------------------------------------------------------------*/
/*!
 * Package application data buffer size
 */
#define LORAWAN_APP_DATA_BUFFER_MAX_SIZE            242

/* Private macro -------------------------------------------------------------*/
/*!
 * Hex 8 split buffer
 */
#define HEX8(X)   X[0], X[1], X[2], X[3], X[4], X[5], X[6], X[7]

/*!
 * Hex 16 split buffer
 */
#define HEX16(X)  HEX8(X), X[8], X[9], X[10], X[11], X[12], X[13], X[14], X[15]

/* Private variables ---------------------------------------------------------*/
static CommissioningParams_t CommissioningParams =
{
    .DevEui = { 0 },  /* Automatically filed from secure-element */
    .JoinEui = { 0 }, /* Automatically filed from secure-element */
    .DevAddr = 0,     /* Automatically filed from secure-element */
    .NetworkId = LORAWAN_NETWORK_ID,
};

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
/*!
 * LoRaWAN compliance tests handler parameters
 */
static LmhpComplianceParams_t LmhpComplianceParams =
{
    .AdrEnabled =       LORAMAC_HANDLER_ADR_ON,
    .DutyCycleEnabled = false,
    .StopPeripherals =  NULL,
    .StartPeripherals = NULL,
};
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
/*!
 * LoRaWAN compliance tests handler parameters
 */
static LmhpComplianceParams_t LmhpComplianceParams =
{
    .FwVersion.Value = 0U,                 /* To be initialized by LmHandler */
    .OnTxPeriodicityChanged = NULL,        /* To be initialized by LmHandler */
    .OnTxFrameCtrlChanged = NULL,          /* To be initialized by LmHandler */
    .OnPingSlotPeriodicityChanged = NULL,  /* To be initialized by LmHandler */
};
#endif /* LORAMAC_VERSION */

static LmhPackage_t *LmHandlerPackages[PKG_MAX_NUMBER];

/*!
 * Upper layer LoRaMac parameters
 */
static LmHandlerParams_t LmHandlerParams;

/*!
 * Upper layer callbacks
 */
static LmHandlerCallbacks_t *LmHandlerCallbacks;

/*!
 * Used to notify LmHandler of LoRaMac events
 */
static LoRaMacPrimitives_t LoRaMacPrimitives;

/*!
 * LoRaMac callbacks
 */
static LoRaMacCallback_t LoRaMacCallbacks;

static LmHandlerJoinParams_t JoinParams =
{
    .CommissioningParams = &CommissioningParams,
    .Mode = ACTIVATION_TYPE_NONE,
    .forceRejoin = false,
    .Datarate = DR_0,
    .TxPower = TX_POWER_0,
    .Status = LORAMAC_HANDLER_ERROR
};

static LmHandlerTxParams_t TxParams =
{
    .CommissioningParams = &CommissioningParams,
    .MsgType = LORAMAC_HANDLER_UNCONFIRMED_MSG,
    .AckReceived = 0,
    .Datarate = DR_0,
    .UplinkCounter = 0,
    .AppData =
    {
        .Port = 0,
        .BufferSize = 0,
        .Buffer = NULL,
    },
    .TxPower = TX_POWER_0,
    .Channel = 0,
};

static LmHandlerRxParams_t RxParams =
{
    .CommissioningParams = &CommissioningParams,
    .Rssi = 0,
    .Snr = 0,
    .DownlinkCounter = 0,
    .RxSlot = -1,
    .LinkCheck = false,
    .DemodMargin = 0,
    .NbGateways = 0,
};

#if ( LORAMAC_CLASSB_ENABLED == 1 )
static LmHandlerBeaconParams_t BeaconParams =
{
    .State = LORAMAC_HANDLER_BEACON_ACQUIRING,
    .Info =
    {
        .Time = { .Seconds = 0, .SubSeconds = 0 },
        .Frequency = 0,
        .Datarate = 0,
        .Rssi = 0,
        .Snr = 0,
        .GwSpecific =
        {
            .InfoDesc = 0,
            .Info = { 0 },
        },
    },
};
#endif /* LORAMAC_CLASSB_ENABLED == 1 */

#if ( LORAMAC_CLASSB_ENABLED == 1 )
/*!
 * Indicates if a switch to Class B operation is pending or not.
 */
static bool IsClassBSwitchPending = false;
#endif /* LORAMAC_CLASSB_ENABLED == 1 */

/*!
 * Stores the time to wait before next transmission
 *
 */
static TimerTime_t DutyCycleWaitTime = 0;

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
/*!
 * Indicates if an uplink is pending upon MAC layer request
 *
 */
static bool IsUplinkTxPending = false;
#endif /* LORAMAC_VERSION */

/*!
 * Package Application buffer
 */
static uint8_t AppDataBuffer[LORAWAN_APP_DATA_BUFFER_MAX_SIZE];

/*!
 * Package application data structure
 */
static LmHandlerAppData_t AppData = { 0, LORAWAN_APP_DATA_BUFFER_MAX_SIZE, AppDataBuffer };

static bool CtxRestoreDone = false;

/* Private function prototypes -----------------------------------------------*/
/*!
 * \brief   MCPS-Confirm event function
 *
 * \param   [in] mcpsConfirm - Pointer to the confirm structure,
 *                             containing confirm attributes.
 */
static void McpsConfirm( McpsConfirm_t *mcpsConfirm );

/*!
 * \brief   MCPS-Indication event function
 *
 * \param   [in] mcpsIndication - Pointer to the indication structure,
 *               containing indication attributes.
 * \param   [in] rxStatus - Pointer to RX status structure
 */
static void McpsIndication( McpsIndication_t *mcpsIndication, LoRaMacRxStatus_t *rxStatus );

/*!
 * \brief   MLME-Confirm event function
 *
 * \param   [in] mlmeConfirm - Pointer to the confirm structure,
 *               containing confirm attributes.
 */
static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm );

/*!
 * \brief   MLME-Indication event function
 *
 * \param   [in] mlmeIndication - Pointer to the indication structure,
 *               containing indication attributes.
 * \param   [in] rxStatus - Pointer to RX status structure
 */
static void MlmeIndication( MlmeIndication_t *mlmeIndication, LoRaMacRxStatus_t *rxStatus );

#if ( LORAMAC_CLASSB_ENABLED == 1 )
/*!
 * \brief   Starts the beacon search
 *
 * \retval  status Returns \ref LORAMAC_HANDLER_SUCCESS if request has been
 *                 processed else \ref LORAMAC_HANDLER_ERROR
 */
static LmHandlerErrorStatus_t LmHandlerBeaconReq( void );

#endif /* LORAMAC_CLASSB_ENABLED == 1 */

/*!
 * \brief   Notifies the package to process the LoRaMac callbacks.
 *
 * \param   [in] notifyType MAC notification type [PACKAGE_MCPS_CONFIRM,
 *                                                 PACKAGE_MCPS_INDICATION,
 *                                                 PACKAGE_MLME_CONFIRM,
 *                                                 PACKAGE_MLME_INDICATION]
 * \param   [in] params     Notification parameters. The params type can be
 *                          [McpsConfirm_t, McpsIndication_t, MlmeConfirm_t, MlmeIndication_t]
 */
static void LmHandlerPackagesNotify( PackageNotifyTypes_t notifyType, void *params );

/*!
 * \brief   Returns if at least one package transmission is pending or not
 *
 * \retval  status Package transmission status
 *                 [true: pending, false: Not pending]
 */
static bool LmHandlerPackageIsTxPending( void );

/*!
 * \brief   Processes the internal package events.
 */
static void LmHandlerPackagesProcess( void );

/*!
 * \brief   Check if the package ID is initialized
 *
 * \param   [in] id package identifier
 *
 * \retval  status Returns true if initialized else false
 */
static bool LmHandlerPackageIsInitialized( uint8_t id );

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
/*!
 * \brief   Will be called to change applicative Tx frame control
 *
 * \param   [in] isTxConfirmed set the Tx frame in confirmed/unconfirmed control
 * \note    Compliance test protocol callbacks used when TS001-1.0.4 + TS009 1.0.0 are defined
 */
static void LmHandlerOnTxFrameCtrlChanged( LmHandlerMsgTypes_t isTxConfirmed );

/*!
 * \brief   Will be called to change the ping period
 *
 * \param   [in] pingSlotPeriodicity index of ping period as 2^X value
 * \note    Compliance test protocol callbacks used when TS001-1.0.4 + TS009 1.0.0 are defined
 */
static void LmHandlerOnPingSlotPeriodicityChanged( uint8_t pingSlotPeriodicity );
#endif /* LORAMAC_VERSION */

/* Exported functions ---------------------------------------------------------*/
LmHandlerErrorStatus_t LmHandlerInit( LmHandlerCallbacks_t *handlerCallbacks, uint32_t fwVersion )
{
    LmHandlerCallbacks = handlerCallbacks;

    LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
    LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
    LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
    LoRaMacPrimitives.MacMlmeIndication = MlmeIndication;
    LoRaMacCallbacks.GetBatteryLevel = LmHandlerCallbacks->GetBatteryLevel;
    LoRaMacCallbacks.GetTemperatureLevel = LmHandlerCallbacks->GetTemperature;
    LoRaMacCallbacks.GetUniqueId = LmHandlerCallbacks->GetUniqueId;
    LoRaMacCallbacks.GetDevAddress = LmHandlerCallbacks->GetDevAddr;
    LoRaMacCallbacks.NvmDataChange  = NvmDataMgmtEvent;
    LoRaMacCallbacks.MacProcessNotify = LmHandlerCallbacks->OnMacProcess;

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    LmhpComplianceParams.FwVersion.Value = fwVersion;
    LmhpComplianceParams.OnTxPeriodicityChanged = LmHandlerCallbacks->OnTxPeriodicityChanged;
    LmhpComplianceParams.OnTxFrameCtrlChanged = LmHandlerOnTxFrameCtrlChanged;
    LmhpComplianceParams.OnPingSlotPeriodicityChanged = LmHandlerOnPingSlotPeriodicityChanged;
#endif /* LORAMAC_VERSION */

    /*The LoRa-Alliance Compliance protocol package should always be initialized and activated.*/
    if( LmHandlerPackageRegister( PACKAGE_ID_COMPLIANCE, &LmhpComplianceParams ) != LORAMAC_HANDLER_SUCCESS )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    if( LmhpPackagesRegistrationInit( ( Version_t * )&fwVersion ) != LORAMAC_HANDLER_SUCCESS )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerDeInit( void )
{
    if( LoRaMacDeInitialization() == LORAMAC_STATUS_OK )
    {
        LmHandlerCallbacks = NULL;
        memset1( ( uint8_t * )&LoRaMacPrimitives, 0, sizeof( LoRaMacPrimitives_t ) );
        memset1( ( uint8_t * )&LoRaMacCallbacks, 0, sizeof( LoRaMacCallback_t ) );
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_BUSY_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerConfigure( LmHandlerParams_t *handlerParams )
{
    MibRequestConfirm_t mibReq;
    LoraInfo_t *loraInfo;

    memcpy1( ( void * )&LmHandlerParams, ( const void * )handlerParams, sizeof( LmHandlerParams_t ) );

#if ( LORAMAC_CLASSB_ENABLED == 1 )
    IsClassBSwitchPending = false;
#endif /* LORAMAC_CLASSB_ENABLED == 1 */
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    IsUplinkTxPending = false;
#endif /* LORAMAC_VERSION */

    loraInfo = LoraInfo_GetPtr();

    if( 0U == ( ( 1 << ( LmHandlerParams.ActiveRegion ) ) & ( loraInfo->Region ) ) )
    {
        MW_LOG( TS_ON, VLEVEL_ALWAYS, "error: Region is not defined in the MW: set lorawan_conf.h accordingly\r\n" );
        while( 1 ) {} /* error: Region is not defined in the MW */
    }

    if( LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LmHandlerParams.ActiveRegion ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

#if (defined( CONTEXT_MANAGEMENT_ENABLED ) && ( CONTEXT_MANAGEMENT_ENABLED == 1 ))
    /* Try the restore context from the Backup RAM structure if data retention is available */
    mibReq.Type = MIB_NVM_CTXS;
    if( LoRaMacMibSetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
    {
        CtxRestoreDone = true;
    }
    else
    {
        /* Restore context data backup from user callback (stored in FLASH) */
        mibReq.Type = MIB_NVM_BKP_CTXS;
        if( LmHandlerCallbacks->OnRestoreContextRequest != NULL )
        {
            LoRaMacMibGetRequestConfirm( &mibReq );
            LmHandlerCallbacks->OnRestoreContextRequest( mibReq.Param.BackupContexts, sizeof( LoRaMacNvmData_t ) );
        }
        /* Restore context data from backup to main nvm structure */
        mibReq.Type = MIB_NVM_CTXS;
        if( LoRaMacMibSetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
        {
            mibReq.Type = MIB_NETWORK_ACTIVATION;
            LoRaMacMibGetRequestConfirm( &mibReq );
            if( mibReq.Param.NetworkActivation != ACTIVATION_TYPE_NONE )
            {
                CtxRestoreDone = true;
            }
        }
    }

    if( CtxRestoreDone == true )
    {
        if( LmHandlerCallbacks->OnNvmDataChange != NULL )
        {
            LmHandlerCallbacks->OnNvmDataChange( LORAMAC_HANDLER_NVM_RESTORE );
        }

        mibReq.Type = MIB_NVM_CTXS;
        LoRaMacMibGetRequestConfirm( &mibReq );
        LoRaMacNvmData_t *current_nvm = mibReq.Param.Contexts;

        LmHandlerParams.ActiveRegion = current_nvm->MacGroup2.Region;
        LmHandlerParams.DefaultClass = current_nvm->MacGroup2.DeviceClass;
        LmHandlerParams.AdrEnable = current_nvm->MacGroup2.AdrCtrlOn;
    }
    else
#endif /* CONTEXT_MANAGEMENT_ENABLED == 1 */
    {
        mibReq.Type = MIB_NET_ID;
        mibReq.Param.NetID = LORAWAN_NETWORK_ID;
        LoRaMacMibSetRequestConfirm( &mibReq );
    }

    /* Restore ID struct from NVM or Init from callbacks */
    if( SecureElementInitMcuID( LoRaMacCallbacks.GetUniqueId, LoRaMacCallbacks.GetDevAddress ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    /* Read secure-element DEV_EUI, JOIN_EUI and DEV_ADDR values. */
    mibReq.Type = MIB_DEV_ADDR;
    LoRaMacMibGetRequestConfirm( &mibReq );
    CommissioningParams.DevAddr = mibReq.Param.DevAddr;

    /* Override DevAddress value after init from callbacks */
    LoRaMacMibSetRequestConfirm( &mibReq );

    mibReq.Type = MIB_DEV_EUI;
    mibReq.Param.DevEui = CommissioningParams.DevEui;
    LoRaMacMibGetRequestConfirm( &mibReq );

    mibReq.Type = MIB_JOIN_EUI;
    mibReq.Param.JoinEui = CommissioningParams.JoinEui;
    LoRaMacMibGetRequestConfirm( &mibReq );

    SecureElementPrintKeys();

#if (defined (LORAWAN_KMS) && (LORAWAN_KMS == 1))
    MW_LOG( TS_OFF, VLEVEL_L, "###### KMS ENABLED \r\n" );
#endif /* LORAWAN_KMS == 1 */

    mibReq.Type = MIB_PUBLIC_NETWORK;
    mibReq.Param.EnablePublicNetwork = LORAWAN_PUBLIC_NETWORK;
    LoRaMacMibSetRequestConfirm( &mibReq );

    mibReq.Type = MIB_REPEATER_SUPPORT;
    mibReq.Param.EnableRepeaterSupport = LORAWAN_REPEATER_SUPPORT;
    LoRaMacMibSetRequestConfirm( &mibReq );

    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = LmHandlerParams.AdrEnable;
    LoRaMacMibSetRequestConfirm( &mibReq );

    mibReq.Type = MIB_RXB_C_TIMEOUT;
    mibReq.Param.RxBCTimeout = LmHandlerParams.RxBCTimeout;
    LoRaMacMibSetRequestConfirm( &mibReq );

    GetPhyParams_t getPhy;
    PhyParam_t phyParam;
    getPhy.Attribute = PHY_DUTY_CYCLE;
    phyParam = RegionGetPhyParam( LmHandlerParams.ActiveRegion, &getPhy );
    LmHandlerParams.DutyCycleEnabled = ( bool ) phyParam.Value;

    /* Set system maximum tolerated rx error in milliseconds */
    LmHandlerSetSystemMaxRxError( 20 );

    /* override previous value if reconfigure new region */
    LoRaMacTestSetDutyCycleOn( LmHandlerParams.DutyCycleEnabled );

    return LORAMAC_HANDLER_SUCCESS;
}

bool LmHandlerIsBusy( void )
{
    if( LoRaMacIsBusy( ) == true )
    {
        return true;
    }
    if( LmHandlerJoinStatus( ) != LORAMAC_HANDLER_SET )
    {
        /* The network isn't yet joined, try again later. */
        LmHandlerJoin( JoinParams.Mode, JoinParams.forceRejoin );
        return true;
    }

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    if( LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->IsRunning( ) == true )
    {
        return true;
    }
#endif /* LORAMAC_VERSION */

    if( LmHandlerPackageIsTxPending( ) == true )
    {
        return true;
    }

    return false;
}

void LmHandlerProcess( void )
{
    /* Call at first the LoRaMAC process before to run all package process features */
    /* Processes the LoRaMac events */
    LoRaMacProcess( );

    /* Call all packages process functions */
    LmHandlerPackagesProcess( );

    /* Check if a package transmission is pending. */
    /* If it is the case exit function earlier */
    if( LmHandlerPackageIsTxPending( ) == true )
    {
        return;
    }

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    /* If a MAC layer scheduled uplink is still pending try to send it. */
    if( IsUplinkTxPending == true )
    {
        /* Send an empty message */
        LmHandlerAppData_t appData =
        {
            .Buffer = NULL,
            .BufferSize = 0,
            .Port = 0,
        };

        if( LmHandlerSend( &appData, LmHandlerParams.IsTxConfirmed, false ) == LORAMAC_HANDLER_SUCCESS )
        {
            IsUplinkTxPending = false;
        }
    }
#endif /* LORAMAC_VERSION */
}

TimerTime_t LmHandlerGetDutyCycleWaitTime( void )
{
    return DutyCycleWaitTime;
}

void LmHandlerJoin( ActivationType_t mode, bool forceRejoin )
{
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_JOIN;
    mlmeReq.Req.Join.Datarate = LmHandlerParams.TxDatarate;
    mlmeReq.Req.Join.TxPower = LmHandlerParams.TxPower;

    if( mode == ACTIVATION_TYPE_OTAA )
    {
        mlmeReq.Req.Join.NetworkActivation = ACTIVATION_TYPE_OTAA;
        JoinParams.Mode = ACTIVATION_TYPE_OTAA;
        JoinParams.forceRejoin = forceRejoin;
        LoRaMacStart();
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
        /* Starts the OTAA join procedure */
        LoRaMacMlmeRequest( &mlmeReq );
#endif /* LORAMAC_VERSION */
    }
    else
    {
        MibRequestConfirm_t mibReq;

        mlmeReq.Req.Join.NetworkActivation = ACTIVATION_TYPE_ABP;
        JoinParams.Mode = ACTIVATION_TYPE_ABP;
        JoinParams.Datarate = LmHandlerParams.TxDatarate;
        JoinParams.TxPower = LmHandlerParams.TxPower;
        JoinParams.Status = LORAMAC_HANDLER_SUCCESS;
        JoinParams.forceRejoin = forceRejoin;

        if( CtxRestoreDone == false )
        {
            /* Configure the default datarate */
            mibReq.Type = MIB_CHANNELS_DEFAULT_DATARATE;
            mibReq.Param.ChannelsDefaultDatarate = LmHandlerParams.TxDatarate;
            LoRaMacMibSetRequestConfirm( &mibReq );

            mibReq.Type = MIB_CHANNELS_DATARATE;
            mibReq.Param.ChannelsDatarate = LmHandlerParams.TxDatarate;
            LoRaMacMibSetRequestConfirm( &mibReq );

            /* Configure the default Tx Power */
            mibReq.Type = MIB_CHANNELS_DEFAULT_TX_POWER;
            mibReq.Param.ChannelsDefaultTxPower = LmHandlerParams.TxPower;
            LoRaMacMibSetRequestConfirm( &mibReq );

            mibReq.Type = MIB_CHANNELS_TX_POWER;
            mibReq.Param.ChannelsTxPower = LmHandlerParams.TxPower;
            LoRaMacMibSetRequestConfirm( &mibReq );

            /* Tell the MAC layer which network server version are we connecting too. */
            mibReq.Type = MIB_ABP_LORAWAN_VERSION;
            mibReq.Param.AbpLrWanVersion.Value = ABP_ACTIVATION_LRWAN_VERSION;
            LoRaMacMibSetRequestConfirm( &mibReq );
        }

        LoRaMacStart();
        mibReq.Type = MIB_NETWORK_ACTIVATION;
        mibReq.Param.NetworkActivation = ACTIVATION_TYPE_ABP;
        LoRaMacMibSetRequestConfirm( &mibReq );
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
        /* Notify upper layer */
        if( LmHandlerCallbacks->OnJoinRequest != NULL )
        {
            LmHandlerCallbacks->OnJoinRequest( &JoinParams );
        }
        LmHandlerRequestClass( LmHandlerParams.DefaultClass );
#endif /* LORAMAC_VERSION */
    }

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    if( ( CtxRestoreDone == false ) || ( forceRejoin == true ) )
    {
        /* Starts the join procedure */
        LoRaMacMlmeRequest( &mlmeReq );
    }
    DutyCycleWaitTime = mlmeReq.ReqReturn.DutyCycleWaitTime;
#endif /* LORAMAC_VERSION */
}

LmHandlerFlagStatus_t LmHandlerJoinStatus( void )
{
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;

    mibReq.Type = MIB_NETWORK_ACTIVATION;
    status = LoRaMacMibGetRequestConfirm( &mibReq );

    if( status == LORAMAC_STATUS_OK )
    {
        if( mibReq.Param.NetworkActivation == ACTIVATION_TYPE_NONE )
        {
            return LORAMAC_HANDLER_RESET;
        }
        else
        {
            return LORAMAC_HANDLER_SET;
        }
    }
    else
    {
        return LORAMAC_HANDLER_RESET;
    }
}

LmHandlerErrorStatus_t LmHandlerSend( LmHandlerAppData_t *appData, LmHandlerMsgTypes_t isTxConfirmed,
                                      bool allowDelayedTx )
{
    LoRaMacStatus_t status;
    LmHandlerErrorStatus_t lmhStatus = LORAMAC_HANDLER_ERROR;
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;

    if( LoRaMacIsBusy() == true )
    {
        return LORAMAC_HANDLER_BUSY_ERROR;
    }

    if( LoRaMacIsStopped() == true )
    {
        return LORAMAC_HANDLER_NO_NETWORK_JOINED;
    }

    if( LmHandlerJoinStatus( ) != LORAMAC_HANDLER_SET )
    {
        /* The network isn't joined, try again. */
        LmHandlerJoin( JoinParams.Mode, JoinParams.forceRejoin );
        return LORAMAC_HANDLER_NO_NETWORK_JOINED;
    }

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    if( ( LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->IsRunning( ) == true )
        && ( appData->Port != LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->Port ) && ( appData->Port != 0 ) )
    {
        return LORAMAC_HANDLER_COMPLIANCE_RUNNING;
    }
#endif /* LORAMAC_VERSION */

    TxParams.MsgType = isTxConfirmed;
    mcpsReq.Type = ( isTxConfirmed == LORAMAC_HANDLER_UNCONFIRMED_MSG ) ? MCPS_UNCONFIRMED : MCPS_CONFIRMED;
    mcpsReq.Req.Unconfirmed.Datarate = LmHandlerParams.TxDatarate;
    if( LoRaMacQueryTxPossible( appData->BufferSize, &txInfo ) != LORAMAC_STATUS_OK )
    {
        /* Send empty frame in order to flush MAC commands */
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
        lmhStatus = LORAMAC_HANDLER_PAYLOAD_LENGTH_RESTRICTED;
    }
    else
    {
        mcpsReq.Req.Unconfirmed.fPort = appData->Port;
        mcpsReq.Req.Unconfirmed.fBufferSize = appData->BufferSize;
        mcpsReq.Req.Unconfirmed.fBuffer = appData->Buffer;
    }

    TxParams.AppData = *appData;
    TxParams.Datarate = LmHandlerParams.TxDatarate;

    status = LoRaMacMcpsRequest( &mcpsReq, allowDelayedTx );
    DutyCycleWaitTime = mcpsReq.ReqReturn.DutyCycleWaitTime;

    switch( status )
    {
        case LORAMAC_STATUS_OK:
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
            IsUplinkTxPending = false;
#endif /* LORAMAC_VERSION */
            if( lmhStatus != LORAMAC_HANDLER_PAYLOAD_LENGTH_RESTRICTED )
            {
                lmhStatus = LORAMAC_HANDLER_SUCCESS;
            }
            break;
        case LORAMAC_STATUS_BUSY:
        case LORAMAC_STATUS_BUSY_UPLINK_COLLISION:
        case LORAMAC_STATUS_BUSY_BEACON_RESERVED_TIME:
        case LORAMAC_STATUS_BUSY_PING_SLOT_WINDOW_TIME:
            lmhStatus = LORAMAC_HANDLER_BUSY_ERROR;
            break;
        case LORAMAC_STATUS_NO_NETWORK_JOINED:
            lmhStatus = LORAMAC_HANDLER_NO_NETWORK_JOINED;
            break;
        case LORAMAC_STATUS_CRYPTO_ERROR:
            lmhStatus = LORAMAC_HANDLER_CRYPTO_ERROR;
            break;
        case LORAMAC_STATUS_DUTYCYCLE_RESTRICTED:
            lmhStatus = LORAMAC_HANDLER_DUTYCYCLE_RESTRICTED;
            break;
        case LORAMAC_STATUS_SERVICE_UNKNOWN:
        case LORAMAC_STATUS_PARAMETER_INVALID:
        case LORAMAC_STATUS_MAC_COMMAD_ERROR:
        case LORAMAC_STATUS_FCNT_HANDLER_ERROR:
        case LORAMAC_STATUS_REGION_NOT_SUPPORTED:
        case LORAMAC_STATUS_NO_FREE_CHANNEL_FOUND:
        case LORAMAC_STATUS_NO_CHANNEL_FOUND:
        case LORAMAC_STATUS_LENGTH_ERROR:
        default:
            lmhStatus = LORAMAC_HANDLER_ERROR;
            break;
    }

    return lmhStatus;
}

LmHandlerErrorStatus_t LmHandlerDeviceTimeReq( void )
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_DEVICE_TIME;

    status = LoRaMacMlmeRequest( &mlmeReq );
    DutyCycleWaitTime = mlmeReq.ReqReturn.DutyCycleWaitTime;

    if( status == LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

#if ( LORAMAC_CLASSB_ENABLED == 1 )
static LmHandlerErrorStatus_t LmHandlerBeaconReq( void )
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_BEACON_ACQUISITION;

    status = LoRaMacMlmeRequest( &mlmeReq );
    DutyCycleWaitTime = mlmeReq.ReqReturn.DutyCycleWaitTime;

    if( status == LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}
#endif /* LORAMAC_CLASSB_ENABLED == 1 */

LmHandlerErrorStatus_t LmHandlerPingSlotReq( uint8_t periodicity )
{
#if ( LORAMAC_CLASSB_ENABLED == 1 )
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_PING_SLOT_INFO;
    mlmeReq.Req.PingSlotInfo.PingSlot.Fields.Periodicity = periodicity;
    mlmeReq.Req.PingSlotInfo.PingSlot.Fields.RFU = 0;

    status = LoRaMacMlmeRequest( &mlmeReq );
    DutyCycleWaitTime = mlmeReq.ReqReturn.DutyCycleWaitTime;

    if( status == LORAMAC_STATUS_OK )
    {
        LmHandlerParams.PingSlotPeriodicity = periodicity;
        /* Send an empty message */
        LmHandlerAppData_t appData =
        {
            .Buffer = NULL,
            .BufferSize = 0,
            .Port = 0,
        };
        return LmHandlerSend( &appData, LmHandlerParams.IsTxConfirmed, false );
    }
    else
#endif /* LORAMAC_CLASSB_ENABLED == 1 */
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerRequestClass( DeviceClass_t newClass )
{
    MibRequestConfirm_t mibReq;
    DeviceClass_t currentClass;
    LmHandlerErrorStatus_t errorStatus = LORAMAC_HANDLER_SUCCESS;

    if( LoRaMacIsBusy() == true )
    {
        return LORAMAC_HANDLER_BUSY_ERROR;
    }

    if( LmHandlerJoinStatus() != LORAMAC_HANDLER_SET )
    {
        return LORAMAC_HANDLER_NO_NETWORK_JOINED;
    }

    mibReq.Type = MIB_DEVICE_CLASS;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }
    currentClass = mibReq.Param.Class;

    /* Attempt to switch only if class update */
    if( currentClass != newClass )
    {
        switch( newClass )
        {
            case CLASS_A:
                {
                    if( currentClass != CLASS_A )
                    {
                        mibReq.Param.Class = newClass;
                        if( LoRaMacMibSetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
                        {
                            /* Switch is instantaneous */
                            if( LmHandlerCallbacks->OnClassChange != NULL )
                            {
                                LmHandlerCallbacks->OnClassChange( newClass );
                            }
                        }
                        else
                        {
                            errorStatus = LORAMAC_HANDLER_ERROR;
                        }
                    }
                }
                break;
            case CLASS_B:
                {
#if ( LORAMAC_CLASSB_ENABLED == 1 )
                    if( currentClass != CLASS_A )
                    {
                        errorStatus = LORAMAC_HANDLER_ERROR;
                    }
                    else
                    {
                        /* Beacon must first be acquired */
                        errorStatus = LmHandlerDeviceTimeReq( );
                        IsClassBSwitchPending = true;
                    }
#else /* LORAMAC_CLASSB_ENABLED == 0 */
                    errorStatus = LORAMAC_HANDLER_ERROR;
#endif /* LORAMAC_CLASSB_ENABLED */
                }
                break;
            case CLASS_C:
                {
                    if( currentClass != CLASS_A )
                    {
                        errorStatus = LORAMAC_HANDLER_ERROR;
                    }
                    else
                    {
                        /* Switch is instantaneous */
                        mibReq.Param.Class = newClass;
                        if( LoRaMacMibSetRequestConfirm( &mibReq ) == LORAMAC_STATUS_OK )
                        {
                            if( LmHandlerCallbacks->OnClassChange != NULL )
                            {
                                LmHandlerCallbacks->OnClassChange( newClass );
                            }
                        }
                        else
                        {
                            errorStatus = LORAMAC_HANDLER_ERROR;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    return errorStatus;
}

LmHandlerErrorStatus_t LmHandlerGetCurrentClass( DeviceClass_t *deviceClass )
{
    MibRequestConfirm_t mibReq;
    if( deviceClass == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_DEVICE_CLASS;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *deviceClass = mibReq.Param.Class;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetTxDatarate( int8_t *txDatarate )
{
    MibRequestConfirm_t mibGet;
    if( txDatarate == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibGet.Type = MIB_CHANNELS_DATARATE;
    if( LoRaMacMibGetRequestConfirm( &mibGet ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *txDatarate = mibGet.Param.ChannelsDatarate;
    LmHandlerParams.TxDatarate = *txDatarate;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetActiveRegion( LoRaMacRegion_t *region )
{
    if( region == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *region = LmHandlerParams.ActiveRegion;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetSystemMaxRxError( uint32_t maxErrorInMs )
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_SYSTEM_MAX_RX_ERROR;
    mibReq.Param.SystemMaxRxError = maxErrorInMs;
    if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }
    return LORAMAC_HANDLER_SUCCESS;
}

/*
 *=============================================================================
 * LORAMAC NOTIFICATIONS HANDLING
 *=============================================================================
 */

static void McpsConfirm( McpsConfirm_t *mcpsConfirm )
{
    TxParams.IsMcpsConfirm = 1;
    TxParams.Status = mcpsConfirm->Status;
    TxParams.Datarate = mcpsConfirm->Datarate;
    TxParams.UplinkCounter = mcpsConfirm->UpLinkCounter;
    TxParams.TxPower = mcpsConfirm->TxPower;
    TxParams.Channel = mcpsConfirm->Channel;
    TxParams.AckReceived = mcpsConfirm->AckReceived;

    if( LmHandlerCallbacks->OnTxData != NULL )
    {
        LmHandlerCallbacks->OnTxData( &TxParams );
    }

    LmHandlerPackagesNotify( PACKAGE_MCPS_CONFIRM, mcpsConfirm );
}

static void McpsIndication( McpsIndication_t *mcpsIndication, LoRaMacRxStatus_t *rxStatus )
{
    LmHandlerAppData_t appData;
    DeviceClass_t deviceClass = CLASS_A;
    RxParams.IsMcpsIndication = 1;
    RxParams.Status = mcpsIndication->Status;

    if( RxParams.Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
        return;
    }

    RxParams.Datarate = mcpsIndication->RxDatarate;
    RxParams.Rssi = rxStatus->Rssi;
    RxParams.Snr = rxStatus->Snr;
    RxParams.RxSlot = rxStatus->RxSlot;
    RxParams.DownlinkCounter = mcpsIndication->DownLinkCounter;

    appData.Port = mcpsIndication->Port;
    appData.BufferSize = mcpsIndication->BufferSize;
    appData.Buffer = mcpsIndication->Buffer;

    if( LmHandlerCallbacks->OnRxData != NULL )
    {
        LmHandlerCallbacks->OnRxData( &appData, &RxParams );
    }

    if( ( LmHandlerCallbacks->OnSysTimeUpdate != NULL ) && ( mcpsIndication->DeviceTimeAnsReceived == true ) )
    {
        LmHandlerCallbacks->OnSysTimeUpdate( );
    }
    /* Call packages RxProcess function */
    LmHandlerPackagesNotify( PACKAGE_MCPS_INDICATION, mcpsIndication );

    LmHandlerGetCurrentClass( &deviceClass );
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    if( mcpsIndication->IsUplinkTxPending != 0 )
    {
        /* The server signals that it has pending data to be sent. */
        /* We schedule an uplink as soon as possible to flush the server. */

        /* Send an empty message */
        LmHandlerAppData_t appData =
        {
            .Buffer = NULL,
            .BufferSize = 0,
            .Port = 0
        };
        LmHandlerSend( &appData, LORAMAC_HANDLER_UNCONFIRMED_MSG, true );
    }
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    if( mcpsIndication->IsUplinkTxPending != 0 )
    {
        /* The server signals that it has pending data to be sent. */
        /* We schedule an uplink as soon as possible to flush the server. */
        IsUplinkTxPending = true;
    }
#endif /* LORAMAC_VERSION */
}

static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    TxParams.IsMcpsConfirm = 0;
    TxParams.Status = mlmeConfirm->Status;
    if( LmHandlerCallbacks->OnTxData != NULL )
    {
        LmHandlerCallbacks->OnTxData( &TxParams );
    }

    LmHandlerPackagesNotify( PACKAGE_MLME_CONFIRM, mlmeConfirm );

    switch( mlmeConfirm->MlmeRequest )
    {
        case MLME_JOIN:
            {
                MibRequestConfirm_t mibReq;
                mibReq.Type = MIB_DEV_ADDR;
                LoRaMacMibGetRequestConfirm( &mibReq );
                if( SecureElementSetDevAddr( JoinParams.Mode, mibReq.Param.DevAddr ) == SECURE_ELEMENT_SUCCESS )
                {
                    CommissioningParams.DevAddr = mibReq.Param.DevAddr;
                }
                LmHandlerGetTxDatarate( &JoinParams.Datarate );
                LmHandlerGetTxPower( &JoinParams.TxPower );

                if( TxParams.Status == LORAMAC_EVENT_INFO_STATUS_OK )
                {
                    /* Status is OK, node has joined the network */
                    JoinParams.Status = LORAMAC_HANDLER_SUCCESS;
                    LmHandlerRequestClass( LmHandlerParams.DefaultClass );
                }
                else
                {
                    /* Join was not successful. Try to join again */
                    JoinParams.Status = LORAMAC_HANDLER_ERROR;
                }
                /* Notify upper layer */
                if( LmHandlerCallbacks->OnJoinRequest != NULL )
                {
                    LmHandlerCallbacks->OnJoinRequest( &JoinParams );
                }
                if( TxParams.Status == LORAMAC_EVENT_INFO_STATUS_OK )
                {
                    SecureElementPrintSessionKeys( JoinParams.Mode );
                }
            }
            break;
        case MLME_LINK_CHECK:
            {
                RxParams.LinkCheck = true;
                RxParams.DemodMargin = mlmeConfirm->DemodMargin;
                RxParams.NbGateways = mlmeConfirm->NbGateways;
            }
            break;
        case MLME_DEVICE_TIME:
            {
#if ( LORAMAC_CLASSB_ENABLED == 1 )
                if( IsClassBSwitchPending == true )
                {
                    LmHandlerBeaconReq( );
                }
#endif /* LORAMAC_CLASSB_ENABLED == 1 */
            }
            break;
        case MLME_BEACON_ACQUISITION:
            {
                if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
                {
                    /* Beacon has been acquired */
                    /* Request server for ping slot */
                    LmHandlerPingSlotReq( LmHandlerParams.PingSlotPeriodicity );
                }
                else
                {
                    /* Beacon not acquired */
                    /* Request Device Time again. */
                    LmHandlerDeviceTimeReq( );
                }
            }
            break;
        case MLME_PING_SLOT_INFO:
            {
#if ( LORAMAC_CLASSB_ENABLED == 1 )
                if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
                {
                    MibRequestConfirm_t mibReq;

                    /* Class B is now activated */
                    mibReq.Type = MIB_DEVICE_CLASS;
                    mibReq.Param.Class = CLASS_B;
                    LoRaMacMibSetRequestConfirm( &mibReq );
                    /* Notify upper layer */
                    if( LmHandlerCallbacks->OnClassChange != NULL )
                    {
                        LmHandlerCallbacks->OnClassChange( CLASS_B );
                    }
                    IsClassBSwitchPending = false;
                }
                else
                {
                    LmHandlerPingSlotReq( LmHandlerParams.PingSlotPeriodicity );
                }
#endif /* LORAMAC_CLASSB_ENABLED == 1 */
            }
            break;
        default:
            break;
    }
}

static void MlmeIndication( MlmeIndication_t *mlmeIndication, LoRaMacRxStatus_t *rxStatus )
{
    RxParams.IsMcpsIndication = 0;
    RxParams.Status = mlmeIndication->Status;
    RxParams.Datarate = mlmeIndication->RxDatarate;
    RxParams.Rssi = rxStatus->Rssi;
    RxParams.Snr = rxStatus->Snr;
    RxParams.RxSlot = rxStatus->RxSlot;
    RxParams.DownlinkCounter = mlmeIndication->DownLinkCounter;
    if( ( LmHandlerCallbacks->OnRxData != NULL ) && ( mlmeIndication->MlmeIndication != MLME_BEACON ) && ( mlmeIndication->MlmeIndication != MLME_BEACON_LOST ) )
    {
        LmHandlerCallbacks->OnRxData( NULL, &RxParams );
    }

    /* Call packages RxProcess function */
    LmHandlerPackagesNotify( PACKAGE_MLME_INDICATION, mlmeIndication );

    switch( mlmeIndication->MlmeIndication )
    {
        case MLME_BEACON_LOST:
            {
#if ( LORAMAC_CLASSB_ENABLED == 1 )
                MibRequestConfirm_t mibReq;
                /* Switch to class A again */
                mibReq.Type = MIB_DEVICE_CLASS;
                mibReq.Param.Class = CLASS_A;
                LoRaMacMibSetRequestConfirm( &mibReq );

                BeaconParams.State = LORAMAC_HANDLER_BEACON_LOST;
                BeaconParams.Info.Time.Seconds = 0;
                BeaconParams.Info.GwSpecific.InfoDesc = 0;
                memset1( BeaconParams.Info.GwSpecific.Info, 0, 6 );

                if( LmHandlerCallbacks->OnClassChange != NULL )
                {
                    LmHandlerCallbacks->OnClassChange( CLASS_A );
                }
                if( LmHandlerCallbacks->OnBeaconStatusChange != NULL )
                {
                    LmHandlerCallbacks->OnBeaconStatusChange( &BeaconParams );
                }

                LmHandlerDeviceTimeReq( );
#endif /* LORAMAC_CLASSB_ENABLED == 1 */
            }
            break;
        case MLME_BEACON:
            {
#if ( LORAMAC_CLASSB_ENABLED == 1 )
                if( mlmeIndication->Status == LORAMAC_EVENT_INFO_STATUS_BEACON_LOCKED )
                {
                    BeaconParams.State = LORAMAC_HANDLER_BEACON_RX;
                    BeaconParams.Info = mlmeIndication->BeaconInfo;

                    if( LmHandlerCallbacks->OnBeaconStatusChange != NULL )
                    {
                        LmHandlerCallbacks->OnBeaconStatusChange( &BeaconParams );
                    }
                }
                else if( mlmeIndication->Status == LORAMAC_EVENT_INFO_STATUS_BEACON_NOT_FOUND )
                {
                    BeaconParams.State = LORAMAC_HANDLER_BEACON_NRX;
                    BeaconParams.Info = mlmeIndication->BeaconInfo;

                    if( LmHandlerCallbacks->OnBeaconStatusChange != NULL )
                    {
                        LmHandlerCallbacks->OnBeaconStatusChange( &BeaconParams );
                    }
                }
#endif /* LORAMAC_CLASSB_ENABLED == 1 */
            }
            break;
        default:
            break;
    }
}

/*
 *=============================================================================
 * PACKAGES HANDLING
 *=============================================================================
 */

LmHandlerErrorStatus_t LmHandlerPackageRegister( uint8_t id, void *params )
{
    LmhPackage_t *package = NULL;
    switch( id )
    {
        case PACKAGE_ID_COMPLIANCE:
            {
                package = LmhpCompliancePackageFactory( );
                break;
            }
        default:
            {
                if( LORAMAC_HANDLER_SUCCESS != LmhpPackagesRegister( id, &package ) )
                {
                    return LORAMAC_HANDLER_ERROR;
                }
                break;
            }
    }
    if( package != NULL )
    {
        LmHandlerPackages[id] = package;
        LmHandlerPackages[id]->OnJoinRequest = LmHandlerJoin;
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
        LmHandlerPackages[id]->OnSendRequest = LmHandlerSend;
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
        LmHandlerPackages[id]->OnSysTimeUpdate = LmHandlerCallbacks->OnSysTimeUpdate;
        LmHandlerPackages[id]->OnSystemReset = LmHandlerCallbacks->OnSystemReset;
#endif /* LORAMAC_VERSION */
        LmHandlerPackages[id]->OnDeviceTimeRequest = LmHandlerDeviceTimeReq;
        LmHandlerPackages[id]->OnPackageProcessEvent = LmHandlerCallbacks->OnMacProcess;
        LmHandlerPackages[id]->Init( params, AppData.Buffer, AppData.BufferSize );

        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

static bool LmHandlerPackageIsInitialized( uint8_t id )
{
    if( ( id < PKG_MAX_NUMBER ) && ( LmHandlerPackages[id]->IsInitialized != NULL ) )
    {
        return LmHandlerPackages[id]->IsInitialized( );
    }
    else
    {
        return false;
    }
}

static void LmHandlerPackagesNotify( PackageNotifyTypes_t notifyType, void *params )
{
    for( int8_t i = 0; i < PKG_MAX_NUMBER; i++ )
    {
        if( LmHandlerPackages[i] != NULL )
        {
            switch( notifyType )
            {
                case PACKAGE_MCPS_CONFIRM:
                    {
                        if( LmHandlerPackages[i]->OnMcpsConfirmProcess != NULL )
                        {
                            LmHandlerPackages[i]->OnMcpsConfirmProcess( ( McpsConfirm_t * ) params );
                        }
                        break;
                    }
                case PACKAGE_MCPS_INDICATION:
                    {
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
                        if( ( LmHandlerPackages[i]->OnMcpsIndicationProcess != NULL ) &&
                            ( ( LmHandlerPackages[i]->Port == ( ( McpsIndication_t * )params )->Port ) ||
                              ( ( i == PACKAGE_ID_COMPLIANCE ) && ( LmHandlerPackages[PACKAGE_ID_COMPLIANCE]->IsRunning() ) ) ) )
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
                        if( LmHandlerPackages[i]->OnMcpsIndicationProcess != NULL )
#endif /* LORAMAC_VERSION */
                        {
                            LmHandlerPackages[i]->OnMcpsIndicationProcess( ( McpsIndication_t * )params );
                        }
                        break;
                    }
                case PACKAGE_MLME_CONFIRM:
                    {
                        if( LmHandlerPackages[i]->OnMlmeConfirmProcess != NULL )
                        {
                            LmHandlerPackages[i]->OnMlmeConfirmProcess( ( MlmeConfirm_t * )params );
                        }
                        break;
                    }
                case PACKAGE_MLME_INDICATION:
                    {
                        if( LmHandlerPackages[i]->OnMlmeIndicationProcess != NULL )
                        {
                            LmHandlerPackages[i]->OnMlmeIndicationProcess( params );
                        }
                        break;
                    }
                default:
                    {
                        break;
                    }
            }
        }
    }
}

static bool LmHandlerPackageIsTxPending( void )
{
    for( int8_t i = 0; i < PKG_MAX_NUMBER; i++ )
    {
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
        if( ( LmHandlerPackages[i] != NULL ) && ( i != PACKAGE_ID_COMPLIANCE ) )
#else
        if( LmHandlerPackages[i] != NULL )
#endif /* LORAMAC_VERSION */
        {
            if( LmHandlerPackages[i]->IsTxPending( ) == true )
            {
                return true;
            }
        }
    }
    return false;
}

static void LmHandlerPackagesProcess( void )
{
    for( int8_t i = 0; i < PKG_MAX_NUMBER; i++ )
    {
        if( ( LmHandlerPackages[i] != NULL ) &&
            ( LmHandlerPackages[i]->Process != NULL ) &&
            ( LmHandlerPackageIsInitialized( i ) != false ) )
        {
            LmHandlerPackages[i]->Process( );
        }
    }
}

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
static void LmHandlerOnTxFrameCtrlChanged( LmHandlerMsgTypes_t isTxConfirmed )
{
    LmHandlerParams.IsTxConfirmed = isTxConfirmed;

    if (LmHandlerCallbacks->OnTxFrameCtrlChanged != NULL)
    {
        LmHandlerCallbacks->OnTxFrameCtrlChanged( isTxConfirmed );
    }
}

static void LmHandlerOnPingSlotPeriodicityChanged( uint8_t pingSlotPeriodicity )
{
    LmHandlerParams.PingSlotPeriodicity = pingSlotPeriodicity;

    if (LmHandlerCallbacks->OnPingSlotPeriodicityChanged != NULL)
    {
        LmHandlerCallbacks->OnPingSlotPeriodicityChanged( pingSlotPeriodicity );
    }
}
#endif /* LORAMAC_VERSION */

/*
 *=============================================================================
 * ST ADDITIONAL FUNCTIONS
 *=============================================================================
 */

LmHandlerErrorStatus_t LmHandlerGetVersion( LmHandlerVersionType_t lmhType, uint32_t *featureVersion )
{
    if( featureVersion == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    switch( lmhType )
    {
        case LORAMAC_HANDLER_L2_VERSION:
            *featureVersion = LORAMAC_VERSION;
            break;
        case LORAMAC_HANDLER_REGION_VERSION:
            *featureVersion = REGION_VERSION;
            break;
        default:
            break;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerStop( void )
{
    if( LoRaMacDeInitialization() == LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_BUSY_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerHalt( void )
{
    if( LoRaMacHalt() == LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_BUSY_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerLinkCheckReq( void )
{
    LoRaMacStatus_t status;
    MlmeReq_t mlmeReq;

    mlmeReq.Type = MLME_LINK_CHECK;

    status = LoRaMacMlmeRequest( &mlmeReq );

    if( status == LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerGetDevEUI( uint8_t *devEUI )
{
    MibRequestConfirm_t mibReq;
    if( devEUI == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_DEV_EUI;
    mibReq.Param.DevEui = devEUI;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetDevEUI( uint8_t *devEUI )
{
    MibRequestConfirm_t mibReq;

    /* Not yet joined */
    if( LmHandlerJoinStatus() != LORAMAC_HANDLER_SET )
    {
        mibReq.Type = MIB_DEV_EUI;
        mibReq.Param.DevEui = devEUI;
        if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change Keys in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerGetAppEUI( uint8_t *appEUI )
{
    MibRequestConfirm_t mibReq;

    if( appEUI == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_JOIN_EUI;
    mibReq.Param.JoinEui = appEUI;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetAppEUI( uint8_t *appEUI )
{
    MibRequestConfirm_t mibReq;

    /* Not yet joined */
    if( LmHandlerJoinStatus() != LORAMAC_HANDLER_SET )
    {
        mibReq.Type = MIB_JOIN_EUI;
        mibReq.Param.JoinEui = appEUI;
        if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change Keys in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerGetNetworkID( uint32_t *networkId )
{
    MibRequestConfirm_t mibReq;

    if( networkId == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_NET_ID;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }
    *networkId = mibReq.Param.NetID;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetNetworkID( uint32_t networkId )
{
    MibRequestConfirm_t mibReq;

    /* Not yet joined */
    if( LmHandlerJoinStatus() != LORAMAC_HANDLER_SET )
    {
        mibReq.Type = MIB_NET_ID;
        mibReq.Param.NetID = networkId;
        if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change NetworkID in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerGetDevAddr( uint32_t *devAddr )
{
    MibRequestConfirm_t mibReq;

    if( devAddr == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_DEV_ADDR;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }
    *devAddr = mibReq.Param.DevAddr;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetDevAddr( uint32_t devAddr )
{
    MibRequestConfirm_t mibReq;

    /* Not yet joined */
    if( LmHandlerJoinStatus() != LORAMAC_HANDLER_SET )
    {
        mibReq.Type = MIB_DEV_ADDR;
        mibReq.Param.DevAddr = devAddr;
        if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
        {
            return LORAMAC_HANDLER_ERROR;
        }
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change DevAddr in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerSetActiveRegion( LoRaMacRegion_t region )
{
    /* Not yet joined */
    if( LmHandlerJoinStatus() != LORAMAC_HANDLER_SET )
    {
        LmHandlerParams.ActiveRegion = region;
        return LmHandlerConfigure( &LmHandlerParams );
    }
    else
    {
        /* Cannot change Region in running state */
        return LORAMAC_HANDLER_ERROR;
    }
}

LmHandlerErrorStatus_t LmHandlerGetAdrEnable( bool *adrEnable )
{
    if( adrEnable == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *adrEnable = LmHandlerParams.AdrEnable;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetAdrEnable( bool adrEnable )
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_ADR;
    mibReq.Param.AdrEnable = adrEnable;
    if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }
    LmHandlerParams.AdrEnable = adrEnable;

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetTxDatarate( int8_t txDatarate )
{
    if( LmHandlerParams.AdrEnable == true )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_CHANNELS_DATARATE;
    mibReq.Param.ChannelsDatarate = txDatarate;
    if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    LmHandlerParams.TxDatarate = txDatarate;

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetDutyCycleEnable( bool *dutyCycleEnable )
{
    if( dutyCycleEnable == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *dutyCycleEnable = LmHandlerParams.DutyCycleEnabled;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetDutyCycleEnable( bool dutyCycleEnable )
{
    LmHandlerParams.DutyCycleEnabled = dutyCycleEnable;
    LoRaMacTestSetDutyCycleOn( dutyCycleEnable );

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetRX2Params( RxChannelParams_t *rxParams )
{
    if( rxParams == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_RX2_CHANNEL;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    rxParams->Frequency = mibReq.Param.Rx2Channel.Frequency;
    rxParams->Datarate = mibReq.Param.Rx2Channel.Datarate;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetTxPower( int8_t *txPower )
{
    MibRequestConfirm_t mibReq;
    if( txPower == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_CHANNELS_TX_POWER;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *txPower = mibReq.Param.ChannelsTxPower;
    LmHandlerParams.TxPower = *txPower;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetRx1Delay( uint32_t *rxDelay )
{
    MibRequestConfirm_t mibReq;
    if( rxDelay == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_RECEIVE_DELAY_1;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *rxDelay = mibReq.Param.ReceiveDelay1;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetRx2Delay( uint32_t *rxDelay )
{
    MibRequestConfirm_t mibReq;
    if( rxDelay == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_RECEIVE_DELAY_2;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *rxDelay = mibReq.Param.ReceiveDelay2;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetJoinRx1Delay( uint32_t *rxDelay )
{
    MibRequestConfirm_t mibReq;
    if( rxDelay == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_JOIN_ACCEPT_DELAY_1;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *rxDelay = mibReq.Param.JoinAcceptDelay1;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetJoinRx2Delay( uint32_t *rxDelay )
{
    MibRequestConfirm_t mibReq;
    if( rxDelay == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_JOIN_ACCEPT_DELAY_2;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *rxDelay = mibReq.Param.JoinAcceptDelay2;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetTxPower( int8_t txPower )
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_CHANNELS_TX_POWER;
    mibReq.Param.ChannelsTxPower = txPower;
    if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    LmHandlerParams.TxPower = txPower;
    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetRX2Params( RxChannelParams_t *rxParams )
{
    MibRequestConfirm_t mibReq;

    mibReq.Type = MIB_RX2_CHANNEL;
    mibReq.Param.Rx2Channel.Frequency = rxParams->Frequency;
    mibReq.Param.Rx2Channel.Datarate = rxParams->Datarate;
    if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetRx1Delay( uint32_t rxDelay )
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RECEIVE_DELAY_1;
    mibReq.Param.ReceiveDelay1 = rxDelay;
    if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetRx2Delay( uint32_t rxDelay )
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_RECEIVE_DELAY_2;
    mibReq.Param.ReceiveDelay2 = rxDelay;
    if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetJoinRx1Delay( uint32_t rxDelay )
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_JOIN_ACCEPT_DELAY_1;
    mibReq.Param.JoinAcceptDelay1 = rxDelay;
    if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetJoinRx2Delay( uint32_t rxDelay )
{
    MibRequestConfirm_t mibReq;
    mibReq.Type = MIB_JOIN_ACCEPT_DELAY_2;
    mibReq.Param.JoinAcceptDelay2 = rxDelay;
    if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerGetPingPeriodicity( uint8_t *pingPeriodicity )
{
#if ( LORAMAC_CLASSB_ENABLED == 1 )
    if( pingPeriodicity == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    *pingPeriodicity = LmHandlerParams.PingSlotPeriodicity;
    return LORAMAC_HANDLER_SUCCESS;
#else /* LORAMAC_CLASSB_ENABLED == 0 */
    return LORAMAC_HANDLER_ERROR;
#endif /* LORAMAC_CLASSB_ENABLED */
}

LmHandlerErrorStatus_t LmHandlerSetPingPeriodicity( uint8_t pingPeriodicity )
{
#if ( LORAMAC_CLASSB_ENABLED == 1 )
    /* Not yet joined */
    if( LmHandlerJoinStatus() != LORAMAC_HANDLER_SET )
    {
        LmHandlerParams.PingSlotPeriodicity = pingPeriodicity;
        return LORAMAC_HANDLER_SUCCESS;
    }
    else
    {
        /* Cannot change Region in running state */
        return LmHandlerPingSlotReq( pingPeriodicity );
    }
#else /* LORAMAC_CLASSB_ENABLED == 0 */
    return LORAMAC_HANDLER_ERROR;
#endif /* LORAMAC_CLASSB_ENABLED */
}

LmHandlerErrorStatus_t LmHandlerGetBeaconState( BeaconState_t *beaconState )
{
#if ( LORAMAC_CLASSB_ENABLED == 1 )
    MibRequestConfirm_t mibReq;

    if( beaconState == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }

    mibReq.Type = MIB_BEACON_STATE;
    if( LoRaMacMibGetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
    {
        return LORAMAC_HANDLER_ERROR;
    }
    *beaconState =  mibReq.Param.BeaconState;
    return LORAMAC_HANDLER_SUCCESS;
#else /* LORAMAC_CLASSB_ENABLED == 0 */
    return LORAMAC_HANDLER_ERROR;
#endif /* LORAMAC_CLASSB_ENABLED */
}

LmHandlerErrorStatus_t LmHandlerGetKey( KeyIdentifier_t keyID, uint8_t *key )
{
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    Key_t *keyItem;
#endif /* LORAWAN_KMS */

    if( key == NULL )
    {
        return LORAMAC_HANDLER_ERROR;
    }
#if (!defined (LORAWAN_KMS) || (LORAWAN_KMS == 0))
    if( SECURE_ELEMENT_SUCCESS != SecureElementGetKeyByID( keyID, &keyItem ) )
    {
        return LORAMAC_HANDLER_ERROR;
    }
    memcpy1( key, keyItem->KeyValue, 16 );
#else
    if( SECURE_ELEMENT_SUCCESS != SecureElementGetKeyByID( keyID, key ) )
    {
        return LORAMAC_HANDLER_ERROR;
    }

#endif /* LORAWAN_KMS */

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerSetKey( KeyIdentifier_t keyID, uint8_t *key )
{
    /* Not yet joined */
    if( LmHandlerJoinStatus( ) != LORAMAC_HANDLER_SET )
    {
        if( keyID == APP_KEY ) /* Specific usage because APP_KEY is used to derive other keys */
        {
            MibRequestConfirm_t mibReq;
            mibReq.Type = MIB_APP_KEY;
            mibReq.Param.AppKey = key;
            if( LoRaMacMibSetRequestConfirm( &mibReq ) != LORAMAC_STATUS_OK )
            {
                return LORAMAC_HANDLER_ERROR;
            }
            return LORAMAC_HANDLER_SUCCESS;
        }
        else if( SECURE_ELEMENT_SUCCESS != SecureElementSetKey( keyID, key ) )
        {
            return LORAMAC_HANDLER_ERROR;
        }
    }
    else
    {
        /* Cannot change Keys in running state */
        return LORAMAC_HANDLER_ERROR;
    }

    return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmHandlerNvmDataStore( void )
{
#if (defined( CONTEXT_MANAGEMENT_ENABLED ) && ( CONTEXT_MANAGEMENT_ENABLED == 1 ))
    LoRaMacNvmData_t *nvm;
    uint32_t nvm_size;
    LmHandlerErrorStatus_t lmhStatus = LORAMAC_HANDLER_SUCCESS;
    int32_t status = NVM_DATA_OK;

    lmhStatus = LmHandlerHalt();

    if( lmhStatus == LORAMAC_HANDLER_SUCCESS )
    {
        status = NvmDataMgmtStoreBegin();

        if( status == NVM_DATA_NO_UPDATED_DATA )
        {
            lmhStatus = LORAMAC_HANDLER_NVM_DATA_UP_TO_DATE;
        }
        else if( ( status != NVM_DATA_OK ) || ( LmHandlerCallbacks->OnStoreContextRequest == NULL ) )
        {
            lmhStatus = LORAMAC_HANDLER_ERROR;
        }
        else
        {
            MibRequestConfirm_t mibReq;
            mibReq.Type = MIB_NVM_CTXS;
            LoRaMacMibGetRequestConfirm( &mibReq );
            nvm = ( LoRaMacNvmData_t * )mibReq.Param.Contexts;
            nvm_size = ( ( sizeof( LoRaMacNvmData_t ) + 7 ) & ~0x07 );
            LmHandlerCallbacks->OnStoreContextRequest( nvm, nvm_size );
        }

        if( NvmDataMgmtStoreEnd() != NVM_DATA_OK )
        {
            lmhStatus = LORAMAC_HANDLER_ERROR;
        }
    }

    if( ( lmhStatus == LORAMAC_HANDLER_SUCCESS ) && ( LmHandlerCallbacks->OnNvmDataChange != NULL ) )
    {
        LmHandlerCallbacks->OnNvmDataChange( LORAMAC_HANDLER_NVM_STORE );
    }

    return lmhStatus;
#else
    return LORAMAC_HANDLER_ERROR;
#endif /* CONTEXT_MANAGEMENT_ENABLED */
}
