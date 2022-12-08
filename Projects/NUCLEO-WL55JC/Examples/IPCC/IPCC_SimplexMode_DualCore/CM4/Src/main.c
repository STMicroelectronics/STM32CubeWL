/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    IPCC/IPCC_SimplexMode_DualCore/CM4/Src/main.c
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
#include <stdio.h>
#include <string.h>
#include "stm32wlxx_nucleo.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
IPCC_HandleTypeDef hipcc;

/* USER CODE BEGIN PV */

/* A buffer is reserved inside the shared memory to exchange data between cpu.*/
/* by convention, in this example, the reserved space is 16 char from 0x2000FFF0 to 0x2000FFFF*/
#define BUFFER_SIZE 16
__IO char *sharedBuffer = (char *)0x2000FFF0;

/* By convention, the address 0x2000FFEC is reserved to indicate the state of the CPU2: Initialised or not */
#define CPU2_INITIALISED 0xAA
#define CPU2_NOT_INITIALISED 0xBB
__IO char *cpu2InitDone = (char *)0x2000FFEC;

__IO uint32_t Simplex_Send = 1;

IPCC_HandleTypeDef hipcc = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_IPCC_Init(void);
/* USER CODE BEGIN PFP */
void Simplex_Send_callback(struct __IPCC_HandleTypeDef *hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir);
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
  HAL_StatusTypeDef err = HAL_OK;
  uint32_t ChannelId = 0; /* This parameter can take any value between 0 and (IPCC_CHANNEL_NUMBER-1) */
  uint32_t index = 0; /* A counter to execute several times the communication process */

  /* Set remote cpu as not initialised */
  *cpu2InitDone = CPU2_NOT_INITIALISED;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* Initialize the LEDs (by convention, this is only done by CPU1) */
  BSP_LED_Init(LED_BLUE);
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* IPCC initialisation */
   MX_IPCC_Init();

  /* USER CODE BEGIN SysInit */
 
  /* Configure a user callback. In this example, it is named "Simplex_Send_callback". */
  /* The callback is triggered when the other cpu has finished to handle the message. (IPCC_CHANNEL_DIR_TX) */
  err = HAL_IPCC_ActivateNotification(&hipcc, ChannelId, IPCC_CHANNEL_DIR_TX, Simplex_Send_callback);
  if(HAL_OK != err)
  {
    Error_Handler();
  }

  /* Initialize the content of the sharedBuffer */
  memset((char*)sharedBuffer, '\0', BUFFER_SIZE);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  
  /* USER CODE END 2 */

  /* Boot CPU2 */
  HAL_PWREx_ReleaseCore(PWR_CORE_CPU2);

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  /* Wait until cpu2 is ready to receive commands */
  while (*cpu2InitDone != CPU2_INITIALISED);

  /* Execute several transmission */
  while (index <= 1000)
  {
    /* Wait until the transmission is completed */
    if(Simplex_Send == 1)
    {
      Simplex_Send = 0;
      
      /* Display processsor 1 activity */
      if(index%100 == 0) /* Add some delay to observe the Led blinking */
      {
        HAL_Delay(100);
        BSP_LED_Toggle(LED_BLUE);
      }
      
      /* Write communication data to memory */
      snprintf((char*)sharedBuffer, BUFFER_SIZE, "cpu1->cpu2:%04lu", (long unsigned int) index++);
      
      /* Notify remote cpu of the on-going transaction */
      err = HAL_IPCC_NotifyCPU(&hipcc, ChannelId, IPCC_CHANNEL_DIR_TX);
      if(HAL_OK != err)
      {
        Error_Handler();
      }    
    }
  }
  
  /* Remove the callback management. (No more interrupt are triggered on message reception) */
  err = HAL_IPCC_DeActivateNotification(&hipcc, ChannelId, IPCC_CHANNEL_DIR_TX);  
  if(HAL_OK != err)
  {
    Error_Handler();
  }
  
  /* At the end of this example, set the Led On */
  BSP_LED_On(LED_BLUE);

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the CPU, AHB and APB buses clocks
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

/**
  * @brief IPCC Initialization Function
  * @param None
  * @retval None
  */
static void MX_IPCC_Init(void)
{

  /* USER CODE BEGIN IPCC_Init 0 */

  /* USER CODE END IPCC_Init 0 */

  /* USER CODE BEGIN IPCC_Init 1 */

  /* USER CODE END IPCC_Init 1 */
  hipcc.Instance = IPCC;
  if (HAL_IPCC_Init(&hipcc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN IPCC_Init 2 */

  /* USER CODE END IPCC_Init 2 */

}

/* USER CODE BEGIN 4 */
void Simplex_Send_callback(struct __IPCC_HandleTypeDef *hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  Simplex_Send = 1;
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
  BSP_LED_On(LED_RED);
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
  BSP_LED_On(LED_RED);
  while (1)
  {
  }

  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
