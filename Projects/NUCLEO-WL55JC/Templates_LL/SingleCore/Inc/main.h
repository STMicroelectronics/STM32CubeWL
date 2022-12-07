/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Templates_LL/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_ll_rcc.h"
#include "stm32wlxx_ll_bus.h"
#include "stm32wlxx_ll_system.h"
#include "stm32wlxx_ll_exti.h"
#include "stm32wlxx_ll_cortex.h"
#include "stm32wlxx_ll_utils.h"
#include "stm32wlxx_ll_pwr.h"
#include "stm32wlxx_ll_dma.h"
#include "stm32wlxx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/**
  * @brief LEDs
  */
/**
  * @brief LED1
  */
/* NUCLEO-WL55JC board LED1 (green) */
#define LED1_PIN                               LL_GPIO_PIN_8
#define LED1_GPIO_PORT                         GPIOB
#define LED1_GPIO_CLK_ENABLE()                 LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define LED1_GPIO_CLK_DISABLE()                LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOB)

/**
  * @brief LED2
  */
/* NUCLEO-WL55JC board LED2 (red) */
#define LED2_PIN                               LL_GPIO_PIN_9
#define LED2_GPIO_PORT                         GPIOB
#define LED2_GPIO_CLK_ENABLE()                 LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define LED2_GPIO_CLK_DISABLE()                LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOB)

/**
  * @brief LED3
  */
/* NUCLEO-WL55JC board LED3 (blue) */
#define LED3_PIN                               LL_GPIO_PIN_15
#define LED3_GPIO_PORT                         GPIOB
#define LED3_GPIO_CLK_ENABLE()                 LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB)
#define LED3_GPIO_CLK_DISABLE()                LL_AHB2_GRP1_DisableClock(LL_AHB2_GRP1_PERIPH_GPIOB)

/**
  * @brief Push-buttons
  */
#define BUTTON_SW1_PIN                         LL_GPIO_PIN_0
#define BUTTON_SW1_GPIO_PORT                   GPIOA
#define BUTTON_SW1_GPIO_PUPD                   LL_GPIO_PULL_UP
#define BUTTON_SW1_GPIO_CLK_ENABLE()           LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define BUTTON_SW1_EXTI_LINE                   LL_EXTI_LINE_0
#define BUTTON_SW1_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(BUTTON_SW1_EXTI_LINE)
#define BUTTON_SW1_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(BUTTON_SW1_EXTI_LINE)
#define BUTTON_SW1_SYSCFG_SET_EXTI()           do {                                                                     \
                                                  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE0);  \
                                                } while(0)
#define BUTTON_SW1_EXTI_IRQn                   EXTI0_IRQn
#define BUTTON_SW1_IRQHANDLER                  EXTI0_IRQHandler

#define BUTTON_SW2_PIN                         LL_GPIO_PIN_1
#define BUTTON_SW2_GPIO_PORT                   GPIOA
#define BUTTON_SW1_GPIO_PUPD                   LL_GPIO_PULL_UP
#define BUTTON_SW2_GPIO_CLK_ENABLE()           LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA)
#define BUTTON_SW2_EXTI_LINE                   LL_EXTI_LINE_1
#define BUTTON_SW2_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(BUTTON_SW2_EXTI_LINE)
#define BUTTON_SW2_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(BUTTON_SW2_EXTI_LINE)
#define BUTTON_SW2_SYSCFG_SET_EXTI()           do {                                                                     \
                                                  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE1);  \
                                                } while(0)
#define BUTTON_SW2_EXTI_IRQn                   EXTI1_IRQn
#define BUTTON_SW2_IRQHANDLER                  EXTI1_IRQHandler

#define BUTTON_SW3_PIN                         LL_GPIO_PIN_3
#define BUTTON_SW3_GPIO_PORT                   GPIOC
#define BUTTON_SW1_GPIO_PUPD                   LL_GPIO_PULL_UP
#define BUTTON_SW3_GPIO_CLK_ENABLE()           LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC)
#define BUTTON_SW3_EXTI_LINE                   LL_EXTI_LINE_3
#define BUTTON_SW3_EXTI_LINE_ENABLE()          LL_EXTI_EnableIT_0_31(BUTTON_SW3_EXTI_LINE)
#define BUTTON_SW3_EXTI_FALLING_TRIG_ENABLE()  LL_EXTI_EnableFallingTrig_0_31(BUTTON_SW1_EXTI_LINE)
#define BUTTON_SW3_SYSCFG_SET_EXTI()           do {                                                                     \
                                                  LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE3);  \
                                                } while(0)
#define BUTTON_SW3_EXTI_IRQn                   EXTI3_IRQn
#define BUTTON_SW3_IRQHANDLER                  EXTI3_IRQHandler

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
