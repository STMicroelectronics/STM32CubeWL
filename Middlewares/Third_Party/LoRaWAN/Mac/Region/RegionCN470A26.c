/*!
 * \file      RegionCN470A26.c
 *
 * \brief     Specific Region implementation of CN470 Channel plan type A, 26MHz.
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
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \addtogroup  REGIONCN470
 *
 * \{
 */
#include "RegionCN470.h"
#include "RegionBaseUS.h"
#include "RegionCN470A26.h"

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

uint32_t RegionCN470A26GetDownlinkFrequency( uint8_t channel, uint8_t joinChannelIndex, bool isPingSlot )
{
    return CN470_A26_BEACON_FREQ;
}

uint8_t RegionCN470A26GetBeaconChannelOffset( uint8_t joinChannelIndex )
{
    return 0;
}

uint8_t RegionCN470A26LinkAdrChMaskUpdate( uint16_t* channelsMask, uint8_t chMaskCntl,
                                              uint16_t chanMask, ChannelParams_t* channels )
{
    uint8_t status = 0x07;

    if( ( chMaskCntl == 5 ) || ( chMaskCntl == 6 ) || ( chMaskCntl == 7 ) )
    {
        // RFU
        status &= 0xFE; // Channel mask KO
    }
    else if( chMaskCntl == 3 )
    {
        // Enable all channels
        channelsMask[0] = 0xFFFF;
        channelsMask[1] = 0xFFFF;
        channelsMask[2] = 0xFFFF;
        channelsMask[3] = 0xFFFF;
        channelsMask[4] = 0x0000;
        channelsMask[5] = 0x0000;
    }
    else if( chMaskCntl == 4 )
    {
        // Disable all channels
        channelsMask[0] = 0x0000;
        channelsMask[1] = 0x0000;
        channelsMask[2] = 0x0000;
        channelsMask[3] = 0x0000;
        channelsMask[4] = 0x0000;
        channelsMask[5] = 0x0000;
    }
    else
    {
        // chMaskCntl 0 to 2
        for( uint8_t i = 0; i < 16; i++ )
        {
            if( ( ( chanMask & ( 1 << i ) ) != 0 ) &&
                ( channels[chMaskCntl * 16 + i].Frequency == 0 ) )
            {// Trying to enable an undefined channel
                status &= 0xFE; // Channel mask KO
            }
        }
            channelsMask[chMaskCntl] = chanMask;
    }
    return status;
}

bool RegionCN470A26VerifyRfFreq( uint32_t freq )
{
    // Downstream group 1
    if( RegionBaseUSVerifyFrequencyGroup( freq, CN470_A26_FIRST_RX_CHANNEL,
                                          CN470_A26_LAST_RX_CHANNEL,
                                          CN470_A26_STEPWIDTH_RX_CHANNEL ) == false )
    {
        return false;
    }
    return true;
}

void RegionCN470A26InitializeChannels( ChannelParams_t* channels )
{
    // Upstream group 1
    for( uint8_t i = 0; i < 48; i++ )
    {
        channels[i].Frequency = CN470_A26_FIRST_TX_CHANNEL + i * CN470_A26_STEPWIDTH_RX_CHANNEL;
        channels[i].DrRange.Value = ( CN470_TX_MAX_DATARATE << 4 ) | CN470_TX_MIN_DATARATE;
        channels[i].Band = 0;
    }
}

void RegionCN470A26InitializeChannelsMask( uint16_t* channelsDefaultMask )
{
#if ( HYBRID_ENABLED == 1 )
    channelsDefaultMask[0] = HYBRID_DEFAULT_MASK0;
    channelsDefaultMask[1] = HYBRID_DEFAULT_MASK1;
    channelsDefaultMask[2] = HYBRID_DEFAULT_MASK2;
    channelsDefaultMask[3] = HYBRID_DEFAULT_MASK3;
    channelsDefaultMask[4] = HYBRID_DEFAULT_MASK4;
    channelsDefaultMask[5] = HYBRID_DEFAULT_MASK5;
#else
    // Enable all possible channels
    channelsDefaultMask[0] = 0xFFFF;
    channelsDefaultMask[1] = 0xFFFF;
    channelsDefaultMask[2] = 0xFFFF;
    channelsDefaultMask[3] = 0x0000;
    channelsDefaultMask[4] = 0x0000;
    channelsDefaultMask[5] = 0x0000;
#endif /* HYBRID_ENABLED == 1 */
}

uint32_t RegionCN470A26GetRx1Frequency( uint8_t channel )
{
    return ( CN470_A26_FIRST_RX_CHANNEL + ( ( channel % 24 ) * CN470_A26_STEPWIDTH_RX_CHANNEL ) );
}

uint32_t RegionCN470A26GetRx2Frequency( uint8_t joinChannelIndex, bool isOtaaDevice )
{
    return CN470_A26_RX_WND_2_FREQ;
}

/*! \} defgroup LORAMACCLASSB */
