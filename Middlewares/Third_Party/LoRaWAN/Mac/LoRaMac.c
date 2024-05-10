/*!
 * \file      LoRaMac.c
 *
 * \brief     LoRa MAC layer implementation
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
 *              (C)2013-2017 Semtech
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
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    LoRaMac.c
  * @author  MCD Application Team
  * @brief   LoRa MAC layer implementation
  ******************************************************************************
  */
#include "utilities.h"
#include "Region.h"
#include "LoRaMacClassB.h"
#include "secure-element.h"
#include "LoRaMacTest.h"
#include "LoRaMacConfirmQueue.h"
#include "LoRaMacMessageTypes.h"
#include "LoRaMacParser.h"
#include "LoRaMacCommands.h"
#include "LoRaMacAdr.h"
#include "LoRaMacSerializer.h"
#include "LoRaMacVersion.h"
#include "radio.h"

#include "LoRaMac.h"
#include "mw_log_conf.h"

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000300 ) || ( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
#else
#error LORAMAC_VERSION not valid
#endif /* LORAMAC_VERSION */
/*!
 * Maximum PHY layer payload size
 */
#define LORAMAC_PHY_MAXPAYLOAD                      255

/*!
 * Maximum length of the fOpts field
 */
#define LORA_MAC_COMMAND_MAX_FOPTS_LENGTH           15

/*!
 * LoRaMac duty cycle for the back-off procedure during the first hour.
 */
#define BACKOFF_DC_1_HOUR                           100

/*!
 * LoRaMac duty cycle for the back-off procedure during the next 10 hours.
 */
#define BACKOFF_DC_10_HOURS                         1000

/*!
 * LoRaMac duty cycle for the back-off procedure during the next 24 hours.
 */
#define BACKOFF_DC_24_HOURS                         10000

/*!
 * Maximum value for the ADR ack counter
 */
#define ADR_ACK_COUNTER_MAX                         0xFFFFFFFF

#if defined(__ICCARM__)
#ifndef __NO_INIT
#define __NO_INIT __no_init
#endif
#ifndef __ROOT
#define __ROOT __root
#endif
#else
#ifndef __NO_INIT
#define __NO_INIT
#endif
#ifndef __ROOT
#define __ROOT
#endif
#endif
/*!
 * LoRaMAC Max EIRP (dBm) table
 */
static const uint8_t LoRaMacMaxEirpTable[] = { 8, 10, 12, 13, 14, 16, 18, 20, 21, 24, 26, 27, 29, 30, 33, 36 };

/*!
 * LoRaMac internal states
 */
enum eLoRaMacState
{
    LORAMAC_IDLE          = 0x00000000,
    LORAMAC_STOPPED       = 0x00000001,
    LORAMAC_TX_RUNNING    = 0x00000002,
    LORAMAC_RX            = 0x00000004,
    LORAMAC_ACK_RETRY     = 0x00000010,
    LORAMAC_TX_DELAYED    = 0x00000020,
    LORAMAC_TX_CONFIG     = 0x00000040,
    LORAMAC_RX_ABORT      = 0x00000080,
};

/*
 * Request permission state
 */
typedef enum eLoRaMacRequestHandling
{
    LORAMAC_REQUEST_HANDLING_OFF = 0,
    LORAMAC_REQUEST_HANDLING_ON = !LORAMAC_REQUEST_HANDLING_OFF
}LoRaMacRequestHandling_t;

typedef struct sLoRaMacCtx
{
    /*!
     * Length of packet in PktBuffer
     */
    uint16_t PktBufferLen;
    /*!
     * Buffer containing the data to be sent or received.
     */
    uint8_t PktBuffer[LORAMAC_PHY_MAXPAYLOAD];
    /*!
     * Current processed transmit message
     */
    LoRaMacMessage_t TxMsg;
    /*!
     * Buffer containing the data received by the application.
     */
    uint8_t AppData[LORAMAC_PHY_MAXPAYLOAD];
    /*!
     * Size of buffer containing the application data.
     */
    uint8_t AppDataSize;
    /*!
     * Buffer containing the upper layer data.
     */
    uint8_t RxPayload[LORAMAC_PHY_MAXPAYLOAD];
    SysTime_t LastTxSysTime;
    /*!
     * LoRaMac internal state
     */
    uint32_t MacState;
    /*!
     * LoRaMac upper layer event functions
     */
    LoRaMacPrimitives_t* MacPrimitives;
    /*!
     * LoRaMac upper layer callback functions
     */
    LoRaMacCallback_t* MacCallbacks;
    /*!
     * Radio events function pointer
     */
    RadioEvents_t RadioEvents;
    /*!
     * LoRaMac duty cycle delayed Tx timer
     */
    TimerEvent_t TxDelayedTimer;
    /*!
     * LoRaMac reception windows timers
     */
    TimerEvent_t RxWindowTimer1;
    TimerEvent_t RxWindowTimer2;
    /*!
     * LoRaMac reception windows delay
     * \remark normal frame: RxWindowXDelay = ReceiveDelayX - RADIO_WAKEUP_TIME
     *         join frame  : RxWindowXDelay = JoinAcceptDelayX - RADIO_WAKEUP_TIME
     */
    uint32_t RxWindow1Delay;
    uint32_t RxWindow2Delay;
    /*!
     * LoRaMac Rx windows configuration
     */
    RxConfigParams_t RxWindow1Config;
    RxConfigParams_t RxWindow2Config;
    RxConfigParams_t RxWindowCConfig;
    /*!
     * Acknowledge timeout timer. Used for packet retransmissions.
     */
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    TimerEvent_t AckTimeoutTimer;
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    TimerEvent_t RetransmitTimeoutTimer;
#endif /* LORAMAC_VERSION */
    /*!
     * Uplink messages repetitions counter
     */
    uint8_t ChannelsNbTransCounter;
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    /*!
     * Number of trials to get a frame acknowledged
     */
    uint8_t AckTimeoutRetries;
    /*!
     * Number of trials to get a frame acknowledged
     */
    uint8_t AckTimeoutRetriesCounter;
    /*!
     * Indicates if the AckTimeout timer has expired or not
     */
    bool AckTimeoutRetry;
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    /*!
     * Indicates if the AckTimeout timer has expired or not
     */
    bool RetransmitTimeoutRetry;
#endif /* LORAMAC_VERSION */
    /*!
     * If the node has sent a FRAME_TYPE_DATA_CONFIRMED_UP this variable indicates
     * if the nodes needs to manage the server acknowledgement.
     */
    bool NodeAckRequested;
    /*!
     * Current channel index
     */
    uint8_t Channel;
    /*!
     * Last transmission time on air
     */
    TimerTime_t TxTimeOnAir;
    /*!
     * Structure to hold an MCPS indication data.
     */
    McpsIndication_t McpsIndication;
    /*!
     * Structure to hold MCPS confirm data.
     */
    McpsConfirm_t McpsConfirm;
    /*!
     * Structure to hold MLME confirm data.
     */
    MlmeConfirm_t MlmeConfirm;
    /*!
     * Structure to hold MLME indication data.
     */
    MlmeIndication_t MlmeIndication;
    /*!
     * Structure to hold global Rx Status.
     */
    LoRaMacRxStatus_t RxStatus;
    /*!
     * Holds the current rx window slot
     */
    LoRaMacRxSlot_t RxSlot;
    /*!
     * LoRaMac tx/rx operation state
     */
    LoRaMacFlags_t MacFlags;
    /*!
     * Data structure indicating if a request is allowed or not.
     */
    LoRaMacRequestHandling_t AllowRequests;
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    /*!
     * Cycle timer for Type 0 Rejoin requests
     */
    TimerEvent_t Rejoin0CycleTimer;
    /*!
     * Cycle timer for Type 1 Rejoin requests
     */
    TimerEvent_t Rejoin1CycleTimer;
    /*!
     * Cycle timer for Rejoin requests triggered by ForceRejoinReq MAC command
     */
    TimerEvent_t ForceRejoinReqCycleTimer;
    /*!
     * Time of Type 0 Rejoin requests cycles
     */
    TimerTime_t Rejoin0CycleTime;
    /*!
     * Time of Type 1 Rejoin requests cycles
     */
    TimerTime_t Rejoin1CycleTime;
    /*
    * Time of Force Rejoin requests cycles
    */
    TimerTime_t ForceRejoinCycleTime;
#endif /* LORAMAC_VERSION */
    /*!
     * Duty cycle wait time
     */
    TimerTime_t DutyCycleWaitTime;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    /*!
     * Start time of the response timeout
     */
    TimerTime_t ResponseTimeoutStartTime;
#endif /* LORAMAC_VERSION */
    /*!
     * Buffer containing the MAC layer commands
     */
    uint8_t MacCommandsBuffer[LORA_MAC_COMMAND_MAX_LENGTH];
    /*!
     * Time on air accumulation
     */
    uint32_t SumSendTime;
    /*!
     * Send count accumulation
     */
    uint32_t SumSendCount;
}LoRaMacCtx_t;

/*!
 * Module context.
 */
static LoRaMacCtx_t MacCtx;

#if (defined( CONTEXT_MANAGEMENT_ENABLED ) && ( CONTEXT_MANAGEMENT_ENABLED == 1 ))
#if defined(__ICCARM__)
__NO_INIT __ROOT static LoRaMacNvmData_t Nvm @ ".LW_NVM_RAM";
#elif defined(__GNUC__)
__attribute__((section(".bss.LW_NVM_RAM")))
__NO_INIT __ROOT static LoRaMacNvmData_t Nvm;
#else
#warning NVM RAM placement not defined
__NO_INIT __ROOT static LoRaMacNvmData_t Nvm;
#endif

#if defined(__ICCARM__)
__NO_INIT __ROOT static LoRaMacNvmData_t NvmBackup @ ".LW_NVM_BACKUP_RAM";
#elif defined(__GNUC__)
__attribute__((section(".bss.LW_NVM_BACKUP_RAM")))
__NO_INIT __ROOT static LoRaMacNvmData_t NvmBackup;
#else
#warning NVM RAM placement not defined
__NO_INIT __ROOT static LoRaMacNvmData_t NvmBackup;
#endif
#else
static LoRaMacNvmData_t Nvm;
#endif /* CONTEXT_MANAGEMENT_ENABLED */

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
static Band_t RegionBands[REGION_NVM_MAX_NB_BANDS];
#endif /* LORAMAC_VERSION */

static const KeyIdentifier_t MCKeys[LORAMAC_MAX_MC_CTX] = {
#if ( LORAMAC_MAX_MC_CTX > 0 )
    MC_KEY_0,
#endif /* LORAMAC_MAX_MC_CTX > 0 */
#if ( LORAMAC_MAX_MC_CTX > 1 )
    MC_KEY_1,
#endif /* LORAMAC_MAX_MC_CTX > 1 */
#if ( LORAMAC_MAX_MC_CTX > 2 )
    MC_KEY_2,
#endif /* LORAMAC_MAX_MC_CTX > 2 */
#if ( LORAMAC_MAX_MC_CTX > 3 )
    MC_KEY_3,
#endif /* LORAMAC_MAX_MC_CTX > 3 */
};

static const KeyIdentifier_t MCAppSKeys[LORAMAC_MAX_MC_CTX] = {
#if ( LORAMAC_MAX_MC_CTX > 0 )
    MC_APP_S_KEY_0,
#endif /* LORAMAC_MAX_MC_CTX > 0 */
#if ( LORAMAC_MAX_MC_CTX > 1 )
    MC_APP_S_KEY_1,
#endif /* LORAMAC_MAX_MC_CTX > 1 */
#if ( LORAMAC_MAX_MC_CTX > 2 )
    MC_APP_S_KEY_2,
#endif /* LORAMAC_MAX_MC_CTX > 2 */
#if ( LORAMAC_MAX_MC_CTX > 3 )
    MC_APP_S_KEY_3,
#endif /* LORAMAC_MAX_MC_CTX > 3 */
};
static const KeyIdentifier_t MCNwkSKeys[LORAMAC_MAX_MC_CTX] = {
#if ( LORAMAC_MAX_MC_CTX > 0 )
    MC_NWK_S_KEY_0,
#endif /* LORAMAC_MAX_MC_CTX > 0 */
#if ( LORAMAC_MAX_MC_CTX > 1 )
    MC_NWK_S_KEY_1,
#endif /* LORAMAC_MAX_MC_CTX > 1 */
#if ( LORAMAC_MAX_MC_CTX > 2 )
    MC_NWK_S_KEY_2,
#endif /* LORAMAC_MAX_MC_CTX > 2 */
#if ( LORAMAC_MAX_MC_CTX > 3 )
    MC_NWK_S_KEY_3,
#endif /* LORAMAC_MAX_MC_CTX > 3 */
};

/*!
 * Defines the LoRaMac radio events status
 */
typedef union uLoRaMacRadioEvents
{
    uint32_t Value;
    struct sEvents
    {
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
        uint32_t RxProcessPending : 1;
#endif /* LORAMAC_VERSION */
        uint32_t RxTimeout        : 1;
        uint32_t RxError          : 1;
        uint32_t TxTimeout        : 1;
        uint32_t RxDone           : 1;
        uint32_t TxDone           : 1;
    }Events;
}LoRaMacRadioEvents_t;

/*!
 * LoRaMac radio events status
 */
LoRaMacRadioEvents_t LoRaMacRadioEvents = { .Value = 0 };

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
static void OnRadioTxDone( void );

/*!
 * \brief This function prepares the MAC to abort the execution of function
 *        OnRadioRxDone in case of a reception error.
 */
static void PrepareRxDoneAbort( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
static void OnRadioRxDone( uint8_t* payload, uint16_t size, int16_t rssi, int8_t snr );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
static void OnRadioTxTimeout( void );

/*!
 * \brief Function executed on Radio Rx error event
 */
static void OnRadioRxError( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
static void OnRadioRxTimeout( void );

/*!
 * \brief Function executed on duty cycle delayed Tx  timer event
 */
static void OnTxDelayedTimerEvent( void* context );

/*!
 * \brief Function executed on first Rx window timer event
 */
static void OnRxWindow1TimerEvent( void* context );

/*!
 * \brief Function executed on second Rx window timer event
 */
static void OnRxWindow2TimerEvent( void* context );

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
/*!
 * \brief Function executed on Rejoin Type 0 cycle timer event
 */
static void OnRejoin0CycleTimerEvent( void* context );

/*!
 * \brief Function executed on Rejoin Type 0 cycle timer event
 */
static void OnRejoin1CycleTimerEvent( void* context );

/*!
 * \brief Function executed on Rejoin Type 0 or 2 cycle timer event
 *        which was requested by a ForceRejoinReq MAC command.
 */
static void OnForceRejoinReqCycleTimerEvent( void* context );
#endif /* LORAMAC_VERSION */

/*!
 * \brief Function executed on AckTimeout timer event
 */
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
static void OnAckTimeoutTimerEvent( void* context );
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
static void OnRetransmitTimeoutTimerEvent( void* context );
#endif /* LORAMAC_VERSION */

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
/*!
 * Computes next 32 bit downlink counter value and determines the frame counter ID.
 *
 * \param[IN]     addrID                - Address identifier
 * \param[IN]     fType                 - Frame type
 * \param[IN]     macMsg                - Data message object, holding the current 16 bit transmitted frame counter
 * \param[IN]     lrWanVersion          - LoRaWAN version
 * \param[IN]     maxFCntGap            - Maximum allowed frame counter difference (only for 1.0.X necessary)
 * \param[OUT]    fCntID                - Frame counter identifier
 * \param[OUT]    currentDown           - Current downlink counter value
 *
 * \retval                              - Status of the operation
 */
static LoRaMacCryptoStatus_t GetFCntDown( AddressIdentifier_t addrID, FType_t fType, LoRaMacMessageData_t* macMsg, Version_t lrWanVersion,
                                          uint16_t maxFCntGap, FCntIdentifier_t* fCntID, uint32_t* currentDown );
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
/*!
 * Computes next 32 bit downlink counter value and determines the frame counter ID.
 *
 * \param [in]    addrID                - Address identifier
 * \param [in]    fType                 - Frame type
 * \param [in]    macMsg                - Data message object, holding the current 16 bit transmitted frame counter
 * \param [in]    lrWanVersion          - LoRaWAN version
 * \param [out]   fCntID                - Frame counter identifier
 * \param [out]   currentDown           - Current downlink counter value
 *
 * \retval                              - Status of the operation
 */
static LoRaMacCryptoStatus_t GetFCntDown( AddressIdentifier_t addrID, FType_t fType, LoRaMacMessageData_t* macMsg, Version_t lrWanVersion,
                                          FCntIdentifier_t* fCntID, uint32_t* currentDown );
#endif /* LORAMAC_VERSION */

/*!
 * \brief Switches the device class
 *
 * \param [in] deviceClass Device class to switch to
 */
static LoRaMacStatus_t SwitchClass( DeviceClass_t deviceClass );

/*!
 * \brief Gets the maximum application payload length in the absence of the optional FOpt field.
 *
 * \param [in] datarate        Current datarate
 *
 * \retval                    Max length
 */
static uint8_t GetMaxAppPayloadWithoutFOptsLength( int8_t datarate );

/*!
 * \brief Validates if the payload fits into the frame, taking the datarate
 *        into account.
 *
 * \details Refer to chapter 4.3.2 of the LoRaWAN specification, v1.0
 *
 * \param lenN Length of the application payload. The length depends on the
 *             datarate and is region specific
 *
 * \param datarate Current datarate
 *
 * \param fOptsLen Length of the fOpts field
 *
 * \retval [false: payload does not fit into the frame, true: payload fits into
 *          the frame]
 */
static bool ValidatePayloadLength( uint8_t lenN, int8_t datarate, uint8_t fOptsLen );

/*!
 * \brief Decodes MAC commands in the fOpts field and in the payload
 *
 * \param [in] payload      A pointer to the payload
 * \param [in] macIndex     The index of the payload where the MAC commands start
 * \param [in] commandsSize The size of the MAC commands
 * \param [in] snr          The SNR value  of the frame
 * \param [in] rxSlot       The RX slot where the frame was received
 */
static void ProcessMacCommands( uint8_t* payload, uint8_t macIndex, uint8_t commandsSize, int8_t snr, LoRaMacRxSlot_t rxSlot );

/*!
 * \brief LoRaMAC layer generic send frame
 *
 * \param [in] macHdr      MAC header field
 * \param [in] fPort       MAC payload port
 * \param [in] fBuffer     MAC data buffer to be sent
 * \param [in] fBufferSize MAC data buffer size
 * \param [in] allowDelayedTx When set to true, the frame will be delayed
 * \retval status          Status of the operation.
 */
static LoRaMacStatus_t Send( LoRaMacHeader_t* macHdr, uint8_t fPort, void* fBuffer, uint16_t fBufferSize, bool allowDelayedTx );

/*!
 * \brief LoRaMAC layer send join/rejoin request
 *
 * \param [in] joinReqType Type of join-request or rejoin
 *
 * \retval status          Status of the operation.
 */
static LoRaMacStatus_t SendReJoinReq( JoinReqIdentifier_t joinReqType );

/*!
 * \brief LoRaMAC layer frame buffer initialization
 *
 * \param [in] macHdr      MAC header field
 * \param [in] fCtrl       MAC frame control field
 * \param [in] fPort       MAC payload port
 * \param [in] fBuffer     MAC data buffer to be sent
 * \param [in] fBufferSize MAC data buffer size
 * \retval status          Status of the operation.
 */
static LoRaMacStatus_t PrepareFrame( LoRaMacHeader_t* macHdr, LoRaMacFrameCtrl_t* fCtrl, uint8_t fPort, void* fBuffer, uint16_t fBufferSize );

/*!
 * \brief Schedules the frame according to the duty cycle
 *
 * \param [in] allowDelayedTx When set to true, the frame will be delayed,
 *                            the duty cycle restriction is active
 * \retval Status of the operation
 */
static LoRaMacStatus_t ScheduleTx( bool allowDelayedTx );

/*!
 * \brief Secures the current processed frame ( TxMsg )
 * \param [in]    txDr      Data rate used for the transmission
 * \param [in]    txCh      Index of the channel used for the transmission
 * \retval status           Status of the operation
 */
static LoRaMacStatus_t SecureFrame( uint8_t txDr, uint8_t txCh );

/*!
 * \brief Calculates the aggregated back off time.
 */
static void CalculateBackOff( void );

/*!
 * \brief Function to remove pending MAC commands
 *
 * \param [in] rxSlot     The RX slot on which the frame was received
 * \param [in] fCtrl      The frame control field of the received frame
 * \param [in] request    The request type
 */
static void RemoveMacCommands( LoRaMacRxSlot_t rxSlot, LoRaMacFrameCtrl_t fCtrl, Mcps_t request );

/*!
 * \brief LoRaMAC layer prepared frame buffer transmission with channel specification
 *
 * \remark PrepareFrame must be called at least once before calling this
 *         function.
 *
 * \param [in] channel     Channel to transmit on
 * \retval status          Status of the operation.
 */
static LoRaMacStatus_t SendFrameOnChannel( uint8_t channel );

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
/*!
 * \brief Sets the radio in continuous transmission mode
 *
 * \remark Uses the radio parameters set on the previous transmission.
 *
 * \param [IN] timeout     Time in seconds while the radio is kept in continuous wave mode
 * \retval status          Status of the operation.
 */
static LoRaMacStatus_t SetTxContinuousWave( uint16_t timeout );

/*!
 * \brief Sets the radio in continuous transmission mode
 *
 * \remark Uses the radio parameters set on the previous transmission.
 *
 * \param [IN] timeout     Time in seconds while the radio is kept in continuous wave mode
 * \param [IN] frequency   RF frequency to be set.
 * \param [IN] power       RF output power to be set.
 * \retval status          Status of the operation.
 */
static LoRaMacStatus_t SetTxContinuousWave1( uint16_t timeout, uint32_t frequency, uint8_t power );
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
/*!
 * \brief Sets the radio in continuous transmission mode
 *
 * \remark Uses the radio parameters set on the previous transmission.
 *
 * \param [in] timeout     Time in seconds while the radio is kept in continuous wave mode
 * \param [in] frequency   RF frequency to be set.
 * \param [in] power       RF output power to be set.
 * \retval status          Status of the operation.
 */
static LoRaMacStatus_t SetTxContinuousWave( uint16_t timeout, uint32_t frequency, uint8_t power );
#endif /* LORAMAC_VERSION */
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
/*!
 * \brief Converts a second based Rejoin Cycle base in the from timer module required format.
 *
 * \param [IN]  rejoinCycleTime     The time in second
 * \param [out] timeInMiliSec       The time in second
 * \retval status          Status of the operation.
 */
static bool ConvertRejoinCycleTime( uint32_t rejoinCycleTime, uint32_t* timeInMiliSec );

/*!
 * \brief Checks if it's required to send a Rejoin (Type 0) request.
 *
 * \retval [false: Rejoin not required, true: Rejoin required]
 */
static bool IsReJoin0Required( void );
#endif /* LORAMAC_VERSION */

/*!
 * \brief Resets MAC specific parameters to default
 *
 * \param [in] isRejoin             Reset activation or not.
 */
static void ResetMacParameters( bool isRejoin );

/*!
 * \brief Initializes and opens the reception window
 *
 * \param [in] rxTimer  Window timer to be topped.
 * \param [in] rxConfig Window parameters to be setup
 */
static void RxWindowSetup( TimerEvent_t* rxTimer, RxConfigParams_t* rxConfig );

/*!
 * \brief Opens up a continuous RX C window. This is used for
 *        class c devices.
 */
static void OpenContinuousRxCWindow( void );

/*!
 * \brief   Restoring of internal module contexts
 *
 * \details This function allows to restore module contexts by a given pointer.
 *
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 */
static LoRaMacStatus_t RestoreNvmData( void );

/*!
 * \brief   Determines the frame type
 *
 * \param [in] macMsg Data message object
 *
 * \param [out] fType Frame type
 *
 * \retval  LoRaMacStatus_t Status of the operation. Possible returns are:
 *          returns are:
 *          \ref LORAMAC_STATUS_OK,
 *          \ref LORAMAC_STATUS_PARAMETER_INVALID,
 */
static LoRaMacStatus_t DetermineFrameType( LoRaMacMessageData_t* macMsg, FType_t* fType );

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
/*!
 * \brief Verifies, if the retransmission counter has reached the limit
 *
 * \param [in] counter Current retransmission counter
 * \param [in] limit Retransmission counter limit
 *
 * \retval Returns true if the number of retransmissions have reached the limit.
 */
static bool CheckRetrans( uint8_t counter, uint8_t limit );
#endif /* LORAMAC_VERSION */

/*!
 * \brief Checks if the retransmission should be stopped in case of a unconfirmed uplink
 *
 * \retval Returns true if it should be stopped.
 */
static bool CheckRetransUnconfirmedUplink( void );

/*!
 * \brief Checks if the retransmission should be stopped in case of a confirmed uplink
 *
 * \retval Returns true it should be stopped.
 */
static bool CheckRetransConfirmedUplink( void );

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
/*!
 * \brief Increases the ADR ack counter. Takes the maximum
 *        value into account.
 *
 * \param [in] counter Current counter value.
 *
 * \retval Returns the next counter value.
 */
static uint32_t IncreaseAdrAckCounter( uint32_t counter );
#endif /* LORAMAC_VERSION */

/*!
 * \brief Stops the uplink retransmission
 *
 * \retval Returns true if successful.
 */
static bool StopRetransmission( void );

/*!
 * \brief Calls the MacProcessNotify callback to indicate that a LoRaMacProcess call is pending
 */
static void OnMacProcessNotify( void );

/*!
 * \brief Calls the callback to indicate that a context changed
 */
static void CallNvmDataChangeCallback( uint16_t notifyFlags );

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
/*!
 * \brief Handles the ACK retries algorithm.
 *        Increments the re-tries counter up until the specified number of
 *        trials or the allowed maximum. Decrease the uplink datarate every 2
 *        trials.
 */
static void AckTimeoutRetriesProcess( void );

/*!
 * \brief Finalizes the ACK retries algorithm.
 *        If no ACK is received restores the default channels
 */
static void AckTimeoutRetriesFinalize( void );
#endif /* LORAMAC_VERSION */

/*!
 * \brief Verifies if a request is pending currently
 *
 * \retval 1: Request pending, 0: request not pending
 */
static uint8_t IsRequestPending( void );

/*!
 * \brief Enabled the possibility to perform requests
 *
 * \param [in] requestState Request permission state
 */
static void LoRaMacEnableRequests( LoRaMacRequestHandling_t requestState );

/*!
 * \brief This function verifies if a RX abort occurred
 */
static void LoRaMacCheckForRxAbort( void );

/*!
 * \brief This function verifies if a beacon acquisition MLME
 *        request was pending
 *
 * \retval 1: Request pending, 0: no request pending
 */
static uint8_t LoRaMacCheckForBeaconAcquisition( void );

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
/*!
 * \brief Returns true, if the device must apply the minimum datarate
 *
 * \param [in] adr ADR status bit
 *
 * \param [in] activation Activation type of the device
 *
 * \param [in] datarateChanged Set to true, if the datarate was changed
 *                             with the LinkAdrReq.
 */
static bool CheckForMinimumAbpDatarate( bool adr, ActivationType_t activation, bool datarateChanged );
#endif /* LORAMAC_VERSION */

/*!
 * \brief This function handles join request
 */
static void LoRaMacHandleMlmeRequest( void );

/*!
 * \brief This function handles mcps request
 */
static void LoRaMacHandleMcpsRequest( void );

/*!
 * \brief This function handles callback events for requests
 */
static void LoRaMacHandleRequestEvents( void );

/*!
 * \brief This function handles callback events for indications
 */
static void LoRaMacHandleIndicationEvents( void );

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
/*!
 * \brief This function handles events for re-join procedure
 */
static void LoRaMacHandleRejoinEvents( void );
#endif /* LORAMAC_VERSION */

/*!
 * \brief This function handles callback events for NVM updates
 *
 * \param [in] nvmData Data structure containing NVM data.
 */
static void LoRaMacHandleNvm( LoRaMacNvmData_t* nvmData );

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
/*!
 * \brief This function verifies if the response timeout has been elapsed. If
 *        this is the case, the status of Nvm.MacGroup1.SrvAckRequested will be
 *        reset.
 *
 * \param [in] timeoutInMs Timeout [ms] to be compared.
 *
 * \param [in] startTimeInMs Start time [ms] used as a base. If set to 0,
 *                           no comparison will be done.
 *
 * \retval true: Response timeout has been elapsed, false: Response timeout
 *         has not been elapsed or startTimeInMs is 0.
 */
static bool LoRaMacHandleResponseTimeout( TimerTime_t timeoutInMs, TimerTime_t startTimeInMs );
#endif /* LORAMAC_VERSION */

/*!
 * Structure used to store the radio Tx event data
 */
typedef struct
{
    TimerTime_t CurTime;
}TxDoneParams_t;

static TxDoneParams_t TxDoneParams;

/*!
 * Structure used to store the radio Rx event data
 */
typedef struct
{
    TimerTime_t LastRxDone;
    uint8_t *Payload;
    uint16_t Size;
    int16_t Rssi;
    int8_t Snr;
}RxDoneParams_t;

static RxDoneParams_t RxDoneParams;

static void OnRadioTxDone( void )
{
    TxDoneParams.CurTime = TimerGetCurrentTime( );
    MacCtx.LastTxSysTime = SysTimeGet( );

    LoRaMacRadioEvents.Events.TxDone = 1;

    OnMacProcessNotify( );
    MW_LOG(TS_ON, VLEVEL_M, "MAC txDone\r\n" );
}

static void OnRadioRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    RxDoneParams.LastRxDone = TimerGetCurrentTime( );
    RxDoneParams.Payload = payload;
    RxDoneParams.Size = size;
    RxDoneParams.Rssi = rssi;
    RxDoneParams.Snr = snr;

    LoRaMacRadioEvents.Events.RxDone = 1;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    LoRaMacRadioEvents.Events.RxProcessPending = 1;
#endif /* LORAMAC_VERSION */

    OnMacProcessNotify( );
    MW_LOG(TS_ON, VLEVEL_M, "MAC rxDone\r\n" );
}

static void OnRadioTxTimeout( void )
{
    LoRaMacRadioEvents.Events.TxTimeout = 1;

    OnMacProcessNotify( );
    MW_LOG(TS_ON, VLEVEL_M, "MAC txTimeOut\r\n" );
}

static void OnRadioRxError( void )
{
    LoRaMacRadioEvents.Events.RxError = 1;

    OnMacProcessNotify( );
}

static void OnRadioRxTimeout( void )
{
    LoRaMacRadioEvents.Events.RxTimeout = 1;

    OnMacProcessNotify( );
    MW_LOG(TS_ON, VLEVEL_M, "MAC rxTimeOut\r\n" );
}

static void UpdateRxSlotIdleState( void )
{
    if( Nvm.MacGroup2.DeviceClass != CLASS_C )
    {
        MacCtx.RxSlot = RX_SLOT_NONE;
    }
    else
    {
        MacCtx.RxSlot = RX_SLOT_WIN_CLASS_C;
    }
}

static void ProcessRadioTxDone( void )
{
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;
    SetBandTxDoneParams_t txDone;

    if( Nvm.MacGroup2.DeviceClass != CLASS_C )
    {
        Radio.Sleep( );
    }
#if ( !defined(DISABLE_LORAWAN_RX_WINDOW) || (DISABLE_LORAWAN_RX_WINDOW == 0) )
    // Setup timers
    CRITICAL_SECTION_BEGIN( );
    uint32_t offset = TimerGetCurrentTime( ) - TxDoneParams.CurTime;
    TimerSetValue( &MacCtx.RxWindowTimer1, MacCtx.RxWindow1Delay - offset );
    TimerStart( &MacCtx.RxWindowTimer1 );
    TimerSetValue( &MacCtx.RxWindowTimer2, MacCtx.RxWindow2Delay - offset );
    TimerStart( &MacCtx.RxWindowTimer2 );
    CRITICAL_SECTION_END( );
#else
    if (Nvm.MacGroup2.NetworkActivation == ACTIVATION_TYPE_NONE)
    {
        // Setup timers
        CRITICAL_SECTION_BEGIN( );
        uint32_t offset = TimerGetCurrentTime( ) - TxDoneParams.CurTime;
        TimerSetValue( &MacCtx.RxWindowTimer1, MacCtx.RxWindow1Delay - offset );
        TimerStart( &MacCtx.RxWindowTimer1 );
        TimerSetValue( &MacCtx.RxWindowTimer2, MacCtx.RxWindow2Delay - offset );
        TimerStart( &MacCtx.RxWindowTimer2 );
        CRITICAL_SECTION_END( );
    }
    else
    {
        MacCtx.MacState |= LORAMAC_RX_ABORT;
        MacCtx.MacFlags.Bits.MacDone = 1;
    }
#endif /* DISABLE_LORAWAN_RX_WINDOW */

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    if( ( Nvm.MacGroup2.DeviceClass == CLASS_C ) || ( MacCtx.NodeAckRequested == true ) )
    {
        getPhy.Attribute = PHY_ACK_TIMEOUT;
        phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
        TimerSetValue( &MacCtx.AckTimeoutTimer, MacCtx.RxWindow2Delay + phyParam.Value );
        TimerStart( &MacCtx.AckTimeoutTimer );
    }
    else if( MacCtx.NodeAckRequested == false )
    {
        MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_OK;
    }
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    if( MacCtx.NodeAckRequested == true )
    {
        getPhy.Attribute = PHY_RETRANSMIT_TIMEOUT;
        phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
        TimerSetValue( &MacCtx.RetransmitTimeoutTimer, MacCtx.RxWindow2Delay + phyParam.Value );
        TimerStart( &MacCtx.RetransmitTimeoutTimer );
    }
    else
    {
        // Transmission successful, setup status directly
        MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_OK;
    }
#endif /* LORAMAC_VERSION */

    // Update Aggregated last tx done time
    Nvm.MacGroup1.LastTxDoneTime = TxDoneParams.CurTime;

    // Update last tx done time for the current channel
    txDone.Channel = MacCtx.Channel;
    txDone.LastTxDoneTime = TxDoneParams.CurTime;
    txDone.ElapsedTimeSinceStartUp = SysTimeSub( SysTimeGetMcuTime( ), Nvm.MacGroup2.InitializationTime );
    txDone.LastTxAirTime = MacCtx.TxTimeOnAir;
    txDone.Joined  = true;
    if( Nvm.MacGroup2.NetworkActivation == ACTIVATION_TYPE_NONE )
    {
        txDone.Joined  = false;
    }

    RegionSetBandTxDone( Nvm.MacGroup2.Region, &txDone );
}

static void PrepareRxDoneAbort( void )
{
    MacCtx.MacState |= LORAMAC_RX_ABORT;

    if( MacCtx.NodeAckRequested == true )
    {
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
        OnAckTimeoutTimerEvent( NULL );
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
        OnRetransmitTimeoutTimerEvent( NULL );
#endif /* LORAMAC_VERSION */
    }

    MacCtx.MacFlags.Bits.McpsInd = 1;
    MacCtx.MacFlags.Bits.MacDone = 1;

    UpdateRxSlotIdleState( );
}

static void ProcessRadioRxDone( void )
{
    LoRaMacHeader_t macHdr;
    ApplyCFListParams_t applyCFList;
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;
    LoRaMacCryptoStatus_t macCryptoStatus = LORAMAC_CRYPTO_ERROR;

    LoRaMacMessageData_t macMsgData;
    LoRaMacMessageJoinAccept_t macMsgJoinAccept;
    uint8_t *payload = RxDoneParams.Payload;
    uint16_t size = RxDoneParams.Size;
    int16_t rssi = RxDoneParams.Rssi;
    int8_t snr = RxDoneParams.Snr;

    uint8_t pktHeaderLen = 0;

    uint32_t downLinkCounter = 0;
    uint32_t address = Nvm.MacGroup2.DevAddr;
    uint8_t multicast = 0;
    AddressIdentifier_t addrID = UNICAST_DEV_ADDR;
    FCntIdentifier_t fCntID;
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    uint8_t macCmdPayload[2] = { 0 };
#endif /* LORAMAC_VERSION */
    Mlme_t joinType = MLME_JOIN;

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    LoRaMacRadioEvents.Events.RxProcessPending = 0;
#endif /* LORAMAC_VERSION */

    MacCtx.McpsConfirm.AckReceived = false;
    MacCtx.RxStatus.Rssi = rssi;
    MacCtx.RxStatus.Snr = snr;
    MacCtx.RxStatus.RxSlot = MacCtx.RxSlot;
    MacCtx.McpsIndication.Port = 0;
    MacCtx.McpsIndication.Multicast = 0;
    MacCtx.McpsIndication.IsUplinkTxPending = 0;
    MacCtx.McpsIndication.Buffer = NULL;
    MacCtx.McpsIndication.BufferSize = 0;
    MacCtx.McpsIndication.RxData = false;
    MacCtx.McpsIndication.AckReceived = false;
    MacCtx.McpsIndication.DownLinkCounter = 0;
    MacCtx.McpsIndication.McpsIndication = MCPS_UNCONFIRMED;
    MacCtx.McpsIndication.DevAddress = 0;
    MacCtx.McpsIndication.DeviceTimeAnsReceived = false;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    MacCtx.McpsIndication.ResponseTimeout = 0;
#endif /* LORAMAC_VERSION */

    Radio.Sleep( );

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    TimerStop( &MacCtx.RxWindowTimer2 );
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    if( MacCtx.RxStatus.RxSlot == RX_SLOT_WIN_1 )
    {
        TimerStop( &MacCtx.RxWindowTimer2 );
    }
#endif /* LORAMAC_VERSION */

    // This function must be called even if we are not in class b mode yet.
    if( LoRaMacClassBRxBeacon( payload, size ) == true )
    {
        MacCtx.MlmeIndication.BeaconInfo.Rssi = rssi;
        MacCtx.MlmeIndication.BeaconInfo.Snr = snr;
        return;
    }
    // Check if we expect a ping or a multicast slot.
    if( Nvm.MacGroup2.DeviceClass == CLASS_B )
    {
        if( LoRaMacClassBIsPingExpected( ) == true )
        {
            LoRaMacClassBSetPingSlotState( PINGSLOT_STATE_CALC_PING_OFFSET );
            LoRaMacClassBPingSlotTimerEvent( NULL );
            MacCtx.RxStatus.RxSlot = RX_SLOT_WIN_CLASS_B_PING_SLOT;
        }
        else if( LoRaMacClassBIsMulticastExpected( ) == true )
        {
            LoRaMacClassBSetMulticastSlotState( PINGSLOT_STATE_CALC_PING_OFFSET );
            LoRaMacClassBMulticastSlotTimerEvent( NULL );
            MacCtx.RxStatus.RxSlot = RX_SLOT_WIN_CLASS_B_MULTICAST_SLOT;
        }
    }

    // Abort on empty radio frames
    if( size == 0 )
    {
        MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
        PrepareRxDoneAbort( );
        return;
    }

    macHdr.Value = payload[pktHeaderLen++];

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    // Accept frames of LoRaWAN Major Version 1 only
    if( macHdr.Bits.Major != 0 )
    {
        MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
        PrepareRxDoneAbort( );
        return;
    }
#endif /* LORAMAC_VERSION */

    switch( macHdr.Bits.MType )
    {
        case FRAME_TYPE_JOIN_ACCEPT:
        {
            uint8_t joinEui[SE_EUI_SIZE];
            // Check if the received frame size is valid
            if( size < LORAMAC_JOIN_ACCEPT_FRAME_MIN_SIZE )
            {
                MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                PrepareRxDoneAbort( );
                return;
            }
            macMsgJoinAccept.Buffer = payload;
            macMsgJoinAccept.BufSize = size;

            // Abort in case if the device is already joined and no rejoin request is ongoing.
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
            if( ( Nvm.MacGroup2.NetworkActivation != ACTIVATION_TYPE_NONE ) && ( Nvm.MacGroup2.IsRejoinAcceptPending == false ) )
#else
            if( Nvm.MacGroup2.NetworkActivation != ACTIVATION_TYPE_NONE )
#endif /* LORAMAC_VERSION */
            {
                MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                PrepareRxDoneAbort( );
                return;
            }

            SecureElementGetJoinEui( joinEui );
            macCryptoStatus = LoRaMacCryptoHandleJoinAccept( JOIN_REQ, joinEui, &macMsgJoinAccept );

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
            if( LORAMAC_CRYPTO_SUCCESS != macCryptoStatus )
            {
                macCryptoStatus = LoRaMacCryptoHandleJoinAccept( REJOIN_REQ_0, joinEui, &macMsgJoinAccept );
                joinType = MLME_REJOIN_0;
            }

            if( LORAMAC_CRYPTO_SUCCESS != macCryptoStatus )
            {
                macCryptoStatus = LoRaMacCryptoHandleJoinAccept( REJOIN_REQ_1, joinEui, &macMsgJoinAccept );
                joinType = MLME_REJOIN_1;
            }

            if( LORAMAC_CRYPTO_SUCCESS != macCryptoStatus )
            {
                macCryptoStatus = LoRaMacCryptoHandleJoinAccept( REJOIN_REQ_2, joinEui, &macMsgJoinAccept );
                joinType = MLME_REJOIN_2;
            }
#endif /* LORAMAC_VERSION */

            VerifyParams_t verifyRxDr;
            bool rxDrValid = false;
            verifyRxDr.DatarateParams.Datarate = macMsgJoinAccept.DLSettings.Bits.RX2DataRate;
            verifyRxDr.DatarateParams.DownlinkDwellTime = Nvm.MacGroup2.MacParams.DownlinkDwellTime;
            rxDrValid = RegionVerify( Nvm.MacGroup2.Region, &verifyRxDr, PHY_RX_DR );

            if( ( LORAMAC_CRYPTO_SUCCESS == macCryptoStatus ) && ( rxDrValid == true ) )
            {
                // Network ID
                Nvm.MacGroup2.NetID = ( uint32_t ) macMsgJoinAccept.NetID[0];
                Nvm.MacGroup2.NetID |= ( ( uint32_t ) macMsgJoinAccept.NetID[1] << 8 );
                Nvm.MacGroup2.NetID |= ( ( uint32_t ) macMsgJoinAccept.NetID[2] << 16 );

                // Device Address
                Nvm.MacGroup2.DevAddr = macMsgJoinAccept.DevAddr;
                // Update NVM DevAddrOTAA with network value
                SecureElementSetDevAddr( ACTIVATION_TYPE_OTAA, Nvm.MacGroup2.DevAddr );

                // DLSettings
                Nvm.MacGroup2.MacParams.Rx1DrOffset = macMsgJoinAccept.DLSettings.Bits.RX1DRoffset;
                Nvm.MacGroup2.MacParams.Rx2Channel.Datarate = macMsgJoinAccept.DLSettings.Bits.RX2DataRate;
                Nvm.MacGroup2.MacParams.RxCChannel.Datarate = macMsgJoinAccept.DLSettings.Bits.RX2DataRate;

                // RxDelay
                Nvm.MacGroup2.MacParams.ReceiveDelay1 = macMsgJoinAccept.RxDelay;
                if( Nvm.MacGroup2.MacParams.ReceiveDelay1 == 0 )
                {
                    Nvm.MacGroup2.MacParams.ReceiveDelay1 = 1;
                }
                Nvm.MacGroup2.MacParams.ReceiveDelay1 *= 1000;
                Nvm.MacGroup2.MacParams.ReceiveDelay2 = Nvm.MacGroup2.MacParams.ReceiveDelay1 + 1000;

                // Reset NbTrans to default value
                Nvm.MacGroup2.MacParams.ChannelsNbTrans = 1;

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
                // Is Networkserver's LoRaWAN Version before 1.1.0 ?
                if( macMsgJoinAccept.DLSettings.Bits.OptNeg == 0 )
                {
                    Nvm.MacGroup2.Version.Value = LORAMAC_FALLBACK_VERSION;
                }
                else
                {
                    Nvm.MacGroup2.Version.Value = LORAMAC_VERSION;
                }
#else
                Nvm.MacGroup2.Version.Fields.Minor = 0;
#endif /* LORAMAC_VERSION */

                // Apply CF list
                applyCFList.Payload = macMsgJoinAccept.CFList;
                // Size of the regular payload is 12. Plus 1 byte MHDR and 4 bytes MIC
                applyCFList.Size = size - 17;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
                // Apply the last tx channel
                applyCFList.JoinChannel = MacCtx.Channel;
#endif /* LORAMAC_VERSION */

                RegionApplyCFList( Nvm.MacGroup2.Region, &applyCFList );

                Nvm.MacGroup2.NetworkActivation = ACTIVATION_TYPE_OTAA;

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
                // Add a RekeyInd MAC command to confirm the security key update.
                if( Nvm.MacGroup2.Version.Fields.Minor >= 1 )
                {
                    Nvm.MacGroup1.RekeyIndUplinksCounter = 0;
                    macCmdPayload[0] = Nvm.MacGroup2.Version.Fields.Minor;
                    LoRaMacCommandsAddCmd( MOTE_MAC_REKEY_IND, macCmdPayload, 1 );
                }
#endif /* LORAMAC_VERSION */

                // MLME handling
                if( LoRaMacConfirmQueueIsCmdActive( joinType ) == true )
                {
                    LoRaMacConfirmQueueSetStatus( LORAMAC_EVENT_INFO_STATUS_OK, joinType );
                }

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
                // Rejoin handling
                if( Nvm.MacGroup2.IsRejoinAcceptPending == true )
                {
                    Nvm.MacGroup2.IsRejoinAcceptPending = false;

                    // Stop in any case the ForceRejoinReqCycleTimer
                    TimerStop( &MacCtx.ForceRejoinReqCycleTimer );
                }

                // Reset MAC parameters for specific re-join types
                if( ( joinType == MLME_REJOIN_0 ) || ( joinType == MLME_REJOIN_1 ) )
                {
                    ResetMacParameters( true );
                }
#endif /* LORAMAC_VERSION */
            }
            else
            {
                // MLME handling
                if( LoRaMacConfirmQueueIsCmdActive( MLME_JOIN ) == true )
                {
                    LoRaMacConfirmQueueSetStatus( LORAMAC_EVENT_INFO_STATUS_JOIN_FAIL, MLME_JOIN );
                }
            }

            break;
        }
        case FRAME_TYPE_DATA_CONFIRMED_DOWN:
            MacCtx.McpsIndication.McpsIndication = MCPS_CONFIRMED;
            // Intentional fall through
        case FRAME_TYPE_DATA_UNCONFIRMED_DOWN:
            // Check if the received payload size is valid
            getPhy.UplinkDwellTime = Nvm.MacGroup2.MacParams.DownlinkDwellTime;
            getPhy.Datarate = MacCtx.McpsIndication.RxDatarate;
            getPhy.Attribute = PHY_MAX_PAYLOAD;

            // Get the maximum payload length
            if( Nvm.MacGroup2.MacParams.RepeaterSupport == true )
            {
                getPhy.Attribute = PHY_MAX_PAYLOAD_REPEATER;
            }

            phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
            if( ( MAX( 0, ( int16_t )( ( int16_t ) size - ( int16_t ) LORAMAC_FRAME_PAYLOAD_OVERHEAD_SIZE ) ) > ( int16_t )phyParam.Value ) ||
                ( size < LORAMAC_FRAME_PAYLOAD_MIN_SIZE ) )
            {
                MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                PrepareRxDoneAbort( );
                return;
            }
            macMsgData.Buffer = payload;
            macMsgData.BufSize = size;
            macMsgData.FRMPayload = MacCtx.RxPayload;
            macMsgData.FRMPayloadSize = LORAMAC_PHY_MAXPAYLOAD;

            if( LORAMAC_PARSER_SUCCESS != LoRaMacParserData( &macMsgData ) )
            {
                MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                PrepareRxDoneAbort( );
                return;
            }

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
            // Handle Class B
            // Check if we expect a ping or a multicast slot.
            if( Nvm.MacGroup2.DeviceClass == CLASS_B )
            {
                if( LoRaMacClassBIsPingExpected( ) == true )
                {
                    LoRaMacClassBSetPingSlotState( PINGSLOT_STATE_CALC_PING_OFFSET );
                    LoRaMacClassBPingSlotTimerEvent( NULL );
                    MacCtx.RxStatus.RxSlot = RX_SLOT_WIN_CLASS_B_PING_SLOT;
                    LoRaMacClassBSetFPendingBit( macMsgData.FHDR.DevAddr, ( uint8_t ) macMsgData.FHDR.FCtrl.Bits.FPending );
                }
                else if( LoRaMacClassBIsMulticastExpected( ) == true )
                {
                    LoRaMacClassBSetMulticastSlotState( PINGSLOT_STATE_CALC_PING_OFFSET );
                    LoRaMacClassBMulticastSlotTimerEvent( NULL );
                    MacCtx.RxStatus.RxSlot = RX_SLOT_WIN_CLASS_B_MULTICAST_SLOT;
                    LoRaMacClassBSetFPendingBit( macMsgData.FHDR.DevAddr, ( uint8_t ) macMsgData.FHDR.FCtrl.Bits.FPending );
                }
            }
#endif /* LORAMAC_VERSION */

            // Store device address
            MacCtx.McpsIndication.DevAddress = macMsgData.FHDR.DevAddr;

            FType_t fType;
            if( LORAMAC_STATUS_OK != DetermineFrameType( &macMsgData, &fType ) )
            {
                MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                PrepareRxDoneAbort( );
                return;
            }

            //Check if it is a multicast message
            multicast = 0;
            downLinkCounter = 0;
            for( uint8_t i = 0; i < LORAMAC_MAX_MC_CTX; i++ )
            {
                if( ( Nvm.MacGroup2.MulticastChannelList[i].ChannelParams.Address == macMsgData.FHDR.DevAddr ) &&
                    ( Nvm.MacGroup2.MulticastChannelList[i].ChannelParams.IsEnabled == true ) )
                {
                    multicast = 1;
                    addrID = Nvm.MacGroup2.MulticastChannelList[i].ChannelParams.GroupID;
                    downLinkCounter = *( Nvm.MacGroup2.MulticastChannelList[i].DownLinkCounter );
                    address = Nvm.MacGroup2.MulticastChannelList[i].ChannelParams.Address;
                    if( Nvm.MacGroup2.DeviceClass == CLASS_C )
                    {
                        MacCtx.RxStatus.RxSlot = RX_SLOT_WIN_CLASS_C_MULTICAST;
                    }
                    break;
                }
            }

            // Filter messages according to multicast downlink exceptions
            if( ( multicast == 1 ) && ( ( fType != FRAME_TYPE_D ) ||
                                        ( macMsgData.FHDR.FCtrl.Bits.Ack != 0 ) ||
                                        ( macMsgData.FHDR.FCtrl.Bits.AdrAckReq != 0 ) ) )
            {
                MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                PrepareRxDoneAbort( );
                return;
            }

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
            // Get maximum allowed counter difference
            getPhy.Attribute = PHY_MAX_FCNT_GAP;
            phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );

            // Get downlink frame counter value
            macCryptoStatus = GetFCntDown( addrID, fType, &macMsgData, Nvm.MacGroup2.Version, phyParam.Value, &fCntID, &downLinkCounter );
            if( macCryptoStatus != LORAMAC_CRYPTO_SUCCESS )
            {
                if( macCryptoStatus == LORAMAC_CRYPTO_FAIL_FCNT_DUPLICATED )
                {
                    // Catch the case of repeated downlink frame counter
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_DOWNLINK_REPEATED;
                    if( ( Nvm.MacGroup2.Version.Fields.Minor == 0 ) && ( macHdr.Bits.MType == FRAME_TYPE_DATA_CONFIRMED_DOWN ) && ( Nvm.MacGroup1.LastRxMic == macMsgData.MIC ) )
                    {
                        Nvm.MacGroup1.SrvAckRequested = true;
                    }
                }
                else if( macCryptoStatus == LORAMAC_CRYPTO_FAIL_MAX_GAP_FCNT )
                {
                    // Lost too many frames
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_DOWNLINK_TOO_MANY_FRAMES_LOSS;
                }
                else
                {
                    // Other errors
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                }
                MacCtx.McpsIndication.DownLinkCounter = downLinkCounter;
                MacCtx.MlmeIndication.DownLinkCounter = downLinkCounter;
                PrepareRxDoneAbort( );
                return;
            }

            macCryptoStatus = LoRaMacCryptoUnsecureMessage( addrID, address, fCntID, downLinkCounter, &macMsgData );
            if( macCryptoStatus != LORAMAC_CRYPTO_SUCCESS )
            {
                if( macCryptoStatus == LORAMAC_CRYPTO_FAIL_ADDRESS )
                {
                    // We are not the destination of this frame.
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ADDRESS_FAIL;
                }
                else
                {
                    // MIC calculation fail
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_MIC_FAIL;
                }
                PrepareRxDoneAbort( );
                return;
            }
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
            // Get downlink frame counter value
            macCryptoStatus = GetFCntDown( addrID, fType, &macMsgData, Nvm.MacGroup2.Version, &fCntID, &downLinkCounter );
            if( macCryptoStatus != LORAMAC_CRYPTO_SUCCESS )
            {
                if( macCryptoStatus == LORAMAC_CRYPTO_FAIL_FCNT_DUPLICATED )
                {
                    // Catch the case of repeated downlink frame counter
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_DOWNLINK_REPEATED;
                }
                else
                {
                    // Other errors
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                }
                MacCtx.McpsIndication.DownLinkCounter = downLinkCounter;
                MacCtx.MlmeIndication.DownLinkCounter = downLinkCounter;
                PrepareRxDoneAbort( );
                return;
            }

            macCryptoStatus = LoRaMacCryptoUnsecureMessage( addrID, address, fCntID, downLinkCounter, &macMsgData );
            if( macCryptoStatus != LORAMAC_CRYPTO_SUCCESS )
            {
                if( macCryptoStatus == LORAMAC_CRYPTO_FAIL_ADDRESS )
                {
                    // We are not the destination of this frame.
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ADDRESS_FAIL;
                }
                else
                {
                    // MIC calculation fail
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_MIC_FAIL;
                }
                PrepareRxDoneAbort( );
                return;
            }
#endif /* LORAMAC_VERSION */

            MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_OK;
            MacCtx.McpsIndication.Multicast = multicast;
            MacCtx.McpsIndication.Buffer = NULL;
            MacCtx.McpsIndication.BufferSize = 0;
            MacCtx.McpsIndication.DownLinkCounter = downLinkCounter;
            MacCtx.MlmeIndication.DownLinkCounter = downLinkCounter;
            MacCtx.McpsIndication.AckReceived = macMsgData.FHDR.FCtrl.Bits.Ack;

            MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_OK;
            MacCtx.McpsConfirm.AckReceived = macMsgData.FHDR.FCtrl.Bits.Ack;

            // Reset ADR ACK Counter only, when RX1 or RX2 slot
            if( ( MacCtx.RxStatus.RxSlot == RX_SLOT_WIN_1 ) ||
                ( MacCtx.RxStatus.RxSlot == RX_SLOT_WIN_2 ) )
            {
                Nvm.MacGroup1.AdrAckCounter = 0;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
                Nvm.MacGroup2.DownlinkReceived = true;
#endif /* LORAMAC_VERSION */
            }

            // MCPS Indication and ack requested handling
            if( multicast == 1 )
            {
                MacCtx.McpsIndication.McpsIndication = MCPS_MULTICAST;
            }
            else
            {
                if( macHdr.Bits.MType == FRAME_TYPE_DATA_CONFIRMED_DOWN )
                {
                    Nvm.MacGroup1.SrvAckRequested = true;
                    if( Nvm.MacGroup2.Version.Fields.Minor == 0 )
                    {
                        Nvm.MacGroup1.LastRxMic = macMsgData.MIC;
                    }
                    MacCtx.McpsIndication.McpsIndication = MCPS_CONFIRMED;

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
                    // Handle response timeout for class c and class b downlinks
                    if( ( MacCtx.RxStatus.RxSlot != RX_SLOT_WIN_1 ) &&
                        ( MacCtx.RxStatus.RxSlot != RX_SLOT_WIN_2 ) )
                    {
                        // Calculate timeout
                        MacCtx.McpsIndication.ResponseTimeout = Nvm.MacGroup2.MacParams.RxBCTimeout;
                        MacCtx.ResponseTimeoutStartTime = RxDoneParams.LastRxDone;
                    }
#endif /* LORAMAC_VERSION */
                }
                else
                {
                    Nvm.MacGroup1.SrvAckRequested = false;
                    MacCtx.McpsIndication.McpsIndication = MCPS_UNCONFIRMED;
                }
            }

            // Set the pending status
            /* if( ( ( ( Nvm.MacGroup1.SrvAckRequested == true ) || ( macMsgData.FHDR.FCtrl.Bits.FPending > 0 ) ) && ( Nvm.MacGroup2.DeviceClass == CLASS_A ) ) ||
                ( MacCtx.McpsIndication.ResponseTimeout > 0 ) ) */
            if( ( ( Nvm.MacGroup1.SrvAckRequested == true ) || ( macMsgData.FHDR.FCtrl.Bits.FPending > 0 ) ) && ( Nvm.MacGroup2.DeviceClass == CLASS_A ) )
            {
                MacCtx.McpsIndication.IsUplinkTxPending = 1;
            }

            RemoveMacCommands( MacCtx.RxStatus.RxSlot, macMsgData.FHDR.FCtrl, MacCtx.McpsConfirm.McpsRequest );

            switch( fType )
            {
                case FRAME_TYPE_A:
                {  /* +----------+------+-------+--------------+
                    * | FOptsLen | Fopt | FPort |  FRMPayload  |
                    * +----------+------+-------+--------------+
                    * |    > 0   |   X  |  > 0  |       X      |
                    * +----------+------+-------+--------------+
                    */

                    // Decode MAC commands in FOpts field
                    ProcessMacCommands( macMsgData.FHDR.FOpts, 0, macMsgData.FHDR.FCtrl.Bits.FOptsLen, snr, MacCtx.RxStatus.RxSlot );
                    MacCtx.McpsIndication.Port = macMsgData.FPort;
                    MacCtx.McpsIndication.Buffer = macMsgData.FRMPayload;
                    MacCtx.McpsIndication.BufferSize = macMsgData.FRMPayloadSize;
                    MacCtx.McpsIndication.RxData = true;
                    break;
                }
                case FRAME_TYPE_B:
                {  /* +----------+------+-------+--------------+
                    * | FOptsLen | Fopt | FPort |  FRMPayload  |
                    * +----------+------+-------+--------------+
                    * |    > 0   |   X  |   -   |       -      |
                    * +----------+------+-------+--------------+
                    */

                    // Decode MAC commands in FOpts field
                    ProcessMacCommands( macMsgData.FHDR.FOpts, 0, macMsgData.FHDR.FCtrl.Bits.FOptsLen, snr, MacCtx.RxStatus.RxSlot );
                    MacCtx.McpsIndication.Port = macMsgData.FPort;
                    break;
                }
                case FRAME_TYPE_C:
                {  /* +----------+------+-------+--------------+
                    * | FOptsLen | Fopt | FPort |  FRMPayload  |
                    * +----------+------+-------+--------------+
                    * |    = 0   |   -  |  = 0  | MAC commands |
                    * +----------+------+-------+--------------+
                    */

                    // Decode MAC commands in FRMPayload
                    ProcessMacCommands( macMsgData.FRMPayload, 0, macMsgData.FRMPayloadSize, snr, MacCtx.RxStatus.RxSlot );
                    MacCtx.McpsIndication.Port = macMsgData.FPort;
                    break;
                }
                case FRAME_TYPE_D:
                {  /* +----------+------+-------+--------------+
                    * | FOptsLen | Fopt | FPort |  FRMPayload  |
                    * +----------+------+-------+--------------+
                    * |    = 0   |   -  |  > 0  |       X      |
                    * +----------+------+-------+--------------+
                    */

                    // No MAC commands just application payload
                    MacCtx.McpsIndication.Port = macMsgData.FPort;
                    MacCtx.McpsIndication.Buffer = macMsgData.FRMPayload;
                    MacCtx.McpsIndication.BufferSize = macMsgData.FRMPayloadSize;
                    MacCtx.McpsIndication.RxData = true;
                    break;
                }
                default:
                    MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
                    PrepareRxDoneAbort( );
                    break;
            }

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
            // Rejoin handling
            if( Nvm.MacGroup2.IsRejoinAcceptPending == true )
            {
                Nvm.MacGroup2.IsRejoinAcceptPending = false;

                // Stop in any case the ForceRejoinReqCycleTimer
                TimerStop( &MacCtx.ForceRejoinReqCycleTimer );

                // If the rejoin was triggered by MLME, set confirmation status
                if( MacCtx.MacFlags.Bits.MlmeReq == 1 )
                {
                    MacCtx.MlmeConfirm.Status = LORAMAC_EVENT_INFO_STATUS_OK;
                }
            }
#endif /* LORAMAC_VERSION */

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
            if( ( macMsgData.FPort == LORAMAC_CERT_FPORT ) && ( Nvm.MacGroup2.IsCertPortOn == false ) )
            { // Do not notify the upper layer of data reception on FPort LORAMAC_CERT_FPORT if the port
              // handling is disabled.
                MacCtx.McpsIndication.Port = macMsgData.FPort;
                MacCtx.McpsIndication.Buffer = NULL;
                MacCtx.McpsIndication.BufferSize = 0;
                MacCtx.McpsIndication.RxData = false;
            }
#endif /* LORAMAC_VERSION */

            // Provide always an indication, skip the callback to the user application,
            // in case of a confirmed downlink retransmission.
            MacCtx.MacFlags.Bits.McpsInd = 1;

            break;
        case FRAME_TYPE_PROPRIETARY:
            memcpy1( MacCtx.RxPayload, &payload[pktHeaderLen], size - pktHeaderLen );

            MacCtx.McpsIndication.McpsIndication = MCPS_PROPRIETARY;
            MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_OK;
            MacCtx.McpsIndication.Buffer = MacCtx.RxPayload;
            MacCtx.McpsIndication.BufferSize = size - pktHeaderLen;

            MacCtx.MacFlags.Bits.McpsInd = 1;
            break;
        default:
            MacCtx.McpsIndication.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
            PrepareRxDoneAbort( );
            break;
    }

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    // Verify if we need to disable the AckTimeoutTimer
    if( MacCtx.NodeAckRequested == true )
    {
        if( MacCtx.McpsConfirm.AckReceived == true )
        {
            OnAckTimeoutTimerEvent( NULL );
        }
    }
    else
    {
        if( Nvm.MacGroup2.DeviceClass == CLASS_C )
        {
            OnAckTimeoutTimerEvent( NULL );
        }
    }
    MacCtx.MacFlags.Bits.MacDone = 1;
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    // Verify if we need to disable the RetransmitTimeoutTimer
    // Only applies if downlink is received on Rx1 or Rx2 windows.
    if( ( MacCtx.RxStatus.RxSlot == RX_SLOT_WIN_1 ) ||
        ( MacCtx.RxStatus.RxSlot == RX_SLOT_WIN_2 ) )
    {
        if( MacCtx.NodeAckRequested == true )
        {
            if( MacCtx.McpsConfirm.AckReceived == true )
            {
                OnRetransmitTimeoutTimerEvent( NULL );
            }
        }
    }

    if( MacCtx.RxStatus.RxSlot != RX_SLOT_WIN_CLASS_C )
    {
        MacCtx.MacFlags.Bits.MacDone = 1;
    }
#endif /* LORAMAC_VERSION */

    UpdateRxSlotIdleState( );
}

static void ProcessRadioTxTimeout( void )
{
    if( Nvm.MacGroup2.DeviceClass != CLASS_C )
    {
        Radio.Sleep( );
    }
    UpdateRxSlotIdleState( );

    MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_TX_TIMEOUT;
    LoRaMacConfirmQueueSetStatusCmn( LORAMAC_EVENT_INFO_STATUS_TX_TIMEOUT );
    if( MacCtx.NodeAckRequested == true )
    {
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
        MacCtx.AckTimeoutRetry = true;
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
        MacCtx.RetransmitTimeoutRetry = true;
#endif /* LORAMAC_VERSION */
    }
    MacCtx.MacFlags.Bits.MacDone = 1;
}

static void HandleRadioRxErrorTimeout( LoRaMacEventInfoStatus_t rx1EventInfoStatus, LoRaMacEventInfoStatus_t rx2EventInfoStatus )
{
    bool classBRx = false;

    if( Nvm.MacGroup2.DeviceClass != CLASS_C )
    {
        Radio.Sleep( );
    }

    if( LoRaMacClassBIsBeaconExpected( ) == true )
    {
        LoRaMacClassBSetBeaconState( BEACON_STATE_TIMEOUT );
        LoRaMacClassBBeaconTimerEvent( NULL );
        classBRx = true;
    }
    if( Nvm.MacGroup2.DeviceClass == CLASS_B )
    {
        if( LoRaMacClassBIsPingExpected( ) == true )
        {
            LoRaMacClassBSetPingSlotState( PINGSLOT_STATE_CALC_PING_OFFSET );
            LoRaMacClassBPingSlotTimerEvent( NULL );
            classBRx = true;
        }
        if( LoRaMacClassBIsMulticastExpected( ) == true )
        {
            LoRaMacClassBSetMulticastSlotState( PINGSLOT_STATE_CALC_PING_OFFSET );
            LoRaMacClassBMulticastSlotTimerEvent( NULL );
            classBRx = true;
        }
    }

    if( classBRx == false )
    {
        if( MacCtx.RxSlot == RX_SLOT_WIN_1 )
        {
            if( MacCtx.NodeAckRequested == true )
            {
                MacCtx.McpsConfirm.Status = rx1EventInfoStatus;
            }
            LoRaMacConfirmQueueSetStatusCmn( rx1EventInfoStatus );

            if( TimerGetElapsedTime( Nvm.MacGroup1.LastTxDoneTime ) >= MacCtx.RxWindow2Delay )
            {
                TimerStop( &MacCtx.RxWindowTimer2 );
                MacCtx.MacFlags.Bits.MacDone = 1;
            }
        }
        else
        {
            if( MacCtx.NodeAckRequested == true )
            {
                MacCtx.McpsConfirm.Status = rx2EventInfoStatus;
            }
            LoRaMacConfirmQueueSetStatusCmn( rx2EventInfoStatus );

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
            if( Nvm.MacGroup2.DeviceClass != CLASS_C )
            {
                MacCtx.MacFlags.Bits.MacDone = 1;
            }
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
            MacCtx.MacFlags.Bits.MacDone = 1;
#endif /* LORAMAC_VERSION */
        }
    }

    UpdateRxSlotIdleState( );
}

static void ProcessRadioRxError( void )
{
    HandleRadioRxErrorTimeout( LORAMAC_EVENT_INFO_STATUS_RX1_ERROR, LORAMAC_EVENT_INFO_STATUS_RX2_ERROR );
}

static void ProcessRadioRxTimeout( void )
{
    HandleRadioRxErrorTimeout( LORAMAC_EVENT_INFO_STATUS_RX1_TIMEOUT, LORAMAC_EVENT_INFO_STATUS_RX2_TIMEOUT );
}

static void LoRaMacHandleIrqEvents( void )
{
    LoRaMacRadioEvents_t events;

    CRITICAL_SECTION_BEGIN( );
    events = LoRaMacRadioEvents;
    LoRaMacRadioEvents.Value = 0;
    CRITICAL_SECTION_END( );

    if( events.Value != 0 )
    {
        if( events.Events.TxDone == 1 )
        {
            ProcessRadioTxDone( );
        }
        if( events.Events.RxDone == 1 )
        {
            ProcessRadioRxDone( );
        }
        if( events.Events.TxTimeout == 1 )
        {
            ProcessRadioTxTimeout( );
        }
        if( events.Events.RxError == 1 )
        {
            ProcessRadioRxError( );
        }
        if( events.Events.RxTimeout == 1 )
        {
            ProcessRadioRxTimeout( );
        }
    }
}

bool LoRaMacIsBusy( void )
{
    if( MacCtx.MacState == LORAMAC_STOPPED )
    {
        return false;
    }

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    if( LoRaMacRadioEvents.Events.RxProcessPending == 1 )
    {
        return true;
    }
#endif /* LORAMAC_VERSION */

    if( ( MacCtx.MacState == LORAMAC_IDLE ) &&
        ( MacCtx.AllowRequests == LORAMAC_REQUEST_HANDLING_ON ) )
    {
        return false;
    }
    return true;
}

bool LoRaMacIsStopped( void )
{
    if( MacCtx.MacState == LORAMAC_STOPPED )
    {
        return true;
    }
    return false;
}

static void LoRaMacEnableRequests( LoRaMacRequestHandling_t requestState )
{
    MacCtx.AllowRequests = requestState;
}

static void LoRaMacHandleRequestEvents( void )
{
    // Handle events
    LoRaMacFlags_t reqEvents = MacCtx.MacFlags;

    if( MacCtx.MacState == LORAMAC_IDLE )
    {
        // Update event bits
        if( MacCtx.MacFlags.Bits.McpsReq == 1 )
        {
            MacCtx.MacFlags.Bits.McpsReq = 0;
        }

        if( MacCtx.MacFlags.Bits.MlmeReq == 1 )
        {
            MacCtx.MacFlags.Bits.MlmeReq = 0;
        }

        // Allow requests again
        LoRaMacEnableRequests( LORAMAC_REQUEST_HANDLING_ON );

        // Handle callbacks
        if( reqEvents.Bits.McpsReq == 1 )
        {
            MacCtx.MacPrimitives->MacMcpsConfirm( &MacCtx.McpsConfirm );
        }

        if( reqEvents.Bits.MlmeReq == 1 )
        {
            LoRaMacConfirmQueueHandleCb( &MacCtx.MlmeConfirm );
            if( LoRaMacConfirmQueueGetCnt( ) > 0 )
            {
                MacCtx.MacFlags.Bits.MlmeReq = 1;
            }
        }

        // Start beaconing again
        LoRaMacClassBResumeBeaconing( );

        // Procedure done. Reset variables.
        MacCtx.MacFlags.Bits.MacDone = 0;
    }
}

static void LoRaMacHandleIndicationEvents( void )
{
    // Handle MLME indication
    if( MacCtx.MacFlags.Bits.MlmeInd == 1 )
    {
        MacCtx.MacFlags.Bits.MlmeInd = 0;
        MacCtx.MacPrimitives->MacMlmeIndication( &MacCtx.MlmeIndication, &MacCtx.RxStatus );
    }

    // Handle MCPS indication
    if( MacCtx.MacFlags.Bits.McpsInd == 1 )
    {
        MacCtx.MacFlags.Bits.McpsInd = 0;
        MacCtx.MacPrimitives->MacMcpsIndication( &MacCtx.McpsIndication, &MacCtx.RxStatus );
    }
}

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
static void LoRaMacHandleRejoinEvents( void )
{
    if( MacCtx.MacState == LORAMAC_IDLE )
    {
        MlmeReq_t mlmeReq;
        if( IsReJoin0Required( ) == true )
        {
            mlmeReq.Type = MLME_REJOIN_0;
            LoRaMacMlmeRequest( &mlmeReq );
        }
        else if( Nvm.MacGroup2.IsRejoin0RequestQueued == true )
        {
            mlmeReq.Type = MLME_REJOIN_0;
            if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
            {
                Nvm.MacGroup2.IsRejoin0RequestQueued = false;
            }
        }
        else if( Nvm.MacGroup2.IsRejoin1RequestQueued == true )
        {
            mlmeReq.Type = MLME_REJOIN_1;
            if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
            {
                Nvm.MacGroup2.IsRejoin1RequestQueued = false;
            }
        }
        else if( Nvm.MacGroup2.IsRejoin2RequestQueued == true )
        {
            mlmeReq.Type = MLME_REJOIN_2;
            if( LoRaMacMlmeRequest( &mlmeReq ) == LORAMAC_STATUS_OK )
            {
                Nvm.MacGroup2.IsRejoin2RequestQueued = false;
            }
        }
    }
}
#endif /* LORAMAC_VERSION */

static void LoRaMacHandleMcpsRequest( void )
{
    // Handle MCPS uplinks
    if( MacCtx.MacFlags.Bits.McpsReq == 1 )
    {
        bool stopRetransmission = false;
        bool waitForRetransmission = false;

        if( ( MacCtx.McpsConfirm.McpsRequest == MCPS_UNCONFIRMED ) ||
            ( MacCtx.McpsConfirm.McpsRequest == MCPS_PROPRIETARY ) )
        {
            stopRetransmission = CheckRetransUnconfirmedUplink( );
        }
        else if( MacCtx.McpsConfirm.McpsRequest == MCPS_CONFIRMED )
        {
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
            if( MacCtx.AckTimeoutRetry == true )
            {
                stopRetransmission = CheckRetransConfirmedUplink( );

                if( Nvm.MacGroup2.Version.Fields.Minor == 0 )
                {
                    if( stopRetransmission == false )
                    {
                        AckTimeoutRetriesProcess( );
                    }
                    else
                    {
                        AckTimeoutRetriesFinalize( );
                    }
                }
            }
            else
            {
                waitForRetransmission = true;
            }
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
            if( MacCtx.RetransmitTimeoutRetry == true )
            {
                stopRetransmission = CheckRetransConfirmedUplink( );
            }
            else
            {
                waitForRetransmission = true;
            }
#endif /* LORAMAC_VERSION */
        }

        if( stopRetransmission == true )
        {// Stop retransmission
            TimerStop( &MacCtx.TxDelayedTimer );
            MacCtx.MacState &= ~LORAMAC_TX_DELAYED;
            StopRetransmission( );
        }
        else if( waitForRetransmission == false )
        {// Arrange further retransmission
            MacCtx.MacFlags.Bits.MacDone = 0;
            // Reset the state of the AckTimeout
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
            MacCtx.AckTimeoutRetry = false;
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
            MacCtx.RetransmitTimeoutRetry = false;
#endif /* LORAMAC_VERSION */
            // Sends the same frame again
            OnTxDelayedTimerEvent( NULL );
        }
    }
}

static void LoRaMacHandleMlmeRequest( void )
{
    // Handle join request
    if( MacCtx.MacFlags.Bits.MlmeReq == 1 )
    {
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
        if( ( LoRaMacConfirmQueueIsCmdActive( MLME_JOIN ) == true ) ||
            ( LoRaMacConfirmQueueIsCmdActive( MLME_REJOIN_0 ) == true ) ||
            ( LoRaMacConfirmQueueIsCmdActive( MLME_REJOIN_1 ) == true ) ||
            ( LoRaMacConfirmQueueIsCmdActive( MLME_REJOIN_2 ) == true ) )
        {
            MacCtx.ChannelsNbTransCounter = 0;
#else
        if( LoRaMacConfirmQueueIsCmdActive( MLME_JOIN ) == true )
        {
            if( LoRaMacConfirmQueueGetStatus( MLME_JOIN ) == LORAMAC_EVENT_INFO_STATUS_OK )
            {// Node joined successfully
                MacCtx.ChannelsNbTransCounter = 0;
            }
#endif /* LORAMAC_VERSION */
            MacCtx.MacState &= ~LORAMAC_TX_RUNNING;
        }
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
        else if( ( LoRaMacConfirmQueueIsCmdActive( MLME_TXCW ) == true ) ||
                 ( LoRaMacConfirmQueueIsCmdActive( MLME_TXCW_1 ) == true ) )
        {
            MacCtx.MacState &= ~LORAMAC_TX_RUNNING;
        }
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
        else if( LoRaMacConfirmQueueIsCmdActive( MLME_TXCW ) == true )
        {
            MacCtx.MacState &= ~LORAMAC_TX_RUNNING;
        }
#endif /* LORAMAC_VERSION */
    }
}

static uint8_t LoRaMacCheckForBeaconAcquisition( void )
{
    if( ( LoRaMacConfirmQueueIsCmdActive( MLME_BEACON_ACQUISITION ) == true ) &&
        ( MacCtx.MacFlags.Bits.McpsReq == 0 ) )
    {
        if( MacCtx.MacFlags.Bits.MlmeReq == 1 )
        {
            MacCtx.MacState &= ~LORAMAC_TX_RUNNING;
            return 0x01;
        }
    }
    return 0x00;
}

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
static bool CheckForMinimumAbpDatarate( bool adr, ActivationType_t activation, bool datarateChanged )
{
    if( ( adr == true ) &&
        ( activation == ACTIVATION_TYPE_ABP ) &&
        ( datarateChanged == false ) )
    {
        return true;
    }
    return false;
}
#endif /* LORAMAC_VERSION */

static void LoRaMacCheckForRxAbort( void )
{
    // A error occurs during receiving
    if( ( MacCtx.MacState & LORAMAC_RX_ABORT ) == LORAMAC_RX_ABORT )
    {
        MacCtx.MacState &= ~LORAMAC_RX_ABORT;
        MacCtx.MacState &= ~LORAMAC_TX_RUNNING;
    }
}

static void LoRaMacHandleNvm( LoRaMacNvmData_t* nvmData )
{
    uint32_t crc = 0;
    uint16_t notifyFlags = LORAMAC_NVM_NOTIFY_FLAG_NONE;

    if( MacCtx.MacState != LORAMAC_IDLE )
    {
        return;
    }

    // Crypto
    crc = Crc32( ( uint8_t* ) &nvmData->Crypto, sizeof( nvmData->Crypto ) -
                                                sizeof( nvmData->Crypto.Crc32 ) );
    if( crc != nvmData->Crypto.Crc32 )
    {
        nvmData->Crypto.Crc32 = crc;
        notifyFlags |= LORAMAC_NVM_NOTIFY_FLAG_CRYPTO;
    }

    // MacGroup1
    crc = Crc32( ( uint8_t* ) &nvmData->MacGroup1, sizeof( nvmData->MacGroup1 ) -
                                                   sizeof( nvmData->MacGroup1.Crc32 ) );
    if( crc != nvmData->MacGroup1.Crc32 )
    {
        nvmData->MacGroup1.Crc32 = crc;
        notifyFlags |= LORAMAC_NVM_NOTIFY_FLAG_MAC_GROUP1;
    }

    // MacGroup2
    crc = Crc32( ( uint8_t* ) &nvmData->MacGroup2, sizeof( nvmData->MacGroup2 ) -
                                                   sizeof( nvmData->MacGroup2.Crc32 ) );
    if( crc != nvmData->MacGroup2.Crc32 )
    {
        nvmData->MacGroup2.Crc32 = crc;
        notifyFlags |= LORAMAC_NVM_NOTIFY_FLAG_MAC_GROUP2;
    }

    // Secure Element
    crc = Crc32( ( uint8_t* ) &nvmData->SecureElement, sizeof( nvmData->SecureElement ) -
                                                       sizeof( nvmData->SecureElement.Crc32 ) );
    if( crc != nvmData->SecureElement.Crc32 )
    {
        nvmData->SecureElement.Crc32 = crc;
        notifyFlags |= LORAMAC_NVM_NOTIFY_FLAG_SECURE_ELEMENT;
    }

    // Region
    crc = Crc32( ( uint8_t* ) &nvmData->RegionGroup1, sizeof( nvmData->RegionGroup1 ) -
                                                sizeof( nvmData->RegionGroup1.Crc32 ) );
    if( crc != nvmData->RegionGroup1.Crc32 )
    {
        nvmData->RegionGroup1.Crc32 = crc;
        notifyFlags |= LORAMAC_NVM_NOTIFY_FLAG_REGION_GROUP1;
    }

    crc = Crc32( ( uint8_t* ) &nvmData->RegionGroup2, sizeof( nvmData->RegionGroup2 ) -
                                                sizeof( nvmData->RegionGroup2.Crc32 ) );
    if( crc != nvmData->RegionGroup2.Crc32 )
    {
        nvmData->RegionGroup2.Crc32 = crc;
        notifyFlags |= LORAMAC_NVM_NOTIFY_FLAG_REGION_GROUP2;
    }

    // ClassB
    crc = Crc32( ( uint8_t* ) &nvmData->ClassB, sizeof( nvmData->ClassB ) -
                                                sizeof( nvmData->ClassB.Crc32 ) );
    if( crc != nvmData->ClassB.Crc32 )
    {
        nvmData->ClassB.Crc32 = crc;
        notifyFlags |= LORAMAC_NVM_NOTIFY_FLAG_CLASS_B;
    }

    CallNvmDataChangeCallback( notifyFlags );
}

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
static bool LoRaMacHandleResponseTimeout( TimerTime_t timeoutInMs, TimerTime_t startTimeInMs )
{
    if( startTimeInMs != 0 )
    {
        TimerTime_t elapsedTime = TimerGetElapsedTime( startTimeInMs );
        if( elapsedTime > timeoutInMs )
        {
            Nvm.MacGroup1.SrvAckRequested = false;
            return true;
        }
    }
    return false;
}
#endif /* LORAMAC_VERSION */

void LoRaMacProcess( void )
{
    uint8_t noTx = false;

    LoRaMacHandleIrqEvents( );
    LoRaMacClassBProcess( );

    // MAC proceeded a state and is ready to check
    if( MacCtx.MacFlags.Bits.MacDone == 1 )
    {
        LoRaMacEnableRequests( LORAMAC_REQUEST_HANDLING_OFF );
        LoRaMacCheckForRxAbort( );

        // An error occurs during transmitting
        if( IsRequestPending( ) > 0 )
        {
            noTx |= LoRaMacCheckForBeaconAcquisition( );
        }

        if( noTx == 0x00 )
        {
            LoRaMacHandleMlmeRequest( );
            LoRaMacHandleMcpsRequest( );
        }
        LoRaMacHandleRequestEvents( );
        LoRaMacEnableRequests( LORAMAC_REQUEST_HANDLING_ON );
        MacCtx.MacFlags.Bits.NvmHandle = 1;
    }
    LoRaMacHandleIndicationEvents( );
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    LoRaMacHandleRejoinEvents( );
#endif /* LORAMAC_VERSION */

    if( MacCtx.RxSlot == RX_SLOT_WIN_CLASS_C )
    {
        OpenContinuousRxCWindow( );
    }
    if( MacCtx.MacFlags.Bits.NvmHandle == 1 )
    {
        MacCtx.MacFlags.Bits.NvmHandle = 0;
        LoRaMacHandleNvm( &Nvm );
    }
}

static void OnTxDelayedTimerEvent( void* context )
{
    TimerStop( &MacCtx.TxDelayedTimer );
    MacCtx.MacState &= ~LORAMAC_TX_DELAYED;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    if( LoRaMacHandleResponseTimeout( Nvm.MacGroup2.MacParams.RxBCTimeout,
                                      MacCtx.ResponseTimeoutStartTime ) == true )
    {
        // Skip retransmission
        return;
    }
#endif /* LORAMAC_VERSION */

    // Schedule frame, allow delayed frame transmissions
    switch( ScheduleTx( true ) )
    {
        case LORAMAC_STATUS_OK:
        case LORAMAC_STATUS_DUTYCYCLE_RESTRICTED:
        {
            break;
        }
        default:
        {
            // Stop retransmission attempt
            MacCtx.McpsConfirm.Datarate = Nvm.MacGroup1.ChannelsDatarate;
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
            MacCtx.McpsConfirm.NbRetries = MacCtx.AckTimeoutRetriesCounter;
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
            MacCtx.McpsConfirm.NbTrans = MacCtx.ChannelsNbTransCounter;
#endif /* LORAMAC_VERSION */
            MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_TX_DR_PAYLOAD_SIZE_ERROR;
            LoRaMacConfirmQueueSetStatusCmn( LORAMAC_EVENT_INFO_STATUS_TX_DR_PAYLOAD_SIZE_ERROR );
            StopRetransmission( );
            break;
        }
    }
}

static void OnRxWindow1TimerEvent( void* context )
{
    MacCtx.RxWindow1Config.Channel = MacCtx.Channel;
    MacCtx.RxWindow1Config.DrOffset = Nvm.MacGroup2.MacParams.Rx1DrOffset;
    MacCtx.RxWindow1Config.DownlinkDwellTime = Nvm.MacGroup2.MacParams.DownlinkDwellTime;
    MacCtx.RxWindow1Config.RepeaterSupport = Nvm.MacGroup2.MacParams.RepeaterSupport;
    MacCtx.RxWindow1Config.RxContinuous = false;
    MacCtx.RxWindow1Config.RxSlot = RX_SLOT_WIN_1;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    MacCtx.RxWindow1Config.NetworkActivation = Nvm.MacGroup2.NetworkActivation;
#endif /* LORAMAC_VERSION */

    RxWindowSetup( &MacCtx.RxWindowTimer1, &MacCtx.RxWindow1Config );
}

static void OnRxWindow2TimerEvent( void* context )
{
    // Check if we are processing Rx1 window.
    // If yes, we don't setup the Rx2 window.
    if( MacCtx.RxSlot == RX_SLOT_WIN_1 )
    {
        return;
    }
    MacCtx.RxWindow2Config.Channel = MacCtx.Channel;
    MacCtx.RxWindow2Config.Frequency = Nvm.MacGroup2.MacParams.Rx2Channel.Frequency;
    MacCtx.RxWindow2Config.DownlinkDwellTime = Nvm.MacGroup2.MacParams.DownlinkDwellTime;
    MacCtx.RxWindow2Config.RepeaterSupport = Nvm.MacGroup2.MacParams.RepeaterSupport;
    MacCtx.RxWindow2Config.RxContinuous = false;
    MacCtx.RxWindow2Config.RxSlot = RX_SLOT_WIN_2;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    MacCtx.RxWindow2Config.NetworkActivation = Nvm.MacGroup2.NetworkActivation;
#endif /* LORAMAC_VERSION */

    RxWindowSetup( &MacCtx.RxWindowTimer2, &MacCtx.RxWindow2Config );
}
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
static void OnAckTimeoutTimerEvent( void* context )
{
    TimerStop( &MacCtx.AckTimeoutTimer );

    if( MacCtx.NodeAckRequested == true )
    {
        MacCtx.AckTimeoutRetry = true;
    }
    if( Nvm.MacGroup2.DeviceClass == CLASS_C )
    {
        MacCtx.MacFlags.Bits.MacDone = 1;
    }
    OnMacProcessNotify( );
}

static LoRaMacCryptoStatus_t GetFCntDown( AddressIdentifier_t addrID, FType_t fType, LoRaMacMessageData_t* macMsg, Version_t lrWanVersion,
                                          uint16_t maxFCntGap, FCntIdentifier_t* fCntID, uint32_t* currentDown )
{
    if( ( macMsg == NULL ) || ( fCntID == NULL ) ||
        ( currentDown == NULL ) )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    // Determine the frame counter identifier and choose counter from FCntList
    switch( addrID )
    {
        case UNICAST_DEV_ADDR:
            if( lrWanVersion.Fields.Minor == 1 )
            {
                if( ( fType == FRAME_TYPE_A ) || ( fType == FRAME_TYPE_D ) )
                {
                    *fCntID = A_FCNT_DOWN;
                }
                else
                {
                    *fCntID = N_FCNT_DOWN;
                }
            }
            else
            { // For LoRaWAN 1.0.X
                *fCntID = FCNT_DOWN;
            }
            break;
#if ( LORAMAC_MAX_MC_CTX > 0 )
        case MULTICAST_0_ADDR:
            *fCntID = MC_FCNT_DOWN_0;
            break;
#endif /* LORAMAC_MAX_MC_CTX > 0 */
#if ( LORAMAC_MAX_MC_CTX > 1 )
        case MULTICAST_1_ADDR:
            *fCntID = MC_FCNT_DOWN_1;
            break;
#endif /* LORAMAC_MAX_MC_CTX > 1 */
#if ( LORAMAC_MAX_MC_CTX > 2 )
        case MULTICAST_2_ADDR:
            *fCntID = MC_FCNT_DOWN_2;
            break;
#endif /* LORAMAC_MAX_MC_CTX > 2 */
#if ( LORAMAC_MAX_MC_CTX > 3 )
        case MULTICAST_3_ADDR:
            *fCntID = MC_FCNT_DOWN_3;
            break;
#endif /* LORAMAC_MAX_MC_CTX > 3 */
        default:
            return LORAMAC_CRYPTO_FAIL_FCNT_ID;
    }

    return LoRaMacCryptoGetFCntDown( *fCntID, maxFCntGap, macMsg->FHDR.FCnt, currentDown );
}
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
static void OnRetransmitTimeoutTimerEvent( void* context )
{
    TimerStop( &MacCtx.RetransmitTimeoutTimer );

    if( MacCtx.NodeAckRequested == true )
    {
        MacCtx.RetransmitTimeoutRetry = true;
    }
    OnMacProcessNotify( );
}

static LoRaMacCryptoStatus_t GetFCntDown( AddressIdentifier_t addrID, FType_t fType, LoRaMacMessageData_t* macMsg, Version_t lrWanVersion,
                                          FCntIdentifier_t* fCntID, uint32_t* currentDown )
{
    if( ( macMsg == NULL ) || ( fCntID == NULL ) ||
        ( currentDown == NULL ) )
    {
        return LORAMAC_CRYPTO_ERROR_NPE;
    }

    // Determine the frame counter identifier and choose counter from FCntList
    switch( addrID )
    {
        case UNICAST_DEV_ADDR:
            if( lrWanVersion.Fields.Minor == 1 )
            {
                if( ( fType == FRAME_TYPE_A ) || ( fType == FRAME_TYPE_D ) )
                {
                    *fCntID = A_FCNT_DOWN;
                }
                else
                {
                    *fCntID = N_FCNT_DOWN;
                }
            }
            else
            { // For LoRaWAN 1.0.X
                *fCntID = FCNT_DOWN;
            }
            break;
#if ( LORAMAC_MAX_MC_CTX > 0 )
        case MULTICAST_0_ADDR:
            *fCntID = MC_FCNT_DOWN_0;
            break;
#endif /* LORAMAC_MAX_MC_CTX > 0 */
#if ( LORAMAC_MAX_MC_CTX > 1 )
        case MULTICAST_1_ADDR:
            *fCntID = MC_FCNT_DOWN_1;
            break;
#endif /* LORAMAC_MAX_MC_CTX > 1 */
#if ( LORAMAC_MAX_MC_CTX > 2 )
        case MULTICAST_2_ADDR:
            *fCntID = MC_FCNT_DOWN_2;
            break;
#endif /* LORAMAC_MAX_MC_CTX > 2 */
#if ( LORAMAC_MAX_MC_CTX > 3 )
        case MULTICAST_3_ADDR:
            *fCntID = MC_FCNT_DOWN_3;
            break;
#endif /* LORAMAC_MAX_MC_CTX > 3 */
        default:
            return LORAMAC_CRYPTO_FAIL_FCNT_ID;
    }

    return LoRaMacCryptoGetFCntDown( *fCntID, macMsg->FHDR.FCnt, currentDown );
}
#endif /* LORAMAC_VERSION */

static LoRaMacStatus_t SwitchClass( DeviceClass_t deviceClass )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_PARAMETER_INVALID;

    switch( Nvm.MacGroup2.DeviceClass )
    {
        case CLASS_A:
        {
            if( deviceClass == CLASS_A )
            {
                // Revert back RxC parameters
                Nvm.MacGroup2.MacParams.RxCChannel = Nvm.MacGroup2.MacParams.Rx2Channel;

                status = LORAMAC_STATUS_OK;
            }
            if( deviceClass == CLASS_B )
            {
                status = LoRaMacClassBSwitchClass( deviceClass );
                if( status == LORAMAC_STATUS_OK )
                {
                    Nvm.MacGroup2.DeviceClass = deviceClass;
                }
            }

            if( deviceClass == CLASS_C )
            {
                Nvm.MacGroup2.DeviceClass = deviceClass;

                MacCtx.RxWindowCConfig = MacCtx.RxWindow2Config;
                MacCtx.RxWindowCConfig.RxSlot = RX_SLOT_WIN_CLASS_C;

                for( int8_t i = 0; i < LORAMAC_MAX_MC_CTX; i++ )
                {
                    if( ( Nvm.MacGroup2.MulticastChannelList[i].ChannelParams.IsEnabled == true ) &&
                        ( Nvm.MacGroup2.MulticastChannelList[i].ChannelParams.RxParams.Class == CLASS_C ) )
                    {
                        Nvm.MacGroup2.MacParams.RxCChannel.Frequency = Nvm.MacGroup2.MulticastChannelList[i].ChannelParams.RxParams.Params.ClassC.Frequency;
                        Nvm.MacGroup2.MacParams.RxCChannel.Datarate = Nvm.MacGroup2.MulticastChannelList[i].ChannelParams.RxParams.Params.ClassC.Datarate;

                        MacCtx.RxWindowCConfig.Channel = MacCtx.Channel;
                        MacCtx.RxWindowCConfig.Frequency = Nvm.MacGroup2.MacParams.RxCChannel.Frequency;
                        MacCtx.RxWindowCConfig.DownlinkDwellTime = Nvm.MacGroup2.MacParams.DownlinkDwellTime;
                        MacCtx.RxWindowCConfig.RepeaterSupport = Nvm.MacGroup2.MacParams.RepeaterSupport;
                        MacCtx.RxWindowCConfig.RxSlot = RX_SLOT_WIN_CLASS_C_MULTICAST;
                        MacCtx.RxWindowCConfig.RxContinuous = true;
                        break;
                    }
                }

                // Set the NodeAckRequested indicator to default
                MacCtx.NodeAckRequested = false;
                // Set the radio into sleep mode in case we are still in RX mode
                Radio.Sleep( );

                OpenContinuousRxCWindow( );

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
                // Add a DeviceModeInd MAC Command to indicate the network a device mode change.
                if( Nvm.MacGroup2.Version.Fields.Minor >= 1 )
                {
                    LoRaMacCommandsAddCmd( MOTE_MAC_DEVICE_MODE_IND, ( uint8_t* )&Nvm.MacGroup2.DeviceClass, 1 );
                }
#endif /* LORAMAC_VERSION */

                status = LORAMAC_STATUS_OK;
            }
            break;
        }
        case CLASS_B:
        {
            status = LoRaMacClassBSwitchClass( deviceClass );
            if( status == LORAMAC_STATUS_OK )
            {
                Nvm.MacGroup2.DeviceClass = deviceClass;
            }
            break;
        }
        case CLASS_C:
        {
            if( deviceClass == CLASS_A )
            {
                // Reset RxSlot to NONE
                MacCtx.RxSlot = RX_SLOT_NONE;

                Nvm.MacGroup2.DeviceClass = deviceClass;

                // Set the radio into sleep to setup a defined state
                Radio.Sleep( );

                status = LORAMAC_STATUS_OK;

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
                // Add a DeviceModeInd MAC Command to indicate the network a device mode change.
                if( Nvm.MacGroup2.Version.Fields.Minor >= 1 )
                {
                    LoRaMacCommandsAddCmd( MOTE_MAC_DEVICE_MODE_IND, ( uint8_t* )&Nvm.MacGroup2.DeviceClass, 1 );
                }
#endif /* LORAMAC_VERSION */
            }
            break;
        }
    }

    return status;
}

static uint8_t GetMaxAppPayloadWithoutFOptsLength( int8_t datarate )
{
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;

    // Setup PHY request
    getPhy.UplinkDwellTime = Nvm.MacGroup2.MacParams.UplinkDwellTime;
    getPhy.Datarate = datarate;
    getPhy.Attribute = PHY_MAX_PAYLOAD;

    // Get the maximum payload length
    if( Nvm.MacGroup2.MacParams.RepeaterSupport == true )
    {
        getPhy.Attribute = PHY_MAX_PAYLOAD_REPEATER;
    }
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );

    return phyParam.Value;
}

static bool ValidatePayloadLength( uint8_t lenN, int8_t datarate, uint8_t fOptsLen )
{
    uint16_t maxN = 0;
    uint16_t payloadSize = 0;

    maxN = GetMaxAppPayloadWithoutFOptsLength( datarate );

    // Calculate the resulting payload size
    payloadSize = ( lenN + fOptsLen );

    // Validation of the application payload size
    if( ( payloadSize <= maxN ) && ( payloadSize <= LORAMAC_PHY_MAXPAYLOAD ) )
    {
        return true;
    }
    return false;
}

static void ProcessMacCommands( uint8_t *payload, uint8_t macIndex, uint8_t commandsSize, int8_t snr, LoRaMacRxSlot_t rxSlot )
{
    uint8_t status = 0;
    bool adrBlockFound = false;
    uint8_t macCmdPayload[2] = { 0x00, 0x00 };
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    MacCommand_t* macCmd;
#endif /* LORAMAC_VERSION */

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    if( ( rxSlot != RX_SLOT_WIN_1 ) && ( rxSlot != RX_SLOT_WIN_2 ) )
    {
        // Do only parse MAC commands for Class A RX windows
        return;
    }
#endif /* LORAMAC_VERSION */

    while( macIndex < commandsSize )
    {
        // Make sure to parse only complete MAC commands
        if( ( LoRaMacCommandsGetCmdSize( payload[macIndex] ) + macIndex ) > commandsSize )
        {
            return;
        }

        // Decode Frame MAC commands
        switch( payload[macIndex++] )
        {
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
            case SRV_MAC_RESET_CONF:
            {
                uint8_t serverMinorVersion = payload[macIndex++];

                // Compare own LoRaWAN Version with server's
                if( Nvm.MacGroup2.Version.Fields.Minor >= serverMinorVersion )
                {
                    // If they equal remove the sticky ResetInd MAC-Command.
                    if( LoRaMacCommandsGetCmd( MOTE_MAC_RESET_IND, &macCmd) == LORAMAC_COMMANDS_SUCCESS )
                    {
                        LoRaMacCommandsRemoveCmd( macCmd );
                    }
                }
                break;
            }
#endif /* LORAMAC_VERSION */
            case SRV_MAC_LINK_CHECK_ANS:
            {
                if( LoRaMacConfirmQueueIsCmdActive( MLME_LINK_CHECK ) == true )
                {
                    LoRaMacConfirmQueueSetStatus( LORAMAC_EVENT_INFO_STATUS_OK, MLME_LINK_CHECK );
                    MacCtx.MlmeConfirm.DemodMargin = payload[macIndex++];
                    MacCtx.MlmeConfirm.NbGateways = payload[macIndex++];
                }
                break;
            }
            case SRV_MAC_LINK_ADR_REQ:
            {
                LinkAdrReqParams_t linkAdrReq;
                int8_t linkAdrDatarate = DR_0;
                int8_t linkAdrTxPower = TX_POWER_0;
                uint8_t linkAdrNbRep = 0;
                uint8_t linkAdrNbBytesParsed = 0;

                // The end node is allowed to process one block of LinkAdrRequests.
                // It must ignore subsequent blocks
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
                if( adrBlockFound == false )
                {
                    adrBlockFound = true;

                    // Fill parameter structure
                    linkAdrReq.Payload = &payload[macIndex - 1];
                    linkAdrReq.PayloadSize = commandsSize - ( macIndex - 1 );
                    linkAdrReq.AdrEnabled = Nvm.MacGroup2.AdrCtrlOn;
                    linkAdrReq.UplinkDwellTime = Nvm.MacGroup2.MacParams.UplinkDwellTime;
                    linkAdrReq.CurrentDatarate = Nvm.MacGroup1.ChannelsDatarate;
                    linkAdrReq.CurrentTxPower = Nvm.MacGroup1.ChannelsTxPower;
                    linkAdrReq.CurrentNbRep = Nvm.MacGroup2.MacParams.ChannelsNbTrans;
                    linkAdrReq.Version = Nvm.MacGroup2.Version;

                    // Process the ADR requests
                    status = RegionLinkAdrReq( Nvm.MacGroup2.Region, &linkAdrReq, &linkAdrDatarate,
                                               &linkAdrTxPower, &linkAdrNbRep, &linkAdrNbBytesParsed );

                    if( ( status & 0x07 ) == 0x07 )
                    {
                        Nvm.MacGroup1.ChannelsDatarate = linkAdrDatarate;
                        Nvm.MacGroup1.ChannelsTxPower = linkAdrTxPower;
                        Nvm.MacGroup2.MacParams.ChannelsNbTrans = linkAdrNbRep;
                    }

                    // Add the answers to the buffer
                    for( uint8_t i = 0; i < ( linkAdrNbBytesParsed / 5 ); i++ )
                    {
                        LoRaMacCommandsAddCmd( MOTE_MAC_LINK_ADR_ANS, &status, 1 );
                    }
                    // Update MAC index
                    macIndex += linkAdrNbBytesParsed - 1;
                }
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
                if( adrBlockFound == false )
                {
                    adrBlockFound = true;

                    do
                    {
                        // Fill parameter structure
                        linkAdrReq.Payload = &payload[macIndex - 1];
                        linkAdrReq.AdrEnabled = Nvm.MacGroup2.AdrCtrlOn;
                        linkAdrReq.UplinkDwellTime = Nvm.MacGroup2.MacParams.UplinkDwellTime;
                        linkAdrReq.CurrentDatarate = Nvm.MacGroup1.ChannelsDatarate;
                        linkAdrReq.CurrentTxPower = Nvm.MacGroup1.ChannelsTxPower;
                        linkAdrReq.CurrentNbRep = Nvm.MacGroup2.MacParams.ChannelsNbTrans;
                        linkAdrReq.Version = Nvm.MacGroup2.Version;

                        // There is a fundamental difference in reporting the status
                        // of the LinkAdrRequests when ADR is on or off. When ADR is on, every
                        // LinkAdrAns contains the same value. This does not hold when ADR is off,
                        // where every LinkAdrAns requires an individual status.
                        if( Nvm.MacGroup2.AdrCtrlOn == true )
                        {
                            // When ADR is on, the function RegionLinkAdrReq will take care
                            // about the parsing and interpretation of the LinkAdrRequest block and
                            // it provides one status which shall be applied to every LinkAdrAns
                            linkAdrReq.PayloadSize = commandsSize - ( macIndex - 1 );
                        }
                        else
                        {
                            // When ADR is off, this function will loop over the individual LinkAdrRequests
                            // and will call RegionLinkAdrReq for each individually, as every request
                            // requires an individual answer.
                            // When ADR is off, the function RegionLinkAdrReq ignores the new values for
                            // ChannelsDatarate, ChannelsTxPower and ChannelsNbTrans.
                            linkAdrReq.PayloadSize = 5;
                        }

                        // Process the ADR requests
                        status = RegionLinkAdrReq( Nvm.MacGroup2.Region, &linkAdrReq, &linkAdrDatarate,
                                                &linkAdrTxPower, &linkAdrNbRep, &linkAdrNbBytesParsed );

                        if( ( status & 0x07 ) == 0x07 )
                        {
                            // Set the status that the datarate has been increased
                            if( linkAdrDatarate > Nvm.MacGroup1.ChannelsDatarate )
                            {
                                Nvm.MacGroup2.ChannelsDatarateChangedLinkAdrReq = true;
                            }
                            Nvm.MacGroup1.ChannelsDatarate = linkAdrDatarate;
                            Nvm.MacGroup1.ChannelsTxPower = linkAdrTxPower;
                            Nvm.MacGroup2.MacParams.ChannelsNbTrans = linkAdrNbRep;
                        }

                        // Add the answers to the buffer
                        for( uint8_t i = 0; i < ( linkAdrNbBytesParsed / 5 ); i++ )
                        {
                            LoRaMacCommandsAddCmd( MOTE_MAC_LINK_ADR_ANS, &status, 1 );
                        }
                        // Update MAC index
                        macIndex += linkAdrNbBytesParsed - 1;

                        // Check to prevent invalid access
                        if( macIndex >= commandsSize )
                            break;

                    } while( payload[macIndex++] == SRV_MAC_LINK_ADR_REQ );

                    if( macIndex < commandsSize )
                    {
                        // Decrease the index such that it points to the next MAC command
                        macIndex--;
                    }
                }
                else
                {
                    // Increase the index by the MAC command size (without command)
                    macIndex += 4;
                }
#endif /* LORAMAC_VERSION */
                break;
            }
            case SRV_MAC_DUTY_CYCLE_REQ:
            {
                Nvm.MacGroup2.MaxDCycle = payload[macIndex++] & 0x0F;
                Nvm.MacGroup2.AggregatedDCycle = 1 << Nvm.MacGroup2.MaxDCycle;
                LoRaMacCommandsAddCmd( MOTE_MAC_DUTY_CYCLE_ANS, macCmdPayload, 0 );
                break;
            }
            case SRV_MAC_RX_PARAM_SETUP_REQ:
            {
                RxParamSetupReqParams_t rxParamSetupReq;
                status = 0x07;

                rxParamSetupReq.DrOffset = ( payload[macIndex] >> 4 ) & 0x07;
                rxParamSetupReq.Datarate = payload[macIndex] & 0x0F;
                macIndex++;

                rxParamSetupReq.Frequency = ( uint32_t ) payload[macIndex++];
                rxParamSetupReq.Frequency |= ( uint32_t ) payload[macIndex++] << 8;
                rxParamSetupReq.Frequency |= ( uint32_t ) payload[macIndex++] << 16;
                rxParamSetupReq.Frequency *= 100;

                // Perform request on region
                status = RegionRxParamSetupReq( Nvm.MacGroup2.Region, &rxParamSetupReq );

                if( ( status & 0x07 ) == 0x07 )
                {
                    Nvm.MacGroup2.MacParams.Rx2Channel.Datarate = rxParamSetupReq.Datarate;
                    Nvm.MacGroup2.MacParams.RxCChannel.Datarate = rxParamSetupReq.Datarate;
                    Nvm.MacGroup2.MacParams.Rx2Channel.Frequency = rxParamSetupReq.Frequency;
                    Nvm.MacGroup2.MacParams.RxCChannel.Frequency = rxParamSetupReq.Frequency;
                    Nvm.MacGroup2.MacParams.Rx1DrOffset = rxParamSetupReq.DrOffset;
                }
                macCmdPayload[0] = status;
                LoRaMacCommandsAddCmd( MOTE_MAC_RX_PARAM_SETUP_ANS, macCmdPayload, 1 );
                break;
            }
            case SRV_MAC_DEV_STATUS_REQ:
            {
                uint8_t batteryLevel = BAT_LEVEL_NO_MEASURE;
                if( ( MacCtx.MacCallbacks != NULL ) && ( MacCtx.MacCallbacks->GetBatteryLevel != NULL ) )
                {
                    batteryLevel = MacCtx.MacCallbacks->GetBatteryLevel( );
                }
                macCmdPayload[0] = batteryLevel;
                macCmdPayload[1] = ( uint8_t )( snr & 0x3F );
                LoRaMacCommandsAddCmd( MOTE_MAC_DEV_STATUS_ANS, macCmdPayload, 2 );
                break;
            }
            case SRV_MAC_NEW_CHANNEL_REQ:
            {
                NewChannelReqParams_t newChannelReq;
                ChannelParams_t chParam;
                status = 0x03;

                newChannelReq.ChannelId = payload[macIndex++];
                newChannelReq.NewChannel = &chParam;

                chParam.Frequency = ( uint32_t ) payload[macIndex++];
                chParam.Frequency |= ( uint32_t ) payload[macIndex++] << 8;
                chParam.Frequency |= ( uint32_t ) payload[macIndex++] << 16;
                chParam.Frequency *= 100;
                chParam.Rx1Frequency = 0;
                chParam.DrRange.Value = payload[macIndex++];

                status = ( uint8_t )RegionNewChannelReq( Nvm.MacGroup2.Region, &newChannelReq );

                if( ( int8_t )status >= 0 )
                {
                    macCmdPayload[0] = status;
                    LoRaMacCommandsAddCmd( MOTE_MAC_NEW_CHANNEL_ANS, macCmdPayload, 1 );
                }
                break;
            }
            case SRV_MAC_RX_TIMING_SETUP_REQ:
            {
                uint8_t delay = payload[macIndex++] & 0x0F;

                if( delay == 0 )
                {
                    delay++;
                }
                Nvm.MacGroup2.MacParams.ReceiveDelay1 = delay * 1000;
                Nvm.MacGroup2.MacParams.ReceiveDelay2 = Nvm.MacGroup2.MacParams.ReceiveDelay1 + 1000;
                LoRaMacCommandsAddCmd( MOTE_MAC_RX_TIMING_SETUP_ANS, macCmdPayload, 0 );
                break;
            }
            case SRV_MAC_TX_PARAM_SETUP_REQ:
            {
                TxParamSetupReqParams_t txParamSetupReq;
                GetPhyParams_t getPhy;
                PhyParam_t phyParam;
                uint8_t eirpDwellTime = payload[macIndex++];

                txParamSetupReq.UplinkDwellTime = 0;
                txParamSetupReq.DownlinkDwellTime = 0;

                if( ( eirpDwellTime & 0x20 ) == 0x20 )
                {
                    txParamSetupReq.DownlinkDwellTime = 1;
                }
                if( ( eirpDwellTime & 0x10 ) == 0x10 )
                {
                    txParamSetupReq.UplinkDwellTime = 1;
                }
                txParamSetupReq.MaxEirp = eirpDwellTime & 0x0F;

                // Check the status for correctness
                if( RegionTxParamSetupReq( Nvm.MacGroup2.Region, &txParamSetupReq ) != -1 )
                {
                    // Accept command
                    Nvm.MacGroup2.MacParams.UplinkDwellTime = txParamSetupReq.UplinkDwellTime;
                    Nvm.MacGroup2.MacParams.DownlinkDwellTime = txParamSetupReq.DownlinkDwellTime;
                    Nvm.MacGroup2.MacParams.MaxEirp = LoRaMacMaxEirpTable[txParamSetupReq.MaxEirp];
                    // Update the datarate in case of the new configuration limits it
                    getPhy.Attribute = PHY_MIN_TX_DR;
                    getPhy.UplinkDwellTime = Nvm.MacGroup2.MacParams.UplinkDwellTime;
                    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
                    Nvm.MacGroup1.ChannelsDatarate = MAX( Nvm.MacGroup1.ChannelsDatarate, ( int8_t )phyParam.Value );

                    // Add command response
                    LoRaMacCommandsAddCmd( MOTE_MAC_TX_PARAM_SETUP_ANS, macCmdPayload, 0 );
                }
                break;
            }
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
            case SRV_MAC_REKEY_CONF:
            {
                uint8_t serverMinorVersion = payload[macIndex++];

                // Compare own LoRaWAN Version with server's
                if( Nvm.MacGroup2.Version.Fields.Minor >= serverMinorVersion )
                {
                    // If they equal remove the sticky RekeyInd MAC-Command.
                    if( LoRaMacCommandsGetCmd( MOTE_MAC_REKEY_IND, &macCmd) == LORAMAC_COMMANDS_SUCCESS )
                    {
                        LoRaMacCommandsRemoveCmd( macCmd );
                    }
                }
                break;
            }
#endif /* LORAMAC_VERSION */
            case SRV_MAC_DL_CHANNEL_REQ:
            {
                DlChannelReqParams_t dlChannelReq;
                status = 0x03;

                dlChannelReq.ChannelId = payload[macIndex++];
                dlChannelReq.Rx1Frequency = ( uint32_t ) payload[macIndex++];
                dlChannelReq.Rx1Frequency |= ( uint32_t ) payload[macIndex++] << 8;
                dlChannelReq.Rx1Frequency |= ( uint32_t ) payload[macIndex++] << 16;
                dlChannelReq.Rx1Frequency *= 100;

                status = ( uint8_t )RegionDlChannelReq( Nvm.MacGroup2.Region, &dlChannelReq );

                if( ( int8_t )status >= 0 )
                {
                    macCmdPayload[0] = status;
                    LoRaMacCommandsAddCmd( MOTE_MAC_DL_CHANNEL_ANS, macCmdPayload, 1 );
                }
                break;
            }
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
            case SRV_MAC_ADR_PARAM_SETUP_REQ:
            {
                /* ADRParamSetupReq Payload:  ADRparam
                 * +----------------+---------------+
                 * | 7:4 Limit_exp  | 3:0 Delay_exp |
                 * +----------------+---------------+
                 */

                uint8_t delayExp = 0x0F & payload[macIndex];
                uint8_t limitExp = 0x0F & ( payload[macIndex] >> 4 );
                macIndex++;

                // ADR_ACK_ DELAY = 2^Delay_exp
                Nvm.MacGroup2.MacParams.AdrAckDelay = 0x01 << delayExp;

                // ADR_ACK_LIMIT = 2^Limit_exp
                Nvm.MacGroup2.MacParams.AdrAckLimit = 0x01 << limitExp;

                LoRaMacCommandsAddCmd( MOTE_MAC_ADR_PARAM_SETUP_ANS, macCmdPayload, 0 );
                break;
            }
            case SRV_MAC_FORCE_REJOIN_REQ:
            {
                /* ForceRejoinReq Payload:
                 * +--------------+------------------+-------+----------------+--------+
                 * | 13:11 Period | 10:8 Max_Retries | 7 RFU | 6:4 RejoinType | 3:0 DR |
                 * +--------------+------------------+-------+----------------+--------+
                 */

                // Parse payload
                uint8_t period = ( 0x38 & payload[macIndex] ) >> 3;
                Nvm.MacGroup2.ForceRejoinMaxRetries = 0x07 & payload[macIndex];
                macIndex++;
                Nvm.MacGroup2.ForceRejoinType = ( 0x70 & payload[macIndex] ) >> 4;
                Nvm.MacGroup1.ChannelsDatarate = 0x0F & payload[macIndex];
                macIndex ++;

                // Calc delay between retransmissions: 32 seconds x 2^Period + Rand32
                uint32_t rejoinCycleInSec = 32 * ( 0x01 << period ) + randr( 0, 32 );

                MacCtx.ForceRejoinCycleTime = 0;
                Nvm.MacGroup1.ForceRejoinRetriesCounter = 0;
                ConvertRejoinCycleTime( rejoinCycleInSec, &MacCtx.ForceRejoinCycleTime );
                OnForceRejoinReqCycleTimerEvent( NULL );
                break;
            }
            case SRV_MAC_REJOIN_PARAM_REQ:
            {
                /* RejoinParamSetupReq Payload:
                 * +----------------+---------------+
                 * | 7:4 MaxTimeN   | 3:0 MaxCountN |
                 * +----------------+---------------+
                 */
                uint8_t maxCountN = 0x0F & payload[macIndex];
                uint8_t maxTimeN = 0x0F & ( payload[macIndex] >> 4 );
                uint32_t cycleInSec = 0x01 << ( maxTimeN + 10 );
                uint32_t timeInMs = 0;
                uint16_t uplinkLimit = 0x01 << ( maxCountN + 4 );
                macIndex++;
                macCmdPayload[0] = 0;

                if( ConvertRejoinCycleTime( cycleInSec, &timeInMs ) == true )
                {
                    // Calc delay between retransmissions: 2^(maxTimeN+10)
                    Nvm.MacGroup2.Rejoin0CycleInSec = cycleInSec;
                    // Calc number if uplinks without rejoin request: 2^(maxCountN+4)
                    Nvm.MacGroup2.Rejoin0UplinksLimit = uplinkLimit;
                    MacCtx.Rejoin0CycleTime = timeInMs;

                    macCmdPayload[0] = 0x01;
                    TimerStop( &MacCtx.Rejoin0CycleTimer );
                    TimerSetValue( &MacCtx.Rejoin0CycleTimer, MacCtx.Rejoin0CycleTime );
                    TimerStart( &MacCtx.Rejoin0CycleTimer );
                }
                LoRaMacCommandsAddCmd( MOTE_MAC_REJOIN_PARAM_ANS, macCmdPayload, 1 );
                break;
             }
            case SRV_MAC_DEVICE_MODE_CONF:
            {
                // 1 byte payload which we do not handle.
                macIndex++;
                if( LoRaMacCommandsGetCmd( MOTE_MAC_DEVICE_MODE_IND, &macCmd) == LORAMAC_COMMANDS_SUCCESS )
                {
                    LoRaMacCommandsRemoveCmd( macCmd );
                }
                break;
            }
#endif /* LORAMAC_VERSION */
            case SRV_MAC_DEVICE_TIME_ANS:
            {
                // The mote time can be updated only when the time is received in classA
                // receive windows only.
                if( LoRaMacConfirmQueueIsCmdActive( MLME_DEVICE_TIME ) == true )
                {
                    LoRaMacConfirmQueueSetStatus( LORAMAC_EVENT_INFO_STATUS_OK, MLME_DEVICE_TIME );

                    SysTime_t gpsEpochTime = { 0 };
                    SysTime_t sysTime = { 0 };
                    SysTime_t sysTimeCurrent = { 0 };

                    gpsEpochTime.Seconds = ( uint32_t )payload[macIndex++];
                    gpsEpochTime.Seconds |= ( uint32_t )payload[macIndex++] << 8;
                    gpsEpochTime.Seconds |= ( uint32_t )payload[macIndex++] << 16;
                    gpsEpochTime.Seconds |= ( uint32_t )payload[macIndex++] << 24;
                    gpsEpochTime.SubSeconds = payload[macIndex++];

                    // Convert the fractional second received in ms
                    // round( pow( 0.5, 8.0 ) * 1000 ) = 3.90625
                    gpsEpochTime.SubSeconds = ( int16_t )( ( ( int32_t )gpsEpochTime.SubSeconds * 1000 ) >> 8 );

                    // Copy received GPS Epoch time into system time
                    sysTime = gpsEpochTime;
                    // Add Unix to Gps epoch offset. The system time is based on Unix time.
                    sysTime.Seconds += UNIX_GPS_EPOCH_OFFSET;

                    // Compensate time difference between Tx Done time and now
                    sysTimeCurrent = SysTimeGet( );
                    sysTime = SysTimeAdd( sysTimeCurrent, SysTimeSub( sysTime, MacCtx.LastTxSysTime ) );

                    // Apply the new system time.
                    SysTimeSet( sysTime );
                    LoRaMacClassBDeviceTimeAns( );
                    MacCtx.McpsIndication.DeviceTimeAnsReceived = true;
                }
                else
                {
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
                    // In case of other receive windows the Device Time Answer is not received.
                    MacCtx.McpsIndication.DeviceTimeAnsReceived = false;
#endif /* LORAMAC_VERSION */
                }
                break;
            }
            case SRV_MAC_PING_SLOT_INFO_ANS:
            {
                if( LoRaMacConfirmQueueIsCmdActive( MLME_PING_SLOT_INFO ) == true )
                {
                    LoRaMacConfirmQueueSetStatus( LORAMAC_EVENT_INFO_STATUS_OK, MLME_PING_SLOT_INFO );
                    // According to the specification, it is not allowed to process this answer in
                    // a ping or multicast slot
                    if( ( MacCtx.RxSlot != RX_SLOT_WIN_CLASS_B_PING_SLOT ) && ( MacCtx.RxSlot != RX_SLOT_WIN_CLASS_B_MULTICAST_SLOT ) )
                    {
                        LoRaMacClassBPingSlotInfoAns( );
                    }
                }
                break;
            }
            case SRV_MAC_PING_SLOT_CHANNEL_REQ:
            {
                uint8_t status = 0x03;
                uint32_t frequency = 0;
                uint8_t datarate;

                frequency = ( uint32_t )payload[macIndex++];
                frequency |= ( uint32_t )payload[macIndex++] << 8;
                frequency |= ( uint32_t )payload[macIndex++] << 16;
                frequency *= 100;
                datarate = payload[macIndex++] & 0x0F;

                status = LoRaMacClassBPingSlotChannelReq( datarate, frequency );
                macCmdPayload[0] = status;
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
                LoRaMacCommandsAddCmd( MOTE_MAC_PING_SLOT_FREQ_ANS, macCmdPayload, 1 );
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
                LoRaMacCommandsAddCmd( MOTE_MAC_PING_SLOT_CHANNEL_ANS, macCmdPayload, 1 );
#endif /* LORAMAC_VERSION */
                break;
            }
            case SRV_MAC_BEACON_TIMING_ANS:
            {
                if( LoRaMacConfirmQueueIsCmdActive( MLME_BEACON_TIMING ) == true )
                {
                    LoRaMacConfirmQueueSetStatus( LORAMAC_EVENT_INFO_STATUS_OK, MLME_BEACON_TIMING );
                    uint16_t beaconTimingDelay = 0;
                    uint8_t beaconTimingChannel = 0;

                    beaconTimingDelay = ( uint16_t )payload[macIndex++];
                    beaconTimingDelay |= ( uint16_t )payload[macIndex++] << 8;
                    beaconTimingChannel = payload[macIndex++];

                    LoRaMacClassBBeaconTimingAns( beaconTimingDelay, beaconTimingChannel, RxDoneParams.LastRxDone );
                }
                break;
            }
            case SRV_MAC_BEACON_FREQ_REQ:
                {
                    uint32_t frequency = 0;

                    frequency = ( uint32_t )payload[macIndex++];
                    frequency |= ( uint32_t )payload[macIndex++] << 8;
                    frequency |= ( uint32_t )payload[macIndex++] << 16;
                    frequency *= 100;

                    if( LoRaMacClassBBeaconFreqReq( frequency ) == true )
                    {
                        macCmdPayload[0] = 1;
                    }
                    else
                    {
                        macCmdPayload[0] = 0;
                    }
                    LoRaMacCommandsAddCmd( MOTE_MAC_BEACON_FREQ_ANS, macCmdPayload, 1 );
                }
                break;
            default:
                // Unknown command. ABORT MAC commands processing
                return;
        }
    }
}

static LoRaMacStatus_t Send( LoRaMacHeader_t* macHdr, uint8_t fPort, void* fBuffer, uint16_t fBufferSize, bool allowDelayedTx )
{
    LoRaMacFrameCtrl_t fCtrl;
    LoRaMacStatus_t status = LORAMAC_STATUS_PARAMETER_INVALID;
    int8_t datarate = Nvm.MacGroup1.ChannelsDatarate;
    int8_t txPower = Nvm.MacGroup1.ChannelsTxPower;
    uint32_t adrAckCounter = Nvm.MacGroup1.AdrAckCounter;
    CalcNextAdrParams_t adrNext;

    // Check if we are joined
    if( Nvm.MacGroup2.NetworkActivation == ACTIVATION_TYPE_NONE )
    {
        return LORAMAC_STATUS_NO_NETWORK_JOINED;
    }
    if( Nvm.MacGroup2.MaxDCycle == 0 )
    {
        Nvm.MacGroup1.AggregatedTimeOff = 0;
    }

    fCtrl.Value = 0;
    fCtrl.Bits.FOptsLen      = 0;
    fCtrl.Bits.Adr           = Nvm.MacGroup2.AdrCtrlOn;

    // Check class b
    if( Nvm.MacGroup2.DeviceClass == CLASS_B )
    {
        fCtrl.Bits.FPending      = 1;
    }
    else
    {
        fCtrl.Bits.FPending      = 0;
    }

    // Check server ack
    if( Nvm.MacGroup1.SrvAckRequested == true )
    {
        fCtrl.Bits.Ack = 1;
    }

    // ADR next request
    adrNext.UpdateChanMask = true;
    adrNext.AdrEnabled = fCtrl.Bits.Adr;
    adrNext.AdrAckCounter = Nvm.MacGroup1.AdrAckCounter;
    adrNext.AdrAckLimit = Nvm.MacGroup2.MacParams.AdrAckLimit;
    adrNext.AdrAckDelay = Nvm.MacGroup2.MacParams.AdrAckDelay;
    adrNext.Datarate = Nvm.MacGroup1.ChannelsDatarate;
    adrNext.TxPower = Nvm.MacGroup1.ChannelsTxPower;
    adrNext.UplinkDwellTime =  Nvm.MacGroup2.MacParams.UplinkDwellTime;
    adrNext.Region = Nvm.MacGroup2.Region;
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    adrNext.Version = Nvm.MacGroup2.Version;
    fCtrl.Bits.AdrAckReq = LoRaMacAdrCalcNext( &adrNext, &Nvm.MacGroup1.ChannelsDatarate,
                                               &Nvm.MacGroup1.ChannelsTxPower, &adrAckCounter );
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    adrNext.NbTrans = Nvm.MacGroup2.MacParams.ChannelsNbTrans;
    fCtrl.Bits.AdrAckReq = LoRaMacAdrCalcNext( &adrNext, &Nvm.MacGroup1.ChannelsDatarate,
                                               &Nvm.MacGroup1.ChannelsTxPower,
                                               &Nvm.MacGroup2.MacParams.ChannelsNbTrans, &adrAckCounter );
#endif /* LORAMAC_VERSION */

    // Prepare the frame
    status = PrepareFrame( macHdr, &fCtrl, fPort, fBuffer, fBufferSize );

    // Validate status
    if( ( status == LORAMAC_STATUS_OK ) || ( status == LORAMAC_STATUS_SKIPPED_APP_DATA ) )
    {
        // Schedule frame, do not allow delayed transmissions
        status = ScheduleTx( allowDelayedTx );
    }

    // Post processing
    if( status != LORAMAC_STATUS_OK )
    {
        // Bad case - restore
        // Store local variables
        Nvm.MacGroup1.ChannelsDatarate = datarate;
        Nvm.MacGroup1.ChannelsTxPower = txPower;
    }
    else
    {
        // Good case
        Nvm.MacGroup1.SrvAckRequested = false;
        Nvm.MacGroup1.AdrAckCounter = adrAckCounter;
        // Remove all none sticky MAC commands
        if( LoRaMacCommandsRemoveNoneStickyCmds( ) != LORAMAC_COMMANDS_SUCCESS )
        {
            return LORAMAC_STATUS_MAC_COMMAD_ERROR;
        }
    }
    return status;
}

static LoRaMacStatus_t SendReJoinReq( JoinReqIdentifier_t joinReqType )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_OK;
    LoRaMacHeader_t macHdr;
    macHdr.Value = 0;
    bool allowDelayedTx = true;

    // Setup join/rejoin message
    switch( joinReqType )
    {
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
        case REJOIN_REQ_1:
        {
            Nvm.MacGroup2.IsRejoinAcceptPending = true;

            MacCtx.TxMsg.Type = LORAMAC_MSG_TYPE_RE_JOIN_1;
            MacCtx.TxMsg.Message.ReJoin1.Buffer = MacCtx.PktBuffer;
            MacCtx.TxMsg.Message.ReJoin1.BufSize = LORAMAC_PHY_MAXPAYLOAD;

            macHdr.Bits.MType = FRAME_TYPE_REJOIN;
            MacCtx.TxMsg.Message.ReJoin1.MHDR.Value = macHdr.Value;

            MacCtx.TxMsg.Message.ReJoin1.ReJoinType = 1;

            SecureElementGetJoinEui( MacCtx.TxMsg.Message.ReJoin1.JoinEUI );
            SecureElementGetDevEui( MacCtx.TxMsg.Message.ReJoin1.DevEUI );

            if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoGetRJcount( RJ_COUNT_1, &MacCtx.TxMsg.Message.ReJoin1.RJcount1 ) )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }

            break;
        }
        case REJOIN_REQ_0:
        case REJOIN_REQ_2:
        {
            if( joinReqType == REJOIN_REQ_0 )
            {
                MacCtx.TxMsg.Message.ReJoin0or2.ReJoinType = 0;
            }
            else
            {
                MacCtx.TxMsg.Message.ReJoin0or2.ReJoinType = 2;
            }

            Nvm.MacGroup2.IsRejoinAcceptPending = true;

            MacCtx.TxMsg.Type = LORAMAC_MSG_TYPE_RE_JOIN_0_2;
            MacCtx.TxMsg.Message.ReJoin0or2.Buffer = MacCtx.PktBuffer;
            MacCtx.TxMsg.Message.ReJoin0or2.BufSize = LORAMAC_PHY_MAXPAYLOAD;

            macHdr.Bits.MType = FRAME_TYPE_REJOIN;
            MacCtx.TxMsg.Message.ReJoin0or2.MHDR.Value = macHdr.Value;

            MacCtx.TxMsg.Message.ReJoin0or2.NetID[0] = Nvm.MacGroup2.NetID & 0xFF;
            MacCtx.TxMsg.Message.ReJoin0or2.NetID[1] = ( Nvm.MacGroup2.NetID >> 8 ) & 0xFF;
            MacCtx.TxMsg.Message.ReJoin0or2.NetID[2] = ( Nvm.MacGroup2.NetID >> 16 ) & 0xFF;

            SecureElementGetDevEui( MacCtx.TxMsg.Message.ReJoin0or2.DevEUI );

            if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoGetRJcount( RJ_COUNT_0, &MacCtx.TxMsg.Message.ReJoin0or2.RJcount0 ) )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }

            break;
        }
#endif /* LORAMAC_VERSION */
        case JOIN_REQ:
        {
            SwitchClass( CLASS_A );

            MacCtx.TxMsg.Type = LORAMAC_MSG_TYPE_JOIN_REQUEST;
            MacCtx.TxMsg.Message.JoinReq.Buffer = MacCtx.PktBuffer;
            MacCtx.TxMsg.Message.JoinReq.BufSize = LORAMAC_PHY_MAXPAYLOAD;

            macHdr.Bits.MType = FRAME_TYPE_JOIN_REQ;
            MacCtx.TxMsg.Message.JoinReq.MHDR.Value = macHdr.Value;

            SecureElementGetJoinEui( MacCtx.TxMsg.Message.JoinReq.JoinEUI );
            SecureElementGetDevEui( MacCtx.TxMsg.Message.JoinReq.DevEUI );

            allowDelayedTx = false;

            break;
        }
        default:
            status = LORAMAC_STATUS_SERVICE_UNKNOWN;
            break;
    }

    // Schedule frame
    status = ScheduleTx( allowDelayedTx );
    return status;
}

static LoRaMacStatus_t CheckForClassBCollision( void )
{
    if( LoRaMacClassBIsBeaconExpected( ) == true )
    {
        return LORAMAC_STATUS_BUSY_BEACON_RESERVED_TIME;
    }

    if( Nvm.MacGroup2.DeviceClass == CLASS_B )
    {
        if( LoRaMacClassBIsPingExpected( ) == true )
        {
            return LORAMAC_STATUS_BUSY_PING_SLOT_WINDOW_TIME;
        }
        else if( LoRaMacClassBIsMulticastExpected( ) == true )
        {
            return LORAMAC_STATUS_BUSY_PING_SLOT_WINDOW_TIME;
        }
    }
    return LORAMAC_STATUS_OK;
}

static void ComputeRxWindowParameters( void )
{
    // Compute Rx1 windows parameters
    RegionComputeRxWindowParameters( Nvm.MacGroup2.Region,
                                     RegionApplyDrOffset( Nvm.MacGroup2.Region,
                                                          Nvm.MacGroup2.MacParams.DownlinkDwellTime,
                                                          Nvm.MacGroup1.ChannelsDatarate,
                                                          Nvm.MacGroup2.MacParams.Rx1DrOffset ),
                                     Nvm.MacGroup2.MacParams.MinRxSymbols,
                                     Nvm.MacGroup2.MacParams.SystemMaxRxError,
                                     &MacCtx.RxWindow1Config );
    // Compute Rx2 windows parameters
    RegionComputeRxWindowParameters( Nvm.MacGroup2.Region,
                                     Nvm.MacGroup2.MacParams.Rx2Channel.Datarate,
                                     Nvm.MacGroup2.MacParams.MinRxSymbols,
                                     Nvm.MacGroup2.MacParams.SystemMaxRxError,
                                     &MacCtx.RxWindow2Config );

    // Default setup, in case the device joined
    MacCtx.RxWindow1Delay = Nvm.MacGroup2.MacParams.ReceiveDelay1 + MacCtx.RxWindow1Config.WindowOffset;
    MacCtx.RxWindow2Delay = Nvm.MacGroup2.MacParams.ReceiveDelay2 + MacCtx.RxWindow2Config.WindowOffset;

    if( MacCtx.TxMsg.Type != LORAMAC_MSG_TYPE_DATA )
    {
        MacCtx.RxWindow1Delay = Nvm.MacGroup2.MacParams.JoinAcceptDelay1 + MacCtx.RxWindow1Config.WindowOffset;
        MacCtx.RxWindow2Delay = Nvm.MacGroup2.MacParams.JoinAcceptDelay2 + MacCtx.RxWindow2Config.WindowOffset;
    }
}

static LoRaMacStatus_t VerifyTxFrame( void )
{
    size_t macCmdsSize = 0;

    if( Nvm.MacGroup2.NetworkActivation != ACTIVATION_TYPE_NONE )
    {
        if( LoRaMacCommandsGetSizeSerializedCmds( &macCmdsSize ) != LORAMAC_COMMANDS_SUCCESS )
        {
            return LORAMAC_STATUS_MAC_COMMAD_ERROR;
        }

        if( ValidatePayloadLength( MacCtx.AppDataSize, Nvm.MacGroup1.ChannelsDatarate, macCmdsSize ) == false )
        {
            return LORAMAC_STATUS_LENGTH_ERROR;
        }
    }
    return LORAMAC_STATUS_OK;
}

static LoRaMacStatus_t SerializeTxFrame( void )
{
    LoRaMacSerializerStatus_t serializeStatus;

    switch( MacCtx.TxMsg.Type )
    {
        case LORAMAC_MSG_TYPE_JOIN_REQUEST:
            serializeStatus = LoRaMacSerializerJoinRequest( &MacCtx.TxMsg.Message.JoinReq );
            if( LORAMAC_SERIALIZER_SUCCESS != serializeStatus )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }
            MacCtx.PktBufferLen = MacCtx.TxMsg.Message.JoinReq.BufSize;
            break;
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
        case LORAMAC_MSG_TYPE_RE_JOIN_1:
            serializeStatus = LoRaMacSerializerReJoinType1( &MacCtx.TxMsg.Message.ReJoin1 );
            if( LORAMAC_SERIALIZER_SUCCESS != serializeStatus )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }
            MacCtx.PktBufferLen = MacCtx.TxMsg.Message.ReJoin1.BufSize;
            break;
        case LORAMAC_MSG_TYPE_RE_JOIN_0_2:
            serializeStatus = LoRaMacSerializerReJoinType0or2( &MacCtx.TxMsg.Message.ReJoin0or2 );
            if( LORAMAC_SERIALIZER_SUCCESS != serializeStatus )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }
            MacCtx.PktBufferLen = MacCtx.TxMsg.Message.ReJoin0or2.BufSize;
            break;
#endif /* LORAMAC_VERSION */
        case LORAMAC_MSG_TYPE_DATA:
            serializeStatus = LoRaMacSerializerData( &MacCtx.TxMsg.Message.Data );
            if( LORAMAC_SERIALIZER_SUCCESS != serializeStatus )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }
            MacCtx.PktBufferLen = MacCtx.TxMsg.Message.Data.BufSize;
            break;
        case LORAMAC_MSG_TYPE_JOIN_ACCEPT:
        case LORAMAC_MSG_TYPE_UNDEF:
        default:
            return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    return LORAMAC_STATUS_OK;
}

static LoRaMacStatus_t ScheduleTx( bool allowDelayedTx )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_PARAMETER_INVALID;
    NextChanParams_t nextChan;

    // Check class b collisions
    status = CheckForClassBCollision( );
    if( status != LORAMAC_STATUS_OK )
    {
        return status;
    }

    // Update back-off
    CalculateBackOff( );

    // Serialize frame
    status = SerializeTxFrame( );
    if( status != LORAMAC_STATUS_OK )
    {
        return status;
    }

    nextChan.AggrTimeOff = Nvm.MacGroup1.AggregatedTimeOff;
    nextChan.Datarate = Nvm.MacGroup1.ChannelsDatarate;
    nextChan.DutyCycleEnabled = Nvm.MacGroup2.DutyCycleOn;
    nextChan.ElapsedTimeSinceStartUp = SysTimeSub( SysTimeGetMcuTime( ), Nvm.MacGroup2.InitializationTime );
    nextChan.LastAggrTx = Nvm.MacGroup1.LastTxDoneTime;
    nextChan.LastTxIsJoinRequest = false;
    nextChan.Joined = true;
    nextChan.PktLen = MacCtx.PktBufferLen;

    // Setup the parameters based on the join status
    if( Nvm.MacGroup2.NetworkActivation == ACTIVATION_TYPE_NONE )
    {
        nextChan.LastTxIsJoinRequest = true;
        nextChan.Joined = false;
    }

    // Select channel
    status = RegionNextChannel( Nvm.MacGroup2.Region, &nextChan, &MacCtx.Channel, &MacCtx.DutyCycleWaitTime, &Nvm.MacGroup1.AggregatedTimeOff );

    if( status != LORAMAC_STATUS_OK )
    {
        if( status == LORAMAC_STATUS_DUTYCYCLE_RESTRICTED )
        {
            if( MacCtx.DutyCycleWaitTime != 0 )
            {
                if( allowDelayedTx == true )
                {
                    // Allow delayed transmissions. We have to allow it in case
                    // the MAC must retransmit a frame with the frame repetitions
                    MacCtx.MacState |= LORAMAC_TX_DELAYED;
                    TimerSetValue( &MacCtx.TxDelayedTimer, MacCtx.DutyCycleWaitTime );
                    TimerStart( &MacCtx.TxDelayedTimer );
                    return LORAMAC_STATUS_OK;
                }
                // Need to delay, but allowDelayedTx does not allow it
                return status;
            }
        }
        else
        {// State where the MAC cannot send a frame
            return status;
        }
    }

    // Compute window parameters, offsets, rx symbols, system errors etc.
    ComputeRxWindowParameters( );

    // Verify TX frame
    status = VerifyTxFrame( );
    if( status != LORAMAC_STATUS_OK )
    {
        return status;
    }

    // Try to send now
    return SendFrameOnChannel( MacCtx.Channel );
}

static LoRaMacStatus_t SecureFrame( uint8_t txDr, uint8_t txCh )
{
    LoRaMacCryptoStatus_t macCryptoStatus = LORAMAC_CRYPTO_ERROR;
    uint32_t fCntUp = 0;

    switch( MacCtx.TxMsg.Type )
    {
        case LORAMAC_MSG_TYPE_JOIN_REQUEST:
            macCryptoStatus = LoRaMacCryptoPrepareJoinRequest( &MacCtx.TxMsg.Message.JoinReq );
            if( LORAMAC_CRYPTO_SUCCESS != macCryptoStatus )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }
            MacCtx.PktBufferLen = MacCtx.TxMsg.Message.JoinReq.BufSize;
            break;
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
        case LORAMAC_MSG_TYPE_RE_JOIN_1:
            macCryptoStatus = LoRaMacCryptoPrepareReJoinType1( &MacCtx.TxMsg.Message.ReJoin1 );
            if( LORAMAC_CRYPTO_SUCCESS != macCryptoStatus )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }
            MacCtx.PktBufferLen = MacCtx.TxMsg.Message.ReJoin1.BufSize;
            break;
        case LORAMAC_MSG_TYPE_RE_JOIN_0_2:
            macCryptoStatus = LoRaMacCryptoPrepareReJoinType0or2( &MacCtx.TxMsg.Message.ReJoin0or2 );
            if( LORAMAC_CRYPTO_SUCCESS != macCryptoStatus )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }
            MacCtx.PktBufferLen = MacCtx.TxMsg.Message.ReJoin0or2.BufSize;
            break;
#endif /* LORAMAC_VERSION */
        case LORAMAC_MSG_TYPE_DATA:

            if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoGetFCntUp( &fCntUp ) )
            {
                return LORAMAC_STATUS_FCNT_HANDLER_ERROR;
            }

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
            if( ( MacCtx.ChannelsNbTransCounter >= 1 ) || ( MacCtx.AckTimeoutRetriesCounter > 1 ) )
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
            if( MacCtx.ChannelsNbTransCounter >= 1 )
#endif /* LORAMAC_VERSION */
            {
                fCntUp -= 1;
            }

            macCryptoStatus = LoRaMacCryptoSecureMessage( fCntUp, txDr, txCh, &MacCtx.TxMsg.Message.Data );
            if( LORAMAC_CRYPTO_SUCCESS != macCryptoStatus )
            {
                return LORAMAC_STATUS_CRYPTO_ERROR;
            }
            MacCtx.PktBufferLen = MacCtx.TxMsg.Message.Data.BufSize;
            break;
        case LORAMAC_MSG_TYPE_JOIN_ACCEPT:
        case LORAMAC_MSG_TYPE_UNDEF:
        default:
            return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    return LORAMAC_STATUS_OK;
}

static void CalculateBackOff( void )
{
    // Make sure that the calculation of the backoff time for the aggregated time off will only be done in
    // case the value is zero. It will be set to zero in the function RegionNextChannel.
    if( Nvm.MacGroup1.AggregatedTimeOff == 0 )
    {
        // Update aggregated time-off. This must be an assignment and no incremental
        // update as we do only calculate the time-off based on the last transmission
        Nvm.MacGroup1.AggregatedTimeOff = ( MacCtx.TxTimeOnAir * Nvm.MacGroup2.AggregatedDCycle - MacCtx.TxTimeOnAir );
    }
}

static void RemoveMacCommands( LoRaMacRxSlot_t rxSlot, LoRaMacFrameCtrl_t fCtrl, Mcps_t request )
{
    if( rxSlot == RX_SLOT_WIN_1 || rxSlot == RX_SLOT_WIN_2  )
    {
        // Remove all sticky MAC commands answers since we can assume
        // that they have been received by the server.
        if( request == MCPS_CONFIRMED )
        {
            if( fCtrl.Bits.Ack == 1 )
            {  // For confirmed uplinks only if we have received an ACK.
                LoRaMacCommandsRemoveStickyAnsCmds( );
            }
        }
        else
        {
            LoRaMacCommandsRemoveStickyAnsCmds( );
        }
    }
}

static void ResetMacParameters( bool isRejoin )
{
    LoRaMacClassBCallback_t classBCallbacks;
    LoRaMacClassBParams_t classBParams;

    if( isRejoin == false )
    {
        Nvm.MacGroup2.NetworkActivation = ACTIVATION_TYPE_NONE;
    }

    // ADR counter
    Nvm.MacGroup1.AdrAckCounter = 0;

    MacCtx.ChannelsNbTransCounter = 0;
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    MacCtx.AckTimeoutRetries = 1;
    MacCtx.AckTimeoutRetriesCounter = 1;
    MacCtx.AckTimeoutRetry = false;
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    MacCtx.RetransmitTimeoutRetry = false;
    MacCtx.ResponseTimeoutStartTime = 0;
#endif /* LORAMAC_VERSION */

    Nvm.MacGroup2.MaxDCycle = 0;
    Nvm.MacGroup2.AggregatedDCycle = 1;

    Nvm.MacGroup1.ChannelsTxPower = Nvm.MacGroup2.ChannelsTxPowerDefault;
    Nvm.MacGroup1.ChannelsDatarate = Nvm.MacGroup2.ChannelsDatarateDefault;
    Nvm.MacGroup2.MacParams.Rx1DrOffset = Nvm.MacGroup2.MacParamsDefaults.Rx1DrOffset;
    Nvm.MacGroup2.MacParams.Rx2Channel = Nvm.MacGroup2.MacParamsDefaults.Rx2Channel;
    Nvm.MacGroup2.MacParams.RxCChannel = Nvm.MacGroup2.MacParamsDefaults.RxCChannel;
    Nvm.MacGroup2.MacParams.UplinkDwellTime = Nvm.MacGroup2.MacParamsDefaults.UplinkDwellTime;
    Nvm.MacGroup2.MacParams.DownlinkDwellTime = Nvm.MacGroup2.MacParamsDefaults.DownlinkDwellTime;
    Nvm.MacGroup2.MacParams.MaxEirp = Nvm.MacGroup2.MacParamsDefaults.MaxEirp;
    Nvm.MacGroup2.MacParams.AntennaGain = Nvm.MacGroup2.MacParamsDefaults.AntennaGain;
    Nvm.MacGroup2.MacParams.AdrAckLimit = Nvm.MacGroup2.MacParamsDefaults.AdrAckLimit;
    Nvm.MacGroup2.MacParams.AdrAckDelay = Nvm.MacGroup2.MacParamsDefaults.AdrAckDelay;

    MacCtx.NodeAckRequested = false;
    Nvm.MacGroup1.SrvAckRequested = false;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    Nvm.MacGroup2.ChannelsDatarateChangedLinkAdrReq = false;
    Nvm.MacGroup2.DownlinkReceived = false;
#endif /* LORAMAC_VERSION */

    Nvm.MacGroup2.Rejoin0UplinksLimit = 0;
    Nvm.MacGroup2.ForceRejoinMaxRetries = 0;
    Nvm.MacGroup2.ForceRejoinType = 0;
    Nvm.MacGroup2.Rejoin0CycleInSec = 0;
    Nvm.MacGroup2.Rejoin1CycleInSec = 0;
    Nvm.MacGroup2.IsRejoin0RequestQueued = 0;
    Nvm.MacGroup2.IsRejoin1RequestQueued = 0;
    Nvm.MacGroup2.IsRejoin2RequestQueued = 0;

    // Reset to application defaults
    InitDefaultsParams_t params;
    params.Type = INIT_TYPE_RESET_TO_DEFAULT_CHANNELS;
    params.NvmGroup1 = &Nvm.RegionGroup1;
    params.NvmGroup2 = &Nvm.RegionGroup2;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    params.Bands = &RegionBands;
#endif /* LORAMAC_VERSION */
    RegionInitDefaults( Nvm.MacGroup2.Region, &params );

    // Initialize channel index.
    MacCtx.Channel = 0;

    // Initialize Rx2 config parameters.
    MacCtx.RxWindow2Config.Channel = MacCtx.Channel;
    MacCtx.RxWindow2Config.Frequency = Nvm.MacGroup2.MacParams.Rx2Channel.Frequency;
    MacCtx.RxWindow2Config.DownlinkDwellTime = Nvm.MacGroup2.MacParams.DownlinkDwellTime;
    MacCtx.RxWindow2Config.RepeaterSupport = Nvm.MacGroup2.MacParams.RepeaterSupport;
    MacCtx.RxWindow2Config.RxContinuous = false;
    MacCtx.RxWindow2Config.RxSlot = RX_SLOT_WIN_2;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    MacCtx.RxWindow2Config.NetworkActivation = Nvm.MacGroup2.NetworkActivation;
#endif /* LORAMAC_VERSION */

    // Initialize RxC config parameters.
    MacCtx.RxWindowCConfig = MacCtx.RxWindow2Config;
    MacCtx.RxWindowCConfig.RxContinuous = true;
    MacCtx.RxWindowCConfig.RxSlot = RX_SLOT_WIN_CLASS_C;

    // Initialize class b
    // Apply callback
    classBCallbacks.GetTemperatureLevel = NULL;
    classBCallbacks.MacProcessNotify = NULL;

    if( MacCtx.MacCallbacks != NULL )
    {
        classBCallbacks.GetTemperatureLevel = MacCtx.MacCallbacks->GetTemperatureLevel;
        classBCallbacks.MacProcessNotify = MacCtx.MacCallbacks->MacProcessNotify;
    }

    // Must all be static. Don't use local references.
    classBParams.MlmeIndication = &MacCtx.MlmeIndication;
    classBParams.McpsIndication = &MacCtx.McpsIndication;
    classBParams.MlmeConfirm = &MacCtx.MlmeConfirm;
    classBParams.LoRaMacFlags = &MacCtx.MacFlags;
    classBParams.LoRaMacDevAddr = &Nvm.MacGroup2.DevAddr;
    classBParams.LoRaMacRegion = &Nvm.MacGroup2.Region;
    classBParams.LoRaMacParams = &Nvm.MacGroup2.MacParams;
    classBParams.MulticastChannels = &Nvm.MacGroup2.MulticastChannelList[0];
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    classBParams.NetworkActivation = &Nvm.MacGroup2.NetworkActivation;
#endif /* LORAMAC_VERSION */

    LoRaMacClassBInit( &classBParams, &classBCallbacks, &Nvm.ClassB );
}

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
static bool IsReJoin0Required( )
{

    if( ( Nvm.MacGroup2.Rejoin0UplinksLimit == Nvm.MacGroup1.Rejoin0UplinksCounter ) &&
        ( Nvm.MacGroup2.Version.Fields.Minor >= 1 ) &&
        ( Nvm.MacGroup2.Rejoin0UplinksLimit != 0 ) )
    {
        Nvm.MacGroup1.Rejoin0UplinksCounter = 0;
        return true;
    }
    return false;
}
#endif /* LORAMAC_VERSION */

/*!
 * \brief Initializes and opens the reception window
 *
 * \param [in] rxTimer  Window timer to be topped.
 * \param [in] rxConfig Window parameters to be setup
 */
static void RxWindowSetup( TimerEvent_t* rxTimer, RxConfigParams_t* rxConfig )
{
    TimerStop( rxTimer );

    // Ensure the radio is Idle
    Radio.Standby( );

    if( RegionRxConfig( Nvm.MacGroup2.Region, rxConfig, ( int8_t* )&MacCtx.McpsIndication.RxDatarate ) == true )
    {
        MacCtx.MlmeIndication.RxDatarate = MacCtx.McpsIndication.RxDatarate;
        Radio.Rx( Nvm.MacGroup2.MacParams.MaxRxWindow );
        MacCtx.RxSlot = rxConfig->RxSlot;
    }
}

static void OpenContinuousRxCWindow( void )
{
    // Compute RxC windows parameters
    RegionComputeRxWindowParameters( Nvm.MacGroup2.Region,
                                     Nvm.MacGroup2.MacParams.RxCChannel.Datarate,
                                     Nvm.MacGroup2.MacParams.MinRxSymbols,
                                     Nvm.MacGroup2.MacParams.SystemMaxRxError,
                                     &MacCtx.RxWindowCConfig );

    MacCtx.RxWindowCConfig.RxSlot = RX_SLOT_WIN_CLASS_C;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    MacCtx.RxWindowCConfig.NetworkActivation = Nvm.MacGroup2.NetworkActivation;
#endif /* LORAMAC_VERSION */
    // Setup continuous listening
    MacCtx.RxWindowCConfig.RxContinuous = true;

    // At this point the Radio should be idle.
    // Thus, there is no need to set the radio in standby mode.
    if( RegionRxConfig( Nvm.MacGroup2.Region, &MacCtx.RxWindowCConfig, ( int8_t* )&MacCtx.McpsIndication.RxDatarate ) == true )
    {
        MacCtx.MlmeIndication.RxDatarate = MacCtx.McpsIndication.RxDatarate;
        Radio.Rx( 0 ); // Continuous mode
        MacCtx.RxSlot = MacCtx.RxWindowCConfig.RxSlot;
    }
}

static LoRaMacStatus_t PrepareFrame( LoRaMacHeader_t* macHdr, LoRaMacFrameCtrl_t* fCtrl, uint8_t fPort, void* fBuffer, uint16_t fBufferSize )
{
    MacCtx.PktBufferLen = 0;
    MacCtx.NodeAckRequested = false;
    uint32_t fCntUp = 0;
    size_t macCmdsSize = 0;
    uint8_t availableSize = 0;

    if( fBuffer == NULL )
    {
        fBufferSize = 0;
    }

    memcpy1( MacCtx.AppData, ( uint8_t* ) fBuffer, fBufferSize );
    MacCtx.AppDataSize = fBufferSize;
    MacCtx.PktBuffer[0] = macHdr->Value;

    switch( macHdr->Bits.MType )
    {
        case FRAME_TYPE_DATA_CONFIRMED_UP:
            MacCtx.NodeAckRequested = true;
            // Intentional fall through
        case FRAME_TYPE_DATA_UNCONFIRMED_UP:
            MacCtx.TxMsg.Type = LORAMAC_MSG_TYPE_DATA;
            MacCtx.TxMsg.Message.Data.Buffer = MacCtx.PktBuffer;
            MacCtx.TxMsg.Message.Data.BufSize = LORAMAC_PHY_MAXPAYLOAD;
            MacCtx.TxMsg.Message.Data.MHDR.Value = macHdr->Value;
            MacCtx.TxMsg.Message.Data.FPort = fPort;
            MacCtx.TxMsg.Message.Data.FHDR.DevAddr = Nvm.MacGroup2.DevAddr;
            MacCtx.TxMsg.Message.Data.FHDR.FCtrl.Value = fCtrl->Value;
            MacCtx.TxMsg.Message.Data.FRMPayloadSize = MacCtx.AppDataSize;
            MacCtx.TxMsg.Message.Data.FRMPayload = MacCtx.AppData;

            if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoGetFCntUp( &fCntUp ) )
            {
                return LORAMAC_STATUS_FCNT_HANDLER_ERROR;
            }
            MacCtx.TxMsg.Message.Data.FHDR.FCnt = ( uint16_t )fCntUp;

            // Reset confirm parameters
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
            MacCtx.McpsConfirm.NbRetries = 0;
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
            MacCtx.McpsConfirm.NbTrans = 0;
#endif /* LORAMAC_VERSION */
            MacCtx.McpsConfirm.AckReceived = false;
            MacCtx.McpsConfirm.UpLinkCounter = fCntUp;

            // Handle the MAC commands if there are any available
            if( LoRaMacCommandsGetSizeSerializedCmds( &macCmdsSize ) != LORAMAC_COMMANDS_SUCCESS )
            {
                return LORAMAC_STATUS_MAC_COMMAD_ERROR;
            }

            if( macCmdsSize > 0 )
            {
                availableSize = GetMaxAppPayloadWithoutFOptsLength( Nvm.MacGroup1.ChannelsDatarate );

                // There is application payload available and the MAC commands fit into FOpts field.
                if( ( MacCtx.AppDataSize > 0 ) && ( macCmdsSize <= LORA_MAC_COMMAND_MAX_FOPTS_LENGTH ) )
                {
                    if( LoRaMacCommandsSerializeCmds( LORA_MAC_COMMAND_MAX_FOPTS_LENGTH, &macCmdsSize, MacCtx.TxMsg.Message.Data.FHDR.FOpts ) != LORAMAC_COMMANDS_SUCCESS )
                    {
                        return LORAMAC_STATUS_MAC_COMMAD_ERROR;
                    }
                    fCtrl->Bits.FOptsLen = macCmdsSize;
                    // Update FCtrl field with new value of FOptionsLength
                    MacCtx.TxMsg.Message.Data.FHDR.FCtrl.Value = fCtrl->Value;
                }
                // There is application payload available but the MAC commands does NOT fit into FOpts field.
                else if( ( MacCtx.AppDataSize > 0 ) && ( macCmdsSize > LORA_MAC_COMMAND_MAX_FOPTS_LENGTH ) )
                {

                    if( LoRaMacCommandsSerializeCmds( availableSize, &macCmdsSize, MacCtx.MacCommandsBuffer ) != LORAMAC_COMMANDS_SUCCESS )
                    {
                        return LORAMAC_STATUS_MAC_COMMAD_ERROR;
                    }
                    return LORAMAC_STATUS_SKIPPED_APP_DATA;
                }
                // No application payload available therefore add all mac commands to the FRMPayload.
                else
                {
                    if( LoRaMacCommandsSerializeCmds( availableSize, &macCmdsSize, MacCtx.MacCommandsBuffer ) != LORAMAC_COMMANDS_SUCCESS )
                    {
                        return LORAMAC_STATUS_MAC_COMMAD_ERROR;
                    }
                    // Force FPort to be zero
                    MacCtx.TxMsg.Message.Data.FPort = 0;

                    MacCtx.TxMsg.Message.Data.FRMPayload = MacCtx.MacCommandsBuffer;
                    MacCtx.TxMsg.Message.Data.FRMPayloadSize = macCmdsSize;
                }
            }

            break;
        case FRAME_TYPE_PROPRIETARY:
            if( ( fBuffer != NULL ) && ( MacCtx.AppDataSize > 0 ) )
            {
                memcpy1( MacCtx.PktBuffer + LORAMAC_MHDR_FIELD_SIZE, ( uint8_t* ) fBuffer, MacCtx.AppDataSize );
                MacCtx.PktBufferLen = LORAMAC_MHDR_FIELD_SIZE + MacCtx.AppDataSize;
            }
            break;
        default:
            return LORAMAC_STATUS_SERVICE_UNKNOWN;
    }

    return LORAMAC_STATUS_OK;
}

static LoRaMacStatus_t SendFrameOnChannel( uint8_t channel )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_PARAMETER_INVALID;
    TxConfigParams_t txConfig;
    int8_t txPower = 0;

    txConfig.Channel = channel;
    txConfig.Datarate = Nvm.MacGroup1.ChannelsDatarate;
    txConfig.TxPower = Nvm.MacGroup1.ChannelsTxPower;
    txConfig.MaxEirp = Nvm.MacGroup2.MacParams.MaxEirp;
    txConfig.AntennaGain = Nvm.MacGroup2.MacParams.AntennaGain;
    txConfig.PktLen = MacCtx.PktBufferLen;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    txConfig.NetworkActivation = Nvm.MacGroup2.NetworkActivation;
#endif /* LORAMAC_VERSION */

    RegionTxConfig( Nvm.MacGroup2.Region, &txConfig, &txPower, &MacCtx.TxTimeOnAir );

    MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
    MacCtx.McpsConfirm.Datarate = Nvm.MacGroup1.ChannelsDatarate;
    MacCtx.McpsConfirm.TxPower = txPower;
    MacCtx.McpsConfirm.Channel = channel;

    MacCtx.SumSendCount++;
    MacCtx.SumSendTime += MacCtx.TxTimeOnAir;

    // Store the time on air
    MacCtx.McpsConfirm.TxTimeOnAir = MacCtx.TxTimeOnAir;
    MacCtx.MlmeConfirm.TxTimeOnAir = MacCtx.TxTimeOnAir;

    if( LoRaMacClassBIsBeaconModeActive( ) == true )
    {
        // Currently, the Time-On-Air can only be computed when the radio is configured with
        // the TX configuration
        TimerTime_t collisionTime = LoRaMacClassBIsUplinkCollision( MacCtx.TxTimeOnAir );

        if( collisionTime > 0 )
        {
            return LORAMAC_STATUS_BUSY_UPLINK_COLLISION;
        }
    }

    if( Nvm.MacGroup2.DeviceClass == CLASS_B )
    {
        // Stop slots for class b
        LoRaMacClassBStopRxSlots( );
    }

    LoRaMacClassBHaltBeaconing( );

    // Secure frame
    status = SecureFrame( Nvm.MacGroup1.ChannelsDatarate, MacCtx.Channel );
    if( status != LORAMAC_STATUS_OK )
    {
        return status;
    }

    MacCtx.MacState |= LORAMAC_TX_RUNNING;
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    if( MacCtx.NodeAckRequested == false )
    {
        MacCtx.ChannelsNbTransCounter++;
    }
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    MacCtx.ChannelsNbTransCounter++;
    MacCtx.McpsConfirm.NbTrans = MacCtx.ChannelsNbTransCounter;
    MacCtx.ResponseTimeoutStartTime = 0;
#endif /* LORAMAC_VERSION */

    // Send now
    Radio.Send( MacCtx.PktBuffer, MacCtx.PktBufferLen );

    return LORAMAC_STATUS_OK;
}

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
static LoRaMacStatus_t SetTxContinuousWave( uint16_t timeout )
{
    ContinuousWaveParams_t continuousWave;

    continuousWave.Channel = MacCtx.Channel;
    continuousWave.Datarate = Nvm.MacGroup1.ChannelsDatarate;
    continuousWave.TxPower = Nvm.MacGroup1.ChannelsTxPower;
    continuousWave.MaxEirp = Nvm.MacGroup2.MacParams.MaxEirp;
    continuousWave.AntennaGain = Nvm.MacGroup2.MacParams.AntennaGain;
    continuousWave.Timeout = timeout;

    RegionSetContinuousWave( Nvm.MacGroup2.Region, &continuousWave );

    MacCtx.MacState |= LORAMAC_TX_RUNNING;

    return LORAMAC_STATUS_OK;
}

static LoRaMacStatus_t SetTxContinuousWave1( uint16_t timeout, uint32_t frequency, uint8_t power )
{
    Radio.SetTxContinuousWave( frequency, power, timeout );

    MacCtx.MacState |= LORAMAC_TX_RUNNING;

    return LORAMAC_STATUS_OK;
}
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
static LoRaMacStatus_t SetTxContinuousWave( uint16_t timeout, uint32_t frequency, uint8_t power )
{
    Radio.SetTxContinuousWave( frequency, power, timeout );

    MacCtx.MacState |= LORAMAC_TX_RUNNING;

    return LORAMAC_STATUS_OK;
}
#endif /* LORAMAC_VERSION */

static LoRaMacStatus_t RestoreNvmData( void )
{
#if (defined( CONTEXT_MANAGEMENT_ENABLED ) && ( CONTEXT_MANAGEMENT_ENABLED == 1 ))
    uint32_t crc = 0;

    // Status and parameter validation
    if( MacCtx.MacState != LORAMAC_STOPPED )
    {
        return LORAMAC_STATUS_BUSY;
    }

    // Crypto
    crc = Crc32( ( uint8_t* ) &(NvmBackup.Crypto), sizeof( NvmBackup.Crypto ) -
                                            sizeof( NvmBackup.Crypto.Crc32 ) );
    if( crc != NvmBackup.Crypto.Crc32 )
    {
        return LORAMAC_STATUS_NVM_DATA_INCONSISTENT;
    }

    // MacGroup1
    crc = Crc32( ( uint8_t* ) &(NvmBackup.MacGroup1), sizeof( NvmBackup.MacGroup1 ) -
                                               sizeof( NvmBackup.MacGroup1.Crc32 ) );
    if( crc != NvmBackup.MacGroup1.Crc32 )
    {
        return LORAMAC_STATUS_NVM_DATA_INCONSISTENT;
    }

    // MacGroup2
    crc = Crc32( ( uint8_t* ) &(NvmBackup.MacGroup2), sizeof( NvmBackup.MacGroup2 ) -
                                               sizeof( NvmBackup.MacGroup2.Crc32 ) );
    if( crc != NvmBackup.MacGroup2.Crc32 )
    {
        return LORAMAC_STATUS_NVM_DATA_INCONSISTENT;
    }

    // Secure Element
    crc = Crc32( ( uint8_t* ) &(NvmBackup.SecureElement), sizeof( NvmBackup.SecureElement ) -
                                                   sizeof( NvmBackup.SecureElement.Crc32 ) );
    if( crc != NvmBackup.SecureElement.Crc32 )
    {
        return LORAMAC_STATUS_NVM_DATA_INCONSISTENT;
    }

    // RegionGroup1
    crc = Crc32( ( uint8_t* ) &(NvmBackup.RegionGroup1), sizeof( NvmBackup.RegionGroup1 ) -
                                            sizeof( NvmBackup.RegionGroup1.Crc32 ) );
    if( crc != NvmBackup.RegionGroup1.Crc32 )
    {
        return LORAMAC_STATUS_NVM_DATA_INCONSISTENT;
    }

    // RegionGroup2
    crc = Crc32( ( uint8_t* ) &(NvmBackup.RegionGroup2), sizeof( NvmBackup.RegionGroup2 ) -
                                            sizeof( NvmBackup.RegionGroup2.Crc32 ) );
    if( crc != NvmBackup.RegionGroup2.Crc32 )
    {
        return LORAMAC_STATUS_NVM_DATA_INCONSISTENT;
    }

    crc = Crc32( ( uint8_t* ) &(NvmBackup.ClassB), sizeof( NvmBackup.ClassB ) -
                                            sizeof( NvmBackup.ClassB.Crc32 ) );
    if( crc != NvmBackup.ClassB.Crc32 )
    {
        return LORAMAC_STATUS_NVM_DATA_INCONSISTENT;
    }

    memcpy1( ( uint8_t* ) &Nvm, ( uint8_t* ) &NvmBackup, sizeof( LoRaMacNvmData_t ) );
    memset1( ( uint8_t* ) &NvmBackup, 0, sizeof( LoRaMacNvmData_t ) );

    // Initialize RxC config parameters.
    MacCtx.RxWindowCConfig.Channel = MacCtx.Channel;
    MacCtx.RxWindowCConfig.Frequency = Nvm.MacGroup2.MacParams.RxCChannel.Frequency;
    MacCtx.RxWindowCConfig.DownlinkDwellTime = Nvm.MacGroup2.MacParams.DownlinkDwellTime;
    MacCtx.RxWindowCConfig.RxContinuous = true;
    MacCtx.RxWindowCConfig.RxSlot = RX_SLOT_WIN_CLASS_C;

    // The public/private network flag may change upon reloading MacGroup2
    // from NVM and we thus need to synchronize the radio. The same function
    // is invoked in LoRaMacInitialization.
    Radio.SetPublicNetwork( Nvm.MacGroup2.PublicNetwork );
#endif /* CONTEXT_MANAGEMENT_ENABLED == 1 */

    return LORAMAC_STATUS_OK;
}

static LoRaMacStatus_t DetermineFrameType( LoRaMacMessageData_t* macMsg, FType_t* fType )
{
    if( ( macMsg == NULL ) || ( fType == NULL ) )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    /* The LoRaWAN specification allows several possible configurations how data up/down frames are built up.
     * In sake of clearness the following naming is applied. Please keep in mind that this is
     * implementation specific since there is no definition in the LoRaWAN specification included.
     *
     * X -> Field is available
     * - -> Field is not available
     *
     * +-------+  +----------+------+-------+--------------+
     * | FType |  | FOptsLen | Fopt | FPort |  FRMPayload  |
     * +-------+  +----------+------+-------+--------------+
     * |   A   |  |    > 0   |   X  |  > 0  |       X      |
     * +-------+  +----------+------+-------+--------------+
     * |   B   |  |   >= 0   |  X/- |   -   |       -      |
     * +-------+  +----------+------+-------+--------------+
     * |   C   |  |    = 0   |   -  |  = 0  | MAC commands |
     * +-------+  +----------+------+-------+--------------+
     * |   D   |  |    = 0   |   -  |  > 0  |       X      |
     * +-------+  +----------+------+-------+--------------+
     */

    if( ( macMsg->FHDR.FCtrl.Bits.FOptsLen > 0 ) && ( macMsg->FPort > 0 ) )
    {
        *fType = FRAME_TYPE_A;
    }
    else if( macMsg->FRMPayloadSize == 0 )
    {
        *fType = FRAME_TYPE_B;
    }
    else if( ( macMsg->FHDR.FCtrl.Bits.FOptsLen == 0 ) && ( macMsg->FPort == 0 ) )
    {
        *fType = FRAME_TYPE_C;
    }
    else if( ( macMsg->FHDR.FCtrl.Bits.FOptsLen == 0 ) && ( macMsg->FPort > 0 ) )
    {
        *fType = FRAME_TYPE_D;
    }
    else
    {
        // Should never happen.
        return LORAMAC_STATUS_ERROR;
    }

    return LORAMAC_STATUS_OK;
}

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
static bool CheckRetransUnconfirmedUplink( void )
{
    // Unconfirmed uplink, when all retransmissions are done.
    if( MacCtx.ChannelsNbTransCounter >=
        Nvm.MacGroup2.MacParams.ChannelsNbTrans )
    {
        return true;
    }
    else if( MacCtx.MacFlags.Bits.McpsInd == 1 )
    {
        // For Class A stop in each case
        if( Nvm.MacGroup2.DeviceClass == CLASS_A )
        {
            return true;
        }
        else
        {// For Class B & C stop only if the frame was received in RX1 window
            if( MacCtx.RxStatus.RxSlot == RX_SLOT_WIN_1 )
            {
                return true;
            }
        }
    }
    return false;
}

static bool CheckRetransConfirmedUplink( void )
{
    // Confirmed uplink, when all retransmissions ( tries to get a ack ) are done.
    if( MacCtx.AckTimeoutRetriesCounter >=
        MacCtx.AckTimeoutRetries )
    {
        return true;
    }
    else if( MacCtx.MacFlags.Bits.McpsInd == 1 )
    {
        if( MacCtx.McpsConfirm.AckReceived == true )
        {
            return true;
        }
    }
    return false;
}
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
static bool CheckRetrans( uint8_t counter, uint8_t limit )
{
    if( counter >= limit )
    {
        return true;
    }
    return false;
}

static bool CheckRetransUnconfirmedUplink( void )
{
    // Verify, if the max number of retransmissions have been reached
    if( CheckRetrans( MacCtx.ChannelsNbTransCounter,
                      Nvm.MacGroup2.MacParams.ChannelsNbTrans ) == true )
    {
        return true;
    }

    if( MacCtx.MacFlags.Bits.McpsInd == 1 )
    {
        // Stop the retransmissions, if a valid downlink is received
        // a class A RX window. This holds also for class B and C.
        if( ( MacCtx.RxStatus.RxSlot == RX_SLOT_WIN_1 ) ||
            ( MacCtx.RxStatus.RxSlot == RX_SLOT_WIN_2 ) )
        {
            return true;
        }
    }
    return false;
}

static bool CheckRetransConfirmedUplink( void )
{
    // Verify, if the max number of retransmissions have been reached
    if( CheckRetrans( MacCtx.ChannelsNbTransCounter,
                      Nvm.MacGroup2.MacParams.ChannelsNbTrans ) == true )
    {
        return true;
    }

    if( MacCtx.MacFlags.Bits.McpsInd == 1 )
    {
        if( MacCtx.McpsConfirm.AckReceived == true )
        {
            return true;
        }
    }
    return false;
}

static uint32_t IncreaseAdrAckCounter( uint32_t counter )
{
    if( counter < ADR_ACK_COUNTER_MAX )
    {
        counter++;
    }
    return counter;
}
#endif /* LORAMAC_VERSION */

static bool StopRetransmission( void )
{
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    // Increase Rejoin Uplinks counter
    if( Nvm.MacGroup2.Rejoin0UplinksLimit != 0 )
    {
        Nvm.MacGroup1.Rejoin0UplinksCounter++;
    }

    if( Nvm.MacGroup2.Version.Fields.Minor >= 1 )
    {
        MacCommand_t* macCmd;
        if( LoRaMacCommandsGetCmd( MOTE_MAC_REKEY_IND, &macCmd ) == LORAMAC_COMMANDS_SUCCESS )
        {
            // Increase the Rekey Uplinks counter
            Nvm.MacGroup1.RekeyIndUplinksCounter++;

            /*
             * If the device has not received a RekeyConf within
             * the first ADR_ACK_LIMIT uplinks it SHALL revert to the Join state.
             */
            if( Nvm.MacGroup1.RekeyIndUplinksCounter == Nvm.MacGroup2.MacParams.AdrAckLimit )
            {
                Nvm.MacGroup2.NetworkActivation = ACTIVATION_TYPE_NONE;
                MacCtx.MacFlags.Bits.MlmeInd = 1;
                MacCtx.MlmeIndication.MlmeIndication = MLME_REVERT_JOIN;
            }
        }
    }
#endif /* LORAMAC_VERSION */

    if( ( MacCtx.MacFlags.Bits.McpsInd == 0 ) ||
        ( ( MacCtx.RxStatus.RxSlot != RX_SLOT_WIN_1 ) &&
          ( MacCtx.RxStatus.RxSlot != RX_SLOT_WIN_2 ) ) )
    {   // Maximum repetitions without downlink. Increase ADR Ack counter.
        // Only process the case when the MAC did not receive a downlink.
        if( Nvm.MacGroup2.AdrCtrlOn == true )
        {
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
            Nvm.MacGroup1.AdrAckCounter++;
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
            Nvm.MacGroup1.AdrAckCounter = IncreaseAdrAckCounter( Nvm.MacGroup1.AdrAckCounter );
#endif /* LORAMAC_VERSION */
        }
    }

    MacCtx.ChannelsNbTransCounter = 0;
    MacCtx.NodeAckRequested = false;
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    MacCtx.AckTimeoutRetry = false;
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    MacCtx.RetransmitTimeoutRetry = false;
#endif /* LORAMAC_VERSION */
    MacCtx.MacState &= ~LORAMAC_TX_RUNNING;

    return true;
}

static void OnMacProcessNotify( void )
{
    if( ( MacCtx.MacCallbacks != NULL ) && ( MacCtx.MacCallbacks->MacProcessNotify != NULL ) )
    {
        MacCtx.MacCallbacks->MacProcessNotify( );
    }
}

static void CallNvmDataChangeCallback( uint16_t notifyFlags )
{
    if( ( MacCtx.MacCallbacks != NULL ) && ( MacCtx.MacCallbacks->NvmDataChange  != NULL ) )
    {
        MacCtx.MacCallbacks->NvmDataChange ( notifyFlags );
    }
}

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
static void AckTimeoutRetriesProcess( void )
{
    if( MacCtx.AckTimeoutRetriesCounter < MacCtx.AckTimeoutRetries )
    {
        MacCtx.AckTimeoutRetriesCounter++;
        if( ( MacCtx.AckTimeoutRetriesCounter % 2 ) == 1 )
        {
            GetPhyParams_t getPhy;
            PhyParam_t phyParam;

            getPhy.Attribute = PHY_NEXT_LOWER_TX_DR;
            getPhy.UplinkDwellTime = Nvm.MacGroup2.MacParams.UplinkDwellTime;
            getPhy.Datarate = Nvm.MacGroup1.ChannelsDatarate;
            phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
            Nvm.MacGroup1.ChannelsDatarate = phyParam.Value;
        }
    }
}

static void AckTimeoutRetriesFinalize( void )
{
    if( MacCtx.McpsConfirm.AckReceived == false )
    {
        InitDefaultsParams_t params;
        params.Type = INIT_TYPE_ACTIVATE_DEFAULT_CHANNELS;
        params.NvmGroup1 = &Nvm.RegionGroup1;
        params.NvmGroup2 = &Nvm.RegionGroup2;
        RegionInitDefaults( Nvm.MacGroup2.Region, &params );

        MacCtx.NodeAckRequested = false;
        MacCtx.McpsConfirm.AckReceived = false;
    }
    MacCtx.McpsConfirm.NbRetries = MacCtx.AckTimeoutRetriesCounter;
}
#endif /* LORAMAC_VERSION */

static uint8_t IsRequestPending( void )
{
    if( ( MacCtx.MacFlags.Bits.MlmeReq == 1 ) ||
        ( MacCtx.MacFlags.Bits.McpsReq == 1 ) )
    {
        return 1;
    }
    return 0;
}

LoRaMacStatus_t LoRaMacInitialization( LoRaMacPrimitives_t* primitives, LoRaMacCallback_t* callbacks, LoRaMacRegion_t region )
{
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;

    if( ( primitives == NULL ) ||
        ( callbacks == NULL ) )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    if( ( primitives->MacMcpsConfirm == NULL ) ||
        ( primitives->MacMcpsIndication == NULL ) ||
        ( primitives->MacMlmeConfirm == NULL ) ||
        ( primitives->MacMlmeIndication == NULL ) )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    // Verify if the region is supported
    if( RegionIsActive( region ) == false )
    {
        return LORAMAC_STATUS_REGION_NOT_SUPPORTED;
    }

    // Confirm queue reset
    LoRaMacConfirmQueueInit( primitives );

    // Initialize the module context with zeros
    memset1( ( uint8_t* ) &Nvm, 0x00, sizeof( LoRaMacNvmData_t ) );
    memset1( ( uint8_t* ) &MacCtx, 0x00, sizeof( LoRaMacCtx_t ) );

    // Set non zero variables to its default value
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    MacCtx.AckTimeoutRetriesCounter = 1;
    MacCtx.AckTimeoutRetries = 1;
#endif /* LORAMAC_VERSION */
    Nvm.MacGroup2.Region = region;
    Nvm.MacGroup2.DeviceClass = CLASS_A;
    Nvm.MacGroup2.MacParams.RepeaterSupport = false;

    // Setup version
    Nvm.MacGroup2.Version.Value = LORAMAC_VERSION;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    InitDefaultsParams_t params;
    params.Type = INIT_TYPE_DEFAULTS;
    params.NvmGroup1 = &Nvm.RegionGroup1;
    params.NvmGroup2 = &Nvm.RegionGroup2;
    params.Bands = &RegionBands;
    RegionInitDefaults( Nvm.MacGroup2.Region, &params );
#endif /* LORAMAC_VERSION */

    // Reset to defaults
    getPhy.Attribute = PHY_DUTY_CYCLE;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.DutyCycleOn = ( bool ) phyParam.Value;

    getPhy.Attribute = PHY_DEF_TX_POWER;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.ChannelsTxPowerDefault = phyParam.Value;

    getPhy.Attribute = PHY_DEF_TX_DR;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.ChannelsDatarateDefault = phyParam.Value;

    getPhy.Attribute = PHY_MAX_RX_WINDOW;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.MaxRxWindow = phyParam.Value;

    getPhy.Attribute = PHY_RECEIVE_DELAY1;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.ReceiveDelay1 = phyParam.Value;

    getPhy.Attribute = PHY_RECEIVE_DELAY2;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.ReceiveDelay2 = phyParam.Value;

    getPhy.Attribute = PHY_JOIN_ACCEPT_DELAY1;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.JoinAcceptDelay1 = phyParam.Value;

    getPhy.Attribute = PHY_JOIN_ACCEPT_DELAY2;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.JoinAcceptDelay2 = phyParam.Value;

    getPhy.Attribute = PHY_DEF_DR1_OFFSET;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.Rx1DrOffset = phyParam.Value;

    getPhy.Attribute = PHY_DEF_RX2_FREQUENCY;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.Rx2Channel.Frequency = phyParam.Value;
    Nvm.MacGroup2.MacParamsDefaults.RxCChannel.Frequency = phyParam.Value;

    getPhy.Attribute = PHY_DEF_RX2_DR;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.Rx2Channel.Datarate = phyParam.Value;
    Nvm.MacGroup2.MacParamsDefaults.RxCChannel.Datarate = phyParam.Value;

    getPhy.Attribute = PHY_DEF_UPLINK_DWELL_TIME;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.UplinkDwellTime = phyParam.Value;

    getPhy.Attribute = PHY_DEF_DOWNLINK_DWELL_TIME;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.DownlinkDwellTime = phyParam.Value;

    getPhy.Attribute = PHY_DEF_MAX_EIRP;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.MaxEirp = phyParam.fValue;

    getPhy.Attribute = PHY_DEF_ANTENNA_GAIN;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.AntennaGain = phyParam.fValue;

    getPhy.Attribute = PHY_DEF_ADR_ACK_LIMIT;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.AdrAckLimit = phyParam.Value;

    getPhy.Attribute = PHY_DEF_ADR_ACK_DELAY;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    Nvm.MacGroup2.MacParamsDefaults.AdrAckDelay = phyParam.Value;

    // Init parameters which are not set in function ResetMacParameters
    Nvm.MacGroup2.MacParamsDefaults.ChannelsNbTrans = 1;
    Nvm.MacGroup2.MacParamsDefaults.SystemMaxRxError = 10;
    Nvm.MacGroup2.MacParamsDefaults.MinRxSymbols = 6;

    Nvm.MacGroup2.MacParams.SystemMaxRxError = Nvm.MacGroup2.MacParamsDefaults.SystemMaxRxError;
    Nvm.MacGroup2.MacParams.MinRxSymbols = Nvm.MacGroup2.MacParamsDefaults.MinRxSymbols;
    Nvm.MacGroup2.MacParams.MaxRxWindow = Nvm.MacGroup2.MacParamsDefaults.MaxRxWindow;
    Nvm.MacGroup2.MacParams.ReceiveDelay1 = Nvm.MacGroup2.MacParamsDefaults.ReceiveDelay1;
    Nvm.MacGroup2.MacParams.ReceiveDelay2 = Nvm.MacGroup2.MacParamsDefaults.ReceiveDelay2;
    Nvm.MacGroup2.MacParams.JoinAcceptDelay1 = Nvm.MacGroup2.MacParamsDefaults.JoinAcceptDelay1;
    Nvm.MacGroup2.MacParams.JoinAcceptDelay2 = Nvm.MacGroup2.MacParamsDefaults.JoinAcceptDelay2;
    Nvm.MacGroup2.MacParams.ChannelsNbTrans = Nvm.MacGroup2.MacParamsDefaults.ChannelsNbTrans;

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    InitDefaultsParams_t params;
    params.Type = INIT_TYPE_DEFAULTS;
    params.NvmGroup1 = &Nvm.RegionGroup1;
    params.NvmGroup2 = &Nvm.RegionGroup2;
    RegionInitDefaults( Nvm.MacGroup2.Region, &params );
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    // FPort 224 is enabled by default.
    Nvm.MacGroup2.IsCertPortOn = true;
#endif /* LORAMAC_VERSION */

    MacCtx.MacCallbacks = callbacks;
    ResetMacParameters( false );

    Nvm.MacGroup2.PublicNetwork = true;

    MacCtx.MacPrimitives = primitives;
    MacCtx.MacFlags.Value = 0;
    MacCtx.MacState = LORAMAC_STOPPED;

    // Reset duty cycle times
    Nvm.MacGroup1.LastTxDoneTime = 0;
    Nvm.MacGroup1.AggregatedTimeOff = 0;

    // Initialize timers
    TimerInit( &MacCtx.TxDelayedTimer, OnTxDelayedTimerEvent );
    TimerInit( &MacCtx.RxWindowTimer1, OnRxWindow1TimerEvent );
    TimerInit( &MacCtx.RxWindowTimer2, OnRxWindow2TimerEvent );
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    TimerInit( &MacCtx.AckTimeoutTimer, OnAckTimeoutTimerEvent );
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    TimerInit( &MacCtx.RetransmitTimeoutTimer, OnRetransmitTimeoutTimerEvent );
#endif /* LORAMAC_VERSION */
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
    TimerInit( &MacCtx.Rejoin0CycleTimer, OnRejoin0CycleTimerEvent );
    TimerInit( &MacCtx.Rejoin1CycleTimer, OnRejoin1CycleTimerEvent );
    TimerInit( &MacCtx.ForceRejoinReqCycleTimer, OnForceRejoinReqCycleTimerEvent );
#endif /* LORAMAC_VERSION */

    // Store the current initialization time
    Nvm.MacGroup2.InitializationTime = SysTimeGetMcuTime( );

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    // Initialize MAC radio events
    LoRaMacRadioEvents.Value = 0;
#endif /* LORAMAC_VERSION */

    // Initialize Radio driver
    MacCtx.RadioEvents.TxDone = OnRadioTxDone;
    MacCtx.RadioEvents.RxDone = OnRadioRxDone;
    MacCtx.RadioEvents.RxError = OnRadioRxError;
    MacCtx.RadioEvents.TxTimeout = OnRadioTxTimeout;
    MacCtx.RadioEvents.RxTimeout = OnRadioRxTimeout;
    Radio.Init( &MacCtx.RadioEvents );

    // Initialize the Secure Element driver
    if( SecureElementInit( &Nvm.SecureElement ) != SECURE_ELEMENT_SUCCESS )
    {
        return LORAMAC_STATUS_CRYPTO_ERROR;
    }

    // Initialize Crypto module
    if( LoRaMacCryptoInit( &Nvm.Crypto ) != LORAMAC_CRYPTO_SUCCESS )
    {
        return LORAMAC_STATUS_CRYPTO_ERROR;
    }

    // Initialize MAC commands module
    if( LoRaMacCommandsInit( ) != LORAMAC_COMMANDS_SUCCESS )
    {
        return LORAMAC_STATUS_MAC_COMMAD_ERROR;
    }

    // Set multicast downlink counter reference
    if( LoRaMacCryptoSetMulticastReference( Nvm.MacGroup2.MulticastChannelList ) != LORAMAC_CRYPTO_SUCCESS )
    {
        return LORAMAC_STATUS_CRYPTO_ERROR;
    }

    // Random seed initialization
    srand1( Radio.Random( ) );

    Radio.SetPublicNetwork( Nvm.MacGroup2.PublicNetwork );
    Radio.Sleep( );

    LoRaMacEnableRequests( LORAMAC_REQUEST_HANDLING_ON );

    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t LoRaMacStart( void )
{
    MacCtx.MacState = LORAMAC_IDLE;
    UpdateRxSlotIdleState();
    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t LoRaMacStop( void )
{
    if( LoRaMacIsBusy( ) == false )
    {
        if( Nvm.MacGroup2.DeviceClass == CLASS_C )
        {
            Radio.Sleep( );
        }
        MacCtx.MacState = LORAMAC_STOPPED;
        return LORAMAC_STATUS_OK;
    }
    else if(  MacCtx.MacState == LORAMAC_STOPPED )
    {
        return LORAMAC_STATUS_OK;
    }
    return LORAMAC_STATUS_BUSY;
}

LoRaMacStatus_t LoRaMacHalt( void )
{
    // Stop Timers
    TimerStop( &MacCtx.TxDelayedTimer );
    TimerStop( &MacCtx.RxWindowTimer1 );
    TimerStop( &MacCtx.RxWindowTimer2 );
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    TimerStop( &MacCtx.AckTimeoutTimer );
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    TimerStop( &MacCtx.RetransmitTimeoutTimer );
#endif /* LORAMAC_VERSION */

    // Take care about class B
    LoRaMacClassBHaltBeaconing( );

    // Switch off Radio
    Radio.Sleep( );

    MacCtx.MacState = LORAMAC_IDLE;

#if (defined( CONTEXT_MANAGEMENT_ENABLED ) && ( CONTEXT_MANAGEMENT_ENABLED == 1 ))
    LoRaMacHandleNvm( &Nvm );

    // Preserve the Nvm context if data retention
    memcpy1( ( uint8_t* ) &NvmBackup, ( uint8_t* ) &Nvm, sizeof( LoRaMacNvmData_t ) );
#endif /* CONTEXT_MANAGEMENT_ENABLED */

    MacCtx.MacState = LORAMAC_STOPPED;

    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t LoRaMacQueryTxPossible( uint8_t size, LoRaMacTxInfo_t* txInfo )
{
    CalcNextAdrParams_t adrNext;
    uint32_t adrAckCounter = Nvm.MacGroup1.AdrAckCounter;
    int8_t datarate = Nvm.MacGroup2.ChannelsDatarateDefault;
    int8_t txPower = Nvm.MacGroup2.ChannelsTxPowerDefault;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    uint8_t nbTrans = MacCtx.ChannelsNbTransCounter;
#endif /* LORAMAC_VERSION */
    size_t macCmdsSize = 0;

    if( txInfo == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    // Setup ADR request
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    adrNext.Version = Nvm.MacGroup2.Version;
#endif /* LORAMAC_VERSION */
    adrNext.UpdateChanMask = false;
    adrNext.AdrEnabled = Nvm.MacGroup2.AdrCtrlOn;
    adrNext.AdrAckCounter = Nvm.MacGroup1.AdrAckCounter;
    adrNext.AdrAckLimit = Nvm.MacGroup2.MacParams.AdrAckLimit;
    adrNext.AdrAckDelay = Nvm.MacGroup2.MacParams.AdrAckDelay;
    adrNext.Datarate = Nvm.MacGroup1.ChannelsDatarate;
    adrNext.TxPower = Nvm.MacGroup1.ChannelsTxPower;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    adrNext.NbTrans = MacCtx.ChannelsNbTransCounter;
#endif /* LORAMAC_VERSION */
    adrNext.UplinkDwellTime = Nvm.MacGroup2.MacParams.UplinkDwellTime;
    adrNext.Region = Nvm.MacGroup2.Region;

    // We call the function for information purposes only. We don't want to
    // apply the datarate, the tx power and the ADR ack counter.
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    LoRaMacAdrCalcNext( &adrNext, &datarate, &txPower, &adrAckCounter );
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    LoRaMacAdrCalcNext( &adrNext, &datarate, &txPower, &nbTrans, &adrAckCounter );
#endif /* LORAMAC_VERSION */

    txInfo->CurrentPossiblePayloadSize = GetMaxAppPayloadWithoutFOptsLength( datarate );

    if( LoRaMacCommandsGetSizeSerializedCmds( &macCmdsSize ) != LORAMAC_COMMANDS_SUCCESS )
    {
        return LORAMAC_STATUS_MAC_COMMAD_ERROR;
    }

    // Verify if the MAC commands fit into the FOpts and into the maximum payload.
    if( ( LORA_MAC_COMMAND_MAX_FOPTS_LENGTH >= macCmdsSize ) && ( txInfo->CurrentPossiblePayloadSize >= macCmdsSize ) )
    {
        txInfo->MaxPossibleApplicationDataSize = txInfo->CurrentPossiblePayloadSize - macCmdsSize;

        // Verify if the application data together with MAC command fit into the maximum payload.
        if( txInfo->CurrentPossiblePayloadSize >= ( macCmdsSize + size ) )
        {
            return LORAMAC_STATUS_OK;
        }
        else
        {
           return LORAMAC_STATUS_LENGTH_ERROR;
        }
    }
    else
    {
        txInfo->MaxPossibleApplicationDataSize = 0;
        return LORAMAC_STATUS_LENGTH_ERROR;
    }
}

LoRaMacStatus_t LoRaMacMibGetRequestConfirm( MibRequestConfirm_t* mibGet )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_OK;
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;

    if( mibGet == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    switch( mibGet->Type )
    {
        case MIB_DEVICE_CLASS:
        {
            mibGet->Param.Class = Nvm.MacGroup2.DeviceClass;
            break;
        }
        case MIB_NETWORK_ACTIVATION:
        {
            mibGet->Param.NetworkActivation = Nvm.MacGroup2.NetworkActivation;
            break;
        }
        case MIB_DEV_EUI:
        {
            SecureElementGetDevEui( mibGet->Param.DevEui );
            break;
        }
        case MIB_JOIN_EUI:
        {
             SecureElementGetJoinEui( mibGet->Param.JoinEui );
            break;
        }
        case MIB_ADR:
        {
            mibGet->Param.AdrEnable = Nvm.MacGroup2.AdrCtrlOn;
            break;
        }
        case MIB_NET_ID:
        {
            mibGet->Param.NetID = Nvm.MacGroup2.NetID;
            break;
        }
        case MIB_DEV_ADDR:
        {
            SecureElementGetDevAddr( Nvm.MacGroup2.NetworkActivation, &mibGet->Param.DevAddr );
            break;
        }
        case MIB_PUBLIC_NETWORK:
        {
            mibGet->Param.EnablePublicNetwork = Nvm.MacGroup2.PublicNetwork;
            break;
        }
        case MIB_REPEATER_SUPPORT:
        {
            mibGet->Param.EnableRepeaterSupport = Nvm.MacGroup2.MacParams.RepeaterSupport;
            break;
        }
        case MIB_CHANNELS:
        {
            getPhy.Attribute = PHY_CHANNELS;
            phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );

            mibGet->Param.ChannelList = phyParam.Channels;
            break;
        }
        case MIB_RX2_CHANNEL:
        {
            mibGet->Param.Rx2Channel = Nvm.MacGroup2.MacParams.Rx2Channel;
            break;
        }
        case MIB_RX2_DEFAULT_CHANNEL:
        {
            mibGet->Param.Rx2Channel = Nvm.MacGroup2.MacParamsDefaults.Rx2Channel;
            break;
        }
        case MIB_RXC_CHANNEL:
        {
            mibGet->Param.RxCChannel = Nvm.MacGroup2.MacParams.RxCChannel;
            break;
        }
        case MIB_RXC_DEFAULT_CHANNEL:
        {
            mibGet->Param.RxCChannel = Nvm.MacGroup2.MacParamsDefaults.RxCChannel;
            break;
        }
        case MIB_CHANNELS_DEFAULT_MASK:
        {
            getPhy.Attribute = PHY_CHANNELS_DEFAULT_MASK;
            phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );

            mibGet->Param.ChannelsDefaultMask = phyParam.ChannelsMask;
            break;
        }
        case MIB_CHANNELS_MASK:
        {
            getPhy.Attribute = PHY_CHANNELS_MASK;
            phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );

            mibGet->Param.ChannelsMask = phyParam.ChannelsMask;
            break;
        }
        case MIB_CHANNELS_NB_TRANS:
        {
            mibGet->Param.ChannelsNbTrans = Nvm.MacGroup2.MacParams.ChannelsNbTrans;
            break;
        }
        case MIB_MAX_RX_WINDOW_DURATION:
        {
            mibGet->Param.MaxRxWindow = Nvm.MacGroup2.MacParams.MaxRxWindow;
            break;
        }
        case MIB_RECEIVE_DELAY_1:
        {
            mibGet->Param.ReceiveDelay1 = Nvm.MacGroup2.MacParams.ReceiveDelay1;
            break;
        }
        case MIB_RECEIVE_DELAY_2:
        {
            mibGet->Param.ReceiveDelay2 = Nvm.MacGroup2.MacParams.ReceiveDelay2;
            break;
        }
        case MIB_JOIN_ACCEPT_DELAY_1:
        {
            mibGet->Param.JoinAcceptDelay1 = Nvm.MacGroup2.MacParams.JoinAcceptDelay1;
            break;
        }
        case MIB_JOIN_ACCEPT_DELAY_2:
        {
            mibGet->Param.JoinAcceptDelay2 = Nvm.MacGroup2.MacParams.JoinAcceptDelay2;
            break;
        }
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
        case MIB_CHANNELS_MIN_TX_DATARATE:
        {
            getPhy.Attribute = PHY_MIN_TX_DR;
            getPhy.UplinkDwellTime = Nvm.MacGroup2.MacParams.UplinkDwellTime;
            phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );

            mibGet->Param.ChannelsMinTxDatarate = phyParam.Value;
            break;
        }
#endif /* LORAMAC_VERSION */
        case MIB_CHANNELS_DEFAULT_DATARATE:
        {
            mibGet->Param.ChannelsDefaultDatarate = Nvm.MacGroup2.ChannelsDatarateDefault;
            break;
        }
        case MIB_CHANNELS_DATARATE:
        {
            mibGet->Param.ChannelsDatarate = Nvm.MacGroup1.ChannelsDatarate;
            break;
        }
        case MIB_CHANNELS_DEFAULT_TX_POWER:
        {
            mibGet->Param.ChannelsDefaultTxPower = Nvm.MacGroup2.ChannelsTxPowerDefault;
            break;
        }
        case MIB_CHANNELS_TX_POWER:
        {
            mibGet->Param.ChannelsTxPower = Nvm.MacGroup1.ChannelsTxPower;
            break;
        }
        case MIB_SYSTEM_MAX_RX_ERROR:
        {
            mibGet->Param.SystemMaxRxError = Nvm.MacGroup2.MacParams.SystemMaxRxError;
            break;
        }
        case MIB_MIN_RX_SYMBOLS:
        {
            mibGet->Param.MinRxSymbols = Nvm.MacGroup2.MacParams.MinRxSymbols;
            break;
        }
        case MIB_ANTENNA_GAIN:
        {
            mibGet->Param.AntennaGain = Nvm.MacGroup2.MacParams.AntennaGain;
            break;
        }
        case MIB_NVM_CTXS:
        {
            mibGet->Param.Contexts = &Nvm;
            break;
        }
        case MIB_NVM_BKP_CTXS:
        {
#if (defined( CONTEXT_MANAGEMENT_ENABLED ) && ( CONTEXT_MANAGEMENT_ENABLED == 1 ))
            mibGet->Param.BackupContexts = &NvmBackup;
#else
            mibGet->Param.BackupContexts = NULL;
#endif /* CONTEXT_MANAGEMENT_ENABLED */
            break;
        }
        case MIB_DEFAULT_ANTENNA_GAIN:
        {
            mibGet->Param.DefaultAntennaGain = Nvm.MacGroup2.MacParamsDefaults.AntennaGain;
            break;
        }
        case MIB_LORAWAN_VERSION:
        {
            mibGet->Param.LrWanVersion.LoRaWan = Nvm.MacGroup2.Version;
            mibGet->Param.LrWanVersion.LoRaWanRegion = RegionGetVersion( );
            break;
        }
    case MIB_RXB_C_TIMEOUT:
        {
            mibGet->Param.RxBCTimeout = Nvm.MacGroup2.MacParams.RxBCTimeout;
            break;
        }
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
        case MIB_IS_CERT_FPORT_ON:
        {
            mibGet->Param.IsCertPortOn = Nvm.MacGroup2.IsCertPortOn;
            break;
        }
#endif /* LORAMAC_VERSION */
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
        case MIB_REJOIN_0_CYCLE:
        {
            mibGet->Param.Rejoin0CycleInSec = Nvm.MacGroup2.Rejoin0CycleInSec;
            break;
        }
        case MIB_REJOIN_1_CYCLE:
        {
            mibGet->Param.Rejoin1CycleInSec = Nvm.MacGroup2.Rejoin1CycleInSec;
            break;
        }
#endif /* LORAMAC_VERSION */
        case MIB_ADR_ACK_LIMIT:
        {
            mibGet->Param.AdrAckLimit = Nvm.MacGroup2.MacParams.AdrAckLimit;
            break;
        }
        case MIB_ADR_ACK_DELAY:
        {
            mibGet->Param.AdrAckDelay = Nvm.MacGroup2.MacParams.AdrAckDelay;
            break;
        }
        case MIB_ADR_ACK_DEFAULT_LIMIT:
        {
            mibGet->Param.AdrAckLimit = Nvm.MacGroup2.MacParamsDefaults.AdrAckLimit;
            break;
        }
        case MIB_ADR_ACK_DEFAULT_DELAY:
        {
            mibGet->Param.AdrAckDelay = Nvm.MacGroup2.MacParamsDefaults.AdrAckDelay;
            break;
        }
        case MIB_RSSI_FREE_THRESHOLD:
        {
#if defined(REGION_KR920) || defined(REGION_AS923)
            if( Nvm.MacGroup2.Region != LORAMAC_REGION_AS923 && Nvm.MacGroup2.Region != LORAMAC_REGION_KR920 )
            {
                status = LORAMAC_STATUS_ERROR;
            }
            else
            {
                mibGet->Param.RssiFreeThreshold = Nvm.RegionGroup2.RssiFreeThreshold;
            }
#else
            status = LORAMAC_STATUS_ERROR;
#endif
            break;
        }
        case MIB_CARRIER_SENSE_TIME:
        {
#if defined(REGION_KR920) || defined(REGION_AS923)
            if( Nvm.MacGroup2.Region != LORAMAC_REGION_AS923 && Nvm.MacGroup2.Region != LORAMAC_REGION_KR920 )
            {
                status = LORAMAC_STATUS_ERROR;
            }
            else
            {
                mibGet->Param.CarrierSenseTime = Nvm.RegionGroup2.CarrierSenseTime;
            }
#else
            status = LORAMAC_STATUS_ERROR;
#endif
            break;
        }
        default:
        {
            status = LoRaMacClassBMibGetRequestConfirm( mibGet );
            break;
        }
    }
    return status;
}

LoRaMacStatus_t LoRaMacMibSetRequestConfirm( MibRequestConfirm_t* mibSet )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_OK;
    ChanMaskSetParams_t chanMaskSet;
    VerifyParams_t verify;

    if( mibSet == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    if( ( MacCtx.MacState & LORAMAC_TX_RUNNING ) == LORAMAC_TX_RUNNING )
    {
        return LORAMAC_STATUS_BUSY;
    }

    switch( mibSet->Type )
    {
        case MIB_DEVICE_CLASS:
        {
            status = SwitchClass( mibSet->Param.Class );
            break;
        }
        case MIB_NETWORK_ACTIVATION:
        {
            if( mibSet->Param.NetworkActivation != ACTIVATION_TYPE_OTAA  )
            {
                Nvm.MacGroup2.NetworkActivation = mibSet->Param.NetworkActivation;
            }
            else
            {   // Do not allow to set ACTIVATION_TYPE_OTAA since the MAC will set it automatically after a successful join process.
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_DEV_EUI:
        {
            if( SecureElementSetDevEui( mibSet->Param.DevEui ) != SECURE_ELEMENT_SUCCESS )
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_JOIN_EUI:
        {
            if( SecureElementSetJoinEui( mibSet->Param.JoinEui ) != SECURE_ELEMENT_SUCCESS )
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_ADR:
        {
            Nvm.MacGroup2.AdrCtrlOn = mibSet->Param.AdrEnable;
            break;
        }
        case MIB_NET_ID:
        {
            Nvm.MacGroup2.NetID = mibSet->Param.NetID;
            break;
        }
        case MIB_DEV_ADDR:
        {
            if(SecureElementSetDevAddr( Nvm.MacGroup2.NetworkActivation, mibSet->Param.DevAddr ) != SECURE_ELEMENT_SUCCESS )
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            else
            {
                /* Update Nvm.MacGroup2.devAdr to handle set/get sequence */
                Nvm.MacGroup2.DevAddr = mibSet->Param.DevAddr;
            }
            break;
        }
        case MIB_APP_KEY:
        {
            if( mibSet->Param.AppKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( APP_KEY, mibSet->Param.AppKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_NWK_KEY:
        {
            if( mibSet->Param.NwkKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( NWK_KEY, mibSet->Param.NwkKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
        case MIB_J_S_INT_KEY:
        {
            if( mibSet->Param.JSIntKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( J_S_INT_KEY, mibSet->Param.JSIntKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_J_S_ENC_KEY:
        {
            if( mibSet->Param.JSEncKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( J_S_ENC_KEY, mibSet->Param.JSEncKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_F_NWK_S_INT_KEY:
        {
            if( mibSet->Param.FNwkSIntKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( F_NWK_S_INT_KEY, mibSet->Param.FNwkSIntKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_S_NWK_S_INT_KEY:
        {
            if( mibSet->Param.SNwkSIntKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( S_NWK_S_INT_KEY, mibSet->Param.SNwkSIntKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_NWK_S_ENC_KEY:
        {
            if( mibSet->Param.NwkSEncKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( NWK_S_ENC_KEY, mibSet->Param.NwkSEncKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
#else
        case MIB_NWK_S_KEY:
        {
            if( mibSet->Param.NwkSKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( NWK_S_KEY, mibSet->Param.NwkSKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
#endif /* LORAMAC_VERSION */
        case MIB_APP_S_KEY:
        {
            if( mibSet->Param.AppSKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( APP_S_KEY, mibSet->Param.AppSKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_KE_KEY:
        {
            if( mibSet->Param.McKEKey != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_KE_KEY, mibSet->Param.McKEKey ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
#if ( LORAMAC_MAX_MC_CTX > 0 )
        case MIB_MC_KEY_0:
        {
            if( mibSet->Param.McKey0 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_KEY_0, mibSet->Param.McKey0 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_APP_S_KEY_0:
        {
            if( mibSet->Param.McAppSKey0 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_APP_S_KEY_0, mibSet->Param.McAppSKey0 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_NWK_S_KEY_0:
        {
            if( mibSet->Param.McNwkSKey0 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_NWK_S_KEY_0, mibSet->Param.McNwkSKey0 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
#endif /* LORAMAC_MAX_MC_CTX > 0 */
#if ( LORAMAC_MAX_MC_CTX > 1 )
        case MIB_MC_KEY_1:
        {
            if( mibSet->Param.McKey1 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_KEY_1, mibSet->Param.McKey1 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_APP_S_KEY_1:
        {
            if( mibSet->Param.McAppSKey1 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_APP_S_KEY_1, mibSet->Param.McAppSKey1 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_NWK_S_KEY_1:
        {
            if( mibSet->Param.McNwkSKey1 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_NWK_S_KEY_1, mibSet->Param.McNwkSKey1 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
#endif /* LORAMAC_MAX_MC_CTX > 1 */
#if ( LORAMAC_MAX_MC_CTX > 2 )
        case MIB_MC_KEY_2:
        {
            if( mibSet->Param.McKey2 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_KEY_2, mibSet->Param.McKey2 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_APP_S_KEY_2:
        {
            if( mibSet->Param.McAppSKey2 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_APP_S_KEY_2, mibSet->Param.McAppSKey2 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_NWK_S_KEY_2:
        {
            if( mibSet->Param.McNwkSKey2 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_NWK_S_KEY_2, mibSet->Param.McNwkSKey2 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
#endif /* LORAMAC_MAX_MC_CTX > 2 */
#if ( LORAMAC_MAX_MC_CTX > 3 )
        case MIB_MC_KEY_3:
        {
            if( mibSet->Param.McKey3 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_KEY_3, mibSet->Param.McKey3 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_APP_S_KEY_3:
        {
            if( mibSet->Param.McAppSKey3 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_APP_S_KEY_3, mibSet->Param.McAppSKey3 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MC_NWK_S_KEY_3:
        {
            if( mibSet->Param.McNwkSKey3 != NULL )
            {
                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MC_NWK_S_KEY_3, mibSet->Param.McNwkSKey3 ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
#endif /* LORAMAC_MAX_MC_CTX > 3 */
        case MIB_PUBLIC_NETWORK:
        {
            Nvm.MacGroup2.PublicNetwork = mibSet->Param.EnablePublicNetwork;
            Radio.SetPublicNetwork( Nvm.MacGroup2.PublicNetwork );
            Radio.Sleep( );
            break;
        }
        case MIB_REPEATER_SUPPORT:
        {
            Nvm.MacGroup2.MacParams.RepeaterSupport = mibSet->Param.EnableRepeaterSupport;
            break;
        }
        case MIB_RX2_CHANNEL:
        {
            verify.DatarateParams.Datarate = mibSet->Param.Rx2Channel.Datarate;
            verify.DatarateParams.DownlinkDwellTime = Nvm.MacGroup2.MacParams.DownlinkDwellTime;
            if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_RX_DR ) != true )
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            else
            {
                verify.Frequency = mibSet->Param.Rx2Channel.Frequency;
                if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_FREQUENCY ) != true )
                {
                    status = LORAMAC_STATUS_PARAMETER_INVALID;
                }
                else
                {
                    Nvm.MacGroup2.MacParams.Rx2Channel = mibSet->Param.Rx2Channel;
                }
            }
            break;
        }
        case MIB_RX2_DEFAULT_CHANNEL:
        {
            verify.DatarateParams.Datarate = mibSet->Param.Rx2Channel.Datarate;
            verify.DatarateParams.DownlinkDwellTime = Nvm.MacGroup2.MacParams.DownlinkDwellTime;

            if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_RX_DR ) == true )
            {
                Nvm.MacGroup2.MacParamsDefaults.Rx2Channel = mibSet->Param.Rx2DefaultChannel;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_RXC_CHANNEL:
        {
            verify.DatarateParams.Datarate = mibSet->Param.RxCChannel.Datarate;
            verify.DatarateParams.DownlinkDwellTime = Nvm.MacGroup2.MacParams.DownlinkDwellTime;

            if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_RX_DR ) == true )
            {
                Nvm.MacGroup2.MacParams.RxCChannel = mibSet->Param.RxCChannel;

                if( ( Nvm.MacGroup2.DeviceClass == CLASS_C ) && ( Nvm.MacGroup2.NetworkActivation != ACTIVATION_TYPE_NONE ) )
                {
                    // We can only compute the RX window parameters directly, if we are already
                    // in class c mode and joined. We cannot setup an RX window in case of any other
                    // class type.
                    // Set the radio into sleep mode in case we are still in RX mode
                    Radio.Sleep( );

                    OpenContinuousRxCWindow( );
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_RXC_DEFAULT_CHANNEL:
        {
            verify.DatarateParams.Datarate = mibSet->Param.RxCChannel.Datarate;
            verify.DatarateParams.DownlinkDwellTime = Nvm.MacGroup2.MacParams.DownlinkDwellTime;

            if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_RX_DR ) == true )
            {
                Nvm.MacGroup2.MacParamsDefaults.RxCChannel = mibSet->Param.RxCDefaultChannel;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_CHANNELS_DEFAULT_MASK:
        {
            chanMaskSet.ChannelsMaskIn = mibSet->Param.ChannelsDefaultMask;
            chanMaskSet.ChannelsMaskType = CHANNELS_DEFAULT_MASK;

            if( RegionChanMaskSet( Nvm.MacGroup2.Region, &chanMaskSet ) == false )
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_CHANNELS_MASK:
        {
            chanMaskSet.ChannelsMaskIn = mibSet->Param.ChannelsMask;
            chanMaskSet.ChannelsMaskType = CHANNELS_MASK;

            if( RegionChanMaskSet( Nvm.MacGroup2.Region, &chanMaskSet ) == false )
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_CHANNELS_NB_TRANS:
        {
            if( ( mibSet->Param.ChannelsNbTrans >= 1 ) &&
                ( mibSet->Param.ChannelsNbTrans <= 15 ) )
            {
                Nvm.MacGroup2.MacParams.ChannelsNbTrans = mibSet->Param.ChannelsNbTrans;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_MAX_RX_WINDOW_DURATION:
        {
            Nvm.MacGroup2.MacParams.MaxRxWindow = mibSet->Param.MaxRxWindow;
            break;
        }
        case MIB_RECEIVE_DELAY_1:
        {
            Nvm.MacGroup2.MacParams.ReceiveDelay1 = mibSet->Param.ReceiveDelay1;
            break;
        }
        case MIB_RECEIVE_DELAY_2:
        {
            Nvm.MacGroup2.MacParams.ReceiveDelay2 = mibSet->Param.ReceiveDelay2;
            break;
        }
        case MIB_JOIN_ACCEPT_DELAY_1:
        {
            Nvm.MacGroup2.MacParams.JoinAcceptDelay1 = mibSet->Param.JoinAcceptDelay1;
            break;
        }
        case MIB_JOIN_ACCEPT_DELAY_2:
        {
            Nvm.MacGroup2.MacParams.JoinAcceptDelay2 = mibSet->Param.JoinAcceptDelay2;
            break;
        }
        case MIB_CHANNELS_DEFAULT_DATARATE:
        {
            verify.DatarateParams.Datarate = mibSet->Param.ChannelsDefaultDatarate;

            if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_DEF_TX_DR ) == true )
            {
                Nvm.MacGroup2.ChannelsDatarateDefault = verify.DatarateParams.Datarate;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_CHANNELS_DATARATE:
        {
            verify.DatarateParams.Datarate = mibSet->Param.ChannelsDatarate;
            verify.DatarateParams.UplinkDwellTime = Nvm.MacGroup2.MacParams.UplinkDwellTime;

            if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_TX_DR ) == true )
            {
                Nvm.MacGroup1.ChannelsDatarate = verify.DatarateParams.Datarate;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_CHANNELS_DEFAULT_TX_POWER:
        {
            verify.TxPower = mibSet->Param.ChannelsDefaultTxPower;

            if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_DEF_TX_POWER ) == true )
            {
                Nvm.MacGroup2.ChannelsTxPowerDefault = verify.TxPower;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_CHANNELS_TX_POWER:
        {
            verify.TxPower = mibSet->Param.ChannelsTxPower;

            if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_TX_POWER ) == true )
            {
                Nvm.MacGroup1.ChannelsTxPower = verify.TxPower;
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_SYSTEM_MAX_RX_ERROR:
        {
            Nvm.MacGroup2.MacParams.SystemMaxRxError = Nvm.MacGroup2.MacParamsDefaults.SystemMaxRxError = mibSet->Param.SystemMaxRxError;
            break;
        }
        case MIB_MIN_RX_SYMBOLS:
        {
            Nvm.MacGroup2.MacParams.MinRxSymbols = Nvm.MacGroup2.MacParamsDefaults.MinRxSymbols = mibSet->Param.MinRxSymbols;
            break;
        }
        case MIB_ANTENNA_GAIN:
        {
            Nvm.MacGroup2.MacParams.AntennaGain = mibSet->Param.AntennaGain;
            break;
        }
        case MIB_DEFAULT_ANTENNA_GAIN:
        {
            Nvm.MacGroup2.MacParamsDefaults.AntennaGain = mibSet->Param.DefaultAntennaGain;
            break;
        }
        case MIB_NVM_CTXS:
        {
            status = RestoreNvmData( );
            break;
        }
        case MIB_ABP_LORAWAN_VERSION:
        {
            if( mibSet->Param.AbpLrWanVersion.Fields.Minor <= 1 )
            {
                Nvm.MacGroup2.Version = mibSet->Param.AbpLrWanVersion;

                if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetLrWanVersion( mibSet->Param.AbpLrWanVersion ) )
                {
                    return LORAMAC_STATUS_CRYPTO_ERROR;
                }
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_RXB_C_TIMEOUT:
        {
            Nvm.MacGroup2.MacParams.RxBCTimeout = mibSet->Param.RxBCTimeout;
            break;
        }
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
        case MIB_IS_CERT_FPORT_ON:
        {
            Nvm.MacGroup2.IsCertPortOn = mibSet->Param.IsCertPortOn;
            break;
        }
#endif /* LORAMAC_VERSION */
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
        case MIB_REJOIN_0_CYCLE:
        {
            uint32_t cycleTime = 0;
            if( ( ConvertRejoinCycleTime( mibSet->Param.Rejoin0CycleInSec, &cycleTime ) == true ) &&
                ( Nvm.MacGroup2.NetworkActivation == ACTIVATION_TYPE_OTAA ) )
            {
                Nvm.MacGroup2.Rejoin0CycleInSec = mibSet->Param.Rejoin0CycleInSec;
                MacCtx.Rejoin0CycleTime = cycleTime;
                TimerStop( &MacCtx.Rejoin0CycleTimer );
                TimerSetValue( &MacCtx.Rejoin0CycleTimer, MacCtx.Rejoin0CycleTime );
                TimerStart( &MacCtx.Rejoin0CycleTimer );
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
        case MIB_REJOIN_1_CYCLE:
        {
            uint32_t cycleTime = 0;
            if( ( ConvertRejoinCycleTime( mibSet->Param.Rejoin1CycleInSec, &cycleTime ) == true ) &&
                ( Nvm.MacGroup2.NetworkActivation == ACTIVATION_TYPE_OTAA ) )
            {
                Nvm.MacGroup2.Rejoin1CycleInSec = mibSet->Param.Rejoin1CycleInSec;
                MacCtx.Rejoin0CycleTime = cycleTime;
                TimerStop( &MacCtx.Rejoin1CycleTimer );
                TimerSetValue( &MacCtx.Rejoin1CycleTimer, MacCtx.Rejoin1CycleTime );
                TimerStart( &MacCtx.Rejoin1CycleTimer );
            }
            else
            {
                status = LORAMAC_STATUS_PARAMETER_INVALID;
            }
            break;
        }
#endif /* LORAMAC_VERSION */
        case MIB_ADR_ACK_LIMIT:
        {
            Nvm.MacGroup2.MacParams.AdrAckLimit = mibSet->Param.AdrAckLimit;
            break;
        }
        case MIB_ADR_ACK_DELAY:
        {
            Nvm.MacGroup2.MacParams.AdrAckDelay = mibSet->Param.AdrAckDelay;
            break;
        }
        case MIB_ADR_ACK_DEFAULT_LIMIT:
        {
            Nvm.MacGroup2.MacParamsDefaults.AdrAckLimit = mibSet->Param.AdrAckLimit;
            break;
        }
        case MIB_ADR_ACK_DEFAULT_DELAY:
        {
            Nvm.MacGroup2.MacParamsDefaults.AdrAckDelay = mibSet->Param.AdrAckDelay;
            break;
        }
        case MIB_RSSI_FREE_THRESHOLD:
        {
#if defined(REGION_KR920) || defined(REGION_AS923)
            if( Nvm.MacGroup2.Region != LORAMAC_REGION_AS923 && Nvm.MacGroup2.Region != LORAMAC_REGION_KR920 )
            {
                status = LORAMAC_STATUS_ERROR;
            }
            else
            {
                Nvm.RegionGroup2.RssiFreeThreshold = mibSet->Param.RssiFreeThreshold;
            }
#else
            status = LORAMAC_STATUS_ERROR;
#endif
            break;
        }
        case MIB_CARRIER_SENSE_TIME:
        {
#if defined(REGION_KR920) || defined(REGION_AS923)
            if( Nvm.MacGroup2.Region != LORAMAC_REGION_AS923 && Nvm.MacGroup2.Region != LORAMAC_REGION_KR920 )
            {
                status = LORAMAC_STATUS_ERROR;
            }
            else
            {
                Nvm.RegionGroup2.CarrierSenseTime = mibSet->Param.CarrierSenseTime;
            }
#else
            status = LORAMAC_STATUS_ERROR;
#endif
            break;
        }
        default:
        {
            status = LoRaMacMibClassBSetRequestConfirm( mibSet );
            break;
        }
    }

#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    if( status == LORAMAC_STATUS_OK )
    {
        // Handle NVM potential changes
        MacCtx.MacFlags.Bits.NvmHandle = 1;
    }
#endif /* LORAMAC_VERSION */
    return status;
}

LoRaMacStatus_t LoRaMacChannelAdd( uint8_t id, ChannelParams_t params )
{
    ChannelAddParams_t channelAdd;

    // Validate if the MAC is in a correct state
    if( ( MacCtx.MacState & LORAMAC_TX_RUNNING ) == LORAMAC_TX_RUNNING )
    {
        if( ( MacCtx.MacState & LORAMAC_TX_CONFIG ) != LORAMAC_TX_CONFIG )
        {
            return LORAMAC_STATUS_BUSY;
        }
    }

    channelAdd.NewChannel = &params;
    channelAdd.ChannelId = id;
    return RegionChannelAdd( Nvm.MacGroup2.Region, &channelAdd );
}

LoRaMacStatus_t LoRaMacChannelRemove( uint8_t id )
{
    ChannelRemoveParams_t channelRemove;

    if( ( MacCtx.MacState & LORAMAC_TX_RUNNING ) == LORAMAC_TX_RUNNING )
    {
        if( ( MacCtx.MacState & LORAMAC_TX_CONFIG ) != LORAMAC_TX_CONFIG )
        {
            return LORAMAC_STATUS_BUSY;
        }
    }

    channelRemove.ChannelId = id;

    if( RegionChannelsRemove( Nvm.MacGroup2.Region, &channelRemove ) == false )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }
    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t LoRaMacMcChannelSetup( McChannelParams_t *channel )
{
    if( ( MacCtx.MacState & LORAMAC_TX_RUNNING ) == LORAMAC_TX_RUNNING )
    {
        return LORAMAC_STATUS_BUSY;
    }

    if( channel->GroupID >= LORAMAC_MAX_MC_CTX )
    {
        return LORAMAC_STATUS_MC_GROUP_UNDEFINED;
    }

    Nvm.MacGroup2.MulticastChannelList[channel->GroupID].ChannelParams = *channel;
    MacCtx.MacFlags.Bits.NvmHandle = 1;

    if( channel->IsRemotelySetup == true )
    {
        if( LoRaMacCryptoSetKey( MCKeys[channel->GroupID], channel->McKeys.McKeyE ) != LORAMAC_CRYPTO_SUCCESS )
        {
            return LORAMAC_STATUS_CRYPTO_ERROR;
        }

        if( LoRaMacCryptoDeriveMcSessionKeyPair( channel->GroupID, channel->Address ) != LORAMAC_CRYPTO_SUCCESS )
        {
            return LORAMAC_STATUS_CRYPTO_ERROR;
        }
    }
    else
    {
        if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MCAppSKeys[channel->GroupID], channel->McKeys.Session.McAppSKey ) )
        {
            return LORAMAC_STATUS_CRYPTO_ERROR;
        }
        if( LORAMAC_CRYPTO_SUCCESS != LoRaMacCryptoSetKey( MCNwkSKeys[channel->GroupID], channel->McKeys.Session.McNwkSKey ) )
        {
            return LORAMAC_STATUS_CRYPTO_ERROR;
        }
    }

    // Reset multicast channel downlink counter to initial value.
    *Nvm.MacGroup2.MulticastChannelList[channel->GroupID].DownLinkCounter = FCNT_DOWN_INITIAL_VALUE;
    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t LoRaMacMcChannelDelete( AddressIdentifier_t groupID )
{
    if( ( MacCtx.MacState & LORAMAC_TX_RUNNING ) == LORAMAC_TX_RUNNING )
    {
        return LORAMAC_STATUS_BUSY;
    }

    if( ( groupID >= LORAMAC_MAX_MC_CTX ) ||
        ( Nvm.MacGroup2.MulticastChannelList[groupID].ChannelParams.IsEnabled == false ) )
    {
        return LORAMAC_STATUS_MC_GROUP_UNDEFINED;
    }

    McChannelParams_t channel;

    // Set all channel fields with 0
    memset1( ( uint8_t* )&channel, 0, sizeof( McChannelParams_t ) );

    Nvm.MacGroup2.MulticastChannelList[groupID].ChannelParams = channel;
    MacCtx.MacFlags.Bits.NvmHandle = 1;
    return LORAMAC_STATUS_OK;
}

uint8_t LoRaMacMcChannelGetGroupId( uint32_t mcAddress )
{
    for( uint8_t i = 0; i < LORAMAC_MAX_MC_CTX; i++ )
    {
        if( mcAddress == Nvm.MacGroup2.MulticastChannelList[i].ChannelParams.Address )
        {
            return i;
        }
    }
    return 0xFF;
}

LoRaMacStatus_t LoRaMacMcChannelSetupRxParams( AddressIdentifier_t groupID, McRxParams_t *rxParams, uint8_t *status )
{
   *status = 0x1C + ( groupID & 0x03 );

    if( ( MacCtx.MacState & LORAMAC_TX_RUNNING ) == LORAMAC_TX_RUNNING )
    {
        return LORAMAC_STATUS_BUSY;
    }

    if( ( rxParams->Class == CLASS_A ) || ( rxParams->Class > CLASS_C ) )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    if( ( groupID >= LORAMAC_MAX_MC_CTX ) ||
        ( Nvm.MacGroup2.MulticastChannelList[groupID].ChannelParams.IsEnabled == false ) )
    {
        return LORAMAC_STATUS_MC_GROUP_UNDEFINED;
    }
    *status &= 0x0F; // groupID OK

    VerifyParams_t verify;
    // Check datarate
    if( rxParams->Class == CLASS_B )
    {
        verify.DatarateParams.Datarate = rxParams->Params.ClassB.Datarate;
    }
    else
    {
        verify.DatarateParams.Datarate = rxParams->Params.ClassC.Datarate;
    }
    verify.DatarateParams.DownlinkDwellTime = Nvm.MacGroup2.MacParams.DownlinkDwellTime;

    if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_RX_DR ) == true )
    {
        *status &= 0xFB; // datarate OK
    }

    // Check frequency
    if( rxParams->Class == CLASS_B )
    {
        verify.Frequency = rxParams->Params.ClassB.Frequency;
    }
    else
    {
        verify.Frequency = rxParams->Params.ClassC.Frequency;
    }
    if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_FREQUENCY ) == true )
    {
        *status &= 0xF7; // frequency OK
    }

    if( *status == ( groupID & 0x03 ) )
    {
        // Apply parameters
        Nvm.MacGroup2.MulticastChannelList[groupID].ChannelParams.RxParams = *rxParams;
        MacCtx.MacFlags.Bits.NvmHandle = 1;
    }
    else
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }

    if( rxParams->Class == CLASS_B )
    {
        // Calculate class b parameters
        LoRaMacClassBSetMulticastPeriodicity( &Nvm.MacGroup2.MulticastChannelList[groupID] );
    }
    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t LoRaMacProcessMicForDatablock( uint8_t *buffer, uint32_t size, uint16_t sessionCnt, uint8_t fragIndex, uint32_t descriptor, uint32_t *mic )
{
    LoRaMacCryptoStatus_t macCryptoStatus = LORAMAC_CRYPTO_ERROR;

    macCryptoStatus = LoRaMacCryptoComputeDataBlock( buffer, size, sessionCnt, fragIndex, descriptor, mic );
    if( LORAMAC_CRYPTO_SUCCESS != macCryptoStatus )
    {
        return LORAMAC_STATUS_CRYPTO_ERROR;
    }

    return LORAMAC_STATUS_OK;
}

LoRaMacStatus_t LoRaMacMlmeRequest( MlmeReq_t* mlmeRequest )
{
    LoRaMacStatus_t status = LORAMAC_STATUS_SERVICE_UNKNOWN;
    MlmeConfirmQueue_t queueElement;
    uint8_t macCmdPayload[2] = { 0x00, 0x00 };

    if( mlmeRequest == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    // Initialize mlmeRequest->ReqReturn.DutyCycleWaitTime to 0 in order to
    // return a valid value in case the MAC is busy.
    mlmeRequest->ReqReturn.DutyCycleWaitTime = 0;
#endif /* LORAMAC_VERSION */

    if( LoRaMacIsBusy( ) == true )
    {
        return LORAMAC_STATUS_BUSY;
    }
    if( LoRaMacConfirmQueueIsFull( ) == true )
    {
        return LORAMAC_STATUS_BUSY;
    }

    if( LoRaMacConfirmQueueGetCnt( ) == 0 )
    {
        memset1( ( uint8_t* ) &MacCtx.MlmeConfirm, 0, sizeof( MacCtx.MlmeConfirm ) );
    }
    MacCtx.MlmeConfirm.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;

    MacCtx.MacFlags.Bits.MlmeReq = 1;
    queueElement.Request = mlmeRequest->Type;
    queueElement.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;
    queueElement.RestrictCommonReadyToHandle = false;
#if (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    queueElement.ReadyToHandle = false;
#endif /* LORAMAC_VERSION */

    switch( mlmeRequest->Type )
    {
        case MLME_JOIN:
        {
            if( ( MacCtx.MacState & LORAMAC_TX_DELAYED ) == LORAMAC_TX_DELAYED )
            {
                return LORAMAC_STATUS_BUSY;
            }

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
            ResetMacParameters( false );

            Nvm.MacGroup1.ChannelsDatarate = RegionAlternateDr( Nvm.MacGroup2.Region, mlmeRequest->Req.Join.Datarate, ALTERNATE_DR );
            Nvm.MacGroup1.ChannelsTxPower = mlmeRequest->Req.Join.TxPower;

            queueElement.Status = LORAMAC_EVENT_INFO_STATUS_JOIN_FAIL;

            status = SendReJoinReq( JOIN_REQ );

            if( status != LORAMAC_STATUS_OK )
            {
                // Revert back the previous datarate ( mainly used for US915 like regions )
                Nvm.MacGroup1.ChannelsDatarate = RegionAlternateDr( Nvm.MacGroup2.Region, mlmeRequest->Req.Join.Datarate, ALTERNATE_DR_RESTORE );
            }
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
            if( mlmeRequest->Req.Join.NetworkActivation == ACTIVATION_TYPE_OTAA )
            {
                ResetMacParameters( false );

                Nvm.MacGroup1.ChannelsDatarate = RegionAlternateDr( Nvm.MacGroup2.Region, mlmeRequest->Req.Join.Datarate, ALTERNATE_DR );
                Nvm.MacGroup1.ChannelsTxPower = mlmeRequest->Req.Join.TxPower;

                queueElement.Status = LORAMAC_EVENT_INFO_STATUS_JOIN_FAIL;

                status = SendReJoinReq( JOIN_REQ );

                if( status != LORAMAC_STATUS_OK )
                {
                    // Revert back the previous datarate ( mainly used for US915 like regions )
                    Nvm.MacGroup1.ChannelsDatarate = RegionAlternateDr( Nvm.MacGroup2.Region, mlmeRequest->Req.Join.Datarate, ALTERNATE_DR_RESTORE );
                }
            }
            else if( mlmeRequest->Req.Join.NetworkActivation == ACTIVATION_TYPE_ABP )
            {
                // Restore default value for ChannelsDatarateChangedLinkAdrReq
                Nvm.MacGroup2.ChannelsDatarateChangedLinkAdrReq = false;

                // Activate the default channels
                InitDefaultsParams_t params;
                params.Type = INIT_TYPE_ACTIVATE_DEFAULT_CHANNELS;
                RegionInitDefaults( Nvm.MacGroup2.Region, &params );

                Nvm.MacGroup2.NetworkActivation = mlmeRequest->Req.Join.NetworkActivation;
                queueElement.Status = LORAMAC_EVENT_INFO_STATUS_OK;
                queueElement.ReadyToHandle = true;
                OnMacProcessNotify( );
                MacCtx.MacFlags.Bits.MacDone = 1;
                status = LORAMAC_STATUS_OK;
            }
#endif /* LORAMAC_VERSION */
            break;
        }
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
        case MLME_REJOIN_0:
        {
            MacCtx.MacFlags.Bits.MlmeReq = 1;
            MacCtx.MlmeConfirm.MlmeRequest = mlmeRequest->Type;

            status = SendReJoinReq( REJOIN_REQ_0 );

            break;
        }
        case MLME_REJOIN_1:
        {
            MacCtx.MacFlags.Bits.MlmeReq = 1;
            MacCtx.MlmeConfirm.MlmeRequest = mlmeRequest->Type;

            status = SendReJoinReq( REJOIN_REQ_1 );

            break;
        }
        case MLME_REJOIN_2:
        {
            MacCtx.MacFlags.Bits.MlmeReq = 1;
            MacCtx.MlmeConfirm.MlmeRequest = mlmeRequest->Type;

            status = SendReJoinReq( REJOIN_REQ_2 );

            break;
        }
#endif /* LORAMAC_VERSION */
        case MLME_LINK_CHECK:
        {
            // LoRaMac will send this command piggy-pack
            status = LORAMAC_STATUS_OK;
            if( LoRaMacCommandsAddCmd( MOTE_MAC_LINK_CHECK_REQ, macCmdPayload, 0 ) != LORAMAC_COMMANDS_SUCCESS )
            {
                status = LORAMAC_STATUS_MAC_COMMAD_ERROR;
            }
            break;
        }
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
        case MLME_TXCW:
        {
            status = SetTxContinuousWave( mlmeRequest->Req.TxCw.Timeout );
            break;
        }
        case MLME_TXCW_1:
        {

            status = SetTxContinuousWave1( mlmeRequest->Req.TxCw.Timeout, mlmeRequest->Req.TxCw.Frequency, mlmeRequest->Req.TxCw.Power );
            break;
        }
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
        case MLME_TXCW:
        {
            status = SetTxContinuousWave( mlmeRequest->Req.TxCw.Timeout, mlmeRequest->Req.TxCw.Frequency, mlmeRequest->Req.TxCw.Power );
            break;
        }
#endif /* LORAMAC_VERSION */
        case MLME_DEVICE_TIME:
        {
            // LoRaMac will send this command piggy-pack
            status = LORAMAC_STATUS_OK;
            MacCommand_t* newCmd;
            /* ST_CODE Begin: Add MAC command condition to prevent some duplicated request */
            if (LoRaMacCommandsGetCmd( MOTE_MAC_DEVICE_TIME_REQ, &newCmd ) == LORAMAC_COMMANDS_SUCCESS)
            {
                status = LORAMAC_STATUS_OK;
            }
            /* ST_CODE End */
            else if( LoRaMacCommandsAddCmd( MOTE_MAC_DEVICE_TIME_REQ, macCmdPayload, 0 ) != LORAMAC_COMMANDS_SUCCESS )
            {
                status = LORAMAC_STATUS_MAC_COMMAD_ERROR;
            }
            break;
        }
        case MLME_PING_SLOT_INFO:
        {
            if( Nvm.MacGroup2.DeviceClass == CLASS_A )
            {
                uint8_t value = mlmeRequest->Req.PingSlotInfo.PingSlot.Value;

                // LoRaMac will send this command piggy-pack
                LoRaMacClassBSetPingSlotInfo( mlmeRequest->Req.PingSlotInfo.PingSlot.Fields.Periodicity );
                macCmdPayload[0] = value;
                status = LORAMAC_STATUS_OK;
                if( LoRaMacCommandsAddCmd( MOTE_MAC_PING_SLOT_INFO_REQ, macCmdPayload, 1 ) != LORAMAC_COMMANDS_SUCCESS )
                {
                    status = LORAMAC_STATUS_MAC_COMMAD_ERROR;
                }
            }
            break;
        }
        case MLME_BEACON_TIMING:
        {
            // LoRaMac will send this command piggy-pack
            status = LORAMAC_STATUS_OK;
            if( LoRaMacCommandsAddCmd( MOTE_MAC_BEACON_TIMING_REQ, macCmdPayload, 0 ) != LORAMAC_COMMANDS_SUCCESS )
            {
                status = LORAMAC_STATUS_MAC_COMMAD_ERROR;
            }
            break;
        }
        case MLME_BEACON_ACQUISITION:
        {
            // Apply the request
            queueElement.RestrictCommonReadyToHandle = true;

            if( LoRaMacClassBIsAcquisitionInProgress( ) == false )
            {
                // Start class B algorithm
                LoRaMacClassBSetBeaconState( BEACON_STATE_ACQUISITION );
                LoRaMacClassBBeaconTimerEvent( NULL );

                status = LORAMAC_STATUS_OK;
            }
            else
            {
                status = LORAMAC_STATUS_BUSY;
            }
            break;
        }
        default:
            break;
    }

    // Fill return structure
    mlmeRequest->ReqReturn.DutyCycleWaitTime = MacCtx.DutyCycleWaitTime;

    if( status != LORAMAC_STATUS_OK )
    {
        if( LoRaMacConfirmQueueGetCnt( ) == 0 )
        {
            MacCtx.NodeAckRequested = false;
            MacCtx.MacFlags.Bits.MlmeReq = 0;
        }
    }
    else
    {
        LoRaMacConfirmQueueAdd( &queueElement );
    }
    return status;
}

LoRaMacStatus_t LoRaMacMcpsRequest( McpsReq_t* mcpsRequest, bool allowDelayedTx )
{
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;
    LoRaMacStatus_t status = LORAMAC_STATUS_SERVICE_UNKNOWN;
    LoRaMacHeader_t macHdr;
    VerifyParams_t verify;
    uint8_t fPort = 0;
    void* fBuffer = NULL;
    uint16_t fBufferSize;
    int8_t datarate = DR_0;
    bool readyToSend = false;

    if( mcpsRequest == NULL )
    {
        return LORAMAC_STATUS_PARAMETER_INVALID;
    }
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
    if( LoRaMacIsBusy( ) == true )
    {
        return LORAMAC_STATUS_BUSY;
    }

    macHdr.Value = 0;
    memset1( ( uint8_t* ) &MacCtx.McpsConfirm, 0, sizeof( MacCtx.McpsConfirm ) );
    MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;

    // AckTimeoutRetriesCounter must be reset every time a new request (unconfirmed or confirmed) is performed.
    MacCtx.AckTimeoutRetriesCounter = 1;

    switch( mcpsRequest->Type )
    {
        case MCPS_UNCONFIRMED:
        {
            readyToSend = true;
            MacCtx.AckTimeoutRetries = 1;

            macHdr.Bits.MType = FRAME_TYPE_DATA_UNCONFIRMED_UP;
            fPort = mcpsRequest->Req.Unconfirmed.fPort;
            fBuffer = mcpsRequest->Req.Unconfirmed.fBuffer;
            fBufferSize = mcpsRequest->Req.Unconfirmed.fBufferSize;
            datarate = mcpsRequest->Req.Unconfirmed.Datarate;
            break;
        }
        case MCPS_CONFIRMED:
        {
            readyToSend = true;
            MacCtx.AckTimeoutRetries = MIN( mcpsRequest->Req.Confirmed.NbTrials, MAX_ACK_RETRIES );

            macHdr.Bits.MType = FRAME_TYPE_DATA_CONFIRMED_UP;
            fPort = mcpsRequest->Req.Confirmed.fPort;
            fBuffer = mcpsRequest->Req.Confirmed.fBuffer;
            fBufferSize = mcpsRequest->Req.Confirmed.fBufferSize;
            datarate = mcpsRequest->Req.Confirmed.Datarate;
            break;
        }
        case MCPS_PROPRIETARY:
        {
            readyToSend = true;
            MacCtx.AckTimeoutRetries = 1;

            macHdr.Bits.MType = FRAME_TYPE_PROPRIETARY;
            fBuffer = mcpsRequest->Req.Proprietary.fBuffer;
            fBufferSize = mcpsRequest->Req.Proprietary.fBufferSize;
            datarate = mcpsRequest->Req.Proprietary.Datarate;
            break;
        }
        default:
            break;
    }

    // Get the minimum possible datarate
    getPhy.Attribute = PHY_MIN_TX_DR;
    getPhy.UplinkDwellTime = Nvm.MacGroup2.MacParams.UplinkDwellTime;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    // Apply the minimum possible datarate.
    // Some regions have limitations for the minimum datarate.
    datarate = MAX( datarate, ( int8_t )phyParam.Value );

    if( readyToSend == true )
    {
        if( Nvm.MacGroup2.AdrCtrlOn == false )
        {
            verify.DatarateParams.Datarate = datarate;
            verify.DatarateParams.UplinkDwellTime = Nvm.MacGroup2.MacParams.UplinkDwellTime;

            if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_TX_DR ) == true )
            {
                Nvm.MacGroup1.ChannelsDatarate = verify.DatarateParams.Datarate;
            }
            else
            {
                return LORAMAC_STATUS_PARAMETER_INVALID;
            }
        }

        status = Send( &macHdr, fPort, fBuffer, fBufferSize, allowDelayedTx );
        if( status == LORAMAC_STATUS_OK )
        {
            MacCtx.McpsConfirm.McpsRequest = mcpsRequest->Type;
            MacCtx.MacFlags.Bits.McpsReq = 1;
        }
        else
        {
            MacCtx.NodeAckRequested = false;
        }
    }
#elif (defined( LORAMAC_VERSION ) && (( LORAMAC_VERSION == 0x01000400 ) || ( LORAMAC_VERSION == 0x01010100 )))
    // Initialize mcpsRequest->ReqReturn.DutyCycleWaitTime to 0 in order to
    // return a valid value in case the MAC is busy.
    mcpsRequest->ReqReturn.DutyCycleWaitTime = 0;

    if( LoRaMacIsBusy( ) == true )
    {
        return LORAMAC_STATUS_BUSY;
    }

    McpsReq_t request = *mcpsRequest;

    macHdr.Value = 0;
    memset1( ( uint8_t* ) &MacCtx.McpsConfirm, 0, sizeof( MacCtx.McpsConfirm ) );
    MacCtx.McpsConfirm.Status = LORAMAC_EVENT_INFO_STATUS_ERROR;

    // Apply confirmed downlinks, if the device has not received a valid
    // downlink after a join accept.
    if( ( Nvm.MacGroup2.NetworkActivation == ACTIVATION_TYPE_OTAA ) &&
        ( Nvm.MacGroup2.DeviceClass == CLASS_C ) &&
        ( Nvm.MacGroup2.DownlinkReceived == false ) &&
        ( request.Type == MCPS_UNCONFIRMED ) )
    {
        request.Type = MCPS_CONFIRMED;
    }

    switch( request.Type )
    {
        case MCPS_UNCONFIRMED:
        {
            readyToSend = true;

            macHdr.Bits.MType = FRAME_TYPE_DATA_UNCONFIRMED_UP;
            fPort = request.Req.Unconfirmed.fPort;
            fBuffer = request.Req.Unconfirmed.fBuffer;
            fBufferSize = request.Req.Unconfirmed.fBufferSize;
            datarate = request.Req.Unconfirmed.Datarate;
            break;
        }
        case MCPS_CONFIRMED:
        {
            readyToSend = true;

            macHdr.Bits.MType = FRAME_TYPE_DATA_CONFIRMED_UP;
            fPort = request.Req.Confirmed.fPort;
            fBuffer = request.Req.Confirmed.fBuffer;
            fBufferSize = request.Req.Confirmed.fBufferSize;
            datarate = request.Req.Confirmed.Datarate;
            break;
        }
        case MCPS_PROPRIETARY:
        {
            readyToSend = true;

            macHdr.Bits.MType = FRAME_TYPE_PROPRIETARY;
            fBuffer = request.Req.Proprietary.fBuffer;
            fBufferSize = request.Req.Proprietary.fBufferSize;
            datarate = request.Req.Proprietary.Datarate;
            break;
        }
        default:
            break;
    }

    // Make sure that the input datarate is compliant
    // to the regional specification.
    getPhy.Attribute = PHY_MIN_TX_DR;
    getPhy.UplinkDwellTime = Nvm.MacGroup2.MacParams.UplinkDwellTime;
    phyParam = RegionGetPhyParam( Nvm.MacGroup2.Region, &getPhy );
    // Apply the minimum possible datarate.
    // Some regions have limitations for the minimum datarate.
    datarate = MAX( datarate, ( int8_t )phyParam.Value );

    // Apply minimum datarate in this special case.
    if( CheckForMinimumAbpDatarate( Nvm.MacGroup2.AdrCtrlOn, Nvm.MacGroup2.NetworkActivation,
                                    Nvm.MacGroup2.ChannelsDatarateChangedLinkAdrReq ) == true )
    {
        datarate = ( int8_t )phyParam.Value;
    }

    if( readyToSend == true )
    {
        if( ( Nvm.MacGroup2.AdrCtrlOn == false ) ||
            ( CheckForMinimumAbpDatarate( Nvm.MacGroup2.AdrCtrlOn, Nvm.MacGroup2.NetworkActivation,
                                          Nvm.MacGroup2.ChannelsDatarateChangedLinkAdrReq ) == true ) )
        {
            verify.DatarateParams.Datarate = datarate;
            verify.DatarateParams.UplinkDwellTime = Nvm.MacGroup2.MacParams.UplinkDwellTime;

            if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_TX_DR ) == true )
            {
                Nvm.MacGroup1.ChannelsDatarate = verify.DatarateParams.Datarate;
            }
            else
            {
                return LORAMAC_STATUS_PARAMETER_INVALID;
            }
        }

        // Verification of response timeout for class b and class c
        LoRaMacHandleResponseTimeout( Nvm.MacGroup2.MacParams.RxBCTimeout,
                                      MacCtx.ResponseTimeoutStartTime );

        status = Send( &macHdr, fPort, fBuffer, fBufferSize, allowDelayedTx );
        if( status == LORAMAC_STATUS_OK )
        {
            MacCtx.McpsConfirm.McpsRequest = request.Type;
            MacCtx.MacFlags.Bits.McpsReq = 1;
        }
        else
        {
            MacCtx.NodeAckRequested = false;
        }
    }
#endif /* LORAMAC_VERSION */

    // Fill return structure
    mcpsRequest->ReqReturn.DutyCycleWaitTime = MacCtx.DutyCycleWaitTime;

    return status;
}

#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01010100 ))
static bool ConvertRejoinCycleTime( uint32_t rejoinCycleTime, uint32_t* timeInMiliSec )
{
    // Our timer implementation do not allow longer times than 4294967295 ms
    if( rejoinCycleTime <= 4294967 )
    {
        *timeInMiliSec = rejoinCycleTime * 1000;
        return true;
    }
    else
    {
        return false;
    }
}

static void OnRejoin0CycleTimerEvent( void* context )
{
    TimerStop( &MacCtx.Rejoin0CycleTimer );
    ConvertRejoinCycleTime( Nvm.MacGroup2.Rejoin0CycleInSec, &MacCtx.Rejoin0CycleTime );

    OnMacProcessNotify( );

    Nvm.MacGroup2.IsRejoin0RequestQueued = true;

    TimerSetValue( &MacCtx.Rejoin0CycleTimer, MacCtx.Rejoin0CycleTime );
    TimerStart( &MacCtx.Rejoin0CycleTimer );
}

static void OnRejoin1CycleTimerEvent( void* context )
{
    TimerStop( &MacCtx.Rejoin1CycleTimer );
    ConvertRejoinCycleTime( Nvm.MacGroup2.Rejoin1CycleInSec, &MacCtx.Rejoin1CycleTime );

    OnMacProcessNotify( );

    Nvm.MacGroup2.IsRejoin1RequestQueued = true;

    TimerSetValue( &MacCtx.Rejoin1CycleTimer, MacCtx.Rejoin1CycleTime );
    TimerStart( &MacCtx.Rejoin1CycleTimer );
}

static void OnForceRejoinReqCycleTimerEvent( void* context )
{
    Nvm.MacGroup1.ForceRejoinRetriesCounter++;
    if( ( Nvm.MacGroup2.ForceRejoinType == 0 ) || ( Nvm.MacGroup2.ForceRejoinType == 1 ) )
    {
        Nvm.MacGroup2.IsRejoin0RequestQueued = true;
    }
    else
    {
        Nvm.MacGroup2.IsRejoin2RequestQueued = true;
    }

    if( Nvm.MacGroup1.ForceRejoinRetriesCounter >= Nvm.MacGroup2.ForceRejoinMaxRetries )
    {
        TimerStop( &MacCtx.ForceRejoinReqCycleTimer );
        Nvm.MacGroup1.ForceRejoinRetriesCounter = 0;
    }
    else
    {
        TimerSetValue( &MacCtx.ForceRejoinReqCycleTimer, MacCtx.ForceRejoinCycleTime );
        TimerStart( &MacCtx.ForceRejoinReqCycleTimer );
    }

    OnMacProcessNotify( );
}
#endif /* LORAMAC_VERSION */

void LoRaMacTestSetDutyCycleOn( bool enable )
{
    VerifyParams_t verify;

    verify.DutyCycle = enable;

    if( RegionVerify( Nvm.MacGroup2.Region, &verify, PHY_DUTY_CYCLE ) == true )
    {
        Nvm.MacGroup2.DutyCycleOn = enable;
        // Handle NVM potential changes
        MacCtx.MacFlags.Bits.NvmHandle = 1;
    }
}

LoRaMacStatus_t LoRaMacDeInitialization( void )
{
    // Check the current state of the LoRaMac
    if ( LoRaMacStop( ) == LORAMAC_STATUS_OK )
    {
        // Stop Timers
        TimerStop( &MacCtx.TxDelayedTimer );
        TimerStop( &MacCtx.RxWindowTimer1 );
        TimerStop( &MacCtx.RxWindowTimer2 );
#if (defined( LORAMAC_VERSION ) && ( LORAMAC_VERSION == 0x01000300 ))
        TimerStop( &MacCtx.AckTimeoutTimer );
#endif /* LORAMAC_VERSION */

        // Take care about class B
        LoRaMacClassBHaltBeaconing( );

        // Reset Mac parameters
        ResetMacParameters( false );

        // Switch off Radio
        Radio.Sleep( );

        // Return success
        return LORAMAC_STATUS_OK;
    }
    else
    {
        return LORAMAC_STATUS_BUSY;
    }
}

uint8_t LoRaMacGetMaxPayloadLength(void)
{
    return GetMaxAppPayloadWithoutFOptsLength(Nvm.MacGroup2.ChannelsDatarateDefault);
}

void LoRaMacSendInfoGet(uint32_t *count, uint32_t *time)
{
    if(count != NULL) {
        *count = MacCtx.SumSendCount;
    }
    if(time != NULL) {
        *time = MacCtx.SumSendTime;
    }
}

void LoRaMacSendInfoClear(void)
{
    MacCtx.SumSendCount = 0;
    MacCtx.SumSendTime = 0;
}