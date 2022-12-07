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
#include <stdio.h>
#include <string.h>
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
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
void LED_On(void);
void LED_Toggle(void);
void LED_Blinking(uint32_t Period);
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

  uint32_t index = 0; /* A counter to execute several times the communication process */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  /* USER CODE BEGIN Init */

  /* Notify remote cpu that the initialisation is completed */
  *cpu2InitDone = CPU2_INITIALISED;
  
  /* Enable message reception */
  NVIC_EnableIRQ(IPCC_C2_RX_C2_TX_IRQn);
  LL_C2_IPCC_EnableIT_RXO(IPCC);
  LL_C2_IPCC_EnableIT_TXF(IPCC);
  LL_C2_IPCC_EnableReceiveChannel(IPCC, LL_IPCC_CHANNEL_1);
  
  /* USER CODE END Init */

  /* Update SystemCoreClock variable */
  SystemCoreClockUpdate();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
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
    LL_C2_IPCC_ClearFlag_CHx(IPCC, LL_IPCC_CHANNEL_1); 
    LL_C2_IPCC_EnableReceiveChannel(IPCC, LL_IPCC_CHANNEL_1);
    
    /* Retrieve communication content */
    if( 1 != sscanf(userBuffer,"cpu1->cpu2:%04lu", (long unsigned int *) &index))
    {
      Error_Handler();
    }
    
    /* Display processor 2 activity */
    if(index%100 == 0) /* Add some delay to observe the Led blinking */
    {
      LED_Toggle();
    }
    
    /* If 1000 transactions have been done */
    if(index == 1000)
    {
      /* At the end of this example, set the Led On */
      LED_On();
    }
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  LL_C2_AHB2_GRP1_EnableClock(LL_C2_AHB2_GRP1_PERIPH_GPIOB);

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
  * @brief  Turn-on LED2.
  * @param  None
  * @retval None
  */
void LED_On(void)
{
  /* Turn LED2 on */
  LL_GPIO_SetOutputPin(LED2_GPIO_Port, LED2_Pin); 
}

/**
  * @brief  Toggle LED2.
  * @param  None
  * @retval None
  */
void LED_Toggle(void)
{
  /* Turn LED2 on */
   LL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); 
}

/**
  * @brief  Set LED2 to Blinking mode for an infinite loop (toggle period based on value provided as input parameter).
  * @param  Period : Period of time (in ms) between each toggling of LED
  *   This parameter can be user defined values. Pre-defined values used in that example are :
  *     @arg LED_BLINK_FAST : Fast Blinking
  *     @arg LED_BLINK_SLOW : Slow Blinking
  *     @arg LED_BLINK_ERROR : Error specific Blinking
  * @retval None
  */
void LED_Blinking(uint32_t Period)
{
  /* Toggle LED2 in an infinite loop */
  while (1)
  {
    LL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin); 
    LL_mDelay(Period);
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
