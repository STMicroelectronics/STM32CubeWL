/*!
 * \file      radio.h
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
  * @file    radio.h
  * @author  MCD Application Team
  * @brief   Radio driver API definition
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RADIO_H__
#define __RADIO_H__

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include "radio_def.h"
#include "radio_ex.h"
#include "lr_fhss_v1_base_types.h"

/* Private typedef -----------------------------------------------------------*/

/*!
 * Radio driver internal state machine states definition
 */
typedef enum
{
    RF_IDLE = 0,   //!< The radio is idle
    RF_RX_RUNNING, //!< The radio is in reception state
    RF_TX_RUNNING, //!< The radio is in transmission state
    RF_CAD,        //!< The radio is doing channel activity detection
} RadioState_t;


typedef struct radio_lr_fhss_params_s
{
    lr_fhss_v1_params_t lr_fhss_params;
    uint32_t            center_frequency_in_hz;
    int8_t              device_offset;
} radio_lr_fhss_params_t;

/*!
 * Radio LR-FHSS configuration parameters
 */
typedef struct loramac_radio_lr_fhss_cfg_params_s
{
    int8_t               tx_rf_pwr_in_dbm;  //!< Radio RF output power
    radio_lr_fhss_params_t radio_lr_fhss_params;    //!< LR-FHSS parameters
    uint32_t             tx_timeout_in_ms;  //!< Radio tx timeout
} radio_lr_fhss_cfg_params_t;

/*!
 * Radio LoRa time on air configuration parameters
 */
typedef struct loramac_radio_lr_fhss_time_on_air_params_s
{
    radio_lr_fhss_params_t radio_lr_fhss_params;    //!< LR-FHSS parameters
    uint8_t              pld_len_in_bytes;  //!< LoRa payload length in bytes
} radio_lr_fhss_time_on_air_params_t;


/* Function prototypes -----------------------------------------------------------*/

/*!
 * \brief Radio driver definition
 */
struct Radio_s
{
    /*!
     * \brief Initializes the radio
     *
     * \param [in] events Structure containing the driver callback functions
     */
    void    ( *Init )( RadioEvents_t *events );
    /*!
     * Return current radio status
     *
     * \return status Radio status.[RF_IDLE, RF_RX_RUNNING, RF_TX_RUNNING]
     */
    RadioState_t ( *GetStatus )( void );
    /*!
     * \brief Configures the radio with the given modem
     *
     * \param [in] modem Modem to be used [0: FSK, 1: LoRa]
     */
    void    ( *SetModem )( RadioModems_t modem );
    /*!
     * \brief Sets the channel frequency
     *
     * \param [in] freq         Channel RF frequency
     */
    void    ( *SetChannel )( uint32_t freq );
    /*!
     * \brief Checks if the channel is free for the given time
     *
     * \remark The FSK modem is always used for this task as we can select the Rx bandwidth at will.
     *
     * \param [in] freq                Channel RF frequency in Hertz
     * \param [in] rxBandwidth         Rx bandwidth in Hertz
     * \param [in] rssiThresh          RSSI threshold in dBm
     * \param [in] maxCarrierSenseTime Max time in milliseconds while the RSSI is measured
     *
     * \retval isFree         [true: Channel is free, false: Channel is not free]
     */
    bool    ( *IsChannelFree )( uint32_t freq, uint32_t rxBandwidth, int16_t rssiThresh, uint32_t maxCarrierSenseTime );
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
    uint32_t ( *Random )( void );
    /*!
     * \brief Sets the reception parameters
     *
     * \param [in] modem        Radio modem to be used [0: FSK, 1: LoRa]
     * \param [in] bandwidth    Sets the bandwidth
     *                          FSK : >= 2600 and <= 250000 Hz
     *                          LoRa: [0: 125 kHz, 1: 250 kHz,
     *                                 2: 500 kHz, 3: Reserved]
     * \param [in] datarate     Sets the Datarate
     *                          FSK : 600..300000 bits/s
     *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
     *                                10: 1024, 11: 2048, 12: 4096  chips]
     * \param [in] coderate     Sets the coding rate (LoRa only)
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
     * \param [in] bandwidthAfc Sets the AFC Bandwidth (FSK only)
     *                          FSK : >= 2600 and <= 250000 Hz
     *                          LoRa: N/A ( set to 0 )
     * \param [in] preambleLen  Sets the Preamble length
     *                          FSK : Number of bytes
     *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
     * \param [in] symbTimeout  Sets the RxSingle timeout value
     *                          FSK : timeout in number of bytes
     *                          LoRa: timeout in symbols
     * \param [in] fixLen       Fixed length packets [0: variable, 1: fixed]
     * \param [in] payloadLen   Sets payload length when fixed length is used
     * \param [in] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]
     * \param [in] freqHopOn    Enables disables the intra-packet frequency hopping
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [0: OFF, 1: ON]
     * \param [in] hopPeriod    Number of symbols between each hop
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: Number of symbols
     * \param [in] iqInverted   Inverts IQ signals (LoRa only)
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [0: not inverted, 1: inverted]
     * \param [in] rxContinuous Sets the reception in continuous mode
     *                          [false: single mode, true: continuous mode]
     */
    void    ( *SetRxConfig )( RadioModems_t modem, uint32_t bandwidth,
                              uint32_t datarate, uint8_t coderate,
                              uint32_t bandwidthAfc, uint16_t preambleLen,
                              uint16_t symbTimeout, bool fixLen,
                              uint8_t payloadLen,
                              bool crcOn, bool freqHopOn, uint8_t hopPeriod,
                              bool iqInverted, bool rxContinuous );
    /*!
     * \brief Sets the transmission parameters
     *
     * \param [in] modem        Radio modem to be used [0: FSK, 1: LoRa]
     * \param [in] power        Sets the output power [dBm]
     * \param [in] fdev         Sets the frequency deviation (FSK only)
     *                          FSK : [Hz]
     *                          LoRa: 0
     * \param [in] bandwidth    Sets the bandwidth (LoRa only)
     *                          FSK : 0
     *                          LoRa: [0: 125 kHz, 1: 250 kHz,
     *                                 2: 500 kHz, 3: Reserved]
     * \param [in] datarate     Sets the Datarate
     *                          FSK : 600..300000 bits/s
     *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
     *                                10: 1024, 11: 2048, 12: 4096  chips]
     * \param [in] coderate     Sets the coding rate (LoRa only)
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
     * \param [in] preambleLen  Sets the preamble length
     *                          FSK : Number of bytes
     *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
     * \param [in] fixLen       Fixed length packets [0: variable, 1: fixed]
     * \param [in] crcOn        Enables disables the CRC [0: OFF, 1: ON]
     * \param [in] freqHopOn    Enables disables the intra-packet frequency hopping
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [0: OFF, 1: ON]
     * \param [in] hopPeriod    Number of symbols between each hop
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: Number of symbols
     * \param [in] iqInverted   Inverts IQ signals (LoRa only)
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [0: not inverted, 1: inverted]
     * \param [in] timeout      Transmission timeout [ms]
     */
    void    ( *SetTxConfig )( RadioModems_t modem, int8_t power, uint32_t fdev,
                              uint32_t bandwidth, uint32_t datarate,
                              uint8_t coderate, uint16_t preambleLen,
                              bool fixLen, bool crcOn, bool freqHopOn,
                              uint8_t hopPeriod, bool iqInverted, uint32_t timeout );
    /*!
     * \brief Checks if the given RF frequency is supported by the hardware
     *
     * \param [in] frequency RF frequency to be checked
     * \retval isSupported [true: supported, false: unsupported]
     */
    bool    ( *CheckRfFrequency )( uint32_t frequency );
    /*!
     * \brief Computes the packet time on air in ms for the given payload
     *
     * \remark Can only be called once SetRxConfig or SetTxConfig have been called
     *
     * \param [in] modem      Radio modem to be used [0: FSK, 1: LoRa]
     * \param [in] bandwidth    Sets the bandwidth
     *                          FSK : >= 2600 and <= 250000 Hz
     *                          LoRa: [0: 125 kHz, 1: 250 kHz,
     *                                 2: 500 kHz, 3: Reserved]
     * \param [in] datarate     Sets the Datarate
     *                          FSK : 600..300000 bits/s
     *                          LoRa: [6: 64, 7: 128, 8: 256, 9: 512,
     *                                10: 1024, 11: 2048, 12: 4096  chips]
     * \param [in] coderate     Sets the coding rate (LoRa only)
     *                          FSK : N/A ( set to 0 )
     *                          LoRa: [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8]
     * \param [in] preambleLen  Sets the Preamble length
     *                          FSK : Number of bytes
     *                          LoRa: Length in symbols (the hardware adds 4 more symbols)
     * \param [in] fixLen       Fixed length packets [0: variable, 1: fixed]
     * \param [in] payloadLen   Sets payload length when fixed length is used
     * \param [in] crcOn        Enables/Disables the CRC [0: OFF, 1: ON]
     *
     * \retval airTime        Computed airTime (ms) for the given packet payload length
     */
    uint32_t  ( *TimeOnAir )( RadioModems_t modem, uint32_t bandwidth,
                              uint32_t datarate, uint8_t coderate,
                              uint16_t preambleLen, bool fixLen, uint8_t payloadLen,
                              bool crcOn );
    /*!
     * \brief Sends the buffer of size. Prepares the packet to be sent and sets
     *        the radio in transmission
     *
     * \param [in] buffer     Buffer pointer
     * \param [in] size       Buffer size
     *
     * \retval status        (OK, ERROR, ...)
     */
    radio_status_t    ( *Send )( uint8_t *buffer, uint8_t size );
    /*!
     * \brief Sets the radio in sleep mode
     */
    void    ( *Sleep )( void );
    /*!
     * \brief Sets the radio in standby mode
     */
    void    ( *Standby )( void );
    /*!
     * \brief Sets the radio in reception mode for the given time
     * \param [in] timeout Reception timeout [ms]
     *                     [0: continuous, others timeout]
     */
    void    ( *Rx )( uint32_t timeout );
    /*!
     * \brief Start a Channel Activity Detection
     */
    void    ( *StartCad )( void );
    /*!
     * \brief Sets the radio in continuous wave transmission mode
     *
     * \param [in] freq       Channel RF frequency
     * \param [in] power      Sets the output power [dBm]
     * \param [in] time       Transmission mode timeout [s]
     */
    void    ( *SetTxContinuousWave )( uint32_t freq, int8_t power, uint16_t time );
    /*!
     * \brief Reads the current RSSI value
     *
     * \retval rssiValue Current RSSI value in [dBm]
     */
    int16_t ( *Rssi )( RadioModems_t modem );
    /*!
     * \brief Writes the radio register at the specified address
     *
     * \param [in] addr Register address
     * \param [in] data New register value
     */
    void    ( *Write )( uint16_t addr, uint8_t data );
    /*!
     * \brief Reads the radio register at the specified address
     *
     * \param [in] addr Register address
     * \retval data Register value
     */
    uint8_t ( *Read )( uint16_t addr );
    /*!
     * \brief Writes multiple radio registers starting at address
     *
     * \param [in] addr   First Radio register address
     * \param [in] buffer Buffer containing the new register's values
     * \param [in] size   Number of registers to be written
     */
    void    ( *WriteRegisters )( uint16_t addr, uint8_t *buffer, uint8_t size );
    /*!
     * \brief Reads multiple radio registers starting at address
     *
     * \param [in] addr First Radio register address
     * \param [out] buffer Buffer where to copy the registers data
     * \param [in] size Number of registers to be read
     */
    void    ( *ReadRegisters )( uint16_t addr, uint8_t *buffer, uint8_t size );
    /*!
     * \brief Sets the maximum payload length.
     *
     * \param [in] modem      Radio modem to be used [0: FSK, 1: LoRa]
     * \param [in] max        Maximum payload length in bytes
     */
    void    ( *SetMaxPayloadLength )( RadioModems_t modem, uint8_t max );
    /*!
     * \brief Sets the network to public or private. Updates the sync byte.
     *
     * \remark Applies to LoRa modem only
     *
     * \param [in] enable if true, it enables a public network
     */
    void    ( *SetPublicNetwork )( bool enable );
    /*!
     * \brief Gets the time required for the board plus radio to get out of sleep.[ms]
     *
     * \retval time Radio plus board wakeup time in ms.
     */
    uint32_t  ( *GetWakeupTime )( void );
    /*!
     * \brief Process radio irq
     */
    void    ( *IrqProcess )( void );
    /*!
     * \brief Sets the radio in reception mode with Max LNA gain for the given time
     *
     * \param [in] timeout Reception timeout [ms]
     *                     [0: continuous, others timeout]
     */
    void    ( *RxBoosted )( uint32_t timeout );
    /*!
     * \brief Sets the Rx duty cycle management parameters
     *
     * \param [in]  rxTime        Structure describing reception timeout value
     * \param [in]  sleepTime     Structure describing sleep timeout value
     */
    void    ( *SetRxDutyCycle )( uint32_t rxTime, uint32_t sleepTime );
    /*!
     * @brief Sets the Transmitter in continuous PRBS mode
     *
     * \remark power and datarate shall be configured prior calling TxPrbs
     */
    void    ( *TxPrbs )( void );
    /*!
     * \brief Sets the Transmitter in continuous un-modulated Carrier mode at power dBm
     *
     * \param [in] power Tx power in dBm
     */
    void    ( *TxCw )( int8_t power );
    /*!
     * \brief Sets the reception parameters
     *
     * \param [in] modem        Radio modem to be used [GENERIC_FSK or GENERIC_FSK]
     * \param [in] config       configuration of receiver
     *                          fsk field to be used if modem =GENERIC_FSK
     *                          lora field to be used if modem =GENERIC_LORA
     * \param [in] rxContinuous Sets the reception in continuous mode
     *                          [0: single mode, otherwise continuous mode]
     * \param [in] symbTimeout  Sets the RxSingle timeout value
     *                          FSK : timeout in number of bytes
     *                          LoRa: timeout in symbols
     * \return 0 when no parameters error, -1 otherwise
     */
    int32_t ( *RadioSetRxGenericConfig )( GenericModems_t modem, RxConfigGeneric_t* config, uint32_t rxContinuous, uint32_t symbTimeout );
    /*!
     * \brief Sets the transmission parameters
     *
     * \param [in] modem        Radio modem to be used [GENERIC_FSK or GENERIC_FSK or GENERIC_BPSK]
     * \param [in] config       configuration of receiver
     *                          fsk field to be used if modem =GENERIC_FSK
     *                          lora field to be used if modem =GENERIC_LORA
     *                          bpsk field to be used if modem =GENERIC_BPSK
     * \param [in] power        Sets the output power [dBm]
     * \param [in] timeout      Reception timeout [ms]
     * \return 0 when no parameters error, -1 otherwise
     */
    int32_t ( *RadioSetTxGenericConfig )( GenericModems_t modem, TxConfigGeneric_t* config, int8_t power, uint32_t timeout );
    /*!
     * \brief Starts sending long Packet, packet maybe short
     *
     * \param [in] payload_size        total payload size to be sent
     * \param [in] timeout             in ms
     * \param [in] TxLongPacketGetNextChunkCb   callback to be implemented on user side to feed partial chunk
     *                                  buffer: source buffer allocated by the app
     *                                  size: size in bytes to feed
     * \return 0 when no parameters error, -1 otherwise
     */
    int32_t ( *TransmitLongPacket )( uint16_t payload_size, uint32_t timeout,void (*TxLongPacketGetNextChunkCb) ( uint8_t** buffer, uint8_t buffer_size ) );
    /*!
     * \brief Starts receiving long Packet, packet maybe short
     *
     * \param [in] boosted_mode        boosted_mode: 0 normal Rx, 1:improved sensitivity
     * \param [in] timeout             Reception timeout [ms]
     * \param [in] RxLongStorePacketChunkCb   callback to be implemented on user side to record partial chunk in the application
     *                                  buffer: source buffer allocated in the radio driver
     *                                  size: size in bytes to record
     * \return 0 when no parameters error, -1 otherwise
     */
    int32_t ( *ReceiveLongPacket )( uint8_t boosted_mode, uint32_t timeout, void (*RxLongStorePacketChunkCb) ( uint8_t* buffer, uint8_t chunk_size ) );
    /* LrFhss extended radio functions */
    /*!
     * \brief Configure the radio LR-FHSS modem parameters
     *
     * \param [in] cfg_params LR-FHSS modem configuration parameters
     *
     * \returns Operation status
     */
    radio_status_t ( *LrFhssSetCfg)( const radio_lr_fhss_cfg_params_t *cfg_params );
    /*!
     * \brief Get the time on air in millisecond for LR-FHSS packet
     *
     * \param [in] params Pointer to LR-FHSS time on air parameters
     * \param [out] time_on_air_in_ms  time on air parameters results in ms
     *
     * \returns Time-on-air value in ms for LR-FHSS packet LrFhssGetTimeOnAirInMs
     */
    radio_status_t ( *LrFhssGetTimeOnAirInMs)( const radio_lr_fhss_time_on_air_params_t *params, uint32_t  *time_on_air_in_ms );
};

/*!
 * \brief Radio driver
 *
 * \remark This variable is defined and initialized in the specific radio
 *         board implementation
 */
extern const struct Radio_s Radio;

#ifdef __cplusplus
}
#endif

#endif // __RADIO_H__
