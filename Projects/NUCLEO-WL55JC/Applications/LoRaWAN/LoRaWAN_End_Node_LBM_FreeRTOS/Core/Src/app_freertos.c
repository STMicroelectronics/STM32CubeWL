/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "stm32_lpm.h"
#include "app_lorawan.h"
#include "app_dispatcher.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define configTICK_RATE_HZ_1MS                 1000

#define portNVIC_SYSTICK_CTRL_REG               ( * ( ( volatile uint32_t * ) 0xe000e010 ) )
#define portNVIC_SYSTICK_LOAD_REG               ( * ( ( volatile uint32_t * ) 0xe000e014 ) )
#define portNVIC_SYSTICK_CURRENT_VALUE_REG      ( * ( ( volatile uint32_t * ) 0xe000e018 ) )
#define portNVIC_SYSTICK_ENABLE_BIT             ( 1UL << 0UL )
#define CORE_TICK_RATE                          (( configCPU_CLOCK_HZ ) / ( configTICK_RATE_HZ ))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static UTIL_TIMER_Object_t WakeUpTimer;
static volatile uint32_t Time_BeforeSleep;
uint32_t WaitTimeoutSleep_time_ms = 0;
/* USER CODE END Variables */
/* Definitions for LoraTask */
osThreadId_t LoraTaskHandle;
const osThreadAttr_t LoraTask_attributes = {
  .name = "LoraTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 512 * 4
};
/* Definitions for BlinkLedTask */
osThreadId_t BlinkLedTaskHandle;
const osThreadAttr_t BlinkLedTask_attributes = {
  .name = "BlinkLedTask",
  .priority = (osPriority_t) osPriorityBelowNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/**
  * @brief  Callback when wakeup: do nothing
  * @param  None
  * @retval None
  */
static void  WakeUpTimer_Cb(void *context);

/**
  * @brief  Convert time in ms to RTOS ticks
  * @param  value in milliseconds
  * @retval value in RTOS tick
  */
static uint32_t app_freertos_ms_to_tick(uint32_t ms);

/**
  * @brief  Convert time in RTOS ticks to ms
  * @param  value in RTOS tick
  * @retval value in milliseconds
  */
static uint32_t app_freertos_tick_to_ms(uint32_t tick);

void StartBlinkTask(void *argument);
/* USER CODE END FunctionPrototypes */

void StartLoraTask(void *argument);
void StartBlinkTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN PREPOSTSLEEP */
void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{
  /* Called by the kernel before it places the MCU into a sleep mode because
  configPRE_SLEEP_PROCESSING() is #defined to PreSleepProcessing().

  NOTE:  Additional actions can be taken here to get the power consumption
  even lower.  For example, peripherals can be turned off here, and then back
  on again in the post sleep processing function.  For maximum power saving
  ensure all unused pins are in their lowest power state. */

  uint32_t WakeUpTimer_timeOut_ms = app_freertos_tick_to_ms(ulExpectedIdleTime);

  UTIL_TIMER_SetPeriod(&WakeUpTimer, WakeUpTimer_timeOut_ms);
  UTIL_TIMER_Start(&WakeUpTimer);
  Time_BeforeSleep = UTIL_TIMER_GetCurrentTime();

  /*Stop the systick here so that it stops even in sleep mode*/
  portNVIC_SYSTICK_CTRL_REG &= ~portNVIC_SYSTICK_ENABLE_BIT;

  UTIL_LPM_EnterLowPower();
}

void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
  /* Called by the kernel when the MCU exits a sleep mode because
  configPOST_SLEEP_PROCESSING is #defined to PostSleepProcessing(). */
  uint32_t CurrentValue;
  uint32_t ReloadValue;
  uint32_t ElapsedTicks;
  uint64_t ElapsedCounts;
  uint32_t SleepDuration = UTIL_TIMER_GetElapsedTime(Time_BeforeSleep);

  /* Avoid compiler warnings about the unused parameter. */
  UNUSED(ulExpectedIdleTime);

  UTIL_TIMER_Stop(&WakeUpTimer);

  CurrentValue = portNVIC_SYSTICK_CURRENT_VALUE_REG;
  ElapsedTicks = app_freertos_ms_to_tick(SleepDuration);
  ElapsedCounts = (uint64_t)ElapsedTicks * (uint64_t)CORE_TICK_RATE;

  /* Set the new reload value. */
  if (ElapsedCounts < CurrentValue)
  {
    /*what remains to sleep*/
    ReloadValue = CurrentValue - (uint32_t)ElapsedCounts;

    if (ReloadValue < CORE_TICK_RATE)
    {
      ReloadValue = CORE_TICK_RATE;
    }
  }
  else
  {
    ReloadValue = CORE_TICK_RATE;
  }

  portNVIC_SYSTICK_LOAD_REG = ReloadValue;

  /* Clear the SysTick count flag and set the count value back to zero. */
  portNVIC_SYSTICK_CURRENT_VALUE_REG = 0UL;

  /* Restart SysTick. */
  portNVIC_SYSTICK_CTRL_REG |= portNVIC_SYSTICK_ENABLE_BIT;

  if (LoraTaskHandle != NULL)
  {
    (void)osThreadFlagsSet(LoraTaskHandle, APP_DISPATCHER_LORA_FLAG);
  }
}
/* USER CODE END PREPOSTSLEEP */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  UTIL_TIMER_Create(&WakeUpTimer, 0, UTIL_TIMER_ONESHOT, WakeUpTimer_Cb, NULL);

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of LoraTask */
  LoraTaskHandle = osThreadNew(StartLoraTask, NULL, &LoraTask_attributes);

  /* creation of BlinkLedTask */
  BlinkLedTaskHandle = osThreadNew(StartBlinkTask, NULL, &BlinkLedTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartLoraTask */
/**
  * @brief  Function implementing the LoraTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartLoraTask */
void StartLoraTask(void *argument)
{
  /* USER CODE BEGIN StartLoraTask */
  /* init code for LoRaWAN */
  MX_LoRaWAN_Init();
  /* Infinite loop */
  for(;;)
  {
    MX_LoRaWAN_Process();

    if (WaitTimeoutSleep_time_ms > 0U)
    {
      (void)osThreadFlagsWait(APP_DISPATCHER_LORA_FLAG, osFlagsWaitAll, app_freertos_ms_to_tick(WaitTimeoutSleep_time_ms));
    }
  }
  /* USER CODE END StartLoraTask */
}

/* USER CODE BEGIN Header_StartBlinkTask */
/**
* @brief Function implementing the BlinkLedTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBlinkTask */
void StartBlinkTask(void *argument)
{
  /* USER CODE BEGIN StartBlinkTask */
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    (void)osDelay(10000);
  }
  /* USER CODE END StartBlinkTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
static void WakeUpTimer_Cb(void *context)
{
  /*Nothing to do*/
  UNUSED(context);
}

static uint32_t app_freertos_ms_to_tick(uint32_t ms)
{
  uint32_t tick = ms;
  if (configTICK_RATE_HZ != configTICK_RATE_HZ_1MS)
  {
    tick = (uint32_t)((((uint64_t)(ms)) * configTICK_RATE_HZ) / configTICK_RATE_HZ_1MS);
  }
  return tick;
}

static uint32_t app_freertos_tick_to_ms(uint32_t tick)
{
  uint32_t ms = tick;
  if (configTICK_RATE_HZ != configTICK_RATE_HZ_1MS)
  {
    ms = (uint32_t)((((uint64_t)(tick)) * configTICK_RATE_HZ_1MS) / configTICK_RATE_HZ);
  }
  return ms;
}

/* USER CODE END Application */
