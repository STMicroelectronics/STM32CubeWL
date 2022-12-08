/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#define MAIN_C

#include "sfu_fwimg_regions.h"
#include "sfu_low_level_security.h"
#include "sfu_boot.h"
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
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
  uint32_t jump_address ;
  typedef void (*Function_Pointer)(void);
  Function_Pointer  p_jump_to_function;
  __IO uint32_t *pSbsfuFlag = (__IO uint32_t *)CM4_SBFU_BOOT_FLAG_ADDRESS;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* LED Init*/
  BSP_LED_Init(LED_RED);

  /* Reset the SRAM1 flag */
  *pSbsfuFlag = SBSFU_NOT_BOOTED;

  /* Configure the security features */
  if (SFU_BOOT_CheckApplySecurityProtections(SFU_INITIAL_CONFIGURATION) != SFU_SUCCESS)
  {
    SFU_EXCPT_Security_Error();
  }
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_RUNTIME_PROTECT);

  /* Boot CPU2 */
  HAL_PWREx_ReleaseCore(PWR_CORE_CPU2);

  /* Wait for the flag */
  while (*pSbsfuFlag == SBSFU_NOT_BOOTED)
  {
    BSP_LED_Toggle(LED_RED);
    HAL_Delay(500U);
  }
  BSP_LED_Off(LED_RED);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* Check the security features once CPU2 has booted */
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_RUNTIME_PROTECT);
  FLOW_CONTROL_INIT(uFlowProtectValue, FLOW_CTRL_INIT_VALUE);
  if (SFU_BOOT_CheckApplySecurityProtections(SFU_SECOND_CONFIGURATION) != SFU_SUCCESS)
  {
    SFU_EXCPT_Security_Error();
  }
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_RUNTIME_PROTECT);


  if (*pSbsfuFlag == SBSFU_BOOTED)
  {

    /* Check the security features before jumping in the user application CPU2 */
    FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_RUNTIME_PROTECT);
    FLOW_CONTROL_INIT(uFlowProtectValue, FLOW_CTRL_INIT_VALUE);
    if (SFU_BOOT_CheckApplySecurityProtections(SFU_THIRD_CONFIGURATION) != SFU_SUCCESS)
    {
      SFU_EXCPT_Security_Error();
    }
    FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_RUNTIME_PROTECT);

#if defined(SFU_MPU_USERAPP_ACTIVATION)
    SFU_LL_SECU_SetProtectionMPU_UserApp();
#else
    HAL_MPU_Disable();
#endif /* SFU_MPU_USERAPP_ACTIVATION */

    jump_address = *(__IO uint32_t *)((SLOT_ACTIVE_2_START + SFU_IMG_IMAGE_OFFSET + 4));
    /* Jump to user application */
    p_jump_to_function = (Function_Pointer) jump_address;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t *)(SLOT_ACTIVE_2_START + SFU_IMG_IMAGE_OFFSET));

    /* JUMP into User App */
    p_jump_to_function();
  }

  /* We shouldn't reach this point */
  SFU_EXCPT_Security_Error();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE BEGIN 3 */

    /* USER CODE END 3 */
  }
  /* USER CODE END WHILE */
}

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

/* USER CODE BEGIN 4 */

/**
  * @brief  Check (and Apply when possible) the security/safety/integrity protections.
  *         The "Apply" part depends on @ref SECBOOT_OB_DEV_MODE and @ref SFU_PROTECT_RDP_LEVEL.
  * @param  None
  * @note   This operation should be done as soon as possible after a reboot.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_BOOT_CheckApplySecurityProtections(uint8_t uStep)
{
  SFU_ErrorStatus e_ret_status;

  /* Apply Static protections involving Option Bytes */
  e_ret_status = SFU_LL_SECU_CheckApplyStaticProtections();
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_STATIC_PROTECT);
  if (e_ret_status == SFU_SUCCESS)
  {
    /* Apply runtime protections needed to be enabled after each Reset */
    e_ret_status = SFU_LL_SECU_CheckApplyRuntimeProtections(uStep);
  }
  FLOW_CONTROL_CHECK(uFlowProtectValue, FLOW_CTRL_RUNTIME_PROTECT);

  return e_ret_status;
}

/**
  * @brief  Stop in case of security error
  * @retval None
  */
void SFU_EXCPT_Security_Error(void)
{
  HAL_Delay(1000);
  /* This is the last operation executed. Force a System Reset. */
  NVIC_SystemReset();
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
  /* Security issue : execution stopped ! */
  SFU_EXCPT_Security_Error();
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
  /* Security issue : execution stopped ! */
  SFU_EXCPT_Security_Error();
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */