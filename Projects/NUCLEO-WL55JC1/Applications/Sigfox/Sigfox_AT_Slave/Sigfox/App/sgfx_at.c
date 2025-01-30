/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sgfx_at.c
  * @author  MCD Application Team
  * @brief   Header for driver sgfx_at.c module
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

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "sgfx_at.h"
#include "sigfox_types.h"
#include "st_sigfox_api.h"
#include "usart_if.h"
#include "stm32_tiny_sscanf.h"
#include "app_version.h"
#include "sigfox_version.h"
#include "subghz_phy_version.h"
#include "sgfx_eeprom_if.h"
#include "sigfox_monarch_api.h"
#include "se_nvm.h"
#include "adc_if.h"
#include "sys_app.h"
#include "stm32_seq.h"
#include "utilities_def.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define MN_ON

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/**
  * @brief Macro to return when an error occurs
  */
#define CHECK_STATUS(status) do {                                                  \
                                  ATEerror_t at_status = translate_status(status); \
                                  if (at_status != AT_OK) { return at_status; }    \
                                } while (0)

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
#ifdef MN_ON

/**
  * @brief      monarch scan callback
  * @param[out] rc_bit_mask bit mask of detected region configuration
  * @param[out] rssi of the detected monarch signal
  * @retval     SFX_ERR_NONE
  */
static sfx_u8 app_callback_handler(sfx_u8 rc_bit_mask, sfx_s16 rssi);

/**
  * @brief  rf test mode call back starting a while wait loop
  */
static void sfx_monarch_test_mode_wait_start_cb(void);

/**
  * @brief  rf test mode call back exiting a while wait loop
  */
static void sfx_monarch_test_mode_wait_end_cb(void);
#endif /* MN_ON */
/**

  * @brief  Print n bytes as %02x
  * @param  pt pointer containing the bytes to print
  * @param  n number of bytes
  */
static void print_n(uint8_t *pt, uint32_t n);

/**
  * @brief  Print received n bytes as %02x
  * @param  pt pointer containing the bytes to print
  * @param  n number of bytes
  */
static void print_rx(uint8_t *pt, uint32_t n);

/**
  * @brief  Print an unsigned int
  * @param  value to print
  */
static void print_u(uint32_t value);

/**
  * @brief  get the length and the number of parameters of the string
  * @param  str the string
  * @param  data output vector
  * @param  dataSize length of the output vector
  */
static ErrorStatus stringToData(const char *str, uint8_t *data, uint32_t *dataSize);

/**
  * @brief  Check if character in parameter is alphanumeric
  * @param  Char for the alphanumeric check
  * @retval ERROR or SUCCESS
  */
static ErrorStatus isHex(char Char);

/**
  * @brief  Converts hex string to a nibble ( 0x0X )
  * @param  Char hex string
  * @retval the nibble. Returns 0xF0 in case input was not an hex string (a..f; A..F or 0..9)
  */
static uint8_t Char2Nibble(char Char);

/**
  * @brief  reopens Sigfox and restores radio configuration as from EEPROM
  * @param  sfx_rc region configuration
  * @param  config_words configuration parameters
  * @return sfx_error_t status
  */
static sfx_error_t SIGFOX_reopen_and_reconf(sfx_rc_enum_t sfx_rc,  sfx_u32 *config_words);

/**
  * @brief  run testmode 12
  * @note   specific for SE_MON_FDL lib as HW SE do not support public switch key
  * @param  rc_enum region configuration
  * @return sfx_error_t status
  */
static sfx_error_t testmode_12(sfx_rc_enum_t rc_enum);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

ATEerror_t AT_return_ok(const char *param)
{
  return AT_OK;
}

ATEerror_t AT_return_error(const char *param)
{
  return AT_ERROR;
}

ATEerror_t AT_reset(const char *param)
{
  /* USER CODE BEGIN AT_reset_1 */

  /* USER CODE END AT_reset_1 */
  NVIC_SystemReset();
  /* USER CODE BEGIN AT_reset_2 */

  /* USER CODE END AT_reset_2 */
}

ATEerror_t AT_restore_factory_settings(const char *param)
{
  /* USER CODE BEGIN AT_restore_factory_settings_1 */

  /* USER CODE END AT_restore_factory_settings_1 */
  E2P_RestoreFs();
  return AT_OK;
  /* USER CODE BEGIN AT_restore_factory_settings_2 */

  /* USER CODE END AT_restore_factory_settings_2 */
}

ATEerror_t AT_SendBit(const char *param)
{
  /* USER CODE BEGIN AT_SendBit_1 */

  /* USER CODE END AT_SendBit_1 */
  uint32_t Bit;
  uint32_t dlFlag = 0;
  uint32_t txRepeat = 1;
  int32_t nbParam;
  uint8_t dl_msg[SGFX_MAX_DL_PAYLOAD_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  sfx_error_t error;

  nbParam = tiny_sscanf(param, "%u,%u,%d", &Bit, &dlFlag, &txRepeat);

  if (nbParam > 3)
  {
    return AT_PARAM_ERROR;
  }

  if (dlFlag == 0)
  {
    /* no dlFlag */
    error = SIGFOX_API_send_bit(Bit, (sfx_u8 *) dl_msg, txRepeat, SFX_FALSE);
    if (error != SFX_ERR_NONE)
    {
      if (error == SFX_ERR_INT_PROCESS_UPLINK_CS_TIMEOUT)
      {
        return AT_TX_TIMEOUT;
      }
      else
      {
        APP_LOG(TS_OFF, VLEVEL_H, "ERROR 0x%04X\r\n", error);
        return AT_LIB_ERROR;
      }
    }
  }
  else
  {
    error = SIGFOX_API_send_bit(Bit, (sfx_u8 *) dl_msg, txRepeat, SFX_TRUE);

    if (error == SFX_ERR_NONE)
    {
      print_rx(dl_msg, SGFX_MAX_DL_PAYLOAD_SIZE * 2);
    }
    else if (error == SFX_ERR_INT_GET_RECEIVED_FRAMES_TIMEOUT)
    {
      return AT_RX_TIMEOUT;
    }
    else
    {
      APP_LOG(TS_OFF, VLEVEL_H, "ERROR 0x%04X\r\n", error);
      return AT_LIB_ERROR;
    }
  }
  return AT_OK;
  /* USER CODE BEGIN AT_SendBit_2 */

  /* USER CODE END AT_SendBit_2 */
}

ATEerror_t AT_SendFrame(const char *param)
{
  /* USER CODE BEGIN AT_SendFrame_1 */

  /* USER CODE END AT_SendFrame_1 */
  uint8_t ul_msg[SGFX_MAX_UL_PAYLOAD_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t dl_msg[SGFX_MAX_DL_PAYLOAD_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint32_t dlFlag = 0;   /* default */
  uint32_t  ul_size = 0;
  uint32_t nbParam;
  uint32_t txRepeat = 1;
  sfx_error_t error;

  /* convert AT param to sgfx param */
  if (stringToData(param, ul_msg, &ul_size) != SUCCESS)
  {
    return AT_PARAM_ERROR;
  }
  if (param[2 * ul_size] == ',')
  {
    nbParam = tiny_sscanf(&param[2 * ul_size + 1], "%u,%u", &dlFlag, &txRepeat);

    if (nbParam > 2)
    {
      return AT_PARAM_ERROR;
    }
  }

  /*Send Bytes to Sigfox Network */
  if (dlFlag == 0)
  {
    error = SIGFOX_API_send_frame(ul_msg, ul_size, dl_msg, txRepeat, SFX_FALSE);
    if (error != SFX_ERR_NONE)
    {
      if (error == SFX_ERR_INT_PROCESS_UPLINK_CS_TIMEOUT)
      {
        return AT_TX_TIMEOUT;
      }
      else
      {
        APP_LOG(TS_OFF, VLEVEL_H, "ERROR 0x%04X\r\n", error);
        return AT_LIB_ERROR;
      }
    }
  }
  else
  {
    error = SIGFOX_API_send_frame(ul_msg, ul_size, dl_msg, txRepeat, SFX_TRUE);

    if (error == SFX_ERR_NONE)
    {
      print_rx(dl_msg, SGFX_MAX_DL_PAYLOAD_SIZE * 2);
    }
    else if (error == SFX_ERR_INT_GET_RECEIVED_FRAMES_TIMEOUT)
    {
      return AT_RX_TIMEOUT;
    }
    else
    {
      APP_LOG(TS_OFF, VLEVEL_H, "ERROR 0x%04X\r\n", error);
      return AT_LIB_ERROR;
    }
  }
  return AT_OK;
  /* USER CODE BEGIN AT_SendFrame_2 */

  /* USER CODE END AT_SendFrame_2 */
}

ATEerror_t AT_SendHexFrame(const char *param)
{
  /* USER CODE BEGIN AT_SendHexFrame_1 */

  /* USER CODE END AT_SendHexFrame_1 */
  uint8_t ul_msg[SGFX_MAX_UL_PAYLOAD_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t dl_msg[SGFX_MAX_DL_PAYLOAD_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint32_t dlFlag = 0;   /* default */
  uint32_t  ul_size = 0;
  uint32_t nbParam;
  uint32_t txRepeat = 1;
  sfx_error_t error;

  /* convert AT param to sgfx param */
  /* ul_size, ul_msg[ul_size], dlFlag, txRepeat*/

  tiny_sscanf(param, "%u", &ul_size);

  if (ul_size > SGFX_MAX_UL_PAYLOAD_SIZE)
  {
    return AT_PARAM_ERROR;
  }
  param += 2;

  for (int32_t i = 0; i < ul_size; i++)
  {
    ul_msg[i] = param[i];
  }

  if (param[ul_size] == ',')
  {
    nbParam = tiny_sscanf(&param[ul_size + 1], "%u,%u", &dlFlag, &txRepeat);

    if (nbParam > 2)
    {
      return AT_PARAM_ERROR;
    }
  }

  /*Send Bytes to Sigfox Network */
  if (dlFlag == 0)
  {
    error = SIGFOX_API_send_frame(ul_msg, ul_size, dl_msg, txRepeat, SFX_FALSE);
    if (error != SFX_ERR_NONE)
    {
      if (error == SFX_ERR_INT_PROCESS_UPLINK_CS_TIMEOUT)
      {
        return AT_TX_TIMEOUT;
      }
      else
      {
        APP_LOG(TS_OFF, VLEVEL_H, "ERROR 0x%04X\r\n", error);
        return AT_LIB_ERROR;
      }
    }
  }
  else
  {
    error = SIGFOX_API_send_frame(ul_msg, ul_size, dl_msg, txRepeat, SFX_TRUE);

    if (error == SFX_ERR_NONE)
    {
      print_rx(dl_msg, SGFX_MAX_DL_PAYLOAD_SIZE * 2);
    }
    else if (error == SFX_ERR_INT_GET_RECEIVED_FRAMES_TIMEOUT)
    {
      return AT_RX_TIMEOUT;
    }
    else
    {
      APP_LOG(TS_OFF, VLEVEL_H, "ERROR 0x%04X\r\n", error);
      return AT_LIB_ERROR;
    }
  }
  return AT_OK;
  /* USER CODE BEGIN AT_SendHexFrame_2 */

  /* USER CODE END AT_SendHexFrame_2 */
}

ATEerror_t AT_version_get(const char *param)
{
  /* USER CODE BEGIN AT_version_get_1 */

  /* USER CODE END AT_version_get_1 */
  sfx_u8 *version;
  sfx_u8 size;

  /* Get Sigfox APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "APPLICATION_VERSION: V%X.%X.%X\r\n",
          (uint8_t)(APP_VERSION_MAIN),
          (uint8_t)(APP_VERSION_SUB1),
          (uint8_t)(APP_VERSION_SUB2));

  /* Get MW Sigfox info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_SIGFOX_VERSION:   V%X.%X.%X\r\n",
          (uint8_t)(SIGFOX_VERSION_MAIN),
          (uint8_t)(SIGFOX_VERSION_SUB1),
          (uint8_t)(SIGFOX_VERSION_SUB2));

  /* Get MW SubGhz_Phy info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:    V%X.%X.%X\r\n",
          (uint8_t)(SUBGHZ_PHY_VERSION_MAIN),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB1),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB2));

  SIGFOX_API_get_version(&version, &size, VERSION_SIGFOX);
  print_n(version, size);
  AT_PRINTF("\r\n");

  SIGFOX_API_get_version(&version, &size, VERSION_RF);
  print_n(version, size);
  AT_PRINTF("\r\n");

  SIGFOX_API_get_version(&version, &size, VERSION_MCU);
  print_n(version, size);
  AT_PRINTF("\r\n");

  SIGFOX_API_get_version(&version, &size, VERSION_SE);
  print_n(version, size);
  AT_PRINTF("\r\n");

  SIGFOX_API_get_version(&version, &size, VERSION_MONARCH);
  print_n(version, size);
  AT_PRINTF("\r\n");

  return AT_OK;
  /* USER CODE BEGIN AT_version_get_2 */

  /* USER CODE END AT_version_get_2 */
}

ATEerror_t AT_DevPac_get(const char *param)
{
  /* USER CODE BEGIN AT_DevPac_get_1 */

  /* USER CODE END AT_DevPac_get_1 */
  sfx_u8 SfxPac[PAC_LENGTH];

  sfx_error_t error;

  error = SIGFOX_API_get_initial_pac(SfxPac);

  if (error != SFX_ERR_NONE)
  {
    return AT_LIB_ERROR;
  }

  for (int32_t i = 0; i < PAC_LENGTH; i++)
  {
    AT_PRINTF("%02X", SfxPac[i]);
  }
  AT_PRINTF("\r\n");
  return AT_OK;
  /* USER CODE BEGIN AT_DevPac_get_2 */

  /* USER CODE END AT_DevPac_get_2 */
}

ATEerror_t AT_DevId_get(const char *param)
{
  /* USER CODE BEGIN AT_DevId_get_1 */

  /* USER CODE END AT_DevId_get_1 */
  sfx_u8 SfxId[ID_LENGTH];

  sfx_error_t error;

  error = SIGFOX_API_get_device_id(SfxId);

  if (error != SFX_ERR_NONE)
  {
    return AT_LIB_ERROR;
  }
  for (int32_t i = ID_LENGTH; i > 0; i--)
  {
    AT_PRINTF("%02X", SfxId[i - 1]);
  }
  AT_PRINTF("\r\n");
  return AT_OK;
  /* USER CODE BEGIN AT_DevId_get_2 */

  /* USER CODE END AT_DevId_get_2 */
}

ATEerror_t AT_PublicKey_set(const char *param)
{
  /* USER CODE BEGIN AT_PublicKey_set_1 */

  /* USER CODE END AT_PublicKey_set_1 */
  if ((param[0] == '0') && (param[1] == '\0'))
  {
    E2P_Write_KeyType(CREDENTIALS_KEY_PRIVATE);
  }
  else if ((param[0] == '1') && (param[1] == '\0'))
  {
    E2P_Write_KeyType(CREDENTIALS_KEY_PUBLIC);
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_PublicKey_set_2 */

  /* USER CODE END AT_PublicKey_set_2 */
}

ATEerror_t AT_PublicKey_get(const char *param)
{
  /* USER CODE BEGIN AT_PublicKey_get_1 */

  /* USER CODE END AT_PublicKey_get_1 */
  AT_PRINTF("%d\r\n", (uint8_t) E2P_Read_KeyType());

  return AT_OK;
  /* USER CODE BEGIN AT_PublicKey_get_2 */

  /* USER CODE END AT_PublicKey_get_2 */
}

ATEerror_t AT_PayloadEncryption_set(const char *param)
{
  /* USER CODE BEGIN AT_PayloadEncryption_set_1 */

  /* USER CODE END AT_PayloadEncryption_set_1 */
  if ((param[0] == '0') && (param[1] == '\0'))
  {
    E2P_Write_EncryptionFlag(0);
  }
  else if ((param[0] == '1') && (param[1] == '\0'))
  {
    E2P_Write_EncryptionFlag(1);
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_PayloadEncryption_set_2 */

  /* USER CODE END AT_PayloadEncryption_set_2 */
}

ATEerror_t AT_PayloadEncryption_get(const char *param)
{
  /* USER CODE BEGIN AT_PayloadEncryption_get_1 */

  /* USER CODE END AT_PayloadEncryption_get_1 */
  AT_PRINTF("%d\r\n", (uint8_t) E2P_Read_EncryptionFlag());

  return AT_OK;
  /* USER CODE BEGIN AT_PayloadEncryption_get_2 */

  /* USER CODE END AT_PayloadEncryption_get_2 */
}

ATEerror_t AT_bat_get(const char *param)
{
  /* USER CODE BEGIN AT_bat_get_1 */

  /* USER CODE END AT_bat_get_1 */
  print_u(SYS_GetBatteryLevel());
  return AT_OK;
  /* USER CODE BEGIN AT_bat_get_2 */

  /* USER CODE END AT_bat_get_2 */
}

ATEerror_t AT_test_cw(const char *param)
{
  /* USER CODE BEGIN AT_test_cw_1 */

  /* USER CODE END AT_test_cw_1 */
  uint32_t freq = 0;

  if (tiny_sscanf(param, "%u", &freq) > 1)
  {
    return AT_PARAM_ERROR;
  }

  if (freq == 0)
  {
    sfx_rc_enum_t sfx_rc = E2P_Read_Rc();

    sfx_u32 config_words[3] = {0};

    E2P_Read_ConfigWords(sfx_rc, config_words);

    SIGFOX_API_stop_continuous_transmission();
    /*reopen after test*/
    if (SIGFOX_reopen_and_reconf(sfx_rc, config_words) != SFX_ERR_NONE)
    {
      return AT_RECONF_ERROR;
    }
  }
  else if ((freq > ((uint32_t) 100e6)) && (freq < ((uint32_t) 1e9)))
  {
    SIGFOX_API_close();
    if (SIGFOX_API_start_continuous_transmission(freq, SFX_NO_MODULATION) != SFX_ERR_NONE)
    {
      return AT_PARAM_ERROR;
    }
  }
  else if ((freq >  100) && (freq <  1000))
  {
    /* user meant Mega... */
    SIGFOX_API_close();
    if (SIGFOX_API_start_continuous_transmission(freq * 1000000, SFX_NO_MODULATION) != SFX_ERR_NONE)
    {
      return AT_PARAM_ERROR;
    }
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_test_cw_2 */

  /* USER CODE END AT_test_cw_2 */
}

ATEerror_t AT_test_pn(const char *param)
{
  /* USER CODE BEGIN AT_test_pn_1 */

  /* USER CODE END AT_test_pn_1 */
  uint32_t freq = 0;
  uint32_t bitrate = 0;
  sfx_modulation_type_t sfx_bitrate = SFX_DBPSK_100BPS;

  if (tiny_sscanf(param, "%u,%u", &freq, &bitrate) > 2)
  {
    return AT_PARAM_ERROR;
  }

  if (bitrate == 100)
  {
    sfx_bitrate = SFX_DBPSK_100BPS;
  }
  else if (bitrate == 600)
  {
    sfx_bitrate = SFX_DBPSK_600BPS;
  }
  else if (bitrate == 0)
  {
    if (freq != 0)
    {
      return AT_PARAM_ERROR;
    }
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  if (freq == 0)
  {
    sfx_rc_enum_t sfx_rc = E2P_Read_Rc();

    sfx_u32 config_words[3] = {0};

    E2P_Read_ConfigWords(sfx_rc, config_words);

    SIGFOX_API_stop_continuous_transmission();
    /*reopen after test*/
    if (SIGFOX_reopen_and_reconf(sfx_rc, config_words) != SFX_ERR_NONE)
    {
      return AT_RECONF_ERROR;
    }
  }
  else if ((freq > ((uint32_t) 100e6)) && (freq < ((uint32_t) 1e9)))
  {
    SIGFOX_API_close();
    if (SIGFOX_API_start_continuous_transmission(freq, sfx_bitrate) != SFX_ERR_NONE)
    {
      return AT_PARAM_ERROR;
    }
  }
  else if ((freq >  100) && (freq <  1000))
  {
    /* user meant Mega... */
    SIGFOX_API_close();
    if (SIGFOX_API_start_continuous_transmission(freq * 1000000, sfx_bitrate) != SFX_ERR_NONE)
    {
      return AT_PARAM_ERROR;
    }
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_test_pn_2 */

  /* USER CODE END AT_test_pn_2 */
}

ATEerror_t AT_scan_mn(const char *param)
{
  /* USER CODE BEGIN AT_scan_mn_1 */

  /* USER CODE END AT_scan_mn_1 */
#ifdef MN_ON
  ATEerror_t at_status = AT_OK;

  sfx_u8 rc_capabilities_bit_mask = 0x7F; /*all RCs from RC1 to RC7*/
  sfx_u16 timer = 5;

  sfx_rc_enum_t sfx_rc = E2P_Read_Rc();

  sfx_u32 config_words[3] = {0};

  E2P_Read_ConfigWords(sfx_rc, config_words);

  if (tiny_sscanf(param, "%u", &timer) > 1)
  {
    return AT_PARAM_ERROR;
  }
  SIGFOX_API_close();

  SIGFOX_MONARCH_API_execute_rc_scan(rc_capabilities_bit_mask, timer, SFX_TIME_S, app_callback_handler);

  UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_Monarch);

  if (SIGFOX_reopen_and_reconf(sfx_rc, config_words) != SFX_ERR_NONE)
  {
    at_status = AT_RECONF_ERROR;
  }

  return at_status;
#else
  return AT_ERROR;
#endif /* MN_ON */
  /* USER CODE BEGIN AT_scan_mn_2 */

  /* USER CODE END AT_scan_mn_2 */
}

#ifdef MN_ON
static sfx_u8 app_callback_handler(sfx_u8 rc_bit_mask, sfx_s16 rssi)
{
  /* USER CODE BEGIN app_callback_handler_1 */

  /* USER CODE END app_callback_handler_1 */
  switch (rc_bit_mask)
  {
    case 0:
    {
      AT_PRINTF("NO RC found\r\n");
      break;
    }
    case (1<<SFX_BITFIELD_SHIFT_RC1):
    {
      AT_PRINTF("RC 1 found\r\n");
      break;
    }
    case (1<<SFX_BITFIELD_SHIFT_RC2):
    {
      AT_PRINTF("RC 2 found\r\n");
      break;
    }
    case (1<<SFX_BITFIELD_SHIFT_RC3):
    {
      AT_PRINTF("RC 3 found\r\n");
      break;
    }
    case (1<<SFX_BITFIELD_SHIFT_RC4):
    {
      AT_PRINTF("RC 4 found\r\n");
      break;
    }
    case (1<<SFX_BITFIELD_SHIFT_RC5):
    {
      AT_PRINTF("RC 5 found\r\n");
      break;
    }
    case (1<<SFX_BITFIELD_SHIFT_RC6):
    {
      AT_PRINTF("RC 6 found\r\n");
      break;
    }
    case (1<<SFX_BITFIELD_SHIFT_RC7):
    {
      AT_PRINTF("RC 7 found\r\n");
      break;
    }
    default:
      AT_PRINTF("RC %d found\r\n", rc_bit_mask);
      break;
  }

  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_Monarch);

  return SFX_ERR_NONE;
  /* USER CODE BEGIN app_callback_handler_2 */

  /* USER CODE END app_callback_handler_2 */
}

static void sfx_monarch_test_mode_wait_start_cb(void)
{
  UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_Monarch);
}

static void sfx_monarch_test_mode_wait_end_cb(void)
{
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_Monarch);
}
#endif /* MN_ON */

ATEerror_t AT_test_mode(const char *param)
{
  /* USER CODE BEGIN AT_test_mode_1 */

  /* USER CODE END AT_test_mode_1 */
  char test_mode;
  sfx_test_mode_t tm;
  sfx_rc_enum_t rc = SFX_RC1;
  sfx_error_t sfx_error;
#ifdef MN_ON
  sfx_u8 rc_capabilities_bit_mask = 0x7F;
#endif /* MN_ON */
  sfx_u32 config_words_restore[3] = {0};

  sfx_rc_enum_t rc_enum_restore = E2P_Read_Rc();

  ATEerror_t at_status = AT_OK;

  if ((param[0] == '1') && (param[1] == ','))
  {
    rc = SFX_RC1;
    param += 2;
  }
  else if ((param[0] == '2') && (param[1] == ','))
  {
    rc = SFX_RC2;
    param += 2;
  }
  else if ((param[0] == '3') && (param[2] == ','))
  {
    if ((param[1] == 'A') || (param[1] == 'a'))
    {
      rc = SFX_RC3A;
      param += 3;
    }
    else if ((param[1] == 'C') || (param[1] == 'c'))
    {
      rc = SFX_RC3C;
      param += 3;
    }
    else
    {
      return AT_PARAM_ERROR;
    }
  }
  else if ((param[0] == '4') && (param[1] == ','))
  {
    rc = SFX_RC4;
    param += 2;
  }
  else if ((param[0] == '5') && (param[1] == ','))
  {
    rc = SFX_RC5;
    param += 2;
  }
  else if ((param[0] == '6') && (param[1] == ','))
  {
    rc = SFX_RC6;
    param += 2;
  }
  else if ((param[0] == '7') && (param[1] == ','))
  {
    rc = SFX_RC7;
    param += 2;
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  tiny_sscanf(param, "%u", &test_mode);

  if (test_mode < 14)
  {
    tm = (sfx_test_mode_t)(test_mode);
  }
  else
  {
    return AT_PARAM_ERROR;
  }

  SIGFOX_API_close();
  /*rc not propagated to mcu_delay. Set it now*/
  E2P_Write_Rc(rc);

  if ((test_mode >= 7) && (test_mode <= 10))
  {
#ifdef MN_ON
    sfx_error = ST_ADDON_SIGFOX_RF_PROTOCOL_API_monarch_test_mode_async(rc, tm, rc_capabilities_bit_mask,
                                                                        sfx_monarch_test_mode_wait_start_cb,
                                                                        sfx_monarch_test_mode_wait_end_cb);
#else
    at_status = AT_ERROR;
#endif /* MN_ON */
  }
  else if (test_mode == 12)
  {
    sfx_error = testmode_12(rc);
  }
  else
  {
    sfx_error = ADDON_SIGFOX_RF_PROTOCOL_API_test_mode(rc, tm);
  }

  /*run command*/
  if (sfx_error != SFX_ERR_NONE)
  {
    if (sfx_error == SFX_ERR_INT_GET_RECEIVED_FRAMES_TIMEOUT)
    {
      at_status = AT_RX_TIMEOUT;
    }
    else
    {
      APP_LOG(TS_OFF, VLEVEL_L, "ERROR 0x%02X", sfx_error);
      at_status = AT_ERROR;
    }
  }
  /*restore config*/
  E2P_Write_Rc(rc_enum_restore);

  E2P_Read_ConfigWords(rc_enum_restore, config_words_restore);

  if (SIGFOX_reopen_and_reconf(rc_enum_restore, config_words_restore) != SFX_ERR_NONE)
  {
    at_status = AT_RECONF_ERROR;
  }
  return at_status;
  /* USER CODE BEGIN AT_test_mode_2 */

  /* USER CODE END AT_test_mode_2 */
}

ATEerror_t AT_power_set(const char *param)
{
  /* USER CODE BEGIN AT_power_set_1 */

  /* USER CODE END AT_power_set_1 */
  int8_t power;

  if (tiny_sscanf(param, "%hhd", &power) != 1)
  {
    return AT_PARAM_ERROR;
  }
  E2P_Write_Power(E2P_Read_Rc(), power);

  return AT_OK;
  /* USER CODE BEGIN AT_power_set_2 */

  /* USER CODE END AT_power_set_2 */
}

ATEerror_t AT_power_get(const char *param)
{
  /* USER CODE BEGIN AT_power_get_1 */

  /* USER CODE END AT_power_get_1 */
  AT_PRINTF("%d\r\n", E2P_Read_Power(E2P_Read_Rc()));

  return AT_OK;
  /* USER CODE BEGIN AT_power_get_2 */

  /* USER CODE END AT_power_get_2 */
}

ATEerror_t AT_outOfBand_run(const char *param)
{
  /* USER CODE BEGIN AT_outOfBand_run_1 */

  /* USER CODE END AT_outOfBand_run_1 */
  SIGFOX_API_send_outofband(SFX_OOB_SERVICE);

  return AT_OK;
  /* USER CODE BEGIN AT_outOfBand_run_2 */

  /* USER CODE END AT_outOfBand_run_2 */
}

ATEerror_t AT_ChannelConfigFcc_set(const char *param)
{
  /* USER CODE BEGIN AT_ChannelConfigFcc_set_1 */

  /* USER CODE END AT_ChannelConfigFcc_set_1 */
  sfx_u32 config_words[3] = {0};
  sfx_u16 timer_state;
  int32_t i;
  int32_t j;
  uint8_t nibble;
  sfx_rc_enum_t sfx_rc = E2P_Read_Rc();

  /* can change the value only in SFX_FH */
  if ((sfx_rc != SFX_RC2) && (sfx_rc != SFX_RC4))
  {
    return AT_PARAM_ERROR;
  }

  for (i = 0, j = (2 * 32 + 28); i < 24; i++, j -= 4)
  {
    nibble = Char2Nibble(*param++);
    if (nibble < 16)
    {
      config_words[i / 8] |= (nibble << (j % 32));
    }
    else
    {
      return AT_PARAM_ERROR;
    }
  }

  if (tiny_sscanf(param, ",%hu", &timer_state) != 1)
  {
    return AT_PARAM_ERROR;
  }

  E2P_Write_ConfigWords(sfx_rc, config_words);

  if (sfx_rc == SFX_RC2)
  {
    sfx_rc_t rc = RC2;

    if ((SIGFOX_API_close()) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
    if ((SIGFOX_API_open(&rc)) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
    if (SIGFOX_API_set_std_config(config_words, (sfx_bool) timer_state) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
  }

  if (sfx_rc == SFX_RC4)
  {
    sfx_rc_t rc = RC4;

    if ((SIGFOX_API_close()) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
    if ((SIGFOX_API_open(&rc)) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
    if (SIGFOX_API_set_std_config(config_words, (sfx_bool) timer_state) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
  }

  return AT_OK;
  /* USER CODE BEGIN AT_ChannelConfigFcc_set_2 */

  /* USER CODE END AT_ChannelConfigFcc_set_2 */
}

ATEerror_t AT_rc_get(const char *param)
{
  /* USER CODE BEGIN AT_rc_get_1 */

  /* USER CODE END AT_rc_get_1 */
  sfx_rc_enum_t sfx_rc = E2P_Read_Rc();

  /*record RCZ*/
  switch (sfx_rc)
  {
    case SFX_RC1:
    {
      AT_PRINTF("1\r\n");
      break;
    }
    case SFX_RC2:
    {
      AT_PRINTF("2\r\n");
      break;
    }
    case SFX_RC3A:
    {
      AT_PRINTF("3A\r\n");
      break;
    }
    case SFX_RC3C:
    {
      AT_PRINTF("3C\r\n");
      break;
    }
    case SFX_RC4:
    {
      AT_PRINTF("4\r\n");
      break;
    }
    case SFX_RC5:
    {
      AT_PRINTF("5\r\n");
      break;
    }
    case SFX_RC6:
    {
      AT_PRINTF("6\r\n");
      break;
    }
    case SFX_RC7:
    {
      AT_PRINTF("7\r\n");
      break;
    }
    default:
      break;
  }
  return AT_OK;
  /* USER CODE BEGIN AT_rc_get_2 */

  /* USER CODE END AT_rc_get_2 */
}

/*AT$RC={1, 2, 3, 4}*/
ATEerror_t AT_rc_set(const char *param)
{
  /* USER CODE BEGIN AT_rc_set_1 */

  /* USER CODE END AT_rc_set_1 */
  /*record RC*/
  if (param[0] == '1' && param[1] == '\0')
  {
    sfx_rc_enum_t SgfxRcId = SFX_RC1;
    sfx_rc_t SgfxRc = RC1;

    E2P_Write_Rc(SgfxRcId);

    if ((SIGFOX_API_close()) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
    if ((SIGFOX_API_open(&SgfxRc)) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
  }
  else if (param[0] == '2' && param[1] == '\0')
  {
    sfx_u32 config_words[3];

    sfx_rc_enum_t SgfxRcId = SFX_RC2;
    sfx_rc_t SgfxRc = RC2;

    E2P_Write_Rc(SgfxRcId);

    E2P_Read_ConfigWords(SgfxRcId, config_words);

    if ((SIGFOX_API_close()) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
    if ((SIGFOX_API_open(&SgfxRc)) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }

    if (SIGFOX_API_set_std_config(config_words, RC2_SET_STD_TIMER_ENABLE) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
  }
  else if (param[0] == '3')
  {
    if (((param[1] == 'a') || (param[1] == 'A')) && (param[2] == '\0'))
    {
      /* RC3C */
      sfx_u32 config_words[3];

      sfx_rc_enum_t SgfxRcId = SFX_RC3A;
      sfx_rc_t SgfxRc = RC3A;

      E2P_Write_Rc(SgfxRcId);

      E2P_Read_ConfigWords(SgfxRcId, config_words);

      if ((SIGFOX_API_close()) != SFX_ERR_NONE)
      {
        return AT_ERROR;
      }
      if ((SIGFOX_API_open(&SgfxRc)) != SFX_ERR_NONE)
      {
        return AT_ERROR;
      }

      if (SIGFOX_API_set_std_config(config_words, NA) != SFX_ERR_NONE)
      {
        return AT_ERROR;
      }
    }
    else  if (((param[1] == 'c') || (param[1] == 'C')) && (param[2] == '\0'))
    {
      /* RC3C */
      sfx_u32 config_words[3];

      sfx_rc_enum_t SgfxRcId = SFX_RC3C;
      sfx_rc_t SgfxRc = RC3C;

      E2P_Write_Rc(SgfxRcId);

      E2P_Read_ConfigWords(SgfxRcId, config_words);

      if ((SIGFOX_API_close()) != SFX_ERR_NONE)
      {
        return AT_ERROR;
      }
      if ((SIGFOX_API_open(&SgfxRc)) != SFX_ERR_NONE)
      {
        return AT_ERROR;
      }

      if (SIGFOX_API_set_std_config(config_words, NA) != SFX_ERR_NONE)
      {
        return AT_ERROR;
      }
    }
    else
    {
      return AT_PARAM_ERROR;
    }
  }
  else if (param[0] == '4' && param[1] == '\0')
  {
    sfx_u32 config_words[3];

    sfx_rc_enum_t SgfxRcId = SFX_RC4;
    sfx_rc_t SgfxRc = RC4;

    E2P_Write_Rc(SgfxRcId);

    E2P_Read_ConfigWords(SgfxRcId, config_words);

    if ((SIGFOX_API_close()) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
    if ((SIGFOX_API_open(&SgfxRc)) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }

    if (SIGFOX_API_set_std_config(config_words, RC4_SET_STD_TIMER_ENABLE) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }

  }
  else if (param[0] == '5' && param[1] == '\0')
  {
    sfx_u32 config_words[3];

    sfx_rc_enum_t SgfxRcId = SFX_RC5;
    sfx_rc_t SgfxRc = RC5;

    E2P_Write_Rc(SgfxRcId);

    E2P_Read_ConfigWords(SgfxRcId, config_words);

    if ((SIGFOX_API_close()) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
    if ((SIGFOX_API_open(&SgfxRc)) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }

    if (SIGFOX_API_set_std_config(config_words, NA) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
  }
  else  if (param[0] == '6' && param[1] == '\0')
  {
    sfx_rc_enum_t SgfxRcId = SFX_RC6;
    sfx_rc_t SgfxRc = RC6;

    E2P_Write_Rc(SgfxRcId);

    if ((SIGFOX_API_close()) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
    if ((SIGFOX_API_open(&SgfxRc
                        )) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }

  }
  else  if (param[0] == '7' && param[1] == '\0')
  {
    sfx_rc_enum_t SgfxRcId = SFX_RC7;
    sfx_rc_t SgfxRc = RC7;

    E2P_Write_Rc(SgfxRcId);

    if ((SIGFOX_API_close()) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }
    if ((SIGFOX_API_open(&SgfxRc
                        )) != SFX_ERR_NONE)
    {
      return AT_ERROR;
    }

  }
  else
  {
    return AT_PARAM_ERROR;
  }

  return AT_OK;
  /* USER CODE BEGIN AT_rc_set_2 */

  /* USER CODE END AT_rc_set_2 */
}

ATEerror_t AT_rssi_cal_set(const char *param)
{
  /* USER CODE BEGIN AT_rssi_cal_set_1 */

  /* USER CODE END AT_rssi_cal_set_1 */
  int16_t rssi_cal;
  if (tiny_sscanf(param, "%hd", &rssi_cal) != 1)
  {
    return AT_PARAM_ERROR;
  }

  E2P_Write_RssiCal(rssi_cal);

  return AT_OK;
  /* USER CODE BEGIN AT_rssi_cal_set_2 */

  /* USER CODE END AT_rssi_cal_set_2 */
}

ATEerror_t AT_rssi_cal_get(const char *param)
{
  /* USER CODE BEGIN AT_rssi_cal_get_1 */

  /* USER CODE END AT_rssi_cal_get_1 */
  int16_t rssi_cal =  E2P_Read_RssiCal();
  AT_PRINTF("%d dB\r\n", rssi_cal);

  return AT_OK;
  /* USER CODE BEGIN AT_rssi_cal_get_2 */

  /* USER CODE END AT_rssi_cal_get_2 */
}

ATEerror_t AT_echo_set(const char *param)
{
  /* USER CODE BEGIN AT_echo_set_1 */

  /* USER CODE END AT_echo_set_1 */
  uint32_t echoState;
  if (tiny_sscanf(param, "%u", &echoState) != 1)
  {
    return AT_PARAM_ERROR;
  }

  if (echoState == 0)
  {
    E2P_Write_AtEcho(echoState);
  }
  else if (echoState == 1)
  {
    E2P_Write_AtEcho(echoState);
  }
  else
  {
    return AT_PARAM_ERROR;
  }
  return AT_OK;
  /* USER CODE BEGIN AT_echo_set_2 */

  /* USER CODE END AT_echo_set_2 */
}

ATEerror_t AT_echo_get(const char *param)
{
  /* USER CODE BEGIN AT_echo_get_1 */

  /* USER CODE END AT_echo_get_1 */
  uint32_t echoState = E2P_Read_AtEcho();

  AT_PRINTF("%d\r\n", echoState);
  return AT_OK;
  /* USER CODE BEGIN AT_echo_get_2 */

  /* USER CODE END AT_echo_get_2 */
}

ATEerror_t AT_verbose_set(const char *param)
{
  /* USER CODE BEGIN AT_verbose_set_1 */

  /* USER CODE END AT_verbose_set_1 */
  const char *buf = param;
  int32_t lvl_nb;

  /* read and set the verbose level */
  if (1 != tiny_sscanf(buf, "%u", &lvl_nb))
  {
    AT_PRINTF("AT+VL: verbose level is not well set");
    return AT_PARAM_ERROR;
  }
  if ((lvl_nb > VLEVEL_H) || (lvl_nb < VLEVEL_OFF))
  {
    AT_PRINTF("AT+VL: verbose level out of range => 0(VLEVEL_OFF) to 3(VLEVEL_H)");
    return AT_PARAM_ERROR;
  }

  UTIL_ADV_TRACE_SetVerboseLevel(lvl_nb);
  /*write in eeprom*/
  E2P_Write_VerboseLevel(lvl_nb);

  return AT_OK;
  /* USER CODE BEGIN AT_verbose_set_2 */

  /* USER CODE END AT_verbose_set_2 */
}

ATEerror_t AT_verbose_get(const char *param)
{
  /* USER CODE BEGIN AT_verbose_get_1 */

  /* USER CODE END AT_verbose_get_1 */
  print_u(UTIL_ADV_TRACE_GetVerboseLevel());

  return AT_OK;
  /* USER CODE BEGIN AT_verbose_get_2 */

  /* USER CODE END AT_verbose_get_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

static void print_rx(uint8_t *pt, uint32_t n)
{
  /* USER CODE BEGIN print_rx_1 */

  /* USER CODE END print_rx_1 */
  AT_PRINTF("\r\n+RX=");
  for (int32_t i = 0; i < n; i++)
  {
    AT_PRINTF("%c", pt[i]);
  }
  AT_PRINTF("\r\n");
  /* USER CODE BEGIN print_rx_2 */

  /* USER CODE END print_rx_2 */
}

static void print_n(uint8_t *pt, uint32_t n)
{
  /* USER CODE BEGIN print_n_1 */

  /* USER CODE END print_n_1 */
  for (int32_t i = 0; i < n; i++)
  {
    AT_PRINTF("%c", pt[i]);
  }
  /* USER CODE BEGIN print_n_2 */

  /* USER CODE END print_n_2 */
}

static void print_u(uint32_t value)
{
  /* USER CODE BEGIN print_u_1 */

  /* USER CODE END print_u_1 */
  AT_PRINTF("%u\r\n", value);
  /* USER CODE BEGIN print_u_2 */

  /* USER CODE END print_u_2 */
}

static ErrorStatus stringToData(const char *str, uint8_t *data, uint32_t *dataSize)
{
  /* USER CODE BEGIN stringToData_1 */

  /* USER CODE END stringToData_1 */
  uint8_t ii = 0;
  char hex[3];
  hex[2] = 0;
  while ((*str != '\0') && (*str != ','))
  {
    hex[0] = *str++;
    hex[1] = *str++;

    /*check if input is hex */
    if ((isHex(hex[0]) == ERROR) || (isHex(hex[1]) == ERROR))
    {
      return ERROR;
    }
    /*check if input is even nb of character*/
    if ((hex[1] == '\0') || (hex[1] == ','))
    {
      return ERROR;
    }
    if (tiny_sscanf(hex, "%hhx", &data[ii]) != 1)
    {
      return ERROR;
    }
    ii++;
    if (ii > SGFX_MAX_UL_PAYLOAD_SIZE)
    {
      return ERROR;
    }
  }
  *dataSize = ii;
  if (ii == 0)
  {
    return ERROR;
  }
  return SUCCESS;
  /* USER CODE BEGIN stringToData_2 */

  /* USER CODE END stringToData_2 */
}

static ErrorStatus isHex(char Char)
{
  /* USER CODE BEGIN isHex_1 */

  /* USER CODE END isHex_1 */
  if (((Char >= '0') && (Char <= '9')) ||
      ((Char >= 'a') && (Char <= 'f')) ||
      ((Char >= 'A') && (Char <= 'F')))
  {
    return SUCCESS;
  }
  else
  {
    return ERROR;
  }
  /* USER CODE BEGIN isHex_2 */

  /* USER CODE END isHex_2 */
}

static uint8_t Char2Nibble(char Char)
{
  /* USER CODE BEGIN Char2Nibble_1 */

  /* USER CODE END Char2Nibble_1 */
  if (((Char >= '0') && (Char <= '9')))
  {
    return Char - '0';
  }
  else if (((Char >= 'a') && (Char <= 'f')))
  {
    return Char - 'a' + 10;
  }
  else if ((Char >= 'A') && (Char <= 'F'))
  {
    return Char - 'A' + 10;
  }
  else
  {
    return 0xF0;
  }
  /* USER CODE BEGIN Char2Nibble_2 */

  /* USER CODE END Char2Nibble_2 */
}

static sfx_error_t SIGFOX_reopen_and_reconf(sfx_rc_enum_t sfx_rc,  sfx_u32 *config_words)
{
  /* USER CODE BEGIN SIGFOX_reopen_and_reconf_1 */

  /* USER CODE END SIGFOX_reopen_and_reconf_1 */
  sfx_error_t error = SFX_ERR_NONE;

  /*record RCZ*/
  switch (sfx_rc)
  {
    case SFX_RC1:
    {
      sfx_rc_t SgfxRc = RC1;
      error = SIGFOX_API_open(&SgfxRc);
      break;
    }
    case SFX_RC2:
    {
      sfx_rc_t SgfxRc = RC2;

      error = SIGFOX_API_open(&SgfxRc);

      if (error == SFX_ERR_NONE)
      {
        error = SIGFOX_API_set_std_config(config_words, RC2_SET_STD_TIMER_ENABLE);
      }

      break;
    }
    case SFX_RC3A:
    {
      sfx_rc_t SgfxRc = RC3A;

      error = SIGFOX_API_open(&SgfxRc);

      if (error == SFX_ERR_NONE)
      {
        error = SIGFOX_API_set_std_config(config_words, NA);
      }

      break;
    }
    case SFX_RC3C:
    {
      sfx_rc_t SgfxRc = RC3C;

      error = SIGFOX_API_open(&SgfxRc);

      if (error == SFX_ERR_NONE)
      {
        error = SIGFOX_API_set_std_config(config_words, NA);
      }

      break;
    }
    case SFX_RC4:
    {
      sfx_rc_t SgfxRc = RC4;

      error = SIGFOX_API_open(&SgfxRc);

      if (error == SFX_ERR_NONE)
      {
        error = SIGFOX_API_set_std_config(config_words, RC4_SET_STD_TIMER_ENABLE);
      }

      break;
    }
    case SFX_RC5:
    {
      sfx_rc_t SgfxRc = RC5;

      error = SIGFOX_API_open(&SgfxRc);

      if (error == SFX_ERR_NONE)
      {
        error = SIGFOX_API_set_std_config(config_words, NA);
      }

      break;
    }
    case SFX_RC6:
    {
      sfx_rc_t SgfxRc = RC6;

      error = SIGFOX_API_open(&SgfxRc);

      break;
    }
    case SFX_RC7:
    {
      sfx_rc_t SgfxRc = RC7;

      error = SIGFOX_API_open(&SgfxRc);

      break;
    }
    default:
    {
      error = SFX_ERR_API_OPEN;
      break;
    }
  }
  return error;
  /* USER CODE BEGIN SIGFOX_reopen_and_reconf_2 */

  /* USER CODE END SIGFOX_reopen_and_reconf_2 */
}

static sfx_error_t testmode_12(sfx_rc_enum_t rc_enum)
{
  /* USER CODE BEGIN testmode_12_1 */

  /* USER CODE END testmode_12_1 */
  sfx_error_t sfx_err = SFX_ERR_NONE;

  sfx_u8 test_data[1];

  sfx_key_type_t key = E2P_Read_KeyType();

  test_data[0] = 0xAB;

  sfx_u32 config_words[3] = {0};

  E2P_Read_ConfigWords(rc_enum, config_words);

  /*open for test mode 12 with right rc_enum and conf*/
  sfx_err = SIGFOX_reopen_and_reconf(rc_enum, config_words);

  /* Switch public key ON */
  E2P_Write_KeyType(CREDENTIALS_KEY_PUBLIC);

  /* Only call SIGFOX_API_send_xxx */
  sfx_err = SIGFOX_API_send_frame(test_data, 1 /* size of the test data */, (sfx_u8 *)SFX_NULL, 2, SFX_FALSE);

  /* Switch back key type */
  E2P_Write_KeyType(key);

  /* Close the lib before return */
  SIGFOX_API_close();

  return sfx_err;
  /* USER CODE BEGIN testmode_12_2 */

  /* USER CODE END testmode_12_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
