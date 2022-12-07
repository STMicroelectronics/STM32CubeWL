/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PWR/PWR_Stop2_DualCore/CM4/Src/main.c
  * @author  MCD Application Team
  * @brief   Main program body
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
#include "system_clock.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LED_TOGGLE_FREQ_FAST            ( 10 * 2) /* LED toggle frequency fast (10Hz). Note: Factor 2 to convert toggle frequency to signal frequency. */
#define LED_TOGGLE_FREQ_SLOW            (  1 * 2) /* LED toggle frequency slow ( 1Hz). Note: Factor 2 to convert toggle frequency to signal frequency. */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t button_event = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void LED_toggle(uint32_t toggle_frequency, uint32_t toggle_duration_ms);
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

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  /* Configure LEDs */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED3);

  /* Configure User push-button (B1) */
  BSP_PB_Init(BUTTON_SW1, BUTTON_MODE_EXTI);

  /* Set default state from potential system reset: Clear Stop2 flag of CPU1 */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_STOP2);

  /* USER CODE END 2 */

  /* Boot CPU2 */
  HAL_PWREx_ReleaseCore(PWR_CORE_CPU2);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(button_event == 1)
    {
      /* Turn LED off */
      BSP_LED_Off(LED1);
      
      /* Suspend systick for debug purpose (case of debugger attached to CPU1 and CPU2 in Run mode:
         if CPU1 requests to enter in DeepSleep mode, it will wake up immediately).
         In application situation (debugger not attached), this action is not needed. */
      HAL_SuspendTick();
      
      /* Request of CPU to enter in Stop2 mode */
      HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
      
      /* Resume systick */
      HAL_ResumeTick();
      
      /* Check system state at wake up */
      /* Note: Flag PWR_FLAG_C2STOP2 is cleared by program of CPU2 */
      if(   (__HAL_PWR_GET_FLAG(PWR_FLAG_STOP2) != 0)
         && (__HAL_PWR_GET_FLAG(PWR_FLAG_C2STOP2) != 0)
        )
      {
        /* Wake up from system Stop (the other CPU is still in Stop, Standby or Shutdown mode)
           - For the purpose of this example, check that system clock is at the
             expected frequency at wake up from system Stop: MSI 4MHz
           - Reconfigure clock as initially done by "SystemClock_Config()"
           - toggle LED during 4s
        */
        
        /* Clear Stop2 flag of CPU1 */
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_STOP2);
        
        /* Check system frequency */
        SystemCoreClock = HAL_RCC_GetHCLKFreq();
        if(SystemCoreClock != 4000000)
        {
          Error_Handler();
        }
        
        if(SystemClock_Config_RestoreFromStop() != HAL_OK)
        {
          Error_Handler();
        }
        
        LED_toggle(LED_TOGGLE_FREQ_FAST, 4);
      }
      else
      {
        /* Wake up from system Run
           - toggle LED during 2s or 1s depending on system state during this CPU low-power mode
        */
        /* Note: This CPU wakes up at system Run, but does not know what happened
           during time where it was in CStop mode.
           Wake up from system Run can come from the different scenario:
           - system went through a phase of system Stop (both CPU were in CStop, the other CPU woke-up first and set system in Run mode): toggle LED during 2s
           - system remained in Run mode (only this CPU was in CStop, the other CPU remained in Run mode): toggle LED during 1s
        */
        
        if(__HAL_PWR_GET_FLAG(PWR_FLAG_STOP2) != 0)
        {
          /* During time where this CPU was in CStop mode, system went through a phase of system Stop */
          LED_toggle(LED_TOGGLE_FREQ_FAST, 2);
        }
        else
        {
          /* During time where this CPU was in CStop mode, system remained in Run mode */
          LED_toggle(LED_TOGGLE_FREQ_FAST,1);
        }
        
        /* Clear Stop2 flag of CPU1 */
        __HAL_PWR_CLEAR_FLAG(PWR_FLAG_STOP2);
      }
      
      button_event = 0;
    }
    
    /* Toggle LED */
    BSP_LED_Toggle(LED1);
    HAL_Delay(1000 / LED_TOGGLE_FREQ_SLOW); /* Toggle delay (value 1000 for conversion from Hz to ms) */

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 6;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
  * @brief  Toggle LED at specified frequency during specified duration
  * @param  toggle_frequency_hz LED toggle frequency (unit: Hz)
  * @param  toggle_duration_s LED toggle duration  (unit: s)
  * @retval None
  */
void LED_toggle(uint32_t toggle_frequency_hz, uint32_t toggle_duration_s)
{
  uint32_t period_ms = (1000 / toggle_frequency_hz); /* Conversion from Hz to ms */
  uint32_t wait_index = (toggle_frequency_hz * toggle_duration_s);
  
  while(wait_index-- != 0)
  {
    /* Toggle LED */
    BSP_LED_Toggle(LED1);
    HAL_Delay(period_ms);
  }
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == BUTTON_SW1_PIN)
  {
    button_event = 1;
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  
  /* Turn on LED */
  BSP_LED_On(LED3);
  
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
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
