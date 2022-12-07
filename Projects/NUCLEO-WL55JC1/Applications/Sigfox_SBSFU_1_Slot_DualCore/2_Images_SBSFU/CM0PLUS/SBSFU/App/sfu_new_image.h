/**
  ******************************************************************************
  * @file    sfu_new_image.h
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage the New Firmware
  *          Image storage and installation.
  *          This file contains the services the bootloader can use to
  *          know where to store a new FW image and request its installation.
  *          The same services are offered to the user application thanks to a similar
  *          file integrated in the user application.
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
#ifndef SFU_NEW_IMAGE_H
#define SFU_NEW_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "sfu_def.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief Specifies a structure containing values related to the management of multi-images in Flash.
  */
typedef struct
{
  uint32_t  MaxSizeInBytes;        /*!< The maximum allowed size for the FwImage in User Flash (in Bytes) */
  uint32_t  DownloadAddr;          /*!< The download address for the FwImage in UserFlash */
  uint32_t  ImageOffsetInBytes;    /*!< Image write starts at this offset */
  uint32_t  ExecutionAddr;         /*!< The execution address for the FwImage in UserFlash */
} SFU_FwImageFlashTypeDef;

/* Exported functions ------------------------------------------------------- */
SFU_ErrorStatus SFU_IMG_InstallAtNextReset(uint8_t *fw_header);
void SFU_IMG_GetDownloadAreaInfo(uint32_t DwlSlot, SFU_FwImageFlashTypeDef *pArea);

#ifdef __cplusplus
}
#endif

#endif /* SFU_NEW_IMAGE_H */