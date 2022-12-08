/*!
 * \file      RegionCN470.c
 *
 * \brief     Region implementation for CN470
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
*/
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    RegionCN470.c
  * @author  MCD Application Team
  * @brief   Region implementation for CN470
  ******************************************************************************
  */
#include "radio.h"
#include "RegionCN470.h"
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
#include "RegionBaseUS.h"
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
#include "RegionCN470A20.h"
#include "RegionCN470B20.h"
#include "RegionCN470A26.h"
#include "RegionCN470B26.h"
#endif /* REGION_VERSION */

// Definitions
#define CHANNELS_MASK_SIZE              6

/* The HYBRID_DEFAULT_MASKx define the enabled channels in Hybrid mode*/
/* Note: they can be redefined in lorawan_conf.h*/
#ifndef HYBRID_DEFAULT_MASK0 /*enabled channels from channel 15 down to channel 0*/
#define HYBRID_DEFAULT_MASK0 0x00FF /*channel 7 down to channel 0  enabled*/
#endif
#ifndef HYBRID_DEFAULT_MASK1 /*enabled channels from channel 31 down to channel 16*/
#define HYBRID_DEFAULT_MASK1 0x0000
#endif
#ifndef HYBRID_DEFAULT_MASK2 /*enabled channels from channel 47 down to channel 32*/
#define HYBRID_DEFAULT_MASK2 0x0000
#endif
#ifndef HYBRID_DEFAULT_MASK3 /*enabled channels from channel 63 down to channel 48*/
#define HYBRID_DEFAULT_MASK3 0x0000
#endif
#ifndef HYBRID_DEFAULT_MASK4 /*enabled channels from channel 79 down to channel 64*/
#define HYBRID_DEFAULT_MASK4 0x0000
#endif
#ifndef HYBRID_DEFAULT_MASK5 /*enabled channels from channel 95 down to channel 80*/
#define HYBRID_DEFAULT_MASK5 0x0000
#endif

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
#ifndef REGION_CN470_DEFAULT_CHANNEL_PLAN
#define REGION_CN470_DEFAULT_CHANNEL_PLAN CHANNEL_PLAN_20MHZ_TYPE_A
#endif

#ifndef REGION_CN470_DEFAULT_RX_WND_2_FREQ
#define REGION_CN470_DEFAULT_RX_WND_2_FREQ CN470_A20_RX_WND_2_FREQ_ABP
#endif

ChannelParams_t CommonJoinChannels[] = CN470_COMMON_JOIN_CHANNELS;

/*!
 * Definition of the regional channel plan.
 */
typedef struct sRegionCN470ChannelPlanCtx
{
    /*!
     * Size of the channels mask. Must be smaller
     * or equal than CHANNELS_MASK_SIZE.
     */
    uint8_t ChannelsMaskSize;
    /*!
     * Number of elements in the join accept list.
     */
    uint8_t JoinAcceptListSize;
    /*!
     * Number of available channels for beaconing.
     */
    uint8_t NbBeaconChannels;
    /*!
     * Number of available channels for ping slots.
     */
    uint8_t NbPingSlotChannels;
    /*!
     * \brief Calculation of the beacon frequency.
     *
     * \param [in] channel The Beacon channel number.
     *
     * \param [in] joinChannelIndex The join channel index.
     *
     * \param [in] isPingSlot Set to true, if its a ping slot.
     *
     * \retval Returns the beacon frequency.
     */
    uint32_t ( *GetDownlinkFrequency )( uint8_t channel, uint8_t joinChannelIndex, bool isPingSlot );
    /*!
     * \brief Performs the update of the channelsMask based on the input parameters.
     *
     * \param [in] joinChannelIndex The join channel index.
     *
     * \retval Returns the offset for the given join channel.
     */
    uint8_t ( *GetBeaconChannelOffset )( uint8_t joinChannelIndex );
    /*!
     * \brief Performs the update of the channelsMask based on the input parameters.
     *
     * \param [in] channelsMask A pointer to the channels mask.
     *
     * \param [in] chMaskCntl The value of the chMaskCntl field of the LinkAdrReq.
     *
     * \param [in] chanMask The value of the chanMask field of the LinkAdrReq.
     *
     * \param [in] channels A pointer to the available channels.
     *
     * \retval Status of the operation. Return 0x07 if the channels mask is valid.
     */
    uint8_t ( *LinkAdrChMaskUpdate )( uint16_t* channelsMask, uint8_t chMaskCntl,
                                      uint16_t chanMask, ChannelParams_t* channels );
    /*!
     * \brief Verifies if the frequency provided is valid.
     *
     * \param [in] frequency The frequency to verify.
     *
     * \retval Returns true, if the frequency is valid.
     */
    bool ( *VerifyRfFreq )( uint32_t frequency );
    /*!
     * \brief Initializes all channels, datarates, frequencies and bands.
     *
     * \param [in] channels A pointer to the available channels.
     */
    void ( *InitializeChannels )( ChannelParams_t* channels );
    /*!
     * \brief Initializes the channels mask and the channels default mask.
     *
     * \param [in] channelsDefaultMask A pointer to the channels default mask.
     */
    void ( *InitializeChannelsMask )( uint16_t* channelsDefaultMask );
    /*!
     * \brief Computes the frequency for the RX1 window.
     *
     * \param [in] channel The channel utilized currently.
     *
     * \retval Returns the frequency which shall be used.
     */
    uint32_t ( *GetRx1Frequency )( uint8_t channel );
    /*!
     * \brief Computes the frequency for the RX2 window.
     *
     * \param [in] joinChannelIndex The join channel index.
     *
     * \param [in] isOtaaDevice Set to true, if the device is an OTAA device.
     *
     * \retval Returns the frequency which shall be used.
     */
    uint32_t ( *GetRx2Frequency )( uint8_t joinChannelIndex, bool isOtaaDevice );
}RegionCN470ChannelPlanCtx_t;
#endif /* REGION_VERSION */

#if defined( REGION_CN470 )
/*
 * Non-volatile module context.
 */
static RegionNvmDataGroup1_t* RegionNvmGroup1;
static RegionNvmDataGroup2_t* RegionNvmGroup2;
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
static Band_t* RegionBands;

/*
 * Context for the current channel plan.
 */
static RegionCN470ChannelPlanCtx_t ChannelPlanCtx;

// Static functions
static void ApplyChannelPlanConfig( RegionCN470ChannelPlan_t channelPlan, RegionCN470ChannelPlanCtx_t* ctx )
{
    switch( channelPlan )
    {
        case CHANNEL_PLAN_20MHZ_TYPE_A:
        {
            ctx->ChannelsMaskSize = CN470_A20_CHANNELS_MASK_SIZE;
            ctx->JoinAcceptListSize = CN470_A20_JOIN_ACCEPT_LIST_SIZE;
            ctx->NbBeaconChannels = CN470_A20_BEACON_NB_CHANNELS;
            ctx->NbPingSlotChannels = CN470_A20_PING_SLOT_NB_CHANNELS;
            ctx->GetDownlinkFrequency = RegionCN470A20GetDownlinkFrequency;
            ctx->GetBeaconChannelOffset = RegionCN470A20GetBeaconChannelOffset;
            ctx->LinkAdrChMaskUpdate = RegionCN470A20LinkAdrChMaskUpdate;
            ctx->VerifyRfFreq = RegionCN470A20VerifyRfFreq;
            ctx->InitializeChannels = RegionCN470A20InitializeChannels;
            ctx->InitializeChannelsMask = RegionCN470A20InitializeChannelsMask;
            ctx->GetRx1Frequency = RegionCN470A20GetRx1Frequency;
            ctx->GetRx2Frequency = RegionCN470A20GetRx2Frequency;
            break;
        }
        case CHANNEL_PLAN_20MHZ_TYPE_B:
        {
            ctx->ChannelsMaskSize = CN470_B20_CHANNELS_MASK_SIZE;
            ctx->JoinAcceptListSize = CN470_B20_JOIN_ACCEPT_LIST_SIZE;
            ctx->NbBeaconChannels = CN470_B20_BEACON_NB_CHANNELS;
            ctx->NbPingSlotChannels = CN470_B20_PING_SLOT_NB_CHANNELS;
            ctx->GetDownlinkFrequency = RegionCN470B20GetDownlinkFrequency;
            ctx->GetBeaconChannelOffset = RegionCN470B20GetBeaconChannelOffset;
            ctx->LinkAdrChMaskUpdate = RegionCN470B20LinkAdrChMaskUpdate;
            ctx->VerifyRfFreq = RegionCN470B20VerifyRfFreq;
            ctx->InitializeChannels = RegionCN470B20InitializeChannels;
            ctx->InitializeChannelsMask = RegionCN470B20InitializeChannelsMask;
            ctx->GetRx1Frequency = RegionCN470B20GetRx1Frequency;
            ctx->GetRx2Frequency = RegionCN470B20GetRx2Frequency;
            break;
        }
        case CHANNEL_PLAN_26MHZ_TYPE_A:
        {
            ctx->ChannelsMaskSize = CN470_A26_CHANNELS_MASK_SIZE;
            ctx->JoinAcceptListSize = CN470_A26_JOIN_ACCEPT_LIST_SIZE;
            ctx->NbBeaconChannels = CN470_A26_BEACON_NB_CHANNELS;
            ctx->NbPingSlotChannels = CN470_A26_PING_SLOT_NB_CHANNELS;
            ctx->GetDownlinkFrequency = RegionCN470A26GetDownlinkFrequency;
            ctx->GetBeaconChannelOffset = RegionCN470A26GetBeaconChannelOffset;
            ctx->LinkAdrChMaskUpdate = RegionCN470A26LinkAdrChMaskUpdate;
            ctx->VerifyRfFreq = RegionCN470A26VerifyRfFreq;
            ctx->InitializeChannels = RegionCN470A26InitializeChannels;
            ctx->InitializeChannelsMask = RegionCN470A26InitializeChannelsMask;
            ctx->GetRx1Frequency = RegionCN470A26GetRx1Frequency;
            ctx->GetRx2Frequency = RegionCN470A26GetRx2Frequency;
            break;
        }
        case CHANNEL_PLAN_26MHZ_TYPE_B:
        {
            ctx->ChannelsMaskSize = CN470_B26_CHANNELS_MASK_SIZE;
            ctx->JoinAcceptListSize = CN470_B26_JOIN_ACCEPT_LIST_SIZE;
            ctx->NbBeaconChannels = CN470_B26_BEACON_NB_CHANNELS;
            ctx->NbPingSlotChannels = CN470_B26_PING_SLOT_NB_CHANNELS;
            ctx->GetDownlinkFrequency = RegionCN470B26GetDownlinkFrequency;
            ctx->GetBeaconChannelOffset = RegionCN470B26GetBeaconChannelOffset;
            ctx->LinkAdrChMaskUpdate = RegionCN470B26LinkAdrChMaskUpdate;
            ctx->VerifyRfFreq = RegionCN470B26VerifyRfFreq;
            ctx->InitializeChannels = RegionCN470B26InitializeChannels;
            ctx->InitializeChannelsMask = RegionCN470B26InitializeChannelsMask;
            ctx->GetRx1Frequency = RegionCN470B26GetRx1Frequency;
            ctx->GetRx2Frequency = RegionCN470B26GetRx2Frequency;
            break;
        }
        default:
        {
            // Apply CHANNEL_PLAN_20MHZ_TYPE_A
            ctx->ChannelsMaskSize = CN470_A20_CHANNELS_MASK_SIZE;
            ctx->JoinAcceptListSize = CN470_A20_JOIN_ACCEPT_LIST_SIZE;
            ctx->NbBeaconChannels = CN470_A20_BEACON_NB_CHANNELS;
            ctx->NbPingSlotChannels = CN470_A20_PING_SLOT_NB_CHANNELS;
            ctx->GetDownlinkFrequency = RegionCN470A20GetDownlinkFrequency;
            ctx->GetBeaconChannelOffset = RegionCN470A20GetBeaconChannelOffset;
            ctx->LinkAdrChMaskUpdate = RegionCN470A20LinkAdrChMaskUpdate;
            ctx->VerifyRfFreq = RegionCN470A20VerifyRfFreq;
            ctx->InitializeChannels = RegionCN470A20InitializeChannels;
            ctx->InitializeChannelsMask = RegionCN470A20InitializeChannelsMask;
            ctx->GetRx1Frequency = RegionCN470A20GetRx1Frequency;
            ctx->GetRx2Frequency = RegionCN470A20GetRx2Frequency;
            break;
        }
    }
}

static RegionCN470ChannelPlan_t IdentifyChannelPlan( uint8_t joinChannel )
{
    RegionCN470ChannelPlan_t channelPlan = CHANNEL_PLAN_UNKNOWN;

    if( joinChannel <= 7 )
    {
        channelPlan = CHANNEL_PLAN_20MHZ_TYPE_A;
    }
    else if ( ( joinChannel <= 9 ) && ( joinChannel >= 8 ) )
    {
        channelPlan = CHANNEL_PLAN_20MHZ_TYPE_B;
    }
    else if ( ( joinChannel <= 14 ) && ( joinChannel >= 10 ) )
    {
        channelPlan = CHANNEL_PLAN_26MHZ_TYPE_A;
    }
    else if( ( joinChannel <= 19 ) && ( joinChannel >= 15 ) )
    {
        channelPlan = CHANNEL_PLAN_26MHZ_TYPE_B;
    }
    return channelPlan;
}
#endif /* REGION_VERSION */

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
static bool VerifyRfFreq( uint32_t freq )
{
    // Check radio driver support
    if( Radio.CheckRfFrequency( freq ) == false )
    {
        return false;
    }

    // Rx frequencies
    if( ( freq < CN470_FIRST_RX1_CHANNEL ) ||
        ( freq > CN470_LAST_RX1_CHANNEL ) ||
        ( ( ( freq - ( uint32_t ) CN470_FIRST_RX1_CHANNEL ) % ( uint32_t ) CN470_STEPWIDTH_RX1_CHANNEL ) != 0 ) )
    {
        return false;
    }

    // Test for frequency range - take RX and TX frequencies into account
    if( ( freq < 470300000 ) ||  ( freq > 509700000 ) )
    {
        return false;
    }
    return true;
}
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
static bool VerifyRfFreq( uint32_t frequency )
{
    // Check radio driver support
    if( Radio.CheckRfFrequency( frequency ) == false )
    {
        return false;
    }

    return ChannelPlanCtx.VerifyRfFreq( frequency );
}
#endif /* REGION_VERSION */

static TimerTime_t GetTimeOnAir( int8_t datarate, uint16_t pktLen )
{
    int8_t phyDr = DataratesCN470[datarate];
    uint32_t bandwidth = RegionCommonGetBandwidth( datarate, BandwidthsCN470 );

    return Radio.TimeOnAir( MODEM_LORA, bandwidth, phyDr, 1, 8, false, pktLen, true );
}
#endif /* REGION_CN470 */

PhyParam_t RegionCN470GetPhyParam( GetPhyParams_t* getPhy )
{
    PhyParam_t phyParam = { 0 };

#if defined( REGION_CN470 )
    switch( getPhy->Attribute )
    {
        case PHY_MIN_RX_DR:
        {
            phyParam.Value = CN470_RX_MIN_DATARATE;
            break;
        }
        case PHY_MIN_TX_DR:
        {
            phyParam.Value = CN470_TX_MIN_DATARATE;
            break;
        }
        case PHY_DEF_TX_DR:
        {
            phyParam.Value = CN470_DEFAULT_DATARATE;
            break;
        }
        case PHY_NEXT_LOWER_TX_DR:
        {
            RegionCommonGetNextLowerTxDrParams_t nextLowerTxDrParams =
            {
                .CurrentDr = getPhy->Datarate,
                .MaxDr = ( int8_t )CN470_TX_MAX_DATARATE,
                .MinDr = ( int8_t )CN470_TX_MIN_DATARATE,
                .NbChannels = CN470_MAX_NB_CHANNELS,
                .ChannelsMask = RegionNvmGroup2->ChannelsMask,
                .Channels = RegionNvmGroup2->Channels,
            };
            phyParam.Value = RegionCommonGetNextLowerTxDr( &nextLowerTxDrParams );
            break;
        }
        case PHY_MAX_TX_POWER:
        {
            phyParam.Value = CN470_MAX_TX_POWER;
            break;
        }
        case PHY_DEF_TX_POWER:
        {
            phyParam.Value = CN470_DEFAULT_TX_POWER;
            break;
        }
        case PHY_DEF_ADR_ACK_LIMIT:
        {
            phyParam.Value = REGION_COMMON_DEFAULT_ADR_ACK_LIMIT;
            break;
        }
        case PHY_DEF_ADR_ACK_DELAY:
        {
            phyParam.Value = REGION_COMMON_DEFAULT_ADR_ACK_DELAY;
            break;
        }
        case PHY_MAX_PAYLOAD:
        {
            phyParam.Value = MaxPayloadOfDatarateCN470[getPhy->Datarate];
            break;
        }
        case PHY_MAX_PAYLOAD_REPEATER:
        {
            phyParam.Value = MaxPayloadOfDatarateRepeaterCN470[getPhy->Datarate];
            break;
        }
        case PHY_DUTY_CYCLE:
        {
            phyParam.Value = CN470_DUTY_CYCLE_ENABLED;
            break;
        }
        case PHY_MAX_RX_WINDOW:
        {
            phyParam.Value = CN470_MAX_RX_WINDOW;
            break;
        }
        case PHY_RECEIVE_DELAY1:
        {
            phyParam.Value = REGION_COMMON_DEFAULT_RECEIVE_DELAY1;
            break;
        }
        case PHY_RECEIVE_DELAY2:
        {
            phyParam.Value = REGION_COMMON_DEFAULT_RECEIVE_DELAY2;
            break;
        }
        case PHY_JOIN_ACCEPT_DELAY1:
        {
            phyParam.Value = REGION_COMMON_DEFAULT_JOIN_ACCEPT_DELAY1;
            break;
        }
        case PHY_JOIN_ACCEPT_DELAY2:
        {
            phyParam.Value = REGION_COMMON_DEFAULT_JOIN_ACCEPT_DELAY2;
            break;
        }
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
        case PHY_MAX_FCNT_GAP:
        {
            phyParam.Value = REGION_COMMON_DEFAULT_MAX_FCNT_GAP;
            break;
        }
        case PHY_ACK_TIMEOUT:
        {
            phyParam.Value = ( REGION_COMMON_DEFAULT_ACK_TIMEOUT + randr( -REGION_COMMON_DEFAULT_ACK_TIMEOUT_RND, REGION_COMMON_DEFAULT_ACK_TIMEOUT_RND ) );
            break;
        }
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
        case PHY_RETRANSMIT_TIMEOUT:
        {
            phyParam.Value = ( REGION_COMMON_DEFAULT_RETRANSMIT_TIMEOUT + randr( -REGION_COMMON_DEFAULT_RETRANSMIT_TIMEOUT_RND, REGION_COMMON_DEFAULT_RETRANSMIT_TIMEOUT_RND ) );
            break;
        }
#endif /* REGION_VERSION */
        case PHY_DEF_DR1_OFFSET:
        {
            phyParam.Value = REGION_COMMON_DEFAULT_RX1_DR_OFFSET;
            break;
        }
        case PHY_DEF_RX2_FREQUENCY:
        {
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
            phyParam.Value = CN470_RX_WND_2_FREQ;
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
            phyParam.Value = REGION_CN470_DEFAULT_RX_WND_2_FREQ;

            if( RegionNvmGroup2->ChannelPlan != CHANNEL_PLAN_UNKNOWN )
            {
                phyParam.Value = ChannelPlanCtx.GetRx2Frequency( RegionNvmGroup2->CommonJoinChannelIndex, RegionNvmGroup2->IsOtaaDevice );
            }
#endif /* REGION_VERSION */
            break;
        }
        case PHY_DEF_RX2_DR:
        {
            phyParam.Value = CN470_RX_WND_2_DR;
            break;
        }
        case PHY_CHANNELS_MASK:
        {
            phyParam.ChannelsMask = RegionNvmGroup2->ChannelsMask;
            break;
        }
        case PHY_CHANNELS_DEFAULT_MASK:
        {
            phyParam.ChannelsMask = RegionNvmGroup2->ChannelsDefaultMask;
            break;
        }
        case PHY_MAX_NB_CHANNELS:
        {
            phyParam.Value = CN470_MAX_NB_CHANNELS;
            break;
        }
        case PHY_CHANNELS:
        {
            phyParam.Channels = RegionNvmGroup2->Channels;
            break;
        }
        case PHY_DEF_UPLINK_DWELL_TIME:
        {
            phyParam.Value = CN470_DEFAULT_UPLINK_DWELL_TIME;
            break;
        }
        case PHY_DEF_DOWNLINK_DWELL_TIME:
        {
            phyParam.Value = REGION_COMMON_DEFAULT_DOWNLINK_DWELL_TIME;
            break;
        }
        case PHY_DEF_MAX_EIRP:
        {
            phyParam.fValue = CN470_DEFAULT_MAX_EIRP;
            break;
        }
        case PHY_DEF_ANTENNA_GAIN:
        {
            phyParam.fValue = CN470_DEFAULT_ANTENNA_GAIN;
            break;
        }
        case PHY_BEACON_CHANNEL_FREQ:
        {
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
            phyParam.Value = RegionBaseUSCalcDownlinkFrequency( getPhy->Channel,
                                                                CN470_BEACON_CHANNEL_FREQ,
                                                                CN470_BEACON_CHANNEL_STEPWIDTH );
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
            phyParam.Value = REGION_CN470_DEFAULT_RX_WND_2_FREQ;

            // Implementation depending on the join channel
            if( RegionNvmGroup2->ChannelPlan != CHANNEL_PLAN_UNKNOWN )
            {
                phyParam.Value = ChannelPlanCtx.GetDownlinkFrequency( getPhy->Channel,
                                                                      RegionNvmGroup2->CommonJoinChannelIndex,
                                                                      false );
            }
#endif /* REGION_VERSION */
            break;
        }
        case PHY_BEACON_FORMAT:
        {
            phyParam.BeaconFormat.BeaconSize = CN470_BEACON_SIZE;
            phyParam.BeaconFormat.Rfu1Size = CN470_RFU1_SIZE;
            phyParam.BeaconFormat.Rfu2Size = CN470_RFU2_SIZE;
            break;
        }
        case PHY_BEACON_CHANNEL_DR:
        {
            phyParam.Value = CN470_BEACON_CHANNEL_DR;
            break;
        }
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
        case PHY_BEACON_NB_CHANNELS:
        {
            phyParam.Value = CN470_BEACON_NB_CHANNELS;
            break;
        }
        case PHY_PING_SLOT_CHANNEL_FREQ:
        {
            phyParam.Value = RegionBaseUSCalcDownlinkFrequency( getPhy->Channel,
                                                                CN470_PING_SLOT_CHANNEL_FREQ,
                                                                CN470_BEACON_CHANNEL_STEPWIDTH );
            break;
        }
        case PHY_PING_SLOT_CHANNEL_DR:
        {
            phyParam.Value = CN470_PING_SLOT_CHANNEL_DR;
            break;
        }
        case PHY_PING_SLOT_NB_CHANNELS:
        {
            phyParam.Value = CN470_BEACON_NB_CHANNELS;
            break;
        }
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
        case PHY_BEACON_NB_CHANNELS:
        {
            // Implementation depending on the join channel
            if( RegionNvmGroup2->ChannelPlan != CHANNEL_PLAN_UNKNOWN )
            {
                phyParam.Value = ChannelPlanCtx.NbBeaconChannels;
            }
            break;
        }

        case PHY_BEACON_CHANNEL_OFFSET:
        {
            // Implementation depending on the join channel
            if( RegionNvmGroup2->ChannelPlan != CHANNEL_PLAN_UNKNOWN )
            {
                phyParam.Value = ChannelPlanCtx.GetBeaconChannelOffset( RegionNvmGroup2->CommonJoinChannelIndex );
            }
            break;
        }
        case PHY_PING_SLOT_CHANNEL_FREQ:
        {
            phyParam.Value = REGION_CN470_DEFAULT_RX_WND_2_FREQ;

            // Implementation depending on the join channel
            if( RegionNvmGroup2->ChannelPlan != CHANNEL_PLAN_UNKNOWN )
            {
                phyParam.Value = ChannelPlanCtx.GetDownlinkFrequency( getPhy->Channel,
                                                                      RegionNvmGroup2->CommonJoinChannelIndex,
                                                                      true );
            }
            break;
        }
        case PHY_PING_SLOT_CHANNEL_DR:
        {
            phyParam.Value = CN470_PING_SLOT_CHANNEL_DR;
            break;
        }
        case PHY_PING_SLOT_NB_CHANNELS:
        {
            // Implementation depending on the join channel
            if( RegionNvmGroup2->ChannelPlan != CHANNEL_PLAN_UNKNOWN )
            {
                phyParam.Value = ChannelPlanCtx.NbPingSlotChannels;
            }
            break;
        }
#endif /* REGION_VERSION */
        case PHY_SF_FROM_DR:
        {
            phyParam.Value = DataratesCN470[getPhy->Datarate];
            break;
        }
        case PHY_BW_FROM_DR:
        {
            phyParam.Value = RegionCommonGetBandwidth( getPhy->Datarate, BandwidthsCN470 );
            break;
        }
        default:
        {
            break;
        }
    }

#endif /* REGION_CN470 */
    return phyParam;
}

void RegionCN470SetBandTxDone( SetBandTxDoneParams_t* txDone )
{
#if defined( REGION_CN470 )
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
    RegionCommonSetBandTxDone( &RegionNvmGroup1->Bands[RegionNvmGroup2->Channels[txDone->Channel].Band],
                               txDone->LastTxAirTime, txDone->Joined, txDone->ElapsedTimeSinceStartUp );
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
    RegionCommonSetBandTxDone( &RegionBands[RegionNvmGroup2->Channels[txDone->Channel].Band],
                               txDone->LastTxAirTime, txDone->Joined, txDone->ElapsedTimeSinceStartUp );
#endif /* REGION_VERSION */
#endif /* REGION_CN470 */
}

void RegionCN470InitDefaults( InitDefaultsParams_t* params )
{
#if defined( REGION_CN470 )
    Band_t bands[CN470_MAX_NB_BANDS] =
    {
        CN470_BAND0
    };

    switch( params->Type )
    {
        case INIT_TYPE_DEFAULTS:
        {
            if( ( params->NvmGroup1 == NULL ) || ( params->NvmGroup2 == NULL ) )
            {
                return;
            }

            RegionNvmGroup1 = (RegionNvmDataGroup1_t*) params->NvmGroup1;
            RegionNvmGroup2 = (RegionNvmDataGroup2_t*) params->NvmGroup2;
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))

            // Default bands
            memcpy1( ( uint8_t* )RegionNvmGroup1->Bands, ( uint8_t* )bands, sizeof( Band_t ) * CN470_MAX_NB_BANDS );

            // Default channels
            for( uint8_t i = 0; i < CN470_MAX_NB_CHANNELS; i++ )
            {
                // 125 kHz channels
                RegionNvmGroup2->Channels[i].Frequency = 470300000 + i * 200000;
                RegionNvmGroup2->Channels[i].DrRange.Value = ( DR_5 << 4 ) | DR_0;
                RegionNvmGroup2->Channels[i].Band = 0;
            }

            // Default ChannelsMask
#if ( HYBRID_ENABLED == 1 )
            RegionNvmGroup2->ChannelsDefaultMask[0] = HYBRID_DEFAULT_MASK0;
            RegionNvmGroup2->ChannelsDefaultMask[1] = HYBRID_DEFAULT_MASK1;
            RegionNvmGroup2->ChannelsDefaultMask[2] = HYBRID_DEFAULT_MASK2;
            RegionNvmGroup2->ChannelsDefaultMask[3] = HYBRID_DEFAULT_MASK3;
            RegionNvmGroup2->ChannelsDefaultMask[4] = HYBRID_DEFAULT_MASK4;
            RegionNvmGroup2->ChannelsDefaultMask[5] = HYBRID_DEFAULT_MASK5;
#else
            RegionNvmGroup2->ChannelsDefaultMask[0] = 0xFFFF;
            RegionNvmGroup2->ChannelsDefaultMask[1] = 0xFFFF;
            RegionNvmGroup2->ChannelsDefaultMask[2] = 0xFFFF;
            RegionNvmGroup2->ChannelsDefaultMask[3] = 0xFFFF;
            RegionNvmGroup2->ChannelsDefaultMask[4] = 0xFFFF;
            RegionNvmGroup2->ChannelsDefaultMask[5] = 0xFFFF;
#endif /* HYBRID_ENABLED == 1 */

            // Copy channels default mask
            RegionCommonChanMaskCopy( RegionNvmGroup2->ChannelsMask, RegionNvmGroup2->ChannelsDefaultMask, CHANNELS_MASK_SIZE );
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
            RegionBands = (Band_t*) params->Bands;

            // Default bands
            memcpy1( ( uint8_t* )RegionBands, ( uint8_t* )bands, sizeof( Band_t ) * CN470_MAX_NB_BANDS );

            // 125 kHz channels
            RegionNvmGroup2->ChannelPlan = REGION_CN470_DEFAULT_CHANNEL_PLAN;
            RegionNvmGroup2->CommonJoinChannelIndex = 0;
            RegionNvmGroup2->IsOtaaDevice = false;

            // Apply the channel plan configuration
            ApplyChannelPlanConfig( RegionNvmGroup2->ChannelPlan, &ChannelPlanCtx );

            // Default channels
            ChannelPlanCtx.InitializeChannels( RegionNvmGroup2->Channels );

            // Default ChannelsMask
            ChannelPlanCtx.InitializeChannelsMask( RegionNvmGroup2->ChannelsDefaultMask );

            // Copy channels default mask
            RegionCommonChanMaskCopy( RegionNvmGroup2->ChannelsMask, RegionNvmGroup2->ChannelsDefaultMask, CHANNELS_MASK_SIZE );

            // Copy into channels mask remaining
            RegionCommonChanMaskCopy( RegionNvmGroup1->ChannelsMaskRemaining, RegionNvmGroup2->ChannelsMask, CHANNELS_MASK_SIZE );
#endif /* REGION_VERSION */
            break;
        }
        case INIT_TYPE_RESET_TO_DEFAULT_CHANNELS:
        {
            // Intentional fallthrough
        }
        case INIT_TYPE_ACTIVATE_DEFAULT_CHANNELS:
        {
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
            // Copy channels default mask
            RegionCommonChanMaskCopy( RegionNvmGroup2->ChannelsMask, RegionNvmGroup2->ChannelsDefaultMask, CHANNELS_MASK_SIZE );
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
            // Restore channels default mask
            RegionCommonChanMaskCopy( RegionNvmGroup2->ChannelsMask, RegionNvmGroup2->ChannelsDefaultMask, CHANNELS_MASK_SIZE );

            for( uint8_t i = 0; i < CHANNELS_MASK_SIZE; i++ )
            { // Copy-And the channels mask
                RegionNvmGroup1->ChannelsMaskRemaining[i] &= RegionNvmGroup2->ChannelsMask[i];
            }
#endif /* REGION_VERSION */
            break;
        }
        default:
        {
            break;
        }
    }
#endif /* REGION_CN470 */
}

bool RegionCN470Verify( VerifyParams_t* verify, PhyAttribute_t phyAttribute )
{
#if defined( REGION_CN470 )
    switch( phyAttribute )
    {
        case PHY_FREQUENCY:
        {
            return VerifyRfFreq( verify->Frequency );
        }
        case PHY_TX_DR:
        case PHY_DEF_TX_DR:
        {
            return RegionCommonValueInRange( verify->DatarateParams.Datarate, CN470_TX_MIN_DATARATE, CN470_TX_MAX_DATARATE );
        }
        case PHY_RX_DR:
        {
            return RegionCommonValueInRange( verify->DatarateParams.Datarate, CN470_RX_MIN_DATARATE, CN470_RX_MAX_DATARATE );
        }
        case PHY_DEF_TX_POWER:
        case PHY_TX_POWER:
        {
            // Remark: switched min and max!
            return RegionCommonValueInRange( verify->TxPower, CN470_MAX_TX_POWER, CN470_MIN_TX_POWER );
        }
        case PHY_DUTY_CYCLE:
        {
            return CN470_DUTY_CYCLE_ENABLED;
        }
        default:
            return false;
    }
#else
    return false;
#endif /* REGION_CN470 */
}

void RegionCN470ApplyCFList( ApplyCFListParams_t* applyCFList )
{
#if defined( REGION_CN470 )
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
    // Setup the channel plan based on the join channel
    RegionNvmGroup2->CommonJoinChannelIndex = applyCFList->JoinChannel;
    RegionNvmGroup2->IsOtaaDevice = true;
    RegionNvmGroup2->ChannelPlan = IdentifyChannelPlan( RegionNvmGroup2->CommonJoinChannelIndex );

    if( RegionNvmGroup2->ChannelPlan == CHANNEL_PLAN_UNKNOWN )
    {
        // Invalid channel plan, fallback to default
        RegionNvmGroup2->ChannelPlan = REGION_CN470_DEFAULT_CHANNEL_PLAN;
    }
    // Apply the configuration for the channel plan
    ApplyChannelPlanConfig( RegionNvmGroup2->ChannelPlan, &ChannelPlanCtx );
#endif /* REGION_VERSION */

    // Size of the optional CF list must be 16 byte
    if( applyCFList->Size != 16 )
    {
        return;
    }

    // Last byte CFListType must be 0x01 to indicate the CFList contains a series of ChMask fields
    if( applyCFList->Payload[15] != 0x01 )
    {
        return;
    }

    // ChMask0 - ChMask5 must be set (every ChMask has 16 bit)
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
    for( uint8_t chMaskItr = 0, cntPayload = 0; chMaskItr <= 5; chMaskItr++, cntPayload+=2 )
    {
        RegionNvmGroup2->ChannelsMask[chMaskItr] = (uint16_t) (0x00FF & applyCFList->Payload[cntPayload]);
        RegionNvmGroup2->ChannelsMask[chMaskItr] |= (uint16_t) (applyCFList->Payload[cntPayload+1] << 8);
    }
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
    for( uint8_t chMaskItr = 0, cntPayload = 0; chMaskItr < ChannelPlanCtx.JoinAcceptListSize; chMaskItr++, cntPayload+=2 )
    {
        RegionNvmGroup2->ChannelsMask[chMaskItr] = (uint16_t) (0x00FF & applyCFList->Payload[cntPayload]);
        RegionNvmGroup2->ChannelsMask[chMaskItr] |= (uint16_t) (applyCFList->Payload[cntPayload+1] << 8);

        // Set the channel mask to the remaining
        RegionNvmGroup1->ChannelsMaskRemaining[chMaskItr] &= RegionNvmGroup2->ChannelsMask[chMaskItr];
    }
#endif /* REGION_VERSION */
#endif /* REGION_CN470 */
}

bool RegionCN470ChanMaskSet( ChanMaskSetParams_t* chanMaskSet )
{
#if defined( REGION_CN470 )
    switch( chanMaskSet->ChannelsMaskType )
    {
        case CHANNELS_MASK:
        {
            RegionCommonChanMaskCopy( RegionNvmGroup2->ChannelsMask, chanMaskSet->ChannelsMaskIn, CHANNELS_MASK_SIZE );

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
            for( uint8_t i = 0; i < CHANNELS_MASK_SIZE; i++ )
            { // Copy-And the channels mask
                RegionNvmGroup1->ChannelsMaskRemaining[i] &= RegionNvmGroup2->ChannelsMask[i];
            }
#endif /* REGION_VERSION */
            break;
        }
        case CHANNELS_DEFAULT_MASK:
        {
            RegionCommonChanMaskCopy( RegionNvmGroup2->ChannelsDefaultMask, chanMaskSet->ChannelsMaskIn, CHANNELS_MASK_SIZE );
            break;
        }
        default:
            return false;
    }
    return true;
#else
    return false;
#endif /* REGION_CN470 */
}

void RegionCN470ComputeRxWindowParameters( int8_t datarate, uint8_t minRxSymbols, uint32_t rxError, RxConfigParams_t *rxConfigParams )
{
#if defined( REGION_CN470 )
    uint32_t tSymbolInUs = 0;

    // Get the datarate, perform a boundary check
    rxConfigParams->Datarate = MIN( datarate, CN470_RX_MAX_DATARATE );
    rxConfigParams->Bandwidth = RegionCommonGetBandwidth( rxConfigParams->Datarate, BandwidthsCN470 );

    tSymbolInUs = RegionCommonComputeSymbolTimeLoRa( DataratesCN470[rxConfigParams->Datarate], BandwidthsCN470[rxConfigParams->Datarate] );

    RegionCommonComputeRxWindowParameters( tSymbolInUs, minRxSymbols, rxError, Radio.GetWakeupTime( ), &rxConfigParams->WindowTimeout, &rxConfigParams->WindowOffset );
#endif /* REGION_CN470 */
}

bool RegionCN470RxConfig( RxConfigParams_t* rxConfig, int8_t* datarate )
{
#if defined( REGION_CN470 )
    int8_t dr = rxConfig->Datarate;
    uint8_t maxPayload = 0;
    int8_t phyDr = 0;
    uint32_t frequency = rxConfig->Frequency;

    if( Radio.GetStatus( ) != RF_IDLE )
    {
        return false;
    }

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
    if( rxConfig->RxSlot == RX_SLOT_WIN_1 )
    {
        // Apply window 1 frequency
        frequency = CN470_FIRST_RX1_CHANNEL + ( rxConfig->Channel % 48 ) * CN470_STEPWIDTH_RX1_CHANNEL;
    }
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
    // The RX configuration depends on whether the device has joined or not.
    if( rxConfig->NetworkActivation != ACTIVATION_TYPE_NONE )
    {
        // Update the downlink frequency in case of RX_SLOT_WIN_1 or RX_SLOT_WIN_2.
        // Keep the frequency for all other cases.
        if( rxConfig->RxSlot == RX_SLOT_WIN_1 )
        {
            // Apply window 1 frequency
            frequency = ChannelPlanCtx.GetRx1Frequency( rxConfig->Channel );
        }
        else if( rxConfig->RxSlot == RX_SLOT_WIN_2 )
        {
            // Apply window 2 frequency
            frequency = ChannelPlanCtx.GetRx2Frequency( RegionNvmGroup2->CommonJoinChannelIndex, RegionNvmGroup2->IsOtaaDevice );
        }
    }
    else
    {
        // In this case, only RX_SLOT_WIN_1 and RX_SLOT_WIN_2 is possible. There is
        // no need to verify it. The end device is not joined and is an OTAA device.
        frequency = CommonJoinChannels[rxConfig->Channel].Rx1Frequency;
    }
#endif /* REGION_VERSION */

    // Read the physical datarate from the datarates table
    phyDr = DataratesCN470[dr];

    Radio.SetChannel( frequency );

    // Radio configuration
    Radio.SetRxConfig( MODEM_LORA, rxConfig->Bandwidth, phyDr, 1, 0, 8, rxConfig->WindowTimeout, false, 0, false, 0, 0, true, rxConfig->RxContinuous );
    if( rxConfig->RepeaterSupport == true )
    {
        maxPayload = MaxPayloadOfDatarateRepeaterCN470[dr];
    }
    else
    {
        maxPayload = MaxPayloadOfDatarateCN470[dr];
    }
    Radio.SetMaxPayloadLength( MODEM_LORA, maxPayload + LORAMAC_FRAME_PAYLOAD_OVERHEAD_SIZE );

    RegionCommonRxConfigPrint(rxConfig->RxSlot, frequency, dr);

    *datarate = (uint8_t) dr;
    return true;
#else
    return false;
#endif /* REGION_CN470 */
}

bool RegionCN470TxConfig( TxConfigParams_t* txConfig, int8_t* txPower, TimerTime_t* txTimeOnAir )
{
#if defined( REGION_CN470 )
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
    int8_t phyDr = DataratesCN470[txConfig->Datarate];
    int8_t txPowerLimited = RegionCommonLimitTxPower( txConfig->TxPower, RegionNvmGroup1->Bands[RegionNvmGroup2->Channels[txConfig->Channel].Band].TxMaxPower );
    uint32_t bandwidth = RegionCommonGetBandwidth( txConfig->Datarate, BandwidthsCN470 );
    int8_t phyTxPower = 0;

    // Calculate physical TX power
    phyTxPower = RegionCommonComputeTxPower( txPowerLimited, txConfig->MaxEirp, txConfig->AntennaGain );

    // Setup the radio frequency
    Radio.SetChannel( RegionNvmGroup2->Channels[txConfig->Channel].Frequency );

    Radio.SetTxConfig( MODEM_LORA, phyTxPower, 0, bandwidth, phyDr, 1, 8, false, true, 0, 0, false, 4000 );
    RegionCommonTxConfigPrint(RegionNvmGroup2->Channels[txConfig->Channel].Frequency, txConfig->Datarate);

    // Setup maximum payload length of the radio driver
    Radio.SetMaxPayloadLength( MODEM_LORA, txConfig->PktLen );
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
    RadioModems_t modem;
    uint32_t frequency;
    uint8_t band;
    int8_t phyDr = DataratesCN470[txConfig->Datarate];

    // The TX configuration depends on whether the device has joined or not.
    if( txConfig->NetworkActivation != ACTIVATION_TYPE_NONE )
    {
        frequency = RegionNvmGroup2->Channels[txConfig->Channel].Frequency;
        band = RegionNvmGroup2->Channels[txConfig->Channel].Band;
    }
    else
    {
        // The end device is not joined and is an OTAA device.
        frequency = CommonJoinChannels[txConfig->Channel].Frequency;
        band = CommonJoinChannels[txConfig->Channel].Band;
    }

    int8_t txPowerLimited = RegionCommonLimitTxPower( txConfig->TxPower, RegionBands[band].TxMaxPower );
    uint32_t bandwidth = RegionCommonGetBandwidth( txConfig->Datarate, BandwidthsCN470 );
    int8_t phyTxPower = 0;

    // Calculate physical TX power
    phyTxPower = RegionCommonComputeTxPower( txPowerLimited, txConfig->MaxEirp, txConfig->AntennaGain );

    // Setup the radio frequency
    Radio.SetChannel( frequency );

    if( txConfig->Datarate == DR_7 )
    { // High Speed FSK channel
        modem = MODEM_FSK;
        Radio.SetTxConfig( modem, phyTxPower, 25000, bandwidth, phyDr * 1000, 0, 5, false, true, 0, 0, false, 4000 );
    }
    else
    {
        modem = MODEM_LORA;
        Radio.SetTxConfig( modem, phyTxPower, 0, bandwidth, phyDr, 1, 8, false, true, 0, 0, false, 4000 );
    }
    RegionCommonTxConfigPrint(frequency, txConfig->Datarate);

    // Setup maximum payload length of the radio driver
    Radio.SetMaxPayloadLength( modem, txConfig->PktLen );
#endif /* REGION_VERSION */
    // Update time-on-air
    *txTimeOnAir = GetTimeOnAir( txConfig->Datarate, txConfig->PktLen );

    *txPower = txPowerLimited;
    return true;
#else
    return false;
#endif /* REGION_CN470 */
}

uint8_t RegionCN470LinkAdrReq( LinkAdrReqParams_t* linkAdrReq, int8_t* drOut, int8_t* txPowOut, uint8_t* nbRepOut, uint8_t* nbBytesParsed )
{
    uint8_t status = 0x07;
#if defined( REGION_CN470 )
    RegionCommonLinkAdrParams_t linkAdrParams = { 0 };
    uint8_t nextIndex = 0;
    uint8_t bytesProcessed = 0;
    uint16_t channelsMask[CHANNELS_MASK_SIZE] = { 0, 0, 0, 0, 0, 0 };
    GetPhyParams_t getPhy;
    PhyParam_t phyParam;
    RegionCommonLinkAdrReqVerifyParams_t linkAdrVerifyParams;

    // Initialize local copy of channels mask
    RegionCommonChanMaskCopy( channelsMask, RegionNvmGroup2->ChannelsMask, CHANNELS_MASK_SIZE );

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
    while( bytesProcessed < linkAdrReq->PayloadSize )
    {
        // Get ADR request parameters
        nextIndex = RegionCommonParseLinkAdrReq( &( linkAdrReq->Payload[bytesProcessed] ), &linkAdrParams );

        if( nextIndex == 0 )
            break; // break loop, since no more request has been found

        // Update bytes processed
        bytesProcessed += nextIndex;

        // Revert status, as we only check the last ADR request for the channel mask KO
        status = 0x07;

        if( linkAdrParams.ChMaskCtrl == 6 )
        {
            // Enable all 125 kHz channels
            channelsMask[0] = 0xFFFF;
            channelsMask[1] = 0xFFFF;
            channelsMask[2] = 0xFFFF;
            channelsMask[3] = 0xFFFF;
            channelsMask[4] = 0xFFFF;
            channelsMask[5] = 0xFFFF;
        }
        else if( linkAdrParams.ChMaskCtrl == 7 )
        {
            status &= 0xFE; // Channel mask KO
        }
        else
        {
            for( uint8_t i = 0; i < 16; i++ )
            {
                if( ( ( linkAdrParams.ChMask & ( 1 << i ) ) != 0 ) &&
                    ( RegionNvmGroup2->Channels[linkAdrParams.ChMaskCtrl * 16 + i].Frequency == 0 ) )
                {// Trying to enable an undefined channel
                    status &= 0xFE; // Channel mask KO
                }
            }
            channelsMask[linkAdrParams.ChMaskCtrl] = linkAdrParams.ChMask;
        }
    }
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
    while( bytesProcessed < linkAdrReq->PayloadSize )
    {
        // Get ADR request parameters
        nextIndex = RegionCommonParseLinkAdrReq( &( linkAdrReq->Payload[bytesProcessed] ), &linkAdrParams );

        if( nextIndex == 0 )
            break; // break loop, since no more request has been found

        // Update bytes processed
        bytesProcessed += nextIndex;

        // Update the channel plan
        status = ChannelPlanCtx.LinkAdrChMaskUpdate( channelsMask, linkAdrParams.ChMaskCtrl,
                                                     linkAdrParams.ChMask, RegionNvmGroup2->Channels );
    }

    // Make sure at least one channel is active
    if( RegionCommonCountChannels( channelsMask, 0, ChannelPlanCtx.ChannelsMaskSize ) == 0 )
    {
        status &= 0xFE; // Channel mask KO
    }
#endif /* REGION_VERSION */

    // Get the minimum possible datarate
    getPhy.Attribute = PHY_MIN_TX_DR;
    getPhy.UplinkDwellTime = linkAdrReq->UplinkDwellTime;
    phyParam = RegionCN470GetPhyParam( &getPhy );

    linkAdrVerifyParams.Status = status;
    linkAdrVerifyParams.AdrEnabled = linkAdrReq->AdrEnabled;
    linkAdrVerifyParams.Datarate = linkAdrParams.Datarate;
    linkAdrVerifyParams.TxPower = linkAdrParams.TxPower;
    linkAdrVerifyParams.NbRep = linkAdrParams.NbRep;
    linkAdrVerifyParams.CurrentDatarate = linkAdrReq->CurrentDatarate;
    linkAdrVerifyParams.CurrentTxPower = linkAdrReq->CurrentTxPower;
    linkAdrVerifyParams.CurrentNbRep = linkAdrReq->CurrentNbRep;
    linkAdrVerifyParams.NbChannels = CN470_MAX_NB_CHANNELS;
    linkAdrVerifyParams.ChannelsMask = channelsMask;
    linkAdrVerifyParams.MinDatarate = ( int8_t )phyParam.Value;
    linkAdrVerifyParams.MaxDatarate = CN470_TX_MAX_DATARATE;
    linkAdrVerifyParams.Channels = RegionNvmGroup2->Channels;
    linkAdrVerifyParams.MinTxPower = CN470_MIN_TX_POWER;
    linkAdrVerifyParams.MaxTxPower = CN470_MAX_TX_POWER;
    linkAdrVerifyParams.Version = linkAdrReq->Version;

    // Verify the parameters and update, if necessary
    status = RegionCommonLinkAdrReqVerifyParams( &linkAdrVerifyParams, &linkAdrParams.Datarate, &linkAdrParams.TxPower, &linkAdrParams.NbRep );

    // Update channelsMask if everything is correct
    if( status == 0x07 )
    {
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
        // Copy Mask
        RegionCommonChanMaskCopy( RegionNvmGroup2->ChannelsMask, channelsMask, 6 );
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
        // Copy Mask
        RegionCommonChanMaskCopy( RegionNvmGroup2->ChannelsMask, channelsMask, CHANNELS_MASK_SIZE );

        RegionNvmGroup1->ChannelsMaskRemaining[0] &= RegionNvmGroup2->ChannelsMask[0];
        RegionNvmGroup1->ChannelsMaskRemaining[1] &= RegionNvmGroup2->ChannelsMask[1];
        RegionNvmGroup1->ChannelsMaskRemaining[2] &= RegionNvmGroup2->ChannelsMask[2];
        RegionNvmGroup1->ChannelsMaskRemaining[3] &= RegionNvmGroup2->ChannelsMask[3];
        RegionNvmGroup1->ChannelsMaskRemaining[4] = RegionNvmGroup2->ChannelsMask[4];
        RegionNvmGroup1->ChannelsMaskRemaining[5] = RegionNvmGroup2->ChannelsMask[5];
#endif /* REGION_VERSION */
    }

    // Update status variables
    *drOut = linkAdrParams.Datarate;
    *txPowOut = linkAdrParams.TxPower;
    *nbRepOut = linkAdrParams.NbRep;
    *nbBytesParsed = bytesProcessed;

#endif /* REGION_CN470 */
    return status;
}

uint8_t RegionCN470RxParamSetupReq( RxParamSetupReqParams_t* rxParamSetupReq )
{
    uint8_t status = 0x07;
#if defined( REGION_CN470 )

    // Verify radio frequency
    if( VerifyRfFreq( rxParamSetupReq->Frequency ) == false )
    {
        status &= 0xFE; // Channel frequency KO
    }

    // Verify datarate
    if( RegionCommonValueInRange( rxParamSetupReq->Datarate, CN470_RX_MIN_DATARATE, CN470_RX_MAX_DATARATE ) == false )
    {
        status &= 0xFD; // Datarate KO
    }

    // Verify datarate offset
    if( RegionCommonValueInRange( rxParamSetupReq->DrOffset, CN470_MIN_RX1_DR_OFFSET, CN470_MAX_RX1_DR_OFFSET ) == false )
    {
        status &= 0xFB; // Rx1DrOffset range KO
    }

#endif /* REGION_CN470 */
    return status;
}

int8_t RegionCN470NewChannelReq( NewChannelReqParams_t* newChannelReq )
{
    // Do not accept the request
    return -1;
}

int8_t RegionCN470TxParamSetupReq( TxParamSetupReqParams_t* txParamSetupReq )
{
    // Do not accept the request
    return -1;
}

int8_t RegionCN470DlChannelReq( DlChannelReqParams_t* dlChannelReq )
{
    // Do not accept the request
    return -1;
}

int8_t RegionCN470AlternateDr( int8_t currentDr, AlternateDrType_t type )
{
#if defined( REGION_CN470 )
    return MAX(CN470_TX_MIN_DATARATE, currentDr);
#else
    return -1;
#endif /* REGION_CN470 */
}

LoRaMacStatus_t RegionCN470NextChannel( NextChanParams_t* nextChanParams, uint8_t* channel, TimerTime_t* time, TimerTime_t* aggregatedTimeOff )
{
#if defined( REGION_CN470 )
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
    uint8_t nbEnabledChannels = 0;
    uint8_t nbRestrictedChannels = 0;
    uint8_t enabledChannels[CN470_MAX_NB_CHANNELS] = { 0 };
    RegionCommonIdentifyChannelsParam_t identifyChannelsParam;
    RegionCommonCountNbOfEnabledChannelsParams_t countChannelsParams;
    LoRaMacStatus_t status = LORAMAC_STATUS_NO_CHANNEL_FOUND;

    // Count 125kHz channels
    if( RegionCommonCountChannels( RegionNvmGroup2->ChannelsMask, 0, CHANNELS_MASK_SIZE ) == 0 )
    { // Reactivate default channels
        RegionNvmGroup2->ChannelsMask[0] = 0xFFFF;
        RegionNvmGroup2->ChannelsMask[1] = 0xFFFF;
        RegionNvmGroup2->ChannelsMask[2] = 0xFFFF;
        RegionNvmGroup2->ChannelsMask[3] = 0xFFFF;
        RegionNvmGroup2->ChannelsMask[4] = 0xFFFF;
        RegionNvmGroup2->ChannelsMask[5] = 0xFFFF;
    }

    // Search how many channels are enabled
    countChannelsParams.Joined = nextChanParams->Joined;
    countChannelsParams.Datarate = nextChanParams->Datarate;
    countChannelsParams.ChannelsMask = RegionNvmGroup2->ChannelsMask;
    countChannelsParams.Channels = RegionNvmGroup2->Channels;
    countChannelsParams.Bands = RegionNvmGroup1->Bands;
    countChannelsParams.MaxNbChannels = CN470_MAX_NB_CHANNELS;
    countChannelsParams.JoinChannels = NULL;
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
    uint8_t nbEnabledChannels = 0;
    uint8_t nbRestrictedChannels = 0;
    uint8_t enabledChannels[CN470_MAX_NB_CHANNELS] = { 0 };
    uint16_t joinChannelsMask[2] = CN470_JOIN_CHANNELS;
    RegionCommonIdentifyChannelsParam_t identifyChannelsParam;
    RegionCommonCountNbOfEnabledChannelsParams_t countChannelsParams;
    LoRaMacStatus_t status = LORAMAC_STATUS_NO_CHANNEL_FOUND;

    // Count 125kHz channels
    if( RegionCommonCountChannels( RegionNvmGroup1->ChannelsMaskRemaining, 0, ChannelPlanCtx.ChannelsMaskSize ) == 0 )
    { // Reactivate default channels
        RegionNvmGroup2->ChannelsMask[0] = 0xFFFF;
        RegionNvmGroup2->ChannelsMask[1] = 0xFFFF;
        RegionNvmGroup2->ChannelsMask[2] = 0xFFFF;
        RegionNvmGroup2->ChannelsMask[3] = 0xFFFF;
        RegionNvmGroup2->ChannelsMask[4] = 0xFFFF;
        RegionNvmGroup2->ChannelsMask[5] = 0xFFFF;
        RegionCommonChanMaskCopy( RegionNvmGroup1->ChannelsMaskRemaining, RegionNvmGroup2->ChannelsMask, ChannelPlanCtx.ChannelsMaskSize  );
    }

    // Search how many channels are enabled
    countChannelsParams.Joined = nextChanParams->Joined;
    countChannelsParams.Datarate = nextChanParams->Datarate;
    countChannelsParams.ChannelsMask = RegionNvmGroup1->ChannelsMaskRemaining;
    countChannelsParams.Channels = RegionNvmGroup2->Channels;
    countChannelsParams.Bands = RegionBands;
    countChannelsParams.MaxNbChannels = CN470_MAX_NB_CHANNELS;
    countChannelsParams.JoinChannels = NULL;

    // Apply a different channel selection if the device is not joined yet
    // In this case the device shall not follow the individual channel plans for the
    // different type, but instead shall follow the common join channel plan.
    if( countChannelsParams.Joined == false )
    {
        countChannelsParams.ChannelsMask = joinChannelsMask;
        countChannelsParams.Channels = CommonJoinChannels;
        countChannelsParams.MaxNbChannels = CN470_COMMON_JOIN_CHANNELS_SIZE;
        countChannelsParams.JoinChannels = joinChannelsMask;
    }
#endif /* REGION_VERSION */

    identifyChannelsParam.AggrTimeOff = nextChanParams->AggrTimeOff;
    identifyChannelsParam.LastAggrTx = nextChanParams->LastAggrTx;
    identifyChannelsParam.DutyCycleEnabled = nextChanParams->DutyCycleEnabled;
    identifyChannelsParam.MaxBands = CN470_MAX_NB_BANDS;

    identifyChannelsParam.ElapsedTimeSinceStartUp = nextChanParams->ElapsedTimeSinceStartUp;
    identifyChannelsParam.LastTxIsJoinRequest = nextChanParams->LastTxIsJoinRequest;
    identifyChannelsParam.ExpectedTimeOnAir = GetTimeOnAir( nextChanParams->Datarate, nextChanParams->PktLen );

    identifyChannelsParam.CountNbOfEnabledChannelsParam = &countChannelsParams;

    status = RegionCommonIdentifyChannels( &identifyChannelsParam, aggregatedTimeOff, enabledChannels,
                                           &nbEnabledChannels, &nbRestrictedChannels, time );

    if( status == LORAMAC_STATUS_OK )
    {
        // We found a valid channel. Selection is random.
        *channel = enabledChannels[randr( 0, nbEnabledChannels - 1 )];

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
        // Disable the channel in the mask
        RegionCommonChanDisable( RegionNvmGroup1->ChannelsMaskRemaining, *channel, ChannelPlanCtx.ChannelsMaskSize );
#endif /* REGION_VERSION */
    }
    return status;
#else
    return LORAMAC_STATUS_NO_CHANNEL_FOUND;
#endif /* REGION_CN470 */
}

LoRaMacStatus_t RegionCN470ChannelAdd( ChannelAddParams_t* channelAdd )
{
    return LORAMAC_STATUS_PARAMETER_INVALID;
}

bool RegionCN470ChannelsRemove( ChannelRemoveParams_t* channelRemove  )
{
    return LORAMAC_STATUS_PARAMETER_INVALID;
}

#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
void RegionCN470SetContinuousWave( ContinuousWaveParams_t* continuousWave )
{
#if defined( REGION_CN470 )
    int8_t txPowerLimited = RegionCommonLimitTxPower( continuousWave->TxPower, RegionNvmGroup1->Bands[RegionNvmGroup2->Channels[continuousWave->Channel].Band].TxMaxPower );
    int8_t phyTxPower = 0;
    uint32_t frequency = RegionNvmGroup2->Channels[continuousWave->Channel].Frequency;

    // Calculate physical TX power
    phyTxPower = RegionCommonComputeTxPower( txPowerLimited, continuousWave->MaxEirp, continuousWave->AntennaGain );

    Radio.SetTxContinuousWave( frequency, phyTxPower, continuousWave->Timeout );
#endif /* REGION_CN470 */
}
#endif /* REGION_VERSION */

uint8_t RegionCN470ApplyDrOffset( uint8_t downlinkDwellTime, int8_t dr, int8_t drOffset )
{
#if defined( REGION_CN470 )
#if (defined( REGION_VERSION ) && ( REGION_VERSION == 0x01010003 ))
    int8_t datarate = dr - drOffset;
#elif (defined( REGION_VERSION ) && ( REGION_VERSION == 0x02010001 ))
    int8_t datarate = DatarateOffsetsCN470[dr][drOffset];
#endif /* REGION_VERSION */
    if( datarate < 0 )
    {
        datarate = DR_0;
    }
    return datarate;
#else
    return 0;
#endif /* REGION_CN470 */
}

void RegionCN470RxBeaconSetup( RxBeaconSetup_t* rxBeaconSetup, uint8_t* outDr )
{
#if defined( REGION_CN470 )
    RegionCommonRxBeaconSetupParams_t regionCommonRxBeaconSetup;

    regionCommonRxBeaconSetup.Datarates = DataratesCN470;
    regionCommonRxBeaconSetup.Frequency = rxBeaconSetup->Frequency;
    regionCommonRxBeaconSetup.BeaconSize = CN470_BEACON_SIZE;
    regionCommonRxBeaconSetup.BeaconDatarate = CN470_BEACON_CHANNEL_DR;
    regionCommonRxBeaconSetup.BeaconChannelBW = CN470_BEACON_CHANNEL_BW;
    regionCommonRxBeaconSetup.RxTime = rxBeaconSetup->RxTime;
    regionCommonRxBeaconSetup.SymbolTimeout = rxBeaconSetup->SymbolTimeout;

    RegionCommonRxBeaconSetup( &regionCommonRxBeaconSetup );

    // Store downlink datarate
    *outDr = CN470_BEACON_CHANNEL_DR;
#endif /* REGION_CN470 */
}
