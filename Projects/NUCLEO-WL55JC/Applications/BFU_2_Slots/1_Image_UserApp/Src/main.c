/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @brief   Main application file.
  *          This application demonstrates Firmware Update, protections
  *          and crypto testing functionalities.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "se_def.h"
#include "flash_if.h"
#include "se_interface_application.h"
#include "common.h"
#include "test_protections.h"
#include "fw_update_app.h"
#include "se_user_code.h"
#ifdef KMS_ENABLED
#include "tkms.h"
#include "tkms_app.h"
#endif /* KMS_ENABLED */


/** @addtogroup USER_APP User App Example
  * @{
  */


/** @addtogroup USER_APP_COMMON Common
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USER_APP_NBLINKS  ((uint8_t) 1U)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t *pUserAppId;
const uint8_t UserAppId = 'A';
SE_ErrorStatus eRetStatus = SE_ERROR;
SE_StatusTypeDef eStatus;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void FW_APP_PrintMainMenu(void);
void FW_APP_Run(void);

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
  uint32_t i = 0U;
  /*  set example to const : this const changes in binary without rebuild */
  pUserAppId = (uint8_t *)&UserAppId;

  /* STM32WLxx HAL library initialization:
  - Configure the Flash prefetch
  - Systick timer is configured by default as source of time base, but user
  can eventually implement his proper time base source (a general purpose
  timer for example or other time source), keeping in mind that Time base
  duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
  handled in milliseconds basis.
  - Set NVIC Group Priority to 4
  - Low Level Initialization
  */
  HAL_Init();


  /* Configure the system clock */
  SystemClock_Config();

  /* Flash driver initialization*/
  FLASH_If_Init();

  /* Board BSP  Configuration-------------------------------------------------*/

  /* LED Init*/
  BSP_LED_Init(LED_GREEN);
  for (i = 0U; i < USER_APP_NBLINKS; i++)
  {
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(100U);
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(100U);
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(100U);
    BSP_LED_Toggle(LED_GREEN);
    HAL_Delay(100U);
  }

  /* If the SecureBoot configured the IWDG, UserApp must reload IWDG counter with value defined in the reload register*/
  WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);

  /* Configure Communication module */
  COM_Init();

  /* Configure button in EXTI mode */
  BUTTON_INIT_EXTI();

  printf("\r\n======================================================================");
  printf("\r\n=              (C) COPYRIGHT 2017 STMicroelectronics                 =");
  printf("\r\n=                                                                    =");
  printf("\r\n=                          User App #%c                               =", *pUserAppId);
  printf("\r\n======================================================================");
  printf("\r\n\r\n");


  /* User App firmware runs*/
  FW_APP_Run();

  while (1U)
  {}

}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 48000000
  *            HCLK(Hz)                       = 48000000
  *            HCLK1 Prescaler                = 1
  *            HCKL2 Prescaler                = 1
  *            HCKL3 Prescaler                = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 2 //not considered on FPGA as VCO output is fixed at 240Mhz
  *            PLL_N                          = 30 //not considered on FPGA as VCO output is fixed at 240Mhz
  *            PLL_N                          = 20 //Allow to fix VCO to 160Mhz, and reach Sysclk Max Frqe = 32MHz
  *                                                  with PLL_DIV5
  *            PLL_P                          = 5
  *            PLL_Q                          = 5
  *            PLL_R                          = 5
  *            Flash Latency(WS)              = 2
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  /* Activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI |  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSICalibrationValue = 70;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV5;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV5;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while (1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2\
                                 | RCC_CLOCKTYPE_HCLK2 | RCC_CLOCKTYPE_HCLK3);
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    /* Initialization Error */
    while (1);
  }
}


/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
void FW_APP_PrintMainMenu(void)
{
  printf("\r\n=================== Main Menu ============================\r\n\n");
  printf("  Download a new Fw Image ------------------------------- 1\r\n\n");
  printf("  Test Protections -------------------------------------- 2\r\n\n");
  printf("  Test SE User Code ------------------------------------- 3\r\n\n");
  printf("  Multiple download ------------------------------------- 4\r\n\n");
  printf("  Validate a FW Image------------------------------------ 5\r\n\n");
#ifdef KMS_ENABLED
  printf("  Test tKMS --------------------------------------------- a\r\n\n");
#endif /* KMS_ENABLED */
  printf("  Selection :\r\n\n");
}

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
void FW_APP_Run(void)
{
  uint8_t key = 0U;

  /* Print Main Menu message*/
  FW_APP_PrintMainMenu();

  while (1U)
  {
    /* If the SecureBoot configured the IWDG, UserApp must reload IWDG counter with value defined in the reload
       register */
    WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);

    /* Clean the input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      switch (key)
      {
        case '1' :
          FW_UPDATE_Run();
          break;
        case '2' :
          TEST_PROTECTIONS_RunMenu();
          break;
        case '3' :
          SE_USER_CODE_RunMenu();
          break;
        case '4' :
          FW_UPDATE_MULTIPLE_RunMenu();
          break;
        case '5' :
          FW_VALIDATE_RunMenu();
          break;
#ifdef KMS_ENABLED
        case 'a' :
          tkms_app_menu();
          break;
#endif /* KMS_ENABLED */
        default:
          printf("Invalid Number !\r");
          break;
      }

      /*Print Main Menu message*/
      FW_APP_PrintMainMenu();
    }

    BSP_LED_Toggle(LED_GREEN);
  }
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
  /* User can add his own implementation to report the file name and line number,
   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1U)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
