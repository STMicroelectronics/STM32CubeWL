/*!
 * \file      RegionCN779.h
 *
 * \brief     Region definition for CN779
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
 *
 * \defgroup  REGIONCN779 Region CN779
 *            Implementation according to LoRaWAN Specification v1.0.2.
 * \{
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    RegionCN779.h
  * @author  MCD Application Team
  * @brief   Region definition for CN779
  ******************************************************************************
  */
#ifndef __REGION_CN779_H__
#define __REGION_CN779_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "Region.h"

/*!
 * LoRaMac maximum number of channels
 */
#define CN779_MAX_NB_CHANNELS                       16

/*!
 * Number of default channels
 */
#define CN779_NUMB_DEFAULT_CHANNELS                 3

/*!
 * Number of channels to apply for the CF list
 */
#define CN779_NUMB_CHANNELS_CF_LIST                 5

/*!
 * Minimal datarate that can be used by the node
 */
#define CN779_TX_MIN_DATARATE                       DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define CN779_TX_MAX_DATARATE                       DR_7

/*!
 * Minimal datarate that can be used by the node
 */
#define CN779_RX_MIN_DATARATE                       DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define CN779_RX_MAX_DATARATE                       DR_7

/*!
 * Default datarate used by the node
 */
#define CN779_DEFAULT_DATARATE                      DR_0

/*!
 * Minimal Rx1 receive datarate offset
 */
#define CN779_MIN_RX1_DR_OFFSET                     0

/*!
 * Maximal Rx1 receive datarate offset
 */
#define CN779_MAX_RX1_DR_OFFSET                     5

/*!
 * Minimal Tx output power that can be used by the node
 */
#define CN779_MIN_TX_POWER                          TX_POWER_5

/*!
 * Maximal Tx output power that can be used by the node
 */
#define CN779_MAX_TX_POWER                          TX_POWER_0

/*!
 * Default Tx output power used by the node
 */
#define CN779_DEFAULT_TX_POWER                      TX_POWER_0

/*!
 * Default Max EIRP
 */
#define CN779_DEFAULT_MAX_EIRP                      12.15f

/*!
 * Default antenna gain
 */
#define CN779_DEFAULT_ANTENNA_GAIN                  2.15f

/*!
 * Enabled or disabled the duty cycle
 */
#define CN779_DUTY_CYCLE_ENABLED                    1

/*!
 * Maximum RX window duration
 */
#define CN779_MAX_RX_WINDOW                         3000

/*!
 * Verification of default datarate
 */
#if ( CN779_DEFAULT_DATARATE > DR_5 )
#error "A default DR higher than DR_5 may lead to connectivity loss."
#endif

/*!
 * Second reception window channel frequency definition.
 */
#define CN779_RX_WND_2_FREQ                         786000000

/*!
 * Second reception window channel datarate definition.
 */
#define CN779_RX_WND_2_DR                           DR_0

/*!
 * Default uplink dwell time configuration
 */
#define CN779_DEFAULT_UPLINK_DWELL_TIME             0

/*
 * CLASS B
 */
/*!
 * Beacon frequency
 */
#define CN779_BEACON_CHANNEL_FREQ                   785000000

/*!
 * Ping slot channel frequency
 */
#define CN779_PING_SLOT_CHANNEL_FREQ                785000000

/*!
 * Payload size of a beacon frame
 */
#define CN779_BEACON_SIZE                           17

/*!
 * Size of RFU 1 field
 */
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
#define CN779_RFU1_SIZE                             2
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
#define CN779_RFU1_SIZE                             1
#endif /* REGION_VERSION */

/*!
 * Size of RFU 2 field
 */
#define CN779_RFU2_SIZE                             0

/*!
 * Datarate of the beacon channel
 */
#define CN779_BEACON_CHANNEL_DR                     DR_3

/*!
 * Bandwidth of the beacon channel
 */
#define CN779_BEACON_CHANNEL_BW                     0

/*!
 * Ping slot channel datarate
 */
#define CN779_PING_SLOT_CHANNEL_DR                  DR_3

/*!
 * LoRaMac maximum number of bands
 */
#define CN779_MAX_NB_BANDS                           1

/*!
 * Band 0 definition
 * Band = { DutyCycle, TxMaxPower, LastBandUpdateTime, LastMaxCreditAssignTime, TimeCredits, MaxTimeCredits, ReadyForTransmission }
 */
#define CN779_BAND0                                 { 100, CN779_MAX_TX_POWER, 0, 0, 0, 0, 0 } //  1.0 %

/*!
 * LoRaMac default channel 1
 * Channel = { Frequency [Hz], RX1 Frequency [Hz], { ( ( DrMax << 4 ) | DrMin ) }, Band }
 */
#define CN779_LC1                                   { 779500000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
/*!
 * LoRaMac default channel 2
 * Channel = { Frequency [Hz], RX1 Frequency [Hz], { ( ( DrMax << 4 ) | DrMin ) }, Band }
 */
#define CN779_LC2                                   { 779700000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }

/*!
 * LoRaMac default channel 3
 * Channel = { Frequency [Hz], RX1 Frequency [Hz], { ( ( DrMax << 4 ) | DrMin ) }, Band }
 */
#define CN779_LC3                                   { 779900000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }

/*!
 * LoRaMac channels which are allowed for the join procedure
 */
#define CN779_JOIN_CHANNELS                         ( uint16_t )( LC( 1 ) | LC( 2 ) | LC( 3 ) )

/*!
 * Data rates table definition
 */
static const uint8_t DataratesCN779[]  = { 12, 11, 10,  9,  8,  7,  7, 50 };

/*!
 * Bandwidths table definition in Hz
 */
static const uint32_t BandwidthsCN779[] = { 125000, 125000, 125000, 125000, 125000, 125000, 250000, 0 };

/*!
 * Maximum payload with respect to the datarate index. Cannot operate with repeater.
 */
static const uint8_t MaxPayloadOfDatarateCN779[] = { 51, 51, 51, 115, 242, 242, 242, 242 };

/*!
 * Maximum payload with respect to the datarate index. Can operate with repeater.
 */
static const uint8_t MaxPayloadOfDatarateRepeaterCN779[] = { 51, 51, 51, 115, 222, 222, 222, 222 };

/*!
 * \brief The function gets a value of a specific phy attribute.
 *
 * \param [in] getPhy Pointer to the function parameters.
 *
 * \retval Returns a structure containing the PHY parameter.
 */
PhyParam_t RegionCN779GetPhyParam( GetPhyParams_t* getPhy );

/*!
 * \brief Updates the last TX done parameters of the current channel.
 *
 * \param [in] txDone Pointer to the function parameters.
 */
void RegionCN779SetBandTxDone( SetBandTxDoneParams_t* txDone );

/*!
 * \brief Initializes the channels masks and the channels.
 *
 * \param [in] params Sets the initialization type.
 */
void RegionCN779InitDefaults( InitDefaultsParams_t* params );

/*!
 * \brief Verifies a parameter.
 *
 * \param [in] verify Pointer to the function parameters.
 *
 * \param [in] phyAttribute Sets the initialization type.
 *
 * \retval Returns true, if the parameter is valid.
 */
bool RegionCN779Verify( VerifyParams_t* verify, PhyAttribute_t phyAttribute );

/*!
 * \brief The function parses the input buffer and sets up the channels of the
 *        CF list.
 *
 * \param [in] applyCFList Pointer to the function parameters.
 */
void RegionCN779ApplyCFList( ApplyCFListParams_t* applyCFList );

/*!
 * \brief Sets a channels mask.
 *
 * \param [in] chanMaskSet Pointer to the function parameters.
 *
 * \retval Returns true, if the channels mask could be set.
 */
bool RegionCN779ChanMaskSet( ChanMaskSetParams_t* chanMaskSet );

/*!
 * Computes the Rx window timeout and offset.
 *
 * \param [in] datarate     Rx window datarate index to be used
 *
 * \param [in] minRxSymbols Minimum required number of symbols to detect an Rx frame.
 *
 * \param [in] rxError      System maximum timing error of the receiver. In milliseconds
 *                          The receiver will turn on in a [-rxError : +rxError] ms
 *                          interval around RxOffset
 *
 * \param [out] rxConfigParams Returns updated WindowTimeout and WindowOffset fields.
 */
void RegionCN779ComputeRxWindowParameters( int8_t datarate, uint8_t minRxSymbols, uint32_t rxError, RxConfigParams_t *rxConfigParams );

/*!
 * \brief Configuration of the RX windows.
 *
 * \param [in] rxConfig Pointer to the function parameters.
 *
 * \param [out] datarate The datarate index which was set.
 *
 * \retval Returns true, if the configuration was applied successfully.
 */
bool RegionCN779RxConfig( RxConfigParams_t* rxConfig, int8_t* datarate );

/*!
 * \brief TX configuration.
 *
 * \param [in] txConfig Pointer to the function parameters.
 *
 * \param [out] txPower The tx power index which was set.
 *
 * \param [out] txTimeOnAir The time-on-air of the frame.
 *
 * \retval Returns true, if the configuration was applied successfully.
 */
bool RegionCN779TxConfig( TxConfigParams_t* txConfig, int8_t* txPower, TimerTime_t* txTimeOnAir );

/*!
 * \brief The function processes a Link ADR Request.
 *
 * \param [in] linkAdrReq Pointer to the function parameters.
 *
 * \param [out] drOut The datarate which was applied.
 *
 * \param [out] txPowOut The TX power which was applied.
 *
 * \param [out] nbRepOut The number of repetitions to apply.
 *
 * \param [out] nbBytesParsed The number bytes which were parsed.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
uint8_t RegionCN779LinkAdrReq( LinkAdrReqParams_t* linkAdrReq, int8_t* drOut, int8_t* txPowOut, uint8_t* nbRepOut, uint8_t* nbBytesParsed );

/*!
 * \brief The function processes a RX Parameter Setup Request.
 *
 * \param [in] rxParamSetupReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
uint8_t RegionCN779RxParamSetupReq( RxParamSetupReqParams_t* rxParamSetupReq );

/*!
 * \brief The function processes a Channel Request.
 *
 * \param [in] newChannelReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
int8_t RegionCN779NewChannelReq( NewChannelReqParams_t* newChannelReq );

/*!
 * \brief The function processes a TX ParamSetup Request.
 *
 * \param [in] txParamSetupReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 *         Returns -1, if the functionality is not implemented. In this case, the end node
 *         shall not process the command.
 */
int8_t RegionCN779TxParamSetupReq( TxParamSetupReqParams_t* txParamSetupReq );

/*!
 * \brief The function processes a DlChannel Request.
 *
 * \param [in] dlChannelReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
int8_t RegionCN779DlChannelReq( DlChannelReqParams_t* dlChannelReq );

/*!
 * \brief Alternates the datarate of the channel for the join request.
 *
 * \param [in] currentDr Current datarate.
 *
 * \param [in] type Alternation type.
 *
 * \retval Datarate to apply.
 */
int8_t RegionCN779AlternateDr( int8_t currentDr, AlternateDrType_t type );

/*!
 * \brief Searches and set the next random available channel
 *
 * \param [in] nextChanParams pointer of selected channel parameters
 *
 * \param [out] channel Next channel to use for TX.
 *
 * \param [out] time Time to wait for the next transmission according to the duty
 *              cycle.
 *
 * \param [out] aggregatedTimeOff Updates the aggregated time off.
 *
 * \retval Function status [1: OK, 0: Unable to find a channel on the current datarate]
 */
LoRaMacStatus_t RegionCN779NextChannel( NextChanParams_t* nextChanParams, uint8_t* channel, TimerTime_t* time, TimerTime_t* aggregatedTimeOff );

/*!
 * \brief Adds a channel.
 *
 * \param [in] channelAdd Pointer to the function parameters.
 *
 * \retval Status of the operation.
 */
LoRaMacStatus_t RegionCN779ChannelAdd( ChannelAddParams_t* channelAdd );

/*!
 * \brief Removes a channel.
 *
 * \param [in] channelRemove Pointer to the function parameters.
 *
 * \retval Returns true, if the channel was removed successfully.
 */
bool RegionCN779ChannelsRemove( ChannelRemoveParams_t* channelRemove  );

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
/*!
 * \brief Sets the radio into continuous wave mode.
 *
 * \param [IN] continuousWave Pointer to the function parameters.
 */
void RegionCN779SetContinuousWave( ContinuousWaveParams_t* continuousWave );
#endif /* REGION_VERSION */

/*!
 * \brief Computes new datarate according to the given offset
 *
 * \param [in] downlinkDwellTime Downlink dwell time configuration. 0: No limit, 1: 400ms
 *
 * \param [in] dr Current datarate
 *
 * \param [in] drOffset Offset to be applied
 *
 * \retval newDr Computed datarate.
 */
uint8_t RegionCN779ApplyDrOffset( uint8_t downlinkDwellTime, int8_t dr, int8_t drOffset );

/*!
 * \brief Sets the radio into beacon reception mode
 *
 * \param [in] rxBeaconSetup Pointer to the function parameters
 *
 * \param [out] outDr Datarate used to receive the beacon
 */
 void RegionCN779RxBeaconSetup( RxBeaconSetup_t* rxBeaconSetup, uint8_t* outDr );

/*! \} defgroup REGIONCN779 */

#ifdef __cplusplus
}
#endif

#endif // __REGION_CN779_H__
