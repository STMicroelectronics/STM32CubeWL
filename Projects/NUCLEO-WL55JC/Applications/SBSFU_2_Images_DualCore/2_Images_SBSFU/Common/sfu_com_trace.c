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
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sfu_low_level.h"
#include "sfu_trace.h"
#include "sfu_com_trace.h"

#if defined(SFU_DEBUG_MODE) || defined(SFU_TEST_PROTECTION)

/* Private defines -----------------------------------------------------------*/
#if defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int putchar(int ch)
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
