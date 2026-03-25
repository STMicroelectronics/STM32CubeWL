/**
  ******************************************************************************
  * @file    radio_fw.h
  * @author  MCD Application Team
  * @brief   Extends radio capabilities (whitening, long packet)
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RADIO_FW_H__
#define __RADIO_FW_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "radio_def.h"
#include "radio_ex.h"

/* Exported types ------------------------------------------------------------*/

/*reserved for SubGHz_Phy internal MW communication*/
typedef enum
{
    CONFIG_RX = 0,
    CONFIG_TX,
} ConfigGenericRTx_t;

typedef struct
{
    TxConfigGeneric_t *TxConfig;
    RxConfigGeneric_t *RxConfig;
    ConfigGenericRTx_t rtx;
} ConfigGeneric_t;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

/*!
 * @brief Initialise the RFW module and enables custom  whitening, optionally long packet feature
 *
 * @param [in] config             rx or tx config from the application
 * @param [in] RadioEvents        from the radio
 * @param [in] TimeoutTimerEvent  Timer for Rx or Tx timeout event
 * @return 0 when no parameters error, -1 otherwise
 */
int32_t RFW_Init( ConfigGeneric_t *config, RadioEvents_t *RadioEvents, TimerEvent_t *TimeoutTimerEvent );

/*!
 * @brief Return whether the RFW module is enabled
 *
 * @return 0 when not initialised, -1 otherwise
 */
uint8_t RFW_Is_Init( void );

/*!
 * @brief Return whether the RFW module long packet is enabled
 *
 * @return 0 when not initialised, -1 otherwise
 */
uint8_t RFW_Is_LongPacketModeEnabled( void );

/*!
 * @brief Return whether the RFW module long packet is enabled
 *
 * @param [in] Modem set in the radio
 */
void RFW_SetRadioModem( RadioModems_t Modem );

/*!
 * @brief DeInitialise the RFW module and enable custom  whitening and optionally long packet feature
 *
 */
void RFW_DeInit( void );

/*!
 * @brief DeInitialise the TxLongPacket
 *
 */
void RFW_DeInit_TxLongPacket( void );

/*!
 * @brief Set antenna switch output to be used in Tx
 *
 * @param [in] AntSwitch  RFO_LP or FRO_HP
 *
 */
void RFW_SetAntSwitch( uint8_t AntSwitch );

/*!
 * @brief Initialise reception for IBM whitening case
 *
 * @return 0 when RFW_ENABLE exists, -1 otherwise
 */
int32_t RFW_ReceiveInit( void );

/*!
 * @brief Initialise transmission for IBM whitening case
 *
 * @param [in,out] inOutBuffer pointer of exchange buffer to send or receive data
 * @param [in]     size input buffer size
 * @param [out]    outSize output buffer size
 *
 */
int32_t RFW_TransmitInit( uint8_t *inOutBuffer, uint8_t size, uint8_t *outSize );

/*!
 * @brief Starts receiving payload. Called at Rx Sync IRQ
 *
 */
void RFW_ReceivePayload( void );

/*!
 * @brief Starts transmitting long Packet, note packet length may be on 1 bytes depending on config
 *
 * @param [in] payload_size        total payload size to be sent
 * @param [in] timeout             Reception timeout [ms]
 * @param [in] TxLongPacketGetNextChunkCb   callback to be implemented on user side to feed partial chunk
 *                                  buffer: source buffer allocated by the app
 *                                  size: size in bytes to feed. User to implement the offset based on previous chunk request
 * @return 0 when no parameters error, -1 otherwise
 */
int32_t RFW_TransmitLongPacket( uint16_t payload_size, uint32_t timeout, void ( *TxLongPacketGetNextChunkCb )( uint8_t **buffer, uint8_t buffer_size ) );

/*!
 * @brief Starts receiving long Packet, packet maybe short
 *
 * @param [in] boosted_mode        boosted_mode: 0 normal Rx, 1:improved sensitivity
 * @param [in] timeout             Reception timeout [ms]
 * @param [in] RxLongStorePacketChunkCb   callback to be implemented on user side to record partial chunk in the application
 *                                  buffer: source buffer allocated in the radio driver
 *                                  size: size in bytes to record
 * @return 0 when no parameters error, -1 otherwise
 */
int32_t RFW_ReceiveLongPacket( uint8_t boosted_mode, uint32_t timeout, void ( *RxLongStorePacketChunkCb )( uint8_t *buffer, uint8_t chunk_size ) );

#ifdef __cplusplus
}
#endif

#endif /*__RADIO_FW_H__*/
