/*!
 * \file      RegionCN470.h
 *
 * \brief     Region definition for CN470
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
 * \defgroup  REGIONCN470 Region CN470
 *            Implementation according to LoRaWAN Specification v1.0.2.
 * \{
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    RegionCN470.h
  * @author  MCD Application Team
  * @brief   Region definition for CN470
  ******************************************************************************
  */
#ifndef __REGION_CN470_H__
#define __REGION_CN470_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "Region.h"

/*!
 * LoRaMac maximum number of channels
 */
#define CN470_MAX_NB_CHANNELS                        96

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
/*!
 * Minimal datarate that can be used by the node
 */
#define CN470_TX_MIN_DATARATE                       DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define CN470_TX_MAX_DATARATE                       DR_5

/*!
 * Minimal datarate that can be used by the node
 */
#define CN470_RX_MIN_DATARATE                       DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define CN470_RX_MAX_DATARATE                       DR_5

/*!
 * Default datarate used by the node
 */
#define CN470_DEFAULT_DATARATE                      DR_0
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
/*!
 * Minimal datarate that can be used by the node
 */
#define CN470_TX_MIN_DATARATE                       DR_1

/*!
 * Maximal datarate that can be used by the node
 */
#define CN470_TX_MAX_DATARATE                       DR_5

/*!
 * Minimal datarate that can be used by the node
 */
#define CN470_RX_MIN_DATARATE                       DR_1

/*!
 * Maximal datarate that can be used by the node
 */
#define CN470_RX_MAX_DATARATE                       DR_5

/*!
 * Default datarate used by the node
 */
#define CN470_DEFAULT_DATARATE                      DR_1
#endif /* REGION_VERSION */

/*!
 * Minimal Rx1 receive datarate offset
 */
#define CN470_MIN_RX1_DR_OFFSET                     0

/*!
 * Maximal Rx1 receive datarate offset
 */
#define CN470_MAX_RX1_DR_OFFSET                     5

/*!
 * Minimal Tx output power that can be used by the node
 */
#define CN470_MIN_TX_POWER                          TX_POWER_7

/*!
 * Maximal Tx output power that can be used by the node
 */
#define CN470_MAX_TX_POWER                          TX_POWER_0

/*!
 * Default Tx output power used by the node
 */
#define CN470_DEFAULT_TX_POWER                      TX_POWER_0

/*!
 * Default Max EIRP
 */
#define CN470_DEFAULT_MAX_EIRP                      19.15f

/*!
 * Default antenna gain
 */
#define CN470_DEFAULT_ANTENNA_GAIN                  2.15f

/*!
 * Enabled or disabled the duty cycle
 */
#define CN470_DUTY_CYCLE_ENABLED                    0

/*!
 * Maximum RX window duration
 */
#define CN470_MAX_RX_WINDOW                         3000

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
/*!
 * Second reception window channel frequency definition.
 */
#define CN470_RX_WND_2_FREQ                         505300000

/*!
 * Second reception window channel datarate definition.
 */
#define CN470_RX_WND_2_DR                           DR_0
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
/*!
 * Second reception window channel datarate definition.
 */
#define CN470_RX_WND_2_DR                           DR_1
#endif /* REGION_VERSION */

/*!
 * Default uplink dwell time configuration
 */
#define CN470_DEFAULT_UPLINK_DWELL_TIME             0

/*
 * CLASS B
 */
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
/*!
 * Beacon frequency
 */
#define CN470_BEACON_CHANNEL_FREQ                   508300000

/*!
 * Beacon frequency channel stepwidth
 */
#define CN470_BEACON_CHANNEL_STEPWIDTH              200000

/*!
 * Ping slot channel frequency
 */
#define CN470_PING_SLOT_CHANNEL_FREQ                508300000

/*!
 * Number of possible beacon channels
 */
#define CN470_BEACON_NB_CHANNELS                    8

/*!
 * Payload size of a beacon frame
 */
#define CN470_BEACON_SIZE                           19

/*!
 * Size of RFU 1 field
 */
#define CN470_RFU1_SIZE                             3
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
/*!
 * Payload size of a beacon frame
 */
#define CN470_BEACON_SIZE                           19

/*!
 * Size of RFU 1 field
 */
#define CN470_RFU1_SIZE                             2
#endif /* REGION_VERSION */

/*!
 * Size of RFU 2 field
 */
#define CN470_RFU2_SIZE                             1

/*!
 * Datarate of the beacon channel
 */
#define CN470_BEACON_CHANNEL_DR                     DR_2

/*!
 * Bandwidth of the beacon channel
 */
#define CN470_BEACON_CHANNEL_BW                     0

/*!
 * Ping slot channel datarate
 */
#define CN470_PING_SLOT_CHANNEL_DR                  DR_2

/*!
 * LoRaMac maximum number of bands
 */
#define CN470_MAX_NB_BANDS                          1

/*!
 * Band 0 definition
 * Band = { DutyCycle, TxMaxPower, LastBandUpdateTime, LastMaxCreditAssignTime, TimeCredits, MaxTimeCredits, ReadyForTransmission }
 */
#define CN470_BAND0                                 { 1, CN470_MAX_TX_POWER, 0, 0, 0, 0, 0 } //  100.0 %

/*!
 * Defines the first channel for RX window 1 for CN470 band
 */
#define CN470_FIRST_RX1_CHANNEL                     ( (uint32_t) 500300000 )

/*!
 * Defines the last channel for RX window 1 for CN470 band
 */
#define CN470_LAST_RX1_CHANNEL                      ( (uint32_t) 509700000 )

/*!
 * Defines the step width of the channels for RX window 1
 */
#define CN470_STEPWIDTH_RX1_CHANNEL                 ( (uint32_t) 200000 )

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
#define CN470_DEFAULT_DR_RANGE                      { .Value = ( CN470_TX_MAX_DATARATE << 4 ) | CN470_TX_MIN_DATARATE }

#define CN470_COMMON_JOIN_CHANNELS \
{ \
    { .Frequency = 470900000, .Rx1Frequency = 484500000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 472500000, .Rx1Frequency = 486100000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 474100000, .Rx1Frequency = 487700000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 475700000, .Rx1Frequency = 489300000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 504100000, .Rx1Frequency = 490900000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 505700000, .Rx1Frequency = 492500000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 507300000, .Rx1Frequency = 494100000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 508900000, .Rx1Frequency = 495700000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
\
    { .Frequency = 479900000, .Rx1Frequency = 479900000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 499900000, .Rx1Frequency = 499900000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
\
    { .Frequency = 470300000, .Rx1Frequency = 492500000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 472300000, .Rx1Frequency = 492500000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 474300000, .Rx1Frequency = 492500000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 476300000, .Rx1Frequency = 492500000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 478300000, .Rx1Frequency = 492500000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
\
    { .Frequency = 480300000, .Rx1Frequency = 502500000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 482300000, .Rx1Frequency = 502500000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 484300000, .Rx1Frequency = 502500000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 486300000, .Rx1Frequency = 502500000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
    { .Frequency = 488300000, .Rx1Frequency = 502500000, CN470_DEFAULT_DR_RANGE, .Band = 0 }, \
}

#define CN470_COMMON_JOIN_CHANNELS_SIZE             20

#if ( HYBRID_ENABLED == 1 )
#define CN470_JOIN_CHANNELS                         { 0x0001, 0x0000 }
#else
#define CN470_JOIN_CHANNELS                         { 0xFFFF, 0x000F }
#endif  /* HYBRID_ENABLED */
#endif /* REGION_VERSION */

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
/*!
 * Data rates table definition
 */
static const uint8_t DataratesCN470[]  = { 12, 11, 10,  9,  8,  7 };

/*!
 * Bandwidths table definition in Hz
 */
static const uint32_t BandwidthsCN470[] = { 125000, 125000, 125000, 125000, 125000, 125000 };

/*!
 * Maximum payload with respect to the datarate index. Cannot operate with repeater.
 */
static const uint8_t MaxPayloadOfDatarateCN470[] = { 51, 51, 51, 115, 242, 242 };

/*!
 * Maximum payload with respect to the datarate index. Can operate with repeater.
 */
static const uint8_t MaxPayloadOfDatarateRepeaterCN470[] = { 51, 51, 51, 115, 222, 222 };
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
/*!
 * Data rates table definition
 */
static const uint8_t DataratesCN470[]  = { 12, 11, 10,  9,  8,  7, 7, 50 };

/*!
 * Bandwidths table definition in Hz
 */
static const uint32_t BandwidthsCN470[] = { 125000, 125000, 125000, 125000, 125000, 125000, 500000, 0 };

/*!
 * Up/Down link data rates offset definition
 */
static const int8_t DatarateOffsetsCN470[8][6] =
{
    { DR_0, DR_0, DR_0, DR_0, DR_0, DR_0 }, // DR_0
    { DR_1, DR_1, DR_1, DR_1, DR_1, DR_1 }, // DR_1
    { DR_2, DR_1, DR_1, DR_1, DR_1, DR_1 }, // DR_2
    { DR_3, DR_2, DR_1, DR_1, DR_1, DR_1 }, // DR_3
    { DR_4, DR_3, DR_2, DR_1, DR_1, DR_1 }, // DR_4
    { DR_5, DR_4, DR_3, DR_2, DR_1, DR_1 }, // DR_5
    { DR_6, DR_5, DR_4, DR_3, DR_2, DR_1 }, // DR_6
    { DR_7, DR_6, DR_5, DR_4, DR_3, DR_2 }, // DR_7
};

/*!
 * Maximum payload with respect to the datarate index. Cannot operate with repeater.
 */
static const uint8_t MaxPayloadOfDatarateCN470[] = { 0, 23, 86, 184, 242, 242, 242, 242 };

/*!
 * Maximum payload with respect to the datarate index. Can operate with repeater.
 */
static const uint8_t MaxPayloadOfDatarateRepeaterCN470[] = { 0, 23, 86, 164, 222, 222, 222, 222 };
#endif /* REGION_VERSION */

/*!
 * \brief The function gets a value of a specific phy attribute.
 *
 * \param [in] getPhy Pointer to the function parameters.
 *
 * \retval Returns a structure containing the PHY parameter.
 */
PhyParam_t RegionCN470GetPhyParam( GetPhyParams_t* getPhy );

/*!
 * \brief Updates the last TX done parameters of the current channel.
 *
 * \param [in] txDone Pointer to the function parameters.
 */
void RegionCN470SetBandTxDone( SetBandTxDoneParams_t* txDone );

/*!
 * \brief Initializes the channels masks and the channels.
 *
 * \param [in] params Sets the initialization type.
 */
void RegionCN470InitDefaults( InitDefaultsParams_t* params );

/*!
 * \brief Verifies a parameter.
 *
 * \param [in] verify Pointer to the function parameters.
 *
 * \param [in] phyAttribute Sets the initialization type.
 *
 * \retval Returns true, if the parameter is valid.
 */
bool RegionCN470Verify( VerifyParams_t* verify, PhyAttribute_t phyAttribute );

/*!
 * \brief The function parses the input buffer and sets up the channels of the
 *        CF list.
 *
 * \param [in] applyCFList Pointer to the function parameters.
 */
void RegionCN470ApplyCFList( ApplyCFListParams_t* applyCFList );

/*!
 * \brief Sets a channels mask.
 *
 * \param [in] chanMaskSet Pointer to the function parameters.
 *
 * \retval Returns true, if the channels mask could be set.
 */
bool RegionCN470ChanMaskSet( ChanMaskSetParams_t* chanMaskSet );

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
void RegionCN470ComputeRxWindowParameters( int8_t datarate, uint8_t minRxSymbols, uint32_t rxError, RxConfigParams_t *rxConfigParams );

/*!
 * \brief Configuration of the RX windows.
 *
 * \param [in] rxConfig Pointer to the function parameters.
 *
 * \param [out] datarate The datarate index which was set.
 *
 * \retval Returns true, if the configuration was applied successfully.
 */
bool RegionCN470RxConfig( RxConfigParams_t* rxConfig, int8_t* datarate );

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
bool RegionCN470TxConfig( TxConfigParams_t* txConfig, int8_t* txPower, TimerTime_t* txTimeOnAir );

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
uint8_t RegionCN470LinkAdrReq( LinkAdrReqParams_t* linkAdrReq, int8_t* drOut, int8_t* txPowOut, uint8_t* nbRepOut, uint8_t* nbBytesParsed );

/*!
 * \brief The function processes a RX Parameter Setup Request.
 *
 * \param [in] rxParamSetupReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
uint8_t RegionCN470RxParamSetupReq( RxParamSetupReqParams_t* rxParamSetupReq );

/*!
 * \brief The function processes a Channel Request.
 *
 * \param [in] newChannelReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
int8_t RegionCN470NewChannelReq( NewChannelReqParams_t* newChannelReq );

/*!
 * \brief The function processes a TX ParamSetup Request.
 *
 * \param [in] txParamSetupReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 *         Returns -1, if the functionality is not implemented. In this case, the end node
 *         shall not process the command.
 */
int8_t RegionCN470TxParamSetupReq( TxParamSetupReqParams_t* txParamSetupReq );

/*!
 * \brief The function processes a DlChannel Request.
 *
 * \param [in] dlChannelReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
int8_t RegionCN470DlChannelReq( DlChannelReqParams_t* dlChannelReq );

/*!
 * \brief Alternates the datarate of the channel for the join request.
 *
 * \param [in] currentDr Current datarate.
 *
 * \param [in] type Alternation type.
 *
 * \retval Datarate to apply.
 */
int8_t RegionCN470AlternateDr( int8_t currentDr, AlternateDrType_t type );

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
LoRaMacStatus_t RegionCN470NextChannel( NextChanParams_t* nextChanParams, uint8_t* channel, TimerTime_t* time, TimerTime_t* aggregatedTimeOff );

/*!
 * \brief Adds a channel.
 *
 * \param [in] channelAdd Pointer to the function parameters.
 *
 * \retval Status of the operation.
 */
LoRaMacStatus_t RegionCN470ChannelAdd( ChannelAddParams_t* channelAdd );

/*!
 * \brief Removes a channel.
 *
 * \param [in] channelRemove Pointer to the function parameters.
 *
 * \retval Returns true, if the channel was removed successfully.
 */
bool RegionCN470ChannelsRemove( ChannelRemoveParams_t* channelRemove  );

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
/*!
 * \brief Sets the radio into continuous wave mode.
 *
 * \param [IN] continuousWave Pointer to the function parameters.
 */
void RegionCN470SetContinuousWave( ContinuousWaveParams_t* continuousWave );
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
uint8_t RegionCN470ApplyDrOffset( uint8_t downlinkDwellTime, int8_t dr, int8_t drOffset );

/*!
 * \brief Sets the radio into beacon reception mode
 *
 * \param [in] rxBeaconSetup Pointer to the function parameters
 *
 * \param [out] outDr Datarate used to receive the beacon
 */
void RegionCN470RxBeaconSetup( RxBeaconSetup_t* rxBeaconSetup, uint8_t* outDr );

/*! \} defgroup REGIONCN470 */

#ifdef __cplusplus
}
#endif

#endif // __REGION_CN470_H__
