/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PWR/PWR_SMPS/Src/main.c
  * @author  MCD Application Team
  * @brief   This example describes how to configure and use SMPS through
  *          the STM32WLxx HAL API.
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

/* SMPS clock detection defines */
#define SUBGHZ_SMPSC0R           0x0916 /* SMPS control 0 register address */
#define SUBGHZ_SMPSC0R_CLKDE     0x40   /* SMPS control 0 register, bit SMPS clock detection enable */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SUBGHZ_HandleTypeDef hsubghz;

/* USER CODE BEGIN PV */
__IO uint8_t ubUserButtonEvent = 0;   /* Flag of user button set on interrupt handler */
uint32_t smps_requested_mode;         /* SMPS requested mode (value "PWR_SMPS_STEP_DOWN" for SMPS is in mode step-down (switching enable), value "PWR_SMPS_STEP_DOWN" SMPS is in mode bypass (switching disable) */
uint32_t smps_effective_mode;         /* SMPS requested mode (value "PWR_SMPS_STEP_DOWN" for SMPS is in mode step-down (switching enable), value "PWR_SMPS_STEP_DOWN" SMPS is in mode bypass (switching disable) */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SUBGHZ_Init(void);
/* USER CODE BEGIN PFP */
void SMPSClockDetectionEnable(void);
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
  /* STM32WLxx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user 
         can eventually implement his proper time base source (a general purpose 
         timer for example or other time source), keeping in mind that Time base 
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  
  /* Note: LED and user push-button are controlled by HAL BSP
           and initialized by CubeMX function MX_GPIO_Init.
  */
    
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SUBGHZ_Init();
  /* USER CODE BEGIN 2 */
  
  /* SMPS clock detection enable */
  SMPSClockDetectionEnable();
  
  /* Set SMPS operating mode */
  HAL_PWREx_SMPS_SetMode(PWR_SMPS_STEP_DOWN);
  smps_requested_mode = PWR_SMPS_STEP_DOWN;
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    /* Toggle LED */
    BSP_LED_Toggle(LED2);
    
    /* Get SMPS effective operating mode */
    smps_effective_mode = HAL_PWREx_SMPS_GetEffectiveMode();
    if(smps_effective_mode == PWR_SMPS_STEP_DOWN)
    {
      /* SMPS effective operating mode: step-down */
      /* Toggle LED quickly */
      HAL_Delay(50);
    }
    else
    {
      /* SMPS effective operating mode: bypass */
      /* Toggle LED slowly */
      HAL_Delay(500);
    }
    
    /* Toggle SMPS between step-down and bypass mode at each press on User push-button (B1) */
    if (ubUserButtonEvent == 1)
    {
      ubUserButtonEvent = 0;
      
      if (smps_requested_mode == PWR_SMPS_BYPASS)
      {
        HAL_PWREx_SMPS_SetMode(PWR_SMPS_STEP_DOWN);
        smps_requested_mode = PWR_SMPS_STEP_DOWN;
      }
      else
      {
        HAL_PWREx_SMPS_SetMode(PWR_SMPS_BYPASS);
        smps_requested_mode = PWR_SMPS_BYPASS;
      }
    }
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

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
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
  * @brief SUBGHZ Initialization Function
  * @param None
  * @retval None
  */
static void MX_SUBGHZ_Init(void)
{

  /* USER CODE BEGIN SUBGHZ_Init 0 */

  /* USER CODE END SUBGHZ_Init 0 */

  /* USER CODE BEGIN SUBGHZ_Init 1 */

  /* USER CODE END SUBGHZ_Init 1 */
  hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_8;
  if (HAL_SUBGHZ_Init(&hsubghz) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SUBGHZ_Init 2 */

  /* USER CODE END SUBGHZ_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED2_Pin */
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

}

/* USER CODE BEGIN 4 */

/**
  * @brief SMPS clock detection enable.
  * @note  SMPS clock detection is recommended to be enabled before enable SMPS
  *        in switching mode. 
  *        In case of clock failure, it will automaticcaly switch off SMPS
  *        (refer to reference manual).
  * @note  SMPS clock detection is controlled through HAL SUBGHZ peripheral,
  *        due to SMPS and radio related to the same HSE clock.
  * @param None
  * @retval None
  */
void SMPSClockDetectionEnable(void)
{
  uint8_t radio_register_data;
  uint8_t radio_register_data_readback;
  uint8_t radio_command;
  
  /* Enable SMPS clock detection through HAL SUBGHZ peripheral */
  HAL_SUBGHZ_ReadRegister(&hsubghz, SUBGHZ_SMPSC0R, &radio_register_data);
  radio_register_data |= SUBGHZ_SMPSC0R_CLKDE;
  HAL_SUBGHZ_WriteRegister(&hsubghz, SUBGHZ_SMPSC0R, radio_register_data);
  
  /* Check back data written */
  HAL_SUBGHZ_ReadRegister(&hsubghz, SUBGHZ_SMPSC0R, &radio_register_data_readback);
  if(radio_register_data != radio_register_data_readback)
  {
    Error_Handler();
  }
  
  /* Set back radio in Sleep mode (optional) */
  /* Note: Once clock detection is enabled the radio can be put back in Sleep mode,
           clock detection remains active.
           SMPS can still be controlled through PWR registers */
  /* Note: Using radio middleware, this action can be done more easily 
           with function "RadioSleep()" */
  radio_command = 4; /* Command "Set_Sleep": warm startup and RTC wakeup disabled is value 4 */
  HAL_SUBGHZ_ExecSetCmd( &hsubghz, RADIO_SET_SLEEP, ( uint8_t* )&radio_command, 1 );
}

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
 if (GPIO_Pin == BUTTON_SW1_PIN)
 {
   /* Set variable to report push button event to main program */
   ubUserButtonEvent = 1;
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
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
    HAL_GPIO_WritePin(LED2_GPIO_PORT, LED2_PIN, GPIO_PIN_SET);
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
