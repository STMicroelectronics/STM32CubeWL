/**
  ******************************************************************************
  * @file    kms_object_update_app.c
  * @author  MCD Application Team
  * @brief   Firmware Update module.
  *          This file provides set of firmware functions to manage KMS
  *          Update functionalities.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
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
#include "common.h"
#include "flash_if.h"
#include "ymodem.h"
#include "stm32wlxx_hal.h"
#include <string.h>

#include "tkms.h"

/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup  FW_UPDATE Firmware Update Example
  * @{
  */

typedef struct
{
  uint32_t  MaxSizeInBytes;        /*!< The maximum allowed size for the FwImage in User Flash (in Bytes) */
  uint32_t  DownloadAddr;          /*!< The download address for the FwImage in UserFlash */
  uint32_t  ImageOffsetInBytes;    /*!< Image write starts at this offset */
} KMS_BlobImageFlashTypeDef;

static uint32_t m_uFileSizeYmodem = 0U;  /*!< Ymodem File size*/
static uint32_t m_uNbrBlocksYmodem = 0U; /*!< Ymodem Number of blocks*/
static uint32_t m_uPacketsReceived = 0U; /*!< Ymodem packets received*/


/** @defgroup  FW_UPDATE_Private_Functions Private Functions
  * @{
  */
static void KMS_UPDATE_PrintWelcome(void);
static HAL_StatusTypeDef KMS_UPDATE_ImportBlob(KMS_BlobImageFlashTypeDef *pFwImageDwlArea);

uint32_t KMS_APP_GetDownloadAreaInfo(KMS_BlobImageFlashTypeDef *pArea);
void KMS_Start_LocalUpdate(void);


HAL_StatusTypeDef Ymodem_BlobHeaderPktRxCpltCallback(uint32_t uFileSize);
HAL_StatusTypeDef Ymodem_BlobDataPktRxCpltCallback(uint8_t *pData, uint32_t uFlashDestination, uint32_t uSize);

/**
  * @}
  */
/** @defgroup  FW_UPDATE_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup  FW_UPDATE_Control_Functions Control Functions
  * @{
   */

/**
  * @brief  Provide the area descriptor to write a KMS image in Flash.
  *         This function is used by the User Application to know where to store a new KMS Blob Image before asking for
  *         its installation.
  * @param  pArea pointer to area descriptor
  * @retval HAL_OK if successful, otherwise HAL_ERROR
  */
uint32_t KMS_APP_GetDownloadAreaInfo(KMS_BlobImageFlashTypeDef *pArea)
{
  uint32_t ret;
  if (pArea != NULL)
  {
    /* Application configured with a download area reserved in the mapping from 0x0803D000 to 0x0803DFFF */
    pArea->DownloadAddr = 0x0803D000;
    pArea->MaxSizeInBytes = (uint32_t)0x1000;
    pArea->ImageOffsetInBytes = KMS_BLOB_IMG_OFFSET;
    ret =  HAL_OK;
  }
  else
  {
    ret = HAL_ERROR;
  }
  return ret;
}


/**
  * @brief  Run KMS Blob Update process.
  * @param  None
  * @retval HAL Status.
  */
void KMS_Start_LocalUpdate(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  uint8_t  fw_header_input[KMS_BLOB_HEADER_TOT_LEN];
  KMS_BlobImageFlashTypeDef fw_image_dwl_area;
  uint32_t blob_address;
  CK_RV           e_status;

  /* Print Firmware Update welcome message */
  KMS_UPDATE_PrintWelcome();

  /* Get Info about the download area */
  if (KMS_APP_GetDownloadAreaInfo(&fw_image_dwl_area) != HAL_ERROR)
  {
    /* Download new blob image*/
    ret = KMS_UPDATE_ImportBlob(&fw_image_dwl_area);

    if (HAL_OK == ret)
    {
      /* Read header in SWAP Area */
      memcpy((void *) fw_header_input, (void *) fw_image_dwl_area.DownloadAddr, sizeof(fw_header_input));

      blob_address = fw_image_dwl_area.DownloadAddr + KMS_BLOB_HEADER_TOT_LEN;
      e_status = C_STM_ImportBlob((uint8_t *) &fw_header_input, (uint8_t *) blob_address);
      if (e_status != CKR_OK)
      {
        printf("  -- !!BLOB Import failed!! \r\n\n");
        ret = HAL_ERROR;
      }

      /* In any case, we remove the Blob To Install (regardless of the result) */
      if (FLASH_If_Erase_Size((void *)fw_image_dwl_area.DownloadAddr, fw_image_dwl_area.MaxSizeInBytes) != HAL_OK)
      {
        printf("  -- !!BLOB not erased!!\r\n");
      }
    }
  }

  if (ret != HAL_OK)
  {
    printf("  -- !!Operation failed!! \r\n\n");
    printf("  -- !!Import Blob Result [0x%8x]!! \r\n\n", e_status);
  }
  else
  {
    printf("  -- BLOB import completed successfully \r\n\n");
  }
}

/**
  * @}
  */

/**
  * @}
  */

/** @addtogroup  FW_UPDATE_Private_Functions
  * @{
  */

/**
  * @brief  Display the FW_UPDATE Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
static void KMS_UPDATE_PrintWelcome(void)
{
  printf("\r\n================ New KMS Blob Download =========================\r\n\n");
}

/**
  * @brief Download a new Firmware from the host.
  * @retval HAL status
  */
static HAL_StatusTypeDef KMS_UPDATE_ImportBlob(KMS_BlobImageFlashTypeDef *pFwImageDwlArea)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  COM_StatusTypeDef e_result;
  uint32_t u_fw_size;
  YMODEM_CallbacksTypeDef ymodemCb = {Ymodem_BlobHeaderPktRxCpltCallback, Ymodem_BlobDataPktRxCpltCallback};

  printf("  -- Send KMS Blob \r\n\n");

  /* Clear download area */
  printf("  -- -- Erasing download area ...\r\n\n");
  if ((ret = FLASH_If_Erase_Size((void *)(pFwImageDwlArea->DownloadAddr), pFwImageDwlArea->MaxSizeInBytes)) == HAL_OK)
  {
    /* Download binary */
    printf("  -- -- File> Transfer> YMODEM> Send ");

    /*Init of Ymodem*/
    Ymodem_Init();

    /*Receive through Ymodem*/
    e_result = Ymodem_Receive(&u_fw_size, pFwImageDwlArea->DownloadAddr, &ymodemCb);
    printf("\r\n\n");

    if ((e_result == COM_OK))
    {
      printf("  -- -- Programming Completed Successfully!\r\n\n");
      printf("  -- -- Bytes: %d\r\n\n", u_fw_size);
      ret = HAL_OK;
    }
    else if (e_result == COM_ABORT)
    {
      printf("  -- -- !!Aborted by user!!\r\n\n");
      ret = HAL_ERROR;
    }
    else
    {
      printf("  -- -- !!Error during file download!!\r\n\n");
      ret = HAL_ERROR;
    }

    /* Restore CRC configuration after Ymodem transfer */
    __HAL_RCC_CRC_FORCE_RESET();
    __HAL_RCC_CRC_RELEASE_RESET();
  }
  return ret;
}

/**
  * @brief  Ymodem Header Packet Transfer completed callback.
  * @param  uFileSize Dimension of the file that will be received (Bytes).
  * @retval None
  */
HAL_StatusTypeDef Ymodem_BlobHeaderPktRxCpltCallback(uint32_t uFileSize)
{
  /*Reset of the ymodem variables */
  m_uFileSizeYmodem = 0U;
  m_uPacketsReceived = 0U;
  m_uNbrBlocksYmodem = 0U;

  /*Filesize information is stored*/
  m_uFileSizeYmodem = uFileSize;

  /* compute the number of 1K blocks */
  m_uNbrBlocksYmodem = (m_uFileSizeYmodem + (PACKET_1K_SIZE - 1U)) / PACKET_1K_SIZE;

  /* NOTE : delay inserted for Ymodem protocol*/
  HAL_Delay(1000U);

  return HAL_OK;
}

/**
  * @brief  Ymodem Data Packet Transfer completed callback.
  * @param  pData Pointer to the buffer.
  * @param  uSize Packet dimension (Bytes).
  * @retval None
  */
HAL_StatusTypeDef Ymodem_BlobDataPktRxCpltCallback(uint8_t *pData, uint32_t uFlashDestination, uint32_t uSize)
{
  /* End address of downloaded Image : initialized with first packet (header) and checked along download process */
  static uint32_t m_uDwlImgEnd = 0U;
  /* Current destination address for data packet : initialized with first packet (header), incremented at each flash
     write */
  static uint32_t m_uDwlImgCurrent = 0U;

  HAL_StatusTypeDef e_ret_status = HAL_ERROR;

  m_uPacketsReceived++;

  /*Increase the number of received packets*/
  if (m_uPacketsReceived == m_uNbrBlocksYmodem) /*Last Packet*/
  {
    /*Extracting actual payload from last packet*/
    if (0 == (m_uFileSizeYmodem % PACKET_1K_SIZE))
    {
      /* The last packet must be fully considered */
      uSize = PACKET_1K_SIZE;
    }
    else
    {
      /* The last packet is not full, drop the extra bytes */
      uSize = m_uFileSizeYmodem - ((uint32_t)(m_uFileSizeYmodem / PACKET_1K_SIZE) * PACKET_1K_SIZE);
    }

    m_uPacketsReceived = 0U;
  }

  /* First packet : Contains the FW header */
  if (m_uPacketsReceived == 1)
  {
    /* End of Image to be downloaded */
    m_uDwlImgEnd = uFlashDestination + ((KMS_BlobRawHeaderTypeDef *)pData)->BlobSize + KMS_BLOB_IMG_OFFSET;
    /* Initialize Current destination address for data packet */
    m_uDwlImgCurrent = uFlashDestination;
  }

  /*Adjust dimension to 64-bit length */
  if (uSize % FLASH_IF_MIN_WRITE_LEN != 0U)
  {
    uSize += (FLASH_IF_MIN_WRITE_LEN - (uSize % FLASH_IF_MIN_WRITE_LEN));
    m_uDwlImgEnd += (FLASH_IF_MIN_WRITE_LEN - (uSize % FLASH_IF_MIN_WRITE_LEN));
  }

  /* Write Data in Flash - size has to be 64-bit aligned */

  /* Write in flash only if not beyond allowed area */
  if ((m_uDwlImgCurrent + uSize) <= m_uDwlImgEnd)
  {
    if (FLASH_If_Write((void *)m_uDwlImgCurrent, pData, uSize) == HAL_OK)
    {
      e_ret_status = HAL_OK;
      m_uDwlImgCurrent += uSize;
    }
    else
    {
      e_ret_status = HAL_ERROR;
    }
  }
  else
  {
    e_ret_status = HAL_ERROR;
  }

  if (e_ret_status == HAL_ERROR)
  {

    /*Reset of the ymodem variables */
    m_uFileSizeYmodem = 0U;
    m_uPacketsReceived = 0U;
    m_uNbrBlocksYmodem = 0U;
  }
  return e_ret_status;
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

