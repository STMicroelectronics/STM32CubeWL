/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    PWR_Standby_DualCore/CM0PLUS/Src/main.c
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
#include "system_clock.h"
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
uint32_t button_event;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
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

  /* USER CODE BEGIN SysInit */
  
  /* Configure the system clock */
  /* Note: Configure clock only if not already done by the other CPU
           at wake up from Standby mode. */
  /* Note: Variable "SystemCoreClock" has been updated in HAL_init()
	   to match effective clock configuration */
  if(SystemCoreClock != 48000000)
  {
    if(SystemClock_Config_Common() != HAL_OK)
    {
      Error_Handler();
    }
  }
  
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
  /* Configure LED */
  BSP_LED_Init(LED2);
  
  /* Configure User push-button (B2) */
  BSP_PB_Init(BUTTON_SW2, BUTTON_MODE_EXTI);
  
  /* Configure wake up pin for Standby mode */
  /* Note: Required only at power-on-reset, not wake-up from Standby mode (configuration retained) */
  if(__HAL_PWR_GET_FLAG(PWR_FLAG_C2SB) == 0)
  {
    /* Disable wake up pin */
    HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);
    
    /* Clear wake up pin flag */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
    
    /* Enable GPIO pull-up state in Standby and Shutdown modes */
    HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_C, PWR_GPIO_BIT_13);
    HAL_PWREx_EnablePullUpPullDownConfig();
    
    /* Enable wake up pin */
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_LOW);
  }
  
  /* Clear wake up pin flag */
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
  
  /* Check system state at wake up */
  /* Note: Flag PWR_FLAG_SB is cleared by program of CPU1 */
  if(   (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != 0)
     && (__HAL_PWR_GET_FLAG(PWR_FLAG_C2SB) != 0)
    )
  {
    /* Clear Standby flag of CPU2 */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_C2SB);
    
    /* Wake up from system Standby (the other CPU is still in Standby or Shutdown mode): toggle LED during 4s */
    LED_toggle(LED_TOGGLE_FREQ_FAST, 4);
  }
  else
  {
    /* Clear Standby flag of CPU2 */
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_C2SB);
    
    /* Wake up from system reset or Run after a phase of system Standby (the other CPU woke-up first and is in Run mode): toggle LED during 2s */
    LED_toggle(LED_TOGGLE_FREQ_FAST, 2);
  }
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(button_event == 1)
    {
      /* Turn LED off */
      BSP_LED_Off(LED2);
      
      /* Clear wake up pin flag */
      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF2);
      
      /* Enter Standby mode */
      HAL_PWR_EnterSTANDBYMode();
      
      /* Wake up from system Run (the other CPU is in Run mode (and remained in Run mode during this CPU Standby phase)): toggle LED during 1s */
      LED_toggle(LED_TOGGLE_FREQ_FAST, 1);
      
      button_event = 0;
    }
    
    /* Toggle LED */
    BSP_LED_Toggle(LED2);
    HAL_Delay(1000 / LED_TOGGLE_FREQ_SLOW); /* Toggle delay (value 1000 for conversion from Hz to ms) */
    
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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

/**
  * @brief EXTI line detection callbacks
  * @param GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == BUTTON_SW2_PIN)
  {
    button_event = 1;
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
