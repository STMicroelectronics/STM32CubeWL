/*!
 * \file      subghz_phy_app.c
 *
 * \brief     Ping-Pong implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    subghz_phy_app.c
  * @author  MCD Application Team
  * @brief   Application of the SubGHz_Phy Middleware
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "stm32_timer.h"
#include "sys_app.h"
#include "subghz_phy_app.h"
#include "radio.h"
#include "stm32_seq.h"
#include "utilities_def.h"
#include "app_version.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  LOWPOWER,
  RX,
  RX_TIMEOUT,
  RX_ERROR,
  TX,
  TX_TIMEOUT,
} States_t;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
const uint8_t PingMsg[] = "PING";
const uint8_t PongMsg[] = "PONG";

uint16_t BufferSize = BUFFER_SIZE;
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1")  uint8_t Buffer[BUFFER_SIZE];

States_t State = LOWPOWER;

int8_t RssiValue = 0;
int8_t SnrValue = 0;

/* Led Timers objects*/
static  UTIL_TIMER_Object_t timerLed;

bool isMaster = true;

/* Radio events function pointer */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
 * @brief  Function executed on when led timer elapses
 * @param  LED context
 * @retval none
 */
static void OnledEvent(void *context);

/*!
 * @brief Function to be executed on Radio Tx Done event
 * @param  none
 * @retval none
 */
static void OnTxDone(void);

/*!
 * @brief Function to be executed on Radio Rx Done event
 * @param  payload sent
 * @param  payload size
 * @param  rssi
 * @param  snr
 * @retval none
 */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);

/*!
 * @brief Function executed on Radio Tx Timeout event
 * @param  none
 * @retval none
 */
static void OnTxTimeout(void);

/*!
 * @brief Function executed on Radio Rx Timeout event
 * @param  none
 * @retval none
 */
static void OnRxTimeout(void);

/*!
 * @brief Function executed on Radio Rx Error event
 * @param  none
 * @retval none
 */
static void OnRxError(void);

/*!
 * @brief PingPong state machine implementation
 * @param  none
 * @retval none
 */
static void PingPong_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */

  /* USER CODE END SubghzApp_Init_1 */
  /* Print APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "APP_VERSION= V%X.%X.%X\r\n", (uint8_t)(__CM4_APP_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__CM4_APP_VERSION >> __APP_VERSION_SUB1_SHIFT), (uint8_t)(__CM4_APP_VERSION >> __APP_VERSION_SUB2_SHIFT));

  /* Led Timers*/
  UTIL_TIMER_Create(&timerLed, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnledEvent, NULL);
  UTIL_TIMER_SetPeriod(&timerLed, LED_PERIOD_MS);

  UTIL_TIMER_Start(&timerLed);

  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

#if (( USE_MODEM_LORA == 1 ) && ( USE_MODEM_FSK == 0 ))
  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

  Radio.SetMaxPayloadLength(MODEM_LORA, BUFFER_SIZE);

#elif (( USE_MODEM_LORA == 0 ) && ( USE_MODEM_FSK == 1 ))

  Radio.SetTxConfig(MODEM_FSK, TX_OUTPUT_POWER, FSK_FDEV, 0,
                    FSK_DATARATE, 0,
                    FSK_PREAMBLE_LENGTH, FSK_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, 0, TX_TIMEOUT_VALUE);

  Radio.SetRxConfig(MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE,
                    0, FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH,
                    0, FSK_FIX_LENGTH_PAYLOAD_ON, 0, true,
                    0, 0, false, true);

  Radio.SetMaxPayloadLength(MODEM_FSK, BUFFER_SIZE);

#else
#error "Please define a frequency band in the sys_conf.h file."
#endif /* USE_MODEM_LORA | USE_MODEM_FSK */

  Radio.SetChannel(RF_FREQUENCY);

#if defined(USE_BSP_DRIVER)
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);
#elif defined(MX_BOARD_PSEUDODRIVER)
  SYS_LED_Init(SYS_LED_GREEN);
  SYS_LED_Init(SYS_LED_RED);
#else
#error user to provide its board code or to call his board driver functions
#endif  /* USE_BSP_DRIVER || MX_NUCLEO_WL55JC*/

  Radio.Rx(RX_TIMEOUT_VALUE);

  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_PingPong_Process), UTIL_SEQ_RFU, PingPong_Process);

  /* USER CODE BEGIN SubghzApp_Init_2 */

  /* USER CODE END SubghzApp_Init_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void PingPong_Process(void)
{
  /* USER CODE BEGIN PingPong_Process_1 */

  /* USER CODE END PingPong_Process_1 */
  uint8_t i;

  switch (State)
  {
    case RX:

      if (isMaster == true)
      {
        if (BufferSize > 0)
        {
          if (strncmp((const char *)Buffer, (const char *)PongMsg, 4) == 0)
          {
            UTIL_TIMER_Stop(&timerLed);

#if defined(USE_BSP_DRIVER)
            BSP_LED_Off(LED_GREEN) ;
            /* Indicates on a LED that the received frame is a PONG */
            BSP_LED_Toggle(LED_RED) ;
#elif defined(MX_BOARD_PSEUDODRIVER)
            SYS_LED_Off(SYS_LED_GREEN) ;
            /* Indicates on a LED that the received frame is a PONG */
            SYS_LED_Toggle(SYS_LED_RED) ;
#endif /* USE_BSP_DRIVER || MX_BOARD_PSEUDODRIVER */
            /* Send the next PING frame */
            Buffer[0] = 'P';
            Buffer[1] = 'I';
            Buffer[2] = 'N';
            Buffer[3] = 'G';
            /* We fill the buffer with numbers for the payload */
            for (i = 4; i < BufferSize; i++)
            {
              Buffer[i] = i - 4;
            }
            APP_LOG(TS_ON, VLEVEL_L, "...PING\n\r");

            APP_LOG(TS_ON, VLEVEL_L, "Master Tx start\n\r");

            Radio.SetChannel(RF_FREQUENCY);

            /* Add delay between TX and RX =
            time Busy_signal is ON in RX opening window */
            HAL_Delay(Radio.GetWakeupTime() + TCXO_WORKAROUND_TIME_MARGIN);

            Radio.Send(Buffer, BufferSize);
          }
          else if (strncmp((const char *)Buffer, (const char *)PingMsg, 4) == 0)
          {
            /* A master already exists then become a slave */
            isMaster = false;
            APP_LOG(TS_ON, VLEVEL_L, "Slave Rx start\n\r");

            Radio.SetChannel(RF_FREQUENCY);
            Radio.Rx(RX_TIMEOUT_VALUE);
          }
          else /* valid reception but neither a PING or a PONG message */
          {
            /* Set device as master and start again */
            isMaster = true;
            APP_LOG(TS_ON, VLEVEL_L, "Master Rx start\n\r");

            Radio.SetChannel(RF_FREQUENCY);
            Radio.Rx(RX_TIMEOUT_VALUE);
          }
        }
      }
      else
      {
        if (BufferSize > 0)
        {
          if (strncmp((const char *)Buffer, (const char *)PingMsg, 4) == 0)
          {
            /* Indicates on a LED that the received frame is a PING */
            UTIL_TIMER_Stop(&timerLed);

#if defined(USE_BSP_DRIVER)
            BSP_LED_Off(LED_RED);
            BSP_LED_Toggle(LED_GREEN);
#elif defined(MX_BOARD_PSEUDODRIVER)
            SYS_LED_Off(SYS_LED_RED);
            SYS_LED_Toggle(SYS_LED_GREEN);
#endif /* USE_BSP_DRIVER || MX_BOARD_PSEUDODRIVER */
            /* Send the reply to the PONG string */
            Buffer[0] = 'P';
            Buffer[1] = 'O';
            Buffer[2] = 'N';
            Buffer[3] = 'G';
            /* We fill the buffer with numbers for the payload */
            for (i = 4; i < BufferSize; i++)
            {
              Buffer[i] = i - 4;
            }
            APP_LOG(TS_ON, VLEVEL_L, "Slave  Tx start\n\r");
            Radio.SetChannel(RF_FREQUENCY);

            /* Add delay between TX and RX =
            time Busy_signal is ON in RX opening window */
            HAL_Delay(Radio.GetWakeupTime() + TCXO_WORKAROUND_TIME_MARGIN);

            Radio.Send(Buffer, BufferSize);

            APP_LOG(TS_ON, VLEVEL_L, "...PONG\n\r");
          }
          else /* valid reception but not a PING as expected */
          {
            /* Set device as master and start again */
            isMaster = true;
            APP_LOG(TS_ON, VLEVEL_L, "Master Rx start\n\r");
            Radio.SetChannel(RF_FREQUENCY);
            Radio.Rx(RX_TIMEOUT_VALUE);
          }
        }
      }
      State = LOWPOWER;
      break;
    case TX:
      /* Indicates on a LED that we have sent a PING [Master] */
      /* Indicates on a LED that we have sent a PONG [Slave] */
      Radio.SetChannel(RF_FREQUENCY);
      APP_LOG(TS_ON, VLEVEL_L, "Rx start\n\r");
      Radio.Rx(RX_TIMEOUT_VALUE);
      State = LOWPOWER;
      break;
    case RX_TIMEOUT:
    case RX_ERROR:
      if (isMaster == true)
      {
        /* Send the next PING frame */
        Buffer[0] = 'P';
        Buffer[1] = 'I';
        Buffer[2] = 'N';
        Buffer[3] = 'G';
        for (i = 4; i < BufferSize; i++)
        {
          Buffer[i] = i - 4;
        }
        APP_LOG(TS_ON, VLEVEL_L, "Master Tx start\n\r");

        Radio.SetChannel(RF_FREQUENCY);
        /* Add delay between TX and RX =
        time Busy_signal is ON in RX opening window */
        HAL_Delay(Radio.GetWakeupTime() + TCXO_WORKAROUND_TIME_MARGIN);

        Radio.Send(Buffer, BufferSize);
      }
      else
      {
        APP_LOG(TS_ON, VLEVEL_L, "Slave Rx start\n\r");

        Radio.SetChannel(RF_FREQUENCY);
        Radio.Rx(RX_TIMEOUT_VALUE);
      }
      State = LOWPOWER;
      break;
    case TX_TIMEOUT:
      Radio.SetChannel(RF_FREQUENCY);
      APP_LOG(TS_ON, VLEVEL_L, "Slave Rx start\n\r");
      Radio.Rx(RX_TIMEOUT_VALUE);
      State = LOWPOWER;
      break;
    case LOWPOWER:
    default:
      /* Set low power */
      break;
  }
  /* USER CODE BEGIN PingPong_Process_2 */

  /* USER CODE END PingPong_Process_2 */
}

static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone_1 */

  /* USER CODE END OnTxDone_1 */
  APP_LOG(TS_ON, VLEVEL_L, "OnTxDone\n\r");

  Radio.Sleep();
  State = TX;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_PingPong_Process), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN OnTxDone_2 */

  /* USER CODE END OnTxDone_2 */
}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
  /* USER CODE BEGIN OnRxDone_1 */

  /* USER CODE END OnRxDone_1 */
  APP_LOG(TS_ON, VLEVEL_L, "OnRxDone\n\r");
  APP_LOG(TS_ON, VLEVEL_L,  "RssiValue=%d dBm, SnrValue=%d\n\r", rssi, snr);

  Radio.Sleep();
  BufferSize = size;
  memcpy(Buffer, payload, BufferSize);
  RssiValue = rssi;
  SnrValue = snr;
  State = RX;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_PingPong_Process), CFG_SEQ_Prio_0);

  /* USER CODE BEGIN OnRxDone_2 */

  /* USER CODE END OnRxDone_2 */
}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout_1 */

  /* USER CODE END OnTxTimeout_1 */
  APP_LOG(TS_ON, VLEVEL_L,  "OnTxTimeout\n\r");

  Radio.Sleep();
  State = TX_TIMEOUT;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_PingPong_Process), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN OnTxTimeout_2 */

  /* USER CODE END OnTxTimeout_2 */
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout_1 */

  /* USER CODE END OnRxTimeout_1 */
  APP_LOG(TS_ON, VLEVEL_L,  "OnRxTimeout\n\r");

  Radio.Sleep();
  State = RX_TIMEOUT;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_PingPong_Process), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN OnRxTimeout_2 */

  /* USER CODE END OnRxTimeout_2 */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError_1 */

  /* USER CODE END OnRxError_1 */
  APP_LOG(TS_ON, VLEVEL_L, "OnRxError\n\r");

  Radio.Sleep();
  State = RX_ERROR;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_PingPong_Process), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN OnRxError_2 */

  /* USER CODE END OnRxError_2 */
}

static void OnledEvent(void *context)
{
  /* USER CODE BEGIN OnledEvent_1 */

  /* USER CODE END OnledEvent_1 */
#if defined(USE_BSP_DRIVER)
  BSP_LED_Toggle(LED_GREEN) ;
  BSP_LED_Toggle(LED_RED) ;
#elif defined(MX_BOARD_PSEUDODRIVER)
  SYS_LED_Toggle(SYS_LED_RED) ;
  SYS_LED_Toggle(SYS_LED_GREEN) ;
#endif /* USE_BSP_DRIVER || MX_BOARD_PSEUDODRIVER */

  UTIL_TIMER_Start(&timerLed);
  /* USER CODE BEGIN OnledEvent_2 */

  /* USER CODE END OnledEvent_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
