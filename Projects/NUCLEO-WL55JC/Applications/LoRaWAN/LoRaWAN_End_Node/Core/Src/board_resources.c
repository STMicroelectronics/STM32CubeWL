/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    board_resources.c
  * @author  MCD Application Team
  * @brief   Source file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "board_resources.h"
#include "stm32wlxx_hal_exti.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief Exti Line callback function definition
  */
typedef void (* SYS_RES_EXTI_LineCallback)(void);

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief Ports led list
  */
static GPIO_TypeDef  *SYS_LED_PORT[SYS_LEDn] = {SYS_LED3_GPIO_PORT, SYS_LED2_GPIO_PORT, SYS_LED1_GPIO_PORT, };

/**
  * @brief Pins led list
  */
static const uint16_t SYS_LED_PIN[SYS_LEDn] = {SYS_LED3_PIN, SYS_LED2_PIN, SYS_LED1_PIN, };

/**
  * @brief Ports button list
  */
static GPIO_TypeDef   *SYS_BUTTON_PORT[SYS_BUTTONn] = {SYS_BUTTON1_GPIO_PORT, SYS_BUTTON3_GPIO_PORT, SYS_BUTTON2_GPIO_PORT, };

/**
  * @brief Pins button list
  */
static const uint16_t  SYS_BUTTON_PIN[SYS_BUTTONn] = {SYS_BUTTON1_PIN, SYS_BUTTON3_PIN, SYS_BUTTON2_PIN, };

/**
  * @brief IRQ button IDs list
  */
static const IRQn_Type SYS_BUTTON_IRQn[SYS_BUTTONn] = {SYS_BUTTON1_EXTI_IRQn, SYS_BUTTON3_EXTI_IRQn, SYS_BUTTON2_EXTI_IRQn, };
EXTI_HandleTypeDef sys_hpb_exti[SYS_BUTTONn];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief Button SW1 EXTI line detection callback.
  * @return none
  */
static void SYS_BUTTON1_EXTI_Callback(void);

/**
  * @brief Button SW3 EXTI line detection callback.
  * @return none
  */
static void SYS_BUTTON3_EXTI_Callback(void);

/**
  * @brief Button SW2 EXTI line detection callback.
  * @return none
  */
static void SYS_BUTTON2_EXTI_Callback(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int32_t SYS_LED_Init(Sys_Led_TypeDef Led)
{
  GPIO_InitTypeDef  gpio_init_structure = {0};
  /* Enable the GPIO_SYS_LED Clock */
  SYS_LEDx_GPIO_CLK_ENABLE(Led);

  /* Configure the GPIO_SYS_LED pin */
  gpio_init_structure.Pin = SYS_LED_PIN[Led];
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(SYS_LED_PORT[Led], &gpio_init_structure);
  HAL_GPIO_WritePin(SYS_LED_PORT[Led], SYS_LED_PIN[Led], GPIO_PIN_RESET);

  return 0;
}

int32_t SYS_LED_DeInit(Sys_Led_TypeDef Led)
{
  /* Turn off SYS_LED */
  HAL_GPIO_WritePin(SYS_LED_PORT[Led], SYS_LED_PIN[Led], GPIO_PIN_RESET);

  /* DeInit the GPIO_SYS_LED pin */
  HAL_GPIO_DeInit(SYS_LED_PORT[Led], SYS_LED_PIN[Led]);

  return 0;
}

int32_t SYS_LED_On(Sys_Led_TypeDef Led)
{
  HAL_GPIO_WritePin(SYS_LED_PORT[Led], SYS_LED_PIN[Led], GPIO_PIN_SET);

  return 0;
}

int32_t SYS_LED_Off(Sys_Led_TypeDef Led)
{
  HAL_GPIO_WritePin(SYS_LED_PORT[Led], SYS_LED_PIN[Led], GPIO_PIN_RESET);

  return 0;
}

int32_t SYS_LED_Toggle(Sys_Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(SYS_LED_PORT[Led], SYS_LED_PIN[Led]);

  return 0;
}

int32_t SYS_LED_GetState(Sys_Led_TypeDef Led)
{
  return (int32_t)HAL_GPIO_ReadPin(SYS_LED_PORT[Led], SYS_LED_PIN[Led]);
}

int32_t SYS_PB_Init(Sys_Button_TypeDef Button, Sys_ButtonMode_TypeDef ButtonMode)
{
  GPIO_InitTypeDef gpio_init_structure = {0};
  static SYS_RES_EXTI_LineCallback button_callback[SYS_BUTTONn] = {SYS_BUTTON1_EXTI_Callback, SYS_BUTTON2_EXTI_Callback, SYS_BUTTON3_EXTI_Callback};
  static uint32_t button_interrupt_priority[SYS_BUTTONn] = {SYS_BUTTONx_IT_PRIORITY, SYS_BUTTONx_IT_PRIORITY, SYS_BUTTONx_IT_PRIORITY};
  static const uint32_t button_exti_line[SYS_BUTTONn] = {SYS_BUTTON1_EXTI_LINE, SYS_BUTTON2_EXTI_LINE, SYS_BUTTON3_EXTI_LINE};

  /* Enable the SYS_BUTTON Clock */
  SYS_BUTTONx_GPIO_CLK_ENABLE(Button);

  gpio_init_structure.Pin = SYS_BUTTON_PIN[Button];
  gpio_init_structure.Pull = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

  if (ButtonMode == SYS_BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(SYS_BUTTON_PORT[Button], &gpio_init_structure);
  }
  else /* (ButtonMode == SYS_BUTTON_MODE_EXTI) */
  {
    /* Configure Button pin as input with External interrupt */
    gpio_init_structure.Mode = GPIO_MODE_IT_FALLING;

    HAL_GPIO_Init(SYS_BUTTON_PORT[Button], &gpio_init_structure);

    (void)HAL_EXTI_GetHandle(&sys_hpb_exti[Button], button_exti_line[Button]);
    (void)HAL_EXTI_RegisterCallback(&sys_hpb_exti[Button],  HAL_EXTI_COMMON_CB_ID, button_callback[Button]);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((SYS_BUTTON_IRQn[Button]), button_interrupt_priority[Button], 0x00);
    HAL_NVIC_EnableIRQ((SYS_BUTTON_IRQn[Button]));
  }

  return 0;
}

int32_t SYS_PB_DeInit(Sys_Button_TypeDef Button)
{
  HAL_NVIC_DisableIRQ((SYS_BUTTON_IRQn[Button]));
  HAL_GPIO_DeInit(SYS_BUTTON_PORT[Button], SYS_BUTTON_PIN[Button]);

  return 0;
}

int32_t SYS_PB_GetState(Sys_Button_TypeDef Button)
{
  return (int32_t)HAL_GPIO_ReadPin(SYS_BUTTON_PORT[Button], SYS_BUTTON_PIN[Button]);
}

void SYS_PB_IRQHandler(Sys_Button_TypeDef Button)
{
  HAL_EXTI_IRQHandler(&sys_hpb_exti[Button]);
}

__weak void SYS_PB_Callback(Sys_Button_TypeDef Button)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(Button);

  /* This function should be implemented by the user application.
     It is called into this driver when an event on Button is triggered.*/
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
static void SYS_BUTTON1_EXTI_Callback(void)
{
  SYS_PB_Callback(SYS_BUTTON1);
}

static void SYS_BUTTON3_EXTI_Callback(void)
{
  SYS_PB_Callback(SYS_BUTTON3);
}

static void SYS_BUTTON2_EXTI_Callback(void)
{
  SYS_PB_Callback(SYS_BUTTON2);
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
