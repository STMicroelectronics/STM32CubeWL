/*!
 * \file      radio.c
 *
 * \brief     Radio driver API definition
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
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    radio.c
  * @author  MCD Application Team
  * @brief   Radio driver API definition
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include "timer.h"
#include "radio.h"
#include "radio_fw.h"
#include "radio_driver.h"
#include "radio_conf.h"
#include "mw_log_conf.h"

/* Private typedef -----------------------------------------------------------*/
/* ST_WORKAROUND_BEGIN: Replace global variables by typedef struct */
/*!
 * Radio hardware and global parameters
 */
typedef struct SubgRf_s
{
    RadioModems_t Modem;
    bool RxContinuous;
    uint32_t TxTimeout;
    uint32_t RxTimeout;
    struct
    {
        bool Previous;
        bool Current;
    } PublicNetwork;
    PacketParams_t PacketParams;
    PacketStatus_t PacketStatus;
    ModulationParams_t ModulationParams;
    RadioIrqMasks_t RadioIrq;
    uint8_t AntSwitchPaSelect;
} SubgRf_t;
/* ST_WORKAROUND_END */

/* Private macro -------------------------------------------------------------*/
/* ST_WORKAROUND_BEGIN: Add utilities macro to prevent missing header file */
#define RADIO_BIT_MASK(__n)  (~(1<<__n))

/**
  * \brief Calculates ceiling division of ( X / N )
  *
  * \param [IN] X numerator
  * \param [IN] N denominator
  *
  */
#ifndef DIVC
#define DIVC( X, N )                ( ( ( X ) + ( N ) - 1 ) / ( N ) )
#endif

/**
  * \brief Calculates rounding division of ( X / N )
  *
  * \param [IN] X numerator
  * \param [IN] N denominator
  *
  */
#ifndef DIVR
#define DIVR( X, N )                ( ( ( X ) + ( ((X)>0?(N):(N))>>1 ) ) / ( N ) )
#endif
/* ST_WORKAROUND_END */

/* Private define ------------------------------------------------------------*/
/* ST_WORKAROUND_BEGIN: Add radio defines to prevent missing header file */
/*can be overridden in radio_conf.h*/
#ifndef XTAL_FREQ
#define XTAL_FREQ                                   32000000UL
#endif
/*can be overridden in radio_conf.h*/
#ifndef RADIO_IRQ_PROCESS_INIT
#define RADIO_IRQ_PROCESS_INIT()
#endif
/*can be overridden in radio_conf.h*/
#ifndef RADIO_IRQ_PROCESS
#define RADIO_IRQ_PROCESS()        RadioIrqProcess()
#endif
/*can be overridden in radio_conf.h*/
#ifndef RADIO_RX_TIMEOUT_PROCESS
#define RADIO_RX_TIMEOUT_PROCESS() RadioOnRxTimeoutProcess()
#endif
/*can be overridden in radio_conf.h*/
#ifndef RADIO_TX_TIMEOUT_PROCESS
#define RADIO_TX_TIMEOUT_PROCESS() RadioOnTxTimeoutProcess()
#endif

#ifndef IRQ_TX_DBG
#define IRQ_TX_DBG ((uint16_t) 0)
#endif

#ifndef IRQ_RX_DBG
#define IRQ_RX_DBG ((uint16_t) 0)
#endif

#define RADIO_BUF_SIZE 255
/* ST_WORKAROUND_END */

/* Private function prototypes -----------------------------------------------*/
/*!
 * \brief Initializes the radio
 *
 * \param [IN] events Structure containing the driver callback functions
 */
static void RadioInit( RadioEvents_t *events );

/*!
 * Return current radio status
 *
 * \param status Radio status.[RF_IDLE, RF_RX_RUNNING, RF_TX_RUNNING]
 */
static RadioState_t RadioGetStatus( void );

/*!
 * \brief Configures the radio with the given modem
 *
 * \param [IN] modem Modem to be used [0: FSK, 1: LoRa]
 */
static void RadioSetModem( RadioModems_t modem );

/*!
 * \brief Sets the channel frequency
 *
 * \param [IN] freq         Channel RF frequency
 */
static void RadioSetChannel( uint32_t freq );

/*!
 * \brief Checks if the channel is free for the given time
 *
 * \remark The FSK modem is always used for this task as we can select the Rx bandwidth at will.
 *
 * \param [IN] freq                Channel RF frequency in Hertz
 * \param [IN] rxBandwidth         Rx bandwidth in Hertz
 * \param [IN] rssiThresh          RSSI threshold in dBm
 * \param [IN] maxCarrierSenseTime Max time in milliseconds while the RSSI is measured
 *
 * \retval isFree         [true: Channel is free, false: Channel is not free]
 */
static bool RadioIsChannelFree( uint32_t freq, uint32_t rxBandwidth, int16_t rssiThresh, uint32_t maxCarrierSenseTime );

/*!
 * \brief Generates a 32 bits random value based on the RSSI readings
 *
 * \remark This function sets the radio in LoRa modem mode and disables
 *         all interrupts.
 *         After calling this function either Radio.SetRxConfig or
 *         Radio.SetTxConfig functions must be called.
 *
 * \retval randomValue    32 bits random value
 */
static uint32_t RadioRandom( void );

/*!
 * \brief Sets the reception parameters
 *
 * \param [IN] modem        Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] bandwidth    Sets the bandwidth
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param [IN] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param [IN] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param [IN] bandwidthAfc Sets the AFC Bandwidth (FSK only)
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: N/A ( set to 0 )
 * \param [IN] preambleLen  Sets the Preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param [IN] symbTimeout  Sets the RxSingle timeout value
 *                          FSK : timeout in number of bytes
 *                          LoRa: timeout in symbols
 * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param [IN] payloadLen   Sets payload length when fixed length is used
 * \param [IN] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]
 * \param [IN] FreqHopOn    Enables disables the intra-packet frequency hopping
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: OFF, 1: ON]
 * \param [IN] HopPeriod    Number of symbols between each hop
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: Number of symbols
 * \param [IN] iqInverted   Inverts IQ signals (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: not inverted, 1: inverted]
 * \param [IN] rxContinuous Sets the reception in continuous mode
 *                          [false: single mode, true: continuous mode]
 */
static void RadioSetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                              uint32_t datarate, uint8_t coderate,
                              uint32_t bandwidthAfc, uint16_t preambleLen,
                              uint16_t symbTimeout, bool fixLen,
                              uint8_t payloadLen,
                              bool crcOn, bool FreqHopOn, uint8_t HopPeriod,
                              bool iqInverted, bool rxContinuous );

/*!
 * \brief Sets the transmission parameters
 *
 * \param [IN] modem        Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] power        Sets the output power [dBm]
 * \param [IN] fdev         Sets the frequency deviation (FSK only)
 *                          FSK : [Hz]
 *                          LoRa: 0
 * \param [IN] bandwidth    Sets the bandwidth (LoRa only)
 *                          FSK : 0
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param [IN] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param [IN] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param [IN] preambleLen  Sets the preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param [IN] crcOn        Enables disables the CRC [0: OFF, 1: ON]
 * \param [IN] FreqHopOn    Enables disables the intra-packet frequency hopping
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: OFF, 1: ON]
 * \param [IN] HopPeriod    Number of symbols between each hop
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: Number of symbols
 * \param [IN] iqInverted   Inverts IQ signals (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [0: not inverted, 1: inverted]
 * \param [IN] timeout      Transmission timeout [ms]
 */
static void RadioSetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev,
                              uint32_t bandwidth, uint32_t datarate,
                              uint8_t coderate, uint16_t preambleLen,
                              bool fixLen, bool crcOn, bool FreqHopOn,
                              uint8_t HopPeriod, bool iqInverted, uint32_t timeout );

/*!
 * \brief Checks if the given RF frequency is supported by the hardware
 *
 * \param [IN] frequency RF frequency to be checked
 * \retval isSupported [true: supported, false: unsupported]
 */
static bool RadioCheckRfFrequency( uint32_t frequency );

/*!
 * \brief Computes the packet time on air in ms for the given payload
 *
 * \Remark Can only be called once SetRxConfig or SetTxConfig have been called
 *
 * \param [IN] modem      Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] bandwidth    Sets the bandwidth
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param [IN] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param [IN] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param [IN] preambleLen  Sets the Preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param [IN] payloadLen   Sets payload length when fixed length is used
 * \param [IN] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]
 *
 * \retval airTime        Computed airTime (ms) for the given packet payload length
 */
static uint32_t RadioTimeOnAir( RadioModems_t modem, uint32_t bandwidth,
                                uint32_t datarate, uint8_t coderate,
                                uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                                bool crcOn );

/*!
 * \brief Sends the buffer of size. Prepares the packet to be sent and sets
 *        the radio in transmission
 *
 * \param [IN]: buffer     Buffer pointer
 * \param [IN]: size       Buffer size
 */
static void RadioSend( uint8_t *buffer, uint8_t size );

/*!
 * \brief Sets the radio in sleep mode
 */
static void RadioSleep( void );

/*!
 * \brief Sets the radio in standby mode
 */
static void RadioStandby( void );

/*!
 * \brief Sets the radio in reception mode for the given time
 * \param [IN] timeout Reception timeout [ms]
 *                     [0: continuous, others timeout]
 */
static void RadioRx( uint32_t timeout );

/*!
 * \brief Start a Channel Activity Detection
 */
static void RadioStartCad( void );

/*!
 * \brief Sets the radio in continuous wave transmission mode
 *
 * \param [IN]: freq       Channel RF frequency
 * \param [IN]: power      Sets the output power [dBm]
 * \param [IN]: time       Transmission mode timeout [s]
 */
static void RadioSetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time );

/*!
 * \brief Reads the current RSSI value
 *
 * \retval rssiValue Current RSSI value in [dBm]
 */
static int16_t RadioRssi( RadioModems_t modem );

/* ST_WORKAROUND_BEGIN: Force register addr to uint16_t */
/*!
 * \brief Writes the radio register at the specified address
 *
 * \param [IN]: addr Register address
 * \param [IN]: data New register value
 */
static void RadioWrite( uint16_t addr, uint8_t data );

/*!
 * \brief Reads the radio register at the specified address
 *
 * \param [IN]: addr Register address
 * \retval data Register value
 */
static uint8_t RadioRead( uint16_t addr );

/*!
 * \brief Writes multiple radio registers starting at address
 *
 * \param [IN] addr   First Radio register address
 * \param [IN] buffer Buffer containing the new register's values
 * \param [IN] size   Number of registers to be written
 */
static void RadioWriteRegisters( uint16_t addr, uint8_t *buffer, uint8_t size );

/*!
 * \brief Reads multiple radio registers starting at address
 *
 * \param [IN] addr First Radio register address
 * \param [OUT] buffer Buffer where to copy the registers data
 * \param [IN] size Number of registers to be read
 */
static void RadioReadRegisters( uint16_t addr, uint8_t *buffer, uint8_t size );
/* ST_WORKAROUND_END */

/*!
 * \brief Sets the maximum payload length.
 *
 * \param [IN] modem      Radio modem to be used [0: FSK, 1: LoRa]
 * \param [IN] max        Maximum payload length in bytes
 */
static void RadioSetMaxPayloadLength( RadioModems_t modem, uint8_t max );

/*!
 * \brief Sets the network to public or private. Updates the sync byte.
 *
 * \remark Applies to LoRa modem only
 *
 * \param [IN] enable if true, it enables a public network
 */
static void RadioSetPublicNetwork( bool enable );

/*!
 * \brief Gets the time required for the board plus radio to get out of sleep.[ms]
 *
 * \retval time Radio plus board wakeup time in ms.
 */
static uint32_t RadioGetWakeupTime( void );

/*!
 * \brief Process radio irq
 */
static void RadioIrqProcess( void );

/*!
 * \brief Sets the radio in reception mode with Max LNA gain for the given time
 * \param [IN] timeout Reception timeout [ms]
 *                     [0: continuous, others timeout]
 */
static void RadioRxBoosted( uint32_t timeout );

/*!
 * \brief Sets the Rx duty cycle management parameters
 *
 * \param [in]  rxTime        Structure describing reception timeout value
 * \param [in]  sleepTime     Structure describing sleep timeout value
 */
static void RadioSetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime );

/*!
 * \brief DIO 0 IRQ callback
 */
static void RadioOnDioIrq( RadioIrqMasks_t radioIrq );

/*!
 * \brief Tx timeout timer callback
 */
static void RadioOnTxTimeoutIrq( void * context );

/*!
 * \brief Rx timeout timer callback
 */
static void RadioOnRxTimeoutIrq( void * context );

/*!
 * \brief Rx timeout timer process
 */
static void RadioOnRxTimeoutProcess( void );

/*!
 * \brief Tx timeout timer process
 */
static void RadioOnTxTimeoutProcess( void );

/* ST_WORKAROUND_BEGIN: extended radio functions */
/*!
 * @brief D-BPSK to BPSK
 *
 * @param [out] outBuffer     buffer with frame encoded
 * @param [in]  inBuffer      buffer with frame to encode
 * @param [in]  size          size of the payload to encode
 */
static void payload_integration( uint8_t *outBuffer, uint8_t *inBuffer, uint8_t size);

/*!
 * \brief Set Tx PRBS modulated wave
 * \retval none
 */
static void RadioTxPrbs( void );

/*!
 * \brief Set Tx continuous wave
 * \retval none
 */
static void RadioTxCw( int8_t power );

/*!
 * \brief Sets the reception parameters
 *
 * \param [IN] modem        Radio modem to be used [GENERIC_FSK or GENERIC_FSK]
 * \param [IN] config       configuration of receiver
 *                          fsk field to be used if modem =GENERIC_FSK
*                           lora field to be used if modem =GENERIC_LORA
 * \param [IN] rxContinuous Sets the reception in continuous mode
 *                          [0: single mode, otherwise continuous mode]
 * \param [IN] symbTimeout  Sets the RxSingle timeout value
 *                          FSK : timeout in number of bytes
 *                          LoRa: timeout in symbols
 * \return 0 when no parameters error, -1 otherwise
 */
static int32_t RadioSetRxGenericConfig(GenericModems_t modem, RxConfigGeneric_t* config,
                                       uint32_t rxContinuous, uint32_t symbTimeout);

/*!
 * \brief Sets the transmission parameters
 *
 * \param [IN] modem        Radio modem to be used [GENERIC_FSK or GENERIC_FSK or GENERIC_BPSK]
 * \param [IN] config       configuration of receiver
 *                          fsk field to be used if modem =GENERIC_FSK
*                           lora field to be used if modem =GENERIC_LORA
                            bpsk field to be used if modem =GENERIC_BPSK
 * \param [IN] power        Sets the output power [dBm]
 * \param [IN] timeout      Transmission timeout [ms]
 * \return 0 when no parameters error, -1 otherwise
 */
static int32_t RadioSetTxGenericConfig(GenericModems_t modem, TxConfigGeneric_t *config,
                                       int8_t power, uint32_t timeout);

/*!
 * \brief Convert the bandwidth enum to Hz value
 *
 * \param [IN] bw RF frequency to be checked
 * \retval bandwidthInHz bandwidth value in Hertz
 */
static uint32_t RadioGetLoRaBandwidthInHz( RadioLoRaBandwidths_t bw );

/*!
 * \brief Computes the time on air GFSK numerator
 *
 * \param [IN] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param [IN] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param [IN] preambleLen  Sets the Preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param [IN] payloadLen   Sets payload length when fixed length is used
 * \param [IN] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]
 * \retval numerator        time on air GFSK numerator
 */
static uint32_t RadioGetGfskTimeOnAirNumerator( uint32_t datarate, uint8_t coderate,
                                                uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                                                bool crcOn );

/*!
 * \brief Computes the time on air LoRa numerator
 *
 * \param [IN] bandwidth    Sets the bandwidth
 *                          FSK : >= 2600 and <= 250000 Hz
 *                          LoRa: [0: 125 kHz, 1: 250 kHz,
 *                                 2: 500 kHz, 3: Reserved]
 * \param [IN] datarate     Sets the Datarate
 *                          FSK : 600..300000 bits/s
 *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
 *                                10: 1024, 11: 2048, 12: 4096  chips]
 * \param [IN] coderate     Sets the coding rate (LoRa only)
 *                          FSK : N/A ( set to 0 )
 *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
 * \param [IN] preambleLen  Sets the Preamble length
 *                          FSK : Number of bytes
 *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
 * \param [IN] fixLen       Fixed length packets [0: variable, 1: fixed]
 * \param [IN] payloadLen   Sets payload length when fixed length is used
 * \param [IN] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]
 * \retval numerator        time on air LoRa numerator
 */
static uint32_t RadioGetLoRaTimeOnAirNumerator( uint32_t bandwidth,
                                                uint32_t datarate, uint8_t coderate,
                                                uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                                                bool crcOn );
/* ST_WORKAROUND_END */

/* Private variables ---------------------------------------------------------*/
/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio =
{
    RadioInit,
    RadioGetStatus,
    RadioSetModem,
    RadioSetChannel,
    RadioIsChannelFree,
    RadioRandom,
    RadioSetRxConfig,
    RadioSetTxConfig,
    RadioCheckRfFrequency,
    RadioTimeOnAir,
    RadioSend,
    RadioSleep,
    RadioStandby,
    RadioRx,
    RadioStartCad,
    RadioSetTxContinuousWave,
    RadioRssi,
    RadioWrite,
    RadioRead,
    RadioWriteRegisters,
    RadioReadRegisters,
    RadioSetMaxPayloadLength,
    RadioSetPublicNetwork,
    RadioGetWakeupTime,
    RadioIrqProcess,
    RadioRxBoosted,
    RadioSetRxDutyCycle,
    /* ST_WORKAROUND_BEGIN: extended radio functions */
    RadioTxPrbs,
    RadioTxCw,
    RadioSetRxGenericConfig,
    RadioSetTxGenericConfig,
    RFW_TransmitLongPacket,
    RFW_ReceiveLongPacket
    /* ST_WORKAROUND_END */
};

const RadioLoRaBandwidths_t Bandwidths[] = { LORA_BW_125, LORA_BW_250, LORA_BW_500 };

static uint8_t MaxPayloadLength = RADIO_BUF_SIZE;

static uint8_t RadioBuffer[RADIO_BUF_SIZE];

/*
 * Radio callbacks variable
 */
static RadioEvents_t* RadioEvents;

/*!
 * Radio hardware and global parameters
 */
SubgRf_t SubgRf;

/*!
 * Tx and Rx timers
 */
TimerEvent_t TxTimeoutTimer;
TimerEvent_t RxTimeoutTimer;

/* Private  functions ---------------------------------------------------------*/

static void RadioInit( RadioEvents_t *events )
{
    RadioEvents = events;

    SubgRf.RxContinuous = false;
    SubgRf.TxTimeout = 0;
    SubgRf.RxTimeout = 0;

    SUBGRF_Init( RadioOnDioIrq );
    /*SubgRf.publicNetwork set to false*/
    RadioSetPublicNetwork( false );

    RADIO_IRQ_PROCESS_INIT();

    SUBGRF_SetRegulatorMode(  );

    SUBGRF_SetBufferBaseAddress( 0x00, 0x00 );
    SUBGRF_SetTxParams(RFO_LP, 0, RADIO_RAMP_200_US);
    SUBGRF_SetDioIrqParams( IRQ_RADIO_ALL, IRQ_RADIO_ALL, IRQ_RADIO_NONE, IRQ_RADIO_NONE );

    /* ST_WORKAROUND_BEGIN: Sleep radio */
    RadioSleep();
    /* ST_WORKAROUND_END */
    // Initialize driver timeout timers
    TimerInit( &TxTimeoutTimer, RadioOnTxTimeoutIrq );
    TimerInit( &RxTimeoutTimer, RadioOnRxTimeoutIrq );
    TimerStop( &TxTimeoutTimer );
    TimerStop( &RxTimeoutTimer );
}

static RadioState_t RadioGetStatus( void )
{
    switch( SUBGRF_GetOperatingMode( ) )
    {
        case MODE_TX:
            return RF_TX_RUNNING;
        case MODE_RX:
            return RF_RX_RUNNING;
        case MODE_CAD:
            return RF_CAD;
        default:
            return RF_IDLE;
    }
}

static void RadioSetModem( RadioModems_t modem )
{
    SubgRf.Modem = modem;
    RFW_SetRadioModem(modem);
    switch( modem )
    {
    default:
    case MODEM_FSK:
        SUBGRF_SetPacketType( PACKET_TYPE_GFSK );
        // When switching to GFSK mode the LoRa SyncWord register value is reset
        // Thus, we also reset the RadioPublicNetwork variable
        SubgRf.PublicNetwork.Current = false;
        break;
    case MODEM_LORA:
        SUBGRF_SetPacketType( PACKET_TYPE_LORA );
        // Public/Private network register is reset when switching modems
        if( SubgRf.PublicNetwork.Current != SubgRf.PublicNetwork.Previous )
        {
            SubgRf.PublicNetwork.Current = SubgRf.PublicNetwork.Previous;
            RadioSetPublicNetwork( SubgRf.PublicNetwork.Current );
        }
        break;
    case MODEM_BPSK:
        SUBGRF_SetPacketType( PACKET_TYPE_BPSK );
        // When switching to BPSK mode the LoRa SyncWord register value is reset
        // Thus, we also reset the RadioPublicNetwork variable
        SubgRf.PublicNetwork.Current = false;
        break;
    case MODEM_SIGFOX_TX:
        SUBGRF_SetPacketType( PACKET_TYPE_BPSK );
        // When switching to BPSK mode the LoRa SyncWord register value is reset
        // Thus, we also reset the RadioPublicNetwork variable
        SubgRf.PublicNetwork.Current = false;
        break;
    case MODEM_SIGFOX_RX:
        SUBGRF_SetPacketType( PACKET_TYPE_GFSK );
        // When switching to GFSK mode the LoRa SyncWord register value is reset
        // Thus, we also reset the RadioPublicNetwork variable
        SubgRf.PublicNetwork.Current = false;
        break;
    }
}

static void RadioSetChannel( uint32_t freq )
{
    SUBGRF_SetRfFrequency( freq );
}

static bool RadioIsChannelFree( uint32_t freq, uint32_t rxBandwidth, int16_t rssiThresh, uint32_t maxCarrierSenseTime )
{
    bool status = true;
    int16_t rssi = 0;
    uint32_t carrierSenseTime = 0;

    /* ST_WORKAROUND_BEGIN: Prevent multiple sleeps with TXCO delay */
    RadioStandby( );
    /* ST_WORKAROUND_END */

    RadioSetModem( MODEM_FSK );

    RadioSetChannel( freq );

    // Set Rx bandwidth. Other parameters are not used.
    RadioSetRxConfig( MODEM_FSK, rxBandwidth, 600, 0, rxBandwidth, 3, 0, false,
                      0, false, 0, 0, false, true );
    RadioRx( 0 );

    RADIO_DELAY_MS( RadioGetWakeupTime( ) );

    carrierSenseTime = TimerGetCurrentTime( );

    // Perform carrier sense for maxCarrierSenseTime
    while( TimerGetElapsedTime( carrierSenseTime ) < maxCarrierSenseTime )
    {
        rssi = RadioRssi( MODEM_FSK );

        if( rssi > rssiThresh )
        {
            status = false;
            break;
        }
    }
    /* ST_WORKAROUND_BEGIN: Prevent multiple sleeps with TXCO delay */
    RadioStandby( );
    /* ST_WORKAROUND_END */
    return status;
}

static uint32_t RadioRandom( void )
{
    uint32_t rnd = 0;

    /*
     * Radio setup for random number generation
     */
    // Set LoRa modem ON
    RadioSetModem( MODEM_LORA );

    // Disable LoRa modem interrupts
    SUBGRF_SetDioIrqParams( IRQ_RADIO_NONE, IRQ_RADIO_NONE, IRQ_RADIO_NONE, IRQ_RADIO_NONE );

    rnd = SUBGRF_GetRandom();

    return rnd;
}

static void RadioSetRxConfig( RadioModems_t modem, uint32_t bandwidth,
                              uint32_t datarate, uint8_t coderate,
                              uint32_t bandwidthAfc, uint16_t preambleLen,
                              uint16_t symbTimeout, bool fixLen,
                              uint8_t payloadLen,
                              bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                              bool iqInverted, bool rxContinuous )
{

    uint8_t modReg;
    SubgRf.RxContinuous = rxContinuous;
    RFW_DeInit(); /* ST_WORKAROUND: Switch Off FwPacketDecoding by default */
    if( rxContinuous == true )
    {
        symbTimeout = 0;
    }
    if( fixLen == true )
    {
        MaxPayloadLength = payloadLen;
    }
    else
    {
        MaxPayloadLength = 0xFF;
    }

    switch( modem )
    {
        case MODEM_SIGFOX_RX:
            SUBGRF_SetStopRxTimerOnPreambleDetect( true );
            SubgRf.ModulationParams.PacketType = PACKET_TYPE_GFSK;

            SubgRf.ModulationParams.Params.Gfsk.BitRate = datarate;
            SubgRf.ModulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_G_BT_05;
            SubgRf.ModulationParams.Params.Gfsk.Fdev = 800;
            SubgRf.ModulationParams.Params.Gfsk.Bandwidth = SUBGRF_GetFskBandwidthRegValue( bandwidth );

            SubgRf.PacketParams.PacketType = PACKET_TYPE_GFSK;
            SubgRf.PacketParams.Params.Gfsk.PreambleLength = ( preambleLen << 3 ); // convert byte into bit
            SubgRf.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_OFF;
            SubgRf.PacketParams.Params.Gfsk.SyncWordLength = 2 << 3; // convert byte into bit
            SubgRf.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
            SubgRf.PacketParams.Params.Gfsk.HeaderType = RADIO_PACKET_FIXED_LENGTH;
            SubgRf.PacketParams.Params.Gfsk.PayloadLength = MaxPayloadLength;
            SubgRf.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_OFF;

            SubgRf.PacketParams.Params.Gfsk.DcFree = RADIO_DC_FREE_OFF;

            RadioSetModem( MODEM_SIGFOX_RX );
            SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
            SUBGRF_SetSyncWord( ( uint8_t[] ){0xB2, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } );
            SUBGRF_SetWhiteningSeed( 0x01FF );

            /* NO gfo reset (better sensitivity). Reg 0x8b8, bit4 = 0 */
            modReg= RadioRead(0x8b8);
            modReg&=RADIO_BIT_MASK(4);
            RadioWrite(0x8b8, modReg);
            /* Lower the threshold of cfo_reset */
            RadioWrite(0x8b9, 0x4 );

            /* Bigger rssi_len (stability AGC). Reg 0x89b, bits[2 :4] = 0x1 */
            modReg= RadioRead(0x89b);
            modReg&=( RADIO_BIT_MASK(2) & RADIO_BIT_MASK(3) & RADIO_BIT_MASK(4) );
            RadioWrite(0x89b, (modReg| (0x1<<3) ) );

            /* Bigger afc_pbl_len (better frequency correction). Reg 0x6d1, bits[3 :4] = 0x3 */
            modReg= RadioRead(0x6d1);
            modReg&=( RADIO_BIT_MASK(3) & RADIO_BIT_MASK(4) );
            RadioWrite(0x6d1, (modReg| (0x3<<3) ));

            /* Use of new bit synchronizer (to avoid CRC errors during PER for payloads with a small amount of transitions). Reg 0x6ac, bits[4 :6] = 0x5 */
            modReg= RadioRead(0x6ac);
            modReg&=( RADIO_BIT_MASK(4) & RADIO_BIT_MASK(5) & RADIO_BIT_MASK(6) );
            RadioWrite(0x6ac, (modReg| (0x5<<4) ));
            /*timeout unused when SubgRf.RxContinuous*/
            SubgRf.RxTimeout = ( uint32_t )(( symbTimeout * 8 * 1000 ) /datarate);
            break;
        case MODEM_FSK:
            SUBGRF_SetStopRxTimerOnPreambleDetect( false );
            SubgRf.ModulationParams.PacketType = PACKET_TYPE_GFSK;

            SubgRf.ModulationParams.Params.Gfsk.BitRate = datarate;
            SubgRf.ModulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_G_BT_1;
            SubgRf.ModulationParams.Params.Gfsk.Bandwidth = SUBGRF_GetFskBandwidthRegValue( bandwidth );

            SubgRf.PacketParams.PacketType = PACKET_TYPE_GFSK;
            SubgRf.PacketParams.Params.Gfsk.PreambleLength = ( preambleLen << 3 ); // convert byte into bit
            SubgRf.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS;
            SubgRf.PacketParams.Params.Gfsk.SyncWordLength = 3 << 3; // convert byte into bit
            SubgRf.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
            SubgRf.PacketParams.Params.Gfsk.HeaderType = ( fixLen == true ) ? RADIO_PACKET_FIXED_LENGTH : RADIO_PACKET_VARIABLE_LENGTH;
            SubgRf.PacketParams.Params.Gfsk.PayloadLength = MaxPayloadLength;
            if( crcOn == true )
            {
                SubgRf.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
            }
            else
            {
                SubgRf.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_OFF;
            }
            SubgRf.PacketParams.Params.Gfsk.DcFree = RADIO_DC_FREEWHITENING;

            RadioStandby( );
            RadioSetModem( ( SubgRf.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
            SUBGRF_SetSyncWord( ( uint8_t[] ){ 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 } );
            SUBGRF_SetWhiteningSeed( 0x01FF );

            /*timeout unused when SubgRf.RxContinuous*/
            SubgRf.RxTimeout = ( uint32_t )(( symbTimeout * 8 * 1000 ) /datarate);
            break;

        case MODEM_LORA:
            SUBGRF_SetStopRxTimerOnPreambleDetect( false );
            SubgRf.ModulationParams.PacketType = PACKET_TYPE_LORA;
            SubgRf.ModulationParams.Params.LoRa.SpreadingFactor = ( RadioLoRaSpreadingFactors_t )datarate;
            SubgRf.ModulationParams.Params.LoRa.Bandwidth = Bandwidths[bandwidth];
            SubgRf.ModulationParams.Params.LoRa.CodingRate = ( RadioLoRaCodingRates_t )coderate;

            if( ( ( bandwidth == 0 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
                ( ( bandwidth == 1 ) && ( datarate == 12 ) ) )
            {
                SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x00;
            }

            SubgRf.PacketParams.PacketType = PACKET_TYPE_LORA;

            if( ( SubgRf.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF5 ) ||
                ( SubgRf.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF6 ) )
            {
                if( preambleLen < 12 )
                {
                    SubgRf.PacketParams.Params.LoRa.PreambleLength = 12;
                }
                else
                {
                    SubgRf.PacketParams.Params.LoRa.PreambleLength = preambleLen;
                }
            }
            else
            {
                SubgRf.PacketParams.Params.LoRa.PreambleLength = preambleLen;
            }

            SubgRf.PacketParams.Params.LoRa.HeaderType = ( RadioLoRaPacketLengthsMode_t )fixLen;

            SubgRf.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength;
            SubgRf.PacketParams.Params.LoRa.CrcMode = ( RadioLoRaCrcModes_t )crcOn;
            SubgRf.PacketParams.Params.LoRa.InvertIQ = ( RadioLoRaIQModes_t )iqInverted;

            RadioStandby( );
            RadioSetModem( ( SubgRf.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
            SUBGRF_SetLoRaSymbNumTimeout( symbTimeout );

            // WORKAROUND - Optimizing the Inverted IQ Operation, see DS_SX1261-2_V1.2 datasheet chapter 15.4
            if( SubgRf.PacketParams.Params.LoRa.InvertIQ == LORA_IQ_INVERTED )
            {
                // RegIqPolaritySetup = @address 0x0736
                SUBGRF_WriteRegister( 0x0736, SUBGRF_ReadRegister( 0x0736 ) & ~( 1 << 2 ) );
            }
            else
            {
                // RegIqPolaritySetup @address 0x0736
                SUBGRF_WriteRegister( 0x0736, SUBGRF_ReadRegister( 0x0736 ) | ( 1 << 2 ) );
            }
            // WORKAROUND END

            // Timeout Max, Timeout handled directly in SetRx function
            SubgRf.RxTimeout = 0xFFFF;

            break;
        default:
            break;
    }
}

static void RadioSetTxConfig( RadioModems_t modem, int8_t power, uint32_t fdev,
                              uint32_t bandwidth, uint32_t datarate,
                              uint8_t coderate, uint16_t preambleLen,
                              bool fixLen, bool crcOn, bool freqHopOn,
                              uint8_t hopPeriod, bool iqInverted, uint32_t timeout )
{
    RFW_DeInit(); /* ST_WORKAROUND: Switch Off FwPacketDecoding by default */
    switch( modem )
    {
        case MODEM_FSK:
            SubgRf.ModulationParams.PacketType = PACKET_TYPE_GFSK;
            SubgRf.ModulationParams.Params.Gfsk.BitRate = datarate;

            SubgRf.ModulationParams.Params.Gfsk.ModulationShaping = MOD_SHAPING_G_BT_1;
            SubgRf.ModulationParams.Params.Gfsk.Bandwidth = SUBGRF_GetFskBandwidthRegValue( bandwidth );
            SubgRf.ModulationParams.Params.Gfsk.Fdev = fdev;

            SubgRf.PacketParams.PacketType = PACKET_TYPE_GFSK;
            SubgRf.PacketParams.Params.Gfsk.PreambleLength = ( preambleLen << 3 ); // convert byte into bit
            SubgRf.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS;
            SubgRf.PacketParams.Params.Gfsk.SyncWordLength = 3 << 3 ; // convert byte into bit
            SubgRf.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF;
            SubgRf.PacketParams.Params.Gfsk.HeaderType = ( fixLen == true ) ? RADIO_PACKET_FIXED_LENGTH : RADIO_PACKET_VARIABLE_LENGTH;

            if( crcOn == true )
            {
                SubgRf.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_2_BYTES_CCIT;
            }
            else
            {
                SubgRf.PacketParams.Params.Gfsk.CrcLength = RADIO_CRC_OFF;
            }
            SubgRf.PacketParams.Params.Gfsk.DcFree = RADIO_DC_FREEWHITENING;

            RadioStandby( );
            RadioSetModem( ( SubgRf.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
            SUBGRF_SetSyncWord( ( uint8_t[] ){ 0xC1, 0x94, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00 } );
            SUBGRF_SetWhiteningSeed( 0x01FF );
            break;

        case MODEM_LORA:
            SubgRf.ModulationParams.PacketType = PACKET_TYPE_LORA;
            SubgRf.ModulationParams.Params.LoRa.SpreadingFactor = ( RadioLoRaSpreadingFactors_t ) datarate;
            SubgRf.ModulationParams.Params.LoRa.Bandwidth =  Bandwidths[bandwidth];
            SubgRf.ModulationParams.Params.LoRa.CodingRate= ( RadioLoRaCodingRates_t )coderate;

            if( ( ( bandwidth == 0 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
                ( ( bandwidth == 1 ) && ( datarate == 12 ) ) )
            {
                SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x01;
            }
            else
            {
                SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 0x00;
            }

            SubgRf.PacketParams.PacketType = PACKET_TYPE_LORA;

            if( ( SubgRf.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF5 ) ||
                ( SubgRf.ModulationParams.Params.LoRa.SpreadingFactor == LORA_SF6 ) )
            {
                if( preambleLen < 12 )
                {
                    SubgRf.PacketParams.Params.LoRa.PreambleLength = 12;
                }
                else
                {
                    SubgRf.PacketParams.Params.LoRa.PreambleLength = preambleLen;
                }
            }
            else
            {
                SubgRf.PacketParams.Params.LoRa.PreambleLength = preambleLen;
            }
            SubgRf.PacketParams.Params.LoRa.HeaderType = ( RadioLoRaPacketLengthsMode_t )fixLen;
            SubgRf.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength;
            SubgRf.PacketParams.Params.LoRa.CrcMode = ( RadioLoRaCrcModes_t )crcOn;
            SubgRf.PacketParams.Params.LoRa.InvertIQ = ( RadioLoRaIQModes_t )iqInverted;

            RadioStandby( );
            RadioSetModem( ( SubgRf.ModulationParams.PacketType == PACKET_TYPE_GFSK ) ? MODEM_FSK : MODEM_LORA );
            SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
            break;

        case MODEM_SIGFOX_TX:
            RadioSetModem(MODEM_SIGFOX_TX);
            SubgRf.ModulationParams.PacketType = PACKET_TYPE_BPSK;
            SubgRf.ModulationParams.Params.Bpsk.BitRate           = datarate;
            SubgRf.ModulationParams.Params.Bpsk.ModulationShaping = MOD_SHAPING_DBPSK;
            SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
            break;

        default:
            break;
    }



    SubgRf.AntSwitchPaSelect = SUBGRF_SetRfTxPower( power );
    RFW_SetAntSwitch( SubgRf.AntSwitchPaSelect ); /* ST_WORKAROUND: ?????? */
    SubgRf.TxTimeout = timeout;
}

static bool RadioCheckRfFrequency( uint32_t frequency )
{
    return true;
}

static uint32_t RadioGetLoRaBandwidthInHz( RadioLoRaBandwidths_t bw )
{
    uint32_t bandwidthInHz = 0;

    switch( bw )
    {
    case LORA_BW_007:
        bandwidthInHz = 7812UL;
        break;
    case LORA_BW_010:
        bandwidthInHz = 10417UL;
        break;
    case LORA_BW_015:
        bandwidthInHz = 15625UL;
        break;
    case LORA_BW_020:
        bandwidthInHz = 20833UL;
        break;
    case LORA_BW_031:
        bandwidthInHz = 31250UL;
        break;
    case LORA_BW_041:
        bandwidthInHz = 41667UL;
        break;
    case LORA_BW_062:
        bandwidthInHz = 62500UL;
        break;
    case LORA_BW_125:
        bandwidthInHz = 125000UL;
        break;
    case LORA_BW_250:
        bandwidthInHz = 250000UL;
        break;
    case LORA_BW_500:
        bandwidthInHz = 500000UL;
        break;
    }

    return bandwidthInHz;
}

static uint32_t RadioGetGfskTimeOnAirNumerator( uint32_t datarate, uint8_t coderate,
                                                uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                                                bool crcOn )
{
    /* ST_WORKAROUND_BEGIN: Simplified calculation without const values */
    return ( preambleLen << 3 ) +
           ( ( fixLen == false ) ? 8 : 0 ) + 24 +
           ( ( payloadLen + ( ( crcOn == true ) ? 2 : 0 ) ) << 3 );
    /* ST_WORKAROUND_END */
}

static uint32_t RadioGetLoRaTimeOnAirNumerator( uint32_t bandwidth,
                                                uint32_t datarate, uint8_t coderate,
                                                uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                                                bool crcOn )
{
    int32_t crDenom           = coderate + 4;
    bool    lowDatareOptimize = false;

    // Ensure that the preamble length is at least 12 symbols when using SF5 or SF6
    if( ( datarate == 5 ) || ( datarate == 6 ) )
    {
        if( preambleLen < 12 )
        {
            preambleLen = 12;
        }
    }

    if( ( ( bandwidth == 0 ) && ( ( datarate == 11 ) || ( datarate == 12 ) ) ) ||
        ( ( bandwidth == 1 ) && ( datarate == 12 ) ) )
    {
        lowDatareOptimize = true;
    }

    int32_t ceilDenominator;
    int32_t ceilNumerator = ( payloadLen << 3 ) +
                            ( crcOn ? 16 : 0 ) -
                            ( 4 * datarate ) +
                            ( fixLen ? 0 : 20 );

    if( datarate <= 6 )
    {
        ceilDenominator = 4 * datarate;
    }
    else
    {
        ceilNumerator += 8;

        if( lowDatareOptimize == true )
        {
            ceilDenominator = 4 * ( datarate - 2 );
        }
        else
        {
            ceilDenominator = 4 * datarate;
        }
    }

    if( ceilNumerator < 0 )
    {
        ceilNumerator = 0;
    }

    // Perform integral ceil()
    int32_t intermediate =
        ( ( ceilNumerator + ceilDenominator - 1 ) / ceilDenominator ) * crDenom + preambleLen + 12;

    if( datarate <= 6 )
    {
        intermediate += 2;
    }

    return ( uint32_t )( ( 4 * intermediate + 1 ) * ( 1 << ( datarate - 2 ) ) );
}

static uint32_t RadioTimeOnAir( RadioModems_t modem, uint32_t bandwidth,
                                uint32_t datarate, uint8_t coderate,
                                uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                                bool crcOn )
{
    uint32_t numerator = 0;
    uint32_t denominator = 1;

    switch( modem )
    {
    case MODEM_FSK:
        {
            numerator   = 1000U * RadioGetGfskTimeOnAirNumerator( datarate, coderate,
                                                                  preambleLen, fixLen,
                                                                  payloadLen, crcOn );
            denominator = datarate;
        }
        break;
    case MODEM_LORA:
        {
            numerator   = 1000U * RadioGetLoRaTimeOnAirNumerator( bandwidth, datarate,
                                                                  coderate, preambleLen,
                                                                  fixLen, payloadLen, crcOn );
            denominator = RadioGetLoRaBandwidthInHz( Bandwidths[bandwidth] );
        }
        break;
    default:
        break;
    }
    // Perform integral ceil()
    return DIVC(numerator, denominator); /* ST_WORKAROUND : simplified calculation with macro usage */
}

static void RadioSend( uint8_t *buffer, uint8_t size )
{
    /* ST_WORKAROUND_BEGIN : Set the debug pin and update the radio switch */
    SUBGRF_SetDioIrqParams( IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT | IRQ_TX_DBG,
                            IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT | IRQ_TX_DBG,
                            IRQ_RADIO_NONE,
                            IRQ_RADIO_NONE );
    /* ST_WORKAROUND_END */

    /* ST_WORKAROUND_BEGIN : Set the debug pin and update the radio switch */
    /* Set DBG pin */
    DBG_GPIO_RADIO_TX(SET);

    /* Set RF switch */
    SUBGRF_SetSwitch(SubgRf.AntSwitchPaSelect, RFSWITCH_TX);
    /* WORKAROUND - Modulation Quality with 500 kHz LoRaTM Bandwidth*/
    /* RegTxModulation = @address 0x0889 */
    if((SubgRf.Modem==MODEM_LORA) && (SubgRf.ModulationParams.Params.LoRa.Bandwidth == LORA_BW_500 ))
    {
        SUBGRF_WriteRegister( 0x0889, SUBGRF_ReadRegister( 0x0889 ) & ~( 1 << 2 ) );
    }
    else
    {
        SUBGRF_WriteRegister( 0x0889, SUBGRF_ReadRegister( 0x0889 ) | ( 1 << 2 ) );
    }
    /* WORKAROUND END */
    switch(SubgRf.Modem)
    {
        case MODEM_LORA:
        {
            SubgRf.PacketParams.Params.LoRa.PayloadLength = size;
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
            SUBGRF_SendPayload( buffer, size, 0 );
            break;
        }
        case MODEM_FSK:
        {
            if ( 1UL == RFW_Is_Init( ) )
            {
              uint8_t outsize;
              if ( 0UL == RFW_TransmitInit( buffer,size, &outsize ) )
              {
                  SubgRf.PacketParams.Params.Gfsk.PayloadLength = outsize;
                  SUBGRF_SetPacketParams( &SubgRf.PacketParams );
                  SUBGRF_SendPayload( buffer, outsize, 0 );
              }
              else
              {
                MW_LOG( TS_ON, VLEVEL_M, "RadioSend Oversize\r\n");
                return;
              }
            }
            else
            {
              SubgRf.PacketParams.Params.Gfsk.PayloadLength = size;
              SUBGRF_SetPacketParams( &SubgRf.PacketParams );
              SUBGRF_SendPayload( buffer, size, 0 );
            }
            break;
        }
        case MODEM_BPSK:
        {
            SubgRf.PacketParams.PacketType = PACKET_TYPE_BPSK;
            SubgRf.PacketParams.Params.Bpsk.PayloadLength = size;
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
            SUBGRF_SendPayload( buffer, size, 0 );
            break;
        }
        case MODEM_SIGFOX_TX:
        {
            /* from bpsk to dbpsk */
            /* first 1 bit duplicated */
            /* RadioBuffer is 1 bytes more */
            payload_integration( RadioBuffer, buffer, size );

            SubgRf.PacketParams.PacketType = PACKET_TYPE_BPSK;
            SubgRf.PacketParams.Params.Bpsk.PayloadLength = size + 1;
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );

            RadioWrite( 0x00F1, 0 ); // clean start-up LSB
            RadioWrite( 0x00F0, 0 ); // clean start-up MSB
            if( SubgRf.ModulationParams.Params.Bpsk.BitRate == 100 )
            {
                RadioWrite( 0x00F3, 0x70 ); // clean end of frame LSB
                RadioWrite( 0x00F2, 0x1D ); // clean end of frame MSB
            }
            else // 600 bps
            {
                RadioWrite( 0x00F3, 0xE1 ); // clean end of frame LSB
                RadioWrite( 0x00F2, 0x04 ); // clean end of frame MSB
            }

            uint16_t bitNum = ( size * 8 ) + 2;
            RadioWrite( 0x00F4, ( bitNum >> 8 ) & 0x00FF );    // limit frame
            RadioWrite( 0x00F5, bitNum & 0x00FF );             // limit frame
            SUBGRF_SendPayload( RadioBuffer, size+1 , 0xFFFFFF );
            break;
        }
        default:
            break;
    }

    TimerSetValue( &TxTimeoutTimer, SubgRf.TxTimeout );
    TimerStart( &TxTimeoutTimer );
}

static void RadioSleep( void )
{
    SleepParams_t params = { 0 };

    params.Fields.WarmStart = 1;
    SUBGRF_SetSleep( params );

    RADIO_DELAY_MS( 2 );
}

static void RadioStandby( void )
{
    SUBGRF_SetStandby( STDBY_RC );
}

static void RadioRx( uint32_t timeout )
{
    if ( 1UL == RFW_Is_Init( ) )
    {
      RFW_ReceiveInit( );
    }
    else
    {
      SUBGRF_SetDioIrqParams( IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT | IRQ_CRC_ERROR | IRQ_HEADER_ERROR | IRQ_RX_DBG,
                              IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT | IRQ_CRC_ERROR | IRQ_HEADER_ERROR | IRQ_RX_DBG,
                              IRQ_RADIO_NONE,
                              IRQ_RADIO_NONE );
    }

    if( timeout != 0 )
    {
        TimerSetValue( &RxTimeoutTimer, timeout );
        TimerStart( &RxTimeoutTimer );
    }

    /* ST_WORKAROUND_BEGIN : Set the debug pin and update the radio switch */
    /* Set DBG pin */
    DBG_GPIO_RADIO_RX(SET);

    /* RF switch configuration */
    SUBGRF_SetSwitch(SubgRf.AntSwitchPaSelect, RFSWITCH_RX);
    /* ST_WORKAROUND_END */

    if( SubgRf.RxContinuous == true )
    {
        SUBGRF_SetRx( 0xFFFFFF ); // Rx Continuous
    }
    else
    {
        SUBGRF_SetRx( SubgRf.RxTimeout << 6 );
    }
}

static void RadioRxBoosted( uint32_t timeout )
{
    if (1UL==RFW_Is_Init())
    {
      RFW_ReceiveInit();
    }
    else
    {
      SUBGRF_SetDioIrqParams( IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT | IRQ_CRC_ERROR | IRQ_HEADER_ERROR | IRQ_RX_DBG,
                              IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT | IRQ_CRC_ERROR | IRQ_HEADER_ERROR | IRQ_RX_DBG,
                              IRQ_RADIO_NONE,
                              IRQ_RADIO_NONE );
    }
    if( timeout != 0 )
    {
        TimerSetValue( &RxTimeoutTimer, timeout );
        TimerStart( &RxTimeoutTimer );
    }

    /* RF switch configuration */
    SUBGRF_SetSwitch(SubgRf.AntSwitchPaSelect, RFSWITCH_RX);

    if( SubgRf.RxContinuous == true )
    {
        SUBGRF_SetRxBoosted( 0xFFFFFF ); // Rx Continuous
    }
    else
    {
        SUBGRF_SetRxBoosted( SubgRf.RxTimeout << 6 );
    }
}

static void RadioSetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime )
{
    /* RF switch configuration */
    SUBGRF_SetSwitch(SubgRf.AntSwitchPaSelect, RFSWITCH_RX);

    SUBGRF_SetRxDutyCycle( rxTime, sleepTime );
}

static void RadioStartCad( void )
{
    /* RF switch configuration */
    SUBGRF_SetSwitch(SubgRf.AntSwitchPaSelect, RFSWITCH_RX);

    SUBGRF_SetDioIrqParams( IRQ_CAD_CLEAR | IRQ_CAD_DETECTED,
                            IRQ_CAD_CLEAR | IRQ_CAD_DETECTED,
                            IRQ_RADIO_NONE,
                            IRQ_RADIO_NONE );
    SUBGRF_SetCad( );
}

static void RadioSetTxContinuousWave( uint32_t freq, int8_t power, uint16_t time )
{
    uint32_t timeout = ( uint32_t )time * 1000;
    uint8_t antswitchpow;

    SUBGRF_SetRfFrequency( freq );

    antswitchpow = SUBGRF_SetRfTxPower( power );

    /* Set RF switch */
    SUBGRF_SetSwitch(antswitchpow, RFSWITCH_TX);

    SUBGRF_SetTxContinuousWave( );

    TimerSetValue( &TxTimeoutTimer, timeout );
    TimerStart( &TxTimeoutTimer );
}

static int16_t RadioRssi( RadioModems_t modem )
{
    return SUBGRF_GetRssiInst( );
}

static void RadioWrite( uint16_t addr, uint8_t data )
{
    SUBGRF_WriteRegister(addr, data );
}

static uint8_t RadioRead( uint16_t addr )
{
    return SUBGRF_ReadRegister(addr);
}

static void RadioWriteRegisters( uint16_t addr, uint8_t *buffer, uint8_t size )
{
    SUBGRF_WriteRegisters( addr, buffer, size );
}

static void RadioReadRegisters( uint16_t addr, uint8_t *buffer, uint8_t size )
{
    SUBGRF_ReadRegisters( addr, buffer, size );
}

static void RadioSetMaxPayloadLength( RadioModems_t modem, uint8_t max )
{
    if( modem == MODEM_LORA )
    {
        SubgRf.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength = max;
        SUBGRF_SetPacketParams( &SubgRf.PacketParams );
    }
    else
    {
        if( SubgRf.PacketParams.Params.Gfsk.HeaderType == RADIO_PACKET_VARIABLE_LENGTH )
        {
            SubgRf.PacketParams.Params.Gfsk.PayloadLength = MaxPayloadLength = max;
            SUBGRF_SetPacketParams( &SubgRf.PacketParams );
        }
    }
}

static void RadioSetPublicNetwork( bool enable )
{
    SubgRf.PublicNetwork.Current = SubgRf.PublicNetwork.Previous = enable;

    RadioSetModem( MODEM_LORA );
    if( enable == true )
    {
        // Change LoRa modem SyncWord
        SUBGRF_WriteRegister( REG_LR_SYNCWORD, ( LORA_MAC_PUBLIC_SYNCWORD >> 8 ) & 0xFF );
        SUBGRF_WriteRegister( REG_LR_SYNCWORD + 1, LORA_MAC_PUBLIC_SYNCWORD & 0xFF );
    }
    else
    {
        // Change LoRa modem SyncWord
        SUBGRF_WriteRegister( REG_LR_SYNCWORD, ( LORA_MAC_PRIVATE_SYNCWORD >> 8 ) & 0xFF );
        SUBGRF_WriteRegister( REG_LR_SYNCWORD + 1, LORA_MAC_PRIVATE_SYNCWORD & 0xFF );
    }
}

static uint32_t RadioGetWakeupTime( void )
{
    return SUBGRF_GetRadioWakeUpTime() + RADIO_WAKEUP_TIME;
}

static void RadioOnTxTimeoutIrq( void* context )
{
    RADIO_TX_TIMEOUT_PROCESS();
}

static void RadioOnRxTimeoutIrq( void* context )
{
    RADIO_RX_TIMEOUT_PROCESS();
}

static void RadioOnTxTimeoutProcess( void )
{
    /* ST_WORKAROUND_BEGIN: Reset DBG pin */
    DBG_GPIO_RADIO_TX(RST);
    /* ST_WORKAROUND_END */

    if( ( RadioEvents != NULL ) && ( RadioEvents->TxTimeout != NULL ) )
    {
        RadioEvents->TxTimeout( );
    }
}

static void RadioOnRxTimeoutProcess( void )
{
    /* ST_WORKAROUND_BEGIN: Reset DBG pin */
    DBG_GPIO_RADIO_RX(RST);
    /* ST_WORKAROUND_END */

    if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
    {
        RadioEvents->RxTimeout( );
    }
}

static void RadioOnDioIrq( RadioIrqMasks_t radioIrq )
{
    SubgRf.RadioIrq = radioIrq;

    RADIO_IRQ_PROCESS();
}

static void RadioIrqProcess( void )
{
    uint8_t size = 0;
    int32_t cfo = 0;

    switch ( SubgRf.RadioIrq )
    {
    case IRQ_TX_DONE:
        /* ST_WORKAROUND_BEGIN: Reset DBG pin */
        DBG_GPIO_RADIO_TX(RST);
        /* ST_WORKAROUND_END */

        TimerStop( &TxTimeoutTimer );
        //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
        SUBGRF_SetStandby( STDBY_RC );

        if ( RFW_Is_LongPacketModeEnabled() == 1 )
        {
            RFW_DeInit_TxLongPacket( );
        }

        if( ( RadioEvents != NULL ) && ( RadioEvents->TxDone != NULL ) )
        {
            RadioEvents->TxDone( );
        }
        break;

    case IRQ_RX_DONE:
        /* ST_WORKAROUND_BEGIN: Reset DBG pin */
        DBG_GPIO_RADIO_RX(RST);
        /* ST_WORKAROUND_END */

        TimerStop( &RxTimeoutTimer );
        if( SubgRf.RxContinuous == false )
        {
            //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
            SUBGRF_SetStandby( STDBY_RC );

            // WORKAROUND - Implicit Header Mode Timeout Behavior, see DS_SX1261-2_V1.2 datasheet chapter 15.3
            // RegRtcControl = @address 0x0902
            SUBGRF_WriteRegister( 0x0902, 0x00 );
            // RegEventMask = @address 0x0944
            SUBGRF_WriteRegister( 0x0944, SUBGRF_ReadRegister( 0x0944 ) | ( 1 << 1 ) );
            // WORKAROUND END
        }
        SUBGRF_GetPayload( RadioBuffer, &size , 255 );
        SUBGRF_GetPacketStatus( &(SubgRf.PacketStatus) );
        if( ( RadioEvents != NULL ) && ( RadioEvents->RxDone != NULL ) )
        {
            switch ( SubgRf.PacketStatus.packetType )
            {
            case PACKET_TYPE_LORA:
                RadioEvents->RxDone( RadioBuffer, size, SubgRf.PacketStatus.Params.LoRa.RssiPkt, SubgRf.PacketStatus.Params.LoRa.SnrPkt );
                break;
            default:
                SUBGRF_GetCFO( SubgRf.ModulationParams.Params.Gfsk.BitRate, &cfo );
                RadioEvents->RxDone( RadioBuffer, size, SubgRf.PacketStatus.Params.Gfsk.RssiAvg, (int8_t) DIVR(cfo, 1000) );
                break;
            }
        }
        break;

    case IRQ_CAD_CLEAR:
        //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
        SUBGRF_SetStandby( STDBY_RC );
        if( ( RadioEvents != NULL ) && ( RadioEvents->CadDone != NULL ) )
        {
            RadioEvents->CadDone( false );
        }
        break;
    case IRQ_CAD_DETECTED:
        //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
        SUBGRF_SetStandby( STDBY_RC );
        if( ( RadioEvents != NULL ) && ( RadioEvents->CadDone != NULL ) )
        {
            RadioEvents->CadDone( true );
        }
        break;


    case IRQ_RX_TX_TIMEOUT:
        MW_LOG( TS_ON, VLEVEL_M,  "IRQ_RX_TX_TIMEOUT\r\n" );
        if( SUBGRF_GetOperatingMode( ) == MODE_TX )
        {
            /* ST_WORKAROUND_BEGIN: Reset DBG pin */
            DBG_GPIO_RADIO_TX(RST);
            /* ST_WORKAROUND_END */

            TimerStop( &TxTimeoutTimer );
            //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
            SUBGRF_SetStandby( STDBY_RC );
            if( ( RadioEvents != NULL ) && ( RadioEvents->TxTimeout != NULL ) )
            {
                RadioEvents->TxTimeout( );
            }
        }
        else if( SUBGRF_GetOperatingMode( ) == MODE_RX )
        {
            /* ST_WORKAROUND_BEGIN: Reset DBG pin */
            DBG_GPIO_RADIO_RX(RST);
            /* ST_WORKAROUND_END */

            TimerStop( &RxTimeoutTimer );
            //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
            SUBGRF_SetStandby( STDBY_RC );
            if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
            {
                RadioEvents->RxTimeout( );
            }
        }
        break;

    case IRQ_PREAMBLE_DETECTED:
        MW_LOG( TS_ON, VLEVEL_M,  "PRE OK\r\n" );
        break;

    case IRQ_SYNCWORD_VALID:
        MW_LOG( TS_ON, VLEVEL_M,  "SYNC OK\r\n" );
        if ( 1UL == RFW_Is_Init( ) )
        {
            RFW_ReceivePayload( );
        }
        break;

    case IRQ_HEADER_VALID:
        MW_LOG( TS_ON, VLEVEL_M,  "HDR OK\r\n" );
        break;

    case IRQ_HEADER_ERROR:
        TimerStop( &RxTimeoutTimer );
        if( SubgRf.RxContinuous == false )
        {
            //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
            SUBGRF_SetStandby( STDBY_RC );
        }
        if( ( RadioEvents != NULL ) && ( RadioEvents->RxTimeout != NULL ) )
        {
            RadioEvents->RxTimeout( );
            MW_LOG( TS_ON, VLEVEL_M,  "HDR KO\r\n" );
        }
        break;

    case IRQ_CRC_ERROR:
        MW_LOG( TS_ON, VLEVEL_M,  "IRQ_CRC_ERROR\r\n" );

        if( SubgRf.RxContinuous == false )
        {
            //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
            SUBGRF_SetStandby( STDBY_RC );
        }
        if( ( RadioEvents != NULL ) && ( RadioEvents->RxError ) )
        {
            RadioEvents->RxError( );
        }
        break;
    default:
        break;
  }
}

static void RadioTxPrbs( void )
{
    SUBGRF_SetSwitch( SubgRf.AntSwitchPaSelect, RFSWITCH_TX );
    Radio.Write( SUBGHZ_PKTCTL1A, 0x2d );  // sel mode prbs9 instead of preamble
    SUBGRF_SetTxInfinitePreamble( );
    SUBGRF_SetTx( 0x0fffff );
}

static void RadioTxCw( int8_t power )
{
    uint8_t paselect = SUBGRF_SetRfTxPower( power );
    SUBGRF_SetSwitch( paselect, RFSWITCH_TX );
    SUBGRF_SetTxContinuousWave( );
}

static void payload_integration( uint8_t *outBuffer, uint8_t *inBuffer, uint8_t size )
{
    uint8_t prevInt = 0;
    uint8_t currBit;
    uint8_t index_bit;
    uint8_t index_byte;
    uint8_t index_bit_out;
    uint8_t index_byte_out;
    int i = 0;

    for (i = 0; i < size; i++)
    {
        /* reverse all inputs */
        inBuffer[i] = ~inBuffer[i];
        /* init outBuffer */
        outBuffer[i] = 0;
    }

    for (i = 0; i < (size * 8); i++)
    {
        /* index to take bit in inBuffer */
        index_bit = 7 - ( i % 8 );
        index_byte = i / 8;
        /* index to place bit in outBuffer is shifted 1 bit right */
        index_bit_out = 7 - ( ( i + 1 ) % 8 );
        index_byte_out = ( i + 1 ) / 8;
        /* extract current bit from input */
        currBit = ( inBuffer[index_byte] >> index_bit ) & 0x01;
        /* integration */
        prevInt ^= currBit;
        /* write result integration in output */
        outBuffer[index_byte_out] |= ( prevInt << index_bit_out );
    }

    outBuffer[size] = ( prevInt << 7 ) | ( prevInt << 6 ) | ( ( ( !prevInt ) & 0x01 ) << 5 ) ;
}

static int32_t RadioSetRxGenericConfig( GenericModems_t modem, RxConfigGeneric_t* config, uint32_t rxContinuous, uint32_t symbTimeout )
{
    int32_t status = 0;
    uint8_t syncword[8] = {0};
    uint8_t MaxPayloadLength;

    RFW_DeInit( ); /* switch Off FwPacketDecoding by default */

    if( rxContinuous != 0 )
    {
        symbTimeout = 0;
    }
    SubgRf.RxContinuous = ( rxContinuous == 0 ) ? false : true;

    switch( modem )
    {
    case GENERIC_FSK:
        if( ( config->fsk.BitRate == 0 ) || ( config->fsk.PreambleLen == 0 ) )
        {
            return -1;
        }
        if( config->fsk.SyncWordLength > 8 )
        {
            return -1;
        }
        else
        {
            for(int i = 0; i < config->fsk.SyncWordLength; i++)
            {
                syncword[i] = config->fsk.SyncWord[i];
            }
        }

        SUBGRF_SetStopRxTimerOnPreambleDetect( ( config->fsk.StopTimerOnPreambleDetect == 0 ) ? false : true );

        SubgRf.ModulationParams.PacketType = PACKET_TYPE_GFSK;
        SubgRf.ModulationParams.Params.Gfsk.BitRate = config->fsk.BitRate;
        SubgRf.ModulationParams.Params.Gfsk.ModulationShaping = (RadioModShapings_t) config->fsk.ModulationShaping;
        SubgRf.ModulationParams.Params.Gfsk.Bandwidth = SUBGRF_GetFskBandwidthRegValue( config->fsk.Bandwidth );

        SubgRf.PacketParams.PacketType = PACKET_TYPE_GFSK;
        SubgRf.PacketParams.Params.Gfsk.PreambleLength = ( config->fsk.PreambleLen ) << 3 ; // convert byte into bit
        SubgRf.PacketParams.Params.Gfsk.PreambleMinDetect = ( RadioPreambleDetection_t ) config->fsk.PreambleMinDetect;
        SubgRf.PacketParams.Params.Gfsk.SyncWordLength = ( config->fsk.SyncWordLength ) << 3; // convert byte into bit
        SubgRf.PacketParams.Params.Gfsk.AddrComp = ( RadioAddressComp_t ) config->fsk.AddrComp;

        if( config->fsk.LengthMode == RADIO_FSK_PACKET_FIXED_LENGTH )
        {
            SubgRf.PacketParams.Params.Gfsk.PayloadLength = config->fsk.MaxPayloadLength;
        }
        else if( config->fsk.LengthMode == RADIO_FSK_PACKET_2BYTES_LENGTH )
        {
          /* Set max in the radio, in long packet mode will be tuned based dynamically on received chunk */
          SubgRf.PacketParams.Params.Gfsk.PayloadLength = 0xFF;
        }
        else
        {
          /* Set max in the radio */
          SubgRf.PacketParams.Params.Gfsk.PayloadLength = 0xFF;
        }

        if( ( config->fsk.Whitening == RADIO_FSK_DC_IBM_WHITENING ) || ( config->fsk.LengthMode == RADIO_FSK_PACKET_2BYTES_LENGTH ) )
        {
          /* Supports only RADIO_FSK_CRC_2_BYTES_IBM or RADIO_FSK_CRC_2_BYTES_CCIT*/
          if( ( config->fsk.CrcLength != RADIO_FSK_CRC_2_BYTES_IBM ) && ( config->fsk.CrcLength != RADIO_FSK_CRC_2_BYTES_CCIT ) && ( config->fsk.CrcLength != RADIO_FSK_CRC_OFF ) )
          {
            return -1;
          }
          ConfigGeneric_t ConfigGeneric;
          ConfigGeneric.rtx = CONFIG_RX;
          ConfigGeneric.RxConfig = config;
          if( 0UL != RFW_Init( &ConfigGeneric, RadioEvents, &RxTimeoutTimer ) )
          {
            return -1;
          }
          /* Whitening off, will be processed by FW, switch off built-in radio whitening*/
          SubgRf.PacketParams.Params.Gfsk.DcFree = ( RadioDcFree_t ) RADIO_FSK_DC_FREE_OFF;
          /* Crc off, Crc processed by FW, switch off built-in radio Crc*/
          SubgRf.PacketParams.Params.Gfsk.CrcLength = ( RadioCrcTypes_t ) RADIO_CRC_OFF;
          /* Length contained in Tx, but will be processed by FW after de-whitening*/
          SubgRf.PacketParams.Params.Gfsk.HeaderType = ( RadioPacketLengthModes_t ) RADIO_PACKET_FIXED_LENGTH;
        }
        else
        {
          SubgRf.PacketParams.Params.Gfsk.CrcLength = ( RadioCrcTypes_t ) config->fsk.CrcLength;
          SubgRf.PacketParams.Params.Gfsk.DcFree = ( RadioDcFree_t ) config->fsk.Whitening;
          SubgRf.PacketParams.Params.Gfsk.HeaderType = ( RadioPacketLengthModes_t ) config->fsk.LengthMode;
        }

        RadioStandby( );
        RadioSetModem( MODEM_FSK );
        SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
        SUBGRF_SetPacketParams( &SubgRf.PacketParams );
        SUBGRF_SetSyncWord( syncword );
        SUBGRF_SetWhiteningSeed( config->fsk.whiteSeed );
        SUBGRF_SetCrcPolynomial( config->fsk.CrcPolynomial );
        /* timeout unused when SubgRf.RxContinuous */
        SubgRf.RxTimeout = ( uint32_t )( ( symbTimeout * 1000 * 8 ) / config->fsk.BitRate );
        break;
    case GENERIC_LORA:
        if( config->lora.PreambleLen == 0 )
        {
            return -1;
        }

        if( config->lora.LengthMode == RADIO_LORA_PACKET_FIXED_LENGTH )
        {
            MaxPayloadLength = config->fsk.MaxPayloadLength;
        }
        else
        {
            MaxPayloadLength = 0xFF;
        }
        SUBGRF_SetStopRxTimerOnPreambleDetect( ( config->lora.StopTimerOnPreambleDetect == 0 ) ? false : true );
        SUBGRF_SetLoRaSymbNumTimeout( symbTimeout );

        SubgRf.ModulationParams.PacketType = PACKET_TYPE_LORA;
        SubgRf.ModulationParams.Params.LoRa.SpreadingFactor = ( RadioLoRaSpreadingFactors_t ) config->lora.SpreadingFactor;
        SubgRf.ModulationParams.Params.LoRa.Bandwidth = ( RadioLoRaBandwidths_t ) config->lora.Bandwidth;
        SubgRf.ModulationParams.Params.LoRa.CodingRate = ( RadioLoRaCodingRates_t ) config->lora.Coderate;
        switch( config->lora.LowDatarateOptimize )
        {
        case RADIO_LORA_LOWDR_OPT_OFF:
            SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 0;
            break;
        case RADIO_LORA_LOWDR_OPT_ON:
            SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 1;
            break;
        case RADIO_LORA_LOWDR_OPT_AUTO:
            if( ( config->lora.SpreadingFactor == RADIO_LORA_SF11 ) || ( config->lora.SpreadingFactor == RADIO_LORA_SF12 ) )
            {
              SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 1;
            }
            else
            {
              SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 0;
            }
            break;
        default:
            break;
        }

        SubgRf.PacketParams.PacketType = PACKET_TYPE_LORA;
        SubgRf.PacketParams.Params.LoRa.PreambleLength = config->lora.PreambleLen;
        SubgRf.PacketParams.Params.LoRa.HeaderType = ( RadioLoRaPacketLengthsMode_t ) config->lora.LengthMode;
        SubgRf.PacketParams.Params.LoRa.PayloadLength = MaxPayloadLength;
        SubgRf.PacketParams.Params.LoRa.CrcMode = ( RadioLoRaCrcModes_t ) config->lora.CrcMode;
        SubgRf.PacketParams.Params.LoRa.InvertIQ = ( RadioLoRaIQModes_t ) config->lora.IqInverted;

        RadioStandby( );
        RadioSetModem( MODEM_LORA );
        SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
        SUBGRF_SetPacketParams( &SubgRf.PacketParams );

        // WORKAROUND - Optimizing the Inverted IQ Operation, see DS_SX1261-2_V1.2 datasheet chapter 15.4
        if( SubgRf.PacketParams.Params.LoRa.InvertIQ == LORA_IQ_INVERTED )
        {
            // RegIqPolaritySetup = @address 0x0736
            SUBGRF_WriteRegister( 0x0736, SUBGRF_ReadRegister( 0x0736 ) & ~( 1 << 2 ) );
        }
        else
        {
            // RegIqPolaritySetup @address 0x0736
            SUBGRF_WriteRegister( 0x0736, SUBGRF_ReadRegister( 0x0736 ) | ( 1 << 2 ) );
        }
        // WORKAROUND END

        // Timeout Max, Timeout handled directly in SetRx function
        SubgRf.RxTimeout = 0xFFFF;
        break;
    default:
        break;
    }
    return status;
}

static int32_t RadioSetTxGenericConfig( GenericModems_t modem, TxConfigGeneric_t* config, int8_t power, uint32_t timeout )
{
    uint8_t syncword[8] = {0};
    RFW_DeInit( ); /* switch Off FwPacketDecoding by default */
    switch( modem )
    {
    case GENERIC_FSK:
        if( ( config->fsk.BitRate == 0 ) || ( config->fsk.PreambleLen == 0 ) )
        {
            return -1;
        }
        if( config->fsk.SyncWordLength > 8 )
        {
            return -1;
        }
        else
        {
            for(int i = 0; i < config->fsk.SyncWordLength; i++)
            {
                syncword[i] = config->fsk.SyncWord[i];
            }
        }
        SubgRf.ModulationParams.PacketType = PACKET_TYPE_GFSK;
        SubgRf.ModulationParams.Params.Gfsk.BitRate = config->fsk.BitRate;
        SubgRf.ModulationParams.Params.Gfsk.ModulationShaping = ( RadioModShapings_t ) config->fsk.ModulationShaping;
        SubgRf.ModulationParams.Params.Gfsk.Bandwidth = SUBGRF_GetFskBandwidthRegValue( config->fsk.Bandwidth );
        SubgRf.ModulationParams.Params.Gfsk.Fdev = config->fsk.FrequencyDeviation;
    
        SubgRf.PacketParams.PacketType = PACKET_TYPE_GFSK;
        SubgRf.PacketParams.Params.Gfsk.PreambleLength = ( config->fsk.PreambleLen ) << 3; // convert byte into bit
        SubgRf.PacketParams.Params.Gfsk.PreambleMinDetect = RADIO_PREAMBLE_DETECTOR_08_BITS; // don't care in tx
        SubgRf.PacketParams.Params.Gfsk.SyncWordLength = ( config->fsk.SyncWordLength ) << 3; // convert byte into bit
        SubgRf.PacketParams.Params.Gfsk.AddrComp = RADIO_ADDRESSCOMP_FILT_OFF; // don't care in tx

        if( ( config->fsk.Whitening == RADIO_FSK_DC_IBM_WHITENING ) || ( config->fsk.HeaderType == RADIO_FSK_PACKET_2BYTES_LENGTH ) )
        {
            /* Supports only RADIO_FSK_CRC_2_BYTES_IBM or RADIO_FSK_CRC_2_BYTES_CCIT */
            if( ( config->fsk.CrcLength != RADIO_FSK_CRC_2_BYTES_IBM ) && ( config->fsk.CrcLength != RADIO_FSK_CRC_2_BYTES_CCIT ) &&( config->fsk.CrcLength != RADIO_FSK_CRC_OFF ) )
            {
                return -1;
            }
            ConfigGeneric_t ConfigGeneric;
            ConfigGeneric.rtx = CONFIG_TX;
            ConfigGeneric.TxConfig = config;
            if( 0UL != RFW_Init( &ConfigGeneric, RadioEvents, &TxTimeoutTimer ) )
            {
              return -1;
            }
            /* whitening off, will be processed by FW, switch off built-in radio whitening */
            SubgRf.PacketParams.Params.Gfsk.DcFree = ( RadioDcFree_t ) RADIO_FSK_DC_FREE_OFF;
            /* Crc processed by FW, switch off built-in radio Crc */
            SubgRf.PacketParams.Params.Gfsk.CrcLength = (RadioCrcTypes_t) RADIO_CRC_OFF;
            /* length contained in Tx, but will be processed by FW after de-whitening */
            SubgRf.PacketParams.Params.Gfsk.HeaderType = ( RadioPacketLengthModes_t ) RADIO_PACKET_FIXED_LENGTH;
        }
        else
        {
          SubgRf.PacketParams.Params.Gfsk.CrcLength = ( RadioCrcTypes_t ) config->fsk.CrcLength;
          SubgRf.PacketParams.Params.Gfsk.DcFree = ( RadioDcFree_t ) config->fsk.Whitening;
          SubgRf.PacketParams.Params.Gfsk.HeaderType = ( RadioPacketLengthModes_t ) config->fsk.HeaderType;
        }

        RadioStandby( );
        RadioSetModem( MODEM_FSK );
        SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
        SUBGRF_SetPacketParams( &SubgRf.PacketParams );
        SUBGRF_SetSyncWord( syncword );
        SUBGRF_SetWhiteningSeed( config->fsk.whiteSeed );
        SUBGRF_SetCrcPolynomial(config->fsk.CrcPolynomial );
        break;
    case GENERIC_LORA:
        SubgRf.ModulationParams.PacketType = PACKET_TYPE_LORA;
        SubgRf.ModulationParams.Params.LoRa.SpreadingFactor = ( RadioLoRaSpreadingFactors_t ) config->lora.SpreadingFactor;
        SubgRf.ModulationParams.Params.LoRa.Bandwidth = ( RadioLoRaBandwidths_t ) config->lora.Bandwidth;
        SubgRf.ModulationParams.Params.LoRa.CodingRate = ( RadioLoRaCodingRates_t ) config->lora.Coderate;
        switch( config->lora.LowDatarateOptimize )
        {
          case RADIO_LORA_LOWDR_OPT_OFF:
            SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 0;
            break;
          case RADIO_LORA_LOWDR_OPT_ON:
            SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 1;
            break;
          case RADIO_LORA_LOWDR_OPT_AUTO:
            if( ( config->lora.SpreadingFactor == RADIO_LORA_SF11 ) || ( config->lora.SpreadingFactor == RADIO_LORA_SF12 ) )
            {
              SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 1;
            }
            else
            {
              SubgRf.ModulationParams.Params.LoRa.LowDatarateOptimize = 0;
            }
            break;
          default:
            break;
        }

        SubgRf.PacketParams.PacketType = PACKET_TYPE_LORA;
        SubgRf.PacketParams.Params.LoRa.PreambleLength = config->lora.PreambleLen;
        SubgRf.PacketParams.Params.LoRa.HeaderType = ( RadioLoRaPacketLengthsMode_t ) config->lora.LengthMode;
        SubgRf.PacketParams.Params.LoRa.CrcMode = ( RadioLoRaCrcModes_t ) config->lora.CrcMode;
        SubgRf.PacketParams.Params.LoRa.InvertIQ = ( RadioLoRaIQModes_t ) config->lora.IqInverted;

        RadioStandby( );
        RadioSetModem( MODEM_LORA );
        SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
        SUBGRF_SetPacketParams( &SubgRf.PacketParams );
        // WORKAROUND - Modulation Quality with 500 kHz LoRa Bandwidth, see DS_SX1261-2_V1.2 datasheet chapter 15.1
        if( SubgRf.ModulationParams.Params.LoRa.Bandwidth == LORA_BW_500 )
        {
            // RegTxModulation = @address 0x0889
            SUBGRF_WriteRegister( 0x0889, SUBGRF_ReadRegister( 0x0889 ) & ~( 1 << 2 ) );
        }
        else
        {
            // RegTxModulation = @address 0x0889
            SUBGRF_WriteRegister( 0x0889, SUBGRF_ReadRegister( 0x0889 ) | ( 1 << 2 ) );
        }
        // WORKAROUND END
        break;
    case GENERIC_BPSK:
        if( ( config->bpsk.BitRate == 0 ) || ( config->bpsk.BitRate > 1000 ) )
        {
            return -1;
        }
        RadioSetModem( MODEM_BPSK );
        SubgRf.ModulationParams.PacketType = PACKET_TYPE_BPSK;
        SubgRf.ModulationParams.Params.Bpsk.BitRate = config->bpsk.BitRate;
        SubgRf.ModulationParams.Params.Bpsk.ModulationShaping = MOD_SHAPING_DBPSK;
        SUBGRF_SetModulationParams( &SubgRf.ModulationParams );
        break;
    default:
        break;
    }

    SubgRf.AntSwitchPaSelect = SUBGRF_SetRfTxPower( power );
    RFW_SetAntSwitch( SubgRf.AntSwitchPaSelect );
    SubgRf.TxTimeout = timeout;
    return 0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
