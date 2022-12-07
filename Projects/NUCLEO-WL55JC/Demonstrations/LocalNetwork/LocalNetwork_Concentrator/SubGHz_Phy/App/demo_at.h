/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_at.h
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
#ifndef __DEMO_AT_H__
#define __DEMO_AT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sys_app.h" /*For LOG*/



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
  AT_TEST_PARAM_OVERFLOW,
  AT_RX_ERROR,
  AT_MOD_NOT_ALLOWED,
  AT_EUI_NOT_CONNECTED,
  AT_MAX,
} ATEerror_t;

/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* AT printf */
#define AT_PRINTF     APP_PRINTF

/* AT Command strings. Commands start with AT */
#define AT_RESET          "Z"
#define AT_VER            "+VER"
#define AT_VL             "+VL"

#define AT_LIST_REGIONS   "+LIST_REGIONS"

#define AT_DE             "+DE"
#define AT_CR             "+CR"
#define AT_SF             "+SF"
#define AT_BW             "+BW"

#define AT_RISE           "+RISE"
#define AT_BR             "+BR"
#define AT_FDEV           "+FDEV"
#define AT_BT             "+BT"

#define AT_MOD_LORA       "+MOD_LORA"
#define AT_MOD_FSK        "+MOD_FSK"
#define AT_MOD_TEST_LORA  "+MOD_TEST_LORA"
#define AT_MOD_TEST_FSK   "+MOD_TEST_FSK"

#define AT_REGION         "+REGION"
#define AT_SUBREGION      "+SUBREGION"
#define AT_BEACON_ON      "+BEACON_ON"

/* Exported functions ------------------------------------------------------- */

void at_init(void);

/**
  * @brief  Return AT_OK in all cases
  * @param  Param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t at_return_ok(const char *param);

/**
  * @brief  Return AT_ERROR in all cases
  * @param  Param string of the AT command - unused
  * @retval AT_ERROR
  */
ATEerror_t at_return_error(const char *param);

/**
  * @brief  Trig a reset of the MCU
  * @param  Param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t at_reset(const char *param);

/**
  * @brief Get firmware version.
  * @param param
  * @return AT_OK
  */
ATEerror_t at_version_get(const char *param);

/**
  * @brief  Set the verbose level
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t at_verbose_set(const char *param);

/**
  * @brief  Get the verbose level
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t at_verbose_get(const char *param);

ATEerror_t at_list_regions(const char *param);

ATEerror_t at_DE_set(const char *param);
ATEerror_t at_CR_set(const char *param);
ATEerror_t at_SF_set(const char *param);
ATEerror_t at_BW_set(const char *param);
ATEerror_t at_RISE_set(const char *param);
ATEerror_t at_BR_set(const char *param);
ATEerror_t at_FDEV_set(const char *param);
ATEerror_t at_BT_set(const char *param);
ATEerror_t at_REGION_set(const char *param);
ATEerror_t at_SUBREGION_set(const char *param);

ATEerror_t at_DE_get(const char *param);
ATEerror_t at_CR_get(const char *param);
ATEerror_t at_SF_get(const char *param);
ATEerror_t at_BW_get(const char *param);
ATEerror_t at_RISE_get(const char *param);
ATEerror_t at_BR_get(const char *param);
ATEerror_t at_FDEV_get(const char *param);
ATEerror_t at_BT_get(const char *param);
ATEerror_t at_REGION_get(const char *param);
ATEerror_t at_SUBREGION_get(const char *param);

ATEerror_t at_beacon_get(const char *param);
ATEerror_t at_beacon_set(const char *param);
ATEerror_t at_beacon_run(const char *param);

ATEerror_t at_mod_lora(const char *param);
ATEerror_t at_mod_fsk(const char *param);
ATEerror_t at_mod_test_lora(const char *param);
ATEerror_t at_mod_test_fsk(const char *param);

#ifdef __cplusplus
}
#endif

#endif /* __DEMO_AT_H__ */
