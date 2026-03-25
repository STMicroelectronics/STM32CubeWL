/**
 * @file      sx126x_regs.h
 *
 * @brief     SX126x register definitions
 *
 * The Clear BSD License
 * Copyright Semtech Corporation 2021. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef SX126X_REGS_H
#define SX126X_REGS_H

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/**
 * @brief The address of the register holding the first byte defining the CRC seed
 */
#define REG_LR_CRCSEEDBASEADDR                      0x06BC

/**
 * @brief The address of the register holding the first byte defining the CRC polynomial
 */
#define REG_LR_CRCPOLYBASEADDR                      0x06BE

/**
 * @brief The address of the register holding the first byte defining the whitening seed
 */
#define REG_LR_WHITSEEDBASEADDR_MSB                 0x06B8
#define REG_LR_WHITSEEDBASEADDR_LSB                 0x06B9

/**
 * @brief The addresses of the registers holding SyncWords values
 */
#define REG_LR_SYNCWORDBASEADDRESS 0x06C0

/**
 * @brief The addresses of the register holding LoRa Modem SyncWord value
 *        0x1424: LoRaWAN private network,
 *        0x3444: LoRaWAN public network
 */
#define REG_LR_SYNCWORD 0x0740

/**
 * @brief The address of the register holding the coding rate configuration extracted from a received LoRa header
 */
#define REG_LR_HEADER_CR 0x0749
#define REG_LR_HEADER_CR_POS ( 4U )
#define REG_LR_HEADER_CR_MASK ( 0x07UL << REG_LR_HEADER_CR_POS )

/**
 * @brief The address of the register holding the CRC configuration extracted from a received LoRa header
 */
#define REG_LR_HEADER_CRC 0x076B
#define REG_LR_HEADER_CRC_POS ( 4U )
#define REG_LR_HEADER_CRC_MASK ( 0x01UL << REG_LR_HEADER_CRC_POS )

/*!
 * The address of the register giving a 32-bit random number
 */
#define REG_RNGBASEADDRESS 0x0819

/*!
 * The address of the register used to disable the LNA
 */
#define REG_ANA_LNA 0x08E2

/*!
 * The address of the register used to disable the mixer
 */
#define REG_ANA_MIXER 0x08E5

/*!
 * The address of the register holding RX Gain value
 *     0x94: power saving,
 *     0x96: rx boosted
 */
#define REG_RXGAIN 0x08AC

/*!
 * \brief The address of the register holding Bit Sync configuration
 */
#define REG_BIT_SYNC                                0x06AC

/*!
 * \brief The address of the register holding the Tx boost value
 */
#define REG_DRV_CTRL                                0x091F

/**
 * @brief Change the value on the device internal trimming capacitor
 */
#define REG_XTATRIM 0x0911
#define REG_XTBTRIM 0x0912

/**
 * @brief Set the current max value in the over current protection
 */
#define REG_OCP 0x08E7

/**
 * @brief WORKAROUND - Optimizing the Inverted IQ Operation, see DS_SX1261-2_V1.2 datasheet chapter 15.4
 */
#define REG_IQ_POLARITY 0x0736

/**
 * @brief WORKAROUND - Modulation Quality with 500 kHz LoRa Bandwidth, see DS_SX1261-2_V1.2 datasheet chapter 15.1
 */
#define REG_TX_MODULATION 0x0889

/**
 * @brief WORKAROUND - Better resistance to antenna mismatch, see DS_SX1261-2_V1.2 datasheet chapter 15.2
 */
#define REG_TX_CLAMP_CFG 0x08D8
#define REG_TX_CLAMP_CFG_POS ( 1U )
#define REG_TX_CLAMP_CFG_MASK ( 0x0FUL << REG_TX_CLAMP_CFG_POS )

/**
 * @brief RTC control
 */
#define REG_RTC_CTRL 0x0902

/**
 * @brief Event clear
 */
#define REG_EVT_CLR 0x0944
#define REG_EVT_CLR_TIMEOUT_POS ( 1U )
#define REG_EVT_CLR_TIMEOUT_MASK ( 0x01UL << REG_EVT_CLR_TIMEOUT_POS )

/**
 * @brief RX address pointer
 */
#define REG_RX_ADDRESS_POINTER 0x0803

/**
 * @brief RX/TX payload length
 */
#define REG_RXTX_PAYLOAD_LEN 0x06BB

/**
 * @brief Output disable
 */
#define REG_OUT_DIS_REG 0x0580
#define REG_OUT_DIS_REG_DIO3_POS ( 3U )
#define REG_OUT_DIS_REG_DIO3_MASK ( 0x01UL << REG_OUT_DIS_REG_DIO3_POS )

/**
 * @brief Input enable
 */
#define REG_IN_EN_REG 0x0583
#define REG_IN_EN_REG_DIO3_POS ( 3U )
#define REG_IN_EN_REG_DIO3_MASK ( 0x01UL << REG_IN_EN_REG_DIO3_POS )

/**
 * @brief TX bitbang A
 */
#define REG_BITBANG_A_REG 0x0680
#define REG_BITBANG_A_REG_ENABLE_POS ( 4U )
#define REG_BITBANG_A_REG_ENABLE_MASK ( 0x07UL << REG_BITBANG_A_REG_ENABLE_POS )
#define REG_BITBANG_A_REG_ENABLE_VAL ( 0x01UL << REG_BITBANG_A_REG_ENABLE_POS )

/**
 * @brief TX bitbang B
 */
#define REG_BITBANG_B_REG 0x0587
#define REG_BITBANG_B_REG_ENABLE_POS ( 0U )
#define REG_BITBANG_B_REG_ENABLE_MASK ( 0x0FUL << REG_BITBANG_B_REG_ENABLE_POS )
#define REG_BITBANG_B_REG_ENABLE_VAL ( 0x0CUL << REG_BITBANG_B_REG_ENABLE_POS )

/**
 * @brief Number of symbols given as REG_LR_SYNCH_TIMEOUT[7:3] * 2 ^ (2*REG_LR_SYNCH_TIMEOUT[2:0] + 1)
 */
#define REG_LR_SYNCH_TIMEOUT 0x0706

/**
 * @brief Base address of the register retention list
 */
#define REG_RETENTION_LIST_BASE_ADDRESS 0x029F

/**
 * @brief GFSK node address
 *
 * @remark Reset value is 0x00
 */
#define REG_GFSK_NODE_ADDRESS 0x06CD

/**
 * @brief GFSK broadcast address
 *
 * @remark Reset value is 0x00
 */
#define REG_GFSK_BROADCAST_ADDRESS 0x06CE

/**
  * @brief  Sub-GHz radio RAM definition
  * @note   The sub-GHz radio peripheral RAM address can be accessed by sub-GHz radio command
  *         SUBGRF_WriteRegisters() and SUBGRF_ReadRegisters() "
  * @note   These RAM addresses are used to control accurately ramp-up, ramp-down and length of a frame
  */
/*Sub-GHz radio Ramp Up High register*/
#define SUBGHZ_RAM_RAMPUPH                          0x00F0
/*Sub-GHz radio Ramp Up Low register*/
#define SUBGHZ_RAM_RAMPUPL                          0x00F1
/*Sub-GHz radio Ramp Down High register*/
#define SUBGHZ_RAM_RAMPDNH                          0x00F2
/*Sub-GHz radio Ramp Down Low register*/
#define SUBGHZ_RAM_RAMPDNL                          0x00F3
/*Sub-GHz radio frame limit High register*/
#define SUBGHZ_RAM_FRAMELIMH                        0x00F4
/*Sub-GHz radio frame limit Low register*/
#define SUBGHZ_RAM_FRAMELIML                        0x00F5

/**
  * @brief  Sub-GHz radio register (re) definition
  * @note   The sub-GHz radio peripheral registers can be accessed by sub-GHz radio command
  *         SUBGRF_WriteRegisters() and SUBGRF_ReadRegisters() "
  */

/*Sub-GHz radio generic bit synchronization register*/
#define SUBGHZ_GBSYNCR                              REG_BIT_SYNC
/*Sub-GHz radio generic CFO High register */
#define SUBGHZ_GCFORH                               0x06B0
/*Sub-GHz radio generic CFO Low register */
#define SUBGHZ_GCFORL                               0x06B1
/*Sub-GHz radio generic pktCtl1 register*/
#define SUBGHZ_GPKTCTL1R                            0x06B4
/*Sub-GHz radio generic packet control 1A register*/
#define SUBGHZ_GPKTCTL1AR                           REG_LR_WHITSEEDBASEADDR_MSB
/*Sub-GHz radio generic whitening LSB register*/
#define SUBGHZ_GWHITEINIRL                          REG_LR_WHITSEEDBASEADDR_LSB
/*Sub-GHz radio generic rtx register*/
#define SUBGHZ_GRTXPLDLEN                           0x06BB
/*Sub-GHz radio generic CRC initial MSB register*/
#define SUBGHZ_GCRCINIRH                            REG_LR_CRCSEEDBASEADDR
/*Sub-GHz radio generic CRC initial LSB register*/
#define SUBGHZ_GCRCINIRL                            0x06BD
/*Sub-GHz radio generic CRC polynomial MSB register*/
#define SUBGHZ_GCRCPOLRH                            REG_LR_CRCPOLYBASEADDR
/*Sub-GHz radio generic CRC polynomial LSB register*/
#define SUBGHZ_GCRCPOLRL                            0x06BF
/*Sub-GHz radio generic synchronization word control register 0*/
#define SUBGHZ_GSYNCR0                              REG_LR_SYNCWORDBASEADDRESS
/*Sub-GHz radio generic synchronization word control register 1*/
#define SUBGHZ_GSYNCR1                              0x06C1
/*Sub-GHz radio generic synchronization word control register 2*/
#define SUBGHZ_GSYNCR2                              0x06C2
/*Sub-GHz radio generic synchronization word control register 3*/
#define SUBGHZ_GSYNCR3                              0x06C3
/*Sub-GHz radio generic synchronization word control register 4*/
#define SUBGHZ_GSYNCR4                              0x06C4
/*Sub-GHz radio generic synchronization word control register 5*/
#define SUBGHZ_GSYNCR5                              0x06C5
/*Sub-GHz radio generic synchronization word control register 6*/
#define SUBGHZ_GSYNCR6                              0x06C6
/*Sub-GHz radio generic synchronization word control register 7*/
#define SUBGHZ_GSYNCR7                              0x06C7
/*Sub-GHz radio generic node address register*/
#define SUBGHZ_GNODEADDR                            0x06CD
/*Sub-GHz radio generic broadacst address register*/
#define SUBGHZ_GBCASTADDR                           0x06CE
/*Sub-GHz radio generic Afc register*/
#define SUBGHZ_GAFCR                                0x06D1
/*Sub-GHz radio Lora Payload Length */
#define SUBGHZ_LPLDLENR                             REG_LR_PAYLOADLENGTH
/*Sub-GHz radio Lora synchro timeout */
#define SUBGHZ_LSYNCTIMEOUTR                        REG_LR_SYNCH_TIMEOUT
/*Sub-GHz radio Lora IQ polarity register*/
#define SUBGHZ_LIQPOLR                              0x0736
/*Sub-GHz radio LoRa synchronization word MSB register*/
#define SUBGHZ_LSYNCRH                              REG_LR_SYNCWORD
/*Sub-GHz radio LoRa synchronization word LSB register*/
#define SUBGHZ_LSYNCRL                              0x0741
/*Sub-GHz radio txAddrPtr register*/
#define SUBGHZ_TXADRPTR                             0x0802
/*Sub-GHz radio rxAddrPtr register*/
#define SUBGHZ_RXADRPTR                             0x0803
/*Sub-GHz radio Rx Bandwidth selector register */
#define SUBGHZ_BWSELR                               0x0807
/*Sub-GHz radio random number register 3*/
#define SUBGHZ_RNGR3                                RANDOM_NUMBER_GENERATORBASEADDR
/*Sub-GHz radio  random number register 2*/
#define SUBGHZ_RNGR2                                0x081A
/*Sub-GHz radio  random number register 1*/
#define SUBGHZ_RNGR1                                0x081B
/*Sub-GHz radio  random number register 0*/
#define SUBGHZ_RNGR0                                0x081C
/*Sub-GHz radio SD resolution*/
#define SUBGHZ_SDCFG0R                              0x0889
/*Sub-GHz radio Agc Gfo Reset Rssi Control register*/
#define SUBGHZ_AGCRSSICTL0R                         0x089B
/*Sub-GHz radio Agc LoRa register*/
#define SUBGHZ_AGCCFG                               0x08A3
/*Sub-GHz radio receiver gain control register*/
#define SUBGHZ_RXGAINCR                             REG_RX_GAIN
/*Sub-GHz radio Agc Gfo Reset Config register*/
#define SUBGHZ_AGCGFORSTCFGR                        0x08B8
/*Sub-GHz radio Agc Gfo Reset Power Threshold register*/
#define SUBGHZ_AGCGFORSTPOWTHR                      0x08B9
/*Sub-GHz radio Tx clamp register*/
#define SUBGHZ_TXCLAMPR                             REG_TX_CLAMP
/*Sub-GHz radio PA over current protection register*/
#define SUBGHZ_PAOCPR                               REG_OCP
/*Sub-GHz radio rtc control register*/
#define SUBGHZ_RTCCTLR                              0x0902
/*Sub-GHz radio rtc period register*/
#define SUBGHZ_RTCPRDR2                             0x0903
#define SUBGHZ_RTCPRDR1                             0x0904
#define SUBGHZ_RTCPRDR0                             0x0905
/*Sub-GHz radio HSE32 OSC_IN capacitor trim register*/
#define SUBGHZ_HSEINTRIMR                           REG_XTA_TRIM
/*Sub-GHz radio HSE32 OSC_OUT capacitor trim register*/
#define SUBGHZ_HSEOUTTRIMR                          REG_XTB_TRIM
/*Sub-GHz radio SMPS control 0 register */
#define SUBGHZ_SMPSC0R                              0x0916
/*Sub-GHz radio power control register*/
#define SUBGHZ_PCR                                  0x091A
/*Sub-GHz radio SMPS control 2 register */
#define SUBGHZ_SMPSC2R                              0x0923
/*Sub-GHz event mask register*/
#define SUBGHZ_EVENTMASKR                           0x0944



/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

#endif  // SX126X_REGS_H

/* --- EOF ------------------------------------------------------------------ */
