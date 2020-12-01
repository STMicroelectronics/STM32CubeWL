/**
  ******************************************************************************
  * @file    stm32wlxx_nucleo_radio.h
  * @author  MCD Application Team
  * @brief   Header for stm32wlxx_nucleo_radio.c
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef STM32WLXX_NUCLEO_RADIO_H
#define STM32WLXX_NUCLEO_RADIO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_nucleo_errno.h"
#include "stm32wlxx_nucleo_conf.h"

   
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32WLXX_NUCLEO STM32WLXX-NUCLEO
  * @{
  */

/** @defgroup STM32WLXX_NUCLEO_RADIO_LOW_LEVEL RADIO LOW LEVEL
  * @{
  */

/** @defgroup STM32WLXX_NUCLEO_RADIO_LOW_LEVEL_Exported_Types RADIO LOW LEVEL Exported Types
  * @{
  */

typedef enum 
{
  RADIO_SWITCH_OFF    = 0,
  RADIO_SWITCH_RX     = 1,
  RADIO_SWITCH_RFO_LP = 2,
  RADIO_SWITCH_RFO_HP = 3,
}BSP_RADIO_Switch_TypeDef;

/**
  * @}
  */ 

/** @defgroup STM32WLXX_NUCLEO_RADIO_LOW_LEVEL_Exported_Constants RADIO LOW LEVEL Exported Constants
  * @{
  */

/** @defgroup STM32WLXX_NUCLEO_RADIO_LOW_LEVEL_RADIOCONFIG RADIO LOW LEVEL RADIO CONFIG Constants
  * @{
  */ 

#define RADIO_CONF_RFO_LP_HP  0
#define RADIO_CONF_RFO_LP     1
#define RADIO_CONF_RFO_HP     2

/**
  * @}
  */ 

/** @defgroup STM32WLXX_NUCLEO_RADIO_LOW_LEVEL_RFSWITCH RADIO LOW LEVEL RF SWITCH Constants
  * @{
  */ 
  
#define RF_SW_CTRL3_PIN                          GPIO_PIN_3
#define RF_SW_CTRL3_GPIO_PORT                    GPIOC
#define RF_SW_CTRL3_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define RF_SW_CTRL3_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()

#define RF_SW_CTRL1_PIN                          GPIO_PIN_4
#define RF_SW_CTRL1_GPIO_PORT                    GPIOC
#define RF_SW_CTRL1_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define RF_SW_RX_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOC_CLK_DISABLE()

#define RF_SW_CTRL2_PIN                          GPIO_PIN_5
#define RF_SW_CTRL2_GPIO_PORT                    GPIOC
#define RF_SW_CTRL2_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOC_CLK_ENABLE()
#define RF_SW_CTRL2_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOC_CLK_DISABLE()

#define RF_TCXO_VCC_PIN                          GPIO_PIN_0
#define RF_TCXO_VCC_GPIO_PORT                    GPIOB
#define RF_TCXO_VCC_CLK_ENABLE()                 __HAL_RCC_GPIOB_CLK_ENABLE()
#define RF_TCXO_VCC_CLK_DISABLE()                __HAL_RCC_GPIOB_CLK_DISABLE()
/**
 * @}
 */

/**
  * @}
  */

/** @defgroup STM32WLXX_NUCLEO_RADIO_LOW_LEVEL_Exported_Functions RADIO LOW LEVEL Exported Functions
  * @{
  */
int32_t BSP_RADIO_Init(void);
int32_t BSP_RADIO_DeInit(void);
int32_t BSP_RADIO_ConfigRFSwitch(BSP_RADIO_Switch_TypeDef Config);
int32_t BSP_RADIO_GetTxConfig(void);
int32_t BSP_RADIO_GetWakeUpTime(void);
int32_t BSP_RADIO_IsTCXO(void);
int32_t BSP_RADIO_IsDCDC(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* STM32WLXX_NUCLEO_RADIO_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

