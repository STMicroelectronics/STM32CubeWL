/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32WLxx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
BSP_DemoTypeDef  BSP_examples[] =
{
  {Led_demo, "Led Demo", 1},
  {Flash_demo, "External Flash demo", 2},
  {Temperature_demo, "Temperature demo", 3},
  {Pressure_demo, "Pressure demo", 4},
  {Gyro_demo, "Gyroscope Demo", 5},
  {Accelero_demo, "Accelerometer Demo", 6},
  {Magneto_demo, "Magneto Demo", 7},
  {EEPROM_demo, "EEPROM demo", 8 },
  {Radio_demo, "Radio demo", 9 },
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t DemoIndex = 0;
__IO uint32_t ButtonState = 0;
__IO uint8_t NbLoop = 1;
/* Exported variables --------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
extern void initialise_monitor_handles(void);
#endif 

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
  initialise_monitor_handles();
  printf("Semihosting Test...\n\r"); 
#endif
#if (USE_VCP_CONNECTION == 1)
  COM_InitTypeDef COM_Init;
#endif /* USE_VCP_CONNECTION */
  /* USER CODE END 1 */
  /* STM32WLxx HAL library initialization:
    - Configure the Flash prefetch, Flash preread and Buffer caches
    - Systick timer is configured by default as source of time base, but user
      can eventually implement his proper time base source (a general purpose
      timer for example or other time source), keeping in mind that Time base
      duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
      handled in milliseconds basis.
    - Low Level Initialization
  */
  HAL_Init();

  /* Configure the System clock to have a frequency of 48 MHz */
  SystemClock_Config();

  HAL_RCC_MCOConfig(RCC_MCO1_PA8, LL_RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_4);

  /* Configure the User Button 1 in EXTI Mode */
  BSP_PB_Init(BUTTON_USER1, BUTTON_MODE_EXTI);

  BSP_LED_Init(LED_BLUE);

#if (USE_VCP_CONNECTION == 1)
  /* Configure COM port */
  COM_Init.BaudRate   = 115200;
  COM_Init.WordLength = COM_WORDLENGTH_8B;
  COM_Init.StopBits   = COM_STOPBITS_1;
  COM_Init.Parity     = COM_PARITY_NONE;
  COM_Init.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &COM_Init) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  if (BSP_COM_SelectLogPort(COM1) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_VCP_CONNECTION */
  
  while (1)
  {
    if (ButtonState == 1)
    {
      HAL_Delay(400);
      ButtonState = 0;
      printf("=== Run demo: %s\n", BSP_examples[DemoIndex].DemoName);
      BSP_examples[DemoIndex].DemoFunc();
      printf("=== End of demo: %s\n", BSP_examples[DemoIndex].DemoName);

      HAL_Delay(100);
      
      DemoIndex++;
      if (DemoIndex >= COUNT_OF_EXAMPLE(BSP_examples))
      {
        NbLoop++;
        DemoIndex = 0;
      }

    }
    BSP_LED_Toggle(LED_BLUE);
    HAL_Delay(50);
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Configure the main internal regulator output voltage */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Initializes the CPU, AHB and APB busses clocks */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = LL_RCC_MSIRANGE_11; /* MSI = 48 MHz */
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3 | RCC_CLOCKTYPE_HCLK
                                | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1
                                | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;


  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  Gets numeric values from the Hyperterminal.
  * @param  Maximum value allowed (value)
  * @retval The character received
  */
uint32_t Serial_Scanf(uint32_t value)
{
  uint16_t tmp = 0;

  tmp = getchar();
  while (getchar() != '\n');

  if (tmp > value)
  {
    printf("\n\r  !!! Please enter valid number between 0 and %u \n", value);
    return 0xFF;
  }
  return tmp;
}

/**
  * @brief  Check for user input
  * @param  None
  * @retval Input state (1 : active / 0 : Inactive)
  */
uint8_t CheckForUserInput(void)
{
  return ButtonState;
}

/**
  * @brief  Button Callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None
  */
void BSP_PB_Callback(Button_TypeDef Button)
{
  if (Button == BUTTON_USER1)
  {
    ButtonState = 1;
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* Init the LED "Red" */
  BSP_LED_Init(LED_RED);
  while (1)
  {
    HAL_Delay(100);
    BSP_LED_Toggle(LED_RED);
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */
