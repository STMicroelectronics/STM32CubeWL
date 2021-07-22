/**
  ******************************************************************************
  * @file    sfu_fwimg_services.h
  * @author  MCD Application Team
  * @brief   This file contains the 2 images handling service (SFU_FWIMG functionalities)
  *          API definitions.
  *          These services can be called by the bootloader to deal with images handling.
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
#ifndef SFU_FWIMG_SERVICES_H
#define SFU_FWIMG_SERVICES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "se_def.h"
#include "sfu_def.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  SFU_IMG Initialization Status Type Definition
  */
typedef enum
{
  SFU_IMG_INIT_OK = 0x0U,                /*!< SFU Firmware Image Handling (FWIMG) Init OK */
  SFU_IMG_INIT_SLOTS_SIZE_ERROR,         /*!< error related to slots size */
  SFU_IMG_INIT_SWAP_SETTINGS_ERROR,      /*!< error related to swap settings */
  SFU_IMG_INIT_FLASH_CONSTRAINTS_ERROR,  /*!< error related to flash constraints */
  SFU_IMG_INIT_CRYPTO_CONSTRAINTS_ERROR, /*!< error related to crypto constraints */
  SFU_IMG_INIT_ERROR                     /*!< Init is FAILED: unspecified error */
} SFU_IMG_InitStatusTypeDef;

/**
  * @brief  SFU_IMG Image Installation State Type Definition
  */
typedef enum
{
  SFU_IMG_FWIMAGE_TO_INSTALL = 0x0U,   /*!< There is a FW image to be installed */
  SFU_IMG_FWUPDATE_STOPPED,            /*!< A previous installation has been interrupted before it completed : resume installation */
  SFU_IMG_NO_FWUPDATE,                 /*!< No FW image installation pending */
  SFU_IMG_FWIMAGE_ROLLBACK,            /*!< A new FW image installation has not been validated : rollback initiated @ next reset */
} SFU_IMG_ImgInstallStateTypeDef;

/* Exported functions ------------------------------------------------------- */
SFU_IMG_InitStatusTypeDef SFU_IMG_InitImageHandling(void);
SFU_IMG_InitStatusTypeDef SFU_IMG_CheckSwapImageHandling(void);
SFU_IMG_ImgInstallStateTypeDef SFU_IMG_CheckPendingInstallation(uint32_t *pDwlSlotToInstall, \
                                                                uint32_t *pActiveSlotToResume, \
                                                                uint32_t *pActiveSlotToRollback);
SFU_ErrorStatus SFU_IMG_CheckCandidateVersion(uint32_t DwlSlot);
SFU_ErrorStatus SFU_IMG_TriggerImageInstallation(uint32_t DwlSlot);
SFU_ErrorStatus SFU_IMG_TriggerResumeInstallation(uint32_t ActiveSlot, uint32_t DwlSlot);
#ifdef ENABLE_IMAGE_STATE_HANDLING
SFU_ErrorStatus SFU_IMG_TriggerRollbackInstallation(uint32_t ActiveSlot, uint32_t DwlSlot);
SFU_ErrorStatus SFU_IMG_UpdateImageState(uint32_t SlotNumber);
#endif /* ENABLE_IMAGE_STATE_HANDLING */
SFU_ErrorStatus SFU_IMG_EraseDownloadedImg(uint32_t DwlSlot);
uint32_t SFU_IMG_GetTrailerSize(uint32_t SlotNumber);
SFU_ErrorStatus SFU_IMG_InvalidateCurrentFirmware(uint32_t SlotNumber);
SFU_ErrorStatus SFU_IMG_VerifyActiveImgMetadata(uint32_t SlotNumber);
SFU_ErrorStatus SFU_IMG_VerifyActiveImg(uint32_t SlotNumber);
SFU_ErrorStatus SFU_IMG_VerifyActiveSlot(uint32_t SlotNumber);
SFU_ErrorStatus SFU_IMG_VerifyEmptyActiveSlot(uint32_t SlotNumber);
SFU_ErrorStatus SFU_IMG_LaunchActiveImg(uint32_t SlotNumber);
uint16_t SFU_IMG_GetActiveFwVersion(uint32_t SlotNumber);
SFU_ErrorStatus SFU_IMG_DetectFW(uint32_t SlotNumber);
SFU_ErrorStatus SFU_IMG_Validation(uint32_t SlotNumber, SE_FwRawHeaderTypeDef *pFWImageHeader);
SFU_ErrorStatus SFU_IMG_ControlActiveImgTag(uint32_t SlotNumber);
uint32_t SFU_IMG_GetFwImageSlot(SE_FwRawHeaderTypeDef *pFwImageHeader);
SFU_ErrorStatus SFU_IMG_CheckFwVersion(uint32_t ActiveSlot, uint16_t CurrentVersion, uint16_t CandidateVersion);

#ifdef __cplusplus
}
#endif

#endif /* SFU_FWIMG_SERVICES_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

