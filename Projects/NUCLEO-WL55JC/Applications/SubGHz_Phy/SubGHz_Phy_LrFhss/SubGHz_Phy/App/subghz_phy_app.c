/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subghz_phy_app.c
  * @author  MCD Application Team
  * @brief   Application of the SubGHz_Phy Middleware
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
#include "platform.h"
#include "sys_app.h"
#include "subghz_phy_app.h"
#include "radio.h"

/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "stm32_seq.h"
#include "utilities_def.h"
#include "app_version.h"
#include "subghz_phy_version.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  lr_fhss_v1_cr_t coding_rate;
  uint8_t header_count[4];
} SubghzPhyTxPayloadLen_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Configurations */
#define TX_TIMEOUT_VALUE              3000
#define PRBS9_INIT                    ( ( uint16_t) 2 )
/* Application buffer, can be increased further*/
#define MAX_APP_BUFFER_SIZE 255
#if (PAYLOAD_LEN>MAX_APP_BUFFER_SIZE)
#error please, decrease PAYLOAD_LEN
#endif /* (PAYLOAD_LEN>MAX_APP_BUFFER_SIZE) */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define STRUCT_FIELD_SIZE(s, f)   sizeof(((s *)0)->f)

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */
static __IO uint32_t RadioTxDone_flag = 0;
static __IO uint32_t RadioTxTimeout_flag = 0;

uint8_t data_buffer[MAX_APP_BUFFER_SIZE] UTIL_MEM_ALIGN(4);
uint16_t data_offset = 0;

static __IO uint16_t payloadLen = PAYLOAD_LEN;
static uint16_t payloadLenMax = PAYLOAD_LEN;

uint32_t count_RxOk = 0;
uint32_t count_RxKo = 0;
uint32_t PER = 0;

static uint32_t packetCnt = 0;

static const uint8_t lr_fhss_sync_word[LR_FHSS_SYNC_WORD_BYTES] = { 0x2C, 0x0F, 0x79, 0x95 };

/* TxPayloadMode
 * 0: prbs9 mode
 * 1: ramp mode e.g payload=0x00, 0x01, ..,payloadLen-1*/
static __IO uint8_t TxPayloadMode = 0;

static const SubghzPhyTxPayloadLen_t txPayloadLen[] =
{
  {LR_FHSS_V1_CR_5_6, {0, 189, 178, 167}},
  {LR_FHSS_V1_CR_2_3, {0, 151, 142, 133}},
  {LR_FHSS_V1_CR_1_2, {0, 112, 105, 99}},
  {LR_FHSS_V1_CR_1_3, {0, 74, 69, 65}}
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
 * @brief Function to be executed on Radio Tx Done event
 */
static void OnTxDone(void);

/**
  * @brief Function to be executed on Radio Rx Done event
  * @param  payload ptr of buffer received
  * @param  size buffer size
  * @param  rssi
  * @param  LoraSnr_FskCfo
  */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);

/**
  * @brief Function executed on Radio Tx Timeout event
  */
static void OnTxTimeout(void);

/**
  * @brief Function executed on Radio Rx Timeout event
  */
static void OnRxTimeout(void);

/**
  * @brief Function executed on Radio Rx Error event
  */
static void OnRxError(void);

/* USER CODE BEGIN PFP */
/**
  * @brief Packet Error Rate state machine implementation
  */
static void LrFhss_Process(void);

/**
  * @brief Generates a PRBS9 sequence
  * @retval 0
  */
static int32_t tx_payload_generator(void);

/**
  * @brief Calculate payloadLenMax depending on Init Param (CR and nr of headers)
  */
static void set_tx_payload_len_max(lr_fhss_v1_cr_t cr, uint8_t hc);

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */

  radio_lr_fhss_cfg_params_t radio_lr_fhss_cfg_params;
  radio_lr_fhss_time_on_air_params_t time_on_air;
  /* Get SubGHY_Phy APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "APPLICATION_VERSION: V%X.%X.%X\r\n",
          (uint8_t)(APP_VERSION_MAIN),
          (uint8_t)(APP_VERSION_SUB1),
          (uint8_t)(APP_VERSION_SUB2));

  /* Get MW SubGhz_Phy info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:    V%X.%X.%X\r\n",
          (uint8_t)(SUBGHZ_PHY_VERSION_MAIN),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB1),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB2));

  APP_LOG(TS_OFF, VLEVEL_M, "---------------\n\r");
  APP_LOG(TS_OFF, VLEVEL_M, "LRFHSS_MODULATION\n\r");
  /* USER CODE END SubghzApp_Init_1 */

  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

  /* USER CODE BEGIN SubghzApp_Init_2 */
  /* Build payload */
  tx_payload_generator();

  /* Configure Tx parameters */
  radio_lr_fhss_cfg_params.tx_rf_pwr_in_dbm                              = 14;

  radio_lr_fhss_cfg_params.radio_lr_fhss_params.center_frequency_in_hz         = RF_FREQUENCY;
  radio_lr_fhss_cfg_params.radio_lr_fhss_params.device_offset                  = 0;
  radio_lr_fhss_cfg_params.radio_lr_fhss_params.lr_fhss_params.sync_word       = lr_fhss_sync_word;
  radio_lr_fhss_cfg_params.radio_lr_fhss_params.lr_fhss_params.modulation_type = LR_FHSS_V1_MODULATION_TYPE_GMSK_488;
  radio_lr_fhss_cfg_params.radio_lr_fhss_params.lr_fhss_params.cr              = LR_FHSS_V1_CR_5_6;
  radio_lr_fhss_cfg_params.radio_lr_fhss_params.lr_fhss_params.grid            = LR_FHSS_V1_GRID_3906_HZ;
  radio_lr_fhss_cfg_params.radio_lr_fhss_params.lr_fhss_params.bw              = LR_FHSS_V1_BW_136719_HZ;
  radio_lr_fhss_cfg_params.radio_lr_fhss_params.lr_fhss_params.enable_hopping  = true;
  radio_lr_fhss_cfg_params.radio_lr_fhss_params.lr_fhss_params.header_count    = 2;
  radio_lr_fhss_cfg_params.tx_timeout_in_ms = 0 ;

  /* Set max payload*/
  set_tx_payload_len_max(radio_lr_fhss_cfg_params.radio_lr_fhss_params.lr_fhss_params.cr,
                         radio_lr_fhss_cfg_params.radio_lr_fhss_params.lr_fhss_params.header_count);
  /* Get time on air*/
  time_on_air.radio_lr_fhss_params = radio_lr_fhss_cfg_params.radio_lr_fhss_params;
  time_on_air.pld_len_in_bytes = payloadLen;
  Radio.LrFhssGetTimeOnAirInMs(&time_on_air, &radio_lr_fhss_cfg_params.tx_timeout_in_ms);
  /* Apply 50 ms margin*/
  radio_lr_fhss_cfg_params.tx_timeout_in_ms += 50;

  if (0UL != Radio.LrFhssSetCfg(&radio_lr_fhss_cfg_params))
  {
    while (1);
  }
  /* Send Tx Data*/
  if (Radio.Send(data_buffer, payloadLen) != RADIO_STATUS_OK)
  {
    APP_TPRINTF("Error when sending the first packet \r\n");
  }

  /*register task to to be run in while(1) after Radio IT*/
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), UTIL_SEQ_RFU, LrFhss_Process);
  /* USER CODE END SubghzApp_Init_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
  RadioTxDone_flag = 1;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxDone */
}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone */
  /* USER CODE END OnRxDone */
}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
  RadioTxTimeout_flag = 1;
  /* Run process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxTimeout */
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout */
  /* USER CODE END OnRxTimeout */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */
  /* USER CODE END OnRxError */
}

/* USER CODE BEGIN PrFD */
static void LrFhss_Process(void)
{
  packetCnt++;

  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); /* LED_BLUE */
  if (RadioTxDone_flag == 1)
  {
    APP_TPRINTF("OnTxDone\r\n");
  }

  if (RadioTxTimeout_flag == 1)
  {
    APP_TPRINTF("OnTxTimeout\r\n");
  }
  /* Clear TxDone or TxTimeout flags */
  RadioTxDone_flag = 0;
  RadioTxTimeout_flag = 0;

  /* This delay is only to give enough time to allow DMA to empty printf queue*/
  HAL_Delay(500);
  /* Build payload */
  tx_payload_generator();

  /* Send Tx Data*/
  if (Radio.Send(data_buffer, payloadLen) == RADIO_STATUS_OK)
  {
    APP_TPRINTF("Tx %d \r\n", packetCnt);
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); /* LED_BLUE */
  }
  else
  {
    APP_TPRINTF("Tx error with packet %d \r\n", packetCnt);
    APP_TPRINTF("LrFhss_Process will not be reschedule, please restart to debug \r\n");
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case  BUT1_Pin:
      /* Increment by 16*/
      payloadLen += 16;
      if (payloadLen > payloadLenMax)
      {
        payloadLen = 16;
      }
      APP_TPRINTF("New Tx Payload Length= %d\r\n", payloadLen);
      break;
    case  BUT2_Pin:
      /* Increment by 1*/
      payloadLen += 1;
      if (payloadLen > payloadLenMax)
      {
        payloadLen = 1;
      }
      APP_TPRINTF("New Tx Payload Length= %d\r\n", payloadLen);

      break;
    case  BUT3_Pin:
      /* Toggle TxPayloadMode*/
      TxPayloadMode = (TxPayloadMode + 1) % 2;
      if (TxPayloadMode == 1)
      {
        APP_TPRINTF("Payload PRBS9 mode\r\n");
      }
      else
      {
        APP_TPRINTF("Payload Inc mode\r\n");
      }
      break;
    default:
      break;
  }
}

static int32_t tx_payload_generator(void)
{
  if (TxPayloadMode == 1)
  {
    uint16_t prbs9_val = PRBS9_INIT;
    for (int32_t i = 0; i < payloadLen; i++)
    {
      data_buffer[i] = 0;
    }
    for (int32_t i = 0; i < payloadLen * 8; i++)
    {
      /*fill buffer with prbs9 sequence*/
      int32_t newbit = (((prbs9_val >> 8) ^ (prbs9_val >> 4)) & 1);
      prbs9_val = ((prbs9_val << 1) | newbit) & 0x01ff;
      data_buffer[i / 8] |= ((prbs9_val & 0x1) << (i % 8));
    }
  }
  else
  {
    for (int32_t i = 0; i < payloadLen; i++)
    {
      data_buffer[i] = i;
    }
  }
  return 0;
}

static void set_tx_payload_len_max(lr_fhss_v1_cr_t cr, uint8_t hc)
{
  /* The limit is due to a 255-byte maximum physical payload size */
  /*  (after encoding and packet construction)                    */
  if ((hc != 0) && (hc < STRUCT_FIELD_SIZE(SubghzPhyTxPayloadLen_t, header_count)))
  {
    for (uint32_t index = 0; index < (sizeof(txPayloadLen) / sizeof(SubghzPhyTxPayloadLen_t)); index++)
    {
      if (cr == txPayloadLen[index].coding_rate)
      {
        payloadLenMax = txPayloadLen[index].header_count[hc];
        break;
      }
    }
  }
}
/* USER CODE END PrFD */
