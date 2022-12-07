/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Examples_LL/RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Src/main.c
  * @author  MCD Application Team
  * @brief   This code example shows how to configure the RTC in order to work
  *          with the WUT through the STM32WLxx RTC LL API.
  *          Peripheral initialization done using LL unitary services functions.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Oscillator time-out values */
#define LSI_TIMEOUT_VALUE          ((uint32_t)100)   /* 100 ms */
#if !defined  (LSE_TIMEOUT_VALUE)
  #define LSE_TIMEOUT_VALUE          ((uint32_t)5000)/* Time out for LSE start up in ms */
#endif /* LSE_STARTUP_TIMEOUT */
#define RTC_TIMEOUT_VALUE          ((uint32_t)1000)  /* 1 s */

/* Defines related to Clock configuration */
/* Uncomment to enable the adequate Clock Source */
#define RTC_CLOCK_SOURCE_LSE
/*#define RTC_CLOCK_SOURCE_LSI*/

#ifdef RTC_CLOCK_SOURCE_LSI
/* ck_apre=LSIFreq/(ASYNC prediv + 1) = 256Hz with LSIFreq=32 kHz RC */
#define RTC_ASYNCH_PREDIV          ((uint32_t)0x7F)
/* ck_spre=ck_apre/(SYNC prediv + 1) = 1 Hz */
#define RTC_SYNCH_PREDIV           ((uint32_t)0xF9)
/* Value defined for WUT */
#define RTC_WUT_TIME               ((uint32_t)47999)     /* 3 s */
#endif

#ifdef RTC_CLOCK_SOURCE_LSE
/* ck_apre=LSEFreq/(ASYNC prediv + 1) = 256Hz with LSEFreq=32768Hz */
#define RTC_ASYNCH_PREDIV          ((uint32_t)0x7F)
/* ck_spre=ck_apre/(SYNC prediv + 1) = 1 Hz */
#define RTC_SYNCH_PREDIV           ((uint32_t)0x00FF)
/* Value defined for WUT */
#define RTC_WUT_TIME               ((uint32_t)49151)     /* 3 s */
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

#if (USE_TIMEOUT == 1)
uint32_t Timeout = 0; /* Variable used for Timeout management */
#endif /* USE_TIMEOUT */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void     Configure_RTC(void);
void     EnterStandbyMode(void);
uint32_t WaitForSynchro_RTC(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t seconds ;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  /* System interrupt init*/
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  /* Uncomment to avoid losing connection with debugger after wakeup from Standby (Consumption will be increased) */
  //__DBGMCU_EnableDBGStandbyMode();

  /* Uncomment to freeze the RTC during debug */
  //__DBGMCU_FREEZE_RTC();

  /* Check and handle if the system was resumed from StandBy mode */
  if (LL_PWR_IsActiveFlag_C1SB() != 1)
  {
    /* ##### Run after normal reset ##### */
    /* Configure RTC to use WUT */
    Configure_RTC();

    /* Wait for RTC_TR shadow register to be updated with current time value */
    if (WaitForSynchro_RTC() != RTC_ERROR_NONE)
    {
      Error_Handler();
    }
    seconds = (uint32_t)__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));

    /* To ensure consistency between time and date, reading either RTC_SSR or RTC_TR locks the
     * values in the calendar shadow registers until RTC_DR is read.
     * Read RTC_DR in order to unlock the values in the calendar shadow registers */
    LL_RTC_ReadReg(RTC,DR);

    /* As backup registers retain data in standby mode, one backup register is used
     * to store start time at the beginning of the test */
    LL_RTC_BKP_SetRegister(RTC, LL_RTC_BKP_DR0, seconds);
  }
  else
  {
    /* ##### Run after standby mode ##### */
    /* Enable RTC APB clock  */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_RTCAPB);
    /* Clear Standby flag */
    LL_PWR_ClearFlag_C1STOP_C1STB();

    /* Use another backup register to count the number of times the CPU wakes up */
    LL_PWR_EnableBkUpAccess();
    uint32_t number_of_wakeup = LL_RTC_BKP_GetRegister(RTC, LL_RTC_BKP_DR1) + 1;
    LL_RTC_BKP_SetRegister(RTC, LL_RTC_BKP_DR1, number_of_wakeup);
    uint32_t start_time = LL_RTC_BKP_GetRegister(RTC, LL_RTC_BKP_DR0);    

    /* Check that the current time is ok after 5 times the MCU woke up */
    if (number_of_wakeup == 5)
    {
      /* Wait for RTC_TR shadow register to be updated with current time value */
      if (WaitForSynchro_RTC() != RTC_ERROR_NONE)
      {
        Error_Handler();
      }

      /* Get the RTC calendar time */
      seconds = (uint32_t)__LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));

        /* 15 seconds = 5 wakeup after 3 seconds */
      if (((seconds - start_time) == 15) || ((seconds + 60 - start_time) == 15))
      {
        /* Clear Backup reg */
        LL_RTC_BKP_SetRegister(RTC, LL_RTC_BKP_DR0, 0);
        LL_RTC_BKP_SetRegister(RTC, LL_RTC_BKP_DR1, 0);

        /* the test is OK */
        LL_GPIO_SetOutputPin(LED2_GPIO_Port, LED2_Pin);
        while(1);
      }
      else
      {
          Error_Handler();
      }
    }
    else
    {
      /* Toggle the LED_GREEN to indicate a wakeup from STANDBY */
      for (uint32_t i = 0; i < 4; i++)
      {
        LL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
        LL_mDelay(100);
      }
    }

  }

  /* Enable wakeup timer and enter in standby mode */
  EnterStandbyMode();
  /* This code will be never reached */
  Error_Handler();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
  {
  }

  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_MSI_Enable();

   /* Wait till MSI is ready */
  while(LL_RCC_MSI_IsReady() != 1)
  {
  }

  LL_RCC_MSI_EnableRangeSelection();
  LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_11);
  LL_RCC_MSI_SetCalibTrimming(0);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_MSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_MSI)
  {
  }

  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAHB3Prescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  LL_Init1msTick(48000000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(48000000);
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

  /**/
  LL_GPIO_ResetOutputPin(LED2_GPIO_Port, LED2_Pin);

  /**/
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/**
  * Brief   This function configures RTC.
  * Param   None
  * Retval  None
  */
void Configure_RTC(void)
{
  LL_RTC_InitTypeDef RTC_InitStruct = {0};
  LL_RTC_TimeTypeDef RTC_TimeStruct = {0};
  LL_RTC_DateTypeDef RTC_DateStruct = {0};
  /*##-1- Enables the PWR Clock and Enables access to the backup domain #######*/
  /* To change the source clock of the RTC feature (LSE, LSI), you have to:
     - Enable the power clock
     - Enable write access to configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain
     - Configure the needed RTC clock source */
  LL_PWR_EnableBkUpAccess();
  LL_RCC_ForceBackupDomainReset();
  LL_RCC_ReleaseBackupDomainReset();

  /*##-2- Configure LSE/LSI as RTC clock source ###############################*/
#ifdef RTC_CLOCK_SOURCE_LSE
  /* Enable LSE only if disabled.*/
  if (LL_RCC_LSE_IsReady() == 0)
  {

    LL_RCC_LSE_Enable();
#if (USE_TIMEOUT == 1)
    Timeout = LSE_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
    while (LL_RCC_LSE_IsReady() != 1)
    {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        Timeout --;
      }
      if (Timeout == 0)
      {
        /* LSE activation error */
        Error_Handler();
      }
#endif /* USE_TIMEOUT */
    }
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);

    /*##-3- Enable RTC peripheral Clocks #######################################*/
    /* Enable RTC Clock */
    LL_RCC_EnableRTC();
  }
#elif defined(RTC_CLOCK_SOURCE_LSI)
  if (LL_RCC_LSI_IsReady() == 0)
  {

    LL_RCC_LSI_Enable();
#if (USE_TIMEOUT == 1)
    Timeout = LSI_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */
    while (LL_RCC_LSI_IsReady() != 1)
    {
#if (USE_TIMEOUT == 1)
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        Timeout --;
      }
      if (Timeout == 0)
      {
        /* LSI activation error */
        Error_Handler();
      }
#endif /* USE_TIMEOUT */
    }
    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);

    /*##-3- Enable RTC peripheral Clocks #######################################*/
    /* Enable RTC Clock */
    LL_RCC_EnableRTC();
  }

#else
#error "configure clock for RTC"
#endif

  /*##-4- Configure RTC ######################################################*/
/* Enable RTC APB clock  */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_RTCAPB);
  /* Disable RTC registers write protection */
  LL_RTC_DisableWriteProtection(RTC);

  RTC_InitStruct.HourFormat = LL_RTC_HOURFORMAT_AMPM;
  RTC_InitStruct.AsynchPrescaler = RTC_ASYNCH_PREDIV;
  RTC_InitStruct.SynchPrescaler = RTC_SYNCH_PREDIV;
  LL_RTC_Init(RTC, &RTC_InitStruct);
  RTC_TimeStruct.TimeFormat = LL_RTC_TIME_FORMAT_PM;
  RTC_TimeStruct.Hours = 10;
  RTC_TimeStruct.Minutes = 50;
  RTC_TimeStruct.Seconds = 40;
  LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_TimeStruct);
  RTC_DateStruct.WeekDay = LL_RTC_WEEKDAY_MONDAY;
  RTC_DateStruct.Month = LL_RTC_MONTH_JANUARY;
  RTC_DateStruct.Year = 20;
  LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BIN, &RTC_DateStruct);

  /* Wait until it is allowed to Enable Calibration Low Power */
  while (LL_RTC_IsActiveFlag_RECALP(RTC) != 0){}

  /* Enable Calibration Low Power */
  LL_RTC_CAL_LowPower_Enable(RTC);

/*##-5- Configure the wakeup Timer WUT ##########################################*/
  /* Disable wake up timer to modify it */
  LL_RTC_WAKEUP_Disable(RTC);

  /* Wait until it is allow to modify wake up reload value */
#if (USE_TIMEOUT == 1)
  Timeout = RTC_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */

  while (LL_RTC_IsActiveFlag_WUTW(RTC) != 1)
  {
#if (USE_TIMEOUT == 1)
    if (LL_SYSTICK_IsActiveCounterFlag())
    {
      Timeout --;
    }
    if (Timeout == 0)
    {
      /* LSI activation error */
      Error_Handler();
    }
#endif /* USE_TIMEOUT */
  }

  /* Configure wakeup timer clock source: RTC/2 clock is selected  */
  LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_DIV_2);

  /* Configure the Wake up timer to periodically wake up the system every 3 seconds.
   * Wakeup timer auto-reload value (WUT[15:0] in RTC_WUTR) is calculated as follow:
   * If LSE is used as RTC clock source and RTC/2 clock is selected (prescaler = 2): auto-reload = (3u * (32768u / 2u)) - 1 = 49151.
   * If LSI is used as RTC clock source and RTC/2 clock is selected (prescaler = 2): auto-reload = (3u * (32000u / 2u)) - 1 = 47999.   
   * Wakeup auto-reload output clear value (WUTOCLR[15:0] in RTC_WUTR) is set in order to
   * automatically clear wakeup timer flag (WUTF) by hardware.(Please refer to reference manual for further details)*/
  LL_RTC_WAKEUP_SetAutoReload(RTC, (uint32_t)(RTC_WUT_TIME | (RTC_WUT_TIME << RTC_WUTR_WUTOCLR_Pos)));

  /* As Wake up auto clear value (WUTOCLR) is set, we configure EXTI as EVENT instead of Interrupt
     to avoid useless IRQ handler execution.*/
  __LL_RTC_WAKEUPTIMER_EXTI_ENABLE_EVENT();

  /* Enable RTC registers write protection */
  LL_RTC_EnableWriteProtection(RTC);

}
/**
  * @brief  Function to configure and enter in STANDBY Mode.
  * @param  None
  * @retval None
  */
void EnterStandbyMode(void)
{
  /* ######## ENABLE WUT #################################################*/
  /* Disable RTC registers write protection */
  LL_RTC_DisableWriteProtection(RTC);

  /* Enable wake up counter and wake up interrupt */
  /* Note: Periodic wakeup interrupt should be enabled to exit the device
     from low-power modes.*/
  LL_RTC_EnableIT_WUT(RTC);
  LL_RTC_WAKEUP_Enable(RTC);

  /* Enable RTC registers write protection */
  LL_RTC_EnableWriteProtection(RTC);

  /* ######## ENTER IN STANDBY MODE ######################################*/
  /** Request to enter STANDBY mode
    * Following procedure describe in STM32WLxx Reference Manual
    * See PWR part, section Low-power modes, Standby mode
    */

  /* Set Standby mode */
  LL_PWR_SetPowerMode(LL_PWR_MODE_STANDBY);
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  LL_LPM_EnableDeepSleep();

  /* This option is used to ensure that store operations are completed */
#if defined ( __CC_ARM)
  __force_stores();
#endif

  /* Request Wait For Interrupt */
  __WFI();
}
/**
  * @brief  Wait until the RTC Time and Date registers (RTC_TR and RTC_DR) are
  *         synchronized with RTC APB clock.
  * @param  None
  * @retval RTC_ERROR_NONE if no error occurs
  *         RTC_ERROR_TIMEOUT if synchronization fails
  */
uint32_t WaitForSynchro_RTC(void)
{
  uint32_t status = RTC_ERROR_NONE;

  /* Disable RTC registers write protection to allow clearing the RSF flag */
  LL_RTC_DisableWriteProtection(RTC);

  /* Clear RSF flag */
  LL_RTC_ClearFlag_RS(RTC);

#if (USE_TIMEOUT == 1)
  Timeout = RTC_TIMEOUT_VALUE;
#endif /* USE_TIMEOUT */

  /* Wait the registers to be synchronised */
  while (LL_RTC_IsActiveFlag_RS(RTC) != 1)
  {
#if (USE_TIMEOUT == 1)
    if (LL_SYSTICK_IsActiveCounterFlag())
    {
      Timeout --;
    }
    if (Timeout == 0)
    {
      status = RTC_ERROR_TIMEOUT;
    }
#endif /* USE_TIMEOUT */
  }

  /* Set back RTC registers write protection */
  LL_RTC_EnableWriteProtection(RTC);

  return status;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  while (1)
  {
    LL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    LL_mDelay(100);
  }
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
