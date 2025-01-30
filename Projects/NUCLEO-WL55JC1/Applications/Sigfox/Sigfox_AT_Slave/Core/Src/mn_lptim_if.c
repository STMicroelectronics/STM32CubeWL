/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mn_lptim_if.c
  * @author  MCD Application Team
  * @brief   Interface between Sigfox monarch and lptim
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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
#include "mn_lptim_if.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/**
  * @brief LPTIM handle
  */
extern LPTIM_HandleTypeDef hlptim1;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
void MN_LPTIM_IF_Init(void)
{
  /* USER CODE BEGIN MN_LPTIM_IF_Init_0 */

  /* USER CODE END MN_LPTIM_IF_Init_0 */

  /* Force the LPTIM1 Peripheral Clock Reset */
  __HAL_RCC_LPTIM1_FORCE_RESET();
  /* Release the LPTIM1 Peripheral Clock Reset */
  __HAL_RCC_LPTIM1_RELEASE_RESET();

  /* USER CODE BEGIN MN_LPTIM_IF_Init_1 */

  /* USER CODE END MN_LPTIM_IF_Init_1 */

  MX_LPTIM1_Init();

  /* USER CODE BEGIN MN_LPTIM_IF_Init_2 */

  /* USER CODE END MN_LPTIM_IF_Init_2 */

  /* w.a.: LL_EXTI_LINE_X should be enabled in HAL_LPTIM_MspInit */
  LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_29);

  /* USER CODE BEGIN MN_LPTIM_IF_Init_3 */

  /* USER CODE END MN_LPTIM_IF_Init_3 */
}

void MN_LPTIM_IF_DeInit(void)
{
  RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct;

  /* USER CODE BEGIN MN_LPTIM_IF_DeInit_0 */

  /* USER CODE END MN_LPTIM_IF_DeInit_0 */

  if (HAL_LPTIM_DeInit(&hlptim1) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN MN_LPTIM_IF_DeInit_1 */

  /* USER CODE END MN_LPTIM_IF_DeInit_1 */

  /* Select the PCLK clock as LPTIM1 peripheral clock */
  RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LPTIM1;
  RCC_PeriphCLKInitStruct.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_PCLK1;
  HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

  /* USER CODE BEGIN MN_LPTIM_IF_DeInit_2 */

  /* USER CODE END MN_LPTIM_IF_DeInit_2 */

  /* Force the LPTIM1 Peripheral Clock Reset */
  __HAL_RCC_LPTIM1_FORCE_RESET();

  /* Release the LPTIM1 Peripheral Clock Reset */
  __HAL_RCC_LPTIM1_RELEASE_RESET();

  /* USER CODE BEGIN MN_LPTIM_IF_DeInit_3 */

  /* USER CODE END MN_LPTIM_IF_DeInit_3 */

}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
