/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sgfx_at.h
  * @author  MCD Application Team
  * @brief   Header for driver at.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SGFX_AT_H__
#define __SGFX_AT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_adv_trace.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/*
 * AT Command Id errors. Note that they are in sync with ATError_description static array
 * in command.c
 */
typedef enum eATEerror
{
  AT_OK = 0,
  AT_ERROR,
  AT_PARAM_ERROR,
  AT_BUSY_ERROR,
  AT_TEST_PARAM_OVERFLOW,
  AT_LIB_ERROR,
  AT_TX_TIMEOUT, /*CS Channel Busy*/
  AT_RX_TIMEOUT,
  AT_RX_ERROR,
  AT_RECONF_ERROR,
  AT_MAX,
} ATEerror_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* AT printf */
#define AT_PRINTF(...)     do{  UTIL_ADV_TRACE_COND_FSend(VLEVEL_OFF, T_REG_OFF, TS_OFF, __VA_ARGS__);}while(0)
#define AT_PPRINTF(...)    do{ } while( UTIL_ADV_TRACE_OK \
                               != UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__) ) /* Polling Mode */

/* AT Command strings. Commands start with AT */
#define AT_RESET      "Z"
#define AT_RFS        "$RFS"
#define AT_VER        "+VER"
#define AT_ID         "$ID"
#define AT_PAC        "$PAC"
#define AT_S410       "S410"
#define AT_S411       "S411"
#define AT_SENDB      "$SB"
#define AT_SENDF      "$SF"
#define AT_SENDH      "$SH"
#define AT_TM         "$TM"
#define AT_CW         "$CW"
#define AT_PN         "$PN"
#define AT_MN         "$MN"
#define AT_BAT        "+BAT"
#define AT_S302       "S302"
#define AT_S300       "S300"
#define AT_S400       "S400"
#define AT_RC         "$RC"
#define AT_RSSICAL    "$RSSICAL"
#define ATE           "E"
#define AT_VL         "+VL"

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Return AT_OK in all cases
  * @param  param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_return_ok(const char *param);

/**
  * @brief  Return AT_ERROR in all cases
  * @param  param string of the AT command - unused
  * @retval AT_ERROR
  */
ATEerror_t AT_return_error(const char *param);

/**
  * @brief  Trig a reset of the MCU
  * @param  param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_reset(const char *param);

/**
  * @brief  Restore factory settings in Eeprom
  * @param  param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_restore_factory_settings(const char *param);

/**
  * @brief  Send Bit w/wo ack to Sigfox Nw
  * @param  param String pointing to command parameters
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_SendBit(const char *param);

/**
  * @brief  Send  ASCII frame w/wo ack to Sigfox Nw
  * @param  param String pointing to command parameters
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_SendFrame(const char *param);

/**
  * @brief  Send  Hex frame w/wo ack to Sigfox Nw
  * @param  param String pointing to command parameters
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_SendHexFrame(const char *param);

/**
  * @brief  Print last received message
  * @param  param String parameter
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_Send(const char *param);

/**
  * @brief  Get the DevId
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_DevId_get(const char *param);

/**
  * @brief  Get the Dev Pac
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_DevPac_get(const char *param);

/**
  * @brief  Set public Key
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_PublicKey_set(const char *param);

/**
  * @brief  Get public Key
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_PublicKey_get(const char *param);
/**
  * @brief  Set Payload Encryption
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_PayloadEncryption_set(const char *param);

/**
  * @brief  Get Payload Encryption
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_PayloadEncryption_get(const char *param);

/**
  * @brief  Print the version of the AT_Slave FW
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_version_get(const char *param);

/**
  * @brief  Print the battery level
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_bat_get(const char *param);

/**
  * @brief  Test Tone
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_cw(const char *param);

/**
  * @brief  Tx Test with prbs9 modulation
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_pn(const char *param);

/**
  * @brief  Starts a monarch scan
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_scan_mn(const char *param);
/**
  * @brief  Test Tone
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_mode(const char *param);

/**
  * @brief  set the output power of the radio (power in dBm)
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_power_set(const char *param);

/**
  * @brief  get the output power of the radio (power in dBm)
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_power_get(const char *param);
/**
  * @brief  send an out of band message one
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_outOfBand_run(const char *param);

/**
  * @brief  to configure the enabled channels for FCC
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_ChannelConfigFcc_set(const char *param);

/**
  * @brief  set zones (1 2 3 or 4)
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_rc_set(const char *param);

/**
  * @brief  get zones (1 2 3 or 4)
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_rc_get(const char *param);

/**
  * @brief  to get the rssi calibration value from eeprom.
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_rssi_cal_get(const char *param);

/**
  * @brief  to set the rssi calibration state in eeprom.
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_rssi_cal_set(const char *param);
/**
  * @brief  to get the current echo state from eeprom.
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_echo_get(const char *param);

/**
  * @brief  to set the echo state in eeprom.
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_echo_set(const char *param);
/**
  * @brief  Set the verbose level
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_verbose_set(const char *param);

/**
  * @brief  Get the verbose level
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_verbose_get(const char *param);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __SGFX_AT_H__ */
