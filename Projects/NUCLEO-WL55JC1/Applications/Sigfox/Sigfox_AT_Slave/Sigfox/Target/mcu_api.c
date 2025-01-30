/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mcu_api.c
  * @author  MCD Application Team
  * @brief   mcu library interface
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
#include "sigfox_types.h"
#include "sigfox_api.h"
#include "mcu_api.h"
#include "stm32_timer.h"
#include "stm32_seq.h"
#include "sgfx_eeprom_if.h"
#include "sys_debug.h"
#include "sgfx_cstimer.h"
#include "adc_if.h"
#include "utilities_def.h"
#include "sys_app.h" /*for APP_LOG*/
#include "radio_board_if.h"
#include "radio_conf.h" /* RF_WAKEUP_TIME */

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define MCU_VER "MCU_API_V1.0"
#define LIBRARY_MEM_SIZE_MAX 200

#define TERM_RESET   "\033[0m"
#define TERM_BLACK   "\033[30m"      /* Black */
#define TERM_RED     "\033[31m"      /* Red   */
#define TERM_GREEN   "\033[32m"      /* Green */

#define T_RADIO_DELAY_ON (RF_WAKEUP_TIME)
#define T_RADIO_DELAY_OFF (11*10)

#if RF_WAKEUP_TIME<50
#define ARIB_DELAY (50-RF_WAKEUP_TIME)
#else
#define ARIB_DELAY 1
#endif /* RF_WAKEUP_TIME */

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
#ifndef ALIGN
#define ALIGN(n)             __attribute__((aligned(n)))
#endif /* !ALIGN */

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static int32_t rxcount = 0;
static int32_t okcount = 0;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static sfx_u8 mcu_api_version[] = MCU_VER;
/*The allocated memory must be word aligned */
static uint8_t LibraryMem[LIBRARY_MEM_SIZE_MAX] ALIGN(4);

static UTIL_TIMER_Object_t Timer_delayMs;

static void OnTimerDelayEvt(void *context);

/*timer to handle low power delays*/
static UTIL_TIMER_Object_t Timer_Timeout;

static void OnTimerTimeoutEvt(void *context);

/*timer to handleCarrier Sense Timeout*/
static UTIL_TIMER_Object_t Timer_TimeoutCs;

static void Delay_Lp(uint32_t delay_ms);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
sfx_u8 MCU_API_malloc(sfx_u16 size, sfx_u8 **returned_pointer)
{
  sfx_error_t error = SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_malloc_1 */

  /* USER CODE END MCU_API_malloc_1 */

  /* Allocate a memory : static or dynamic allocation */
  /* knowing that the sigfox library will ask for a buffer once at the SIGFOX_API_open() call. */
  /* This buffer will be released after SIGFOX_API_close() call. */
  if (size <= LIBRARY_MEM_SIZE_MAX)
  {
    /* The memory block is free, we can allocate it */
    *returned_pointer = LibraryMem;
  }
  else
  {
    /* No block available */
    error = MCU_ERR_API_MALLOC;
  }

  /* USER CODE BEGIN MCU_API_malloc_2 */

  /* USER CODE END MCU_API_malloc_2 */
  return error;
}

/*******************************************************************/
sfx_u8 MCU_API_free(sfx_u8 *ptr)
{
  /* USER CODE BEGIN MCU_API_free_1 */

  /* USER CODE END MCU_API_free_1 */
  /* Free the buffer allocated during the MCU_API_malloc()
   * called only when SIGFOX_API_close() is called. */
  return SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_free_2 */

  /* USER CODE END MCU_API_free_2 */
}

/*******************************************************************/
sfx_u8 MCU_API_get_voltage_temperature(sfx_u16 *voltage_idle, sfx_u16 *voltage_tx, sfx_s16 *temperature)
{
  sfx_u8 ret = SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_get_voltage_temperature_1 */

  /* USER CODE END MCU_API_get_voltage_temperature_1 */

  /* Get the idle voltage of the complete device
   * Get the TX voltage of the complete device ( during transmission )
   * Get the temperature of the device
   * If those values are not available : set it to 0x0000
   * return the voltage_idle in 1/10 volt on 16bits and 1/10 degrees for the temperature */
  *voltage_idle = (sfx_u16) SYS_GetBatteryLevel(); /* mV */
  *voltage_tx = 0;   /* mV */
  *temperature = (sfx_s16)(GetTemperatureLevel() * 10);  /* */

  APP_LOG(TS_ON, VLEVEL_M, "temp=%d , ", (int32_t) *temperature);
  APP_LOG(TS_OFF, VLEVEL_M, "voltage=%u\n\r", (uint32_t) *voltage_idle);

  /* USER CODE BEGIN MCU_API_get_voltage_temperature_2 */

  /* USER CODE END MCU_API_get_voltage_temperature_2 */
  return ret;
}

/*******************************************************************/
sfx_u8 MCU_API_delay(sfx_delay_t delay_type)
{
  /* Local variables */
  sfx_error_t err = SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_delay_1 */

  /* USER CODE END MCU_API_delay_1 */

  sfx_rc_enum_t sfx_rc = E2P_Read_Rc();

  /* Switch/case */
  switch (delay_type)
  {
    case SFX_DLY_INTER_FRAME_TRX :
      /* Interframe used when sending an Uplink Message with a downlink request */
      /* Delay  is 500ms  in FCC and ETSI
       * In ARIB : minimum delay is 50 ms */
      /* USER CODE BEGIN SFX_DLY_INTER_FRAME_TRX1 */

      /* USER CODE END SFX_DLY_INTER_FRAME_TRX1 */
      if ((sfx_rc == SFX_RC3A) ||
          (sfx_rc == SFX_RC3C) ||
          (sfx_rc == SFX_RC5))
      {
        /* 50 ms */
        Delay_Lp(ARIB_DELAY);
      }
      else if ((sfx_rc == SFX_RC2) ||
               (sfx_rc == SFX_RC4))
      {
        Delay_Lp(510 - T_RADIO_DELAY_ON - T_RADIO_DELAY_OFF / 6); /* 500-525ms */
      }
      else
      {
        Delay_Lp(510 - (T_RADIO_DELAY_ON) - T_RADIO_DELAY_OFF); /* 500-525 ms */
      }
      /* USER CODE BEGIN SFX_DLY_INTER_FRAME_TRX2 */

      /* USER CODE END SFX_DLY_INTER_FRAME_TRX2 */
      break;

    case SFX_DLY_INTER_FRAME_TX :
      /* Interframe used when sending an Uplink Message without downlink request */
      /* Start delay 0 seconds to 2 seconds in FCC and ETSI*/
      /* In ARIB : minimum delay is 50 ms */
      /* USER CODE BEGIN SFX_DLY_INTER_FRAME_TX1 */

      /* USER CODE END SFX_DLY_INTER_FRAME_TX1 */
      if ((sfx_rc == SFX_RC3A) ||
          (sfx_rc == SFX_RC3C) ||
          (sfx_rc == SFX_RC5))
      {
        /* 50 ms */
        Delay_Lp(ARIB_DELAY);
      }
      else
      {
        Delay_Lp(1000 - T_RADIO_DELAY_ON - T_RADIO_DELAY_OFF); /* 1000 ms */
      }
      /* USER CODE BEGIN SFX_DLY_INTER_FRAME_TX2 */

      /* USER CODE END SFX_DLY_INTER_FRAME_TX2 */
      break;

    case SFX_DLY_OOB_ACK :
      /*  Delay between RX frame and sending ack in oob service frame*/
      /* USER CODE BEGIN SFX_DLY_OOB_ACK1 */

      /* USER CODE END SFX_DLY_OOB_ACK1 */
      /* Start delay between 1.4 seconds to 4 seconds in FCC and ETSI */
      Delay_Lp(1600 - T_RADIO_DELAY_ON);
      /* USER CODE BEGIN SFX_DLY_OOB_ACK2 */

      /* USER CODE END SFX_DLY_OOB_ACK2 */
      break;

    case SFX_DLY_CS_SLEEP :
      /* Delay between attempts of carrier sense for the first frame*/
      /* USER CODE BEGIN SFX_DLY_CS_SLEEP1 */

      /* USER CODE END SFX_DLY_CS_SLEEP1 */
      Delay_Lp(500 - T_RADIO_DELAY_ON); /* 500 ms */
      /* USER CODE BEGIN SFX_DLY_CS_SLEEP2 */

      /* USER CODE END SFX_DLY_CS_SLEEP2 */
      break;
    /* USER CODE BEGIN SFX_DLY_ADD_CASE */

    /* USER CODE END SFX_DLY_ADD_CASE */
    default :
      err = MCU_ERR_API_DLY;
      /* USER CODE BEGIN SFX_DLY_DEFAULT */

      /* USER CODE END SFX_DLY_DEFAULT */
      break;
  }
  /* USER CODE BEGIN MCU_API_delay_Last */

  /* USER CODE END MCU_API_delay_Last */
  return err;
}

/*******************************************************************/
sfx_u8 MCU_API_get_nv_mem(sfx_u8 read_data[SFX_NVMEM_BLOCK_SIZE])
{
  sfx_u8 ret = SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_get_nv_mem_1 */

  /* USER CODE END MCU_API_get_nv_mem_1 */
  /* Read out SFX_NVMEM_BLOCK_SIZE bytes in eeprom or equivalent
   * in flash as Non Volatile Memory (NVM)
   * This cannot be in RAM : must be remanent*/
  if (E2P_Read_McuNvm(read_data, SFX_NVMEM_BLOCK_SIZE) != E2P_OK)
  {
    return MCU_ERR_API_GETNVMEM;
  }
  /* USER CODE BEGIN MCU_API_get_nv_mem_2 */

  /* USER CODE END MCU_API_get_nv_mem_2 */
  return ret;
}

/*******************************************************************/
sfx_u8 MCU_API_set_nv_mem(sfx_u8 data_to_write[SFX_NVMEM_BLOCK_SIZE])
{
  sfx_u8 ret = SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_set_nv_mem_1 */

  /* USER CODE END MCU_API_set_nv_mem_1 */
  /* Write down SFX_NVMEM_BLOCK_SIZE bytes
   * in the eeprom or flash such as it can be retrieved
   * later even if device is switched off or rebooting*/
  if (E2P_Write_McuNvm(data_to_write, SFX_NVMEM_BLOCK_SIZE) != E2P_OK)
  {
    return MCU_ERR_API_SETNVMEM;
  }
  /* USER CODE BEGIN MCU_API_set_nv_mem_2 */

  /* USER CODE END MCU_API_set_nv_mem_2 */
  return ret;
}

/*******************************************************************/
sfx_u8 MCU_API_timer_start_carrier_sense(sfx_u16 time_duration_in_ms)
{
  sfx_u8 ret = SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_timer_start_carrier_sense_1 */

  /* USER CODE END MCU_API_timer_start_carrier_sense_1 */
  /* Starts a timer for the carrier sense. You can plug your timer
   * expiration to a callback that will update the timer event
   * status checked by RF_API_wait_for_clear_channel*/
  /* Starts CS timeout during time_duration_in_ms */

  APP_LOG(TS_ON, VLEVEL_M, "CS timeout Started= %d msec\n\r", time_duration_in_ms);
  UTIL_TIMER_Create(&Timer_TimeoutCs, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTimerTimeoutCsEvt, NULL);
  UTIL_TIMER_Stop(&Timer_TimeoutCs);
  UTIL_TIMER_SetPeriod(&Timer_TimeoutCs,  time_duration_in_ms);
  UTIL_TIMER_Start(&Timer_TimeoutCs);
  /* USER CODE BEGIN MCU_API_timer_start_carrier_sense_2 */

  /* USER CODE END MCU_API_timer_start_carrier_sense_2 */
  return ret;
}

/*******************************************************************/
sfx_u8 MCU_API_timer_start(sfx_u32 time_duration_in_s)
{
  sfx_u8 ret = SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_timer_start_1 */

  /* USER CODE END MCU_API_timer_start_1 */
  /* Starts MCU timer during time_duration_in_ms */
  /* This timer start can install a callback to indicate when it occurs
   * This event will be caught in the MCU_API_wait_for_end_timer() function. */
  uint32_t time_duration_in_millisec = time_duration_in_s * 1000;
  APP_LOG(TS_ON, VLEVEL_M, "TIM timeout Started= %d sec\n\r", time_duration_in_s);

  UTIL_TIMER_Create(&Timer_Timeout, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTimerTimeoutEvt, NULL);
  UTIL_TIMER_Stop(&Timer_Timeout);
  UTIL_TIMER_SetPeriod(&Timer_Timeout, time_duration_in_millisec);
  UTIL_TIMER_Start(&Timer_Timeout);

  /* USER CODE BEGIN MCU_API_timer_start_2 */

  /* USER CODE END MCU_API_timer_start_2 */

  return ret;
}

/*******************************************************************/
sfx_u8 MCU_API_timer_stop(void)
{
  sfx_u8 ret = SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_timer_stop_1 */

  /* USER CODE END MCU_API_timer_stop_1 */
  /* Stops MCU timer started in MCU_API_timer_start*/
  APP_LOG(TS_ON, VLEVEL_M, "timer_stop\n\r");

  UTIL_TIMER_Stop(&Timer_Timeout);
  /* USER CODE BEGIN MCU_API_timer_stop_2 */

  /* USER CODE END MCU_API_timer_stop_2 */
  return ret;
}

/*******************************************************************/
sfx_u8 MCU_API_timer_stop_carrier_sense(void)
{
  sfx_u8 ret = SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_timer_stop_carrier_sense_1 */

  /* USER CODE END MCU_API_timer_stop_carrier_sense_1 */
  /* Abort the carrier sense: stops CS timer */
  APP_LOG(TS_ON, VLEVEL_M, "CS timer_stop\n\r");

  UTIL_TIMER_Stop(&Timer_TimeoutCs);
  /* USER CODE BEGIN MCU_API_timer_stop_carrier_sense_2 */

  /* USER CODE END MCU_API_timer_stop_carrier_sense_2 */
  return ret;
}

/*******************************************************************/
sfx_u8 MCU_API_timer_wait_for_end(void)
{
  sfx_u8 ret = SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_timer_wait_for_end_1 */

  /* USER CODE END MCU_API_timer_wait_for_end_1 */
  /* This function is waiting (actively or not) on the event for end timer
   * (started by MCU_API_timer_start) elapsed and then returns*/
  APP_LOG(TS_ON, VLEVEL_M, "TIM timeout Wait\n\r");

  UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_Timeout);
  /* USER CODE BEGIN MCU_API_timer_wait_for_end_2 */

  /* USER CODE END MCU_API_timer_wait_for_end_2 */
  return ret;
}

/*******************************************************************/
sfx_u8 MCU_API_report_test_result(sfx_bool status, sfx_s16 rssi)
{
  sfx_u8 ret = SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_report_test_result_1 */

  /* USER CODE END MCU_API_report_test_result_1 */
  /* this function is used to to report status and rssi
   * optionally, it can log data on terminal*/
  rxcount++;

  if (status == SFX_TRUE)
  {
    /* Last Rx Packet received*/
    /* USER CODE BEGIN SFX_TRUE_1 */
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); /* LED_GREEN */
    HAL_Delay(50);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); /* LED_GREEN */
    /* USER CODE END SFX_TRUE_1 */
    okcount++;
    APP_LOG(TS_ON, VLEVEL_H, TERM_GREEN"RX OK. RSSI= %d dBm, cnt=%d, PER=%d%""%""\n\r  "TERM_RESET, (int32_t) rssi, rxcount,
            ((rxcount - okcount) * 100) / rxcount);
    /* USER CODE BEGIN SFX_TRUE_2 */

    /* USER CODE END SFX_TRUE_2 */
  }
  else
  {
    /* Last Rx Packet on error or timeout*/
    /* USER CODE BEGIN SFX_FALSE_1 */
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); /* LED_BLUE */
    HAL_Delay(50);
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); /* LED_BLUE */
    /* USER CODE END SFX_FALSE_1 */
    APP_LOG(TS_ON, VLEVEL_H, TERM_RED"RX KO. RSSI= %d dBm, cnt=%d, PER=%d%""%""\n\r "TERM_RESET, (int32_t)rssi, rxcount,
            ((rxcount - okcount) * 100) / rxcount);
    /* USER CODE BEGIN SFX_FALSE_2 */

    /* USER CODE END SFX_FALSE_2 */
  }
  /* USER CODE BEGIN MCU_API_report_test_result_2 */

  /* USER CODE END MCU_API_report_test_result_2 */
  return ret;
}

/*******************************************************************/
sfx_u8 MCU_API_get_version(sfx_u8 **version, sfx_u8 *size)
{
  sfx_u8 ret = SFX_ERR_NONE;
  /* USER CODE BEGIN MCU_API_get_version_1 */

  /* USER CODE END MCU_API_get_version_1 */
  /* Reports version and the size of the version buffer*/
  *version = (sfx_u8 *)mcu_api_version;
  if (size == SFX_NULL)
  {
    return MCU_ERR_API_GET_VERSION;
  }
  *size = sizeof(mcu_api_version);
  /* USER CODE BEGIN MCU_API_get_version_2 */

  /* USER CODE END MCU_API_get_version_2 */

  return ret;
}

static void Delay_Lp(uint32_t delay_ms)
{
  /* USER CODE BEGIN Delay_Lp_1 */

  /* USER CODE END Delay_Lp_1 */
  APP_LOG(TS_ON, VLEVEL_M, "Delay= %d ms\n\r", delay_ms);
  if (delay_ms > 5)
  {
    UTIL_TIMER_Create(&Timer_delayMs, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, OnTimerDelayEvt, NULL);
    UTIL_TIMER_Stop(&Timer_delayMs);
    UTIL_TIMER_SetPeriod(&Timer_delayMs,  delay_ms);
    UTIL_TIMER_Start(&Timer_delayMs);
    UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_Delay);
  }
  else
  {
    HAL_Delay(delay_ms);
  }
  APP_LOG(TS_ON, VLEVEL_M, "Delay Up\n\r");
  /* USER CODE BEGIN Delay_Lp_2 */

  /* USER CODE END Delay_Lp_2 */
}
/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
static void OnTimerDelayEvt(void *context)
{
  /* USER CODE BEGIN OnTimerDelayEvt_1 */

  /* USER CODE END OnTimerDelayEvt_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_Delay);
  /* USER CODE BEGIN OnTimerDelayEvt_2 */

  /* USER CODE END OnTimerDelayEvt_2 */
}

static void OnTimerTimeoutEvt(void *context)
{
  /* USER CODE BEGIN OnTimerTimeoutEvt_1 */

  /* USER CODE END OnTimerTimeoutEvt_1 */
  UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_Timeout);

  APP_LOG(TS_ON, VLEVEL_M, "TIM timeout Stopped\n\r");
  /* USER CODE BEGIN OnTimerTimeoutEvt_2 */

  /* USER CODE END OnTimerTimeoutEvt_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
