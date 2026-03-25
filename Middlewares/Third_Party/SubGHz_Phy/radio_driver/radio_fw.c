/*!
 ******************************************************************************
 * @file    radio_fw.c
 * @author  MCD Application Team
 * @brief   extends the limited set of radio HW functionalities by FW algorithms
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2020(-2021) STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include "timer.h"
#include "radio_fw.h"
#include "radio_conf.h"
#include "radio_driver.h"
#include "mw_log_conf.h"

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/

/*!
 * Radio RFWPacket and global parameters
 * In RFWPacket mode, the packet is 'Fixed length' without Hw Crc check and without Hw (de-)whitening
 */
typedef struct
{
    uint8_t Enable;                    /* RFWPacket mode On: 1 or Off: 0*/
    uint8_t PayloadLengthFieldSize;    /* Size of the packet length Field*/
    uint8_t CrcEnable;                 /* enable crc calculation and check*/
    uint8_t CrcFieldSize;              /* size of the packet length Field*/
    uint16_t CrcPolynomial;            /* Init Crc polynomial, to set before running RFW_CrcRun*/
    uint16_t CrcSeed;                  /* Init Crc seed to set before running RFW_CrcRun*/
    RADIO_FSK_CrcTypes_t CrcType;      /* Init Crc types to set before running RFW_CrcRun*/
    uint16_t WhiteSeed;                /* Init whitening seed, to set before running Radio_FwWhiteRun*/
    uint16_t LongPacketMaxRxLength;    /* Maximum expected amount of bytes in payload*/
    RadioModems_t Modem;
    RadioEvents_t *RadioEvents;
} RFwInit_t;

typedef struct
{
    RFwInit_t Init;                    /* Init structure, set at Rx or Tx config*/
    uint16_t CrcLfsrState;             /* State of LFSR crc, set from CrcSeed at beginning of each payload*/
    uint16_t WhiteLfsrState;           /* State of LFSR whitening, set from WhiteSeed at beginning of each payload
                                          use to save LFSR state after rx payload length de-whitening*/
    uint16_t PayloadLength;            /* In Rx, Payload length is first byte(s) of the payload
                                          excluding CrcFieldSize and PayloadLengthFieldSize*/
    uint8_t LongPacketModeEnable;      /* set to one when RFW_TransmitLongPacket or RFW_ReceiveLongPacket. 0 otherwise*/
    TimerEvent_t Timer;                /* Timer to get/Set Rx(Tx)Bytes*/
    uint16_t LongPacketRemainingBytes; /* Count remaining bytes to send receive (including crc)*/
    uint8_t RadioBufferOffset;         /* Radio buffer offset*/
    uint16_t RxPayloadOffset;          /* RxPayloadOffset buffer offset*/
    void ( *RxLongPacketStoreChunkCb )( uint8_t *buffer, uint8_t buffer_size );
    void ( *TxLongPacketGetNextChunkCb )( uint8_t **buffer, uint8_t buffer_size );
    uint8_t AntSwitchPaSelect;
    uint32_t BitRate;
    TimerEvent_t *RxTimeoutTimer;
    TimerEvent_t *TxTimeoutTimer;
} RadioFw_t;

/* Private define ------------------------------------------------------------*/

#define RADIO_BUF_SIZE 255 /* bytes SubG IP internal Buffer */

#define LONGPACKET_CHUNK_LENGTH_BYTES ((int32_t) 128) /* bytes (half Radio fifo) */

/* Private macro -------------------------------------------------------------*/
/*!
 * @brief Calculates ceiling division of ( X / N )
 *
 * @param [in] X numerator
 * @param [in] N denominator
 *
 */
#ifndef DIVC
#define DIVC( X, N )                ( ( ( X ) + ( N ) - 1 ) / ( N ) )
#endif /* DIVC */

/*!
 * @brief Calculates rounding division of ( X / N )
 *
 * @param [in] X numerator
 * @param [in] N denominator
 *
 */
#ifndef DIVR
#define DIVR( X, N )                ( ( ( X ) + ( ((X)>0?(N):(N))>>1 ) ) / ( N ) )
#endif /* DIVR */

/*can be overridden in radio_conf.h*/
#ifndef RFW_IT_INIT
#define RFW_IT_INIT()
#endif /* RFW_IT_INIT */

#ifndef RFW_GET_PAYLOAD_PROCESS
#define RFW_GET_PAYLOAD_PROCESS() RFW_GetPayloadProcess()
#endif /* RFW_GET_PAYLOAD_PROCESS */

/*can be overridden in radio_conf.h*/
#ifndef RFW_TRANSMIT_LONGPACKET_TX_CHUNK_PROCESS
#define RFW_TRANSMIT_LONGPACKET_TX_CHUNK_PROCESS() RFW_TransmitLongPacket_TxChunkProcess()
#endif /* RFW_TRANSMIT_LONGPACKET_TX_CHUNK_PROCESS */

#ifndef RFW_MW_LOG_ENABLE
#define RFW_MW_LOG(...)
#else
#define RFW_MW_LOG   MW_LOG
#endif /* RFW_MW_LOG_ENABLE */

#ifndef RFW_ENABLE
#define RFW_ENABLE 0
#endif /* RFW_ENABLE */

#ifndef RFW_LONGPACKET_ENABLE
#define RFW_LONGPACKET_ENABLE 0
#endif /* v */

#ifndef RADIO_MEMCPY8
#define RADIO_MEMCPY8 memcpy
#endif /* RADIO_MEMCPY8 */

#ifndef RADIO_MEMSET8
#define RADIO_MEMSET8 memset
#endif /* RADIO_MEMSET8 */

#if ((RFW_LONGPACKET_ENABLE==1)&&(RFW_ENABLE==0))
#error RFW_ENABLE must be defined to 1 if RFW_LONGPACKET_ENABLE is defined to 1
#endif /* RFW_LONGPACKET_ENABLE */

/* Private variables ---------------------------------------------------------*/
#if (RFW_ENABLE == 1 )
static RadioFw_t RFWPacket = {0};
/*Radio buffer chunk*/
static uint8_t ChunkBuffer[RADIO_BUF_SIZE];
/*Radio buffer chunk for packet <=RADIO_BUF_SIZE and */
static uint8_t RxBuffer[RADIO_BUF_SIZE];
#endif /* RFW_ENABLE == 1 */
/* Private function prototypes -----------------------------------------------*/
#if (RFW_ENABLE == 1 )
/*!
 * @brief Record the seed of the (de-)Whitening algorithm
 *
 * @param [in] RFwInit_t      the whitening Init structure
 * @param [in] WhiteSeed        the Initial seed of the Whitening algorithm
 */
static void RFW_WhiteInitState( RFwInit_t *Init, uint16_t WhiteSeed );

/*!
 * @brief Set the state of the Whitening algorithm from the seed
 *
 * @param [in] RFWPacket      the whitening structure
 * @param [in] whiteSeed             the Initial seed of the Whitening algorithm
 */
static void RFW_WhiteSetState( RadioFw_t *RFWPacket );

/*!
 * @brief Initialize seed of the Crc algorithm
 *
 * @param [in] RFWPacket      the whitening Init structure
 * @param [in] CrcPolynomial         the Initial seed of the Crc algorithm
 * @param [in] CrcSeed               the Initial seed of the Crc seed
 * @param [in] CrcSeed               the Initial seed of the Crc types (Ibm/CCITT)
 */
static void RFW_CrcInitState( RFwInit_t *Init, const uint16_t CrcPolynomial, const uint16_t CrcSeed,
                              const RADIO_FSK_CrcTypes_t CrcTypes );

/*!
 * @brief Run the Crc algorithm
 * @param [in] RFWPacket      the whitening structure
 */
static void RFW_CrcSetState( RadioFw_t *RFWPacket );
/*!
 * @brief Run the Whitening algorithm
 *
 * @param [in] RFWPacket      the whitening structure
 * @param [in,out] Payload    the Payload to (de-)Whiten and the payload result
 * @param [in] Size           the Payload size
 */
static void RFW_WhiteRun( RadioFw_t *RFWPacket, uint8_t *Payload, uint32_t Size );

/*!
 * @brief Run the Crc algorithm
 *
 * @param [in] RFWPacket      the whitening structure
 * @param [in] Payload        the Payload to calculate the Crc
 * @param [in] Size           the Payload size
 * @param [out] CrcResult     the result of the Crc calculation
 */
static int32_t RFW_CrcRun( RadioFw_t *const RFWPacket, const uint8_t *Payload, const uint32_t Size,
                           uint8_t CrcResult[2] );

/*!
 * @brief Compute Crc Byte
 *
 * @param [in] crc        Crc bytes
 * @param [in] dataByte        the data byte
 * @param [out] polynomial     the polynomial to use
 * @return crc bytes out
 */
uint16_t RFW_CrcRun1Byte( uint16_t Crc, uint8_t DataByte, uint16_t Polynomial );

/*!
 * @brief Get the payload length after sync
 *
 * @param [out] PayloadLength        the length of PayloadOnly excluding CrcLengthField
 * @return 0 when no parameters error, -1 otherwise
 */
static int32_t RFW_GetPacketLength( uint16_t *PayloadLength );

/*!
 * @brief RFW_GetPayloadTimerEvent TimerEvent to get the payload data, de-whitening, and crc verification
 *
 * @Note calls RFW_GetPayloadProcess directly inside IRQ (default) or in background with sequencer or OS
 * @param [in] context        context of the timer
 */
static void RFW_GetPayloadTimerEvent( void *context );

/*!
 * @brief RFW_GetPayloadProcess process to get the payload data, de-whitening, and crc verification
 */
static void RFW_GetPayloadProcess( void );

#if (RFW_LONGPACKET_ENABLE == 1 )
/*!
 * @brief RFW_TransmitLongPacket_NewTxChunkTimer: long packet Tx timer callback to process intermediate chunk of TxData
 * @Note calls RFW_TransmitLongPacket_TxChunkProcess directly inside IRQ (default) or in background with sequencer or any Rtos
 *
 * @param [in] param        unused
 */
static void RFW_TransmitLongPacket_NewTxChunkTimerEvent( void *param );

/*!
 * @brief RFW_TransmitLongPacket_TxChunkProcess: tx long packet process intermediate chunk of TxData
 */
static void RFW_TransmitLongPacket_TxChunkProcess( void );
#endif /* RFW_LONGPACKET_ENABLE == 1 */

/*!
 * @brief RFW_GetPayload get the payload of
 *
 * @param [in] Offset        Offset of the
 * @param [in] Length        carrier frequency offset in Hertz
 */
static void RFW_GetPayload( uint8_t Offset, uint8_t Length );
#endif /* RFW_ENABLE == 1 */

/* Exported functions --------------------------------------------------------*/
int32_t RFW_TransmitLongPacket( uint16_t payload_size, uint32_t timeout,
                                void ( *TxLongPacketGetNextChunkCb )( uint8_t **buffer, uint8_t buffer_size ) )
{
    int32_t status = 0;
#if (RFW_LONGPACKET_ENABLE == 1 )
    uint32_t total_size = payload_size + RFWPacket.Init.PayloadLengthFieldSize + RFWPacket.Init.CrcFieldSize;

    RFW_MW_LOG( TS_ON, VLEVEL_M,  "RevID=%04X\r\n", LL_DBGMCU_GetRevisionID() );

    if( ( TxLongPacketGetNextChunkCb == NULL ) ||
        ( payload_size > ( 1 << ( 8 * RFWPacket.Init.PayloadLengthFieldSize ) ) - 1 ) || /*check that size fits inside the packetLengthField*/
        ( RFWPacket.Init.Enable == 0 ) || /* Can only be used when after RadioSetTxGenericConfig*/
        ( LL_DBGMCU_GetRevisionID() < 0x1003 ) )  /* Only available from stm32wl revision Y*/
    {
        status = -1;
    }
    else
    {
        /*chunk buffer pointer fed by the application*/
        uint8_t *app_chunk_buffer_ptr = NULL;
        /*size of the chunk to be sent*/
        uint8_t chunk_size;
        uint8_t crc_size;
        /*timeout for next chunk*/
        uint32_t chunk_timeout;
        /*Records call back*/
        RFWPacket.TxLongPacketGetNextChunkCb = TxLongPacketGetNextChunkCb;

        /* Radio IRQ is set to DIO1 by default */
        SUBGRF_SetDioIrqParams( IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                                IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT,
                                IRQ_RADIO_NONE,
                                IRQ_RADIO_NONE );

        /* Set DBG pin */
        DBG_GPIO_RADIO_TX( SET );
        /* Set RF switch */
        SUBGRF_SetSwitch( RFWPacket.AntSwitchPaSelect, RFSWITCH_TX );

        switch( RFWPacket.Init.Modem )
        {
        case MODEM_FSK:
        case MODEM_MSK:
        {
            if( RFWPacket.Init.Enable == 1 )
            {
                /*crc will be calculated on the fly along with packet chunk transmission*/
                uint8_t crc_result[2];
                /*init radio buffer offset*/
                RFWPacket.RadioBufferOffset = 0;
                /*long packet mode enable*/
                RFWPacket.LongPacketModeEnable = 1;
                /*Remaining bytes to transmit*/
                RFWPacket.LongPacketRemainingBytes = total_size;
                /*Records total payload bytes to transmit*/
                RFWPacket.PayloadLength = total_size;
                if( total_size > RADIO_BUF_SIZE )
                {
                    /*cut in chunk*/
                    if( total_size < RADIO_BUF_SIZE + RFWPacket.Init.CrcFieldSize )
                    {
                        /*reduce chunk so that crc is treated in the next chunk*/
                        chunk_size = RADIO_BUF_SIZE - RFWPacket.Init.PayloadLengthFieldSize - RFWPacket.Init.CrcFieldSize;
                    }
                    else
                    {
                        chunk_size = RADIO_BUF_SIZE - RFWPacket.Init.PayloadLengthFieldSize;
                    }
                    /*Set crc size for the crc calculation: no crc here because it is not the end of the packet*/
                    crc_size = 0;
                }
                else
                {
                    chunk_size = payload_size;
                    /*Set crc size for the crc calculation*/
                    crc_size = RFWPacket.Init.CrcFieldSize;
                }
                /* Prepend payload size before Payload*/
                if( RFWPacket.Init.PayloadLengthFieldSize == 1 )
                {
                    ChunkBuffer[0] = payload_size;
                }
                else
                {
                    ChunkBuffer[0] = ( uint8_t )( ( payload_size ) >> 8 );
                    ChunkBuffer[1] = ( uint8_t )( ( payload_size ) & 0xFF );
                }
                /* Get Tx chunk from app*/
                TxLongPacketGetNextChunkCb( &app_chunk_buffer_ptr, chunk_size );

                /* Copy first chunk in ChunkBuffer Buffer*/
                RADIO_MEMCPY8( &ChunkBuffer[RFWPacket.Init.PayloadLengthFieldSize], app_chunk_buffer_ptr, chunk_size );

                if( RFWPacket.Init.CrcEnable == 1 )
                {
                    /* Set the state of the Crc to crc_seed*/
                    RFW_CrcSetState( &RFWPacket );
                    /* Run the crc calculation on payload length and payload*/
                    RFW_CrcRun( &RFWPacket, ChunkBuffer, RFWPacket.Init.PayloadLengthFieldSize + chunk_size, crc_result );
                    /* Append the crc result after the payload if total_size<= RADIO_BUF_SIZE*/
                    RADIO_MEMCPY8( &ChunkBuffer[RFWPacket.Init.PayloadLengthFieldSize + chunk_size], crc_result, crc_size );
                }
                /* Init whitening at beginning of the packet*/
                RFW_WhiteSetState( &RFWPacket );
                /* Run the whitening calculation on payload length, payload and crc if crc fits inside 1st chunk*/
                RFW_WhiteRun( &RFWPacket, &ChunkBuffer[0], RFWPacket.Init.PayloadLengthFieldSize + chunk_size + crc_size );
                /* Configure the Transmitter to send all*/
                /* Init radio buffer */
                SUBGRF_WriteRegister( SUBGHZ_GRTXPLDLEN, RFWPacket.Init.PayloadLengthFieldSize + chunk_size + crc_size );
                SUBGRF_WriteRegister( SUBGHZ_TXADRPTR, 0 );
                /* Send*/
                SUBGRF_SendPayload( ChunkBuffer, RFWPacket.Init.PayloadLengthFieldSize + chunk_size + crc_size, 0 );
                if( total_size > RADIO_BUF_SIZE )
                {
                    /*in case total size is greater than RADIO_BUF_SIZE, need to program a timer to get next chunk*/
                    /*RFWPacket.LongPacketRemainingBytes-= RFWPacket.Init.PayloadLengthFieldSize+ chunk_size+ crc_size;*/
                    /*Initialize Timer to get new chunk and update radio ptr*/
                    chunk_timeout = ( LONGPACKET_CHUNK_LENGTH_BYTES * 8 * 1000 ) / RFWPacket.BitRate;
                    RFW_MW_LOG( TS_ON, VLEVEL_M,  "Timeout=%d,\r\n", chunk_timeout );
                    TimerInit( &RFWPacket.Timer, RFW_TransmitLongPacket_NewTxChunkTimerEvent );
                    TimerSetValue( &RFWPacket.Timer, chunk_timeout );
                    TimerStart( &RFWPacket.Timer );
                    /*Write bit infinite_sequence = 1, required for long packet*/
                    uint8_t reg = SUBGRF_ReadRegister( SUBGHZ_GPKTCTL1AR );
                    SUBGRF_WriteRegister( SUBGHZ_GPKTCTL1AR, reg | 0x02 );

                    TimerSetValue( RFWPacket.RxTimeoutTimer, timeout );
                    TimerStart( RFWPacket.RxTimeoutTimer );
                }
            }
            else
            {
                /* error*/
                status = -1;
            }
            break;
        }
        case MODEM_LORA:
        {
            /* not supported by the radio Ip*/
            status = -2;
            break;
        }
        case MODEM_BPSK:
        {
            /* not supported by the FW*/
            status = -2;
            break;
        }
        case MODEM_SIGFOX_TX:
        {
            /* not supported by the FW*/
            status = -2;
            break;
        }
        default:
            break;
        }
    }
#else
    status = -1;
#endif /* RFW_LONGPACKET_ENABLE == 1 */
    return status;
}

int32_t RFW_ReceiveLongPacket( uint8_t boosted_mode, uint32_t timeout,
                               void ( *RxLongPacketStoreChunkCb )( uint8_t *buffer, uint8_t chunk_size ) )
{
    int32_t status = 0;
#if (RFW_LONGPACKET_ENABLE == 1 )
    if( ( RxLongPacketStoreChunkCb == NULL ) ||
        ( RFWPacket.Init.Enable == 0 ) ) /* Can only be used when after RadioSetRxGenericConfig*/
    {
        status = -1;
    }
    else
    {
        /*Records call back*/
        RFWPacket.RxLongPacketStoreChunkCb = RxLongPacketStoreChunkCb;
        SUBGRF_SetDioIrqParams( IRQ_SYNCWORD_VALID | IRQ_RX_TX_TIMEOUT,
                                IRQ_SYNCWORD_VALID | IRQ_RX_TX_TIMEOUT,
                                IRQ_RADIO_NONE,
                                IRQ_RADIO_NONE );
        SUBGRF_SetSwitch( RFWPacket.AntSwitchPaSelect, RFSWITCH_RX );
        /*init radio buffer offset*/
        RFWPacket.RadioBufferOffset = 0;
        /* Init whitening at beginning of the packet*/
        RFW_WhiteSetState( &RFWPacket );
        /* Set the state of the Crc to crc_seed*/
        RFW_CrcSetState( &RFWPacket );
        /* Init radio buffer */
        SUBGRF_WriteRegister( SUBGHZ_GRTXPLDLEN, 255 );
        SUBGRF_WriteRegister( SUBGHZ_RXADRPTR, 0 );
        /*enable long packet*/
        RFWPacket.LongPacketModeEnable = 1;

        if( timeout != 0 )
        {
            TimerSetValue( RFWPacket.RxTimeoutTimer, timeout );
            TimerStart( RFWPacket.RxTimeoutTimer );
        }
        DBG_GPIO_RADIO_RX( SET );
        if( boosted_mode == 1 )
        {
            SUBGRF_SetRxBoosted( 0xFFFFFF ); /* Rx Continuous */
        }
        else
        {
            SUBGRF_SetRx( 0xFFFFFF ); /* Rx Continuous */
        }
    }
#else
    status = -1;
#endif /* RFW_LONGPACKET_ENABLE == 1 */
    return status;
}

int32_t RFW_Init( ConfigGeneric_t *config, RadioEvents_t *RadioEvents, TimerEvent_t *TimeoutTimerEvent )
{
#if (RFW_ENABLE == 1 )
    RADIO_FSK_PacketLengthModes_t HeaderType;
    uint32_t RxMaxPayloadLength = 0;
    RADIO_FSK_CrcTypes_t CrcLength;
    uint16_t whiteSeed;
    uint16_t CrcPolynomial;
    uint16_t CrcSeed;
    if( config->rtx == CONFIG_TX )
    {
        HeaderType =    config->TxConfig->fsk.HeaderType;
        CrcLength =     config->TxConfig->fsk.CrcLength;
        whiteSeed =     config->TxConfig->fsk.whiteSeed;
        CrcPolynomial = config->TxConfig->fsk.CrcPolynomial;
        CrcSeed =       config->TxConfig->fsk.CrcSeed;
        RFWPacket.BitRate = config->TxConfig->fsk.BitRate;
        RFWPacket.TxTimeoutTimer = TimeoutTimerEvent;
    }
    else
    {
        HeaderType =          config->RxConfig->fsk.LengthMode;
        CrcLength =           config->RxConfig->fsk.CrcLength;
        RxMaxPayloadLength = config->RxConfig->fsk.MaxPayloadLength;
        whiteSeed =           config->RxConfig->fsk.whiteSeed;
        CrcPolynomial =       config->RxConfig->fsk.CrcPolynomial;
        CrcSeed =             config->RxConfig->fsk.CrcSeed;
        RFWPacket.BitRate =   config->RxConfig->fsk.BitRate;
        RFWPacket.RxTimeoutTimer = TimeoutTimerEvent;
    }
    if( ( RadioEvents != NULL ) && ( RadioEvents->RxError ) )
    {
        RFWPacket.Init.RadioEvents = RadioEvents;
    }
    else
    {
        return -1;
    }
    if( HeaderType == RADIO_FSK_PACKET_2BYTES_LENGTH )
    {
#if (RFW_LONGPACKET_ENABLE == 1 )
        RFWPacket.Init.PayloadLengthFieldSize = 2;
#else
        return -1;
#endif /* RFW_LONGPACKET_ENABLE == 1 */
    }
    else
    {
        RFWPacket.Init.PayloadLengthFieldSize = 1;
    }
    /*record, used to reject packet in length decoded at sync time out greater than LongPacketMaxRxLength*/
    RFWPacket.Init.LongPacketMaxRxLength = RxMaxPayloadLength;
    if( CrcLength == RADIO_FSK_CRC_OFF )
    {
        RFWPacket.Init.CrcEnable = 0;
        RFWPacket.Init.CrcFieldSize = 0;
    }
    else
    {
        RFWPacket.Init.CrcEnable = 1;
        RFWPacket.Init.CrcFieldSize = 2;
    }
    /*Macro can be used to init interrupt behaviour*/
    RFW_IT_INIT();
    /*Initialise whitening Seed*/
    RFW_WhiteInitState( &RFWPacket.Init, whiteSeed );
    /*Initialise Crc Seed*/
    RFW_CrcInitState( &RFWPacket.Init, CrcPolynomial, CrcSeed, CrcLength );
    /*Enable the RFWPacket decoding*/
    RFWPacket.Init.Enable = 1;
    /* Initialize Timer for end of fixed packet, started at sync*/
    TimerInit( &RFWPacket.Timer, RFW_GetPayloadTimerEvent );
    return 0;
#else
    return -1;
#endif /* RFW_ENABLE == 1 */
}

void RFW_DeInit( void )
{
#if (RFW_ENABLE == 1 )
    RFWPacket.Init.Enable = 0; /*Disable the RFWPacket decoding*/
#endif /* RFW_ENABLE == 1 */
}

uint8_t RFW_Is_Init( void )
{
#if (RFW_ENABLE == 1 )
    return RFWPacket.Init.Enable;
#else
    return 0;
#endif /* RFW_ENABLE == 1 */
}

uint8_t RFW_Is_LongPacketModeEnabled( void )
{
#if (RFW_ENABLE == 1 )
    return RFWPacket.LongPacketModeEnable;
#else
    return 0;
#endif /* RFW_ENABLE == 1 */
}

void RFW_SetAntSwitch( uint8_t AntSwitch )
{
#if (RFW_ENABLE == 1 )
    RFWPacket.AntSwitchPaSelect = AntSwitch;
#endif /* RFW_ENABLE == 1 */
}

int32_t RFW_TransmitInit( uint8_t *inOutBuffer, uint8_t size, uint8_t *outSize )
{
    int32_t status = -1;
#if (RFW_ENABLE == 1 )
    uint8_t crc_result[2];
    if( size + RFWPacket.Init.PayloadLengthFieldSize + RFWPacket.Init.CrcFieldSize > RADIO_BUF_SIZE )
    {
        RFW_MW_LOG( TS_ON, VLEVEL_M, "RadioSend Oversize\r\n" );
        status = -1;
    }
    else
    {
        /* Copy tx buffer in payload*/
        RADIO_MEMCPY8( &ChunkBuffer[RFWPacket.Init.PayloadLengthFieldSize], inOutBuffer, size );
        /* Calculate the crc on */
        /* Payload Size without the packet length field nor the CRC */
        /* Prepend payload size before Payload*/
        if( RFWPacket.Init.PayloadLengthFieldSize == 1 )
        {
            ChunkBuffer[0] = size;
        }
        else
        {
            ChunkBuffer[0] = 0;
            ChunkBuffer[1] = size;
        }
        if( RFWPacket.Init.CrcEnable == 1 )
        {
            /* Set the state of the Crc to crc_seed*/
            RFW_CrcSetState( &RFWPacket );
            /*Run the crc calculation on payload length and payload*/
            RFW_CrcRun( &RFWPacket, &ChunkBuffer[0], size + RFWPacket.Init.PayloadLengthFieldSize, crc_result );
            /*append the crc result after the payload*/
            RADIO_MEMCPY8( &ChunkBuffer[size + RFWPacket.Init.PayloadLengthFieldSize], crc_result, RFWPacket.Init.CrcFieldSize );
        }
        /*init whitening at beginning of the packet*/
        RFW_WhiteSetState( &RFWPacket );
        /*Run the whitening calculation on payload length, payload and crc*/
        RFW_WhiteRun( &RFWPacket, &ChunkBuffer[0], size + RFWPacket.Init.PayloadLengthFieldSize + RFWPacket.Init.CrcFieldSize );
        /*Configure the Transmitter to send all*/
        *outSize = ( uint8_t ) size + RFWPacket.Init.PayloadLengthFieldSize + RFWPacket.Init.CrcFieldSize;
        /*copy result*/
        RADIO_MEMCPY8( inOutBuffer, ChunkBuffer, *outSize );

        RFWPacket.LongPacketModeEnable = 0;

        status = 0;
    }
#endif /* RFW_ENABLE == 1 */
    return status;
}

int32_t RFW_ReceiveInit( void )
{
#if (RFW_ENABLE == 1 )
    /* Radio IRQ is set to DIO1 by default */
    SUBGRF_SetDioIrqParams( IRQ_RADIO_ALL & ( ~IRQ_RX_DONE ), /* IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT, */
                            IRQ_RADIO_ALL & ( ~IRQ_RX_DONE ), /* IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT, */
                            IRQ_RADIO_NONE,
                            IRQ_RADIO_NONE );

    /*init whitening at beginning of the packet*/
    RFW_WhiteSetState( &RFWPacket );
    /* Set the state of the Crc to crc_seed*/
    RFW_CrcSetState( &RFWPacket );

    RFWPacket.RxPayloadOffset = 0;

    RFWPacket.LongPacketModeEnable = 0;
    return 0;
#else
    return -1;
#endif /* RFW_ENABLE == 1 */
}

void RFW_DeInit_TxLongPacket( void )
{
#if (RFW_LONGPACKET_ENABLE == 1 )
    /*long packet WA*/
    uint8_t reg = SUBGRF_ReadRegister( SUBGHZ_GPKTCTL1AR );
    SUBGRF_WriteRegister( SUBGHZ_GPKTCTL1AR, reg & ~0x02 ); /* clear infinite_sequence bit */
    SUBGRF_WriteRegister( SUBGHZ_GRTXPLDLEN, 0xFF ); /* RxTxPldLen: reset to 0xFF */
#endif /* RFW_LONGPACKET_ENABLE == 1 */
}

void RFW_ReceivePayload( void )
{
#if (RFW_ENABLE == 1 )
    uint16_t PayloadLength = 0;
    if( RFW_GetPacketLength( &PayloadLength ) == 0 )
    {
        uint32_t timeout;
        uint32_t packet_length = PayloadLength + RFWPacket.Init.CrcFieldSize;
        /*record payload length*/
        RFWPacket.PayloadLength = PayloadLength;
        /*record remaining payload length*/
        RFWPacket.LongPacketRemainingBytes = ( uint16_t ) packet_length;
        /*record rx buffer offset*/
        RFWPacket.RadioBufferOffset = RFWPacket.Init.PayloadLengthFieldSize;
        /*if decoded PayloadLength is longer than LongPacketMaxRxLength, reject packet*/
        if( PayloadLength > RFWPacket.Init.LongPacketMaxRxLength )
        {
            SUBGRF_SetStandby( STDBY_RC );
            RFWPacket.Init.RadioEvents->RxError( );
            return;
        }
        if( packet_length < LONGPACKET_CHUNK_LENGTH_BYTES )
        {
            /* all in one chunks*/
            /* calculate time to end of packet*/
            timeout = DIVC( ( packet_length ) * 8 * 1000, RFWPacket.BitRate ) + 2;
            /**/
            /* start timer at the end of the packet*/
            RFW_MW_LOG( TS_ON, VLEVEL_M,  "end packet in %dms\r\n", timeout );

        }
        else if( packet_length < ( 3 * LONGPACKET_CHUNK_LENGTH_BYTES / 2 ) )
        {
            /* packet contained in 2 chunks*/
            /* make sure that crc not cut in chunk*/
            timeout = DIVR( ( packet_length * 8 * 1000 ) / 2, RFWPacket.BitRate );
        }
        else
        {
            /* packet contained in multiple chunk*/
            /* program radio timer for first chunk*/
            timeout = DIVR( LONGPACKET_CHUNK_LENGTH_BYTES * 8 * 1000, RFWPacket.BitRate );
        }
        TimerSetValue( &RFWPacket.Timer, timeout );
        TimerStart( &RFWPacket.Timer );
    }
    else
    {
        /*timeout*/
        SUBGRF_SetStandby( STDBY_RC );
        RFWPacket.Init.RadioEvents->RxTimeout( );
    }
#endif /* RFW_ENABLE == 1 */
}

void RFW_SetRadioModem( RadioModems_t Modem )
{
#if (RFW_ENABLE == 1 )
    RFWPacket.Init.Modem = Modem;
#endif /* RFW_ENABLE == 1 */
}

/* Private Functions Definition -----------------------------------------------*/
#if (RFW_LONGPACKET_ENABLE == 1 )
static void RFW_TransmitLongPacket_NewTxChunkTimerEvent( void *param )
{
    RFW_TRANSMIT_LONGPACKET_TX_CHUNK_PROCESS();
}

static void RFW_TransmitLongPacket_TxChunkProcess( void )
{
    uint8_t *app_chunk_buffer_ptr = NULL;
    uint8_t chunk_size = 0;
    uint8_t crc_result[2] = {0};
    uint8_t crc_size;
    uint32_t timeout;/*timeout for next chunk*/
    /*records how much has been sent*/
    uint8_t read_ptr = SUBGRF_ReadRegister( SUBGHZ_TXADRPTR ); /*radio has transmitted up to read_ptr*/
    uint8_t write_ptr = SUBGRF_ReadRegister( SUBGHZ_GRTXPLDLEN ); /*from read_ptr to write_ptr still to be transmitted*/
    /*calculates how much bytes were sent since previous radio loading*/
    uint8_t bytes_sent = read_ptr - RFWPacket.RadioBufferOffset;
    /*bytes already loaded in the radio to send*/
    uint8_t bytes_loaded = write_ptr - read_ptr;

    /* Update offset tx, intentional wrap around*/
    RFWPacket.RadioBufferOffset += bytes_sent;
    /*record payload remaining bytes to send*/
    RFWPacket.LongPacketRemainingBytes -= bytes_sent;
    RFW_MW_LOG( TS_ON, VLEVEL_M,  "read_ptr=%d, write_ptr=%d, bytes_sent=%d, bytes_loaded=%d,remaining to send=%d\r\n",
                read_ptr, write_ptr, bytes_sent, bytes_loaded, RFWPacket.LongPacketRemainingBytes );
    if( RFWPacket.LongPacketRemainingBytes > 256 )
    {
        /*get next chunk */
        /*make sure that at least full CrcFieldSize will be loaded for the last chunk*/
        if( RFWPacket.LongPacketRemainingBytes > 256 + RFWPacket.Init.CrcFieldSize )
        {
            chunk_size = bytes_sent;
        }
        else
        {
            chunk_size = bytes_sent - RFWPacket.Init.CrcFieldSize;
        }
        /*no crc since it is not the last chunk*/
        crc_size = 0;
        /*calculate timeout for next chunk*/
        timeout = DIVR( chunk_size * 8 * 1000, RFWPacket.BitRate );

        TimerSetValue( &RFWPacket.Timer, timeout );
        TimerStart( &RFWPacket.Timer );
    }
    else
    {
        /*last chunk to send*/

        if( RFWPacket.LongPacketRemainingBytes > bytes_loaded )
        {
            chunk_size = RFWPacket.LongPacketRemainingBytes - bytes_loaded;
        }
        else/* nothing to load anymore*/
        {
            chunk_size = RFWPacket.Init.CrcFieldSize;
        }
        /* crc, since it is the last chunk*/
        crc_size = RFWPacket.Init.CrcFieldSize;
        /*no more bytes to send*/
        RFWPacket.LongPacketRemainingBytes = 0;
        /*no need to program another timer, Tx done will complete the Tx process*/
    }
    /*get new chunk from the app*/
    RFWPacket.TxLongPacketGetNextChunkCb( &app_chunk_buffer_ptr, chunk_size - crc_size );
    /* Copy app_chunk_buffer_ptr in ChunkBuffer Buffer*/
    RADIO_MEMCPY8( ChunkBuffer, app_chunk_buffer_ptr, chunk_size - crc_size );
    if( RFWPacket.Init.CrcEnable == 1 )
    {
        /* Run the crc calculation on payload length and payload*/
        RFW_CrcRun( &RFWPacket, ChunkBuffer, chunk_size - crc_size, crc_result );
        /* Append the crc result after the payload (if last chunk)*/
        RADIO_MEMCPY8( &ChunkBuffer[chunk_size - crc_size], crc_result, crc_size );
    }
    /* Run the whitening calculation on payload length, payload and crc*/
    RFW_WhiteRun( &RFWPacket, ChunkBuffer, chunk_size );
    /*write next chunk*/
    SUBGRF_WriteBuffer( write_ptr, ChunkBuffer, chunk_size );

    /*update end ptr*/
    SUBGRF_WriteRegister( SUBGHZ_GRTXPLDLEN, ( uint8_t )( chunk_size + write_ptr ) );

    RFW_MW_LOG( TS_ON, VLEVEL_M,  "next chunk size=%d, new write ptr=%d\n\r", chunk_size + crc_size,
                ( uint8_t )( chunk_size + crc_size + write_ptr ) );
}
#endif /* RFW_LONGPACKET_ENABLE == 1 */

#if (RFW_ENABLE == 1 )
static void RFW_WhiteInitState( RFwInit_t *Init, uint16_t WhiteSeed )
{
    Init->WhiteSeed = WhiteSeed;
}

static void RFW_WhiteSetState( RadioFw_t *RFWPacket )
{
    RFWPacket->WhiteLfsrState = RFWPacket->Init.WhiteSeed;
}

static void RFW_CrcInitState( RFwInit_t *Init, const uint16_t CrcPolynomial, const uint16_t CrcSeed,
                              const RADIO_FSK_CrcTypes_t CrcType )
{
    Init->CrcPolynomial = CrcPolynomial;
    Init->CrcSeed = CrcSeed;
    Init->CrcType = CrcType;
}

static void RFW_CrcSetState( RadioFw_t *RFWPacket )
{
    RFWPacket->CrcLfsrState = RFWPacket->Init.CrcSeed;
}

static void RFW_WhiteRun( RadioFw_t *RFWPacket, uint8_t *Payload, uint32_t Size )
{
    /*run the whitening algo on Size bytes*/
    uint16_t ibmwhite_state = RFWPacket->WhiteLfsrState;
    for( int32_t i = 0; i < Size; i++ )
    {
        Payload[i] ^= ibmwhite_state & 0xFF;
        for( int32_t j = 0; j < 8; j++ )
        {
            uint8_t msb = ( ( ibmwhite_state >> 5 ) & 0x1 ) ^ ( ( ibmwhite_state >> 0 ) & 0x1 );
            ibmwhite_state = ( ( msb << 8 ) | ( ibmwhite_state >> 1 ) );
        }
    }
    RFWPacket->WhiteLfsrState = ibmwhite_state;
}

static int32_t RFW_CrcRun( RadioFw_t *const RFWPacket, const uint8_t *Payload, const uint32_t Size,
                           uint8_t CrcResult[2] )
{
    int32_t status = 0;
    int32_t i = 0;
    uint16_t polynomial = RFWPacket->Init.CrcPolynomial;
    /* Restore state from previous chunk*/
    uint16_t crc = RFWPacket->CrcLfsrState;
    for( i = 0; i < Size; i++ )
    {
        crc = RFW_CrcRun1Byte( crc, Payload[i], polynomial );
    }
    /*Save state for next chunk*/
    RFWPacket->CrcLfsrState = crc;

    if( RFWPacket->Init.CrcType == RADIO_FSK_CRC_2_BYTES_IBM )
    {
        CrcResult[1] = crc & 0xFF;
        CrcResult[0] = crc >> 8;
    }
    else
    {
        crc = ~crc ;
        CrcResult[1] = crc & 0xFF;
        CrcResult[0] = crc >> 8;
    }
    return status;
}

uint16_t RFW_CrcRun1Byte( uint16_t Crc, uint8_t DataByte, uint16_t Polynomial )
{
    uint8_t i;
    for( i = 0; i < 8; i++ )
    {
        if( ( ( ( Crc & 0x8000 ) >> 8 ) ^ ( DataByte & 0x80 ) ) != 0 )
        {
            Crc <<= 1;
            Crc ^= Polynomial;
        }
        else
        {
            Crc <<= 1;
        }
        DataByte <<= 1;
    }
    return Crc;
}

static int32_t RFW_PollRxBytes( uint32_t bytes )
{
    uint32_t now = TimerGetCurrentTime( );
    uint8_t reg_buff_ptr_ref = SUBGRF_ReadRegister( SUBGHZ_RXADRPTR );
    uint8_t reg_buff_ptr = reg_buff_ptr_ref;
    uint32_t timeout = DIVC( bytes * 8 * 1000, RFWPacket.BitRate );
    /* Wait that packet length is received */
    while( ( reg_buff_ptr - reg_buff_ptr_ref ) < bytes )
    {
        /*reading rx address pointer*/
        reg_buff_ptr = SUBGRF_ReadRegister( SUBGHZ_RXADRPTR );
        if( TimerGetElapsedTime( now ) > timeout )
        {
            /*timeout*/
            return -1;
        }
    }
    return 0;
}

static int32_t RFW_GetPacketLength( uint16_t *PayloadLength )
{
    if( 0UL != RFW_PollRxBytes( RFWPacket.Init.PayloadLengthFieldSize ) )
    {
        return -1;
    }
    /* Get buffer from Radio*/
    SUBGRF_ReadBuffer( 0, ChunkBuffer, RFWPacket.Init.PayloadLengthFieldSize );
    /* De-whiten packet length*/
    RFW_WhiteRun( &RFWPacket, ChunkBuffer, RFWPacket.Init.PayloadLengthFieldSize );
    /*do crc 1st calculation packetLengthField and store intermediate result */
    if( RFWPacket.Init.CrcEnable == 1 )
    {
        /*run Crc algo on payloadLengthField*/
        uint8_t crc_dummy[2];
        RFW_CrcRun( &RFWPacket, ChunkBuffer, RFWPacket.Init.PayloadLengthFieldSize, crc_dummy );
    }
    if( RFWPacket.Init.PayloadLengthFieldSize == 1 )
    {
        *PayloadLength = ( uint16_t ) ChunkBuffer[0];
    }
    else
    {
        /*packet length is 2 bytes*/
        *PayloadLength = ( ( ( uint16_t ) ChunkBuffer[0] ) << 8 ) | ChunkBuffer[1];
    }
    RFW_MW_LOG( TS_ON, VLEVEL_M,  "PayloadLength=%d,\r\n", *PayloadLength );
    return 0;
}

static void RFW_GetPayloadTimerEvent( void *context )
{
    RFW_GET_PAYLOAD_PROCESS();
}

static void RFW_GetPayloadProcess( void )
{
    /*long packet mode*/
    uint8_t read_ptr = SUBGRF_ReadRegister( SUBGHZ_RXADRPTR );
    uint8_t size = read_ptr - RFWPacket.RadioBufferOffset;
    uint32_t Timeout;
    /*check remaining size*/
    if( RFWPacket.LongPacketRemainingBytes > size )
    {
        /* update LongPacketRemainingBytes*/
        RFWPacket.LongPacketRemainingBytes -= size;
        /*intermediate chunk*/
        RFW_MW_LOG( TS_ON, VLEVEL_M,  "RxTxPldLen=0x%02X,\r\n", SUBGRF_ReadRegister( SUBGHZ_GRTXPLDLEN ) );
        RFW_MW_LOG( TS_ON, VLEVEL_M,  "RxAddrPtr=0x%02X,\r\n", read_ptr );
        RFW_MW_LOG( TS_ON, VLEVEL_M,  "offset= %d, size=%d, remaining=%d,\r\n", RFWPacket.RadioBufferOffset, size,
                    RFWPacket.LongPacketRemainingBytes );
        /*update pld length so that not reached*/
        SUBGRF_WriteRegister( SUBGHZ_GRTXPLDLEN, read_ptr - 1 );
        /* read data from radio*/
        SUBGRF_ReadBuffer( RFWPacket.RadioBufferOffset, ChunkBuffer, size );
        /* update buffer Offset, with intentional wrap around*/
        RFWPacket.RadioBufferOffset += size;
        /*Run the de-whitening on current chunk*/
        RFW_WhiteRun( &RFWPacket, ChunkBuffer, size );
        if( RFWPacket.Init.CrcEnable == 1 )
        {
            /*run Crc algo on partial chunk*/
            uint8_t crc_dummy[2];
            RFW_CrcRun( &RFWPacket, ChunkBuffer, size, crc_dummy );
        }

        if( RFWPacket.LongPacketModeEnable == 1 )
        {
            /*report rx data chunk to application*/
            RFWPacket.RxLongPacketStoreChunkCb( ChunkBuffer, size );
        }
        else
        {
            if( RFWPacket.RxPayloadOffset += size < RADIO_BUF_SIZE )
            {
                RADIO_MEMCPY8( &RxBuffer[RFWPacket.RxPayloadOffset], ChunkBuffer, size );
                RFWPacket.RxPayloadOffset += size;
            }
            else
            {
                /*stop the radio*/
                SUBGRF_SetStandby( STDBY_RC );
                /*report CRC error*/
                RFWPacket.Init.RadioEvents->RxError( );
                return;
            }
        }
        /*calculate next timer timeout*/
        if( RFWPacket.LongPacketRemainingBytes < LONGPACKET_CHUNK_LENGTH_BYTES )
        {
            /*for the next and last chunk DIVC +1 to make sure crc is received.*/
            Timeout = DIVC( ( RFWPacket.LongPacketRemainingBytes ) * 8 * 1000, RFWPacket.BitRate ) + 2;
        }
        else if( RFWPacket.LongPacketRemainingBytes < ( 3 * LONGPACKET_CHUNK_LENGTH_BYTES ) / 2 )
        {
            /*this is to make sure that last chunk will always be greater than LONGPACKET_CHUNK_LENGTH_BYTES/2 */
            Timeout = DIVR( ( RFWPacket.LongPacketRemainingBytes / 2 )  * 8 * 1000, RFWPacket.BitRate );
        }
        else
        {
            /*size value is close to LONGPACKET_CHUNK_LENGTH_BYTES with +/- errors compensated in closed loop here*/
            Timeout = DIVR( ( LONGPACKET_CHUNK_LENGTH_BYTES )  * 8 * 1000, RFWPacket.BitRate );
        }
        TimerSetValue( &RFWPacket.Timer, Timeout );
        TimerStart( &RFWPacket.Timer );
    }
    else
    {
        if( RFWPacket.LongPacketRemainingBytes < RFWPacket.Init.CrcFieldSize )
        {
            /* force LongPacketRemainingBytes to CrcFieldSize: this should never happen*/
            RFWPacket.LongPacketRemainingBytes = RFWPacket.Init.CrcFieldSize;
        }
        /*last chunk*/
        RFW_MW_LOG( TS_ON, VLEVEL_M,  "LastChunk. offset= %d, size=%d, remaining=%d,\r\n", RFWPacket.RadioBufferOffset, size,
                    RFWPacket.LongPacketRemainingBytes );
        size = RFWPacket.LongPacketRemainingBytes;
        /* update LongPacketRemainingBytes*/
        RFWPacket.LongPacketRemainingBytes = 0;
        /*Process last chunk*/
        RFW_GetPayload( RFWPacket.RadioBufferOffset, size );
    }
}

static void RFW_GetPayload( uint8_t Offset, uint8_t Length )
{
    uint8_t crc_result[2];
    /*stop the radio*/
    SUBGRF_SetStandby( STDBY_RC );
    /*read data buffer*/
    SUBGRF_ReadBuffer( Offset, ChunkBuffer, Length );
    /*Run the de-whitening on all packet*/
    RFW_WhiteRun( &RFWPacket, ChunkBuffer, Length );
    if( RFWPacket.Init.CrcEnable == 1 )
    {
        RFW_CrcRun( &RFWPacket, ChunkBuffer, Length - RFWPacket.Init.CrcFieldSize, crc_result );
    }
    if( RFWPacket.LongPacketModeEnable == 1 )
    {
        /*report rx data chunk to application*/

        RFWPacket.RxLongPacketStoreChunkCb( ChunkBuffer, Length - RFWPacket.Init.CrcFieldSize );
    }
    else
    {
        if( RFWPacket.RxPayloadOffset + Length - RFWPacket.Init.CrcFieldSize < RADIO_BUF_SIZE )
        {
            RADIO_MEMCPY8( &RxBuffer[RFWPacket.RxPayloadOffset], ChunkBuffer, Length - RFWPacket.Init.CrcFieldSize );
            RFWPacket.RxPayloadOffset += Length - RFWPacket.Init.CrcFieldSize;
        }
        else
        {
            /*report CRC error*/
            RFWPacket.Init.RadioEvents->RxError( );
            return;
        }
    }
    TimerStop( RFWPacket.RxTimeoutTimer );
    /* CRC check*/
    RFW_MW_LOG( TS_ON, VLEVEL_M,  "crc_result= 0x%02X%02X, crc_payload=0x%02X%02X\r\n", crc_result[0], crc_result[1],
                ChunkBuffer[Length - 2], ChunkBuffer[Length - 1] );
    if( ( ( crc_result[0] == ChunkBuffer[Length - 2] ) &&
          ( crc_result[1] == ChunkBuffer[Length - 1] ) ) ||
        ( RFWPacket.Init.CrcEnable == 0 ) )
    {
        /*read Rssi sampled at Sync*/
        uint8_t rssi_sync = SUBGRF_ReadRegister( 0x06CA );
        /* Get Carrier Frequency Offset*/
        int32_t cfo;
        SUBGRF_GetCFO( RFWPacket.BitRate, &cfo );
        /*ChunkBuffer[1] to remove packet Length*/
        RFWPacket.Init.RadioEvents->RxDone( RxBuffer,
                                            RFWPacket.RxPayloadOffset,
                                            -( rssi_sync >> 1 ),
                                            ( int8_t ) DIVR( cfo, 1000 ) );
    }
    else
    {
        /*report CRC error*/
        RFWPacket.Init.RadioEvents->RxError( );
    }
    DBG_GPIO_RADIO_RX( RST );
}
#endif /* RFW_ENABLE == 1 */
