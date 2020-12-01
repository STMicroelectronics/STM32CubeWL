/**
  ******************************************************************************
  * @file  : stm32_adv_trace_if_template.c
  * @brief : Source file for interfacing the stm32_adv_trace to hardware
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
*/
#include "stm32_adv_trace.h"
#include "stm32_adv_trace_if.h"
/* USER CODE BEGIN include */

/* USER CODE END include */

/* Exported variables --------------------------------------------------------*/

/**
 *  @brief  trace tracer definition.
 *
 *  list all the driver interface used by the trace application.
 */
const UTIL_ADV_TRACE_Driver_s UTIL_TraceDriver =
{
  UART_Init,
  UART_DeInit,
  UART_StartRx,
  UART_TransmitDMA
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN Private_Function_Prototypes */

/* USER CODE END Private_Function_Prototypes */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN Private_Typedef */

/* USER CODE END Private_Typedef */
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Private_Define */

/* USER CODE END Private_Define */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN Private_Macro */

/* USER CODE END Private_Macro */
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Private_Variables */

/* USER CODE END Private_Variables */

UTIL_ADV_TRACE_Status_t UART_Init(  void (*cb)(void *))
{
/* USER CODE BEGIN UART_Init */
  return UTIL_ADV_TRACE_OK;
/* USER CODE END UART_Init */
}

UTIL_ADV_TRACE_Status_t UART_DeInit( void )
{
/* USER CODE BEGIN UART_DeInit */
  return UTIL_ADV_TRACE_OK;
/* USER CODE END UART_DeInit */
}

UTIL_ADV_TRACE_Status_t UART_StartRx(void (*cb)(uint8_t *pdata, uint16_t size, uint8_t error))
{
/* USER CODE BEGIN UART_DeInit */
  return UTIL_ADV_TRACE_OK;
/* USER CODE END UART_DeInit */
}

UTIL_ADV_TRACE_Status_t UART_TransmitDMA ( uint8_t *pdata, uint16_t size )
{
/* USER CODE BEGIN UART_DeInit */
	return UTIL_ADV_TRACE_OK;
/* USER CODE END UART_DeInit */
}

/* USER CODE BEGIN Private_Functions */

/* USER CODE END Private_Functions */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
