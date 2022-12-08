/*!
 * \file      radio_def.h
 *
 * \brief     Radio driver API definition type definition
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
/*!
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    radio_def.h
  * @author  MCD Application Team
  * @brief   Radio driver API definition
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RADIO_DEF_H__
#define __RADIO_DEF_H__

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>

/* Public typedef -----------------------------------------------------------*/

/*!
 * API return status
 */
typedef enum radio_status_e
{
    RADIO_STATUS_OK,
    RADIO_STATUS_UNSUPPORTED_FEATURE,
    RADIO_STATUS_UNKNOWN_VALUE,
    RADIO_STATUS_ERROR,
} radio_status_t;

/*!
 * Radio driver supported modems
 */
typedef enum
{
    MODEM_FSK = 0,
    MODEM_LORA,
    MODEM_MSK,
    MODEM_BPSK,
    MODEM_SIGFOX_TX,
    MODEM_SIGFOX_RX,
}RadioModems_t;

/*!
 * \brief Radio driver callback functions
 */
typedef struct
{
    /*!
     * \brief  Tx Done callback prototype.
     */
    void    ( *TxDone )( void );
    /*!
     * \brief  Tx Timeout callback prototype.
     */
    void    ( *TxTimeout )( void );
    /*!
     * \brief Rx Done callback prototype.
     *
     * \param [in] payload Received buffer pointer
     * \param [in] size    Received buffer size
     * \param [in] rssi    RSSI value computed while receiving the frame [dBm]
     * \param [in] LoraSnr_FskCfo
     *                     FSK : Carrier Frequency Offset in kHz
     *                     LoRa: SNR value in dB
     */
    void    ( *RxDone )( uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo );
    /*!
     * \brief  Rx Timeout callback prototype.
     */
    void    ( *RxTimeout )( void );
    /*!
     * \brief Rx Error callback prototype.
     */
    void    ( *RxError )( void );
    /*!
     * \brief  FHSS Change Channel callback prototype.
     *
     * \param [in] currentChannel   Index number of the current channel
     */
    void ( *FhssChangeChannel )( uint8_t currentChannel );
    /*!
     * \brief CAD Done callback prototype.
     *
     * \param [in] channelDetected    Channel Activity detected during the CAD
     */
    void ( *CadDone ) ( bool channelActivityDetected );
}RadioEvents_t;

#ifdef __cplusplus
}
#endif

#endif // __RADIO_DEF_H__
