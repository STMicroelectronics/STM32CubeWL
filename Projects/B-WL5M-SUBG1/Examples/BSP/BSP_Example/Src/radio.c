/**
  ******************************************************************************
  * @file    radio.c
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

/** @addtogroup STM32WLxx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  demo of radio.
  */
void Radio_demo(void)
{
  HAL_StatusTypeDef status = HAL_OK;
  ButtonState = 0;

  HAL_Delay(2000);

  /* Init the LEDs */
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Off(LED_GREEN);
  BSP_LED_Init(LED_RED);
  BSP_LED_Off(LED_RED);

  /* Init the Radio */
  status = HAL_RADIO_Init();
  if (status != HAL_OK)
  {
    BSP_LED_On(LED_RED);
    goto end_demo;
  }

  status = HAL_RADIO_SetSwitchConfig(RADIO_SWITCH_RFO_LP);
  if (status != HAL_OK)
  {
    BSP_LED_On(LED_RED);
    goto end_demo;
  }

end_demo:
  /* DeInit the demo */
  HAL_RADIO_DeInit();
  BSP_LED_DeInit(LED_RED);
  BSP_LED_DeInit(LED_GREEN);
}

/**
  * @}
  */

/**
  * @}
  */
