/**
  ******************************************************************************
  * @file    radio_fw.h
  * @author  MCD Application Team
  * @brief   Extends radio capabilities (whitening, long packet)
   ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
/* Exported types ------------------------------------------------------------*/

/*reserved for SubGHz_Phy internal MW communication*/
typedef enum
{
    CONFIG_RX = 0,
    CONFIG_TX,
}ConfigGenericRTx_t;

typedef struct{
  TxConfigGeneric_t* TxConfig;
  RxConfigGeneric_t* RxConfig;
  ConfigGenericRTx_t rtx;
} ConfigGeneric_t;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

/*!
 * @brief Initialise the RFW module and enables custom  whithing, optionally long packet feature
 *
 * @param [IN] config        rx or tx config from the application
 * @param [IN] RadioEvents from the radio
 * @return 0 when no parameters error, -1 otherwise
 */
int32_t RFW_Init( ConfigGeneric_t* config, RadioEvents_t* RadioEvents, TimerEvent_t* TimeoutTimerEvent);

/*!
 * @brief Return whether the RFW module is enabled
 *
 * @return 0 when not initialised, -1 otherwise
 */
uint8_t RFW_Is_Init( void);

/*!
 * @brief Return whether the RFW module long packet is enabled
 *
 * @return 0 when not initialised, -1 otherwise
 */
uint8_t RFW_Is_LongPacketModeEnabled( void);

/*!
 * @brief Return whether the RFW module long packet is enabled
 *
 * @param [IN] RadioModems_t set in the radio
 */
void RFW_SetRadioModem(RadioModems_t Modem);

/*!
 * @brief DeInitialise the RFW module and enable custom  whithing and optionally long packet feature
 *
 */
void RFW_DeInit( void);

/*!
 * @brief DeInitialise the TxLongPacket
 *
 */
void RFW_DeInit_TxLongPacket(void);

/*!
 * @brief Set antenna switch output to be used in Tx
 *
 * @param [IN] AntSwitch  RFO_LP or FRO_HP
 *
 */
void RFW_SetAntSwitch( uint8_t AntSwitch);

/*!
 * @brief Initialise reception for IBM whitening case
 *
 */
int32_t RFW_ReceiveInit( void );

/*!
 * @brief Initialise transmission for IBM whitening case
 *
 */
int32_t RFW_TransmitInit(uint8_t* inOutBuffer, uint8_t size, uint8_t* outSize);

/*!
 * @brief Starts receiving payload. Called at Rx Sync IRQ
 *
 */
void RFW_ReceivePayload(void );

/*!
 * @brief Starts transmitting long Packet, note packet length may be on 1 bytes depending on config
 *
 * @param [IN] payload_size        total payload size to be sent
 * @param [IN] TxLongPacketGetNextChunkCb   callback to be implemented on user side to feed partial chunk
 *                                  buffer: source buffer allocated by the app
 *                                  size: size in bytes to feed. User to implement the offset based on previous chunk request
 * @return 0 when no parameters error, -1 otherwise
 */
int32_t RFW_TransmitLongPacket( uint16_t payload_size, uint32_t timeout, void (*TxLongPacketGetNextChunkCb) (uint8_t** buffer, uint8_t buffer_size) );

/*!
 * @brief Starts receiving long Packet, packet maybe short
 *
 * @param [IN] boosted_mode        boosted_mode: 0 normal Rx, 1:improved sensitivity
 * @param [IN] timeout             Reception timeout [ms]
 * @param [IN] RxLongStorePacketChunkCb   callback to be implemented on user side to record partial chunk in the application 
 *                                  buffer: source buffer allocated in the radio driver
 *                                  size: size in bytes to record
 * @return 0 when no parameters error, -1 otherwise
 */
int32_t RFW_ReceiveLongPacket( uint8_t boosted_mode, uint32_t timeout, void (*RxLongStorePacketChunkCb) (uint8_t* buffer, uint8_t chunk_size) );

#ifdef __cplusplus
}
#endif

#endif /*__RADIO_FW_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
