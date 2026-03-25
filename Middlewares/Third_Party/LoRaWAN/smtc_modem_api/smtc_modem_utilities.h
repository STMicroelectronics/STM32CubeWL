/**
 * @file      smtc_modem_utilities.h
 *
 * @brief   modem utilities
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

#ifndef SMTC_MODEM_UTILITIES_H__
#define SMTC_MODEM_UTILITIES_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>   // C99 types
#include <stdbool.h>  // bool type

#include "smtc_modem_api.h"

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

/**
 * @brief Modem context type enum
 */
typedef enum
{
    CONTEXT_MODEM,
    CONTEXT_KEY_MODEM,
    CONTEXT_LORAWAN_STACK,
    CONTEXT_FUOTA,
    CONTEXT_SECURE_ELEMENT,
    CONTEXT_STORE_AND_FORWARD,
} modem_context_type_t;

/*!
 * \brief User Handler callbacks
 */
typedef struct Callbacks_s{

  /*!
   * Callback event raised internally by the stack
   */
  void ( *EventCallback )( void );

  /*!
   * Restore the NVM Data context from the Flash
   *
 * @param [in]  ctx_type   Type of modem context that need to be restored
 * @param [in]  offset     Memory offset after ctx_type address
 * @param [out] buffer     Buffer pointer to write to
 * @param [in]  size       Buffer size to read in bytes
 */
  void ( *RestoreContext )( const modem_context_type_t ctx_type, uint32_t offset, uint8_t* buffer, const uint32_t size );
  /*!
   * Store the NVM Data context to the Flash
   *
 * @param [in] ctx_type   Type of modem context that need to be saved
 * @param [in] offset     Memory offset after ctx_type address
 * @param [in] buffer     Buffer pointer to write from
 * @param [in] size       Buffer size to write in bytes
 */
  void ( *StoreContext )( const modem_context_type_t ctx_type, uint32_t offset, const uint8_t* buffer, const uint32_t size );
  /*!
   * Get Random value using the RNG module
   *
   * \retval value  Return the random value
   */
  uint32_t ( *GetRandomValue )( void );
  /*!
   * Get the current battery level
   *
   * \retval value  Battery level ( 0: very low, 254: fully charged )
   */
  uint8_t ( *GetBatteryLevel )( void );
  /*!
   * Get the current temperature
   *
   * \retval value  Temperature in degree Celsius
   */
  int16_t ( *GetTemperatureLevel )( void );
  /*!
   * Will be called to reset the system
   * \note Compliance test protocol callbacks used when TS001-1.0.4 + TS009 1.0.0 are defined
   */
  void ( *SystemReset )( void );
}Callbacks_t;

/*!
 * Upper layer callbacks
 */
extern Callbacks_t *HandlerCallbacks;

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

/**
 * @brief Init the soft modem and set the structure of callbacks
 * @remark The callback will be called each time an modem event is raised internally or
 * another callback is called
 *
 * @param [in] handlerCallbacks User callbacks
 */
void smtc_modem_init( Callbacks_t *handlerCallbacks );

/**
 * @brief Run the modem engine
 * @remark This function must be called in main loop. It returns an amount of ms after which the function must at least
 * be called again
 *
 * @return  the time in ms after which the function must at least be called again
 */
uint32_t smtc_modem_run_engine( void );

/**
 * @brief Check if some modem irq flags are pending
 *
 * @return true if some flags are pending, false otherwise
 */
bool smtc_modem_is_irq_flag_pending( void );

/**
 * @brief Set optional user radio context that can be retrieved in radio drivers hal calls
 *
 * @param [in] radio_ctx pointer on context
 */
void smtc_modem_set_radio_context( const void* radio_ctx );

/**
 * @brief Get optional user radio context
 *
 * @returns Radio context reference
 */
const void* smtc_modem_get_radio_context( void );

/**
 * @brief Get optional user radio ral
 *
 * @returns Radio ral reference
 */
const void* smtc_modem_get_radio_ral( void );

#ifdef __cplusplus
}
#endif

#endif  // SMTC_MODEM_UTILITIES_H__

/* --- EOF ------------------------------------------------------------------ */
