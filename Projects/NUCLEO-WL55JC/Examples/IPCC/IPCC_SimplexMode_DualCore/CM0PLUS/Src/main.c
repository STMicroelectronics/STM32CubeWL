/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    IPCC/IPCC_SimplexMode_DualCore/CM0PLUS/Src/main.c
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

__IO uint32_t Simplex_Receive = 0;

/* A buffer not accessible to the remote processor */
char userBuffer[BUFFER_SIZE];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_IPCC_Init(void);
/* USER CODE BEGIN PFP */
void Simplex_Receive_callback(struct __IPCC_HandleTypeDef *hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir);
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
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* IPCC initialisation */
   MX_IPCC_Init();

  /* USER CODE BEGIN SysInit */

  /* Configure a user callback. In this example, it is named "Simplex_Receive_callback". */
  /* The callback is triggered when the remote cpu send a message. (IPCC_CHANNEL_DIR_RX) */
  err = HAL_IPCC_ActivateNotification(&hipcc, ChannelId, IPCC_CHANNEL_DIR_RX, Simplex_Receive_callback);
  if(HAL_OK != err)
  {
    Error_Handler();
  }
  
  /* Notify remote cpu that the initialisation is completed */
  *cpu2InitDone = CPU2_INITIALISED;

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* Loop until remote processor send a message containing the index 1000 */
  /* This index is extracted from the content of the message directly */
  while (1)
  {
    /* Wait for the Simplex_Receive_callback */
    while (Simplex_Receive != 1);
    Simplex_Receive = 0;

    /* Reset content of user buffer */
    memset((char*)userBuffer, '\0', BUFFER_SIZE);
    
    /* Write communication data to memory */
    memcpy ((char*)userBuffer, (char*)sharedBuffer, BUFFER_SIZE);
    
    /* Notify remote cpu that the transaction is completed */
    err = HAL_IPCC_NotifyCPU(&hipcc, ChannelId, IPCC_CHANNEL_DIR_RX);
    if(HAL_OK != err)
    {
      Error_Handler();
    }       
    
    /* Retrieve communication content */
    if( 1 != sscanf(userBuffer,"cpu1->cpu2:%04lu", (long unsigned int *) &index))
    {
      Error_Handler();
    }
    
    /* Display processor 2 activity */
    if(index%100 == 0) /* Add some delay to observe the Led blinking */
    {
      BSP_LED_Toggle(LED_GREEN);
    }
    
    /* If 1000 transactions have been done */
    if(index == 1000)
    {
      /* At the end of this example, set the Led On */
      BSP_LED_On(LED_GREEN);
    }
    
    /* USER CODE BEGIN 3 */

    /* USER CODE END 3 */
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
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
void Simplex_Receive_callback(struct __IPCC_HandleTypeDef *hipcc, uint32_t ChannelIndex, IPCC_CHANNELDirTypeDef ChannelDir)
{
  Simplex_Receive = 1;
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
