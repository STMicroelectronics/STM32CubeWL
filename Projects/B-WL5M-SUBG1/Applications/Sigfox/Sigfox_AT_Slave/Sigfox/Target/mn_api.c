/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mn_api.c
  * @author  MCD Application Team
  * @brief   monarch library interface implementation
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
#include "stdint.h"
#include "stm32wlxx_hal.h"

#include "mn_api.h"
#include "mn_lptim_if.h"
#include "radio.h"
#include "platform.h"          /*For led*/
#include "sys_app.h"           /*For log*/
#include "stm32_lpm.h"
#include "stm32_timer.h"
#include "stm32_seq.h"
#include "utilities_def.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/**
  * @brief LPTIM handle
  */
extern LPTIM_HandleTypeDef hlptim1;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define LPTIM_PERIOD     ((uint32_t)  2)        /* 1/16384~61.03us  */

#define LPTIM_PULSE      ((uint32_t) 0 )

#define RADIO_NOISE_LEVEL (int16_t) (-130)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* RSSi register to be restored after end of Monarch*/
static uint8_t Reg0x089B;

#define SIZE_OF_RSSI_BUFF   (1<<4)
int16_t RssiBuff[SIZE_OF_RSSI_BUFF];
__IO int16_t RssiBuffWriteIdx;
int16_t RssiBuffReadIdx;
static void process_MonarchBackGround(void);

/* Flag to disable rssi read transfer (in timer cB) while configuring radio*/
static uint8_t RssiReadOnIT_Enable = 1;

/*Monarch timeout timer*/
static UTIL_TIMER_Object_t Monarch_TimerTimeout;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*monarch API callback at every timer interrupt*/
static void (*MN_Timer_CB)(int16_t rssi);

/**
  * @brief  Starts Timer
  * @param  None
  * @retval None
  */
static void MN_TIM_Start(void);

/**
  * @brief  Stops Timer
  * @param  None
  * @retval None
  */
static void MN_TIM_Stop(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
void MN_API_Init(void (*MN_API_Timer_CB)(int16_t rssi))
{
  /* USER CODE BEGIN MN_API_Init_1 */

  /* USER CODE END MN_API_Init_1 */
  /*Register call back*/
  MN_Timer_CB = MN_API_Timer_CB;

  MN_LPTIM_IF_Init();

  APP_LOG(TS_ON, VLEVEL_M, "MN Init\r\n");

  Reg0x089B = Radio.Read(0x089B);

  Radio.Write(0x089B, 0x0);

  UTIL_LPM_SetStopMode((1 << CFG_LPM_SGFX_MN_Id), UTIL_LPM_DISABLE);

  /*reset write indices*/
  RssiBuffWriteIdx = 0;
  /*reset read indices*/
  RssiBuffReadIdx = 0;
  /*reset RssiBuff*/
  UTIL_MEM_set_8((uint8_t *) RssiBuff, 0, SIZE_OF_RSSI_BUFF * sizeof(int16_t));
  /* register process_MonarchBackGround under CFG_SEQ_TASK_MONARCH */
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_Monarch), UTIL_SEQ_RFU, process_MonarchBackGround);

  /* USER CODE BEGIN MN_API_Init_2 */

  /* USER CODE END MN_API_Init_2 */
}

void MN_API_DeInit(void)
{
  /* USER CODE BEGIN MN_API_DeInit_1 */

  /* USER CODE END MN_API_DeInit_1 */
  MN_TIM_Stop();

  MN_LPTIM_IF_DeInit();

  UTIL_LPM_SetStopMode((1 << CFG_LPM_SGFX_MN_Id), UTIL_LPM_ENABLE);

  Radio.Write(0x089B, Reg0x089B);

  APP_LOG(TS_ON, VLEVEL_M, "MN Deinit\r\n");
  /* USER CODE BEGIN MN_API_DeInit_2 */

  /* USER CODE END MN_API_DeInit_2 */
}

void MN_API_StartRx(void)
{
  /* USER CODE BEGIN MN_API_StartRx_1 */

  /* USER CODE END MN_API_StartRx_1 */
  RssiReadOnIT_Enable = 0;

  Radio.RxBoosted(0);

  RssiReadOnIT_Enable = 1;
  /* USER CODE BEGIN MN_API_StartRx_2 */

  /* USER CODE END MN_API_StartRx_2 */
}

void MN_API_StopRx(void)
{
  /* USER CODE BEGIN MN_API_StopRx_1 */

  /* USER CODE END MN_API_StopRx_1 */
  /*no need to standby xosc, radio will just
    change freq with MN_API_change_frequency*/
  /* USER CODE BEGIN MN_API_StopRx_2 */

  /* USER CODE END MN_API_StopRx_2 */
}

void MN_API_change_frequency(uint32_t frequency)
{
  /* USER CODE BEGIN MN_API_change_frequency_1 */

  /* USER CODE END MN_API_change_frequency_1 */
  RssiReadOnIT_Enable = 0;

  Radio.SetChannel(frequency);

  RssiReadOnIT_Enable = 1;
  /* USER CODE BEGIN MN_API_change_frequency_2 */

  /* USER CODE END MN_API_change_frequency_2 */
}

void MN_API_Enable16KHzSamplingTimer(void)
{
  /* USER CODE BEGIN MN_API_Enable16KHzSamplingTimer_1 */

  /* USER CODE END MN_API_Enable16KHzSamplingTimer_1 */
  MN_TIM_Start();
  /* USER CODE BEGIN MN_API_Enable16KHzSamplingTimer_2 */

  /* USER CODE END MN_API_Enable16KHzSamplingTimer_2 */
}

void MN_API_Disable16KHzSamplingTimer(void)
{
  /* USER CODE BEGIN MN_API_Disable16KHzSamplingTimer_1 */

  /* USER CODE END MN_API_Disable16KHzSamplingTimer_1 */
  MN_TIM_Stop();
  /* USER CODE BEGIN MN_API_Disable16KHzSamplingTimer_2 */

  /* USER CODE END MN_API_Disable16KHzSamplingTimer_2 */
}

void MN_API_Pattern_Found(int32_t window_type, int32_t pattern, int32_t frequency, int32_t best_rssi)
{
  /* USER CODE BEGIN MN_API_Pattern_Found_1 */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); /* LED_BLUE */
  /* USER CODE END MN_API_Pattern_Found_1 */
  /* debug function to display sweep/window detected pattern*/
  if (window_type == 0)
  {
    APP_LOG(TS_ON, VLEVEL_H, "Sweep detected pattern %d on freq %d at %d\n\r", pattern, frequency, best_rssi);
  }
  else
  {
    APP_LOG(TS_ON, VLEVEL_H, "Window detected pattern %d on freq %d at %d\n\r", pattern, frequency, best_rssi);
  }
  /* USER CODE BEGIN MN_API_Pattern_Found_2 */
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); /* LED_BLUE */
  /* USER CODE END MN_API_Pattern_Found_2 */
}

void MN_API_TimerSart(uint32_t timer_value_ms, void (*TimeoutHandle)(void *Argument))
{
  /* USER CODE BEGIN MN_API_TimerSart_1 */

  /* USER CODE END MN_API_TimerSart_1 */
  APP_LOG(TS_ON, VLEVEL_H, "MONARCH_TIM_START: %d ms\n\r", timer_value_ms);
  UTIL_TIMER_Create(&Monarch_TimerTimeout, 0xFFFFFFFFU, UTIL_TIMER_ONESHOT, TimeoutHandle, NULL);
  UTIL_TIMER_Stop(&Monarch_TimerTimeout);
  UTIL_TIMER_SetPeriod(&Monarch_TimerTimeout, timer_value_ms);
  UTIL_TIMER_Start(&Monarch_TimerTimeout);
  /* USER CODE BEGIN MN_API_TimerSart_2 */

  /* USER CODE END MN_API_TimerSart_2 */
}

void MN_API_TimerStop(void)
{
  /* USER CODE BEGIN MN_API_TimerStop_1 */

  /* USER CODE END MN_API_TimerStop_1 */
  UTIL_TIMER_Stop(&Monarch_TimerTimeout);

  APP_LOG(TS_ON, VLEVEL_H, "MONARCH_TIM_STOP\n\r");
  /* USER CODE BEGIN MN_API_TimerStop_2 */

  /* USER CODE END MN_API_TimerStop_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
static void MN_TIM_Start(void)
{
  /* USER CODE BEGIN MN_TIM_Start_1 */

  /* USER CODE END MN_TIM_Start_1 */
  if (HAL_LPTIM_PWM_Start_IT(&hlptim1, LPTIM_PERIOD - 1, LPTIM_PULSE) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MN_TIM_Start_2 */

  /* USER CODE END MN_TIM_Start_2 */
}

static void MN_TIM_Stop(void)
{
  /* USER CODE BEGIN MN_TIM_Stop_1 */

  /* USER CODE END MN_TIM_Stop_1 */
  /* stop the timer */
  if (HAL_LPTIM_PWM_Stop_IT(&hlptim1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MN_TIM_Stop_2 */

  /* USER CODE END MN_TIM_Stop_2 */
}

/*16kHz timer timeout call back*/
/* Here and pass the bit/rssi as parma for monarch processing*/
void HAL_LPTIM_AutoReloadMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* USER CODE BEGIN HAL_LPTIM_AutoReloadMatchCallback_1 */

  /* USER CODE END HAL_LPTIM_AutoReloadMatchCallback_1 */
  int16_t rssi = RADIO_NOISE_LEVEL;

  /*read rssi*/
  if (RssiReadOnIT_Enable == 1)
  {
    rssi = Radio.Rssi(MODEM_FSK);
    /*record rssi in buffer*/
    RssiBuff[RssiBuffWriteIdx++] = rssi;
    if (RssiBuffWriteIdx == SIZE_OF_RSSI_BUFF)
    {
      RssiBuffWriteIdx = 0;
    }
    /*run process_MonarchBackGround in background*/
    UTIL_SEQ_SetTask(1 << CFG_SEQ_Task_Monarch, CFG_SEQ_Prio_0);
  }
  /* USER CODE BEGIN HAL_LPTIM_AutoReloadMatchCallback_2 */

  /* USER CODE END HAL_LPTIM_AutoReloadMatchCallback_2 */
}

static void process_MonarchBackGround(void)
{
  /* USER CODE BEGIN process_MonarchBackGround_1 */

  /* USER CODE END process_MonarchBackGround_1 */
  int16_t rssi;

  while (RssiBuffWriteIdx != RssiBuffReadIdx)
  {
    rssi = RssiBuff[RssiBuffReadIdx++];

    if (RssiBuffReadIdx == SIZE_OF_RSSI_BUFF)
    {
      RssiBuffReadIdx = 0;
    }
    MN_Timer_CB(rssi);
  }
  /* USER CODE BEGIN process_MonarchBackGround_2 */

  /* USER CODE END process_MonarchBackGround_2 */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
