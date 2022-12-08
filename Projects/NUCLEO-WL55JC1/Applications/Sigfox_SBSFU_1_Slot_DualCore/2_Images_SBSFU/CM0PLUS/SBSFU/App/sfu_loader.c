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
#include "sfu_interface_crypto_scheme.h"
#include "sfu_fwimg_services.h"      /* for version checking & to check if a valid FW is installed (the local
                                        bootloader is a kind of "application" running in SB_SFU) */
#include "app_sfu.h"

#if (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER)

/* Private variables ---------------------------------------------------------*/
static uint8_t
fw_decrypted_chunk[SFU_COM_YMODEM_PACKET_1K_SIZE];/*!< Buffer used for decryption during reception process */
static uint32_t m_uDwlAreaAddress = 0U;                          /*!< Address of to write in download area */
static uint32_t m_uDwlAreaStart = 0U;                            /*!< Address of download area */
static uint32_t m_uDwlAreaSize = 0U;                             /*!< Size of download area */
static uint32_t m_uFileSizeYmodem = 0U;                          /*!< Ymodem file size being received */
static uint32_t m_uNbrBlocksYmodem = 0U;                         /*!< Number of blocks being received via Ymodem*/
static uint32_t m_uPacketsReceived = 0U;                         /*!< Number of packets received via Ymodem*/
static uint32_t m_uDwlSlot = SLOT_INACTIVE;                      /*!< Dwl slot identification : to be initialized after header check */
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
  if (0U != (uint32_t)(sizeof(fw_decrypted_chunk) % (uint32_t)sizeof(SFU_LL_FLASH_write_t)))
  {
    /* The decrypt buffer must be a multiple of the FLASH write length  */
    TRACE("\r\n= [FWIMG] Decrypt buffer size (%d) is not matching the FLASH constraints", sizeof(fw_decrypted_chunk));
    return SFU_ERROR;
  } /* else the FW Header Length is fine with regards to FLASH constraints */

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
  SE_StatusTypeDef e_se_status;
  int32_t fw_tag_len = 0;
  uint8_t fw_tag_output[SE_TAG_LEN];

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
  m_uDwlSlot = SLOT_ACTIVE_1;
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

  if (m_LoaderStatus == SFU_LOADER_OK)
  {
    /* End of decryption process */
    fw_tag_len = sizeof(fw_tag_output);
    if (SE_Decrypt_Finish(&e_se_status, fw_tag_output, &fw_tag_len) != SE_SUCCESS)
    {
      m_LoaderStatus = SFU_LOADER_ERR_CRYPTO;

    }
    else if (e_se_status != SE_OK)
    {
      m_LoaderStatus = SFU_LOADER_ERR_CRYPTO;
    }
    else
    {
      /* Nothing */
    }
  }

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
  int32_t fw_decrypted_chunk_size;
  SE_StatusTypeDef e_se_status = SE_KO;

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

      /* Downloaded Image size : Header size + Image size */
      m_uDwlImgSize = ((SE_FwRawHeaderTypeDef *)(uint32_t)fw_header)->FwSize + SFU_IMG_IMAGE_OFFSET;
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

    /* Initialize decryption process */
    if (e_ret_status == SFU_SUCCESS)
    {
      if (SE_Decrypt_Init(&e_se_status, (SE_FwRawHeaderTypeDef *)fw_header, SE_FW_IMAGE_COMPLETE) != SE_SUCCESS)
      {
        m_LoaderStatus = SFU_LOADER_ERR_CRYPTO;
        e_ret_status = SFU_ERROR;
      }
      else if (e_se_status != SE_OK)
      {
        m_LoaderStatus = SFU_LOADER_ERR_CRYPTO;
        e_ret_status = SFU_ERROR;
      }
      else
      {
        /* Store FW image header in Flash */
        if (SFU_LL_FLASH_Write(&x_flash_info, (uint8_t *)m_uDwlAreaAddress, p_data, SE_FW_HEADER_TOT_LEN) ==
            SFU_SUCCESS)
        {
          /* Shift the DWL area pointer */
          m_uDwlAreaAddress += SE_FW_HEADER_TOT_LEN;
          rx_size -= SE_FW_HEADER_TOT_LEN;
          p_data += SE_FW_HEADER_TOT_LEN;
        }
        else
        {
          m_LoaderStatus = SFU_LOADER_ERR_FLASH;
          e_ret_status = SFU_ERROR;
        }
      }
    }
  }

  if (e_ret_status == SFU_SUCCESS)
  {
    /* if the data is part of SFU_IMG_IMAGE_OFFSET nothing to do  ==> padding */
    if (m_uDwlAreaAddress < (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET))
    {
      /* all the data are located inside SFU_IMG_IMAGE_OFFSET : nothing to do  ==> full padding */
      if ((m_uDwlAreaAddress + rx_size) < (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET))
      {
        m_uDwlAreaAddress += rx_size;
        p_data += rx_size;
        rx_size = 0U;
      }
      /* part of the data is inside SFU_IMG_IMAGE_OFFSET ==> remaining bytes to be decrypted */
      else
      {
        rx_size -= (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET - m_uDwlAreaAddress);
        p_data += (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET - m_uDwlAreaAddress);
        m_uDwlAreaAddress += (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET - m_uDwlAreaAddress);
      }
    }

    /* remaining bytes to decrypt ? */
    if (rx_size != 0U)
    {
      fw_decrypted_chunk_size = rx_size;
      if ((e_ret_status == SFU_SUCCESS)
          && (SE_Decrypt_Append(&e_se_status, p_data, rx_size, fw_decrypted_chunk, &fw_decrypted_chunk_size)
              != SE_SUCCESS))
      {
        m_LoaderStatus = SFU_LOADER_ERR_CRYPTO;
        e_ret_status = SFU_ERROR;
      }
      else if ((e_se_status != SE_OK) || (fw_decrypted_chunk_size != rx_size))
      {
        m_LoaderStatus = SFU_LOADER_ERR_CRYPTO;
        e_ret_status = SFU_ERROR;
      }
      else
      {
        /*Nothing */
      }
    }
  }


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
        fw_decrypted_chunk[uOldSize] = 0xFF;
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
    if (SFU_LL_FLASH_Write(&x_flash_info, (uint8_t *)m_uDwlAreaAddress, fw_decrypted_chunk, rx_size) == SFU_SUCCESS)
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
  int32_t fw_decrypted_chunk_size;
  SE_StatusTypeDef e_se_status = SE_KO;

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
      /* Downloaded Image size : Header size + Image size */
      m_uDwlImgSize = ((SE_FwRawHeaderTypeDef *)fw_header)->FwSize + SFU_IMG_IMAGE_OFFSET;

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
        /* Initialize decryption process */
        if ((e_ret_status == SFU_SUCCESS)
            && (SE_Decrypt_Init(&e_se_status, (SE_FwRawHeaderTypeDef *)fw_header, SE_FW_IMAGE_COMPLETE) != SE_SUCCESS))
        {
          m_LoaderStatus = SFU_LOADER_ERR_CRYPTO;
          e_ret_status = SFU_ERROR;
        }
        else if (e_se_status != SE_OK)
        {
          m_LoaderStatus = SFU_LOADER_ERR_CRYPTO;
          e_ret_status = SFU_ERROR;
        }
        else
        {
          /* Write the FW header (SE_FW_HEADER_TOT_LEN bytes length) at start of DWL area */
          if (SFU_LL_FLASH_Write(&x_flash_info, (uint8_t *)m_uDwlAreaAddress, fw_header, SE_FW_HEADER_TOT_LEN)
              == SFU_SUCCESS)
          {
            /* Shift the DWL area pointer */
            m_uDwlAreaAddress += SE_FW_HEADER_TOT_LEN;
          }
          else
          {
            m_LoaderStatus = SFU_LOADER_ERR_FLASH;
            e_ret_status = SFU_ERROR;
          }
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
     * if the data is part of SFU_IMG_IMAGE_OFFSET nothing to do  ==> padding
     */
    if (m_uDwlAreaAddress < (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET))
    {
      if ((m_uDwlAreaAddress + rx_size) < (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET))
      {
        /* all the data are located inside SFU_IMG_IMAGE_OFFSET : nothing to do  ==> full padding */
        m_uDwlAreaAddress += rx_size;
        p_data += rx_size;
        rx_size = 0U;
      }
      else
      {
        /* part of the data is inside SFU_IMG_IMAGE_OFFSET ==> remaining bytes to be decrypted */
        rx_size -= (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET - m_uDwlAreaAddress);
        p_data += (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET - m_uDwlAreaAddress);
        m_uDwlAreaAddress += (m_uDwlAreaStart + SFU_IMG_IMAGE_OFFSET - m_uDwlAreaAddress);
      }
    }

    /* remaining bytes to decrypt ? */
    if (rx_size != 0)
    {
      fw_decrypted_chunk_size = rx_size;
      if ((e_ret_status == SFU_SUCCESS)
          && (SE_Decrypt_Append(&e_se_status, p_data, rx_size, fw_decrypted_chunk, &fw_decrypted_chunk_size)
              != SE_SUCCESS))
      {
        m_LoaderStatus = SFU_LOADER_ERR_CRYPTO;
        e_ret_status = SFU_ERROR;
      }
      else if ((e_se_status != SE_OK) || (fw_decrypted_chunk_size != rx_size))
      {
        m_LoaderStatus = SFU_LOADER_ERR_CRYPTO;
        e_ret_status = SFU_ERROR;
      }
      else
      {
        /*Nothing */
      }

      /* Check size to avoid writing beyond DWL image size */
      if ((m_uDwlAreaAddress + rx_size) > (m_uDwlAreaStart + m_uDwlImgSize))
      {
        m_LoaderStatus = SFU_LOADER_ERR_FW_LENGTH;
        e_ret_status = SFU_ERROR;
      }

      /* Set dimension to the appropriate length for FLASH programming.
       * Example: 64-bit length for L4.
       */
      if ((rx_size % (uint32_t)sizeof(SFU_LL_FLASH_write_t)) != 0)
      {
        /* By construction, the length of the buffer (fw_decrypted_chunk or p_data) must be a multiple of
           sizeof(SFU_IMG_write_t) to avoid reading out of the buffer */
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
        if (SFU_LL_FLASH_Write(&x_flash_info, (uint8_t *)m_uDwlAreaAddress, fw_decrypted_chunk, rx_size) == SFU_SUCCESS)
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
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SE_StatusTypeDef              e_se_status;
  SE_FwRawHeaderTypeDef         *p_x_fw_raw_header;
  SFU_FwImageFlashTypeDef       fw_image_flash_data;
  uint16_t                      cur_ver;
  uint16_t                      verif_ver;
  uint32_t                      active_slot;

  /*Parse the received buffer*/
  p_x_fw_raw_header = (SE_FwRawHeaderTypeDef *)(uint32_t)pBuffer;
  /*Check if the received header packet is authentic*/
  if (SE_VerifyHeaderSignature(&e_se_status, p_x_fw_raw_header) != SE_ERROR)
  {
    active_slot = SFU_IMG_GetFwImageSlot(p_x_fw_raw_header);
    /*
     * 1 slot per image configuration. Identify the download area.
     *   It will be based on SFU magic :
     *   - dwl slot is the active_slot if configured.
     *   - else : SLOT_ACTIVE_1
     */
    m_uDwlSlot = active_slot;
    if (SLOT_SIZE(m_uDwlSlot) == 1U)
    {
      m_uDwlSlot = SLOT_ACTIVE_1;
    }
    SFU_IMG_GetDownloadAreaInfo(m_uDwlSlot, &fw_image_flash_data);

    /* Assign the download flash address to be used during the YMODEM process */
    m_uDwlAreaStart = fw_image_flash_data.DownloadAddr;
    m_uDwlAreaSize = fw_image_flash_data.MaxSizeInBytes;

    /*
     * Check if the version is allowed
     * ==> SFU_IMG_GetActiveFwVersion() returns 0 if the header of active firmware is not valid
     */
    cur_ver = SFU_IMG_GetActiveFwVersion(active_slot);
    if (SFU_IMG_CheckFwVersion(active_slot, cur_ver, p_x_fw_raw_header->FwVersion) == SFU_SUCCESS)
    {
      /*
       * Check length : Let's make sure the Firmware image can be written in the destination slot.
       */
      if (p_x_fw_raw_header->FwSize > (SLOT_SIZE(m_uDwlSlot) - SFU_IMG_IMAGE_OFFSET))
      {
        /* The firmware cannot be written in dwl slot */
        m_LoaderStatus = SFU_LOADER_ERR_FW_LENGTH;
      }
      else
      {
        /* Installation authorized */
        e_ret_status = SFU_SUCCESS;
      }
    }
    else
    {
      /* The installation is forbidden */
      TRACE("\r\n          Anti-rollback: candidate version(%d) rejected | current version(%d) , min.version(%d) !",
            p_x_fw_raw_header->FwVersion, cur_ver, SFU_FW_VERSION_START_NUM);
      m_LoaderStatus = SFU_LOADER_ERR_FW_VERSION;
    }


    /*
     * Control of version and length is done twice to avoid basic hardware attack
     * This more important for the 1 image configuration as the installation is done during the download procedure
     * For the 2 images configuration anti-rollback will be checked again during installation process
     */
    verif_ver = SFU_IMG_GetActiveFwVersion(active_slot);
    if (SFU_IMG_CheckFwVersion(active_slot, verif_ver, p_x_fw_raw_header->FwVersion) == SFU_SUCCESS)
    {
      /*
       * Check length : Let's make sure the Firmware image can be written in the destination slot.
       */
      if (p_x_fw_raw_header->FwSize > (SLOT_SIZE(m_uDwlSlot) - SFU_IMG_IMAGE_OFFSET))
      {
        /* The firmware cannot be written in dwl slot */
        m_LoaderStatus = SFU_LOADER_ERR_FW_LENGTH;
      }
      else
      {
        /* Installation authorized */
        e_ret_status = SFU_SUCCESS;
      }
    }
    else
    {
      /* The installation is forbidden */
      TRACE("\r\n          Anti-rollback: candidate version(%d) rejected | current version(%d) , min.version(%d) !",
            p_x_fw_raw_header->FwVersion, verif_ver, SFU_FW_VERSION_START_NUM);
      m_LoaderStatus = SFU_LOADER_ERR_FW_VERSION;
    }
  }
  else
  {
    /* Header authentication failure */
    m_LoaderStatus = SFU_LOADER_ERR_AUTH_FAILED;
  }

  return e_ret_status;
}

#endif /* (SECBOOT_LOADER == SECBOOT_USE_LOCAL_LOADER) */