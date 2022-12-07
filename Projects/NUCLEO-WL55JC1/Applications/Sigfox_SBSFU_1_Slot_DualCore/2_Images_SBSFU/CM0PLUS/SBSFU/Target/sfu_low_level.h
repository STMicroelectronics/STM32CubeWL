/**
  ******************************************************************************
  * @file    sfu_low_level.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update low level
  *          interface.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef  SFU_LOW_LEVEL_H
#define  SFU_LOW_LEVEL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sfu_def.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  SFU CRC Configuration definition
  */
typedef enum
{
  SFU_CRC_CONFIG_NONE = 0U,   /*!< None */
  SFU_CRC_CONFIG_DEFAULT,     /*!< Default configuration */
  SFU_CRC_CONFIG_16BIT        /*!< 16 bit configuration */
} SFU_CRC_ConfigTypeDef;

/* Exported constants --------------------------------------------------------*/
#define SFU_UART                                USART2
#define SFU_UART_CLK_ENABLE()                   __HAL_RCC_USART2_CLK_ENABLE()
#define SFU_UART_CLK_DISABLE()                  __HAL_RCC_USART2_CLK_DISABLE()

#define SFU_UART_TX_AF                          GPIO_AF7_USART2
#define SFU_UART_TX_GPIO_PORT                   GPIOA
#define SFU_UART_TX_PIN                         GPIO_PIN_2
#define SFU_UART_TX_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define SFU_UART_TX_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

#define SFU_UART_RX_AF                          GPIO_AF7_USART2
#define SFU_UART_RX_GPIO_PORT                   GPIOA
#define SFU_UART_RX_PIN                         GPIO_PIN_3
#define SFU_UART_RX_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
#define SFU_UART_RX_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()

/* Uncomment to enable the adequate Clock Source */
#define RTC_CLOCK_SOURCE_LSI
/*#define RTC_CLOCK_SOURCE_LSE*/

#ifdef RTC_CLOCK_SOURCE_LSI
#define RTC_ASYNCH_PREDIV    0x1FU
#define RTC_SYNCH_PREDIV     0x3FF
#endif /* RTC_CLOCK_SOURCE_LSI */

#ifdef RTC_CLOCK_SOURCE_LSE
#define RTC_ASYNCH_PREDIV  0x7FU
#define RTC_SYNCH_PREDIV   0x00FFU
#endif /* RTC_CLOCK_SOURCE_LSE */

/* External variables --------------------------------------------------------*/
extern __IO uint32_t  DummyMemAccess;
#if defined(SFU_LOW_LEVEL_C)
#if defined(IT_MANAGEMENT)

/* Dummy memory access : avoid compilation issue with STM32_Secure_Engine middleware
   but not meaningful for this product */
__IO uint32_t DummyMemAccess;
#endif /* defined(IT_MANAGEMENT) */
#endif /* defined(SFU_LOW_LEVEL_C) */


/* Exported macros -----------------------------------------------------------*/
#define IS_SFU_CRC_CONF(CONF) (((CONF) == SFU_CRC_CONFIG_NONE) || \
                               ((CONF) == SFU_CRC_CONFIG_DEFAULT) || \
                               ((CONF) == SFU_CRC_CONFIG_16BIT))

/* Dummy memory access : avoid compilation issue with STM32_Secure_Engine middleware
   but not meaningful for this product */
#define SFU_LL_DummyAccess() \
  do{ \
  }while(0)

/* Exported functions ------------------------------------------------------- */
SFU_ErrorStatus    SFU_LL_Init(void);
SFU_ErrorStatus    SFU_LL_DeInit(void);

SFU_ErrorStatus    SFU_LL_CRC_Init(void);
SFU_ErrorStatus    SFU_LL_CRC_Config(SFU_CRC_ConfigTypeDef eCRCConfg);
uint32_t           SFU_LL_CRC_Calculate(uint32_t pBuffer[], uint32_t BufferLength);

SFU_ErrorStatus    SFU_LL_UART_Init(void);
SFU_ErrorStatus    SFU_LL_UART_DeInit(void);
SFU_ErrorStatus    SFU_LL_UART_Transmit(uint8_t *pData, uint16_t DataLength, uint32_t Timeout);
SFU_ErrorStatus    SFU_LL_UART_Receive(uint8_t *pData, uint16_t DataLength, uint32_t Timeout);
SFU_ErrorStatus    SFU_LL_UART_Flush(void);

#ifndef SFU_TAMPER_PROTECT_ENABLE
SFU_ErrorStatus SFU_LL_RTC_Init(void);
SFU_ErrorStatus SFU_LL_RTC_DeInit(void);
#endif /*SFU_TAMPER_PROTECT_ENABLE*/

void  SFU_LL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void  SFU_LL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc);
void  SFU_LL_UART_MspInit(UART_HandleTypeDef *huart);
void  SFU_LL_UART_MspDeInit(UART_HandleTypeDef *huart);

SFU_ErrorStatus SFU_LL_Buffer_in_ram(void *pBuff, uint32_t Length);
SFU_ErrorStatus SFU_LL_BufferCheck_in_se_ram(const void *pBuff, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /*  SFU_LOW_LEVEL_H */
