/*!
 * \file      radio_ex.h
 *
 * \brief     Extended Radio driver API definition
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
  * @file    radio_ex.h
  * @author  MCD Application Team
  * @brief   generic radio driver definition
  ******************************************************************************
 */
 
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RADIO_EX_H__
#define __RADIO_EX_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/




/*******************************************Radio LORA enum*****************************************/
typedef enum
{
    GENERIC_FSK = 0,
    GENERIC_LORA,
    GENERIC_BPSK,
}GenericModems_t;

/*!
 * @brief Represents the possible spreading factor values in LoRa packet types
 */
typedef enum
{
    RADIO_LORA_SF5                                = 0x05,
    RADIO_LORA_SF6                                = 0x06,
    RADIO_LORA_SF7                                = 0x07,
    RADIO_LORA_SF8                                = 0x08,
    RADIO_LORA_SF9                                = 0x09,
    RADIO_LORA_SF10                               = 0x0A,
    RADIO_LORA_SF11                               = 0x0B,
    RADIO_LORA_SF12                               = 0x0C,
}RADIO_LoRaSpreadingFactors_t;

/*!
 * @brief Represents the coding rate values for LoRa packet type
 */
typedef enum
{
    RADIO_LORA_CR_4_5                             = 0x01,
    RADIO_LORA_CR_4_6                             = 0x02,
    RADIO_LORA_CR_4_7                             = 0x03,
    RADIO_LORA_CR_4_8                             = 0x04,
}RADIO_LoRaCodingRates_t;
/*!
 * @brief Represents the bandwidth values for LoRa packet type
 */
typedef enum
{
    RADIO_LORA_BW_500                             = 6,
    RADIO_LORA_BW_250                             = 5,
    RADIO_LORA_BW_125                             = 4,
    RADIO_LORA_BW_062                             = 3,
    RADIO_LORA_BW_041                             = 10,
    RADIO_LORA_BW_031                             = 2,
    RADIO_LORA_BW_020                             = 9,
    RADIO_LORA_BW_015                             = 1,
    RADIO_LORA_BW_010                             = 8,
    RADIO_LORA_BW_007                             = 0,
}RADIO_LoRaBandwidths_t;

/*!
 * @brief Holds the lengths mode of a LoRa packet type
 */
typedef enum
{
    RADIO_LORA_PACKET_VARIABLE_LENGTH             = 0x00,         //!< The packet is on variable size, header included
    RADIO_LORA_PACKET_FIXED_LENGTH                = 0x01,         //!< The packet is known on both sides, no header included in the packet
    RADIO_LORA_PACKET_EXPLICIT                    = RADIO_LORA_PACKET_VARIABLE_LENGTH,
    RADIO_LORA_PACKET_IMPLICIT                    = RADIO_LORA_PACKET_FIXED_LENGTH,
}RADIO_LoRaPacketLengthsMode_t;

/*!
 * @brief Represents the CRC mode for LoRa packet type
 */
typedef enum
{
    RADIO_LORA_CRC_ON                             = 0x01,         //!< CRC activated
    RADIO_LORA_CRC_OFF                            = 0x00,         //!< CRC not used
}RADIO_LoRaCrcModes_t;

/*!
 * @brief Represents the IQ mode for LoRa packet type
 */
typedef enum
{
    RADIO_LORA_IQ_NORMAL                          = 0x00,
    RADIO_LORA_IQ_INVERTED                        = 0x01,
}RADIO_LoRaIQModes_t;

/*!
 * @brief Represents the IQ mode for LoRa packet type
 */
typedef enum
{
    RADIO_LORA_LOWDR_OPT_OFF                         = 0x00,  /*Forced to 0*/
    RADIO_LORA_LOWDR_OPT_ON                          = 0x01,  /*Forced to 1*/
    RADIO_LORA_LOWDR_OPT_AUTO                        = 0x02,  /*Forced to 1 when SF11 or SF12, 0 otherwise*/
}RADIO_Ld_Opt_t;

/*******************************************Radio FSK enum*****************************************/

/*!
 * @brief Represents the modulation shaping parameter
 */
typedef enum
{
    RADIO_FSK_MOD_SHAPING_OFF                         = 0x00,
    RADIO_FSK_MOD_SHAPING_G_BT_03                     = 0x08,
    RADIO_FSK_MOD_SHAPING_G_BT_05                     = 0x09,
    RADIO_FSK_MOD_SHAPING_G_BT_07                     = 0x0A,
    RADIO_FSK_MOD_SHAPING_G_BT_1                      = 0x0B,
}RADIO_FSK_ModShapings_t;

/*!
 * @brief Represents the preamble length used to detect the packet on Rx side
 */
typedef enum
{
    RADIO_FSK_PREAMBLE_DETECTOR_OFF             = 0x00,         //!< Preamble detection length off
    RADIO_FSK_PREAMBLE_DETECTOR_08_BITS         = 0x04,         //!< Preamble detection length 8 bits
    RADIO_FSK_PREAMBLE_DETECTOR_16_BITS         = 0x05,         //!< Preamble detection length 16 bits
    RADIO_FSK_PREAMBLE_DETECTOR_24_BITS         = 0x06,         //!< Preamble detection length 24 bits
    RADIO_FSK_PREAMBLE_DETECTOR_32_BITS         = 0x07,         //!< Preamble detection length 32 bit
}RADIO_FSK_PreambleDetection_t;

/*!
 * @brief Represents the possible combinations of SyncWord correlators activated
 */
typedef enum
{
    RADIO_FSK_ADDRESSCOMP_FILT_OFF              = 0x00,         //!< No correlator turned on, i.e. do not search for SyncWord
    RADIO_FSK_ADDRESSCOMP_FILT_NODE             = 0x01,
    RADIO_FSK_ADDRESSCOMP_FILT_NODE_BROAD       = 0x02,
}RADIO_FSK_AddressComp_t;

/*!
 *  @brief Radio packet length mode
 */
typedef enum
{
    RADIO_FSK_PACKET_FIXED_LENGTH               = 0x00,         //!< The packet is known on both sides, no header included in the packet
    RADIO_FSK_PACKET_VARIABLE_LENGTH            = 0x01,         //!< 1 byte  packet length field inserted after the sync word*/
    RADIO_FSK_PACKET_2BYTES_LENGTH              = 0x02          //!< 2 bytes packet length field inserted after the sync word, payload size greater than 255 bytes */
}RADIO_FSK_PacketLengthModes_t;

/*!
 * @brief Represents the CRC length
 */
typedef enum
{
    RADIO_FSK_CRC_OFF                           = 0x01,         //!< No CRC in use
    RADIO_FSK_CRC_1_BYTES                       = 0x00,
    RADIO_FSK_CRC_2_BYTES                       = 0x02,
    RADIO_FSK_CRC_1_BYTES_INV                   = 0x04,
    RADIO_FSK_CRC_2_BYTES_INV                   = 0x06,
    RADIO_FSK_CRC_2_BYTES_IBM                   = 0xF1,
    RADIO_FSK_CRC_2_BYTES_CCIT                  = 0xF2,
}RADIO_FSK_CrcTypes_t;

/*!
 * @brief Radio whitening mode Off, CCIT or ibm
 */
typedef enum
{
    RADIO_FSK_DC_FREE_OFF                       = 0x00, /*whitening Off*/
    RADIO_FSK_DC_FREEWHITENING                  = 0x01, /*whitening CCIT*/
    RADIO_FSK_DC_IBM_WHITENING                  = 0x02, /*whitening IBM*/
}RADIO_FSK_DcFree_t;

/*!
 * @brief Radio Lora generic Rx parameters
 */
typedef struct{
  uint32_t StopTimerOnPreambleDetect; /*0 inactive, otherwise active*/
  RADIO_LoRaSpreadingFactors_t SpreadingFactor;
  RADIO_LoRaBandwidths_t Bandwidth;
  RADIO_LoRaCodingRates_t Coderate;
  RADIO_Ld_Opt_t LowDatarateOptimize;/*0 inactive, 1 active, otherwise auto (active for SF11 and SF12)*/
  uint16_t PreambleLen;
  RADIO_LoRaPacketLengthsMode_t LengthMode;
  uint8_t MaxPayloadLength;
  RADIO_LoRaCrcModes_t CrcMode;
  RADIO_LoRaIQModes_t IqInverted;
} generic_param_rx_lora_t;

/*!
 * @brief Radio FSK generic Rx parameters
 */
typedef struct{
  uint32_t StopTimerOnPreambleDetect;
  RADIO_FSK_ModShapings_t ModulationShaping;
  uint32_t Bandwidth;
  uint32_t BitRate;                                   /* BitRate */
  uint32_t PreambleLen;                               /* Preamble length in Byte */
  RADIO_FSK_PreambleDetection_t PreambleMinDetect;
  uint8_t SyncWordLength;                             /* SyncWord Buffer length in Byte*/
  uint8_t* SyncWord;                                  /* SyncWord Buffer, 8 bytes max */
  uint32_t MaxPayloadLength;                          /* maximum Payload length to listen */
  uint16_t whiteSeed;                                 /* WhiteningSeed, whitening can also be disabled by setting this field to 0 */
  RADIO_FSK_AddressComp_t           AddrComp;
  RADIO_FSK_PacketLengthModes_t     LengthMode;       /* If the header is explicit, it will be transmitted in the GFSK packet. If the header is implicit, it will not be transmitted */
  RADIO_FSK_CrcTypes_t              CrcLength;        /* Size of the CRC block in the GFSK packet */
  uint16_t CrcPolynomial;                             /* Polynomial of the Crc*/
  uint16_t CrcSeed;                                   /* Seed of the Crc*/
  RADIO_FSK_DcFree_t                Whitening;
} generic_param_rx_fsk_t;

/*!
 * @brief Radio generic Rx Configuration
 */
typedef struct{
  generic_param_rx_fsk_t fsk;
  generic_param_rx_lora_t lora;
} RxConfigGeneric_t;

/*!
 * @brief Radio BPSK generic Tx parameters
 */
typedef struct{
  uint32_t BitRate;                                   /*BitRate*/
} generic_param_tx_bpsk_t;

/*!
 * @brief Radio Lora generic Tx parameters
 */
typedef struct{
  RADIO_LoRaSpreadingFactors_t SpreadingFactor;
  RADIO_LoRaBandwidths_t Bandwidth;
  RADIO_LoRaCodingRates_t Coderate;
  RADIO_Ld_Opt_t LowDatarateOptimize;                 /*0 inactive, otherwise active*/
  uint16_t PreambleLen;
  RADIO_LoRaPacketLengthsMode_t LengthMode;
  RADIO_LoRaCrcModes_t CrcMode;
  RADIO_LoRaIQModes_t IqInverted;
} generic_param_tx_lora_t;

/*!
 * @brief Radio FSK generic Tx parameters
 */
typedef struct{
  RADIO_FSK_ModShapings_t ModulationShaping;
  uint32_t Bandwidth;
  uint32_t BitRate;                                   /* BitRate */
  uint32_t FrequencyDeviation;                        /* FrequencyDeviation */
  uint32_t PreambleLen;                               /* in Byte */
  uint8_t SyncWordLength;                             /* in Byte */
  uint8_t* SyncWord;                                  /* SyncWord Buffer, 8 bytes max */
  uint16_t whiteSeed;                                 /* Whitening seed, whitening can be disabled by setting this field to 0 */
  RADIO_FSK_PacketLengthModes_t     HeaderType;       /* If the header is explicit, it will be transmitted in the GFSK packet. If the header is implicit, it will not be transmitted */
  RADIO_FSK_CrcTypes_t              CrcLength;        /* Size of the CRC block in the GFSK packet */
  uint16_t CrcPolynomial;
  uint16_t CrcSeed;
  RADIO_FSK_DcFree_t                Whitening;
} generic_param_tx_fsk_t;

/*!
 * @brief Radio generic Tx Configuration
 */
typedef struct{
  generic_param_tx_fsk_t fsk;
  generic_param_tx_lora_t lora;
  generic_param_tx_bpsk_t bpsk;
} TxConfigGeneric_t;

#ifdef __cplusplus
}
#endif

#endif // __RADIO_EX_H__

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
