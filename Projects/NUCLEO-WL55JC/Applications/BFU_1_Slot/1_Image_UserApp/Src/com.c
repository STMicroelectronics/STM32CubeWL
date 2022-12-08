/**
  ******************************************************************************
  * @file    com.c
  * @author  MCD Application Team
  * @brief   COM module.
  *          This file provides set of firmware functions to manage Com
  *          functionalities.
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

/* Includes ------------------------------------------------------------------*/
#include "com.h"
#include "stm32wlxx_hal.h"
#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */



/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup USER_APP_COMMON Common
  * @{
  */

/** @defgroup  COM_Private_Defines Private Defines
  * @{
  */

#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined(__ARMCC_VERSION)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */
/**
  * @}
  */

/** @defgroup  COM_Private_Variables Exported Variables
  * @{
  */

static UART_HandleTypeDef   UartHandle; /*!< Uart Handler*/

/**
  * @}
  */

/** @defgroup  COM_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup  COM_Initialization_Functions Initialization Functions
  * @{
  */

/**
  * @brief  Initialize COM module.
  * @param  None.
  * @retval HAL Status.
  */
HAL_StatusTypeDef  COM_Init(void)
{
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
  setvbuf(stdout, NULL, _IONBF, 0);
#endif /* __GNUC__ */

  /* USART resources configuration (Clock, GPIO pins and USART registers) ----*/
  /* USART configured as follow:
  - BaudRate = 115200 baud
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  UartHandle.Instance = COM_UART;
  UartHandle.Init.BaudRate = 115200U;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Parity = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode = UART_MODE_RX | UART_MODE_TX;
  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
  UartHandle.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;

  return HAL_UART_Init(&UartHandle);
}

/**
  * @brief  DeInitialize COM module.
  * @retval None.
  * @retval HAL Status.
  */
HAL_StatusTypeDef  COM_DeInit(void)
{
  /*
  * ADD SRC CODE HERE
  * ...
  */
  return HAL_OK;
}

/**
  * @brief  UART MSP Init.
  * @param  huart: UART handler pointer.
  * @retval None.
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if (huart->Instance == COM_UART)
  {
    /* Peripheral Clock Enable */
    COM_UART_CLK_ENABLE();

    /* GPIO Ports Clock Enable */
    COM_UART_TX_GPIO_CLK_ENABLE();
    COM_UART_RX_GPIO_CLK_ENABLE();

    /*Configure GPIO pins : COM_UART_TX_Pin  */
    GPIO_InitStruct.Pin = COM_UART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = COM_UART_TX_AF;
    HAL_GPIO_Init(COM_UART_TX_GPIO_PORT, &GPIO_InitStruct);

    /*Configure GPIO pins : COM_UART_RX_Pin  */
    GPIO_InitStruct.Pin = COM_UART_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = COM_UART_RX_AF;
    HAL_GPIO_Init(COM_UART_RX_GPIO_PORT, &GPIO_InitStruct);

  }

}

/**
  * @brief  UART MSP DeInit.
  * @param  huart: UART handler pointer.
  * @retval None.
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  if (huart->Instance == COM_UART)
  {
    /* Peripheral clock disable */
    COM_UART_CLK_DISABLE();

    HAL_GPIO_DeInit(COM_UART_TX_GPIO_PORT, COM_UART_TX_PIN);
    HAL_GPIO_DeInit(COM_UART_RX_GPIO_PORT, COM_UART_RX_PIN);
  }
}


/**
  * @}
  */

/** @defgroup  COM_Control_Functions Control Functions
  * @{
  */

/**
  * @brief Transmit Data.
  * @param uDataLength: Data pointer to the Data to transmit.
  * @param uTimeout: Timeout duration.
  * @retval Status of the Transmit operation.
  */
HAL_StatusTypeDef COM_Transmit(uint8_t *Data, uint16_t uDataLength, uint32_t uTimeout)
{
  return HAL_UART_Transmit(&UartHandle, (uint8_t *)Data, uDataLength, uTimeout);
}

/**
  * @brief Receive Data.
  * @param uDataLength: Data pointer to the Data to receive.
  * @param uTimeout: Timeout duration.
  * @retval Status of the Receive operation.
  */
HAL_StatusTypeDef COM_Receive(uint8_t *Data, uint16_t uDataLength, uint32_t uTimeout)
{
  return HAL_UART_Receive(&UartHandle, (uint8_t *)Data, uDataLength, uTimeout);
}

/**
  * @brief  Flush COM Input.
  * @param None.
  * @retval HAL_Status.
  */
HAL_StatusTypeDef COM_Flush(void)
{
  /* Clean the input path */
  __HAL_UART_FLUSH_DRREGISTER(&UartHandle);
  return HAL_OK;
}

/**
  * @}
  */

/** @addtogroup  COM_Private_Functions
  * @{
  */

/**
  * @}
  */

/** @defgroup COM_Callback_Functions Callback Functions
  * @{
  */

/**
  * @brief  Retargets the C library __write function to the IAR function iar_fputc.
  * @param  file: file descriptor.
  * @param  ptr: pointer to the buffer where the data is stored.
  * @param  len: length of the data to write in bytes.
  * @retval length of the written data in bytes.
  */
#if defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
  size_t idx;
  unsigned char const *pdata = ptr;

  for (idx = 0; idx < len; idx++)
  {
    iar_fputc((int)*pdata);
    pdata++;
  }
  return len;
}
#endif /* __ICCARM__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval ch
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1U, 0xFFFFU);

  return ch;
}

/**
  * @brief  Ymodem Header Packet Transfer completed callback.
  * @param  FileSize Dimension of the file that will be received.
  * @retval None
  */
__weak HAL_StatusTypeDef Ymodem_HeaderPktRxCpltCallback(uint32_t uFileSize)
{

  /* NOTE : This function should not be modified, when the callback is needed,
            the Ymodem_HeaderPktRxCpltCallback could be implemented in the user file
   */
  return HAL_OK;
}

/**
  * @brief  Ymodem Data Packet Transfer completed callback.
  * @param  pData Pointer to the buffer.
  * @param  Size Packet dimension.
  * @retval None
  */
__weak HAL_StatusTypeDef Ymodem_DataPktRxCpltCallback(uint8_t *pData, uint32_t uFlashDestination, uint32_t uSize)
{

  /* NOTE : This function should not be modified, when the callback is needed,
            the Ymodem_DataPktRxCpltCallback could be implemented in the user file
   */
  return HAL_OK;
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
