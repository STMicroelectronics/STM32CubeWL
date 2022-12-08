/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define RADIO_MODE_STANDBY_RC        0x02
#define RADIO_MODE_TX                0x06
#define RADIO_COMMAND_TX_DONE        0x06

#define RADIO_MODE_BITFIELD          0x70
#define RADIO_STATUS_BITFIELD        0x0E
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SUBGHZ_HandleTypeDef hsubghz;

/* USER CODE BEGIN PV */
uint8_t RadioCmd[3] = {0x00, 0x00, 0x00};
uint8_t RadioResult = 0x00;
uint8_t RadioParam  = 0x00;
uint8_t RadioMode   = 0x00;
uint8_t RadioStatus = 0x00;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_SUBGHZ_Init(void);
/* USER CODE BEGIN PFP */

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

  /* Configure LED2 & LED3 */
  BSP_LED_Init(LED2);
  BSP_LED_Init(LED3);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_SUBGHZ_Init();
  /* USER CODE BEGIN 2 */

  /*## 1 - Wakeup the SUBGHZ Radio ###########################################*/
  /* Set Sleep Mode */
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_SLEEP, &RadioParam, 1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Set Standby Mode */
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_STANDBY, &RadioParam, 1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Retrieve Status from SUBGHZ Radio */
  if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &RadioResult, 1) != HAL_OK)
  {
    Error_Handler();
  }
  else
  {
    /* Format Mode and Status receive from SUBGHZ Radio */
    RadioMode   = ((RadioResult & RADIO_MODE_BITFIELD) >> 4); 
    
    /* Check if SUBGHZ Radio is in RADIO_MODE_STANDBY_RC mode */
    if(RadioMode != RADIO_MODE_STANDBY_RC)
    {
      Error_Handler();
    }
  }

  /*## 2 - Set a TX on SUBGHZ Radio side #####################################*/
  /* Set Tx Mode. RadioCmd = 0x00 Timeout deactivated */
  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TX, RadioCmd, 3) != HAL_OK)
  {
    Error_Handler();
  }

  /*## 3 - Get TX status from SUBGHZ Radio side ##############################*/
  /* Check that TX is well ongoing (RADIO_MODE_TX), wait end of transfer */

  /* Reset RadioResult */
  RadioResult = 0x00;

   /* Retrieve Status from SUBGHZ Radio */
  if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &RadioResult, 1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Format Mode and Status receive from SUBGHZ Radio */
  RadioMode   = ((RadioResult & RADIO_MODE_BITFIELD) >> 4);
  RadioStatus = ((RadioResult & RADIO_STATUS_BITFIELD) >> 1);

  if (RadioMode == RADIO_MODE_TX)
  {
    /* Wait end of transfer. SUBGHZ Radio go in Standby Mode */
    do
    {
      /* Reset RadioResult */
      RadioResult = 0x00;

      /* Retrieve Status from SUBGHZ Radio */
      if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_STATUS, &RadioResult, 1) != HAL_OK)
      {
        Error_Handler();
      }

      /* Format Mode and Status receive from SUBGHZ Radio */
      RadioMode   = ((RadioResult & RADIO_MODE_BITFIELD) >> 4); 
      RadioStatus = ((RadioResult & RADIO_STATUS_BITFIELD) >> 1);
    }
    while (RadioMode != RADIO_MODE_STANDBY_RC);
  }
  else
  {
    /* Call Error Handler; LED1 blinking */
    Error_Handler();
  }

  /* Check if TX is well done  (SUBGHZ Radio already in Standby mode) */
  if (RadioStatus == RADIO_COMMAND_TX_DONE)
  {
    /* Turn LED2 on */
    BSP_LED_On(LED2);
  }
  else
  {
    /* Call Error Handler; LED1 blinking */
    Error_Handler();
  }
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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

/* USER CODE BEGIN 4 */

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
    BSP_LED_Toggle(LED3);
    HAL_Delay(500);
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
