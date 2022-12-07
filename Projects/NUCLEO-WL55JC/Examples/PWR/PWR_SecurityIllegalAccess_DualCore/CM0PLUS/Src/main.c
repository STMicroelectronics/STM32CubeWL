/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PWR/PWR_SecurityIllegalAccess_DualCore/CM0PLUS/Src/main.c
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
#define LED_TOGGLE_FREQ_FAST            ( 10 * 2) /* LED toggle frequency fast (10Hz). Note: Factor 2 to convert toggle frequency to signal frequency. */
#define LED_TOGGLE_FREQ_SLOW            (  1 * 2) /* LED toggle frequency slow ( 1Hz). Note: Factor 2 to convert toggle frequency to signal frequency. */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t gtzc_flag;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GTZC_S_Init(void);
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

  /* GTZC initialisation */
  MX_GTZC_S_Init();

  /* USER CODE BEGIN SysInit */
  
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  /* Configure LED */
  BSP_LED_Init(LED2);
  
  /* Check CPU2 boot source */
  if(__HAL_PWR_GET_FLAG(PWR_FLAG_C2BOOTS) != 0UL)
  {
    /* CPU2 boot from normal boot request, by CPU1:
       Intermediate phase during setup of this example.
       - Go to error handler to not continue this example in current configuration
       - Next step: Activate system security (action to be done externally)
    */
    /* Enable CPU2 wake-up from low-power mode on illegal access occurrence */
    /* Note: Default state after reset, set here for demonstration purpose */
    HAL_PWREx_EnableWakeUp_ILAC();
    
   /* Next: Go to error handler below */
  }
  else
  {
    /* CPU2 boot from illegal access, by GTZC peripheral */
    
    HAL_GTZC_TZIC_GetFlag(GTZC_PERIPH_TZIC, &gtzc_flag);
    if(gtzc_flag != 0UL)
    {
      /* Expected state in this example: illegal access to GTZC peripheral */
      
      /* Clear flag */
      HAL_GTZC_TZIC_ClearFlag(GTZC_PERIPH_TZIC);

      /* Toggle LED during 2s */
      LED_toggle(LED_TOGGLE_FREQ_FAST, 2);
      
      /* Enter Stop mode */
      HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
      
      /* Case of CPU2 boot from illegal access: CPU2 entering in DeepSleep mode
         makes CPU2 exit from illegal access mode and set back in reset state.
         Therefore, program should not reach this step (if it is the case,
         go to error handler below). */
    }
    else
    {
      /* Unexpected state in this example: illegal access to another peripheral.
         Go to error handler below. */
    }
  }
  
  /* Go to error handler */
  Error_Handler();
  
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
           this is why GTZC interruptions used in this example (TZIC)
           is not explicitly enabled.
  */

  /* USER CODE END GTZC_S_Init 1 */
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
    BSP_LED_Toggle(LED2);
    HAL_Delay(period_ms);
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
  
  uint32_t led_toggle_period_ms = (1000 / LED_TOGGLE_FREQ_SLOW); /* Toggle delay (value 1000 for conversion from Hz to ms) */
  
  while(1)
  {
    /* Toggle LED */
    BSP_LED_Toggle(LED2);
    HAL_Delay(led_toggle_period_ms);
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
