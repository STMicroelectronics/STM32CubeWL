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

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

DMA_HandleTypeDef hdma_memtomem_dma1_channel1;
/* USER CODE BEGIN PV */
static const uint32_t aSRC_Const_Buffer[BUFFER_SIZE] =
{
  0x01020304, 0x05060708, 0x090A0B0C, 0x0D0E0F10,
  0x11121314, 0x15161718, 0x191A1B1C, 0x1D1E1F20,
  0x21222324, 0x25262728, 0x292A2B2C, 0x2D2E2F30,
  0x31323334, 0x35363738, 0x393A3B3C, 0x3D3E3F40,
  0x41424344, 0x45464748, 0x494A4B4C, 0x4D4E4F50,
  0x51525354, 0x55565758, 0x595A5B5C, 0x5D5E5F60,
  0x61626364, 0x65666768, 0x696A6B6C, 0x6D6E6F70,
  0x71727374, 0x75767778, 0x797A7B7C, 0x7D7E7F80
};

#define UNSECURE_SRAM1_BUFFER_ADDRESS  0x20003000
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_DMA_Init(void);
static void MX_GTZC_S_Init(void);
/* USER CODE BEGIN PFP */
void Cpu2ReadyWarnCpu1(void);
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

  /* GTZC initialisation */
  MX_GTZC_S_Init();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_DMA_Init();
  /* USER CODE BEGIN 2 */
  /* Configure LED1 (Blue) */
  BSP_LED_Init(LED1);
  /* Configure LED3 (Red)*/
  BSP_LED_Init(LED3);

  /* Configure User push-button (B1) button */
  BSP_PB_Init(BUTTON_SW1, BUTTON_MODE_GPIO);

  /* Wait for User push-button (B1) press before starting the Communication */
  while (BSP_PB_GetState(BUTTON_SW1) != GPIO_PIN_RESET)
  {
    BSP_LED_Toggle(LED1);
    HAL_Delay(100);
  }
  BSP_LED_Off(LED1);

  /* Configure the source, destination and buffer size DMA fields and Start DMA Channel/Stream transfer */
  if (HAL_DMA_Start(&hdma_memtomem_dma1_channel1, (uint32_t)&aSRC_Const_Buffer, UNSECURE_SRAM1_BUFFER_ADDRESS, BUFFER_SIZE*4) != HAL_OK)
  {
    /* Transfer Error */
    Error_Handler();
  }

  /* Polling for DMA end of transfer */
  if (HAL_DMA_PollForTransfer(&hdma_memtomem_dma1_channel1, HAL_DMA_FULL_TRANSFER, 100) != HAL_OK)
  {
    /* Transfer Error */
    Error_Handler();
  }

  /* Warn CPU1 that CPU2 is ready */
  Cpu2ReadyWarnCpu1();

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
  * @brief GTZC_S Initialization Function
  * @param None
  * @retval None
  */
static void MX_GTZC_S_Init(void)
{

  /* USER CODE BEGIN GTZC_S_Init 0 */

  /* USER CODE END GTZC_S_Init 0 */

  MPCWM_ConfigTypeDef MPCWM_NonPrivilegeArea_Desc = {0};

  /* USER CODE BEGIN GTZC_S_Init 1 */

  /* Note: GTZC interruptions default state is enable,
           this is why interruptions used in this example (DMA1, Flash, SRAM1)
           are not explicitly enabled.
  */

  /* USER CODE END GTZC_S_Init 1 */
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_DMA1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_FLASH) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_SRAM1) != HAL_OK)
  {
    Error_Handler();
  }
  MPCWM_NonPrivilegeArea_Desc.AreaId = GTZC_TZSC_MPCWM_AREAID_UNPRIV;
  MPCWM_NonPrivilegeArea_Desc.Length = 0x40000;
  if (HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(FLASH_BASE, &MPCWM_NonPrivilegeArea_Desc) != HAL_OK)
  {
    Error_Handler();
  }
  MPCWM_NonPrivilegeArea_Desc.AreaId = GTZC_TZSC_MPCWM_AREAID_UNPRIV_WRITABLE;
  if (HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(FLASH_BASE, &MPCWM_NonPrivilegeArea_Desc) != HAL_OK)
  {
    Error_Handler();
  }
  MPCWM_NonPrivilegeArea_Desc.AreaId = GTZC_TZSC_MPCWM_AREAID_UNPRIV;
  MPCWM_NonPrivilegeArea_Desc.Length = 0x8000;
  if (HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(SRAM1_BASE, &MPCWM_NonPrivilegeArea_Desc) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(SRAM2_BASE, &MPCWM_NonPrivilegeArea_Desc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN GTZC_S_Init 2 */

  /* USER CODE END GTZC_S_Init 2 */

}

/**
  * Enable DMA controller clock
  * Configure DMA for memory to memory transfers
  *   hdma_memtomem_dma1_channel1
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* Configure DMA request hdma_memtomem_dma1_channel1 on DMA1_Channel1 */
  hdma_memtomem_dma1_channel1.Instance = DMA1_Channel1;
  hdma_memtomem_dma1_channel1.Init.Request = DMA_REQUEST_MEM2MEM;
  hdma_memtomem_dma1_channel1.Init.Direction = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem_dma1_channel1.Init.PeriphInc = DMA_PINC_ENABLE;
  hdma_memtomem_dma1_channel1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_memtomem_dma1_channel1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_memtomem_dma1_channel1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_memtomem_dma1_channel1.Init.Mode = DMA_NORMAL;
  hdma_memtomem_dma1_channel1.Init.Priority = DMA_PRIORITY_LOW;
  if (HAL_DMA_Init(&hdma_memtomem_dma1_channel1) != HAL_OK)
  {
    Error_Handler( );
  }

  /*  */
  if (HAL_DMA_ConfigChannelAttributes(&hdma_memtomem_dma1_channel1, DMA_CHANNEL_NPRIV) != HAL_OK)
  {
    Error_Handler( );
  }

  /*  */
  if (HAL_DMA_ConfigChannelAttributes(&hdma_memtomem_dma1_channel1, DMA_CHANNEL_SEC) != HAL_OK)
  {
    Error_Handler( );
  }

  /*  */
  if (HAL_DMA_ConfigChannelAttributes(&hdma_memtomem_dma1_channel1, DMA_CHANNEL_SRC_SEC) != HAL_OK)
  {
    Error_Handler( );
  }

  /*  */
  if (HAL_DMA_ConfigChannelAttributes(&hdma_memtomem_dma1_channel1, DMA_CHANNEL_DEST_NSEC) != HAL_OK)
  {
    Error_Handler( );
  }

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Warn CPU1 that CPU2 is ready by simple Set Event instruction
  * @param  None
  * @retval None
  */
void Cpu2ReadyWarnCpu1(void)
{
  /* Set Cpu Event then consume it to allow next low power entry */
  __SEV();
  __WFE();
}

/**
  * @brief  GTZC TZIC Illegal Access callback.
  * @param  PeriphId Peripheral identifier.
  *         This parameter can be a value of @ref GTZC_Peripheral_Identification
  * @retval None
  */
void HAL_GTZC_TZIC_Callback(uint32_t PeriphId)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(PeriphId);

  Error_Handler();
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
  __disable_irq();
  while (1)
  {
    BSP_LED_On(LED3);
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
