/**
  ******************************************************************************
  * @file    sfu_com_loader.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update COM module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SFU_COM_LOADER_H
#define SFU_COM_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  SFU_COM_LOADER_YMODEM Status structure definition
  */
typedef enum
{
  SFU_COM_YMODEM_OK       = 0x00U,  /*!< OK */
  SFU_COM_YMODEM_ERROR    = 0x01U,  /*!< Error */
  SFU_COM_YMODEM_ABORT    = 0x02U,  /*!< Abort */
  SFU_COM_YMODEM_TIMEOUT  = 0x03U,  /*!< Timeout */
  SFU_COM_YMODEM_DATA     = 0x04U,  /*!< Data */
  SFU_COM_YMODEM_LIMIT    = 0x05U   /*!< Limit*/
} SFU_COM_YMODEM_StatusTypeDef;

/* Exported constants --------------------------------------------------------*/
#ifndef MINICOM_YMODEM
/* Teraterm YMODEM */
#define SFU_COM_YMODEM_PACKET_HEADER_SIZE      ((uint32_t)3U)    /*!<Header Size*/
#define SFU_COM_YMODEM_PACKET_DATA_INDEX       ((uint32_t)4U)    /*!<Data Index*/
#define SFU_COM_YMODEM_PACKET_START_INDEX      ((uint32_t)1U)    /*!<Start Index*/
#define SFU_COM_YMODEM_PACKET_NUMBER_INDEX     ((uint32_t)2U)    /*!<Packet Number Index*/
#define SFU_COM_YMODEM_PACKET_CNUMBER_INDEX    ((uint32_t)3U)    /*!<Cnumber Index*/
#else
/* Minicom YMODEM */
#define SFU_COM_YMODEM_PACKET_HEADER_SIZE      ((uint32_t)6U)    /*!<Header Size*/
#define SFU_COM_YMODEM_PACKET_DATA_INDEX       ((uint32_t)7U)    /*!<Data Index*/
#define SFU_COM_YMODEM_PACKET_START_INDEX      ((uint32_t)4U)    /*!<Start Index*/
#define SFU_COM_YMODEM_PACKET_NUMBER_INDEX     ((uint32_t)5U)    /*!<Packet Number Index*/
#define SFU_COM_YMODEM_PACKET_CNUMBER_INDEX    ((uint32_t)6U)    /*!<Cnumber Index*/
#endif /* MINICOM_YMODEM */
#define SFU_COM_YMODEM_PACKET_TRAILER_SIZE     ((uint32_t)2U)    /*!<Trailer Size*/
#define SFU_COM_YMODEM_PACKET_OVERHEAD_SIZE    (SFU_COM_YMODEM_PACKET_HEADER_SIZE + SFU_COM_YMODEM_PACKET_TRAILER_SIZE \
                                                - 1U)           /*!<Overhead Size*/
#define SFU_COM_YMODEM_PACKET_SIZE             ((uint32_t)128U)  /*!<Packet Size*/
#define SFU_COM_YMODEM_PACKET_1K_SIZE          ((uint32_t)1024U) /*!<Packet 1K Size*/

#define SFU_COM_YMODEM_FILE_NAME_LENGTH        ((uint32_t)64U)   /*!< File name length*/
#define SFU_COM_YMODEM_FILE_SIZE_LENGTH        ((uint32_t)16U)   /*!< File size length*/

#define SFU_COM_YMODEM_SOH                     ((uint8_t)0x01U)  /*!< Start of 128-byte data packet */
#define SFU_COM_YMODEM_STX                     ((uint8_t)0x02U)  /*!< Start of 1024-byte data packet */
#define SFU_COM_YMODEM_EOT                     ((uint8_t)0x04U)  /*!< End of transmission */
#define SFU_COM_YMODEM_ACK                     ((uint8_t)0x06U)  /*!< Acknowledge */
#define SFU_COM_YMODEM_NAK                     ((uint8_t)0x15U)  /*!< Negative acknowledge */
#define SFU_COM_YMODEM_CA                      ((uint8_t)0x18U)  /*!< Two of these in succession aborts transfer */
#define SFU_COM_YMODEM_CRC16                   ((uint8_t)0x43U)  /*!< 'C' == 0x43, request 16-bit CRC */
#define SFU_COM_YMODEM_RB                      ((uint8_t)0x72U)  /*!< Startup sequence */
#define SFU_COM_YMODEM_NEGATIVE_BYTE           ((uint8_t)0xFFU)  /*!< Negative Byte*/

#define SFU_COM_YMODEM_ABORT1                  ((uint8_t)0x41U)  /*!< 'A' == 0x41, abort by user */
#define SFU_COM_YMODEM_ABORT2                  ((uint8_t)0x61U)  /*!< 'a' == 0x61, abort by user */

#define SFU_COM_YMODEM_NAK_TIMEOUT             ((uint32_t)0x100000U)  /*!< NAK Timeout*/
#ifndef MINICOM_YMODEM
/* Teraterm YMODEM */
#define SFU_COM_YMODEM_DOWNLOAD_TIMEOUT        ((uint32_t)3000U) /*!< Retry delay, has to be smaller than IWDG */
#else
/* Minicom YMODEM : reduce the download timeout to speed-up the download operation */
#define SFU_COM_YMODEM_DOWNLOAD_TIMEOUT        ((uint32_t)1000U) /*!< Retry delay, has to be smaller than IWDG */
#endif /* MINICOM_YMODEM */
#define SFU_COM_YMODEM_MAX_ERRORS              ((uint32_t)3U)    /*!< Maximum number of retry*/

/* Exported functions ------------------------------------------------------- */
/*
 * If the debug mode is not enabled then the local loader must also provide the COM init functions
 */
#if !defined(SFU_DEBUG_MODE) && !defined(SFU_TEST_PROTECTION)
SFU_ErrorStatus SFU_COM_Init(void);
SFU_ErrorStatus SFU_COM_DeInit(void);
#endif /* !SFU_DEBUG_MODE && !SFU_TEST_PROTECTION */

SFU_ErrorStatus SFU_COM_Transmit(uint8_t *pData, uint16_t uDataLength, uint32_t uTimeOut);
SFU_ErrorStatus SFU_COM_Receive(uint8_t *pData, uint16_t uDataLength);
SFU_ErrorStatus SFU_COM_Flush(void);
SFU_ErrorStatus SFU_COM_YMODEM_Receive(SFU_COM_YMODEM_StatusTypeDef *peCOMStatus, uint32_t *puSize);
SFU_ErrorStatus SFU_COM_Serial_PutByte(uint8_t uParam);

SFU_ErrorStatus SFU_COM_YMODEM_HeaderPktRxCpltCallback(uint32_t uFileSize);
SFU_ErrorStatus SFU_COM_YMODEM_DataPktRxCpltCallback(uint8_t *pData, uint32_t uSize);

#ifdef __cplusplus
}
#endif

#endif /* SFU_COM_LOADER_H */
