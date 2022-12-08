/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    msg_id.h
  * @author  MCD Application Team
  * @brief   MBMUX message ID enumeration
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
#ifndef __MSG_ID_H__
#define __MSG_ID_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/

typedef enum
{
  /* CmdResp */
  SYS_GET_INFO_LIST_MSG_ID = 0,
  SYS_REGISTER_FEATURE_MSG_ID,
  /* NotifAck */
  SYS_RTC_ALARM_MSG_ID,
  SYS_OTHER_MSG_ID,
  /* USER CODE BEGIN SYSTEM_MsgIdTypeDef */

  /* USER CODE END SYSTEM_MsgIdTypeDef */
  SYS_MSGID_LAST
} SYSTEM_MsgIdTypeDef;

typedef enum
{
  /* CmdResp */
  LMHANDLER_INIT_ID = 0,
  LMHANDLER_DEINIT_ID,
  LMHANDLER_CONFIGURE_ID,
  LMHANDLER_JOIN_STATUS_ID,
  LMHANDLER_JOIN_ID,
  LMHANDLER_STOP_ID,
  LMHANDLER_HALT_ID,
  LMHANDLER_IS_BUSY_ID,
  LMHANDLER_SEND_ID,
  LMHANDLER_PROCESS_ID,
  LMHANDLER_GET_DUTY_CYCLE_TIME_ID,
  LMHANDLER_REQUEST_CLASS_ID,
  LMHANDLER_GET_CLASS_ID,
  LMHANDLER_GET_DEVEUI_ID,
  LMHANDLER_SET_DEVEUI_ID,
  LMHANDLER_GET_APPEUI_ID,
  LMHANDLER_SET_APPEUI_ID,
  LMHANDLER_GET_KEY_ID,
  LMHANDLER_SET_KEY_ID,
  LMHANDLER_GET_NWKID_ID,
  LMHANDLER_SET_NWKID_ID,
  LMHANDLER_GET_DEVADDR_ID,
  LMHANDLER_SET_DEVADDR_ID,
  LMHANDLER_GET_ACTIVE_REGION_ID,
  LMHANDLER_SET_ACTIVE_REGION_ID,
  LMHANDLER_GET_ADR_ENABLE_ID,
  LMHANDLER_SET_ADR_ENABLE_ID,
  LMHANDLER_GET_TX_DATARATE_ID,
  LMHANDLER_SET_TX_DATARATE_ID,
  LMHANDLER_GET_DUTY_CYCLE_ID,
  LMHANDLER_SET_DUTY_CYCLE_ID,
  LMHANDLER_GET_RX2_PARAMS_ID,
  LMHANDLER_GET_TX_PWR_ID,
  LMHANDLER_GET_RX1_DELAY_ID,
  LMHANDLER_GET_RX2_DELAY_ID,
  LMHANDLER_GET_JOIN_RX1_DELAY_ID,
  LMHANDLER_GET_JOIN_RX2_DELAY_ID,
  LMHANDLER_SET_TX_PWR_ID,
  LMHANDLER_SET_RX2_PARAMS_ID,
  LMHANDLER_SET_RX1_DELAY_ID,
  LMHANDLER_SET_RX2_DELAY_ID,
  LMHANDLER_SET_JOIN_RX1_DELAY_ID,
  LMHANDLER_SET_JOIN_RX2_DELAY_ID,
  LMHANDLER_GET_PING_PERIODICITY_ID,
  LMHANDLER_SET_PING_PERIODICITY_ID,
  LMHANDLER_GET_BEACON_STATE_ID,
  LMHANDLER_LINK_CHECK_REQ_ID,
  LMHANDLER_DEVICE_TIME_REQ_ID,
  LMHANDLER_GET_VERSION_ID,
  LMHANDLER_NVM_DATA_STORE_ID,

  LMHANDLER_INFO_INIT_ID,
  /* NotifAck */
  LMHANDLER_GET_BATTERY_LEVEL_CB_ID,
  LMHANDLER_GET_TEMPERATURE_CB_ID,
  LMHANDLER_ON_RESTORE_CONTEXT_REQ_CB_ID,
  LMHANDLER_ON_STORE_CONTEXT_REQ_CB_ID,
  LMHANDLER_ON_NVM_DATA_CHANGE_CB_ID,
  LMHANDLER_ON_NETWORK_PARAMS_CHANGE_CB_ID,
  LMHANDLER_ON_JOIN_REQ_CB_ID,
  LMHANDLER_ON_TX_DATA_CB_ID,
  LMHANDLER_ON_RX_DATA_CB_ID,
  LMHANDLER_ON_CLASS_CHANGE_CB_ID,
  LMHANDLER_ON_BEACON_STATUS_CHANGE_CB_ID,
  LMHANDLER_ON_SYS_TIME_UPDATE_CB_ID,
  LMHANDLER_ON_TX_PERIOD_CHANGED_CB_ID,
  LMHANDLER_ON_TX_CTRL_CHANGED_CB_ID,
  LMHANDLER_ON_PING_SLOT_PERIOD_CHANGED_CB_ID,
  LMHANDLER_ON_SYSTEM_RESET_CB_ID,
  /* USER CODE BEGIN LmHandler_MsgIdTypeDef */

  /* USER CODE END LmHandler_MsgIdTypeDef */
  LMHANDLER_MSGID_LAST
} LmHandler_MsgIdTypeDef;

typedef enum
{
  /* CmdResp */
  RADIO_INIT_ID = 0,
  RADIO_GET_STATUS_ID,
  RADIO_SET_MODEM_ID,
  RADIO_SET_CHANNEL_ID,
  RADIO_IS_CHANNEL_FREE_ID,
  RADIO_RANDOM_ID,
  RADIO_SET_RX_CONFIG_ID,
  RADIO_SET_TX_CONFIG_ID,
  RADIO_CHECK_RF_FREQUENCY_ID,
  RADIO_GET_TIME_ON_AIR_ID,
  RADIO_SEND_ID,
  RADIO_SLEEP_ID,
  RADIO_STANDBY_ID,
  RADIO_RX_ID,
  RADIO_START_CAD_ID,
  RADIO_SET_TX_CONTINUOUS_WAVE_ID,
  RADIO_RSSI_ID,
  RADIO_WRITE_ID,
  RADIO_READ_ID,
  RADIO_WRITE_BUFFER_ID,
  RADIO_READ_BUFFER_ID,
  RADIO_SET_MAX_PAYLOAD_LENGTH_ID,
  RADIO_SET_PUBLIC_NETWORK_ID,
  RADIO_GET_WAKEUP_TIME_ID,
  RADIO_IRQ_PROCESS_ID,
  RADIO_RX_BOOSTED_ID,
  RADIO_SET_RX_DUTY_CYCLE_ID,
  RADIO_TX_PRBS_ID,
  RADIO_TX_CW_ID,
  RADIO_SET_RX_GENERIC_CONFIG_ID,
  RADIO_SET_TX_GENERIC_CONFIG_ID,
  /* NotifAck */
  RADIO_TX_DONE_CB_ID,
  RADIO_TX_TIMEOUT_CB_ID,
  RADIO_RX_DONE_CB_ID,
  RADIO_RX_TIMEOUT_CB_ID,
  RADIO_RX_ERROR_CB_ID,
  RADIO_FHSS_CHANGE_CHANNEL_CB_ID,
  RADIO_CAD_DONE_CB_ID,
  /* USER CODE BEGIN Radio_MsgIdTypeDef */

  /* USER CODE END Radio_MsgIdTypeDef */
  RADIO_MSGID_LAST
} Radio_MsgIdTypeDef;

typedef enum
{
  /* CmdResp */
  KMS_INIT_ID = 0,
  KMS_OPEN_SESSION_ID,
  KMS_CLOSE_SESSION_ID,
  KMS_ENCRYPT_ID,
  KMS_DECRYPT_ID,
  KMS_DERIVE_KEY_ID,
  KMS_CRYPTO_HMAC_SHA256_MSG_ID,
  /* NotifAck */
  /* USER CODE BEGIN KMS_MsgIdTypeDef */

  /* USER CODE END KMS_MsgIdTypeDef */
  KMS_MSGID_LAST
} KMS_MsgIdTypeDef;

typedef enum
{
  /* CmdResp */
  /* NotifAck */
  TRACE_SEND_MSG_ID = 0,
  /* USER CODE BEGIN Trace_MsgIdTypeDef */

  /* USER CODE END Trace_MsgIdTypeDef */
  TRACE_MSGID_LAST
} Trace_MsgIdTypeDef;

typedef enum
{
  /* CmdResp */
  /* NotifAck */
  FLASHIF_WRITE_ID = 0,
  FLASHIF_READ_ID,
  FLASHIF_ERASE_ID,
  /* USER CODE BEGIN FlashIf_MsgIdTypeDef */

  /* USER CODE END FlashIf_MsgIdTypeDef */
  FLASHIF_MSGID_LAST
} FlashIf_MsgIdTypeDef;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/* Dual core Init sequence */
#define CPUS_BOOT_SYNC_PREVENT_CPU2_TO_START  0xFFFF
#define CPUS_BOOT_SYNC_ALLOW_CPU2_TO_START    0x5555
#define CPUS_BOOT_SYNC_CPU2_INIT_COMPLETED    0xAAAA
#define CPUS_BOOT_SYNC_RTC_REGISTERED         0x9999

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __MSG_ID_H__ */
