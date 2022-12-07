/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    GTZC_GlobalSecurityConfiguration_DualCore/CM0PLUS/Src/main.c
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
#if defined ( __ARMCC_VERSION )
  extern uint32_t Image$$ER_IROM1$$Length;
  extern uint32_t Image$$RW_IRAM1$$Length;
  extern uint32_t Image$$RW_IRAM2$$Length;
  extern uint32_t Image$$ER_IROM1$$Base;
  extern uint32_t Image$$RW_IRAM1$$Base;
  extern uint32_t Image$$RW_IRAM2$$Base;
#elif defined ( __ICCARM__ )
  extern uint32_t __region_ROM_cpu2_start__;
  extern uint32_t __region_ROM_cpu2_end__;
  extern uint32_t __region_SRAM1_cpu2_start__;
  extern uint32_t __region_SRAM1_cpu2_end__;
  extern uint32_t __region_SRAM2_cpu2_start__;
  extern uint32_t __region_SRAM2_cpu2_end__;
#elif defined ( __GNUC__ )
  extern uint32_t _ROM2_CPU2_base;
  extern uint32_t _ROM2_CPU2_length;
  extern uint32_t _SRAM1_CPU2_base;
  extern uint32_t _SRAM1_CPU2_length;
  extern uint32_t _SRAM2_CPU2_base;
  extern uint32_t _SRAM2_CPU2_length;
#endif
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_DMA_Init(void);
static void MX_GPIO_Init(void);
static void MX_GTZC_Init(void);
/* USER CODE BEGIN PFP */
static void SystemSecure_Config(void);
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
  /* Get reset reason */
  uint32_t tmpcsr = RCC->CSR;

  /* Clear reset flags */
  RCC->CSR |= RCC_CSR_RMVF;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* At Power On reset, configure security option bytes. We won't return from there */
  if((tmpcsr & RCC_CSR_BORRSTF) != 0x00u)
  {
    SystemSecure_Config();
  }
  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GTZC_Init();
  MX_DMA_Init();
  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Toggle LED1 */
    BSP_LED_Toggle(LED1);

    /* Low frequency toggle */
    HAL_Delay(500);

    /* USER CODE END WHILE */
  
    /* USER CODE END 3 */
  }
  /* USER CODE BEGIN 3 */
}


/* USER CODE BEGIN 4 */
/**
  * @brief  System Secure configuration
  * @param  None
  * @retval None
  */
static void SystemSecure_Config(void)
{
  FLASH_OBProgramInitTypeDef optionbytes;

  /* Unlock flash */
  if(HAL_FLASH_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  /* Unlock Option byte access */
  if(HAL_FLASH_OB_Unlock() != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure security option bytes : overal security, CPU2 boot vector address & SPI SubGhz security enable */
  optionbytes.OptionType = (OPTIONBYTE_C2_BOOT_VECT | OPTIONBYTE_SECURE_MODE | OPTIONBYTE_SUBGHZSPI_SECURE_ACCESS);
  optionbytes.SecureMode = (OB_SECURE_SYSTEM_AND_FLASH_ENABLE | OB_SECURE_SRAM1_ENABLE | OB_SECURE_SRAM2_ENABLE);
  optionbytes.SUBGHZSPISecureAccess = OB_SUBGHZSPI_SECURE_ACCESS_ENABLE;
  optionbytes.C2BootRegion = OB_C2_BOOT_FROM_FLASH;

#if defined ( __ARMCC_VERSION )
  optionbytes.SecureFlashStartAddr = (uint32_t)&Image$$ER_IROM1$$Base;
  optionbytes.SecureSRAM1StartAddr = (uint32_t)&Image$$RW_IRAM1$$Base;
  optionbytes.SecureSRAM2StartAddr = (uint32_t)&Image$$RW_IRAM2$$Base;
  optionbytes.C2SecureBootVectAddr = (uint32_t)&Image$$ER_IROM1$$Base;

#elif defined ( __ICCARM__ )
  optionbytes.SecureFlashStartAddr = (uint32_t)&__region_ROM_cpu2_start__;
  optionbytes.SecureSRAM1StartAddr = (uint32_t)&__region_SRAM1_cpu2_start__;
  optionbytes.SecureSRAM2StartAddr = (uint32_t)&__region_SRAM2_cpu2_start__;
  optionbytes.C2SecureBootVectAddr = (uint32_t)&__region_ROM_cpu2_start__;
  
#elif defined ( __GNUC__  )
  optionbytes.SecureFlashStartAddr = (uint32_t)&_ROM2_CPU2_base;
  optionbytes.SecureSRAM1StartAddr = (uint32_t)&_SRAM1_CPU2_base;
  optionbytes.SecureSRAM2StartAddr = (uint32_t)&_SRAM2_CPU2_base;
  optionbytes.C2SecureBootVectAddr = (uint32_t)&_ROM2_CPU2_base;
  
#endif

  if(HAL_FLASHEx_OBProgram(&optionbytes) != HAL_OK)
  {
    Error_Handler();
  }

  /* Perform option byte reset to reload them */
  HAL_FLASH_OB_Launch();

  /* We should not reach here */
  while(1);
}

/* USER CODE END 4 */


/**
  * @brief  Configure DMA
  * @param  None
  * @retval None
  */
static void MX_DMA_Init(void)
{
  DMA_Channel_TypeDef *dmachannel[14] =  { DMA1_Channel1, DMA1_Channel2, DMA1_Channel3,
                                          DMA1_Channel4, DMA1_Channel5, DMA1_Channel6,
                                          DMA1_Channel7, DMA2_Channel1, DMA2_Channel2,
                                          DMA2_Channel3, DMA2_Channel4, DMA2_Channel5,
                                          DMA2_Channel6, DMA2_Channel7};
  DMA_HandleTypeDef dma;
  uint8_t channelnb;

  /* Enable DMA RCC clock */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* Secure et privilege all DMA channel */
  for(channelnb = 0; channelnb < 14; channelnb++)
  {
    dma.Instance = dmachannel[channelnb];
    HAL_DMA_ConfigChannelAttributes(&dma, (DMA_CHANNEL_PRIV | DMA_CHANNEL_SEC));
  }
}


/**
  * @brief  Configure GPIO
  * @param  None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* Enable GPIOB RCC clock */
  __HAL_RCC_GPIOB_CLK_ENABLE();
}


/**
  * @brief  Configure GTZC
  * @param  None
  * @retval None
  */
static void MX_GTZC_Init(void)
{
  MPCWM_ConfigTypeDef mpcwm;

  /* Enable all GTZC TZIC interrupts */
  HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_ALL);

  /* Enable Illegal Access interrupt */
  HAL_NVIC_EnableIRQ(TZIC_ILA_IRQn);

  /* Secure and privilege all peripherals */
  HAL_GTZC_TZSC_ConfigPeriphAttributes(GTZC_PERIPH_ALL, (GTZC_TZSC_ATTRIBUTE_SEC | GTZC_TZSC_ATTRIBUTE_PRIV));

  /* Set privilege attribute to all area dedicated to CPU2 */
  mpcwm.AreaId = GTZC_TZSC_MPCWM_AREAID_UNPRIV;

#if defined ( __ARMCC_VERSION )
  mpcwm.Length = (uint32_t) &Image$$ER_IROM1$$Length;
  HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(FLASH_BASE, &mpcwm);

  mpcwm.Length = (uint32_t) &Image$$RW_IRAM1$$Length;
  HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(SRAM1_BASE, &mpcwm);

  mpcwm.Length = (uint32_t) &Image$$RW_IRAM2$$Length;
  HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(SRAM2_BASE, &mpcwm);

#elif defined ( __ICCARM__ )
  mpcwm.Length = ((uint32_t)&__region_ROM_cpu2_end__ - (uint32_t)&__region_ROM_cpu2_start__ + 1);
  HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(FLASH_BASE, &mpcwm);

  mpcwm.Length = ((uint32_t)&__region_SRAM1_cpu2_end__ - (uint32_t)&__region_SRAM1_cpu2_start__ + 1);
  HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(SRAM1_BASE, &mpcwm);

  mpcwm.Length = ((uint32_t)&__region_SRAM2_cpu2_end__ - (uint32_t)&__region_SRAM2_cpu2_start__ + 1);
  HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(SRAM2_BASE, &mpcwm);

#elif defined ( __GNUC__ )
  mpcwm.Length = (uint32_t) &_ROM2_CPU2_length;
  HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(FLASH_BASE, &mpcwm);

  mpcwm.Length = (uint32_t) &_SRAM1_CPU2_length;
  HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(SRAM1_BASE, &mpcwm);

  mpcwm.Length = (uint32_t) &_SRAM2_CPU2_length;
  HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(SRAM2_BASE, &mpcwm); 
  
#endif

  /* Lock configuration */
  HAL_GTZC_TZSC_Lock(GTZC_TZSC);
}


/**
  * @brief  GTZC TZIC Illegal Access callback.
  * @param  PeriphId Peripheral identifier.
  *         This parameter can be a value of @ref GTZC_Peripheral_Identification
  * @retval None
  */
void HAL_GTZC_TZIC_Callback(uint32_t PeriphId)
{
  /* User can add his own implementation to report Illegal Access */
  while(1)
  {
  }
}

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
    BSP_LED_On(LED1);
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
