/**
  ******************************************************************************
  * @file    sfu_low_level.c
  * @author  MCD Application Team
  * @brief   SFU Low Level Interface module
  *          This file provides set of firmware functions to manage SFU low level
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

#define SFU_LOW_LEVEL_C

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sfu_low_level.h"
#include "sfu_trace.h"

#ifndef SFU_TAMPER_PROTECT_ENABLE
extern RTC_HandleTypeDef RtcHandle;
#endif /* SFU_TAMPER_PROTECT_ENABLE */

/* Private variables ---------------------------------------------------------*/
static UART_HandleTypeDef   UartHandle;
static CRC_HandleTypeDef    CrcHandle;

/* Private function prototypes -----------------------------------------------*/
static void     SFU_LL_Error_Handler(void);

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  Initialize SFU Interface.
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_Init(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Initialize CRC */
  e_ret_status = SFU_LL_CRC_Init();

  return e_ret_status;
}

/**
  * @brief  DeInitialize SFU Interface.
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_DeInit(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* ADD SRC CODE HERE
       ...
  */
  return e_ret_status;
}

/**
  * @brief  SFU IF CRC Init.
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_CRC_Init(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Configure the peripheral clock */
  __HAL_RCC_CRC_CLK_ENABLE();


  /* Configure CRC with default polynomial - standard configuration */
  e_ret_status = SFU_LL_CRC_Config(SFU_CRC_CONFIG_DEFAULT);

  return e_ret_status;
}

/**
  * @brief  SFU  IF CRC Configuration.
  * @param  eCRCConfg: SFU_CRC_ConfigTypeDef.
  *         This parameter can be a value of @ref SFU_CRC_ConfigTypeDef.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_CRC_Config(SFU_CRC_ConfigTypeDef eCRCConfg)
{
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;

  /* Check the parameters */
  assert_param(IS_SFU_CRC_CONF(eCRCConfg));

  /* Switch to the selected configuration */
  CrcHandle.Instance = CRC;

  /* The input data are not inverted */
  CrcHandle.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;

  /* The output data are not inverted */
  CrcHandle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;

  switch (eCRCConfg)
  {
    case SFU_CRC_CONFIG_DEFAULT:
      /* The Default polynomial is used */
      CrcHandle.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
      /* The default init value is used */
      CrcHandle.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
      /* The input data are 32-bit long words */
      CrcHandle.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
      /* Valid parameter*/
      e_ret_status = SFU_SUCCESS;
      break;

    case SFU_CRC_CONFIG_16BIT:
      /* The CRC-16-CCIT polynomial is used */
      CrcHandle.Init.DefaultPolynomialUse    = DEFAULT_POLYNOMIAL_DISABLE;
      CrcHandle.Init.GeneratingPolynomial    = 0x1021U;
      CrcHandle.Init.CRCLength               = CRC_POLYLENGTH_16B;
      /* The zero init value is used */
      CrcHandle.Init.DefaultInitValueUse     = DEFAULT_INIT_VALUE_DISABLE;
      CrcHandle.Init.InitValue               = 0U;
      /* The input data are 8-bit long */
      CrcHandle.InputDataFormat              = CRC_INPUTDATA_FORMAT_BYTES;
      /* Valid parameter*/
      e_ret_status = SFU_SUCCESS;
      break;

    default:
      /* Invalid parameter */
      e_ret_status = SFU_ERROR;
      break;
  }

  /* Proceed to CRC Init (Correct Parameters) */
  if (e_ret_status == SFU_SUCCESS)
  {
    if (HAL_CRC_Init(&CrcHandle) != HAL_OK)
    {
      e_ret_status = SFU_ERROR;
    }
  }

  return e_ret_status;
}


/**
  * @brief  SFU IF CRC Calculate.
  * @param  pBuffer: pointer to data buffer.
  * @param  BufferLength: buffer length in bytes.
  * @retval uint32_t CRC (returned value LSBs for CRC shorter than 32 bits)
  */
uint32_t SFU_LL_CRC_Calculate(uint32_t pBuffer[], uint32_t BufferLength)
{
  return HAL_CRC_Calculate(&CrcHandle, pBuffer, BufferLength);
}

/**
  * @brief SFU UART Init.
  * @param  None
  * @retval status of the Init operation
  *         SFU_ERROR : if the Init operation failed.
  *         SFU_SUCCESS : if the Init operation is successfully performed.
  */
SFU_ErrorStatus SFU_LL_UART_Init(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* USART resources configuration (Clock, GPIO pins and USART registers) ----*/
  /* USART configured as follow:
  - BaudRate = 115200 baud
  - Word Length = 8 Bits
  - One Stop Bit
  - No parity
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled
  */
  UartHandle.Instance = SFU_UART;
  UartHandle.Init.BaudRate = 115200U;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Parity = UART_PARITY_NONE;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode = UART_MODE_RX | UART_MODE_TX;
  UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
  UartHandle.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
  if (HAL_UART_Init(&UartHandle) == HAL_OK)
  {
    e_ret_status = SFU_SUCCESS;
  }

  return e_ret_status;

}

/**
  * @brief SFU IF UART DeInit.
  * @param  None
  * @retval status of the Init operation
  *         SFU_ERROR : if the Init operation failed.
  *         SFU_SUCCESS : if the Init operation is successfully performed.
  */
SFU_ErrorStatus SFU_LL_UART_DeInit(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /*
    * ADD SRC CODE HERE
    */

  e_ret_status = SFU_SUCCESS;

  return e_ret_status;

}

/**
  * @brief  SFU IF Write data (send).
  * @param  pData: pointer to the 128bit data to write.
  * @param  DataLength: pointer to the 128bit data to write.
  * @param  pData: pointer to the 128bit data to write.
  * @param  Timeout: Timeout duration.
  * @retval status of the write operation
  *         SFU_ERROR : if the write operation is not performed
  *         SFU_SUCCESS : if the write operation is successfully performed
  */
SFU_ErrorStatus SFU_LL_UART_Transmit(uint8_t *pData, uint16_t DataLength, uint32_t Timeout)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check the pointers allocation */
  if (pData == NULL)
  {
    return SFU_ERROR;
  }

  if (HAL_UART_Transmit(&UartHandle, (uint8_t *)pData, DataLength, Timeout) == HAL_OK)
  {
    e_ret_status = SFU_SUCCESS;
  }

  return e_ret_status;
}

/**
  * @brief  SFU IF Read data (receive).
  * @param  pData: pointer to the 128bit data where to store the received data.
  * @param  DataLength: the length of the data to be read in bytes.
  * @param Timeout: Timeout duration.
  * @retval status of the read operation
  *         SFU_ERROR : if the read operation is not performed
  *         SFU_SUCCESS : if the read operation is successfully performed
  */
SFU_ErrorStatus SFU_LL_UART_Receive(uint8_t *pData, uint16_t DataLength, uint32_t Timeout)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check the pointers allocation */
  if (pData == NULL)
  {
    return SFU_ERROR;
  }

  if (HAL_UART_Receive(&UartHandle, (uint8_t *)pData, DataLength, Timeout) == HAL_OK)
  {
    e_ret_status = SFU_SUCCESS;
  }

  return e_ret_status;
}

/**
  * @brief  SFU HAL IF Flush.
  * @param  None.
  * @retval status of the operation.
  */
SFU_ErrorStatus SFU_LL_UART_Flush(void)
{
  /* Clean the input path */
  __HAL_UART_FLUSH_DRREGISTER(&UartHandle);

  return SFU_SUCCESS;
}

#if  defined(CORE_CM0PLUS)
#ifndef SFU_TAMPER_PROTECT_ENABLE
/**
  * @brief SFU IF RTC Init.
  * @param  None
  * @retval status of the Init operation
  *         SFU_ERROR : if the Init operation failed.
  *         SFU_SUCCESS : if the Init operation is successfully performed.
  */
SFU_ErrorStatus SFU_LL_RTC_Init(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Configure RTC prescaler and RTC data registers */
  /* RTC configured as follows:
  - Hour Format    = Format 24
  - Asynch Prediv  = Value according to source clock
  - Synch Prediv   = Value according to source clock
  - OutPut         = Output Disable
  - OutPutPolarity = High Polarity
  - OutPutType     = Open Drain */
  RtcHandle.Instance            = RTC;
  RtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;

  if (HAL_RTC_Init(&RtcHandle) == HAL_OK)
  {
    e_ret_status = SFU_SUCCESS;
  }

  return e_ret_status;
}

/**
  * @brief SFU IF RTC DeInit.
  * @param  None
  * @retval status of the Init operation
  *         SFU_ERROR : if the Init operation failed.
  *         SFU_SUCCESS : if the Init operation is successfully performed.
  */
SFU_ErrorStatus SFU_LL_RTC_DeInit(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /*
    * ADD SRC CODE HERE
    */

  e_ret_status = SFU_SUCCESS;

  return e_ret_status;

}

#endif /*SFU_TAMPER_PROTECT_ENABLE*/
#endif /* CORE_CM0PLUS */

/**
  * @brief SFU IF RTC MSP Initialization
  *        This function configures the hardware resources used in this example
  * @param hrtc: RTC handle pointer.
  * @note  Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
  *        the RTC clock source; in this case the Backup domain will be reset in
  *        order to modify the RTC Clock source, as consequence RTC registers (including
  *        the backup registers) and RCC_BDCR register are set to their reset values.
  * @retval None
  */
void SFU_LL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  UNUSED(hrtc);
  /*-1- Enables access to the backup domain */
  /* To enable access on RTC registers */

  HAL_PWR_EnableBkUpAccess();

  /*-2- Configure LSE/LSI as RTC clock source */
#ifdef RTC_CLOCK_SOURCE_LSE

  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SFU_LL_Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    SFU_LL_Error_Handler();
  }
#elif defined (RTC_CLOCK_SOURCE_LSI)
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.LSIDiv = LL_RCC_LSI_PREDIV_1;
  RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SFU_LL_Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    SFU_LL_Error_Handler();
  }
#else
#error Please select the RTC Clock source inside the main.h file
#endif /*RTC_CLOCK_SOURCE_LSE*/

  /*-3- Enable RTC peripheral Clocks */
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();

  /* Enable RTC APB clock  */
  __HAL_RCC_RTCAPB_CLK_ENABLE();
#ifdef SFU_TAMPER_PROTECT_ENABLE
  /*-4- Configure the NVIC for RTC Tamper */
  HAL_NVIC_SetPriority(RTC_LSECSS_IRQn, 0x0FU, 0U);
  HAL_NVIC_EnableIRQ(RTC_LSECSS_IRQn);
#endif /* SFU_TAMPER_PROTECT_ENABLE */
}

/**
  * @brief RTC MSP De-Initialization
  *        This function frees the hardware resources used in in SFU application:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void SFU_LL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  UNUSED(hrtc);

  __HAL_RCC_RTC_DISABLE();

  /* Disable RTC APB clock  */
  __HAL_RCC_RTCAPB_CLK_DISABLE();
}

/**
  * @brief UART MSP Initialization
  *        This function configures the hardware resources used in SFU application.
  * @param huart: UART handle pointer
  * @retval None
  */
void SFU_LL_UART_MspInit(UART_HandleTypeDef *huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if (huart->Instance == SFU_UART)
  {
    /* Peripheral Clock Enable */
    SFU_UART_CLK_ENABLE();

    /* GPIO Ports Clock Enable */
    SFU_UART_TX_GPIO_CLK_ENABLE();
    SFU_UART_RX_GPIO_CLK_ENABLE();

    /*Configure GPIO pins : SFU_UART_TX_Pin  */
    GPIO_InitStruct.Pin = SFU_UART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = SFU_UART_TX_AF;
    HAL_GPIO_Init(SFU_UART_TX_GPIO_PORT, &GPIO_InitStruct);

    /*Configure GPIO pins : SFU_UART_RX_Pin  */
    GPIO_InitStruct.Pin = SFU_UART_RX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = SFU_UART_RX_AF;
    HAL_GPIO_Init(SFU_UART_RX_GPIO_PORT, &GPIO_InitStruct);

  }

}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in SFU application:
  *          - Disable the Peripheral's clock
  * @param huart: UART handle pointer
  * @retval None
  */
void SFU_LL_UART_MspDeInit(UART_HandleTypeDef *huart)
{

  if (huart->Instance == SFU_UART)
  {
    /* Peripheral clock disable */
    SFU_UART_CLK_DISABLE();

    /* GPIO DeInit*/
    HAL_GPIO_DeInit(SFU_UART_TX_GPIO_PORT, SFU_UART_TX_PIN);
    HAL_GPIO_DeInit(SFU_UART_RX_GPIO_PORT, SFU_UART_RX_PIN);

  }


}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void SFU_LL_Error_Handler(void)
{
  /*
    * ADD SRC CODE HERE
    */

  while (1 == 1)
  {
    NVIC_SystemReset();
  }
}


/**
  * @}
  */
/**
  * @brief function checking if a buffer is in se ram.
  * @param pBuff: Secure Engine protected function ID.
  * @param length: length of buffer in bytes
  * @retval SE_ErrorStatus SE_SUCCESS for buffer in se ram, SE_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LL_BufferCheck_in_se_ram(const void *pBuff, uint32_t length)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  uint32_t addr_start = (uint32_t)pBuff;
  uint32_t addr_end = addr_start + length - 1U;

  /* Check if length is positive with no overflow and included in the correct range */
  if ((length != 0U) && (!((0xFFFFFFFFUL - addr_start) < length))
      && ((addr_end  <= SE_REGION_RAM_END) && (addr_start >= SE_REGION_RAM_START)))
  {
    e_ret_status = SFU_SUCCESS;
  }
  else
  {
    e_ret_status = SFU_ERROR;
  }
  return e_ret_status;
}

/**
  * @brief Check if an array is inside the RAM of the product
  * @param Addr : address  of array
  * @param Length : length of array in byte
  */
SFU_ErrorStatus SFU_LL_Buffer_in_ram(void *pBuff, uint32_t Length)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  uint32_t addr_start = (uint32_t)pBuff;
  uint32_t addr_end = addr_start + Length - 1U;

  /* Check if length is positive with no overflow and included in the correct range */
  if ((Length != 0U) && (!((0xFFFFFFFFUL - addr_start) < Length))
      && ((addr_end  <= SB_REGION_RAM_END) && (addr_start >= SB_REGION_RAM_START)))
  {
    e_ret_status = SFU_SUCCESS;
  }
  else
  {
    e_ret_status = SFU_ERROR;
  }
  return e_ret_status;
}

