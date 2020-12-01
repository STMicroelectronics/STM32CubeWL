/**
  ******************************************************************************
  * @file    stm32wlxx_nucleo_radio.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage:
  *          - RF circuitry available on STM32WLXX-Nucleo
  *            Kit from STMicroelectronics
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

/* Includes ------------------------------------------------------------------*/
#include "stm32wlxx_nucleo_radio.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup STM32WLXX_NUCLEO
  * @{
  */

/** @addtogroup STM32WLXX_NUCLEO_RADIO_LOW_LEVEL
  * @brief This file provides set of firmware functions to Radio switch 
  *        available on STM32WLXX-Nucleo Kit from STMicroelectronics.
  * @{
  */

/** @addtogroup STM32WLXX_NUCLEO_RADIO_LOW_LEVEL_Exported_Functions
  * @{
  */
  
/**
  * @brief  Init Radio Switch 
  * @retval BSP status
  */
int32_t BSP_RADIO_Init(void)
{
  GPIO_InitTypeDef  gpio_init_structure = {0};
  
  /* Enable the Radio Switch Clock */
  RF_SW_CTRL3_GPIO_CLK_ENABLE();
  
  /* Configure the Radio Switch pin */
  gpio_init_structure.Pin   = RF_SW_CTRL1_PIN;
  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull  = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  HAL_GPIO_Init(RF_SW_CTRL1_GPIO_PORT, &gpio_init_structure);
  
  gpio_init_structure.Pin = RF_SW_CTRL2_PIN;
  HAL_GPIO_Init(RF_SW_CTRL2_GPIO_PORT, &gpio_init_structure);
  
  gpio_init_structure.Pin = RF_SW_CTRL3_PIN;
  HAL_GPIO_Init(RF_SW_CTRL3_GPIO_PORT, &gpio_init_structure);

  HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_RESET); 
  HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_RESET); 
  HAL_GPIO_WritePin(RF_SW_CTRL3_GPIO_PORT, RF_SW_CTRL3_PIN, GPIO_PIN_RESET); 

  return BSP_ERROR_NONE;
}

/**
  * @brief  DeInit Radio Switch
  * @retval BSP status
  */
int32_t BSP_RADIO_DeInit(void)
{
  RF_SW_CTRL3_GPIO_CLK_ENABLE();

  /* Turn off switch */
  HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_RESET); 
  HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_RESET); 
  HAL_GPIO_WritePin(RF_SW_CTRL3_GPIO_PORT, RF_SW_CTRL3_PIN, GPIO_PIN_RESET); 
  
  /* DeInit the Radio Switch pin */
  HAL_GPIO_DeInit(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN);
  HAL_GPIO_DeInit(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN);
  HAL_GPIO_DeInit(RF_SW_CTRL3_GPIO_PORT, RF_SW_CTRL3_PIN);

  return BSP_ERROR_NONE;
}

/**
  * @brief  Configure Radio Switch.
  * @param  Config: Specifies the Radio RF switch path to be set. 
  *         This parameter can be one of following parameters:
  *           @arg RADIO_SWITCH_OFF
  *           @arg RADIO_SWITCH_RX
  *           @arg RADIO_SWITCH_RFO_LP
  *           @arg RADIO_SWITCH_RFO_HP
  * @retval BSP status
  */
int32_t BSP_RADIO_ConfigRFSwitch(BSP_RADIO_Switch_TypeDef Config)
{
  switch (Config)
  {
    case RADIO_SWITCH_OFF:
    {
      /* Turn off switch */
      HAL_GPIO_WritePin(RF_SW_CTRL3_GPIO_PORT, RF_SW_CTRL3_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_RESET);
      break;      
    }
    case RADIO_SWITCH_RX:
    {
      /*Turns On in Rx Mode the RF Switch */
      HAL_GPIO_WritePin(RF_SW_CTRL3_GPIO_PORT, RF_SW_CTRL3_PIN, GPIO_PIN_SET);
      HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_SET); 
      HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_RESET); 
      break;
    }
    case RADIO_SWITCH_RFO_LP:
    {
      /*Turns On in Tx Low Power the RF Switch */
      HAL_GPIO_WritePin(RF_SW_CTRL3_GPIO_PORT, RF_SW_CTRL3_PIN, GPIO_PIN_SET);
      HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_SET); 
      HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_SET); 
      break;
    }
    case RADIO_SWITCH_RFO_HP:
    {
      /*Turns On in Tx High Power the RF Switch */
      HAL_GPIO_WritePin(RF_SW_CTRL3_GPIO_PORT, RF_SW_CTRL3_PIN, GPIO_PIN_SET);
      HAL_GPIO_WritePin(RF_SW_CTRL1_GPIO_PORT, RF_SW_CTRL1_PIN, GPIO_PIN_RESET); 
      HAL_GPIO_WritePin(RF_SW_CTRL2_GPIO_PORT, RF_SW_CTRL2_PIN, GPIO_PIN_SET); 
      break;
    }
    default:
      break;    
  }  

  return BSP_ERROR_NONE;
}

/**
  * @brief  Return Board Configuration
  * @retval 
  *  RADIO_CONF_RFO_LP_HP
  *  RADIO_CONF_RFO_LP
  *  RADIO_CONF_RFO_HP
  */
int32_t BSP_RADIO_GetTxConfig(void)
{
  return RADIO_CONF_RFO_LP_HP;
}

/**
  * @brief  Get Radio Wake Time
  * @retval the wake upt time in ms
  */
int32_t BSP_RADIO_GetWakeUpTime(void)
{
  return  RF_WAKEUP_TIME;
}

/**
  * @brief  Get If TCXO is to be present on board
  * @note   never remove called by MW, 
  * @retval return 1 if present, 0 if not present
  */
int32_t BSP_RADIO_IsTCXO(void)  
{
  return IS_TCXO_SUPPORTED;
}

/**
  * @brief  Get If DCDC is to be present on board
  * @note   never remove called by MW, 
  * @retval return 1 if present, 0 if not present
  */
int32_t BSP_RADIO_IsDCDC(void)  
{
  return IS_DCDC_SUPPORTED;
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
