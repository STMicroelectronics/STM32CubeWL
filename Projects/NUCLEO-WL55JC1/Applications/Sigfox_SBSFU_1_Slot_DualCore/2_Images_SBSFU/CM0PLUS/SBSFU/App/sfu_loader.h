/**
  ******************************************************************************
  * @file    sfu_loader.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update local
  *          loader.
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
#ifndef SFU_LOADER_H
#define SFU_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "se_def.h"
#include "sfu_def.h"
#include "sfu_new_image.h"

/* Exported types ------------------------------------------------------------*/
/**
  * Local loader status Type Definition
  */
typedef enum
{
  SFU_LOADER_OK                            = 0x00U,
  SFU_LOADER_ERR_COM                       = 0x01U,
  SFU_LOADER_ERR_AUTH_FAILED               = 0x02U,
  SFU_LOADER_ERR_FW_LENGTH                 = 0x03U,
  SFU_LOADER_ERR_FW_VERSION                = 0x04U,
  SFU_LOADER_ERR_FLASH                     = 0x05U,
  SFU_LOADER_ERR_CRYPTO                    = 0x06U
} SFU_LOADER_StatusTypeDef;

/* Exported functions ------------------------------------------------------- */
SFU_ErrorStatus SFU_LOADER_Init(void);
SFU_ErrorStatus SFU_LOADER_DeInit(void);
SFU_ErrorStatus SFU_LOADER_DownloadNewUserFw(SFU_LOADER_StatusTypeDef *peSFU_LOADER_Status, uint32_t *p_DwlSlot,
                                             uint32_t *p_Size);

#ifdef __cplusplus
}
#endif

#endif /* SFU_LOADER_H */
