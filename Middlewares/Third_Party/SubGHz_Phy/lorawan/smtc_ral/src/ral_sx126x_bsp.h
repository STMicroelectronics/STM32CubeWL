/**
 * @file      ral_sx126x_bsp.h
 *
 * @brief     Board Support Package for the SX126x-specific RAL.
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

#ifndef RAL_SX126X_BSP_H
#define RAL_SX126X_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>
#include "ral_defs.h"
#include "sx126x.h"

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

typedef struct ral_sx126x_bsp_tx_cfg_input_params_s
{
    int8_t   system_output_pwr_in_dbm;
    uint32_t freq_in_hz;
} ral_sx126x_bsp_tx_cfg_input_params_t;

typedef struct ral_sx126x_bsp_tx_cfg_output_params_s
{
    sx126x_pa_cfg_params_t pa_cfg;
    sx126x_ramp_time_t     pa_ramp_time;
    int8_t                 chip_output_pwr_in_dbm_configured;
    int8_t                 chip_output_pwr_in_dbm_expected;
} ral_sx126x_bsp_tx_cfg_output_params_t;

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

/**
 * Get the regulator mode configuration
 *
 * @param [in] context Chip implementation context
 * @param [out] reg_mode
 */
//void ral_sx126x_bsp_get_reg_mode( const void* context, sx126x_reg_mod_t* reg_mode );


static inline void ral_sx126x_bsp_get_reg_mode( const void* context, sx126x_reg_mod_t* reg_mode )
{
    if ( ( 1UL == RBI_IsDCDC() ) && ( 1UL == DCDC_ENABLE ) )
    {
    	*reg_mode = REG_MODE_DCDC ;
    }
    else
    {
    	*reg_mode = REG_MODE_LDO ;
    }
}

/**
 * Get the internal RF switch configuration
 *
 * @param [in] context Chip implementation context
 * @param [out] dio2_is_set_as_rf_switch
 */
//void ral_sx126x_bsp_get_rf_switch_cfg( const void* context, bool* dio2_is_set_as_rf_switch );

/**
 * Get the Tx-related configuration (power amplifier configuration, output power and ramp time) to be applied to the
 * chip
 *
 * @param [in] context Chip implementation context
 * @param [in] input_params Parameters used to compute the chip configuration
 * @param [out] output_params Parameters to be configured in the chip
 */
//void ral_sx126x_bsp_get_tx_cfg( const void* context, const ral_sx126x_bsp_tx_cfg_input_params_t* input_params,
//                                ral_sx126x_bsp_tx_cfg_output_params_t* output_params );

static inline void ral_sx126x_bsp_get_tx_cfg( const void* context, const ral_sx126x_bsp_tx_cfg_input_params_t* input_params,
                                ral_sx126x_bsp_tx_cfg_output_params_t* output_params )
{

    int16_t power = input_params->system_output_pwr_in_dbm;  // + board_tx_pwr_offset_db;

    output_params->pa_ramp_time  = RAMP_40_US;
    output_params->pa_cfg.pa_lut = 0x01;  // reserved value, same for sx1261 sx1262 and sx1268

    //SetRfTxPower (power); without settxpower
    uint8_t paSelect= RFO_LP;

	int32_t TxConfig = RBI_GetTxConfig();

	int32_t max_power;

	switch (TxConfig)
	{
		case RBI_CONF_RFO_LP_HP:
		{
			if (power > 15)
			{
				paSelect = RFO_HP;
			}
			else
			{
				paSelect = RFO_LP;
			}
			break;
		}
		case RBI_CONF_RFO_LP:
		{
			paSelect = RFO_LP;
			break;
		}
		case RBI_CONF_RFO_HP:
		{
			paSelect = RFO_HP;
			break;
		}
		default:
			break;
	}

	//SetTxParams
    if (paSelect == RFO_LP)
    {
        max_power = RBI_GetRFOMaxPowerConfig(RBI_RFO_LP_MAXPOWER);
        if (power >  max_power)
        {
          power = max_power;
        }
        if (max_power == 14)
        {
            output_params->pa_cfg.device_sel                 = 0x01;  // select SX1261 device
            output_params->pa_cfg.hp_max                     = 0x00;  // not used on sx1261
            output_params->pa_cfg.pa_duty_cycle              = 0x04;
            power = 0x0E - (max_power - power);
        }
        else if (max_power == 10)
        {
            output_params->pa_cfg.device_sel                 = 0x01;  // select SX1261 device
            output_params->pa_cfg.hp_max                     = 0x00;  // not used on sx1261
            output_params->pa_cfg.pa_duty_cycle              = 0x01;
            power = 0x0D - (max_power - power);
        }
        else /*default 15dBm*/
        {
            output_params->pa_cfg.device_sel                 = 0x01;  // select SX1261 device
            output_params->pa_cfg.hp_max                     = 0x00;  // not used on sx1261
            output_params->pa_cfg.pa_duty_cycle              = 0x07;
            power = 0x0E - (max_power - power);
        }
        if (power < -17)
        {
            power = -17;
        }
    }
    else /* rfo_hp*/
    {
        /* WORKAROUND - Better Resistance of the RFO High Power Tx to Antenna Mismatch, see STM32WL Erratasheet*/
    	  sx126x_cfg_tx_clamp( context );
        /* WORKAROUND END*/
        max_power = RBI_GetRFOMaxPowerConfig(RBI_RFO_HP_MAXPOWER);
        if (power > max_power)
        {
            power = max_power;
        }
        if (max_power == 20)
        {
            output_params->pa_cfg.device_sel                 = 0x00;  // select SX1261 device
            output_params->pa_cfg.hp_max                     = 0x05;  // not used on sx1261
            output_params->pa_cfg.pa_duty_cycle              = 0x03;
            power = 0x16 - (max_power - power);
        }
        else if (max_power == 17)
        {
            output_params->pa_cfg.device_sel                 = 0x00;  // select SX1261 device
            output_params->pa_cfg.hp_max                     = 0x03;  // not used on sx1261
            output_params->pa_cfg.pa_duty_cycle              = 0x02;
            power = 0x16 - (max_power - power);
        }
        else if (max_power == 14)
        {
            output_params->pa_cfg.device_sel                 = 0x00;  // select SX1261 device
            output_params->pa_cfg.hp_max                     = 0x02;  // not used on sx1261
            output_params->pa_cfg.pa_duty_cycle              = 0x02;
            power = 0x0E - (max_power - power);
        }
        else /*22dBm*/
        {
            output_params->pa_cfg.device_sel                 = 0x00;  // select SX1261 device
            output_params->pa_cfg.hp_max                     = 0x07;  // not used on sx1261
            output_params->pa_cfg.pa_duty_cycle              = 0x04;
            power = 0x16 - (max_power - power);
        }
        if (power < -9)
        {
            power = -9;
        }
    }
    output_params->chip_output_pwr_in_dbm_configured = ( int8_t ) power;
    output_params->chip_output_pwr_in_dbm_expected   = ( int8_t ) max_power;
}

/**
 * Get the trimming capacitor values
 *
 * @param [in] context Chip implementation context
 * @param [out] trimming_cap_xta Value for the trimming capacitor connected to XTA pin
 * @param [out] trimming_cap_xtb Value for the trimming capacitor connected to XTB pin
 */
//void ral_sx126x_bsp_get_trim_cap( const void* context, uint8_t* trimming_cap_xta, uint8_t* trimming_cap_xtb );

static inline void ral_sx126x_bsp_get_trim_cap( const void* context, uint8_t* trimming_cap_xta, uint8_t* trimming_cap_xtb )
{
    // Do nothing, let the driver choose the default values
}

/**
 * Get the Rx boost configuration
 *
 * @param [in] context Chip implementation context
 * @param [out] rx_boost_is_activated Let the caller know if the RX boosted mode is activated
 */
//void ral_sx126x_bsp_get_rx_boost_cfg( const void* context, bool* rx_boost_is_activated );

static inline void ral_sx126x_bsp_get_rx_boost_cfg( const void* context, bool* rx_boost_is_activated )
{
    *rx_boost_is_activated = false;
}


/**
 * @brief Get the Channel Activity Detection (CAD) DetPeak value
 *
 * @param [in] sf                       CAD LoRa spreading factor
 * @param [in] bw                       CAD LoRa bandwidth
 * @param [in] nb_symbol                CAD on number of symbols
 * @param [in, out] in_out_cad_det_peak  CAD DetPeak value proposed by the ral could be overwritten
 */
//void ral_sx126x_bsp_get_lora_cad_det_peak( ral_lora_sf_t sf, ral_lora_bw_t bw, ral_lora_cad_symbs_t nb_symbol,
//                                           uint8_t* in_out_cad_det_peak );

#ifdef __cplusplus
}
#endif

#endif  // RAL_SX126X_BSP_H

/* --- EOF ------------------------------------------------------------------ */
