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
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "se_def.h"
#include "com.h"
#include "common.h"
#include "flash_if.h"
#include "ymodem.h"
#include "stm32wlxx_hal.h"
#include "fw_update_app.h"

#include "sfu_app_new_image.h"
#include "sfu_fwimg_regions.h"

#include "mpu.h"

#include "string.h"

#include "kms.h"
#include "pkcs11.h"
#include "kms_blob_metadata.h"

/** @addtogroup USER_APP User App Example
  * @{
  */

/** @addtogroup  FW_UPDATE Firmware Update Example
  * @{
  */
static uint32_t m_uFileSizeYmodem = 0U;  /* !< Ymodem File size*/
static uint32_t m_uNbrBlocksYmodem = 0U; /* !< Ymodem Number of blocks*/
static uint32_t m_uPacketsReceived = 0U; /* !< Ymodem packets received*/


/** @defgroup  FW_UPDATE_Private_Functions Private Functions
  * @{
  */
static void KMS_UPDATE_PrintMenu(void);
static HAL_StatusTypeDef KMS_UPDATE_ImportBlob(SFU_FwImageFlashTypeDef *pFwImageDwlArea);

HAL_StatusTypeDef KMS_UPDATE_GetDownloadAreaInfo(SFU_FwImageFlashTypeDef *pArea);
void KMS_UPDATE_StartLocalUpdate(void);


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
HAL_StatusTypeDef KMS_UPDATE_GetDownloadAreaInfo(SFU_FwImageFlashTypeDef *pArea)
{
  HAL_StatusTypeDef ret;
  if (pArea != NULL)
  {
    pArea->DownloadAddr = SlotStartAdd[SLOT_DWL_1];
    pArea->MaxSizeInBytes = (uint32_t)SLOT_SIZE(SLOT_DWL_1);
    pArea->ImageOffsetInBytes = SFU_IMG_IMAGE_OFFSET;
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
void KMS_UPDATE_StartLocalUpdate(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  uint8_t  fw_header_input[SE_FW_HEADER_TOT_LEN];
  SFU_FwImageFlashTypeDef fw_image_dwl_area;

  /* Print Firmware Update welcome message */
  KMS_UPDATE_PrintMenu();

  /* Get Info about the download area */
  if (KMS_UPDATE_GetDownloadAreaInfo(&fw_image_dwl_area) != HAL_ERROR)
  {
    /* Download new blob image*/
    ret = KMS_UPDATE_ImportBlob(&fw_image_dwl_area);

    if (HAL_OK == ret)
    {
      /* Read header in SWAP Area */
      (void)memcpy(fw_header_input, (uint32_t *)(fw_image_dwl_area.DownloadAddr), sizeof(fw_header_input));

      /* Ask for installation at next reset */
      /* FOR KMS, data are directly written to the SWAP Area. */
      /* So in the next reset, we will detect that a KMS Header is present */
      /* This will launch a call to KMS ImportBlob */

      /* System Reboot*/
      (void)printf("  -- Image correctly downloaded - reboot\r\n\n");
      HAL_Delay(1000U);
      SVC_NVIC_SystemReset();
    }
  }

  if (ret != HAL_OK)
  {
    (void)printf("  -- !!Operation failed!! \r\n\n");
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
  * @brief  Display the KMS Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
static void KMS_UPDATE_PrintMenu(void)
{
  (void)printf("\r\n================ New KMS Blob Download =========================\r\n\n");
}

/**
  * @brief Download a new BLOB from the host.
  * @retval HAL status
  */
static HAL_StatusTypeDef KMS_UPDATE_ImportBlob(SFU_FwImageFlashTypeDef *pFwImageDwlArea)
{
  HAL_StatusTypeDef ret;
  COM_StatusTypeDef e_result;
  uint32_t u_fw_size;
  YMODEM_CallbacksTypeDef ymodemCb = {Ymodem_BlobHeaderPktRxCpltCallback, Ymodem_BlobDataPktRxCpltCallback};

  (void)printf("  -- Send KMS Blob \r\n\n");
  /* Refresh Watchdog */
  WRITE_REG(IWDG->KR, IWDG_KEY_RELOAD);

  /* Clear download area */
  (void)printf("  -- -- Erasing download area ...\r\n\n");
  ret = FLASH_If_Erase_Size((uint32_t *)(pFwImageDwlArea->DownloadAddr), pFwImageDwlArea->MaxSizeInBytes);
  if (ret == HAL_OK)
  {
    /* Download binary */
    (void)printf("  -- -- File> Transfer> YMODEM> Send ");

    /*Init of Ymodem*/
    Ymodem_Init();

    /*Receive through Ymodem*/
    e_result = Ymodem_Receive(&u_fw_size, pFwImageDwlArea->DownloadAddr, &ymodemCb);
    (void)printf("\r\n\n");

    if ((e_result == COM_OK))
    {
      (void)printf("  -- -- Programming Completed Successfully!\r\n\n");
      (void)printf("  -- -- Bytes: %d\r\n\n", u_fw_size);
      ret = HAL_OK;
    }
    else if (e_result == COM_ABORT)
    {
      (void)printf("  -- -- !!Aborted by user!!\r\n\n");
      ret = HAL_ERROR;
    }
    else
    {
      (void)printf("  -- -- !!Error during file download!!\r\n\n");
      ret = HAL_ERROR;
    }
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
  uint32_t tmpSize = uSize;

  HAL_StatusTypeDef e_ret_status;

  m_uPacketsReceived++;

  /*Increase the number of received packets*/
  if (m_uPacketsReceived == m_uNbrBlocksYmodem) /*Last Packet*/
  {
    /*Extracting actual payload from last packet*/
    if (0UL == (m_uFileSizeYmodem % PACKET_1K_SIZE))
    {
      /* The last packet must be fully considered */
      tmpSize = PACKET_1K_SIZE;
    }
    else
    {
      /* The last packet is not full, drop the extra bytes */
      tmpSize = m_uFileSizeYmodem - ((uint32_t)(m_uFileSizeYmodem / PACKET_1K_SIZE) * PACKET_1K_SIZE);
    }
  }

  /* First packet : Contains the FW header */
  if (m_uPacketsReceived == 1UL)
  {
    /* End of Image to be downloaded */
    m_uDwlImgEnd = uFlashDestination + ((KMS_BlobRawHeaderTypeDef *)(uint32_t)pData)->BlobSize + KMS_BLOB_IMG_OFFSET;
    /* Initialize Current destination address for data packet */
    m_uDwlImgCurrent = uFlashDestination;
  }

  /*Adjust dimension to 64-bit length */
  if ((tmpSize % FLASH_IF_MIN_WRITE_LEN) != 0U)
  {
    tmpSize += (FLASH_IF_MIN_WRITE_LEN - (tmpSize % FLASH_IF_MIN_WRITE_LEN));
    m_uDwlImgEnd += (FLASH_IF_MIN_WRITE_LEN - (tmpSize % FLASH_IF_MIN_WRITE_LEN));
  }

  /* Write Data in Flash - size has to be 64-bit aligned */

  /* Write in flash only if not beyond allowed area */
  if ((m_uDwlImgCurrent + tmpSize) <= m_uDwlImgEnd)
  {
    if (FLASH_If_Write((uint32_t *)m_uDwlImgCurrent, pData, tmpSize) == HAL_OK)
    {
      e_ret_status = HAL_OK;
      m_uDwlImgCurrent += tmpSize;
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

  /* Last packet : reset m_uPacketsReceived */
  if (m_uPacketsReceived == m_uNbrBlocksYmodem)
  {
    m_uPacketsReceived = 0U;
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
