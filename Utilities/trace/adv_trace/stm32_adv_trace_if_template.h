/**
  ******************************************************************************
  * @file  : stm32_adv_trace_if_template.h
  * @brief : Header file for stm32_adv_trace interface file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_ADV_TRACE_IF_H__
#define __STM32_ADV_TRACE_IF_H__

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup ADV_TRACE_IF low layer level of the advanced tracer
  * @{
  */

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */ 

/** 
* @brief  Init the UART and associated DMA.
* @param  cb tx function callback.
* @return @ref UTIL_ADV_TRACE_Status_t
*/
UTIL_ADV_TRACE_Status_t UART_Init(void (*cb)(void *));

/** 
* @brief  DeInit the UART and associated DMA.
* @return @ref UTIL_ADV_TRACE_Status_t
*/
UTIL_ADV_TRACE_Status_t UART_DeInit(void);


/** 
* @brief  send buffer to UART using DMA
* @param  pdata data to be sent
* @param  size of buffer p_data to be sent
* @return @ref UTIL_ADV_TRACE_Status_t
*/
UTIL_ADV_TRACE_Status_t UART_TransmitDMA(uint8_t *pdata, uint16_t size);

/** 
* @brief  start Rx process 
* @param  cb callback to receive the data
* @return @ref UTIL_ADV_TRACE_Status_t
*/
UTIL_ADV_TRACE_Status_t UART_StartRx(void (*cb)(uint8_t *pdata, uint16_t size, uint8_t error));

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

#endif /* __STM32_ADV_TRACE_IF_H__*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
