/**
  ******************************************************************************
  * @file    sfu_loader.c
  * @author  MCD Application Team
  * @brief   Secure Firmware Update LOADER module.
  *          This file provides set of firmware functions to manage SFU local
  *          loader functionalities.
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

#define SFU_LOADER_C

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "sfu_loader.h"
#include "sfu_low_level_flash_int.h"
#include "sfu_low_level_security.h"
#include "sfu_com_loader.h"
#include "sfu_trace.h"
#include "se_interface_bootloader.h" /* for metadata authentication */
#include "app_sfu.h"

#if (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER)

/* Private variables ---------------------------------------------------------*/
#ifdef MINICOM_YMODEM
/*
 * With Minicom Ymodem, the data offset is not aligned because SFU_COM_YMODEM_PACKET_DATA_INDEX is 7
 * So we cannot write in FLASH with p_data as source, we need a copy in an aligned buffer
 */
/* With Minicom YMODEM each packet carries 128 bytes of data */
static uint8_t alignedBuffer[SFU_COM_YMODEM_PACKET_SIZE] __attribute__((aligned(8)));
#endif /* MINICOM_YMODEM */
static uint32_t m_uDwlAreaAddress = 0U;                          /*!< Address of to write in download area */
static uint32_t m_uDwlAreaStart = 0U;                            /*!< Address of download area */
static uint32_t m_uDwlAreaSize = 0U;                             /*!< Size of download area */
static uint32_t m_uFileSizeYmodem = 0U;                          /*!< Ymodem file size being received */
static uint32_t m_uNbrBlocksYmodem = 0U;                         /*!< Number of blocks being received via Ymodem*/
static uint32_t m_uPacketsReceived = 0U;                         /*!< Number of packets received via Ymodem*/
static uint32_t m_uDwlSlot = SLOT_INACTIVE;                      /*!< Dwl slot identification : to be intialialized after header check */
static SFU_LOADER_StatusTypeDef m_LoaderStatus;                  /*!< Status of the loader */

/* Private function prototypes -----------------------------------------------*/
static SFU_ErrorStatus SFU_LOADER_VerifyFwHeader(uint8_t *pBuffer);

/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  Initialize the SFU LOADER.
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LOADER_Init(void)
{
  /*
  * ADD SRC CODE HERE
  */

  /*
   * Sanity check to make sure that the local loader cannot read out of the buffer bounds
   * when doing a length alignment before writing in FLASH.
   */
#ifndef MINICOM_YMODEM
  /* m_aPacketData contains SFU_COM_YMODEM_PACKET_1K_SIZE bytes of payload */
  if (0U != (uint32_t)(SFU_COM_YMODEM_PACKET_1K_SIZE % (uint32_t)sizeof(SFU_LL_FLASH_write_t)))
  {
    /* The packet buffer (payload part) must be a multiple of the FLASH write length  */
    TRACE("\r\n= [FWIMG] Packet Payload size (%d) is not matching the FLASH constraints",
          SFU_COM_YMODEM_PACKET_1K_SIZE);
    return SFU_ERROR;
  } /* else the FW Header Length is fine with regards to FLASH constraints */
#else /* MINICOM_YMODEM */
  /* m_aPacketData contains SFU_COM_YMODEM_PACKET_SIZE bytes of payload */
  if (0U != (uint32_t)(SFU_COM_YMODEM_PACKET_SIZE % (uint32_t)sizeof(SFU_LL_FLASH_write_t)))
  {
    /* The packet buffer (payload part) must be a multiple of the FLASH write length  */
    TRACE("\r\n= [FWIMG] Packet Payload size (%d) is not matching the FLASH constraints", SFU_COM_YMODEM_PACKET_SIZE);
    return SFU_ERROR;
  }
#endif /* MINICOM_YMODEM */

  return SFU_SUCCESS;
}

/**
  * @brief  DeInitialize the SFU LOADER.
  * @param  None
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LOADER_DeInit(void)
{
  /*
   * ADD SRC CODE HERE
   */
  return SFU_SUCCESS;
}

/**
  * @brief  Download a new User Fw.
  *         Writes firmware received via Ymodem in FLASH.
  * @param  peSFU_LOADER_Status: SFU LOADER Status.
  *         This parameter can be a value of @ref SFU_LOADER_Status_Structure_definition.
  * @param  p_DwlSlot identification of the downloaded area
  * @param  p_Size Size of the downloaded image.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_LOADER_DownloadNewUserFw(SFU_LOADER_StatusTypeDef *peSFU_LOADER_Status, uint32_t *pDwlSlot,
                                             uint32_t *pSize)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SFU_COM_YMODEM_StatusTypeDef e_com_status = SFU_COM_YMODEM_ERROR;

  /* Check the pointers allocation */
  if ((peSFU_LOADER_Status == NULL) || (pSize == NULL) || (pDwlSlot == NULL))
  {
    return SFU_ERROR;
  }

  /* Refresh Watchdog */
  (void) SFU_LL_SECU_IWDG_Refresh();

  /* Transfer FW Image via YMODEM protocol */
  TRACE("\r\n\t  File> Transfer> YMODEM> Send ");

  /* Initialize global variables to be used during the YMODEM process */
  m_uDwlAreaAddress = 0U;
  m_uDwlAreaStart = 0U;
  m_uDwlAreaSize = 0U;
  m_uFileSizeYmodem = 0U;
  m_uNbrBlocksYmodem = 0U;
  m_uPacketsReceived = 0U;
  m_uDwlSlot = SLOT_DWL_1;
  m_LoaderStatus = SFU_LOADER_ERR_COM;

  /* Receive the FW in RAM and write it in the Flash*/
  if (SFU_COM_YMODEM_Receive(&e_com_status, pSize) == SFU_SUCCESS)
  {
    if (*pSize > 0U)
    {
      /* File download correct*/
      m_LoaderStatus = SFU_LOADER_OK;
    }
  }
  else
  {
    /* Nothing to do : m_LoaderStatus updated during during YModem process */
  }

  /* Provide slot information to the caller
     m_uDwlSlot updated during VerifyFwHeader() except in case of bad formatted file
     ==> m_uDwlSlot stick at SLOT_DWL_1 value if the header signature is not verified */
  *pDwlSlot = m_uDwlSlot;


  *peSFU_LOADER_Status = m_LoaderStatus;
  if (m_LoaderStatus == SFU_LOADER_OK)
  {
    e_ret_status = SFU_SUCCESS;
  }
  return e_ret_status;
}

/**
  * @brief  Ymodem Header Packet Transfer completed callback.
  * @param  uFileSize: Dimension of the file that will be received.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_COM_YMODEM_HeaderPktRxCpltCallback(uint32_t uFileSize)
{
  /*Reset of the ymodem variables */
  m_uFileSizeYmodem = 0U;
  m_uPacketsReceived = 0U;
  m_uNbrBlocksYmodem = 0U;

  /*Filesize information is stored*/
  m_uFileSizeYmodem = uFileSize;

  /*Compute the number of blocks */
#ifndef MINICOM_YMODEM
  /* Teraterm sends 1kB YMODEM packets */
  m_uNbrBlocksYmodem = (m_uFileSizeYmodem + (SFU_COM_YMODEM_PACKET_1K_SIZE - 1U)) / SFU_COM_YMODEM_PACKET_1K_SIZE;
#else
  /* Minicom sends 128 bytes YMODEM packets */
  m_uNbrBlocksYmodem = (m_uFileSizeYmodem + (SFU_COM_YMODEM_PACKET_SIZE - 1U)) / SFU_COM_YMODEM_PACKET_SIZE;
#endif /* MINICOM_YMODEM */

  /* NOTE : delay inserted for Ymodem protocol*/
  HAL_Delay(1000U);

  return SFU_SUCCESS;
}

/**
  * @brief  Ymodem Data Packet Transfer completed callback.
  * @param  pData: Pointer to the buffer.
  * @param  uSize: Packet dimension.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
#ifndef MINICOM_YMODEM
/* Teraterm variant of the YMODEM protocol: uses 1kB packets */
SFU_ErrorStatus SFU_COM_YMODEM_DataPktRxCpltCallback(uint8_t *pData, uint32_t uSize)
{
  /* The local loader must make a copy of the Firmware metadata,
   * because this memory area is not copied when calling the SE_Decrypt_Init() primitive.
   * Hence we must make sure this memory area still contains the FW header when SE_Decrypt_Finish() is called. */
  static uint8_t fw_header[SE_FW_HEADER_TOT_LEN] __attribute__((aligned(8)));
  /* Size of downloaded Image initialized with first packet (header) and checked along download process */
  static uint32_t m_uDwlImgSize = 0U;

  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  SFU_FLASH_StatusTypeDef x_flash_info;
  uint32_t rx_size = uSize;
  uint8_t *p_data = pData;
  uint32_t uOldSize;
#if  !defined(SFU_NO_SWAP)
  uint32_t uLength;
#endif /* !SFU_NO_SWAP */

  /* Check the pointers allocation */
  if (p_data == NULL)
  {
    return SFU_ERROR;
  }

  /*Increase the number of received packets*/
  m_uPacketsReceived++;

  /* Last packet : size of data to write could be different than SFU_COM_YMODEM_PACKET_1K_SIZE */
  if (m_uPacketsReceived == m_uNbrBlocksYmodem)
  {
    /*Extracting actual payload from last packet*/
    if (0U == (m_uFileSizeYmodem % SFU_COM_YMODEM_PACKET_1K_SIZE))
    {
      /* The last packet must be fully considered */
      rx_size = SFU_COM_YMODEM_PACKET_1K_SIZE;
    }
    else
    {
      /* The last packet is not full, drop the extra bytes */
      rx_size = m_uFileSizeYmodem - ((uint32_t)(m_uFileSizeYmodem / SFU_COM_YMODEM_PACKET_1K_SIZE) *
                                     SFU_COM_YMODEM_PACKET_1K_SIZE);
    }
  }

  /* First packet : Contains the FW header (SE_FW_HEADER_TOT_LEN bytes length) which is not encrypted  */
  if (m_uPacketsReceived == 1U)
  {

    (void) memcpy(fw_header, p_data, SE_FW_HEADER_TOT_LEN);

    /* Verify header */
    e_ret_status = SFU_LOADER_VerifyFwHeader(p_data);
    if (e_ret_status == SFU_SUCCESS)
    {
      m_uDwlAreaAddress = m_uDwlAreaStart;
#if   defined(SFU_NO_SWAP)

      /* Downloaded Image size : Header size + Image size */
      m_uDwlImgSize = ((SE_FwRawHeaderTypeDef *)(uint32_t)fw_header)->FwSize + SFU_IMG_IMAGE_OFFSET;
#else
      /* Downloaded Image size : Header size + gap for image alignment to (UpdateFwOffset % sector size) +
         Partial Image size */
      m_uDwlImgSize = ((SE_FwRawHeaderTypeDef *)(uint32_t)fw_header)->PartialFwSize +
                      (((SE_FwRawHeaderTypeDef *)(uint32_t)fw_header)->PartialFwOffset % SLOT_SIZE(SLOT_SWAP)) +
                      SFU_IMG_IMAGE_OFFSET;
#endif /* (SFU_NO_SWAP) */
    }

    /* Clear Download application area (including TRAILERS area) */
    if (e_ret_status == SFU_SUCCESS)
    {
      if (SFU_LL_FLASH_Erase_Size(&x_flash_info, (uint8_t *) m_uDwlAreaAddress, SLOT_SIZE(m_uDwlSlot)) !=
          SFU_SUCCESS)
      {
        m_LoaderStatus = SFU_LOADER_ERR_FLASH;
        e_ret_status = SFU_ERROR;
      }
    }

  }

#if  !defined(SFU_NO_SWAP)
  if (e_ret_status == SFU_SUCCESS)
  {
    /* This packet : contains end of FW header */
    if ((m_uDwlAreaAddress < (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET)) &&
        ((m_uDwlAreaAddress + rx_size) >= (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET)))
    {
      /* Write the FW header part (SFU_IMG_IMAGE_OFFSET % SFU_COM_YMODEM_PACKET_1K_SIZE bytes length) */
      uLength = SFU_IMG_IMAGE_OFFSET % SFU_COM_YMODEM_PACKET_1K_SIZE;
      if (uLength == 0U)
      {
        uLength = SFU_COM_YMODEM_PACKET_1K_SIZE;
      }
      if (SFU_LL_FLASH_Write(&x_flash_info, (uint8_t *)m_uDwlAreaAddress, p_data, uLength) == SFU_SUCCESS)
      {
        /* Shift the DWL area pointer, to align image with (PartialFwOffset % sector size) in DWL area */
        m_uDwlAreaAddress += uLength + (((SE_FwRawHeaderTypeDef *)(uint32_t)fw_header)->PartialFwOffset %
                                        SLOT_SIZE(SLOT_SWAP));

        /* Update remaining packet size to write */
        rx_size -= uLength;

        /* Update p_data pointer to received packet data */
        p_data += uLength;
      }
      else
      {
        m_LoaderStatus = SFU_LOADER_ERR_FLASH;
        e_ret_status = SFU_ERROR;
      }
    }
  }
#endif /* !SFU_NO_SWAP */

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Check size to avoid writing beyond DWL image size */
    if ((m_uDwlAreaAddress + rx_size) > (m_uDwlAreaStart + m_uDwlImgSize))
    {
      m_LoaderStatus = SFU_LOADER_ERR_FW_LENGTH;
      e_ret_status = SFU_ERROR;
    }
  }

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Set dimension to the appropriate length for FLASH programming.
     * Example: 64-bit length for L4.
     */
    if ((rx_size % (uint32_t)sizeof(SFU_LL_FLASH_write_t)) != 0U)
    {
      /* By construction, the length of the buffer (fw_decrypted_chunk or p_data) must be a multiple of
         sizeof(SFU_IMG_write_t) to avoid reading out of the buffer */
      uOldSize = rx_size;
      rx_size = rx_size + ((uint32_t)sizeof(SFU_LL_FLASH_write_t) - (rx_size % (uint32_t)sizeof(SFU_LL_FLASH_write_t)));
      while (uOldSize < rx_size)
      {
        p_data[uOldSize] = 0xFF;
        uOldSize++;
      }
    }
  }

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Check size to avoid writing beyond DWL area */
    if ((m_uDwlAreaAddress + rx_size) > (m_uDwlAreaStart + m_uDwlAreaSize))
    {
      m_LoaderStatus = SFU_LOADER_ERR_FW_LENGTH;
      e_ret_status = SFU_ERROR;
    }
  }

  /* Write Data in Flash */
  if (e_ret_status == SFU_SUCCESS)
  {
    if (SFU_LL_FLASH_Write(&x_flash_info, (uint8_t *)m_uDwlAreaAddress, p_data, rx_size) == SFU_SUCCESS)
    {
      m_uDwlAreaAddress += (rx_size);
    }
    else
    {
      m_LoaderStatus = SFU_LOADER_ERR_FLASH;
      e_ret_status = SFU_ERROR;
    }
  }


  /* Last packet : reset m_uPacketsReceived */
  if (m_uPacketsReceived == m_uNbrBlocksYmodem)
  {
    m_uPacketsReceived = 0U;
  }

  /* Reset data counters in case of error */
  if (e_ret_status == SFU_ERROR)
  {
    /*Reset of the ymodem variables */
    m_uFileSizeYmodem = 0U;
    m_uPacketsReceived = 0U;
    m_uNbrBlocksYmodem = 0U;
  }

  return e_ret_status;
}
#else /* MINICOM_YMODEM */
/* Minicom variant of the YMODEM protocol: uses 128B packets */
SFU_ErrorStatus SFU_COM_YMODEM_DataPktRxCpltCallback(uint8_t *pData, uint32_t uSize)
{
  /* The local loader must make a copy of the Firmware metadata,
   * because this memory area is not copied when calling the SE_Decrypt_Init() primitive.
   * Hence we must make sure this memory area still contains the FW header when SE_Decrypt_Finish() is called. */
  static uint8_t fw_header[SE_FW_HEADER_TOT_LEN] __attribute__((aligned(8)));
  /* Size of downloaded Image initialized with first packet (header) and checked along download process */
  static uint32_t m_uDwlImgSize = 0U;

  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;
  SFU_FLASH_StatusTypeDef x_flash_info;
  uint32_t rx_size = uSize;
  uint8_t *p_data = pData;
  uint32_t uOldSize;

  /* Check the pointers allocation */
  if (p_data == NULL)
  {
    return SFU_ERROR;
  }

  /*Increase the number of received packets*/
  m_uPacketsReceived++;

  /* Last packet : size of data to write could be different than SFU_COM_YMODEM_PACKET_1K_SIZE */
  if (m_uPacketsReceived == m_uNbrBlocksYmodem)
  {
    /*Extracting actual payload from last packet: minicom works by default with 128B packets */
    if (0 == (m_uFileSizeYmodem % SFU_COM_YMODEM_PACKET_SIZE))
    {
      /* The last packet must be fully considered */
      rx_size = SFU_COM_YMODEM_PACKET_SIZE;
    }
    else
    {
      /* The last packet is not full, drop the extra bytes: minicom works by default with 128B packets */
      rx_size = m_uFileSizeYmodem - ((uint32_t)(m_uFileSizeYmodem / SFU_COM_YMODEM_PACKET_SIZE) *
                                     SFU_COM_YMODEM_PACKET_SIZE);
    }
  }


  /*
   * Minicom splits the header of 192 bytes on 2 packets of 128 bytes.
   */

  /* First packet : Contains the first 128B of FW header (out of SE_FW_HEADER_TOT_LEN bytes length) which is
     not encrypted  */
  if (1 == m_uPacketsReceived)
  {
    /* First packet: beginning of header */
    if (SFU_COM_YMODEM_PACKET_SIZE == rx_size)
    {
      /* Copy the beginning of the header */
      memcpy(fw_header, p_data, rx_size);
      rx_size = 0U; /* nothing more to do */
    }
    else
    {
      /* Unexpected case */
      e_ret_status = SFU_ERROR;
    }
  }
  else if (2U == m_uPacketsReceived)
  {
    /* Second packet: end of header (64 bytes) */
    memcpy(fw_header + SFU_COM_YMODEM_PACKET_SIZE, p_data, 64U);
    rx_size -= 64U;
    p_data += 64U;

    /* Verify header */
    e_ret_status = SFU_LOADER_VerifyFwHeader(fw_header);

    if (e_ret_status == SFU_SUCCESS)
    {
      m_uDwlAreaAddress = m_uDwlAreaStart;
      /* Downloaded Image size : Header size + gap for image alignment to (UpdateFwOffset % sector size) +
         Partial Image size */
      m_uDwlImgSize = ((SE_FwRawHeaderTypeDef *)fw_header)->PartialFwSize +
                      (((SE_FwRawHeaderTypeDef *)fw_header)->PartialFwOffset % SLOT_SIZE(SLOT_SWAP)) +
                      SFU_IMG_IMAGE_OFFSET;

      /* Clear Download application area (including TRAILERS area) */
      if ((e_ret_status == SFU_SUCCESS)
          && (SFU_LL_FLASH_Erase_Size(&x_flash_info, (uint8_t *) m_uDwlAreaAddress, SLOT_SIZE(m_uDwlSlot)) !=
              SFU_SUCCESS))
      {
        m_LoaderStatus = SFU_LOADER_ERR_FLASH;
        e_ret_status = SFU_ERROR;
      }

      if (e_ret_status == SFU_SUCCESS)
      {
        /* Write the FW header (SE_FW_HEADER_TOT_LEN bytes length) at start of DWL area */
        if (SFU_LL_FLASH_Write(&x_flash_info, (uint8_t *)m_uDwlAreaAddress, fw_header, SE_FW_HEADER_TOT_LEN)
            == SFU_SUCCESS)
        {
          /* Shift the DWL area pointer (still to be aligned image with (PartialFwOffset % sector size) in DWL area ) */
          m_uDwlAreaAddress += SE_FW_HEADER_TOT_LEN;
        }
        else
        {
          m_LoaderStatus = SFU_LOADER_ERR_FLASH;
          e_ret_status = SFU_ERROR;
        }
      }
      /* The remaining 64 bytes of data must be handled : standard processing */
    }
  }
  /*
   * Else: standard packet data processing based on rx_size (full packet or the last 64 bytes of the 2nd packet)
   */
  if (e_ret_status == SFU_SUCCESS)
  {
    /*
     * End of the second packet (64B) or full 128B packet
     * Skip padding bytes then take partial FW offset into account
     */
    if (m_uDwlAreaAddress < (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET))
    {
      /* Skip the padding bytes */
      m_uDwlAreaAddress += rx_size;
      rx_size = 0U;
    }
    else
    {
      /*
       * m_uDwlAreaAddress >= (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET)
       */
      if (((SFU_IMG_IMAGE_OFFSET / SFU_COM_YMODEM_PACKET_SIZE) + 1) == m_uPacketsReceived)
      {
        /* End of padding bytes : We must take the partial offset into account */
        m_uDwlAreaAddress += ((SE_FwRawHeaderTypeDef *)fw_header)->PartialFwOffset % SLOT_SIZE(SLOT_SWAP);
      }
      /* else nothing more to do */

      /* Check size to avoid writing beyond DWL image size */
      if ((m_uDwlAreaAddress + rx_size) > (m_uDwlAreaStart + m_uDwlImgSize))
      {
        m_LoaderStatus = SFU_LOADER_ERR_FW_LENGTH;
        e_ret_status = SFU_ERROR;
      }

      /*
       * Set dimension to the appropriate length for FLASH programming.
       * Example: 64-bit length for L4.
       */
      if ((rx_size % (uint32_t)sizeof(SFU_LL_FLASH_write_t)) != 0)
      {
        /*
         * By construction, the length of the buffer (fw_decrypted_chunk or p_data) must be a multiple of
         * sizeof(SFU_IMG_write_t) to avoid reading out of the buffer
         */
        uOldSize = rx_size;
        rx_size = rx_size + ((uint32_t)sizeof(SFU_LL_FLASH_write_t) -
                             (rx_size % (uint32_t)sizeof(SFU_LL_FLASH_write_t)));
        while (uOldSize < rx_size)
        {
          p_data[uOldSize] = 0xFF;
          uOldSize++;
        }
      }

      /* Check size to avoid writing beyond DWL area */
      if ((m_uDwlAreaAddress + rx_size) > (m_uDwlAreaStart + m_uDwlAreaSize))
      {
        m_LoaderStatus = SFU_LOADER_ERR_FW_LENGTH;
        e_ret_status = SFU_ERROR;
      }

      /* Write Data in Flash */
      if (e_ret_status == SFU_SUCCESS)
      {
        /*
         * With Minicom Ymodem, the data offset is not aligned because SFU_COM_YMODEM_PACKET_DATA_INDEX is 7
         * So we cannot write in FLASH with p_data as source, we need a copy in an aligned buffer
         */
        memcpy(alignedBuffer, p_data, rx_size);

        if (SFU_LL_FLASH_Write(&x_flash_info, (uint8_t *)m_uDwlAreaAddress, alignedBuffer, rx_size) == SFU_SUCCESS)
        {
          m_uDwlAreaAddress += (rx_size);
        }
        else
        {
          m_LoaderStatus = SFU_LOADER_ERR_FLASH;
          e_ret_status = SFU_ERROR;
        }
      }
    }
  }


  /* Last packet : reset m_uPacketsReceived */
  if (m_uPacketsReceived == m_uNbrBlocksYmodem)
  {
    m_uPacketsReceived = 0U;
  }

  /* Reset data counters in case of error */
  if (e_ret_status == SFU_ERROR)
  {
    /*Reset of the ymodem variables */
    m_uFileSizeYmodem = 0U;
    m_uPacketsReceived = 0U;
    m_uNbrBlocksYmodem = 0U;
  }

  return e_ret_status;
}
#endif /* MINICOM_YMODEM */


/**
  * @brief  Verifies the Raw Fw Header received. It checks if the header is
  *         authentic and if the fields are ok with the device (e.g. size and version).
  * @note   Please note that when the new image is installed, this metadata is checked
  *         by @ref SFU_IMG_CheckCandidateVersion.
  * @param  peSFU_LOADER_Status: SFU LOADER Status.
  *         This parameter can be a value of @ref SFU_LOADER_Status_Structure_definition.
  * @param  pBuffer: pointer to header Buffer.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */

/* anti-rollback mechanism is tested twice to avoid basic hardware attack
   ==> compiler optimization is disabled for this reason */
#if defined(__ICCARM__)
#pragma optimize=none
#elif defined(__CC_ARM)
#pragma O0
#elif defined(__ARMCC_VERSION)
__attribute__((optnone))
#elif defined(__GNUC__)
__attribute__((optimize("O0")))
#endif /* __ICCARM__ */

static SFU_ErrorStatus SFU_LOADER_VerifyFwHeader(uint8_t *pBuffer)
{
  SFU_FwImageFlashTypeDef       fw_image_flash_data;

  m_uDwlSlot = SLOT_DWL_1;

  SFU_IMG_GetDownloadAreaInfo(m_uDwlSlot, &fw_image_flash_data);

  /* Assign the download flash address to be used during the YMODEM process */
  m_uDwlAreaStart = fw_image_flash_data.DownloadAddr;
  m_uDwlAreaSize = fw_image_flash_data.MaxSizeInBytes;

  return SFU_SUCCESS;
}

#endif /* (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) */
