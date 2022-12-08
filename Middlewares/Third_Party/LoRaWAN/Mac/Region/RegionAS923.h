/*!
 * \file      RegionAS923.h
 *
 * \brief     Region definition for AS923
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
 * \defgroup  REGIONAS923 Region AS923
 *            Implementation according to LoRaWAN Specification v1.0.2.
 * \{
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    RegionAS923.h
  * @author  MCD Application Team
  * @brief   Region definition for AS923
  ******************************************************************************
  */
#ifndef __REGION_AS923_H__
#define __REGION_AS923_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "Region.h"

/*!
 * Channel plan group AS923-1
 * AS923_FREQ_OFFSET = 0
 */
#define CHANNEL_PLAN_GROUP_AS923_1                  1

/*!
 * Channel plan group AS923-2
 * AS923_FREQ_OFFSET = -1.8MHz
 */
#define CHANNEL_PLAN_GROUP_AS923_2                  2

/*!
 * Channel plan group AS923-3
 * AS923_FREQ_OFFSET = -6.6MHz
 */
#define CHANNEL_PLAN_GROUP_AS923_3                  3

/*!
 * Channel plan group AS923-4
 * AS923_FREQ_OFFSET = -5.90MHz
 */
#define CHANNEL_PLAN_GROUP_AS923_4                  4

/*!
 * Channel plan group AS923-1 for Japan
 * AS923_FREQ_OFFSET = 0
 */
#define CHANNEL_PLAN_GROUP_AS923_1_JP               5

/*!
 * LoRaMac maximum number of channels
 */
#define AS923_MAX_NB_CHANNELS                       16

/*!
 * Number of default channels
 */
#define AS923_NUMB_DEFAULT_CHANNELS                 2

/*!
 * Number of channels to apply for the CF list
 */
#define AS923_NUMB_CHANNELS_CF_LIST                 5

/*!
 * Minimal datarate that can be used by the node
 */
#define AS923_TX_MIN_DATARATE                       DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define AS923_TX_MAX_DATARATE                       DR_7

/*!
 * Minimal datarate that can be used by the node
 */
#define AS923_RX_MIN_DATARATE                       DR_0

/*!
 * Maximal datarate that can be used by the node
 */
#define AS923_RX_MAX_DATARATE                       DR_7

/*!
 * Default datarate used by the node
 */
#define AS923_DEFAULT_DATARATE                      DR_2

/*!
 * The minimum datarate which is used when the
 * dwell time is limited.
 */
#define AS923_DWELL_LIMIT_DATARATE                  DR_2

/*!
 * Minimal Rx1 receive datarate offset
 */
#define AS923_MIN_RX1_DR_OFFSET                     0

/*!
 * Maximal Rx1 receive datarate offset
 */
#define AS923_MAX_RX1_DR_OFFSET                     7

/*!
 * Minimal Tx output power that can be used by the node
 */
#define AS923_MIN_TX_POWER                          TX_POWER_7

/*!
 * Maximal Tx output power that can be used by the node
 */
#define AS923_MAX_TX_POWER                          TX_POWER_0

/*!
 * Default Tx output power used by the node
 */
#define AS923_DEFAULT_TX_POWER                      TX_POWER_0

/*!
 * Default uplink dwell time configuration
 */
#define AS923_DEFAULT_UPLINK_DWELL_TIME             1

/*!
 * Default downlink dwell time configuration
 */
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
#define AS923_DEFAULT_DOWNLINK_DWELL_TIME           1
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
#define AS923_DEFAULT_DOWNLINK_DWELL_TIME           REGION_COMMON_DEFAULT_DOWNLINK_DWELL_TIME
#endif /* REGION_VERSION */

/*!
 * Default Max EIRP
 */
#define AS923_DEFAULT_MAX_EIRP                      16.0f

/*!
 * Default antenna gain
 */
#define AS923_DEFAULT_ANTENNA_GAIN                  2.15f

/*!
 * Enabled or disabled the duty cycle
 */
#define AS923_DUTY_CYCLE_ENABLED                    0

/*!
 * Maximum RX window duration
 */
#define AS923_MAX_RX_WINDOW                         3000

#if ( AS923_DEFAULT_DATARATE > DR_5 )
#error "A default DR higher than DR_5 may lead to connectivity loss."
#endif

/*!
 * Second reception window channel frequency definition.
 */
#define AS923_RX_WND_2_FREQ                         923200000

/*!
 * Second reception window channel datarate definition.
 */
#define AS923_RX_WND_2_DR                           DR_2

/*
 * CLASS B
 */
/*!
 * Beacon frequency
 */
#define AS923_BEACON_CHANNEL_FREQ                   923400000

/*!
 * Ping slot channel frequency
 */
#define AS923_PING_SLOT_CHANNEL_FREQ                923400000

/*!
 * Payload size of a beacon frame
 */
#define AS923_BEACON_SIZE                           17

/*!
 * Size of RFU 1 field
 */
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
#define AS923_RFU1_SIZE                             2
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
#define AS923_RFU1_SIZE                             1
#endif /* REGION_VERSION */

/*!
 * Size of RFU 2 field
 */
#define AS923_RFU2_SIZE                             0

/*!
 * Datarate of the beacon channel
 */
#define AS923_BEACON_CHANNEL_DR                     DR_3

/*!
 * Bandwidth of the beacon channel
 */
#define AS923_BEACON_CHANNEL_BW                     0

/*!
 * Ping slot channel datarate
 */
#define AS923_PING_SLOT_CHANNEL_DR                  DR_3

/*!
 * Maximum number of bands
 */
#define AS923_MAX_NB_BANDS                          1

/*!
 * Band 0 definition
 * Band = { DutyCycle, TxMaxPower, LastBandUpdateTime, LastMaxCreditAssignTime, TimeCredits, MaxTimeCredits, ReadyForTransmission }
 */
#define AS923_BAND0                                 { 100, AS923_MAX_TX_POWER, 0, 0, 0, 0, 0 } //  1.0 %

/*!
 * LoRaMac default channel 1
 * Channel = { Frequency [Hz], RX1 Frequency [Hz], { ( ( DrMax << 4 ) | DrMin ) }, Band }
 */
#define AS923_LC1                                   { 923200000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }

/*!
 * LoRaMac default channel 2
 * Channel = { Frequency [Hz], RX1 Frequency [Hz], { ( ( DrMax << 4 ) | DrMin ) }, Band }
 */
#define AS923_LC2                                   { 923400000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }

/*!
 * LoRaMac channels which are allowed for the join procedure
 */
#define AS923_JOIN_CHANNELS                         ( uint16_t )( LC( 1 ) | LC( 2 ) )

/*!
 * Data rates table definition
 */
static const uint8_t DataratesAS923[]  = { 12, 11, 10,  9,  8,  7, 7, 50 };

/*!
 * Bandwidths table definition in Hz
 */
static const uint32_t BandwidthsAS923[] = { 125000, 125000, 125000, 125000, 125000, 125000, 250000, 0 };

/*!
 * Maximum payload with respect to the datarate index. Cannot operate with repeater.
 * The table is valid for the dwell time configuration of 0 for uplinks and downlinks.
 */
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
static const uint8_t MaxPayloadOfDatarateDwell0AS923[] = { 51, 51, 51, 115, 242, 242, 242, 242 };
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
static const uint8_t MaxPayloadOfDatarateDwell0AS923[] = { 51, 51, 115, 115, 242, 242, 242, 242 };
#endif /* REGION_VERSION */

/*!
 * Maximum payload with respect to the datarate index. Can operate with repeater.
 * The table is valid for the dwell time configuration of 0 for uplinks and downlinks.
 */
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
static const uint8_t MaxPayloadOfDatarateRepeaterDwell0AS923[] = { 51, 51, 51, 115, 222, 222, 222, 222 };
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
static const uint8_t MaxPayloadOfDatarateRepeaterDwell0AS923[] = { 51, 51, 115, 115, 222, 222, 222, 222 };
#endif /* REGION_VERSION */

/*!
 * Maximum payload with respect to the datarate index.
 * The table is only valid for uplinks.
 */
static const uint8_t MaxPayloadOfDatarateDwell1AS923[] = { 0, 0, 11, 53, 125, 242, 242, 242 };

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
/*!
 * Effective datarate offsets for receive window 1.
 */
static const int8_t EffectiveRx1DrOffsetAS923[] = { 0, 1, 2, 3, 4, 5, -1, -2 };
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
/*!
 * Effective datarate offsets for receive window 1 when downlink dwell time is zero.
 */
static const int8_t EffectiveRx1DrOffsetDownlinkDwell0AS923[8][8] =
    {
        { DR_0 , DR_0 , DR_0 , DR_0 , DR_0 , DR_0 , DR_1 , DR_2  }, // DR_0
        { DR_1 , DR_0 , DR_0 , DR_0 , DR_0 , DR_0 , DR_2 , DR_3  }, // DR_1
        { DR_2 , DR_1 , DR_0 , DR_0 , DR_0 , DR_0 , DR_3 , DR_4  }, // DR_2
        { DR_3 , DR_2 , DR_1 , DR_0 , DR_0 , DR_0 , DR_4 , DR_5  }, // DR_3
        { DR_4 , DR_3 , DR_2 , DR_1 , DR_0 , DR_0 , DR_5 , DR_6  }, // DR_4
        { DR_5 , DR_4 , DR_3 , DR_2 , DR_1 , DR_0 , DR_6 , DR_7  }, // DR_5
        { DR_6 , DR_5 , DR_4 , DR_3 , DR_2 , DR_1 , DR_7 , DR_7  }, // DR_6
        { DR_7 , DR_6 , DR_5 , DR_4 , DR_3 , DR_2 , DR_7 , DR_7  }, // DR_7
    };

/*!
 * Effective datarate offsets for receive window 1 when downlink dwell time is one.
 */
static const int8_t EffectiveRx1DrOffsetDownlinkDwell1AS923[8][8] =
    {
        { DR_2 , DR_2 , DR_2 , DR_2 , DR_2 , DR_2 , DR_2 , DR_2  }, // DR_0
        { DR_2 , DR_2 , DR_2 , DR_2 , DR_2 , DR_2 , DR_2 , DR_3  }, // DR_1
        { DR_2 , DR_2 , DR_2 , DR_2 , DR_2 , DR_2 , DR_3 , DR_4  }, // DR_2
        { DR_3 , DR_2 , DR_2 , DR_2 , DR_2 , DR_2 , DR_4 , DR_5  }, // DR_3
        { DR_4 , DR_3 , DR_2 , DR_2 , DR_2 , DR_2 , DR_5 , DR_6  }, // DR_4
        { DR_5 , DR_4 , DR_3 , DR_2 , DR_2 , DR_2 , DR_6 , DR_7  }, // DR_5
        { DR_6 , DR_5 , DR_4 , DR_3 , DR_2 , DR_2 , DR_7 , DR_7  }, // DR_6
        { DR_7 , DR_6 , DR_5 , DR_4 , DR_3 , DR_2 , DR_7 , DR_7  }, // DR_7
    };

#endif /* REGION_VERSION */

/*!
 * \brief The function gets a value of a specific phy attribute.
 *
 * \param [in] getPhy Pointer to the function parameters.
 *
 * \retval Returns a structure containing the PHY parameter.
 */
PhyParam_t RegionAS923GetPhyParam( GetPhyParams_t* getPhy );

/*!
 * \brief Updates the last TX done parameters of the current channel.
 *
 * \param [in] txDone Pointer to the function parameters.
 */
void RegionAS923SetBandTxDone( SetBandTxDoneParams_t* txDone );

/*!
 * \brief Initializes the channels masks and the channels.
 *
 * \param [in] params Sets the initialization type.
 */
void RegionAS923InitDefaults( InitDefaultsParams_t* params );

/*!
 * \brief Verifies a parameter.
 *
 * \param [in] verify Pointer to the function parameters.
 *
 * \param [in] phyAttribute Sets the initialization type.
 *
 * \retval Returns true, if the parameter is valid.
 */
bool RegionAS923Verify( VerifyParams_t* verify, PhyAttribute_t phyAttribute );

/*!
 * \brief The function parses the input buffer and sets up the channels of the
 *        CF list.
 *
 * \param [in] applyCFList Pointer to the function parameters.
 */
void RegionAS923ApplyCFList( ApplyCFListParams_t* applyCFList );

/*!
 * \brief Sets a channels mask.
 *
 * \param [in] chanMaskSet Pointer to the function parameters.
 *
 * \retval Returns true, if the channels mask could be set.
 */
bool RegionAS923ChanMaskSet( ChanMaskSetParams_t* chanMaskSet );

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
void RegionAS923ComputeRxWindowParameters( int8_t datarate, uint8_t minRxSymbols, uint32_t rxError, RxConfigParams_t *rxConfigParams );

/*!
 * \brief Configuration of the RX windows.
 *
 * \param [in] rxConfig Pointer to the function parameters.
 *
 * \param [out] datarate The datarate index which was set.
 *
 * \retval Returns true, if the configuration was applied successfully.
 */
bool RegionAS923RxConfig( RxConfigParams_t* rxConfig, int8_t* datarate );

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
bool RegionAS923TxConfig( TxConfigParams_t* txConfig, int8_t* txPower, TimerTime_t* txTimeOnAir );

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
uint8_t RegionAS923LinkAdrReq( LinkAdrReqParams_t* linkAdrReq, int8_t* drOut, int8_t* txPowOut, uint8_t* nbRepOut, uint8_t* nbBytesParsed );

/*!
 * \brief The function processes a RX Parameter Setup Request.
 *
 * \param [in] rxParamSetupReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
uint8_t RegionAS923RxParamSetupReq( RxParamSetupReqParams_t* rxParamSetupReq );

/*!
 * \brief The function processes a Channel Request.
 *
 * \param [in] newChannelReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
int8_t RegionAS923NewChannelReq( NewChannelReqParams_t* newChannelReq );

/*!
 * \brief The function processes a TX ParamSetup Request.
 *
 * \param [in] txParamSetupReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 *         Returns -1, if the functionality is not implemented. In this case, the end node
 *         shall not process the command.
 */
int8_t RegionAS923TxParamSetupReq( TxParamSetupReqParams_t* txParamSetupReq );

/*!
 * \brief The function processes a DlChannel Request.
 *
 * \param [in] dlChannelReq Pointer to the function parameters.
 *
 * \retval Returns the status of the operation, according to the LoRaMAC specification.
 */
int8_t RegionAS923DlChannelReq( DlChannelReqParams_t* dlChannelReq );

/*!
 * \brief Alternates the datarate of the channel for the join request.
 *
 * \param [in] currentDr Current datarate.
 *
 * \param [in] type Alternation type.
 *
 * \retval Datarate to apply.
 */
int8_t RegionAS923AlternateDr( int8_t currentDr, AlternateDrType_t type );

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
LoRaMacStatus_t RegionAS923NextChannel( NextChanParams_t* nextChanParams, uint8_t* channel, TimerTime_t* time, TimerTime_t* aggregatedTimeOff );

/*!
 * \brief Adds a channel.
 *
 * \param [in] channelAdd Pointer to the function parameters.
 *
 * \retval Status of the operation.
 */
LoRaMacStatus_t RegionAS923ChannelAdd( ChannelAddParams_t* channelAdd );

/*!
 * \brief Removes a channel.
 *
 * \param [in] channelRemove Pointer to the function parameters.
 *
 * \retval Returns true, if the channel was removed successfully.
 */
bool RegionAS923ChannelsRemove( ChannelRemoveParams_t* channelRemove  );

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
/*!
 * \brief Sets the radio into continuous wave mode.
 *
 * \param [IN] continuousWave Pointer to the function parameters.
 */
void RegionAS923SetContinuousWave( ContinuousWaveParams_t* continuousWave );
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
uint8_t RegionAS923ApplyDrOffset( uint8_t downlinkDwellTime, int8_t dr, int8_t drOffset );

/*!
 * \brief Sets the radio into beacon reception mode
 *
 * \param [in] rxBeaconSetup Pointer to the function parameters
 *
 * \param [out] outDr Datarate used to receive the beacon
 */
 void RegionAS923RxBeaconSetup( RxBeaconSetup_t* rxBeaconSetup, uint8_t* outDr );

/*! \} defgroup REGIONAS923 */

#ifdef __cplusplus
}
#endif

#endif // __REGION_AS923_H__
