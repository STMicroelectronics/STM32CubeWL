/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rf_api.h
  * @author  MCD Application Team
  * @brief   interface to rf_api.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/*!
\if SIGFOX PATTERN
----------------------------------------------

  _____   _   _____   _____   _____  __    __
 /  ___/ | | /  ___| |  ___| /  _  \ \ \  / /
 | |___  | | | |     | |__   | | | |  \ \/ /
 \___  \ | | | |  _  |  __|  | | | |   }  {
  ___| | | | | |_| | | |     | |_| |  / /\ \
 /_____/ |_| \_____/ |_|     \_____/ /_/  \_\

 ----------------------------------------------

   !!!!  DO NOT MODIFY THIS FILE !!!!

 ----------------------------------------------
 \endif
 ----------------------------------------------*/
/*!
 * \file rf_api.h
 * \brief Sigfox manufacturer functions
 * \author $(SIGFOX_LIB_AUTHOR)
 * \version $(SIGFOX_LIB_VERSION)
 * \date $(SIGFOX_LIB_DATE)
 * \copyright Copyright (c) 2011-2015 SIGFOX, All Rights Reserved. This is unpublished proprietary source code of SIGFOX.
 *
 * This file defines the manufacturer's RF functions to be implemented
 * for library usage.
 */
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __SIGFOX_RF_API_H__
#define __SIGFOX_RF_API_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/********************************************************
  * External API dependencies to link with this library.
  *
  * Error codes of the RF API functions are described below.
  * The Manufacturer can add more error code taking care of the limits defined.
  *
  ********************************************************/

/*!
  * \defgroup RF_ERR_API_xx codes Return Error codes definition for RF API
  *
  * \brief Can be customized to add new error codes.
  * All RF_API_ error codes will be piped with SIGFOX_API_xxx return code.<BR>
  * SFX_ERR_NONE implementation is mandatory for each MCU_API_xxx RF_API_xxx REPEATER_API_xxx or SE_API_xxx
  * functions.
  *
  *  @{
  */

/* ---------------------------------------------------------------- */
/* Bytes reserved for MCU API ERROR CODES : From 0x10 to 0x2F       */
/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/* Bytes reserved for RF API ERROR CODES : From 0x30 to 0x3F        */
/* ---------------------------------------------------------------- */
#define RF_ERR_API_INIT                          (sfx_u8)(0x30) /*!< Error on RF_API_init */
#define RF_ERR_API_SEND                          (sfx_u8)(0x31) /*!< Error on RF_API_send */
#define RF_ERR_API_CHANGE_FREQ                   (sfx_u8)(0x32) /*!< Error on RF_API_change_frequency */
#define RF_ERR_API_STOP                          (sfx_u8)(0x33) /*!< Error on RF_API_stop */
#define RF_ERR_API_WAIT_FRAME                    (sfx_u8)(0x34) /*!< Error on RF_API_wait_frame */
#define RF_ERR_API_WAIT_CLEAR_CHANNEL            (sfx_u8)(0x35) /*!< Error on RF_API_wait_for_clear_channel */
#define RF_ERR_API_START_CONTINUOUS_TRANSMISSION (sfx_u8)(0x36) /*!< Error on RF_API_start_continuous_transmission */
#define RF_ERR_API_STOP_CONTINUOUS_TRANSMISSION  (sfx_u8)(0x37) /*!< Error on RF_API_stop_continuous_transmission */
#define RF_ERR_API_GET_VERSION                   (sfx_u8)(0x38) /*!< Error on RF_API_get_version */
/* ---------------------------------------------------------------- */
/* Bytes reserved for SE API ERROR CODES : From 0x40 to 0x5F        */
/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/* Bytes reserved for REPEATER API ERROR CODES : From 0x60 to 0x7F  */
/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------- */
/* Bytes reserved for MONARCH API ERROR CODES : From 0x80 to 0x8F   */
/* ---------------------------------------------------------------- */

/* ---------------------------------------------------------------------- */
/* Bytes reserved for DEVICE CONFIG API ERROR CODES : From 0x90 to 0x9A   */
/* ---------------------------------------------------------------------- */

/** @}*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/

/*!******************************************************************
  * \fn sfx_u8 RF_API_init(sfx_rf_mode_t rf_mode)
  * \brief Init and configure Radio link in RX/TX
  *
  * [RX Configuration]
  * To receive Sigfox Frame on your device, program the following:
  *  - Preamble  : 0xAAAAAAAAA
  *  - Sync Word : 0xB227
  *  - Packet of the Sigfox frame is 15 bytes length.
  *
  * \param[in] sfx_rf_mode_t rf_mode         Init Radio link in Tx or RX
  * \param[out] none
  *
  * \retval SFX_ERR_NONE:             No error
  * \retval RF_ERR_API_INIT:          Init Radio link error
  *******************************************************************/
sfx_u8 RF_API_init(sfx_rf_mode_t rf_mode);

/*!******************************************************************
  * \fn sfx_u8 RF_API_stop(void)
  * \brief Close Radio link
  *
  * \param[in] none
  * \param[out] none
  *
  * \retval SFX_ERR_NONE:              No error
  * \retval RF_ERR_API_STOP:           Close Radio link error
  *******************************************************************/
sfx_u8 RF_API_stop(void);

/*!******************************************************************
  * \fn sfx_u8 RF_API_send(sfx_u8 *stream, sfx_modulation_type_t type, sfx_u8 size)
  * \brief BPSK Modulation of data stream
  * (from synchro bit field to CRC)
  *
  * NOTE : during this function, the voltage_tx needs to be retrieved and stored in
  *        a variable to be returned into the MCU_API_get_voltage_and_temperature or
  *        MCU_API_get_voltage functions.
  *
  * \param[in] sfx_u8 *stream                Complete stream to modulate
  * \param[in]sfx_modulation_type_t          Type of the modulation ( enum with baudrate and modulation information)
  * \param[in] sfx_u8 size                   Length of stream
  * \param[out] none
  *
  * \retval SFX_ERR_NONE:                    No error
  * \retval RF_ERR_API_SEND:                 Send data stream error
  *******************************************************************/
sfx_u8 RF_API_send(sfx_u8 *stream, sfx_modulation_type_t type, sfx_u8 size);

/*!******************************************************************
  * \fn sfx_u8 RF_API_start_continuous_transmission (sfx_modulation_type_t type)
  * \brief Generate a signal with modulation type. All the configuration ( Init of the RF and Frequency have already been executed
  *        when this function is called.
  *
  * \param[in] sfx_modulation_type_t         Type of the modulation ( enum with baudrate and modulation information is contained in sigfox_api.h)
  *
  * \retval SFX_ERR_NONE:                                 No error
  * \retval RF_ERR_API_START_CONTINUOUS_TRANSMISSION:     Continuous Transmission Start error
  *******************************************************************/
sfx_u8 RF_API_start_continuous_transmission(sfx_modulation_type_t type);

/*!******************************************************************
  * \fn sfx_u8 RF_API_stop_continuous_transmission (void)
  * \brief Stop the current continuous transmission
  *
  * \retval SFX_ERR_NONE:                                 No error
  * \retval RF_ERR_API_STOP_CONTINUOUS_TRANSMISSION:      Continuous Transmission Stop error
  *******************************************************************/
sfx_u8 RF_API_stop_continuous_transmission(void);

/*!******************************************************************
  * \fn sfx_u8 RF_API_change_frequency(sfx_u32 frequency)
  * \brief Change synthesizer carrier frequency
  *
  * \param[in] sfx_u32 frequency             Frequency in Hz to program in the radio chipset
  * \param[out] none
  *
  * \retval SFX_ERR_NONE:                    No error
  * \retval RF_ERR_API_CHANGE_FREQ:          Change frequency error
  *******************************************************************/
sfx_u8 RF_API_change_frequency(sfx_u32 frequency);

/*!******************************************************************
  * \fn sfx_u8 RF_API_wait_frame(sfx_u8 *frame, sfx_s16 *rssi, sfx_rx_state_enum_t * state)
  * \brief Get all GFSK frames received in Rx buffer, structure of
  * frame is : Synchro bit + Synchro frame + 15 Bytes.<BR> This function must
  * be blocking state since data is received or timer of 25 s has elapsed.
  *
  * - If received buffer, function returns SFX_ERR_NONE then the
  *   library will try to decode frame. If the frame is not correct, the
  *   library will recall RF_API_wait_frame.
  *
  * - If 25 seconds timer has elapsed, function returns into the state the timeout enum code.
  *   and then library will stop receive frame phase.
  *
  * \param[in] none
  * \param[out] sfx_s8 *frame                  Receive buffer
  * \param[out] sfx_s16 *rssi                  Chipset RSSI
  * Warning: This is the 'raw' RSSI value. Do not add 100 as made
  * in Library versions 1.x.x
  * Resolution: 1 LSB = 1 dBm
  *
  * \param[out] sfx_rx_state_enum_t state      Indicate the final state of the reception. Value can be DL_TIMEOUT or DL_PASSED
  *                                            if a frame has been received, as defined in sigfox_api.h file.
  *
  * \retval SFX_ERR_NONE:                      No error
  *******************************************************************/
sfx_u8 RF_API_wait_frame(sfx_u8 *frame, sfx_s16 *rssi, sfx_rx_state_enum_t *state);

/*!******************************************************************
  * \fn sfx_u8 RF_API_wait_for_clear_channel (sfx_u8 cs_min, sfx_s8 cs_threshold, sfx_rx_state_enum_t * state);
  * \brief This function is used in ARIB standard for the Listen Before Talk
  *        feature. It listens on a specific frequency band initialized through the RF_API_init(), during a sliding window set
  *        in the MCU_API_timer_start_carrier_sense().
  *        If the channel is clear during the minimum carrier sense
  *        value (cs_min), under the limit of the cs_threshold,
  *        the functions returns with SFX_ERR_NONE (transmission
  *        allowed). Otherwise it continues to listen to the channel till the expiration of the
  *        carrier sense maximum window and then updates the state ( with timeout enum ).
  *
  *
  * \param[in] sfx_u8 cs_min                  Minimum Carrier Sense time in ms.
  * \param[in] sfx_s8 cs_threshold            Power threshold limit to declare the channel clear.
  *                                            i.e : cs_threshold value -80dBm in Japan / -65dBm in Korea
  * \param[out] sfx_rx_state_enum_t state      Indicate the final state of the carrier sense. Value can be DL_TIMEOUT or PASSED
  *                                            as per defined in sigfox_api.h file.
  *
  * \retval SFX_ERR_NONE:                      No error
  *******************************************************************/
sfx_u8 RF_API_wait_for_clear_channel(sfx_u8 cs_min, sfx_s8 cs_threshold, sfx_rx_state_enum_t *state);

/*!******************************************************************
  * \fn sfx_u8 RF_API_get_version(sfx_u8 **version, sfx_u8 *size)
  * \brief Returns current RF API version
  *
  * \param[out] sfx_u8 **version                 Pointer to Byte array (ASCII format) containing library version
  * \param[out] sfx_u8 *size                     Size of the byte array pointed by *version
  *
  * \retval SFX_ERR_NONE:                No error
  * \retval RF_ERR_API_GET_VERSION:      Get Version error
  *******************************************************************/
sfx_u8 RF_API_get_version(sfx_u8 **version, sfx_u8 *size);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*__SIGFOX_RF_API_H__*/
