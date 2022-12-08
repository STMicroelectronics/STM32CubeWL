/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subg_at.h
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
#ifndef __SUBG_AT_H__
#define __SUBG_AT_H__

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
  AT_NO_NET_JOINED,
  AT_RX_ERROR,
  AT_NO_CLASS_B_ENABLE,
  AT_DUTYCYCLE_RESTRICTED,
  AT_CRYPTO_ERROR,
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
                               != UTIL_ADV_TRACE_COND_FSend(VLEVEL_ALWAYS, T_REG_OFF, TS_OFF, __VA_ARGS__) ) /*Polling Mode*/

/* AT Command strings. Commands start with AT */
/* General commands */
#define AT_VER        "+VER"
#define AT_VL         "+VL"
#define AT_LTIME      "+LTIME"
#define AT_RESET      "Z"

/* Radio tests commands */
#define AT_TTONE      "+TTONE"
#define AT_TRSSI      "+TRSSI"
#define AT_TCONF      "+TCONF"
#define AT_TTX        "+TTX"
#define AT_TRX        "+TRX"
#define AT_TTH        "+TTH"
#define AT_TOFF       "+TOFF"

/* Radio access commands */
#define AT_REGW       "+REGW"
#define AT_REGR       "+REGR"

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

/* --------------- General commands --------------- */
/**
  * @brief  Print the version of the AT_Slave FW
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_version_get(const char *param);

/**
  * @brief  Get the verbose level
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_verbose_get(const char *param);

/**
  * @brief  Set the verbose level
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_verbose_set(const char *param);

/**
  * @brief  Get the local time in UTC format
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_LocalTime_get(const char *param);

/**
  * @brief  Trig a reset of the MCU
  * @param  param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_reset(const char *param);

/* --------------- Radio tests commands --------------- */
/**
  * @brief  Start Tx test
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_txTone(const char *param);

/**
  * @brief  Start Rx tone
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_rxRssi(const char *param);

/**
  * @brief  Get Rx or Tx test config
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_get_config(const char *param);

/**
  * @brief  Set Rx or Tx test config
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_set_config(const char *param);

/**
  * @brief  Start Tx LoRa test
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_tx(const char *param);

/**
  * @brief  Start Rx LoRa test
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_rx(const char *param);

/**
  * @brief  Start Tx hopping
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_tx_hopping(const char *param);

/**
  * @brief  stop Rx or Tx test
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_stop(const char *param);

/* --------------- Radio access commands --------------- */
/**
  * @brief  Write Radio Register
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_write_register(const char *param);

/**
  * @brief  Read Radio Register
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_read_register(const char *param);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __SUBG_AT_H__ */
