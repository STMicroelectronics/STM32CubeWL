/**
  ******************************************************************************
  * @file    sfu_kms.c
  * @author  MCD Application Team
  * @brief   This file provides set of functions to manage the KMS Images.
  *          This file contains the kms services the bootloader (sfu_boot) can
  *             use to deal with the KMS Blob images handling.
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

#define SFU_KMS_C


#ifdef KMS_ENABLED

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "sfu_fsm_states.h" /* needed for sfu_error.h */
#include "sfu_error.h"
#include "sfu_low_level_flash_int.h"
#include "sfu_low_level_security.h"
#include "sfu_low_level.h"
#include "sfu_fwimg_services.h"
#include "sfu_fwimg_internal.h"
#include "sfu_trace.h"
#include "sfu_kms.h"
#include "sfu_fwimg_internal.h" /* required to re-use kmsblob_image_header_to_test (and SWAP_HDR) in the KMS blob 
                                   installation procedure */
#include "sfu_fwimg_regions.h"  /* required because KMS re-uses the SBSFU SWAP area */


/* Private variables ---------------------------------------------------------*/
/**
  * Magic pattern identifying the beginning of a KMS blob.
  */
static uint8_t     kms_magic[4] = {"KMSB"};

/**
  * KMS Blob header (metadata) of the candidate Blob in SWAP: structured format (access by fields)
  */
static KMS_BlobRawHeaderTypeDef kmsblob_image_header_to_test;
static uint32_t                 kmsblob_slot;

/* Private macros ------------------------------------------------------------*/
/**
  * The KMS blob header has the same structure as the SBSFU Firmware Header.
  * Therefore, authenticating a KMS blob header can be done by re-using the SBSFU FWIMG service authenticating a FW
  * Header.
  * The authentication of the KMS blob itself is not in the scope of sfu_kms, this is handled by the KMS middleware.
  */
#define SFU_KMS_AUTHENTICATE_BLOB_HEADER SFU_IMG_CheckAndGetFWHeader


/* Functions Definition ------------------------------------------------------*/
/**
  * @brief  Check that there is a Blob to Install ==> KMSB MAGIC present at the beginning of a DWN slot ?
  * @retval SFU_SUCCESS if Image can be installed, a SFU_ERROR  otherwise.
  */
SFU_ErrorStatus SFU_KMS_BlobToInstall(void)
{
  SFU_ErrorStatus  e_ret_status = SFU_ERROR;

  /*
   * The KMS blob header uses a structure dedicated Header.
   * In this function we focus only on the KMS blob header, not the entire KMS blob.
   */

  for (uint32_t i = 0U; i < SFU_NB_MAX_DWL_AREA; i++)
  {
    /* Slot configured ? */
    if (SlotStartAdd[SLOT_DWL_1 + i] != 0U)
    {
      /*  Loading the KMS blob header contained in the SWAP area to verify if it can be a valid KMS blob */
      if (SFU_LL_FLASH_Read((uint8_t *)(uint32_t)(&kmsblob_image_header_to_test),
                            (uint8_t *)SlotStartAdd[SLOT_DWL_1 + i],
                            sizeof(KMS_BlobRawHeaderTypeDef)) == SFU_SUCCESS)
      {
        /* Is there any KMSBlob present in the SWAP ? */
        if (memcmp((uint8_t *)&kmsblob_image_header_to_test.KMSMagic, kms_magic, sizeof(kms_magic)) == 0)
        {
          /*
           * Check the authenticity of the KMS Blob stored in the DWN slot.
           * This check is restricted to the authentication of the KMS blob header.
           * Authenticating the KMS blob itself is the role of the KMS module (KMS middleware).
           */
          kmsblob_slot = SLOT_DWL_1 + i; /* Save also DWN slot for later use */
          e_ret_status = SFU_SUCCESS;
          break;
        }
      }
    }
  }

  return e_ret_status;
}

/**
  * @brief  Retrieve information about the KMS blob hdr and location in flash
  * @param  pHdr is the pointer to the encrypted blob header
  * @param  pFlash is the pointer to the blob location in flash
  * @retval none
  */
SFU_ErrorStatus SFU_KMS_GetBlobInstallInfo(uint8_t **ppHdr, uint8_t **ppFlash)
{
  SFU_ErrorStatus  e_ret_status = SFU_ERROR;
  if ((ppHdr != NULL) && (ppFlash != NULL))
  {
    *ppHdr = (uint8_t *)(uint32_t)&kmsblob_image_header_to_test;
    *ppFlash = (uint8_t *)(((uint32_t)SlotStartAdd[kmsblob_slot]) + sizeof(kmsblob_image_header_to_test));
    e_ret_status = SFU_SUCCESS;
  }
  return e_ret_status;
}

/**
  * @brief  Erase the Blob
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_KMS_EraseBlob(void)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SFU_FLASH_StatusTypeDef flash_if_status;

  /*  Erase in flash  */
  e_ret_status = SFU_LL_FLASH_Erase_Size(&flash_if_status,
                                         (uint8_t *) SlotStartAdd[kmsblob_slot],
                                         SLOT_SIZE(kmsblob_slot));

  return e_ret_status;
}


/**
  * @brief Checks if there is a pending Blob installation.
  *        3 situations can occur:
  *        A. First installation request for a blob: a blob is ready to be installed but the installation has never
  *           been triggered so far.
  *        B. Blob installation to be retried: a blob installation has been interrupted (switch off) and we can restart
  *           its installation.
  *        C. No blob installation pending.
    * @note This function populates the FWIMG module variable: kmsblob_image_header_to_test.
  *         If a KMS blob is present then it is copied in kmsblob_image_header_to_test.
  * @param  None.
  * @retval SFU_KMS_BlobInstallStateTypeDef Pending Installation status (installation requested or not)
  */
SFU_KMS_BlobInstallStateTypeDef SFU_KMS_CheckPendingBlobInstallation(void)
{
  SFU_KMS_BlobInstallStateTypeDef e_ret_state = SFU_KMS_NO_BLOBUPDATE;

  /*
   * While the SWAP AREA contains a KMSB Magic, it means the KMSBlob installation is not finalized.
   *
   */
  if (SFU_SUCCESS == SFU_KMS_BlobToInstall())
  {
    /*
     * A KMS blob is available for installation:
     * kmsblob_image_header_to_test has been populated with this blob.
     */
    e_ret_state = SFU_KMS_BLOBIMAGE_TO_INSTALL;
  }
  else
  {
    /* No pending update */
    e_ret_state = SFU_KMS_NO_BLOBUPDATE;
  }

  return e_ret_state;
}

#endif /* KMS_ENABLED */

