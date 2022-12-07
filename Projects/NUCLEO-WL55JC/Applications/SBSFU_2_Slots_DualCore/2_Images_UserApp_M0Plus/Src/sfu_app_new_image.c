/**
  ******************************************************************************
  * @file    sfu_app_new_image.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage the New Firmware
  *          Image storage and installation.
  *          This file contains the services the user application can use to
  *          know where to store a new FW image and request its installation.
  *          The same services are offered to the local loader thanks to a similar
  *          file integrated in SB_SFU.
  * @note    This file is compiled in the scope of the User Application.
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

#define SFU_APP_NEW_IMAGE_C

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sfu_def.h"
#include "flash_if.h"
#include "sfu_app_new_image.h"
#include "sfu_fwimg_regions.h"
#include "se_def_metadata.h"
#include <string.h> /* needed for memset (see WriteInstallHeader)*/

/* Functions Definition ------------------------------------------------------*/

#if  !defined(SFU_NO_SWAP)
/**
  * @brief  Write the header of the firmware to install
  * @param  pfw_header pointer to header to write.
  * @retval HAL_OK on success otherwise HAL_ERROR
  */
static HAL_StatusTypeDef WriteInstallHeader(uint8_t *pfw_header)
{
  HAL_StatusTypeDef ret = HAL_OK;

  ret = FLASH_If_Erase_Size((void *) SlotStartAdd[SLOT_SWAP], SFU_IMG_IMAGE_OFFSET);
  if (ret == HAL_OK)
  {
    ret = FLASH_If_Write((void *)SlotStartAdd[SLOT_SWAP], pfw_header, SE_FW_HEADER_TOT_LEN);
  }
  return ret;
}
#endif /* !SFU_NO_SWAP */

/**
  * @brief  Write in Flash the next header image to install.
  *         This function is used by the User Application to request a Firmware installation (at next reboot).
  * @param  fw_header FW header of the FW to be installed
  * @retval HAL_OK if successful, otherwise HAL_ERROR
  */
HAL_StatusTypeDef SFU_APP_InstallAtNextReset(uint8_t *fw_header)
{
#if  !defined(SFU_NO_SWAP)
  if (fw_header == NULL)
  {
    return HAL_ERROR;
  }
  if (WriteInstallHeader(fw_header) != HAL_OK)
  {
    return HAL_ERROR;
  }
  return HAL_OK;
#else
  return HAL_OK;                   /* Nothing to do */
#endif /* !SFU_NO_SWAP */
}

/**
  * @brief  Provide the area descriptor to write a FW image in Flash.
  *         This function is used by the User Application to know where to store a new Firmware Image before asking for
  *         its installation.
  * @param  DwlSlot identification of the downloaded area
  * @param  pArea pointer to area descriptor
  * @retval HAL_OK if successful, otherwise HAL_ERROR
  */

void SFU_APP_GetDownloadAreaInfo(uint32_t DwlSlot, SFU_FwImageFlashTypeDef *pArea)
{
  pArea->DownloadAddr = SlotStartAdd[DwlSlot];
  pArea->MaxSizeInBytes = (uint32_t)SLOT_SIZE(DwlSlot);
  pArea->ImageOffsetInBytes = SFU_IMG_IMAGE_OFFSET;
}

#undef SFU_APP_NEW_IMAGE_C
