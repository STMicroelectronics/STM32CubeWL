/**
  ******************************************************************************
  * @file    lora_at.h
  * @author  MCD Application Team
  * @brief   Header for driver at.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LORA_AT_H__
#define __LORA_AT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "LmHandler.h"
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
  AT_DUTYCYLE_RESTRICTED,
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
#define AT_RESET      "Z"
#define AT_VL         "+VL"
#define AT_VER        "+VER"

/* MAC Commands */
#define AT_BAND       "+BAND"
#define AT_DEUI       "+DEUI"
#define AT_DADDR      "+DADDR"
#define AT_JOINEUI    "+APPEUI" /*to match with V1.0.x specification- For V1.1.x "+APPEUI" will be replaced by "+JOINEUI"*/
#define AT_ADR        "+ADR"
#define AT_DR         "+DR"
#define AT_TXP        "+TXP"
#define AT_DCS        "+DCS"
#define AT_RX2FQ      "+RX2FQ"
#define AT_RX2DR      "+RX2DR"
#define AT_RX1DL      "+RX1DL"
#define AT_RX2DL      "+RX2DL"
#define AT_JN1DL      "+JN1DL"
#define AT_JN2DL      "+JN2DL"
#define AT_NWKID      "+NWKID"
#define AT_CLASS      "+CLASS"
#define AT_JOIN       "+JOIN"
#define AT_SEND       "+SEND"
#define AT_PGSLOT     "+PGSLOT"
#define AT_LTIME      "+LTIME"
#define AT_REGW       "+REGW"
#define AT_REGR       "+REGR"

/* Radio Tests Commands */
#define AT_TRSSI      "+TRSSI"
#define AT_TTONE      "+TTONE"
#define AT_TTLRA      "+TTLRA"
#define AT_TRLRA      "+TRLRA"
#define AT_TTX        "+TTX"
#define AT_TRX        "+TRX"
#define AT_TTH        "+TTH"
#define AT_TCONF      "+TCONF"
#define AT_TOFF       "+TOFF"
#define AT_CERTIF     "+CERTIF"

/* Peripherical Commands */
#define AT_BAT        "+BAT"

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Event callback on join
  * @param params
  * @retval None
  */
void AT_event_join(LmHandlerJoinParams_t *params);

/**
  * @brief  Event callback on received data
  * @param appData
  * @param params
  * @retval None
  */
void AT_event_receive(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);

/**
  * @brief  Event callback on confirmed acknowledge
  * @param  params
  * @retval None
  */
void AT_event_confirm(LmHandlerTxParams_t *params);

/**
  * @brief  Return AT_OK in all cases
  * @param  Param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_return_ok(const char *param);

/**
  * @brief  Return AT_ERROR in all cases
  * @param  Param string of the AT command - unused
  * @retval AT_ERROR
  */
ATEerror_t AT_return_error(const char *param);

/**
  * @brief  Trig a reset of the MCU
  * @param  Param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_reset(const char *param);

/**
  * @brief  Print actual Active Region
  * @param  Param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_Region_get(const char *param);

/**
  * @brief  Set Active Region
  * @param  Param string of the AT command
  * @retval AT_OK if OK
  */
ATEerror_t AT_Region_set(const char *param);

/**
  * @brief  Print Device EUI
  * @param  Param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_DevEUI_get(const char *param);

/**
  * @brief  Set Device EUI
  * @param  Param string of the AT command
  * @retval AT_OK if OK
  */
ATEerror_t AT_DevEUI_set(const char *param);

/**
  * @brief  Print Join Eui
  * @param  Param string of the AT command
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_JoinEUI_get(const char *param);

/**
  * @brief  Set Join Eui
  * @param  Param string of the AT command
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_JoinEUI_set(const char *param);

/**
  * @brief  Print the DevAddr
  * @param  String pointing to provided DevAddr
  * @retval AT_OK
  */
ATEerror_t AT_DevAddr_get(const char *param);

/**
  * @brief  Set DevAddr
  * @param  String pointing to provided DevAddr
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_DevAddr_set(const char *param);

/**
  * @brief  Print Adaptative Data Rate setting
  * @param  String pointing to provided ADR setting
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_ADR_get(const char *param);

/**
  * @brief  Set Adaptative Data Rate setting
  * @param  String pointing to provided ADR setting
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_ADR_set(const char *param);

/**
  * @brief  Print Data Rate
  * @param  String pointing to provided rate
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_DataRate_get(const char *param);

/**
  * @brief  Set Data Rate
  * @param  String pointing to provided rate
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_DataRate_set(const char *param);

/**
  * @brief  Set ETSI Duty Cycle parameter
  * @param  String pointing to provided Duty Cycle value
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_DutyCycle_set(const char *param);

/**
  * @brief  Get ETSI Duty Cycle parameter
  * @param  0 if disable, 1 if enable
  * @retval AT_OK
  */
ATEerror_t AT_DutyCycle_get(const char *param);

/**
  * @brief  Print Rx2 window frequency
  * @param  String pointing to parameter
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_Rx2Frequency_get(const char *param);

/**
  * @brief  Print Rx2 window data rate
  * @param  String pointing to parameter
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_Rx2DataRate_get(const char *param);

/**
  * @brief  Print Transmit Power
  * @param  String pointing to provided power
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_TransmitPower_get(const char *param);

/**
  * @brief  Print the delay between the end of the Tx and the Rx Window 1
  * @param  String pointing to provided param
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_Rx1Delay_get(const char *param);

/**
  * @brief  Print the delay between the end of the Tx and the Rx Window 2
  * @param  String pointing to provided param
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_Rx2Delay_get(const char *param);

/**
  * @brief  Print the delay between the end of the Tx and the Join Rx Window 1
  * @param  String pointing to provided param
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_JoinAcceptDelay1_get(const char *param);

/**
  * @brief  Print the delay between the end of the Tx and the Join Rx Window 2
  * @param  String pointing to provided param
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_JoinAcceptDelay2_get(const char *param);

/**
  * @brief  Set Transmit Power
  * @param  String pointing to provided power
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_TransmitPower_set(const char *param);

/**
  * @brief  Set Rx2 window frequency
  * @param  String pointing to parameter
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_Rx2Frequency_set(const char *param);

/**
  * @brief  Set Rx2 window data rate
  * @param  String pointing to parameter
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_Rx2DataRate_set(const char *param);

/**
  * @brief  Set the delay between the end of the Tx and the Rx Window 1
  * @param  String pointing to provided param
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_Rx1Delay_set(const char *param);

/**
  * @brief  Set the delay between the end of the Tx and the Rx Window 2
  * @param  String pointing to provided param
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_Rx2Delay_set(const char *param);

/**
  * @brief  Set the delay between the end of the Tx and the Join Rx Window 1
  * @param  String pointing to provided param
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_JoinAcceptDelay1_set(const char *param);

/**
  * @brief  Set the delay between the end of the Tx and the Join Rx Window 2
  * @param  String pointing to provided param
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_JoinAcceptDelay2_set(const char *param);

/**
  * @brief  Print the Network ID
  * @param  String pointing to provided parameter
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_NetworkID_get(const char *param);

/**
  * @brief  Set the Network ID
  * @param  String pointing to provided parameter
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_NetworkID_set(const char *param);

/**
  * @brief  Print the Device Class
  * @param  String pointing to provided param
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_DeviceClass_get(const char *param);

/**
  * @brief  Set the Device Class
  * @param  String pointing to provided param
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_DeviceClass_set(const char *param);

/**
  * @brief  Join a network
  * @param  String parameter
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_Join(const char *param);

/**
  * @brief  Print last received message
  * @param  String parameter
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_Send(const char *param);

/**
  * @brief  Print the version of the AT_Slave FW
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_version_get(const char *param);

/**
  * @brief  Get the battery level
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_bat_get(const char *param);

/**
  * @brief  Get the unicast pingslot periodicity, Data rate and PsFrequency
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_PingSlot_get(const char *param);

/**
  * @brief  Set the unicast pingslot periodicity
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_PingSlot_set(const char *param);

/**
  * @brief  Get the local time in UTC format
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_LocalTime_get(const char *param);

/**
  * @brief  Sart Tx test
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_txTone(const char *param);

/**
  * @brief  Sart Rx tone
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_rxRssi(const char *param);

/**
  * @brief  stop Rx or Tx test
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_stop(const char *param);

/**
  * @brief  Start Tx LoRa test
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_tx(const char *param);

/**
  * @brief  Start Rx LoRa test
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_rx(const char *param);

/**
  * @brief  Start Tx hopping
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_tx_hopping(const char *param);

/**
  * @brief  Set Rx or Tx test config
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_set_config(const char *param);

/**
  * @brief  Get Rx or Tx test config
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_test_get_config(const char *param);

/**
  * @brief  set the Modem in Certif Mode
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_Certif(const char *param);

/**
  * @brief  Get the verbose level
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_verbose_get(const char *param);

/**
  * @brief  Set the verbose level
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_verbose_set(const char *param);

/**
  * @brief  Write Radio Register
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_write_register(const char *param);

/**
  * @brief  Read Radio Register
  * @param  String parameter
  * @retval AT_OK
  */
ATEerror_t AT_read_register(const char *param);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __LORA_AT_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
