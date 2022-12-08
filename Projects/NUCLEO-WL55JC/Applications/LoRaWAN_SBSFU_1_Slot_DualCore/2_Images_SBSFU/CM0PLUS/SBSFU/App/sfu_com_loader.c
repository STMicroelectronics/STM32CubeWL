/**
  ******************************************************************************
  * @file    sfu_com_loader.c
  * @author  MCD Application Team
  * @brief   Secure Firmware Update COM module.
  *          This file provides set of firmware functions to manage SFU Com
  *          functionalities for the local loader.
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
#include "sfu_low_level_security.h"
#include "sfu_com_loader.h"
#include "sfu_trace.h"


#if (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) || defined(SFU_TEST_PROTECTION)

/* Private defines -----------------------------------------------------------*/
#define SFU_COM_LOADER_TIME_OUT            ((uint32_t )0x800U)   /*!< COM Transmit and Receive Timeout*/
#define SFU_COM_LOADER_SERIAL_TIME_OUT       ((uint32_t )100U)   /*!< Serial PutByte and PutString Timeout*/

/* Private macros ------------------------------------------------------------*/
#define IS_CAP_LETTER(c)    (((c) >= (uint8_t) 'A') && ((c) <= (uint8_t) 'F'))
#define IS_LC_LETTER(c)     (((c) >= (uint8_t) 'a') && ((c) <= (uint8_t) 'f'))
#define IS_09(c)            (((c) >= (uint8_t) '0') && ((c) <= (uint8_t) '9'))
#define ISVALIDHEX(c)       (IS_CAP_LETTER(c) || IS_LC_LETTER(c) || IS_09(c))
#define ISVALIDDEC(c)       IS_09(c)
#define CONVERTDEC(c)       ((c) - (uint8_t) '0')

#define CONVERTHEX_ALPHA(c) (IS_CAP_LETTER(c) ? ((c) - (uint8_t) 'A' + 10U) : ((c) - (uint8_t) 'a' + 10U))
#define CONVERTHEX(c)       (IS_09(c) ? ((c) - (uint8_t) '0') : CONVERTHEX_ALPHA(c))

/* Private function prototypes -----------------------------------------------*/
static HAL_StatusTypeDef ReceivePacket(uint8_t *pData, uint32_t *puLength, uint32_t uTimeOut);
static uint32_t Str2Int(uint8_t *pInputStr, uint32_t *pIntNum);

/* Functions Definition ------------------------------------------------------*/

#if !defined(SFU_DEBUG_MODE) && !defined(SFU_TEST_PROTECTION)
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

#endif /* !SFU_DEBUG_MODE && !SFU_TEST_PROTECTION */

/**
  * @brief  Receive a file using the ymodem protocol with SFU_COM_YMODEM_CRC16.
  * @param  peCOMStatus: SFU_COM_YMODEM_StatusTypeDef result of reception/programming.
  * @param  puSize: size of received file.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise
  */
SFU_ErrorStatus SFU_COM_YMODEM_Receive(SFU_COM_YMODEM_StatusTypeDef *peCOMStatus, uint32_t *puSize)
{
  uint32_t i;
  uint32_t packet_length = 0U;
  uint32_t session_done = 0U;
  uint32_t file_done;
  uint32_t errors = 0U;
  uint32_t session_begin = 0U;
  uint32_t filesize = 0U;
  uint32_t packets_received;
  uint8_t *file_ptr;
  uint8_t file_size[SFU_COM_YMODEM_FILE_SIZE_LENGTH + 1U];
  uint8_t tmp;
  static uint8_t m_aPacketData[SFU_COM_YMODEM_PACKET_1K_SIZE + SFU_COM_YMODEM_PACKET_DATA_INDEX +
                               SFU_COM_YMODEM_PACKET_TRAILER_SIZE] __attribute__((aligned(8)));

  /* Check the pointers allocation */
  if ((peCOMStatus == NULL) || (puSize == NULL))
  {
    return SFU_ERROR;
  }

  *peCOMStatus = SFU_COM_YMODEM_OK;

  while ((session_done == 0U) && (*peCOMStatus == SFU_COM_YMODEM_OK))
  {
    packets_received = 0U;
    file_done = 0U;
    while ((file_done == 0U) && (*peCOMStatus == SFU_COM_YMODEM_OK))
    {
      switch (ReceivePacket(m_aPacketData, &packet_length, SFU_COM_YMODEM_DOWNLOAD_TIMEOUT))
      {
        case HAL_OK:
          errors = 0U;
          switch (packet_length)
          {
            case 3U:
              /* Startup sequence */
              break;
            case 2U:
              /* Abort by sender */
              (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_ACK);
              *peCOMStatus = SFU_COM_YMODEM_ABORT;
              break;
            case 0U:
              /* End of transmission */
              (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_ACK);
              *puSize = filesize;
              file_done = 1U;
              break;
            default:
              /* Normal packet */
              if (m_aPacketData[SFU_COM_YMODEM_PACKET_NUMBER_INDEX] != (packets_received & 0x000000FFU))
              {
                /* No NACK sent for a better synchro with remote : packet will be repeated */
              }
              else
              {
                if (packets_received == 0U)
                {
                  /* File name packet */
                  if (m_aPacketData[SFU_COM_YMODEM_PACKET_DATA_INDEX] != 0U)
                  {
                    /* File name skipped */
                    i = 0U;
                    file_ptr = m_aPacketData + SFU_COM_YMODEM_PACKET_DATA_INDEX;
                    while ((*file_ptr != 0U) && (i < SFU_COM_YMODEM_FILE_NAME_LENGTH))
                    {
                      i++;
                      file_ptr++;
                    }
                    /* end of file name ('\0') skipped */
                    file_ptr++;

                    /* File size extraction */
                    i = 0U;
                    while ((*file_ptr != (uint8_t) ' ') && (i < SFU_COM_YMODEM_FILE_SIZE_LENGTH))
                    {
                      file_size[i] = *file_ptr;
                      i++;
                      file_ptr++;
                    }
                    file_size[i] = (uint8_t) '\0';
                    (void) Str2Int(file_size, &filesize);

                    /* Header packet received callback call*/
                    if (SFU_COM_YMODEM_HeaderPktRxCpltCallback((uint32_t) filesize) == SFU_SUCCESS)
                    {
                      (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_ACK);
                      (void) SFU_LL_UART_Flush();
                      (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_CRC16);
                    }
                    else
                    {
                      /* End session */
                      tmp = SFU_COM_YMODEM_CA;
                      (void) SFU_LL_UART_Transmit(&tmp, 1U, SFU_COM_YMODEM_NAK_TIMEOUT);
                      (void) SFU_LL_UART_Transmit(&tmp, 1U, SFU_COM_YMODEM_NAK_TIMEOUT);
                      return SFU_ERROR;
                    }
                  }
                  /* File header packet is empty, end session */
                  else
                  {
                    (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_ACK);
                    file_done = 1U;
                    session_done = 1U;
                    break;
                  }
                }
                else /* Data packet */
                {
                  /* Data packet received callback call*/
                  if (SFU_COM_YMODEM_DataPktRxCpltCallback(&m_aPacketData[SFU_COM_YMODEM_PACKET_DATA_INDEX],
                                                           packet_length) == SFU_SUCCESS)
                  {
                    (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_ACK);

                  }
                  else /* An error occurred while writing to Flash memory */
                  {
                    /* End session */
                    (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_CA);
                    (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_CA);
                    *peCOMStatus = SFU_COM_YMODEM_DATA;
                  }

                }
                packets_received ++;
                session_begin = 1;
              }
              break;
          }
          break;
        case HAL_BUSY: /* Abort actually */
          (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_CA);
          (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_CA);
          *peCOMStatus = SFU_COM_YMODEM_ABORT;
          break;
        default:
          if (session_begin > 0U)
          {
            errors ++;
          }
          if (errors > SFU_COM_YMODEM_MAX_ERRORS)
          {
            /* Abort communication */
            (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_CA);
            (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_CA);
            *peCOMStatus = SFU_COM_YMODEM_ABORT;
          }
          else
          {
            (void) SFU_COM_Serial_PutByte(SFU_COM_YMODEM_CRC16); /* Ask for a packet */
#ifndef MINICOM_YMODEM
            /* Minicom does not accept this extra character */
            TRACE("\b.");                                 /* Replace C char by . on display console */
#endif /* MINICOM_YMODEM */
            /*
             * Toggling the LED in case no console is available: the toggling frequency depends on
             * SFU_COM_YMODEM_DOWNLOAD_TIMEOUT
             */
            (void) BSP_LED_Toggle(SFU_STATUS_LED);
          }
          break;
      }
    }
  }
  /* Make sure the status LED is turned off */
  (void) BSP_LED_Off(SFU_STATUS_LED);

  if (*peCOMStatus == SFU_COM_YMODEM_OK)
  {
    return SFU_SUCCESS;
  }
  else
  {
    return SFU_ERROR;
  }
}

/**
  * @brief  Transmit a byte to the COM Port.
  * @param  uParam: The byte to be sent.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_COM_Serial_PutByte(uint8_t uParam)
{
  return SFU_LL_UART_Transmit(&uParam, 1U, SFU_COM_LOADER_SERIAL_TIME_OUT);
}

/**
  * @brief  Convert a string to an integer.
  * @param  pInputStr: The string to be converted.
  * @param  pIntNum: The integer value.
  * @retval 1: Correct
  *         0: Error
  */
static uint32_t Str2Int(uint8_t *pInputStr, uint32_t *pIntNum)
{
  uint32_t i = 0U;
  uint32_t res = 0U;
  uint32_t val = 0U;
  uint8_t  digit;

  if ((pInputStr[0] == (uint8_t)'0') && ((pInputStr[1] == (uint8_t)'x') || (pInputStr[1] == (uint8_t)'X')))
  {
    i = 2U;
    while ((i < 11U) && (pInputStr[i] != (uint8_t)'\0'))
    {
      if (ISVALIDHEX(pInputStr[i]))
      {
        digit = CONVERTHEX(pInputStr[i]);
        val = (val << 4U) + digit;
      }
      else
      {
        /* Return 0, Invalid input */
        res = 0U;
        break;
      }
      i++;
    }

    /* valid result */
    if (pInputStr[i] == (uint8_t)'\0')
    {
      *pIntNum = val;
      res = 1U;
    }
  }
  else /* max 10-digit decimal input */
  {
    while ((i < 11U) && (res != 1U))
    {
      if (pInputStr[i] == (uint8_t)'\0')
      {
        *pIntNum = val;
        /* return 1 */
        res = 1U;
      }
      else if (((pInputStr[i] == (uint8_t)'k') || (pInputStr[i] == (uint8_t)'K')) && (i > 0U))
      {
        val = val << 10U;
        *pIntNum = val;
        res = 1U;
      }
      else if (((pInputStr[i] == (uint8_t)'m') || (pInputStr[i] == (uint8_t)'M')) && (i > 0U))
      {
        val = val << 20U;
        *pIntNum = val;
        res = 1U;
      }
      else if (ISVALIDDEC(pInputStr[i]))
      {
        digit =  CONVERTDEC(pInputStr[i]);
        val = (val * 10U) + digit;
      }
      else
      {
        /* return 0, Invalid input */
        res = 0U;
        break;
      }
      i++;
    }
  }

  return res;
}

/**
  * @brief  Receive a packet from sender
  * @param  pData: pointer to received data.
  * @param  puLength
  *     0: end of transmission
  *     2: abort by sender
  *    >0: packet length
  * @param  uTimeOut: receive timeout (ms).
  * @retval HAL_OK: normally return
  *         HAL_BUSY: abort by user
  */
static HAL_StatusTypeDef ReceivePacket(uint8_t *pData, uint32_t *puLength, uint32_t uTimeout)
{
  uint32_t crc;
  uint32_t packet_size = 0U;
  HAL_StatusTypeDef status;
  SFU_ErrorStatus eRetStatus;

  uint8_t char1;

#ifdef MINICOM_YMODEM
  uint32_t myIdx = 0; /* index indicating where to write in pData */
#endif /* MINICOM_YMODEM */

  *puLength = 0U;

  /* This operation could last long. Need to refresh the Watchdog if enabled. It could be implemented as a callback*/
  (void) SFU_LL_SECU_IWDG_Refresh();

  eRetStatus = SFU_LL_UART_Receive(&char1, 1, uTimeout);

  if (eRetStatus == SFU_SUCCESS)
  {
    status = HAL_OK;

    switch (char1)
    {
      case SFU_COM_YMODEM_SOH:
        packet_size = SFU_COM_YMODEM_PACKET_SIZE;
        break;
      case SFU_COM_YMODEM_STX:
        packet_size = SFU_COM_YMODEM_PACKET_1K_SIZE;
        break;
      case SFU_COM_YMODEM_EOT:
        break;
      case SFU_COM_YMODEM_CA:
        if ((SFU_LL_UART_Receive(&char1, 1U, uTimeout) == SFU_SUCCESS) && (char1 == SFU_COM_YMODEM_CA))
        {
          packet_size = 2U;
        }
        else
        {
          status = HAL_ERROR;
        }
        break;
      case SFU_COM_YMODEM_ABORT1:
      case SFU_COM_YMODEM_ABORT2:
        status = HAL_BUSY;
        break;
      case SFU_COM_YMODEM_RB:
        (void) SFU_LL_UART_Receive(&char1, 1U, uTimeout);             /* Ymodem startup sequence : rb ==> 0x72 + 0x62 + 0x0D */
        (void) SFU_LL_UART_Receive(&char1, 1U, uTimeout);
        packet_size = 3U;
        break;
      default:
        status = HAL_ERROR;
        break;
    }
    *pData = char1;

    if (packet_size >= SFU_COM_YMODEM_PACKET_SIZE)
    {

#ifndef MINICOM_YMODEM
      eRetStatus = SFU_LL_UART_Receive(&pData[SFU_COM_YMODEM_PACKET_NUMBER_INDEX],
                                       (uint16_t)(packet_size + SFU_COM_YMODEM_PACKET_OVERHEAD_SIZE), uTimeout);
#else
      eRetStatus = SFU_SUCCESS;
      /* Receive byte per byte until no data left or no space left in buffer */
      while ((SFU_SUCCESS == eRetStatus) &&
             ((myIdx + SFU_COM_YMODEM_PACKET_NUMBER_INDEX) < SFU_COM_YMODEM_PACKET_1K_SIZE +
              SFU_COM_YMODEM_PACKET_DATA_INDEX +
              SFU_COM_YMODEM_PACKET_TRAILER_SIZE)
            )
      {
        eRetStatus = SFU_LL_UART_Receive(&pData[SFU_COM_YMODEM_PACKET_NUMBER_INDEX + myIdx],
                                         1U, uTimeout);
        myIdx++;
      }

      if (myIdx > 1)
      {
        /* We received some bytes */
        eRetStatus = SFU_SUCCESS;
      }
#endif /* MINICOM_YMODEM */

      /* Simple packet sanity check */
      if (eRetStatus == SFU_SUCCESS)
      {
        status = HAL_OK;

        if (pData[SFU_COM_YMODEM_PACKET_NUMBER_INDEX] != ((pData[SFU_COM_YMODEM_PACKET_CNUMBER_INDEX]) ^
                                                          SFU_COM_YMODEM_NEGATIVE_BYTE))
        {
          packet_size = 0U;
          status = HAL_ERROR;
        }
        else
        {
          /* Check packet CRC*/
          crc = (((uint32_t)pData[ packet_size + SFU_COM_YMODEM_PACKET_DATA_INDEX ]) << 8U) & 0x0000FF00U;
          crc += pData[ packet_size + SFU_COM_YMODEM_PACKET_DATA_INDEX + 1U ];

          /*Configure CRC with 16-bit polynomial*/
          if (SFU_LL_CRC_Config(SFU_CRC_CONFIG_16BIT) == SFU_SUCCESS)
          {
            if (SFU_LL_CRC_Calculate((uint32_t *)(uint32_t)&pData[SFU_COM_YMODEM_PACKET_DATA_INDEX], packet_size) != crc)
            {
              packet_size = 0U;
              status = HAL_ERROR;
            }

          }
          else
          {
            packet_size = 0U;
            status = HAL_ERROR;
          }
        }
      }
      else
      {
        status = HAL_ERROR;
      }
    }
  }
  else
  {
    status = HAL_ERROR;
  }

  *puLength = packet_size;
  return status;
}

/**
  * @brief  Ymodem Header Packet Transfer completed callback.
  * @param  uFileSize: Dimension of the file that will be received.
  * @retval None
  */
__weak SFU_ErrorStatus SFU_COM_YMODEM_HeaderPktRxCpltCallback(uint32_t uFileSize)
{

  /* NOTE : This function should not be modified, when the callback is needed,
            the SFU_COM_YMODEM_HeaderPktRxCpltCallback could be implemented in the user file
   */
  UNUSED(uFileSize);
  return SFU_SUCCESS;
}

/**
  * @brief  Ymodem Data Packet Transfer completed callback.
  * @param  pData: Pointer to the buffer.
  * @param  uSize: Packet dimension.
  * @retval None
  */
__weak SFU_ErrorStatus SFU_COM_YMODEM_DataPktRxCpltCallback(uint8_t *pData, uint32_t uSize)
{

  /* NOTE : This function should not be modified, when the callback is needed,
            the SFU_COM_YMODEM_DataPktRxCpltCallback could be implemented in the user file
   */
  UNUSED(pData);
  UNUSED(uSize);
  return SFU_SUCCESS;
}

#endif /* (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) || defined(SFU_TEST_PROTECTION) */