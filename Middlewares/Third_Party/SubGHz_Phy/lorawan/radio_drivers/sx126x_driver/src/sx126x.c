/**
 * @file      sx126x.c
 *
 * @brief     SX126x radio driver implementation
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

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <string.h>  // memcpy
#include "sx126x.h"
#include "sx126x_hal.h"
#include "sx126x_regs.h"

/* External variables ---------------------------------------------------------*/
/*!
 * \brief Sughz handler
 */
extern SUBGHZ_HandleTypeDef hsubghz;

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */

/**
  * @brief drive value used anytime radio is NOT in TX low power mode
  * @note SMPS_DRIVE_SETTING_DEFAULT can be redefined in radio_conf.h
  */
#ifndef SMPS_DRIVE_SETTING_DEFAULT
#define SMPS_DRIVE_SETTING_DEFAULT  SMPS_DRV_40
#endif /* SMPS_DRIVE_SETTING_DEFAULT */

/**
  * @brief drive value used anytime radio is in TX low power mode
  *        TX low power mode is the worst case because the PA sinks from SMPS
  *        while in high power mode, current is sunk directly from the battery
  * @note SMPS_DRIVE_SETTING_MAX can be redefined in radio_conf.h
  */
#ifndef SMPS_DRIVE_SETTING_MAX
#define SMPS_DRIVE_SETTING_MAX      SMPS_DRV_60
#endif /* SMPS_DRIVE_SETTING_MAX */

/**
  * @brief Provides the frequency of the chip running on the radio and the frequency step
  * @remark These defines are used for computing the frequency divider to set the RF frequency
  * @note XTAL_FREQ can be redefined in radio_conf.h
  */
#ifndef XTAL_FREQ
#define XTAL_FREQ                   ( 32000000UL )
#endif /* XTAL_FREQ */

/**
  * @brief in XO mode, set internal capacitor (from 0x00 to 0x2F starting 11.2pF with 0.47pF steps)
  * @note XTAL_DEFAULT_CAP_VALUE can be redefined in radio_conf.h
  */
#ifndef XTAL_DEFAULT_CAP_VALUE
#define XTAL_DEFAULT_CAP_VALUE      ( 0x20UL )
#endif /* XTAL_DEFAULT_CAP_VALUE */

/**
 * @brief Internal frequency of the radio
 */
#define SX126X_RTC_FREQ_IN_HZ 64000UL

/**
 * @brief Scaling factor used to perform fixed-point operations
 */
#define SX126X_PLL_STEP_SHIFT_AMOUNT ( 14 )

/**
 * @brief PLL step - scaled with SX126X_PLL_STEP_SHIFT_AMOUNT
 */
#define SX126X_PLL_STEP_SCALED ( XTAL_FREQ >> ( 25 - SX126X_PLL_STEP_SHIFT_AMOUNT ) )

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE TYPES -----------------------------------------------------------
 */

typedef struct
{
    uint32_t bw;
    uint8_t  param;
} gfsk_bw_t;

gfsk_bw_t gfsk_bw[] = {
    { 4800, GFSK_BW_4800 },     { 5800, GFSK_BW_5800 },     { 7300, GFSK_BW_7300 },
    { 9700, GFSK_BW_9700 },     { 11700, GFSK_BW_11700 },   { 14600, GFSK_BW_14600 },
    { 19500, GFSK_BW_19500 },   { 23400, GFSK_BW_23400 },   { 29300, GFSK_BW_29300 },
    { 39000, GFSK_BW_39000 },   { 46900, GFSK_BW_46900 },   { 58600, GFSK_BW_58600 },
    { 78200, GFSK_BW_78200 },   { 93800, GFSK_BW_93800 },   { 117300, GFSK_BW_117300 },
    { 156200, GFSK_BW_156200 }, { 187200, GFSK_BW_187200 }, { 234300, GFSK_BW_234300 },
    { 312000, GFSK_BW_312000 }, { 373600, GFSK_BW_373600 }, { 467000, GFSK_BW_467000 },
};

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE VARIABLES -------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

/**
 * @brief 15.1.2 Workaround
 *
 * @remark Before any packet transmission, bit #2 of REG_TX_MODULATION shall be set to:
 * 0 if the LoRa BW = 500 kHz
 * 1 for any other LoRa BW
 * 1 for any (G)FSK configuration
 *
 * @param [in] context Chip implementation context.
 * @param [in] pkt_type The modulation type (G)FSK/LoRa
 * @param [in] bw In case of LoRa modulation the bandwith must be specified
 *
 * @returns Operation status
 */
static sx126x_status_t sx126x_tx_modulation_workaround( const void* context, RadioPacketTypes_t pkt_type,
                                                        sx126x_lora_bw_t bw );

static inline uint32_t sx126x_get_gfsk_crc_len_in_bytes( sx126x_gfsk_crc_types_t crc_type );

/*!
 * \brief This set SMPS drive capability wrt. RF mode
 *
 * \param [in]  level       SMPS maximum drive capability level
 */
static void Radio_SMPS_Set( uint8_t level );

/*!
 * \brief IRQ Callback radio function
 */
static DioIrqHandler RadioOnDioIrqCb;

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

sx126x_status_t sx126x_init_irq( DioIrqHandler dioIrq )
{
    if ( dioIrq != NULL)
    {
        RadioOnDioIrqCb = dioIrq;
    }
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_init( DioIrqHandler dioIrq )
{
	uint8_t tmp    = 0;
    if ( dioIrq != NULL)
    {
        RadioOnDioIrqCb = dioIrq;
    }

    RADIO_INIT();

    /* set default SMPS current drive to default*/
    Radio_SMPS_Set(SMPS_DRIVE_SETTING_DEFAULT);

    //ImageCalibrated = false;
    const void* context = NULL;
    sx126x_set_standby( context, STANDBY_CFG_RC );

    // Initialize TCXO control
    if (1U == RBI_IsTCXO() )
    {
    	sx126x_set_dio3_as_tcxo_ctrl( context, TCXO_CTRL_VOLTAGE, RF_WAKEUP_TIME << 6 );// 100 ms
    	tmp = 0;
    	SUBGRF_WriteRegisters(REG_XTATRIM, &tmp, 1 );

        /*enable calibration for cut1.1 and later*/
        sx126x_cal( context, CAL_ALL );
    }
    else
    {
        tmp = XTAL_DEFAULT_CAP_VALUE;
    	SUBGRF_WriteRegisters(REG_XTATRIM, &tmp, 1 );
    	SUBGRF_WriteRegisters(REG_XTBTRIM, &tmp, 1 );
    }

    tmp = (0x7 << 1);
    /* WORKAROUND - Trimming the output voltage power_ldo to 3.3V */
    SUBGRF_WriteRegisters(REG_DRV_CTRL, &tmp, 1 );

    /* Init RF Switch */
    RBI_Init();

    //OperatingMode = MODE_STDBY_RC;
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_sleep( const void* context, const sx126x_sleep_cfgs_t cfg )
{
    /* switch the antenna OFF by SW */
    RBI_ConfigRFSwitch(RBI_SWITCH_OFF);
    Radio_SMPS_Set(SMPS_DRIVE_SETTING_DEFAULT);

    SUBGRF_WriteCommand( RADIO_SET_SLEEP, ( uint8_t* )&cfg, 1 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_standby( const void* context, const sx126x_standby_cfg_t cfg )
{
    SUBGRF_WriteCommand( RADIO_SET_STANDBY, ( uint8_t* )&cfg, 1 );
    return RADIO_STATUS_OK;

}

sx126x_status_t sx126x_set_fs( const void* context )
{
    SUBGRF_WriteCommand( RADIO_SET_FS, 0, 0 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_tx( const void* context, const uint32_t timeout_in_ms )
{
    if( timeout_in_ms > MAX_TIMEOUT_IN_MS )
    {
        return RADIO_STATUS_UNKNOWN_VALUE;
    }

    const uint32_t timeout_in_rtc_step = sx126x_convert_timeout_in_ms_to_rtc_step( timeout_in_ms );

    return sx126x_set_tx_with_timeout_in_rtc_step( context, timeout_in_rtc_step );
}

sx126x_status_t sx126x_set_tx_with_timeout_in_rtc_step( const void* context, const uint32_t timeout_in_rtc_step )
{
	uint8_t buf[3];
    buf[0] = ( uint8_t )( ( timeout_in_rtc_step >> 16 ) & 0xFF );
    buf[1] = ( uint8_t )( ( timeout_in_rtc_step >> 8 ) & 0xFF );
    buf[2] = ( uint8_t )( timeout_in_rtc_step & 0xFF );
    SUBGRF_WriteCommand( RADIO_SET_TX, buf, 3 );

    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_rx( const void* context, const uint32_t timeout_in_ms )
{
    if( timeout_in_ms > MAX_TIMEOUT_IN_MS )
    {
        return RADIO_STATUS_UNKNOWN_VALUE;
    }
    MW_LOG( TS_ON, VLEVEL_H,  " SET RX\r\n" );
    const uint32_t timeout_in_rtc_step = sx126x_convert_timeout_in_ms_to_rtc_step( timeout_in_ms );

    return sx126x_set_rx_with_timeout_in_rtc_step( context, timeout_in_rtc_step );
}

sx126x_status_t sx126x_set_rx_with_timeout_in_rtc_step( const void* context, const uint32_t timeout_in_rtc_step )
{
	uint8_t buf[3];
    buf[0] = ( uint8_t )( ( timeout_in_rtc_step >> 16 ) & 0xFF );
    buf[1] = ( uint8_t )( ( timeout_in_rtc_step >> 8 ) & 0xFF );
    buf[2] = ( uint8_t )( timeout_in_rtc_step & 0xFF );
    SUBGRF_WriteCommand( RADIO_SET_RX, buf, 3 );

    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_stop_timer_on_preamble( const void* context, const bool enable )
{
	SUBGRF_WriteCommand( RADIO_SET_STOPRXTIMERONPREAMBLE, ( uint8_t* )&enable, 1 );
	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_rx_duty_cycle( const void* context, const uint32_t rx_time_in_ms,
                                          const uint32_t sleep_time_in_ms )
{
    const uint32_t rx_time_in_rtc_step    = sx126x_convert_timeout_in_ms_to_rtc_step( rx_time_in_ms );
    const uint32_t sleep_time_in_rtc_step = sx126x_convert_timeout_in_ms_to_rtc_step( sleep_time_in_ms );

    return sx126x_set_rx_duty_cycle_with_timings_in_rtc_step( context, rx_time_in_rtc_step, sleep_time_in_rtc_step );
}

sx126x_status_t sx126x_set_rx_duty_cycle_with_timings_in_rtc_step( const void*    context,
                                                                   const uint32_t rx_time_in_rtc_step,
                                                                   const uint32_t sleep_time_in_rtc_step )
{
    uint8_t buf[6];

    buf[0] = ( uint8_t )( ( rx_time_in_rtc_step >> 16 ) & 0xFF );
    buf[1] = ( uint8_t )( ( rx_time_in_rtc_step >> 8 ) & 0xFF );
    buf[2] = ( uint8_t )( rx_time_in_rtc_step & 0xFF );
    buf[3] = ( uint8_t )( ( sleep_time_in_rtc_step >> 16 ) & 0xFF );
    buf[4] = ( uint8_t )( ( sleep_time_in_rtc_step >> 8 ) & 0xFF );
    buf[5] = ( uint8_t )( sleep_time_in_rtc_step & 0xFF );
    SUBGRF_WriteCommand( RADIO_SET_RXDUTYCYCLE, buf, 6 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_cad( const void* context )
{
	SUBGRF_WriteCommand( RADIO_SET_CAD, 0, 0 );
	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_tx_cw( const void* context )
{
	SUBGRF_WriteCommand( RADIO_SET_TXCONTINUOUSWAVE, 0, 0 );
	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_tx_infinite_preamble( const void* context )
{
	SUBGRF_WriteCommand( RADIO_SET_TXCONTINUOUSPREAMBLE, 0, 0 );
	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_reg_mode( const void* context, const sx126x_reg_mod_t mode )
{
	sx126x_reg_mod_t reg_mode = mode;

    if ( ( 1UL == RBI_IsDCDC() ) && ( 1UL == DCDC_ENABLE ) )
    {
    	reg_mode = REG_MODE_DCDC ;
    }
    else
    {
    	reg_mode = REG_MODE_LDO ;
    }
	SUBGRF_WriteCommand( RADIO_SET_REGULATORMODE, ( uint8_t* )&reg_mode, 1 );
	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_cal( const void* context, const sx126x_cal_mask_t param )
{
	SUBGRF_WriteCommand( RADIO_CALIBRATE, ( uint8_t* )&param, 1 );
	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_cal_img( const void* context, const uint8_t freq1, const uint8_t freq2 )
{
	uint8_t calFreq[2] = {freq1, freq2};
    SUBGRF_WriteCommand( RADIO_CALIBRATEIMAGE, calFreq, 2 );
	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_cal_img_in_mhz( const void* context, const uint16_t freq1_in_mhz, const uint16_t freq2_in_mhz )
{
    // Perform a floor() to get a value for freq1 corresponding to a frequency lower than or equal to freq1_in_mhz
    const uint8_t freq1 = freq1_in_mhz / IMAGE_CALIBRATION_STEP_IN_MHZ;

    // Perform a ceil() to get a value for freq2 corresponding to a frequency higher than or equal to freq2_in_mhz
    const uint8_t freq2 =
        ( freq2_in_mhz + IMAGE_CALIBRATION_STEP_IN_MHZ - 1 ) / IMAGE_CALIBRATION_STEP_IN_MHZ;

    return sx126x_cal_img( context, freq1, freq2 );
}

sx126x_status_t sx126x_set_pa_cfg( const void* context, const sx126x_pa_cfg_params_t* params )
{
    uint8_t buf[4];

    buf[0] = params->pa_duty_cycle;
    buf[1] = params->hp_max;
    buf[2] = params->device_sel;
    buf[3] = params->pa_lut;
    SUBGRF_WriteCommand( RADIO_SET_PACONFIG, buf, 4 );
	return RADIO_STATUS_OK;

}

sx126x_status_t sx126x_set_rx_tx_fallback_mode( const void* context, const sx126x_fallback_modes_t fallback_mode )
{
    SUBGRF_WriteCommand( RADIO_SET_TXFALLBACKMODE, (uint8_t*)&fallback_mode, 1 );
    return RADIO_STATUS_OK;
}

//
// Registers and buffer Access
//

sx126x_status_t sx126x_write_register( const void* context, const uint16_t address, const uint8_t* buffer,
                                       const uint8_t size )
{
    CRITICAL_SECTION_BEGIN();
    HAL_SUBGHZ_WriteRegisters( &hsubghz, address, (uint8_t*)buffer, size );
    CRITICAL_SECTION_END();
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_read_register( const void* context, const uint16_t address, uint8_t* buffer, const uint8_t size )
{
    CRITICAL_SECTION_BEGIN();
    HAL_SUBGHZ_ReadRegisters( &hsubghz, address, buffer, size );
    CRITICAL_SECTION_END();
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_write_buffer( const void* context, const uint8_t offset, const uint8_t* buffer,
                                     const uint8_t size )
{
    CRITICAL_SECTION_BEGIN();
    HAL_SUBGHZ_WriteBuffer( &hsubghz, offset, (uint8_t*)buffer, size );
    CRITICAL_SECTION_END();
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_read_buffer( const void* context, const uint8_t offset, uint8_t* buffer, const uint8_t size )
{
    CRITICAL_SECTION_BEGIN();
    HAL_SUBGHZ_ReadBuffer( &hsubghz, offset, buffer, size );
    CRITICAL_SECTION_END();
    return RADIO_STATUS_OK;
}

//
// DIO and IRQ Control Functions
//
sx126x_status_t sx126x_set_dio_irq_params( const void* context, const uint16_t irq_mask, const uint16_t dio1_mask,
                                           const uint16_t dio2_mask, const uint16_t dio3_mask )
{
    uint8_t buf[8];

    buf[0] = ( uint8_t )( ( irq_mask >> 8 ) & 0x00FF );
    buf[1] = ( uint8_t )( irq_mask & 0x00FF );
    buf[2] = ( uint8_t )( ( dio1_mask >> 8 ) & 0x00FF );
    buf[3] = ( uint8_t )( dio1_mask & 0x00FF );
    buf[4] = ( uint8_t )( ( dio2_mask >> 8 ) & 0x00FF );
    buf[5] = ( uint8_t )( dio2_mask & 0x00FF );
    buf[6] = ( uint8_t )( ( dio3_mask >> 8 ) & 0x00FF );
    buf[7] = ( uint8_t )( dio3_mask & 0x00FF );
    SUBGRF_WriteCommand( RADIO_CFG_DIOIRQ, buf, 8 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_get_irq_status( const void* context, irq_mask_t* irq )
{
    uint8_t irq_local[sizeof( irq_mask_t )] = { 0x00 };
    SUBGRF_ReadCommand( RADIO_GET_IRQSTATUS, irq_local, sizeof( irq_mask_t ) );

    *irq = ( ( irq_mask_t ) irq_local[0] << 8 ) + ( ( irq_mask_t ) irq_local[1] << 0 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_clear_irq_status( const void* context, const irq_mask_t irq_mask )
{
    uint8_t buf[2];

    buf[0] = ( uint8_t )( ( ( uint16_t )irq_mask >> 8 ) & 0x00FF );
    buf[1] = ( uint8_t )( ( uint16_t )irq_mask & 0x00FF );
    SUBGRF_WriteCommand( RADIO_CLR_IRQSTATUS, buf, 2 );

    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_get_and_clear_irq_status( const void* context, irq_mask_t* irq )
{
	irq_mask_t sx126x_irq_mask = IRQ_RADIO_NONE;

    sx126x_status_t status = sx126x_get_irq_status( context, &sx126x_irq_mask );

    if( ( status == RADIO_STATUS_OK ) && ( sx126x_irq_mask != 0 ) )
    {
        status = sx126x_clear_irq_status( context, sx126x_irq_mask );
    }
    if( ( status == RADIO_STATUS_OK ) && ( irq != NULL ) )
    {
        *irq = sx126x_irq_mask;
    }
    return status;
}

sx126x_status_t sx126x_set_dio2_as_rf_sw_ctrl( const void* context, const bool enable )
{
	SUBGRF_WriteCommand( RADIO_SET_RFSWITCHMODE, ( uint8_t* )&enable, 1 );

    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_dio3_as_tcxo_ctrl( const void* context, const sx126x_tcxo_ctrl_voltages_t tcxo_voltage,
                                              const uint32_t timeout )
{
    uint8_t buf[4];

    buf[0] = tcxo_voltage & 0x07;
    buf[1] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
    buf[2] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
    buf[3] = ( uint8_t )( timeout & 0xFF );

    SUBGRF_WriteCommand( RADIO_SET_TCXOMODE, buf, 4 );
    return RADIO_STATUS_OK;
}

//
// RF Modulation and Packet-Related Functions
//

sx126x_status_t sx126x_set_rf_freq( const void* context, const uint32_t freq_in_hz )
{
    const uint32_t freq = sx126x_convert_freq_in_hz_to_pll_step( freq_in_hz );

    return sx126x_set_rf_freq_in_pll_steps( context, freq );
}

sx126x_status_t sx126x_set_rf_freq_in_pll_steps( const void* context, const uint32_t freq )
{
    uint8_t buf[4];

	buf[0] = ( uint8_t )( ( freq >> 24 ) & 0xFF );
	buf[1] = ( uint8_t )( ( freq >> 16 ) & 0xFF );
	buf[2] = ( uint8_t )( ( freq >> 8 ) & 0xFF );
	buf[3] = ( uint8_t )( freq & 0xFF );
    SUBGRF_WriteCommand( RADIO_SET_RFFREQUENCY, buf, 4 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_pkt_type( const void* context, const RadioPacketTypes_t pkt_type )
{
	uint8_t tmp = 0;
    if( (RadioPacketTypes_t)pkt_type == PKT_TYPE_GFSK )
    {
        SUBGRF_WriteRegisters(REG_BIT_SYNC, &tmp, 1 );
    }
    SUBGRF_WriteCommand( RADIO_SET_PACKETTYPE, ( uint8_t* )&pkt_type, 1 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_get_pkt_type( const void* context, RadioPacketTypes_t* pkt_type )
{
	SUBGRF_ReadCommand( RADIO_GET_PACKETTYPE, ( uint8_t* )&pkt_type, 1 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_tx_params( const void* context, const int8_t pwr_in_dbm, const sx126x_ramp_time_t ramp_time )
{
    uint8_t buf[2];
    buf[0] = pwr_in_dbm;
    buf[1] = (uint8_t)ramp_time;
    SUBGRF_WriteCommand(RADIO_SET_TXPARAMS, buf, 2);
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_gfsk_mod_params( const void* context, const sx126x_mod_params_gfsk_t* params )
{
    const uint32_t bitrate = ( uint32_t )( 32 * XTAL_FREQ / params->br_in_bps );
    const uint32_t fdev    = sx126x_convert_freq_in_hz_to_pll_step( params->fdev_in_hz );
    uint8_t buf[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    buf[0] = ( bitrate >> 16 ) & 0xFF;
    buf[1] = ( bitrate >> 8 ) & 0xFF;
    buf[2] = bitrate & 0xFF;
    buf[3] = params->pulse_shape;
    buf[4] = params->bw_dsb_param;
    buf[5] = ( fdev >> 16 ) & 0xFF;
    buf[6] = ( fdev >> 8 ) & 0xFF;
    buf[7] = ( fdev & 0xFF );
    SUBGRF_WriteCommand( RADIO_SET_MODULATIONPARAMS, buf, 8 );

    // WORKAROUND - Modulation Quality with 500 kHz LoRa Bandwidth, see DS_SX1261-2_V1.2 datasheet chapter 15.1
    sx126x_status_t status = sx126x_tx_modulation_workaround( context, PKT_TYPE_GFSK, ( sx126x_lora_bw_t ) 0 );
    // WORKAROUND END
    return status;
}

sx126x_status_t sx126x_set_bpsk_mod_params( const void* context, const sx126x_mod_params_bpsk_t* params )
{
    const uint32_t bitrate = ( uint32_t )( 32 * XTAL_FREQ / params->br_in_bps );
    uint8_t buf[4] = { 0x00, 0x00, 0x00, 0x00 };
    buf[0] = ( bitrate >> 16 ) & 0xFF;
    buf[1] = ( bitrate >> 8 ) & 0xFF;
    buf[2] = bitrate & 0xFF;
    buf[3] = params->pulse_shape;
    SUBGRF_WriteCommand( RADIO_SET_MODULATIONPARAMS, buf, 4);

    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_lora_mod_params( const void* context, const sx126x_mod_params_lora_t* params )
{
    uint8_t buf[4] = { 0x00, 0x00, 0x00, 0x00 };
    buf[0] = params->sf;
    buf[1] = params->bw;
    buf[2] = params->cr;
    buf[3] = params->ldro & 0x01;
    SUBGRF_WriteCommand( RADIO_SET_MODULATIONPARAMS, buf, 4);

    // WORKAROUND - Modulation Quality with 500 kHz LoRa Bandwidth, see datasheet DS_SX1261-2_V1.2 §15.1
    sx126x_status_t status = sx126x_tx_modulation_workaround( context, PKT_TYPE_LORA, params->bw );
    // WORKAROUND END

    return status;
}

sx126x_status_t sx126x_set_gfsk_pkt_params( const void* context, const sx126x_pkt_params_gfsk_t* params )
{
    uint8_t buf[9] = {0};

    buf[0] = (params->preamble_len_in_bits >> 8 ) & 0xFF;
    buf[1] = params->preamble_len_in_bits;
    buf[2] = params->preamble_detector;
    buf[3] =  params->sync_word_len_in_bits;
    buf[4] =  params->address_filtering;
	buf[5] = params->header_type;
	buf[6] = params->pld_len_in_bytes;
	buf[7] =  params->crc_type;
	buf[8] = params->dc_free;

	SUBGRF_WriteCommand( RADIO_SET_PACKETPARAMS, buf, 9 );
	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_bpsk_pkt_params( const void* context, const sx126x_pkt_params_bpsk_t* params )
{
	uint8_t pkt_len = params->pld_len_in_bytes;

	SUBGRF_WriteCommand( RADIO_SET_PACKETPARAMS, &pkt_len, 1 );

    uint8_t buf[6] = {0};
    buf[0] = params->ramp_up_delay >> 8;
    buf[1] = params->ramp_up_delay;
    buf[2] = params->ramp_down_delay >> 8;
    buf[3] = params->ramp_down_delay;
    buf[4] = params->pld_len_in_bits >> 8;
    buf[5] = params->pld_len_in_bits;

    SUBGRF_WriteRegisters( SUBGHZ_RAM_RAMPUPH, buf, 6 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_lora_pkt_params( const void* context, const sx126x_pkt_params_lora_t* params )
{
	uint8_t buf[6] = {0};

	buf[0] = (params->preamble_len_in_symb >> 8) & 0xFF;
	buf[1] = params->preamble_len_in_symb;
	buf[2] = params->header_type;
	buf[3] = params->pld_len_in_bytes;
	buf[4] = params->crc_is_on ? 1 : 0;
	buf[5] = params->invert_iq_is_on ? 1 : 0;

	SUBGRF_WriteCommand( RADIO_SET_PACKETPARAMS, buf, 6 );

    // WORKAROUND - Optimizing the Inverted IQ Operation, see datasheet DS_SX1261-2_V1.2 §15.4
	uint8_t reg_value = 0;
	SUBGRF_ReadRegisters( SUBGHZ_LIQPOLR, ( uint8_t* )&reg_value, 1);

	if( params->invert_iq_is_on == true )
	{
		reg_value &= ~( 1 << 2 );  // Bit 2 set to 0 when using inverted IQ polarity
	}
	else
	{
		reg_value |= ( 1 << 2 );  // Bit 2 set to 1 when using standard IQ polarity
	}
	SUBGRF_WriteRegisters( SUBGHZ_LIQPOLR, &reg_value, 1 );
    // WORKAROUND END

	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_gfsk_pkt_address( const void* context, const uint8_t node_address,
                                             const uint8_t broadcast_address )
{
    const uint8_t addresses[2] = { node_address, broadcast_address };

    SUBGRF_WriteRegisters( SUBGHZ_GNODEADDR, (uint8_t*)addresses, 2 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_cad_params( const void* context, const sx126x_cad_params_t* params )
{
    uint8_t buf[7];

    buf[0] = ( uint8_t )params->cad_symb_nb;
    buf[1] = params->cad_detect_peak;
    buf[2] = params->cad_detect_min;
    buf[3] = ( uint8_t )params->cad_exit_mode;
    buf[4] = ( uint8_t )( ( params->cad_timeout >> 16 ) & 0xFF );
    buf[5] = ( uint8_t )( ( params->cad_timeout >> 8 ) & 0xFF );
    buf[6] = ( uint8_t )( params->cad_timeout & 0xFF );
    SUBGRF_WriteCommand( RADIO_SET_CADPARAMS, buf, 7 );

    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_buffer_base_address( const void* context, const uint8_t tx_base_address,
                                                const uint8_t rx_base_address )
{
    uint8_t buf[2];
    buf[0] = tx_base_address;
    buf[1] = rx_base_address;
    SUBGRF_WriteCommand( RADIO_SET_BUFFERBASEADDRESS, buf, 2 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_lora_symb_nb_timeout( const void* context, const uint8_t nb_of_symbs )
{
    uint8_t exp = 0;
    uint8_t mant = ( ( ( nb_of_symbs > MAX_LORA_SYMB_NUM_TIMEOUT ) ? MAX_LORA_SYMB_NUM_TIMEOUT : nb_of_symbs ) + 1 ) >> 1;
    uint8_t nb_symbol  = 0;
    uint8_t reg  = 0;

    while( mant > 31 )
    {
        mant = ( mant + 3 ) >> 2;
        exp++;
    }

    nb_symbol = mant << ( 2 * exp + 1 );

    SUBGRF_WriteCommand( RADIO_SET_LORASYMBTIMEOUT, &nb_symbol, 1 );

    if( nb_of_symbs > 0 )
    {
        reg = exp + ( mant << 3 );
        SUBGRF_WriteRegisters(REG_LR_SYNCH_TIMEOUT, &reg, 1 );
    }
    return RADIO_STATUS_OK;
}

//
// Communication Status Information
//

sx126x_status_t sx126x_get_status( const void* context, sx126x_chip_status_t* radio_status )
{
    uint8_t status_local = 0;

    SUBGRF_ReadCommand( RADIO_GET_STATUS, &status_local, 1 );

	radio_status->cmd_status = ( sx126x_cmd_status_t )( ( status_local & CMD_STATUS_MASK ) >> CMD_STATUS_POS );
	radio_status->chip_mode = ( sx126x_chip_modes_t )( ( status_local & CHIP_MODES_MASK ) >> CHIP_MODES_POS );
	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_get_rx_buffer_status( const void* context, sx126x_rx_buffer_status_t* rx_buffer_status )
{

    uint8_t status_local[sizeof( sx126x_rx_buffer_status_t )] = { 0x00 };

    SUBGRF_ReadCommand( RADIO_GET_RXBUFFERSTATUS, status_local, sizeof( sx126x_rx_buffer_status_t ) );

    rx_buffer_status->pld_len_in_bytes     = status_local[0];
    rx_buffer_status->buffer_start_pointer = status_local[1];
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_get_gfsk_pkt_status( const void* context, sx126x_pkt_status_gfsk_t* pkt_status )
{
    uint8_t pkt_status_local[3] = { 0x00 };
    SUBGRF_ReadCommand( RADIO_GET_PACKETSTATUS, pkt_status_local, 3 );

	pkt_status->rx_status.pkt_sent = ( ( pkt_status_local[0] & GFSK_RX_STATUS_PKT_SENT_MASK ) != 0 ) ? true : false;
	pkt_status->rx_status.pkt_received = ( ( pkt_status_local[0] & GFSK_RX_STATUS_PKT_RECEIVED_MASK ) != 0 ) ? true : false;
	pkt_status->rx_status.abort_error =	( ( pkt_status_local[0] & GFSK_RX_STATUS_ABORT_ERROR_MASK ) != 0 ) ? true : false;
	pkt_status->rx_status.length_error = ( ( pkt_status_local[0] & GFSK_RX_STATUS_LENGTH_ERROR_MASK ) != 0 ) ? true : false;
	pkt_status->rx_status.crc_error = ( ( pkt_status_local[0] & GFSK_RX_STATUS_CRC_ERROR_MASK ) != 0 ) ? true : false;
	pkt_status->rx_status.adrs_error = ( ( pkt_status_local[0] & GFSK_RX_STATUS_ADRS_ERROR_MASK ) != 0 ) ? true : false;

	pkt_status->rssi_sync = ( int8_t )( -pkt_status_local[1] >> 1 );
	pkt_status->rssi_avg  = ( int8_t )( -pkt_status_local[2] >> 1 );

	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_get_lora_pkt_status( const void* context, sx126x_pkt_status_lora_t* pkt_status )
{
    uint8_t pkt_status_local[sizeof( sx126x_pkt_status_lora_t )] = { 0x00 };
    SUBGRF_ReadCommand( RADIO_GET_PACKETSTATUS, pkt_status_local, sizeof( sx126x_pkt_status_lora_t ) );

    pkt_status->rssi_pkt_in_dbm        = ( int8_t )( -pkt_status_local[0] >> 1 );
    pkt_status->snr_pkt_in_db          = ( ( ( int8_t ) pkt_status_local[1] ) + 2 ) >> 2;
    pkt_status->signal_rssi_pkt_in_dbm = ( int8_t )( -pkt_status_local[2] >> 1 );

    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_get_rssi_inst( const void* context, int16_t* rssi_in_dbm )
{
    uint8_t rssi_local = 0x00;
    SUBGRF_ReadCommand( RADIO_GET_RSSIINST, &rssi_local, 1 );

    *rssi_in_dbm = ( int8_t )( -rssi_local >> 1 );

    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_get_gfsk_stats( const void* context, sx126x_stats_gfsk_t* stats )
{
    uint8_t stats_local[sizeof( sx126x_stats_gfsk_t )] = { 0 };

	SUBGRF_ReadCommand( RADIO_GET_STATS, stats_local, sizeof( sx126x_stats_gfsk_t ) );

    stats->nb_pkt_received  = ( ( uint16_t ) stats_local[0] << 8 ) + ( uint16_t ) stats_local[1];
    stats->nb_pkt_crc_error = ( ( uint16_t ) stats_local[2] << 8 ) + ( uint16_t ) stats_local[3];
    stats->nb_pkt_len_error = ( ( uint16_t ) stats_local[4] << 8 ) + ( uint16_t ) stats_local[5];

    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_get_lora_stats( const void* context, sx126x_stats_lora_t* stats )
{
    uint8_t stats_local[sizeof( sx126x_stats_lora_t )] = { 0 };

	SUBGRF_ReadCommand( RADIO_GET_STATS, stats_local, sizeof( sx126x_stats_lora_t ) );

    stats->nb_pkt_received  = ( ( uint16_t ) stats_local[0] << 8 ) + ( uint16_t ) stats_local[1];
    stats->nb_pkt_crc_error = ( ( uint16_t ) stats_local[2] << 8 ) + ( uint16_t ) stats_local[3];
    stats->nb_pkt_header_error = ( ( uint16_t ) stats_local[4] << 8 ) + ( uint16_t ) stats_local[5];

    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_reset_stats( const void* context )
{
    uint8_t buf[6] = {0};
    SUBGRF_WriteCommand( RADIO_RESET_STATS, buf, 6 );

    return RADIO_STATUS_OK;
}

//
// Miscellaneous
//

sx126x_status_t sx126x_reset( const void* context )
{
    //return ( sx126x_status_t ) sx126x_hal_reset( context );
	// #FIXMELS Probably nothing should be done
	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_wakeup( const void* context )
{
	HAL_SUBGHZ_CheckDeviceReady(&hsubghz);
	return RADIO_STATUS_OK;
    //return ( sx126x_status_t ) sx126x_hal_wakeup( context );
}

sx126x_status_t sx126x_get_device_errors( const void* context, sx126x_errors_mask_t* errors )
{
    uint8_t errors_local[sizeof( sx126x_errors_mask_t )] = { 0x00 };
    SUBGRF_ReadCommand( RADIO_GET_STATS, errors_local, sizeof( sx126x_errors_mask_t ) );

    *errors = ( ( sx126x_errors_mask_t ) errors_local[0] << 8 ) + ( ( sx126x_errors_mask_t ) errors_local[1] << 0 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_clear_device_errors( const void* context )
{
    uint8_t buf[2] = { 0x00, 0x00 };
    SUBGRF_WriteCommand( RADIO_CLR_ERROR, buf, 2 );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_get_gfsk_bw_param( const uint32_t bw, uint8_t* param )
{
    sx126x_status_t status = RADIO_STATUS_ERROR;

    if( bw != 0 )
    {
        status = RADIO_STATUS_UNKNOWN_VALUE;
        for( uint8_t i = 0; i < ( sizeof( gfsk_bw ) / sizeof( gfsk_bw_t ) ); i++ )
        {
            if( bw <= gfsk_bw[i].bw )
            {
                *param = gfsk_bw[i].param;
                status = RADIO_STATUS_OK;
                break;
            }
        }
    }

    return status;
}

uint32_t sx126x_get_lora_bw_in_hz( sx126x_lora_bw_t bw )
{
    uint32_t bw_in_hz = 0;

    switch( bw )
    {
    case LORA_BW_007:
        bw_in_hz = 7812UL;
        break;
    case LORA_BW_010:
        bw_in_hz = 10417UL;
        break;
    case LORA_BW_015:
        bw_in_hz = 15625UL;
        break;
    case LORA_BW_020:
        bw_in_hz = 20833UL;
        break;
    case LORA_BW_031:
        bw_in_hz = 31250UL;
        break;
    case LORA_BW_041:
        bw_in_hz = 41667UL;
        break;
    case LORA_BW_062:
        bw_in_hz = 62500UL;
        break;
    case LORA_BW_125:
        bw_in_hz = 125000UL;
        break;
    case LORA_BW_250:
        bw_in_hz = 250000UL;
        break;
    case LORA_BW_500:
        bw_in_hz = 500000UL;
        break;
    }

    return bw_in_hz;
}

uint32_t sx126x_get_lora_time_on_air_numerator( const sx126x_pkt_params_lora_t* pkt_p,
                                                const sx126x_mod_params_lora_t* mod_p )
{
    const int32_t pld_len_in_bytes = pkt_p->pld_len_in_bytes;
    const int32_t sf               = mod_p->sf;
    const bool    pld_is_fix       = pkt_p->header_type == (sx126x_lora_pkt_len_modes_t)LORA_PKT_IMPLICIT;
    const int32_t cr_denom         = mod_p->cr + 4;

    int32_t ceil_denominator;
    int32_t ceil_numerator =
        ( pld_len_in_bytes << 3 ) + ( pkt_p->crc_is_on ? 16 : 0 ) - ( 4 * sf ) + ( pld_is_fix ? 0 : 20 );

    if( sf <= 6 )
    {
        ceil_denominator = 4 * sf;
    }
    else
    {
        ceil_numerator += 8;

        if( mod_p->ldro )
        {
            ceil_denominator = 4 * ( sf - 2 );
        }
        else
        {
            ceil_denominator = 4 * sf;
        }
    }

    if( ceil_numerator < 0 )
    {
        ceil_numerator = 0;
    }

    // Perform integral ceil()
    int32_t intermed =
        ( ( ceil_numerator + ceil_denominator - 1 ) / ceil_denominator ) * cr_denom + pkt_p->preamble_len_in_symb + 12;

    if( sf <= 6 )
    {
        intermed += 2;
    }

    return ( uint32_t )( ( 4 * intermed + 1 ) * ( 1 << ( sf - 2 ) ) );
}

uint32_t sx126x_get_lora_time_on_air_in_ms( const sx126x_pkt_params_lora_t* pkt_p,
                                            const sx126x_mod_params_lora_t* mod_p )
{
    uint32_t numerator   = 1000U * sx126x_get_lora_time_on_air_numerator( pkt_p, mod_p );
    uint32_t denominator = sx126x_get_lora_bw_in_hz( mod_p->bw );
    // Perform integral ceil()
    return ( numerator + denominator - 1 ) / denominator;
}

uint32_t sx126x_get_gfsk_time_on_air_numerator( const sx126x_pkt_params_gfsk_t* pkt_p )
{
    return pkt_p->preamble_len_in_bits + ( pkt_p->header_type == (sx126x_gfsk_pkt_len_modes_t)GFSK_PKT_VAR_LEN ? 8 : 0 ) +
           pkt_p->sync_word_len_in_bits +
           ( ( pkt_p->pld_len_in_bytes + ( pkt_p->address_filtering == (sx126x_gfsk_address_filtering_t)GFSK_ADDRESS_FILTERING_DISABLE ? 0 : 1 ) +
               sx126x_get_gfsk_crc_len_in_bytes( pkt_p->crc_type ) )
             << 3 );
}

uint32_t sx126x_get_gfsk_time_on_air_in_ms( const sx126x_pkt_params_gfsk_t* pkt_p,
                                            const sx126x_mod_params_gfsk_t* mod_p )
{
    uint32_t numerator   = 1000U * sx126x_get_gfsk_time_on_air_numerator( pkt_p );
    uint32_t denominator = mod_p->br_in_bps;

    // Perform integral ceil()
    return ( numerator + denominator - 1 ) / denominator;
}

sx126x_status_t sx126x_get_random_numbers( const void* context, uint32_t* numbers, unsigned int n )
{
    uint8_t tmp_ana_lna   = 0x00;
    uint8_t tmp_ana_mixer = 0x00;
    uint8_t tmp           = 0x00;

    // Configure for random number generation
	SUBGRF_ReadRegisters(REG_ANA_LNA, &tmp_ana_lna, 1 );

    tmp    = tmp_ana_lna & ~( 1 << 0 );
	SUBGRF_WriteRegisters(REG_ANA_LNA, &tmp, 1 );

	SUBGRF_ReadRegisters(REG_ANA_MIXER, &tmp_ana_mixer, 1 );

    tmp    = tmp_ana_mixer & ~( 1 << 7 );
	SUBGRF_WriteRegisters(REG_ANA_MIXER, &tmp, 1 );

    // Start RX continuous
    sx126x_set_rx_with_timeout_in_rtc_step( context, RX_CONTINUOUS );

    // Store values
    SUBGRF_ReadRegisters(REG_RNGBASEADDRESS, (uint8_t*)numbers, 4 );


    sx126x_set_standby( context, STANDBY_CFG_RC );

    // Restore registers
	SUBGRF_WriteRegisters(REG_ANA_LNA, &tmp_ana_lna, 1 );

	SUBGRF_WriteRegisters(REG_ANA_MIXER, &tmp_ana_mixer, 1 );

    return RADIO_STATUS_OK;
}

uint32_t sx126x_convert_freq_in_hz_to_pll_step( uint32_t freq_in_hz )
{
    uint32_t steps_int;
    uint32_t steps_frac;

    // Get integer and fractional parts of the frequency computed with a PLL step scaled value
    steps_int  = freq_in_hz / SX126X_PLL_STEP_SCALED;
    steps_frac = freq_in_hz - ( steps_int * SX126X_PLL_STEP_SCALED );

    // Apply the scaling factor to retrieve a frequency in Hz (+ ceiling)
    return ( steps_int << SX126X_PLL_STEP_SHIFT_AMOUNT ) +
           ( ( ( steps_frac << SX126X_PLL_STEP_SHIFT_AMOUNT ) + ( SX126X_PLL_STEP_SCALED >> 1 ) ) /
             SX126X_PLL_STEP_SCALED );
}

uint32_t sx126x_convert_timeout_in_ms_to_rtc_step( uint32_t timeout_in_ms )
{
    return ( uint32_t )( timeout_in_ms * ( SX126X_RTC_FREQ_IN_HZ / 1000 ) );
}

sx126x_status_t sx126x_handle_rx_done( const void* context )
{
    return sx126x_stop_rtc( context );
}

//
// Registers access
//

sx126x_status_t sx126x_cfg_rx_boosted( const void* context, const bool state )
{
	uint8_t tmp = 0;
    if( state == true )
    {
    	tmp = 0x96;
        SUBGRF_WriteRegisters(REG_RXGAIN, &tmp, 1 );
    }
    else
    {
    	tmp = 0x94;
        SUBGRF_WriteRegisters(REG_RXGAIN, &tmp, 1 );
    }
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_gfsk_sync_word( const void* context, const uint8_t* sync_word, const uint8_t sync_word_len )
{
    uint8_t         buf[8] = { 0 };

    if( sync_word_len <= 8 )
    {
        memcpy( buf, sync_word, sync_word_len );
        SUBGRF_WriteRegisters( REG_LR_SYNCWORDBASEADDRESS, buf, 8 );
    }
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_lora_sync_word( const void* context, const uint8_t sync_word )
{
    uint8_t buffer[2] = { 0x00 };

    SUBGRF_ReadRegisters(REG_LR_SYNCWORD, buffer, 2 );
	buffer[0] = ( buffer[0] & ~0xF0 ) + ( sync_word & 0xF0 );
	buffer[1] = ( buffer[1] & ~0xF0 ) + ( ( sync_word & 0x0F ) << 4 );

	SUBGRF_WriteRegisters( REG_LR_SYNCWORD, buffer, 2 );

	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_gfsk_crc_seed( const void* context, uint16_t seed )
{
    uint8_t s[] = { ( uint8_t )( seed >> 8 ), ( uint8_t ) seed };
    SUBGRF_WriteRegisters( REG_LR_CRCSEEDBASEADDR, s, sizeof( s ) );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_gfsk_crc_polynomial( const void* context, const uint16_t polynomial )
{
    uint8_t poly[] = { ( uint8_t )( polynomial >> 8 ), ( uint8_t ) polynomial };
    SUBGRF_WriteRegisters( REG_LR_CRCPOLYBASEADDR, poly, sizeof( poly ) );
    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_gfsk_whitening_seed( const void* context, const uint16_t seed )
{
    uint8_t reg_value = 0;

    // The SX126X_REG_WHITSEEDBASEADDRESS @ref LSBit is used for the seed value. The 7 MSBits must not be modified.
    // Thus, we first need to read the current value and then change the LSB according to the provided seed @ref value.
	SUBGRF_ReadRegisters(REG_LR_WHITSEEDBASEADDR_MSB, &reg_value, 1 );

	reg_value = ( reg_value & 0xFE ) | ( ( uint8_t )( seed >> 8 ) & 0x01 );
	SUBGRF_WriteRegisters(REG_LR_WHITSEEDBASEADDR_MSB, &reg_value, 1 );

	reg_value = ( uint8_t ) seed;
	SUBGRF_WriteRegisters(REG_LR_WHITSEEDBASEADDR_MSB + 1, &reg_value, 1 );

	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_cfg_tx_clamp( const void* context )
{
    uint8_t reg_value = 0x00;

	SUBGRF_ReadRegisters(REG_TX_CLAMP_CFG, &reg_value, 1 );

	reg_value |= REG_TX_CLAMP_CFG_MASK;
	SUBGRF_WriteRegisters(REG_TX_CLAMP_CFG, &reg_value, 1 );

    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_stop_rtc( const void* context )
{
    uint8_t reg_value = 0;

    SUBGRF_WriteRegisters(SUBGHZ_RTCCTLR, &reg_value, 1 );

    SUBGRF_ReadRegisters(SUBGHZ_EVENTMASKR, &reg_value, 1 );

	reg_value |= REG_EVT_CLR_TIMEOUT_MASK;
	SUBGRF_WriteRegisters(SUBGHZ_EVENTMASKR, &reg_value, 1 );

    return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_ocp_value( const void* context, const uint8_t ocp_in_step_of_2_5_ma )
{
	uint8_t ocp_value = ocp_in_step_of_2_5_ma;
	SUBGRF_WriteRegisters(REG_OCP, &ocp_value, 1 );
	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_trimming_capacitor_values( const void* context, const uint8_t trimming_cap_xta,
                                                      const uint8_t trimming_cap_xtb )
{
    uint8_t trimming_capacitor_values[2] = { trimming_cap_xta, trimming_cap_xtb };
	SUBGRF_WriteRegisters( REG_XTATRIM, trimming_capacitor_values, 2 );
	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_add_registers_to_retention_list( const void* context, const uint16_t* register_addr,
                                                        uint8_t register_nb )
{
    uint8_t buffer[9] = {0};
    const uint8_t initial_nb_of_registers = buffer[0];
    uint8_t*      register_list           = &buffer[1];

    SUBGRF_ReadRegisters( REG_RETENTION_LIST_BASE_ADDRESS, buffer, 9 );

	for( uint8_t index = 0; index < register_nb; index++ )
	{
		bool register_has_to_be_added = true;

		// Check if the current register is already added to the list
		for( uint8_t i = 0; i < buffer[0]; i++ )
		{
			if( register_addr[index] == ( ( uint16_t ) register_list[2 * i] << 8 ) + register_list[2 * i + 1] )
			{
				register_has_to_be_added = false;
				break;
			}
		}

		if( register_has_to_be_added == true )
		{
			if( buffer[0] < MAX_NB_REG_IN_RETENTION )
			{
				register_list[2 * buffer[0]]     = ( uint8_t )( register_addr[index] >> 8 );
				register_list[2 * buffer[0] + 1] = ( uint8_t )( register_addr[index] >> 0 );
				buffer[0] += 1;
			}
			else
			{
				return RADIO_STATUS_ERROR;
			}
		}
	}

	if( buffer[0] != initial_nb_of_registers )
	{
		SUBGRF_WriteRegisters( REG_RETENTION_LIST_BASE_ADDRESS, buffer, 9 );
	}

	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_init_retention_list( const void* context )
{
    const uint16_t list_of_registers[3] = { REG_RXGAIN, REG_TX_MODULATION, REG_IQ_POLARITY };

    return sx126x_add_registers_to_retention_list( context, list_of_registers,
                                                   sizeof( list_of_registers ) / sizeof( list_of_registers[0] ) );
}

sx126x_status_t sx126x_get_lora_params_from_header( const void* context, sx126x_lora_cr_t* cr, bool* crc_is_on )
{
    uint8_t buffer_cr = 0;
    uint8_t buffer_crc = 0;

    SUBGRF_ReadRegisters(REG_LR_HEADER_CR, &buffer_cr, 1 );

    SUBGRF_ReadRegisters(REG_LR_HEADER_CRC, &buffer_crc, 1 );

	*cr = ( sx126x_lora_cr_t )( ( buffer_cr & REG_LR_HEADER_CR_MASK ) >> REG_LR_HEADER_CR_POS );
	*crc_is_on = ( ( buffer_crc & REG_LR_HEADER_CRC_MASK ) != 0 ) ? true : false;

	return RADIO_STATUS_OK;
}

sx126x_status_t sx126x_set_ant_switch( bool is_tx_on )
{
  RBI_Switch_TypeDef state = RBI_SWITCH_RX;

  uint8_t paSelect= RFO_LP;

  if (is_tx_on)
  {
      if (paSelect == RFO_LP)
      {
          state = RBI_SWITCH_RFO_LP;
          if ( 1U == RBI_IsDCDC() )
          {
            uint8_t modReg;
            HAL_SUBGHZ_ReadRegisters( &hsubghz, SUBGHZ_SMPSC2R, &modReg, 1 );
            modReg&= (~SMPS_DRV_MASK);
            HAL_SUBGHZ_WriteRegisters( &hsubghz, SUBGHZ_SMPSC2R, (uint8_t*)&modReg, 1 );
          }

      }
      if (paSelect == RFO_HP)
      {
          state = RBI_SWITCH_RFO_HP;
      }
  }
  else
  {
      state = RBI_SWITCH_RX;
  }
  RBI_ConfigRFSwitch(state);

  return RADIO_STATUS_OK;
}

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITION --------------------------------------------
 */

static sx126x_status_t sx126x_tx_modulation_workaround( const void* context, RadioPacketTypes_t pkt_type,
                                                        sx126x_lora_bw_t bw )
{
    uint8_t reg_value = 0;

	SUBGRF_ReadRegisters(SUBGHZ_SDCFG0R, &reg_value, 1 );


	if( (RadioPacketTypes_t)pkt_type == PKT_TYPE_LORA )
	{
		if( (sx126x_lora_bw_t)bw == LORA_BW_500 )
		{
			reg_value &= ~( 1 << 2 );  // Bit 2 set to 0 if the LoRa BW = 500 kHz
		}
		else
		{
			reg_value |= ( 1 << 2 );  // Bit 2 set to 1 for any other LoRa BW
		}
	}
	else
	{
		reg_value |= ( 1 << 2 );  // Bit 2 set to 1 for any (G)FSK configuration
	}

	SUBGRF_WriteRegisters(SUBGHZ_SDCFG0R, &reg_value, 1 );
	return RADIO_STATUS_OK;
}

static inline uint32_t sx126x_get_gfsk_crc_len_in_bytes( sx126x_gfsk_crc_types_t crc_type )
{
    switch( crc_type )
    {
    case GFSK_CRC_OFF:
        return 0;
    case GFSK_CRC_1_BYTE:
        return 1;
    case GFSK_CRC_2_BYTES:
        return 2;
    case GFSK_CRC_1_BYTE_INV:
        return 1;
    case GFSK_CRC_2_BYTES_INV:
        return 2;
    }

    return 0;
}

void SUBGRF_WriteCommand( SUBGHZ_RadioSetCmd_t Command, uint8_t *pBuffer,
                                        uint16_t Size )
{
	HAL_StatusTypeDef status = HAL_OK;
    CRITICAL_SECTION_BEGIN();
    status = HAL_SUBGHZ_ExecSetCmd( &hsubghz, Command, pBuffer, Size );
    CRITICAL_SECTION_END();
    if(status !=0)
    	MW_LOG( TS_ON, VLEVEL_M,  " write CMD error %d", status);
}

void SUBGRF_ReadCommand( SUBGHZ_RadioGetCmd_t Command, uint8_t *pBuffer,
                                        uint16_t Size )
{
    CRITICAL_SECTION_BEGIN();
    HAL_SUBGHZ_ExecGetCmd( &hsubghz, Command, pBuffer, Size );
    CRITICAL_SECTION_END();
}

void SUBGRF_WriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    CRITICAL_SECTION_BEGIN();
    HAL_SUBGHZ_WriteRegisters( &hsubghz, address, buffer, size );
    CRITICAL_SECTION_END();
}

void SUBGRF_ReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    CRITICAL_SECTION_BEGIN();
    HAL_SUBGHZ_ReadRegisters( &hsubghz, address, buffer, size );
    CRITICAL_SECTION_END();
}

/* HAL_SUBGHz Callbacks definitions */
void HAL_SUBGHZ_TxCpltCallback(SUBGHZ_HandleTypeDef *hsubghz)
{
    RadioOnDioIrqCb( IRQ_RADIO_TX_DONE );
}

void HAL_SUBGHZ_RxCpltCallback(SUBGHZ_HandleTypeDef *hsubghz)
{
    RadioOnDioIrqCb( IRQ_RADIO_RX_DONE );
}

void HAL_SUBGHZ_CRCErrorCallback (SUBGHZ_HandleTypeDef *hsubghz)
{
    RadioOnDioIrqCb( IRQ_RADIO_CRC_ERROR);
}

void HAL_SUBGHZ_CADStatusCallback(SUBGHZ_HandleTypeDef *hsubghz, HAL_SUBGHZ_CadStatusTypeDef cadstatus)
{
    switch (cadstatus)
    {
        case HAL_SUBGHZ_CAD_CLEAR:
            RadioOnDioIrqCb( IRQ_RADIO_CAD_DONE);
            break;
        case HAL_SUBGHZ_CAD_DETECTED:
            RadioOnDioIrqCb( IRQ_RADIO_CAD_DETECTED);
            break;
        default:
            break;
    }
}

void HAL_SUBGHZ_RxTxTimeoutCallback(SUBGHZ_HandleTypeDef *hsubghz)
{
    RadioOnDioIrqCb( IRQ_RADIO_TIMEOUT );
}

void HAL_SUBGHZ_HeaderErrorCallback(SUBGHZ_HandleTypeDef *hsubghz)
{
    RadioOnDioIrqCb( IRQ_RADIO_HEADER_ERROR );
}

void HAL_SUBGHZ_PreambleDetectedCallback(SUBGHZ_HandleTypeDef *hsubghz)
{
    RadioOnDioIrqCb( IRQ_RADIO_PREAMBLE_DETECTED );
}

void HAL_SUBGHZ_SyncWordValidCallback(SUBGHZ_HandleTypeDef *hsubghz)
{
    RadioOnDioIrqCb( IRQ_RADIO_SYNC_WORD_VALID );
}

void HAL_SUBGHZ_HeaderValidCallback(SUBGHZ_HandleTypeDef *hsubghz)
{
    RadioOnDioIrqCb( IRQ_RADIO_HEADER_VALID );
}

void HAL_SUBGHZ_LrFhssHopCallback(SUBGHZ_HandleTypeDef *hsubghz)
{
    RadioOnDioIrqCb( IRQ_RADIO_LR_FHSS_HOP );
}

static void Radio_SMPS_Set(uint8_t level)
{
  if ( 1U == RBI_IsDCDC() )
  {
    uint8_t modReg;
    uint8_t tmp = 0;
    SUBGRF_ReadRegisters(SUBGHZ_SMPSC2R, &modReg, 1 );
    modReg&= (~SMPS_DRV_MASK);
    tmp = modReg | level;
	SUBGRF_WriteRegisters(SUBGHZ_SMPSC2R, &tmp, 1 );
  }
}

/* --- EOF ------------------------------------------------------------------ */
