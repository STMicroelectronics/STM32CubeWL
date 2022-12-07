/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    GTZC_MemoryWatermarkProtection_DualCore/CM0PLUS/Src/main.c
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
/* SRAM2 secured privileged area start address is half of SRAM2 size */
#define SRAM2_SECURE_PRIVILEGE_ADDRESS      (SRAM2_BASE + (SRAM2_SIZE >> 1))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
__IO uint32_t KeyPress = 0x00u;
__IO uint32_t IlacIrq = 0x00u;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GTZC_S_Init(void);
/* USER CODE BEGIN PFP */
void SetCoreMode(uint32_t Mode);
void __SVC( void);
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
  __IO uint32_t *readpriv = (uint32_t *) SRAM2_SECURE_PRIVILEGE_ADDRESS;
  *readpriv = 0xDEADDEAD;
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
  /* USER CODE BEGIN 2 */
  /* Configure User Button SW1 */
  BSP_PB_Init(BUTTON_SW1, BUTTON_MODE_EXTI);

  /* Enable LED1 GPIO port clock for CPU2 */
  /* Note: GPIO of LED1 has been initialized by CPU1. In case of example
           customization with CPU1 entering in low-power mode Stop, the
           corresponding GPIO bus is enabled for CPU2. */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* Switch to unprivileged mode */
  /* Wait for user event */
  while(KeyPress != 0x01)
  {
    /* Slow toggling */
    BSP_LED_Toggle(LED1);
    HAL_Delay(500);
  }

  /* Set core mode to unprivilege */
  SetCoreMode(THREAD_MODE_UNPRIVILEGED);

  /* Try to read SRAM2 first address that has been privilege watermarked */
  if(*readpriv == 0xDEADDEAD)
  {
    /* Go to Error Handler */
    Error_Handler();
  }

  /* Verify Illegal Access Interrupt occurred */
  if(IlacIrq != 0x01u)
  {
    /* Go to Error Handler */
    Error_Handler();
  }

  /* Reset Ilac global variable */
  IlacIrq = 0x00u;

  /* Set core back to privilege mode */
  SetCoreMode(THREAD_MODE_PRIVILEGED);

  /* With Privilege rights, Try to access SRAM2 first address again */
  if(*readpriv != 0xDEADDEAD)
  {
    /* Go to Error Handler */
    Error_Handler();
  }

  /* Verify Illegal Access Interrupt occurred */
  if(IlacIrq != 0x00u)
  {
    /* Go to Error Handler */
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Toggle LED1 as example successful */
    BSP_LED_Toggle(LED1);

    /* Fast toggling */
    HAL_Delay(100);

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

  /* USER CODE END GTZC_S_Init 1 */
  if (HAL_GTZC_TZIC_EnableIT(GTZC_PERIPH_SRAM2) != HAL_OK)
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
  MPCWM_NonPrivilegeArea_Desc.Length = 0x4000;
  if (HAL_GTZC_TZSC_MPCWM_ConfigMemAttributes(SRAM2_BASE, &MPCWM_NonPrivilegeArea_Desc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN GTZC_S_Init 2 */

  /* USER CODE END GTZC_S_Init 2 */

}

/* USER CODE BEGIN 4 */

/**
  * @brief  Set core to privilege / unprivilege
  * @param  Mode Core mode that can be privileged or unprivileged.
  * @retval None
  */
void SetCoreMode(uint32_t Mode)
{
  if(Mode != THREAD_MODE_UNPRIVILEGED)
  {
    __SVC();
  }
  else
  {
     /*Set Cortex in unprivilege thread mode, use MSP stack*/
    __set_CONTROL(THREAD_MODE_UNPRIVILEGED);
    __ISB();
  }
}

/**
  * @brief  API to call Service Call Exception
  * @param  Mode
  * @retval None
  */
void __SVC( void)
{
  __ASM("svc 0x01");
}


/**
  * @brief  GTZC TZIC Illegal Access callback.
  * @param  PeriphId Peripheral identifier.
  *         This parameter can be a value of @ref GTZC_Peripheral_Identification
  * @retval None
  */
void HAL_GTZC_TZIC_Callback(uint32_t PeriphId)
{
  /* If SRAM2 has been attacked, warn user it */
  if (PeriphId == GTZC_PERIPH_SRAM2)
  {
    IlacIrq = 0x01u;
  }
}

/**
  * @brief  Returns the selected Button state.
  * @param  Button: Specifies the Button to be checked.
  * @retval None
  */
void BSP_PB_Callback(Button_TypeDef Button)
{
  KeyPress = 0x01;
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
