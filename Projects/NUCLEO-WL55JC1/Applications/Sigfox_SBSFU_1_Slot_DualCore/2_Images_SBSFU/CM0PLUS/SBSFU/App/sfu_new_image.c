/**
  ******************************************************************************
  * @file    sfu_new_image.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage the New Firmware Image storage and installation.
  *          This file contains the services the bootloader can use to
  *          know where to store a new FW image and request its installation.
  *          The same services are offered to the user application thanks to a similar file integrated in the user
  *          application.
  * @note    This file is compiled in the scope of SB_SFU.
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
#include "sfu_def.h"
#include "sfu_new_image.h"
#include "sfu_fwimg_services.h"
#include "sfu_fwimg_regions.h"
#include "se_def_metadata.h"
#include <string.h> /* needed for memset (see WriteInstallHeader)*/


/**
  * @brief  Write in Flash the next header image to install.
  *         This function is used by the local loader to request a Firmware installation (at next reboot).
  * @param  fw_header FW header of the FW to be installed
  * @retval SFU_SUCCESS if successful, otherwise SFU_ERROR
  */
SFU_ErrorStatus SFU_IMG_InstallAtNextReset(uint8_t *fw_header)
{
  return SFU_SUCCESS;           /* Nothing to do */
}

/**
  * @brief  Provide the area descriptor to write a FW image in Flash.
  *         This function is used by the local loader to know where to store a new Firmware Image before asking for its
  *         installation.
  * @param  DwlSlot identification of the downloaded area
  * @param  pArea pointer to area descriptor
  * @retval none
  */
void SFU_IMG_GetDownloadAreaInfo(uint32_t DwlSlot, SFU_FwImageFlashTypeDef *pArea)
{
  pArea->DownloadAddr = SlotStartAdd[DwlSlot];
  pArea->MaxSizeInBytes = (uint32_t)SLOT_SIZE(DwlSlot);
  pArea->ImageOffsetInBytes = SFU_IMG_IMAGE_OFFSET;
}