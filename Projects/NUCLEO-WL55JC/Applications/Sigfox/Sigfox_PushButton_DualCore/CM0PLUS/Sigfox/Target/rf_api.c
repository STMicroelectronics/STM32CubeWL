/**
  ****************************************************************************
  * @file    rf_api.c
  * @author  MCD Application Team
  * @brief   rf library interface
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

/* includes -----------------------------------------------------------*/
#include "stdint.h"
#include "sigfox_types.h"
#include "sigfox_api.h"
#include "rf_api.h"
#include "radio.h"
#include "stm32_seq.h"
#include "stm32_mem.h"
#include "utilities_def.h"
#include "sgfx_eeprom_if.h"
#include "sys_debug.h"
#include "sgfx_cstimer.h"
#include "stm32_timer.h"
#include "sys_app.h" /*for APP_LOG*/
#include "platform.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define RF_VER                   "RF_API_V1.0"
#define SFX_PREAMBLE_LENGTH      2
#define MONARCH_PREAMBLE_LENGTH (65535/8) /*max length to never synchronise*/
#define SFX_MAX_PAYLOAD_LENGTH   15
#define SFX_DATARATE_600         600
#define SFX_DATARATE_100         100

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static sfx_u8 rf_api_version[] = RF_VER;

static uint8_t *RxFrame;

static int16_t RxRssi;

static sfx_u8 RxPacketReceived = 0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
  * \brief Waits End of Rx, either by a successful Reception or by a timeout
  * \Return 0 when timeOut or 1 when successful Reception
  */
static sfx_u8 sfx_wait_end_of_rx(void);

/*!
  * \brief Function to be executed on Radio Tx Done event
  */
static void OnTxDone(void);

/*!
  * \brief Function to be executed on Radio Rx Done event
  */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

/*!
  * \brief Function executed on Radio Tx Timeout event
  */
static void OnTxTimeout(void);

/*!
  * \brief Function executed on Radio Rx Timeout event
  */
static void OnRxTimeout(void);

/*!
  * \brief Function executed on Radio Rx Error event
  */
static void OnRxError(void);

/*use for CS*/
static bool RF_API_isChannelFree(int16_t rssiThresh, uint32_t maxCarrierSenseTime);

const RadioEvents_t RfApiRadioEvents =
{
  OnTxDone,
  OnTxTimeout,
  OnRxDone,
  OnRxTimeout,
  OnRxError,
};

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
sfx_u8 RF_API_init(sfx_rf_mode_t rf_mode)
{
  /* USER CODE BEGIN RF_API_init_1 */

  /* USER CODE END RF_API_init_1 */
  /* ------------------------------------------------------ */
  /* PSEUDO code */
  /* ------------------------------------------------------ */
  /* Put here all RF initialization concerning the RC IC you are using. */
  /* this function is dependent of SPI driver or equivalent. */

  switch (rf_mode)
  {
    case SFX_RF_MODE_TX:
    {
      APP_LOG(TS_ON, VLEVEL_M, "RF_API_init in TX\n\r");
      /* nothing to do */
      /* set the RF IC in TX mode : this could be DBPSK100 or 600:  this distinction will be done during the RF_API_send */
      /* you can add some code to switch on TCXO or warm up quartz for stabilization : in case of frequency drift issue */
      break;
    }
    case SFX_RF_MODE_RX:
    {
      APP_LOG(TS_ON, VLEVEL_M, "RF_API_init in RX\n\r");
      Radio.SetRxConfig(MODEM_SIGFOX_RX, 1600,
                        SFX_DATARATE_600, NA,
                        NA, SFX_PREAMBLE_LENGTH,
                        1000, true,
                        SFX_MAX_PAYLOAD_LENGTH,
                        false, NA, NA,
                        NA, true);
      /* set the RF IC in RX mode : GFSK 600bps is the only mode now */
      /* Enable interrupts on RX fifo */
      /* RF IC must configure the SYNCHRO BIT = 0xAAAAAAAAAA and synchro frame = 0xB227 */
      break;
    }
    case SFX_RF_MODE_CS200K_RX:
    {
      APP_LOG(TS_ON, VLEVEL_M, "RF_API_init in CS200\n\r");

      Radio.SetRxConfig(MODEM_SIGFOX_RX, 200000,
                        SFX_DATARATE_600, NA,
                        NA, SFX_PREAMBLE_LENGTH,
                        1000, true,
                        SFX_MAX_PAYLOAD_LENGTH,
                        false, NA, NA,
                        NA, true);
      /* configure the RF IC into sensing 200KHz bandwidth to be able to read out RSSI level */
      /* RSSI level will outputted during the RF_API_wait_for_clear_channel function */
      break;
    }
    case SFX_RF_MODE_CS300K_RX:
    {
      APP_LOG(TS_ON, VLEVEL_M, "RF_API_init in CS300\n\r");

      Radio.SetRxConfig(MODEM_SIGFOX_RX, 300000,
                        SFX_DATARATE_600, NA,
                        NA, SFX_PREAMBLE_LENGTH,
                        1000, true,
                        SFX_MAX_PAYLOAD_LENGTH,
                        false, NA, NA,
                        NA, true);
      /* configure the RF IC into sensing 300KHz bandwidth to be able to read out RSSI level */
      /* This is poosible to make this carrier sense in 2 * 200Kz if you respect the regulation time for listening */
      /* RSSI level will outputted during the RF_API_wait_for_clear_channel function. */
      break;
    }
    case SFX_RF_MODE_MONARCH:
    {
      APP_LOG(TS_ON, VLEVEL_M, "RF_API_init in MN20\n\r");
      /* APP_LOG(TS_ON, VLEVEL_L,"RxBw=20kHz\n\r"); */
      Radio.SetRxConfig(MODEM_SIGFOX_RX, 20000,
                        SFX_DATARATE_100, NA,
                        NA, MONARCH_PREAMBLE_LENGTH,
                        1000, true,
                        SFX_MAX_PAYLOAD_LENGTH,
                        false, NA, NA,
                        NA, true);
      /* configure the RF IC into sensing 20KHz bandwidth to be able to read out RSSI level */
      /* This is poosible to make this carrier sense in 2 * 200Kz if you respect the regulation time for listening */
      /* RSSI level will outputted during the RF_API_wait_for_clear_channel function. */
      break;
    }
    default :
      break;
  }
  return SFX_ERR_NONE;
  /* USER CODE BEGIN RF_API_init_2 */

  /* USER CODE END RF_API_init_2 */
}

sfx_u8 RF_API_stop(void)
{
  /* USER CODE BEGIN RF_API_stop_1 */

  /* USER CODE END RF_API_stop_1 */
  APP_LOG(TS_ON, VLEVEL_M, "RF_API_stop\n\r");

  Radio.Sleep();
  /* USER CODE BEGIN RF_API_stop_2 */

  /* USER CODE END RF_API_stop_2 */

  return SFX_ERR_NONE;
}

sfx_u8 RF_API_send(sfx_u8 *stream, sfx_modulation_type_t type, sfx_u8 size)
{
  /* USER CODE BEGIN RF_API_send_1 */

  /* USER CODE END RF_API_send_1 */
  sfx_u8 status = SFX_ERR_NONE;

  uint32_t datarate;

  int8_t power = E2P_Read_Power(E2P_Read_Rc());

  APP_LOG(TS_ON, VLEVEL_M, "TX START:nB=%d\n\r", size);

  switch (type)
  {
    case SFX_DBPSK_100BPS :
      datarate = SFX_DATARATE_100;
      break;
    case SFX_DBPSK_600BPS :
      datarate = SFX_DATARATE_600;
      break;
    default :
      status = RF_ERR_API_SEND;
      break;
  }

  if (status == SFX_ERR_NONE)
  {
    Radio.SetTxConfig(MODEM_SIGFOX_TX, power, NA,
                      NA, datarate,
                      NA, NA,
                      NA, NA, NA,
                      NA, NA, 3000);

#if defined(USE_BSP_DRIVER)
    BSP_LED_On(LED_BLUE);
#elif defined(MX_BOARD_PSEUDODRIVER)
    SYS_LED_On(SYS_LED_BLUE);
#endif /* defined(USE_BSP_DRIVER) */

    Radio.Send(stream, size);

    APP_LOG(TS_ON, VLEVEL_M, "Wait For End of Tx\n\r");

    UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_TxTimeout);

    APP_LOG(TS_ON, VLEVEL_M, "End Of Tx\n\r");

#if defined(USE_BSP_DRIVER)
    BSP_LED_Off(LED_BLUE);
#elif defined(MX_BOARD_PSEUDODRIVER)
    SYS_LED_Off(SYS_LED_BLUE);
#endif /* defined(USE_BSP_DRIVER) */

  }

  APP_LOG(TS_ON, VLEVEL_M, "TX END\n\r");

  /* BSP_LED_Off( LED_RED2 ); */
  return status;
  /* USER CODE BEGIN RF_API_send_2 */

  /* USER CODE END RF_API_send_2 */
}

sfx_u8 RF_API_start_continuous_transmission(sfx_modulation_type_t type)
{
  /* USER CODE BEGIN RF_API_start_continuous_transmission_1 */

  /* USER CODE END RF_API_start_continuous_transmission_1 */
  sfx_u8 status = SFX_ERR_NONE;

  int8_t power = E2P_Read_Power(E2P_Read_Rc());

  /* ------------------------------------------------------ */
  /* PSEUDO code */
  /* ------------------------------------------------------ */

  switch (type)
  {
    case SFX_NO_MODULATION :
      Radio.TxCw(power);
      break;
    case SFX_DBPSK_100BPS :
      /* Make an infinite DBPSK 100bps modulation on the RF IC at the frequency given by the RF_API_change_frequency() */
      Radio.SetTxConfig(MODEM_SIGFOX_TX, power, NA,
                        NA, SFX_DATARATE_100,
                        NA, NA,
                        NA, NA, NA,
                        NA, NA, 3000);
      Radio.TxPrbs();
      break;
    case SFX_DBPSK_600BPS :
      Radio.SetTxConfig(MODEM_SIGFOX_TX, power, NA,
                        NA, SFX_DATARATE_600,
                        NA, NA,
                        NA, NA, NA,
                        NA, NA, 3000);
      Radio.TxPrbs();
      break;
    default :
      status = RF_ERR_API_SEND;
      break;
  }
  return status;
  /* USER CODE BEGIN RF_API_start_continuous_transmission_2 */

  /* USER CODE END RF_API_start_continuous_transmission_2 */
}

sfx_u8 RF_API_stop_continuous_transmission(void)
{
  /* USER CODE BEGIN RF_API_stop_continuous_transmission_1 */

  /* USER CODE END RF_API_stop_continuous_transmission_1 */
  APP_LOG(TS_ON, VLEVEL_M, "RF_API_stop\n\r");

  Radio.Sleep();
  /* USER CODE BEGIN RF_API_stop_continuous_transmission_2 */

  /* USER CODE END RF_API_stop_continuous_transmission_2 */
  return SFX_ERR_NONE;
}

sfx_u8 RF_API_change_frequency(sfx_u32 frequency)
{
  /* USER CODE BEGIN RF_API_change_frequency_1 */

  /* USER CODE END RF_API_change_frequency_1 */
  APP_LOG(TS_ON, VLEVEL_M, "RF at Freq %d\n\r", frequency);

  Radio.SetChannel(frequency);
  /* USER CODE BEGIN RF_API_change_frequency_2 */

  /* USER CODE END RF_API_change_frequency_2 */

  return SFX_ERR_NONE;
}

sfx_u8 RF_API_wait_frame(sfx_u8 *frame, sfx_s16 *rssi, sfx_rx_state_enum_t *state)
{
  /* USER CODE BEGIN RF_API_wait_frame_1 */

  /* USER CODE END RF_API_wait_frame_1 */
  sfx_error_t status;

  APP_LOG(TS_ON, VLEVEL_M, "RX START\n\r");

  Radio.RxBoosted(0);

  /*save address for callback*/
  RxFrame = frame;

  if (sfx_wait_end_of_rx() == 1)
  {
#if 0
    APP_LOG(TS_ON, VLEVEL_M, "RX= %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n\r",
            RxFrame[0], RxFrame[1], RxFrame[2], RxFrame[3],
            RxFrame[4], RxFrame[5], RxFrame[6], RxFrame[7],
            RxFrame[8], RxFrame[9], RxFrame[10], RxFrame[11],
            RxFrame[12], RxFrame[13], RxFrame[14], RxFrame[15]);
#endif /* 0 */
    status = SFX_ERR_NONE;
    *state = DL_PASSED;
  }
  else
  {
    *state = DL_TIMEOUT;
    status = SFX_ERR_NONE;
  }

  *rssi = (sfx_s16) RxRssi;

  APP_LOG(TS_ON, VLEVEL_M, "RX END\n\r");

  return status;
  /* USER CODE BEGIN RF_API_wait_frame_2 */

  /* USER CODE END RF_API_wait_frame_2 */
}

sfx_u8 RF_API_wait_for_clear_channel(sfx_u8 cs_min, sfx_s8 cs_threshold, sfx_rx_state_enum_t *state)
{
  /* USER CODE BEGIN RF_API_wait_for_clear_channel_1 */

  /* USER CODE END RF_API_wait_for_clear_channel_1 */
  sfx_rx_state_enum_t cs_state = DL_TIMEOUT;
  /**/
  RxCarrierSenseInitStatus();

  Radio.Rx(0);

  HAL_Delay(Radio.GetWakeupTime());

  APP_LOG(TS_ON, VLEVEL_M, "CS start cs_min=%dms, cs_threshold=%dBm\n\r", cs_min, cs_threshold);

  while (RxCarrierSenseGetStatus() == 0)
  {
    if (RF_API_isChannelFree((int16_t) cs_threshold, (uint32_t) cs_min) == true)
    {
      cs_state = DL_PASSED;
      break;
    }
  }
  Radio.Standby();

  if (cs_state == DL_PASSED)
  {
    APP_LOG(TS_ON, VLEVEL_M, "LBT Channel Free\n\r");
  }
  else
  {
    APP_LOG(TS_ON, VLEVEL_M, "LBT Channel Busy\n\r");
  }

  *state = cs_state;
  /* USER CODE BEGIN RF_API_wait_for_clear_channel_2 */

  /* USER CODE END RF_API_wait_for_clear_channel_2 */

  return SFX_ERR_NONE;
}

sfx_u8 RF_API_get_version(sfx_u8 **version, sfx_u8 *size)
{
  /* USER CODE BEGIN RF_API_get_version_1 */

  /* USER CODE END RF_API_get_version_1 */
  *version = (sfx_u8 *)rf_api_version;
  if (size == SFX_NULL)
  {
    return RF_ERR_API_GET_VERSION;
  }
  *size = sizeof(rf_api_version);
  /* USER CODE BEGIN RF_API_get_version_2 */

  /* USER CODE END RF_API_get_version_2 */

  return SFX_ERR_NONE;
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

static sfx_u8 sfx_wait_end_of_rx(void)
{
  /* USER CODE BEGIN sfx_wait_end_of_rx_1 */

  /* USER CODE END sfx_wait_end_of_rx_1 */
  RxPacketReceived = 0;

  RxRssi = -150;

  APP_LOG(TS_ON, VLEVEL_M, "Wait For End of Rx\n\r");

  UTIL_SEQ_ClrEvt(1 << CFG_SEQ_Evt_Timeout);

  UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_Timeout);

  APP_LOG(TS_ON, VLEVEL_M, "End Of Rx\n\r");

  return RxPacketReceived;
  /* USER CODE BEGIN sfx_wait_end_of_rx_2 */

  /* USER CODE END sfx_wait_end_of_rx_2 */
}

static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone_1 */

  /* USER CODE END OnTxDone_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_TxTimeout);

  APP_LOG(TS_ON, VLEVEL_M, "OnTxDone\n\r");

  /* USER CODE BEGIN OnTxDone_2 */

  /* USER CODE END OnTxDone_2 */
}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t offset)
{
  /* USER CODE BEGIN OnRxDone_1 */

  /* USER CODE END OnRxDone_1 */
  UTIL_MEM_cpy_8(RxFrame, payload, size);

  RxRssi = rssi;

  RxPacketReceived = 1;
  /*wakes up the MCU at line UTIL_SEQ_WaitEvt( 1 << CFG_SEQ_Evt_Timeout );*/
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_Timeout);

  APP_LOG(TS_ON, VLEVEL_M, "OnRxDone\n\r");
  /* USER CODE BEGIN OnRxDone_2 */

  /* USER CODE END OnRxDone_2 */
}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout_1 */

  /* USER CODE END OnTxTimeout_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_TxTimeout);

  APP_LOG(TS_ON, VLEVEL_M,  "OnTxTimeout\n\r");
  /* USER CODE BEGIN OnTxTimeout_2 */

  /* USER CODE END OnTxTimeout_2 */
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout_1 */

  /* USER CODE END OnRxTimeout_1 */
  RxPacketReceived = 0;

  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_Timeout);

  APP_LOG(TS_ON, VLEVEL_M,  "OnRxTimeout\n\r");
  /* USER CODE BEGIN OnRxTimeout_2 */

  /* USER CODE END OnRxTimeout_2 */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError_1 */

  /* USER CODE END OnRxError_1 */
  RxPacketReceived = 0;

  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_Timeout);

  APP_LOG(TS_ON, VLEVEL_M, "OnRxError\n\r");
  /* USER CODE BEGIN OnRxError_2 */

  /* USER CODE END OnRxError_2 */
}

bool RF_API_isChannelFree(int16_t rssiThresh, uint32_t maxCarrierSenseTime)
{
  /* USER CODE BEGIN RF_API_isChannelFree_1 */

  /* USER CODE END RF_API_isChannelFree_1 */
  bool status = true;
  int16_t rssi = 0;
  uint32_t carrierSenseTime = 0;

  carrierSenseTime = UTIL_TIMER_GetCurrentTime();

  /* Perform carrier sense for maxCarrierSenseTime */
  while (UTIL_TIMER_GetElapsedTime(carrierSenseTime) < maxCarrierSenseTime)
  {
    rssi = Radio.Rssi(MODEM_FSK);

    if (rssi > rssiThresh)
    {
      status = false;
      break;
    }
  }
  return status;
  /* USER CODE BEGIN RF_API_isChannelFree_2 */

  /* USER CODE END RF_API_isChannelFree_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
