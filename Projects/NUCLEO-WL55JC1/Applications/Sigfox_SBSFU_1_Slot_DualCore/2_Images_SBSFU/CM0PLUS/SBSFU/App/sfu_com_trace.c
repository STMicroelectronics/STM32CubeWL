/**
  ******************************************************************************
  * @file    sfu_com_trace.c
  * @author  MCD Application Team
  * @brief   Secure Firmware Update COM module.
  *          This file provides set of firmware functions to manage SFU Com
  *          functionalities for the trace.
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
#include "main.h"
#include "sfu_low_level.h"
#include "sfu_trace.h"
#include "sfu_com_trace.h"
#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */

#if defined(SFU_DEBUG_MODE) || defined(SFU_TEST_PROTECTION)

/* Private defines -----------------------------------------------------------*/
#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined(__ARMCC_VERSION)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */

#define SFU_COM_TRACE_SERIAL_TIME_OUT       ((uint32_t )100U)   /*!< Serial PutString Timeout*/

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  SFU Com Init function.
  * @param  None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_COM_Init(void)
{
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
  setvbuf(stdout, NULL, _IONBF, 0);
#endif /* __GNUC__ */
  return SFU_LL_UART_Init();
}

/**
  * @brief  SFU Com DeInit function.
  * @param  None.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_COM_DeInit(void)
{
  return SFU_LL_UART_DeInit();
}
/**
  * @}
  */

/** @defgroup SFU_COM_TRACE_Control_Functions Control Functions
  * @{
  */

/**
  * @brief  Print a string on the COM Port.
  * @param  pString: The string to be printed
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_COM_Serial_PutString(uint8_t *pString)
{
  uint16_t length = 0U;

  /* Check the pointers allocation */
  if (pString == NULL)
  {
    return SFU_ERROR;
  }

  while (pString[length] != (uint8_t)'\0')
  {
    length++;
  }

  return SFU_LL_UART_Transmit(pString, length, SFU_COM_TRACE_SERIAL_TIME_OUT);

}

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
  * @brief  Retargets the C library printf function to SFU UART.
  * @param  None
  * @retval ch
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */
  (void) SFU_LL_UART_Transmit((uint8_t *)&ch, 1U, 0xFFFFU);

  return ch;
}

#endif /* SFU_DEBUG_MODE || SFU_TEST_PROTECTION */