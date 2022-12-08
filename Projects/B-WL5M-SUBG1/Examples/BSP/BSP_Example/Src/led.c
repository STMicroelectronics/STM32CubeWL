/**
  ******************************************************************************
  * @file    led.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the led supported by the
  *          B-WL5M-SUBG board
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "b_wl5m_subg_bus.h"

/** @addtogroup STM32WBxx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint32_t ButtonState;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  demo of led.
  */
void Led_demo(void)
{
  ButtonState = 0;

  HAL_Delay(2000);

  /* Init the LEDs */
  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_GREEN);

  /* Turn on Red led for 2s, Toggle it 10 times  and turn off */
  BSP_LED_On(LED_RED);
  BSP_LED_On(LED_GREEN);
  BSP_LED_On(LED_BLUE);

  HAL_Delay(1000);
  for (uint8_t i = 0 ; i < 10 ; i++)
  {
    HAL_Delay(250);
    BSP_LED_Toggle(LED_RED);
    BSP_LED_Toggle(LED_GREEN);
    BSP_LED_Toggle(LED_BLUE);
  }
  HAL_Delay(1000);
  BSP_LED_Off(LED_RED);
  BSP_LED_Off(LED_GREEN);
  BSP_LED_Off(LED_BLUE);

  /* DeInit the LEDs */
  BSP_LED_DeInit(LED_RED);
  BSP_LED_DeInit(LED_GREEN);
}

/**
  * @}
  */

/**
  * @}
  */
