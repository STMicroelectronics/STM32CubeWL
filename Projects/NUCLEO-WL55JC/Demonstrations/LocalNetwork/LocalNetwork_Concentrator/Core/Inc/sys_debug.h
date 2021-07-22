/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sys_debug.h
  * @author  MCD Application Team
  * @brief   Configuration of the debug.c instances
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SYS_DEBUG_H__
#define __SYS_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sys_conf.h"
#include "platform.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* Pin defines */

/**  Definition for Probe Line 1   **/
/**
  * @brief Pin of Probe Line 1
  */
#define PROBE_LINE1_PIN                           GPIO_PIN_12

/**
  * @brief Port of Probe Line 1
  */
#define PROBE_LINE1_PORT                          GPIOB

/**
  * @brief Enable GPIOs clock of Probe Line 1
  */
#define PROBE_LINE1_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()

/**
  * @brief Disable GPIOs clock of Probe Line 1
  */
#define PROBE_LINE1_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

/**  Definition for Probe Line 2   **/
/**
  * @brief Pin of Probe Line 2
  */
#define PROBE_LINE2_PIN                           GPIO_PIN_13

/**
  * @brief Port of Probe Line 2
  */
#define PROBE_LINE2_PORT                          GPIOB

/**
  * @brief Enable GPIOs clock of Probe Line 2
  */
#define PROBE_LINE2_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()

/**
  * @brief Disable GPIOs clock of Probe Line 2
  */
#define PROBE_LINE2_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

/**  Definition for Probe Line 3   **/
/**
  * @brief Pin of Probe Line 3
  */
#define PROBE_LINE3_PIN                           GPIO_PIN_14

/**
  * @brief Port of Probe Line 3
  */
#define PROBE_LINE3_PORT                          GPIOB

/**
  * @brief Enable GPIOs clock of Probe Line 3
  */
#define PROBE_LINE3_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()

/**
  * @brief Disable GPIOs clock of Probe Line 3
  */
#define PROBE_LINE3_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

/**  Definition for Probe Line 4   **/
/**
  * @brief Pin of Probe Line 4
  */
#define PROBE_LINE4_PIN                           GPIO_PIN_10

/**
  * @brief Port of Probe Line 4
  */
#define PROBE_LINE4_PORT                          GPIOB

/**
  * @brief Enable GPIOs clock of Probe Line 4
  */
#define PROBE_LINE4_CLK_ENABLE()                  __HAL_RCC_GPIOB_CLK_ENABLE()

/**
  * @brief Disable GPIOs clock of Probe Line 4
  */
#define PROBE_LINE4_CLK_DISABLE()                 __HAL_RCC_GPIOB_CLK_DISABLE()

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
#if defined (PROBE_PINS_ENABLED) && (PROBE_PINS_ENABLED == 1)

#define PROBE_GPIO_WRITE( gpio, n, x )  HAL_GPIO_WritePin( gpio, n, (GPIO_PinState)(x) )

/**
  * @brief Set pin to high level
  */
#define PROBE_GPIO_SET_LINE( gpio, n )       LL_GPIO_SetOutputPin( gpio, n )

/**
  * @brief Set pin to low level
  */
#define PROBE_GPIO_RST_LINE( gpio, n )       LL_GPIO_ResetOutputPin ( gpio, n )

/*enabling RTC_OUTPUT should be set via STM32CubeMX GUI because MX_RTC_Init is now entirely generated */

#elif defined (PROBE_PINS_ENABLED) && (PROBE_PINS_ENABLED == 0) /* PROBE_PINS_OFF */

#define PROBE_GPIO_WRITE( gpio, n, x )

/**
  * @brief not usable
  */
#define PROBE_GPIO_SET_LINE( gpio, n )

/**
  * @brief not usable
  */
#define PROBE_GPIO_RST_LINE( gpio, n )

/*disabling RTC_OUTPUT should be set via STM32CubeMX GUI because MX_RTC_Init is now entirely generated */

#else
#error "PROBE_PINS_ENABLED not defined or out of range <0,1>"
#endif /* PROBE_PINS_ENABLED */

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief Disable debugger (serial wires pins)
  */
void DBG_Disable(void);

/**
  * @brief Config debugger when working in Low Power Mode
  * @note  When in Dual Core DbgMcu pins should be better disable only after CM0+ is started
  */
void DBG_ConfigForLpm(uint8_t enableDbg);

/**
  * @brief Initializes the SW probes pins and the monitor RF pins via Alternate Function
  */
void DBG_ProbesInit(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __SYS_DEBUG_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
