/**
  ******************************************************************************
  * @file    sfu_loader.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update local
  *          loader.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

