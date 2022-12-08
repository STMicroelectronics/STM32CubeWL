/**
  ******************************************************************************
  * @file    sfu_fwimg_no_swap.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage the Firmware Images.
  *          This file contains the specific functionalities of the image handling for
  *          the installation process without swap area.
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

#define SFU_FWIMG_NO_SWAP_C

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "sfu_fsm_states.h" /* needed for sfu_error.h */
#include "sfu_error.h"
#include "sfu_low_level_flash_int.h"
#include "sfu_low_level_security.h"
#include "se_interface_bootloader.h"
#include "sfu_interface_crypto_scheme.h"
#include "sfu_fwimg_regions.h"
#include "sfu_fwimg_services.h"
#include "sfu_fwimg_internal.h"
#include "sfu_trace.h"
#include "sfu_boot.h"


#if  defined(SFU_NO_SWAP)
/* Private macros ------------------------------------------------------------*/
#define TRAILER_HEADER   (SE_FW_HEADER_TOT_LEN + SE_FW_HEADER_TOT_LEN + MAGIC_LENGTH)

/*             DWL_SLOT_REGION_SIZE             */
/* <------------------------------------------> */
/* |HEADER_TOT_LEN|HEADER_TOT_LEN|MAGIC_LENGTH| */
/* | header 1     | header 2     |SWAP magic  | */

#define TRAILER_BEGIN(A)  (( uint8_t *)(SlotStartAdd[A] + SLOT_SIZE(A) - TRAILER_HEADER))
#define TRAILER_HDR_VALID(A) ((uint8_t *)(TRAILER_BEGIN(A)))
#define TRAILER_HDR_TEST(A)  ((uint8_t *)(TRAILER_BEGIN(A) + SE_FW_HEADER_TOT_LEN))
#define TRAILER_SWAP_ADDR(A) ((uint8_t *)(TRAILER_BEGIN(A) + SE_FW_HEADER_TOT_LEN + SE_FW_HEADER_TOT_LEN))

#endif /* defined(SFU_NO_SWAP) */

/* Functions Definition : helper ---------------------------------------------*/
#if  defined(SFU_NO_SWAP)

/**
  * @brief  Write Trailer Headers : TEST + VALID + SWAP
  * @param  DwlSlot identification of the downloaded area
  * @param  pValidHeader: pointer in ram to header of valid fw to backup
  * @param  pTestHeader: pointer in ram to header of fw to test
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
static SFU_ErrorStatus WriteTrailerHeader(uint32_t DwlSlot, SE_FwRawHeaderTypeDef *pValidHeader,
                                          SE_FwRawHeaderTypeDef *pTestHeader)
{
  /* everything is in place , just compute from present data and write it */
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SFU_FLASH_StatusTypeDef flash_if_info;
  uint8_t  magic[MAGIC_LENGTH];
  uint32_t buffer;

  /* Write Headers VALID + TEST */
  e_ret_status = SFU_LL_FLASH_Write(&flash_if_info, TRAILER_HDR_TEST(DwlSlot), (uint8_t *) pTestHeader,
                                    SE_FW_HEADER_TOT_LEN);
  if (e_ret_status == SFU_SUCCESS)
  {
    e_ret_status = SFU_LL_FLASH_Write(&flash_if_info, TRAILER_HDR_VALID(DwlSlot), (uint8_t *) pValidHeader,
                                      SE_FW_HEADER_TOT_LEN);
  }

  /* Write Magic */
  if (e_ret_status == SFU_SUCCESS)
  {
    buffer = (uint32_t) pValidHeader;
    (void) memcpy(&magic[0U], (uint8_t *)(buffer + SE_FW_AUTH_LEN + (MAGIC_LENGTH / 2U)), MAGIC_LENGTH / 2U);
    buffer = (uint32_t) pTestHeader;
    (void) memcpy(&magic[MAGIC_LENGTH / 2U], (uint8_t *)(buffer + SE_FW_AUTH_LEN + (MAGIC_LENGTH / 2U)),
                  MAGIC_LENGTH / 2U);
    e_ret_status = SFU_LL_FLASH_Write(&flash_if_info, TRAILER_SWAP_ADDR(DwlSlot), magic, MAGIC_LENGTH);
  }

  return e_ret_status;
}

/**
  * @brief  Check the magic from trailer or counter
  * @param  DwlSlot identification of the downloaded area
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
static SFU_ErrorStatus CheckTrailerMagic(uint32_t DwlSlot)
{
  uint8_t  magic[MAGIC_LENGTH];
  uint8_t  signature_valid[MAGIC_LENGTH / 2];
  uint8_t  signature_test[MAGIC_LENGTH / 2];
  uint8_t  erased_flash_pattern[MAGIC_LENGTH];

  /* Pattern initialization */
  (void) memset(erased_flash_pattern, 0xFF, MAGIC_LENGTH);

  /* Read trailer fields : TRAILER_HDR_VALID, TRAILER_HDR_TEST, MAGIC, CLEAN */
  if (SFU_LL_FLASH_Read(signature_valid,
                        (uint8_t *)((uint32_t)TRAILER_HDR_VALID(DwlSlot) + SE_FW_AUTH_LEN + (MAGIC_LENGTH / 2U)),
                        sizeof(signature_valid)) != SFU_SUCCESS)
  {
    return SFU_ERROR;
  }
  if (SFU_LL_FLASH_Read(signature_test,
                        (uint8_t *)((uint32_t) TRAILER_HDR_TEST(DwlSlot) + SE_FW_AUTH_LEN + (MAGIC_LENGTH / 2U)),
                        sizeof(signature_test)) != SFU_SUCCESS)
  {
    return SFU_ERROR;
  }
  if (SFU_LL_FLASH_Read(magic, TRAILER_SWAP_ADDR(DwlSlot), MAGIC_LENGTH) != SFU_SUCCESS)
  {
    return SFU_ERROR;
  }

  /*
   * Check magic validity :
   *  - last 16 bytes of validated header
   *  - last 16 bytes of test header
   *  - significant values (not an erased flash pattern)
   */
  if ((memcmp(magic, signature_valid, sizeof(signature_valid)) != 0)
      || (memcmp(&magic[MAGIC_LENGTH / 2U], signature_test, sizeof(signature_test)) != 0)
      || (memcmp(magic, erased_flash_pattern, MAGIC_LENGTH) == 0))
  {
    return SFU_ERROR;
  }
  return SFU_SUCCESS;
}

/**
  * @brief  check trailer validity to allow resume installation.
  * @param  ActiveSlot identification of the active image
  * @param  DwlSlot identification of the downloaded area
  * @param  pValidHeader: pointer in ram to header of valid fw to backup
  * @param  pTestHeader: pointer in ram to header of fw to test
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
static SFU_ErrorStatus  FirmwareToResume(uint32_t ActiveSlot, uint32_t DwlSlot, SE_FwRawHeaderTypeDef *pValidHeader,
                                          SE_FwRawHeaderTypeDef *pTestHeader)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  uint8_t fw_header_trailer_valid[SE_FW_HEADER_TOT_LEN];
  uint8_t fw_header_trailer_test[SE_FW_HEADER_TOT_LEN];
  uint8_t fw_header_active_slot[SE_FW_HEADER_TOT_LEN];
  uint8_t fw_header_dwl_slot[SE_FW_HEADER_TOT_LEN];

  /*
   *  Initial conditions to be fulfilled :
   *   - Trailer magic is present
   *   - Active slot number is correct
   *   - test header from trailer is signed
   *   - header from dwl slot is same as the one from trailer
   */

  /* check trailer Magic */
  e_ret_status = CheckTrailerMagic(DwlSlot);
  if (e_ret_status == SFU_SUCCESS)
  {
    /* Populate the pTestHeader and verify it the active slot is correct */
    e_ret_status = SFU_LL_FLASH_Read(fw_header_trailer_test, TRAILER_HDR_TEST(DwlSlot), sizeof(fw_header_trailer_test));
    STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_READ_FAILED);
  }

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Populate pTestHeader parameter */
    e_ret_status = ParseFWInfo(pTestHeader, fw_header_trailer_test);
  }

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Active slot number correct ?
       As example : if magic is SFU2 active slot should be SLOT_ACTIVE_2 */
    if (ActiveSlot == SFU_IMG_GetFwImageSlot(pTestHeader))
    {
      /* Check header test field in trailer is signed */
      e_ret_status = VerifyHeaderSignature((SE_FwRawHeaderTypeDef *)(uint32_t)fw_header_trailer_test);
    }
    else
    {
#if defined(SFU_VERBOSE_DEBUG_MODE)
      TRACE("\r\n\t  No resume required : TRAILER_HDR_TEST not valid!");
#endif /* SFU_VERBOSE_DEBUG_MODE */
      e_ret_status = SFU_ERROR;
    }
  }

  /* Read header from dwl slot and verify this is the same as the test header from trailer */
  if (e_ret_status == SFU_SUCCESS)
  {
    e_ret_status = SFU_LL_FLASH_Read(fw_header_dwl_slot, (uint8_t *) SlotStartAdd[DwlSlot],
                                     sizeof(fw_header_dwl_slot));
    STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_READ_FAILED);
    if (memcmp(fw_header_dwl_slot, fw_header_trailer_test, SE_FW_AUTH_LEN) != 0U)
    {
#if defined(SFU_VERBOSE_DEBUG_MODE)
      TRACE("\r\n\t  No resume required : TRAILER_HDR_TEST not same as dwl slot header!");
#endif /* SFU_VERBOSE_DEBUG_MODE */
      e_ret_status = SFU_ERROR;
    }
  }

  /* First part KO : return error */
  if (e_ret_status != SFU_SUCCESS)
  {
    return SFU_ERROR;
  }

  /* Read header in active slot if any */
  e_ret_status = SFU_LL_FLASH_Read(fw_header_active_slot, (uint8_t *) SlotHeaderAdd[ActiveSlot],
                                   sizeof(fw_header_active_slot));
  STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_READ_FAILED);

  if (e_ret_status == SFU_SUCCESS)
  {
    /* Verify the header signature in active slot */
    e_ret_status = VerifyHeaderSignature((SE_FwRawHeaderTypeDef *)(uint32_t)fw_header_active_slot);
  }
  if (e_ret_status == SFU_SUCCESS)
  {
    /* Header in active slot is signed and validated, in protected area: it can be trusted. */

    /* Populate pValidHeader parameter */
    e_ret_status = ParseFWInfo(pValidHeader, fw_header_active_slot);

    if (e_ret_status == SFU_SUCCESS)
    {
      /* Read header valid field in trailer */
      e_ret_status = SFU_LL_FLASH_Read(fw_header_trailer_valid, TRAILER_HDR_VALID(DwlSlot),
                                       sizeof(fw_header_trailer_valid));
      STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_READ_FAILED);
    }

    if (e_ret_status == SFU_SUCCESS)
    {
      /*
       * 3 cases:
       * - Header in active slot is identical to the header valid field
       *   ==> installation interrupted very early. Resume install must be triggered.
       * - Header in active slot is identical to the header test field
       *   ==> installation interrupted after active header erasing. Resume install must be triggered.
       * - Any other values
       *   ==> Hack tentative, resume install must not be triggered.
       */

      /* Check if header in active slot is same as header valid in trailer */
      if (memcmp(fw_header_active_slot, fw_header_trailer_valid, SE_FW_AUTH_LEN) != 0)
      {
        /* Check candidate image version : should be OK */
        e_ret_status = SFU_IMG_CheckFwVersion(ActiveSlot, pValidHeader->FwVersion, pTestHeader->FwVersion);
      }
      /* Check if header in active slot is same as header test in trailer */
      else if (memcmp(fw_header_active_slot, fw_header_trailer_test, SE_FW_AUTH_LEN) == 0U)
      {
        e_ret_status = SFU_SUCCESS;
      }
      else
      {
#if defined(SFU_VERBOSE_DEBUG_MODE)
        TRACE("\r\n\t  No resume required : Header in active slot is neither sames TRAILER_HDR_TEST as nor TRAILER_VALID_TEST!");
#endif /* SFU_VERBOSE_DEBUG_MODE */
        e_ret_status = SFU_ERROR;
      }
    }
    /* At this point, if (e_ret_status == SFU_SUCCESS) then resume of installation is allowed */
  }
  else
  {
    /*
     * Header in active slot is not valid.
     * It is considered this can not be hack attempt, because header in active slot is in
     * protected area.
     * Possible reasons:
     * - Installation has been interrupted during very first image installation (active slot was empty)
     * - Installation has been interrupted during reconstitution of first sector of active slot
     * If header test field in trailer is signed and with proper version, we resume install.
     */

    /* Read header valid field in trailer */
    e_ret_status = SFU_LL_FLASH_Read(fw_header_trailer_valid, TRAILER_HDR_VALID(DwlSlot),
                                     sizeof(fw_header_trailer_valid));
    STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_READ_FAILED);
    if (e_ret_status == SFU_SUCCESS)
    {
      /* Check header valid field in trailer is signed */
      e_ret_status = VerifyHeaderSignature((SE_FwRawHeaderTypeDef *)(uint32_t)fw_header_trailer_valid);

      if (e_ret_status == SFU_SUCCESS)
      {
        /* active slot was containing an active image */

        /* Populate pValidHeader parameter */
        e_ret_status = ParseFWInfo(pValidHeader, fw_header_trailer_valid);

        if (e_ret_status == SFU_SUCCESS)
        {
          /* Check candidate image version */
          e_ret_status = SFU_IMG_CheckFwVersion(ActiveSlot, pValidHeader->FwVersion, pTestHeader->FwVersion);
        }
      }
      else
      {
        /* active slot was empty : we authorize only the installation of a specific version identified by
           SFU_FW_VERSION_INIT_NUM */
        if (pTestHeader->FwVersion == SFU_FW_VERSION_INIT_NUM)
        {
          /* active slot was empty or was containing bricked Fw image */
          e_ret_status = SFU_SUCCESS;
        }
#if defined(SFU_VERBOSE_DEBUG_MODE)
        else
        {
          TRACE("\r\n\t  Init version:%d - Candidate version:%d : Installation not allowed!", SFU_FW_VERSION_INIT_NUM, pTestHeader->FwVersion);
        }
#endif /* SFU_VERBOSE_DEBUG_MODE */
      }
    }

    /* At this point, if (e_ret_status == SFU_SUCCESS) then resume of installation is allowed */
  }

  return e_ret_status;
}

/**
  * @brief  Verify signature on encrypted FW
  * @param  DwlSlot identification of the downloaded area
  * @param  pFwImageHeader pointer to fw header
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
static SFU_ErrorStatus VerifyFwSignatureBeforeDecrypt(uint32_t DwlSlot, SE_FwRawHeaderTypeDef *pFwImageHeader)
{
  SFU_ErrorStatus  e_ret_status = SFU_ERROR;
  SE_StatusTypeDef e_se_status;
  SE_ErrorStatus   se_ret_status;
  uint8_t fw_encrypted_chunk[SFU_IMG_CHUNK_SIZE] __attribute__((aligned(8)));
  uint8_t fw_decrypted_chunk[SFU_IMG_CHUNK_SIZE] __attribute__((aligned(8)));
  uint32_t fw_decrypted_chunk_size;
#if (SECBOOT_CRYPTO_SCHEME != SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
  uint8_t fw_authenticated_chunk[SFU_IMG_CHUNK_SIZE] __attribute__((aligned(8)));
  uint32_t fw_authenticated_chunk_size;
#endif /* (SECBOOT_CRYPTO_SCHEME != SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) */
  uint32_t fw_index;
  uint32_t size;
  uint32_t fw_tag_len;
  uint8_t fw_tag_output[SE_TAG_LEN];

  /* Verify header presence */
  if ((pFwImageHeader == NULL))
  {
    return e_ret_status;
  }

  /* Decryption process initialization
     ================================= */
  se_ret_status = SE_Decrypt_Init(&e_se_status, pFwImageHeader, SE_FW_IMAGE_COMPLETE);
  if (se_ret_status == SE_SUCCESS)
  {
#if (SECBOOT_CRYPTO_SCHEME != SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
    se_ret_status = SE_AuthenticateFW_Init(&e_se_status, pFwImageHeader, SE_FW_IMAGE_COMPLETE);
#endif /* (SECBOOT_CRYPTO_SCHEME != SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) */

    /* Skip header : no decryption needed */
    fw_index = SFU_IMG_IMAGE_OFFSET;

    e_ret_status = SFU_SUCCESS;
    while ((e_ret_status == SFU_SUCCESS) && (se_ret_status == SE_SUCCESS) &&
           (fw_index < (pFwImageHeader->FwSize + SFU_IMG_IMAGE_OFFSET)))
    {
      /* Set size of reading/decryption */
      size = SFU_IMG_CHUNK_SIZE;

      /* Last pass ? */
      if (((pFwImageHeader->FwSize + SFU_IMG_IMAGE_OFFSET) - fw_index) < size)
      {
        /* Default chunk size can be truncated at end of file */
        size = pFwImageHeader->FwSize + SFU_IMG_IMAGE_OFFSET - fw_index;
      }

      /* read then decrypt phase
       ======================= */
      e_ret_status = SFU_LL_FLASH_Read(fw_encrypted_chunk, (uint8_t *)(SlotStartAdd[DwlSlot] + fw_index), size);
      if (e_ret_status == SFU_SUCCESS)
      {
        fw_decrypted_chunk_size = size;
        se_ret_status = SE_Decrypt_Append(&e_se_status, fw_encrypted_chunk, (int32_t)size, fw_decrypted_chunk,
                                          (int32_t *)&fw_decrypted_chunk_size);

        if (se_ret_status == SE_SUCCESS)
        {
#if (SECBOOT_CRYPTO_SCHEME != SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
          fw_authenticated_chunk_size = sizeof(fw_authenticated_chunk);
          se_ret_status = SE_AuthenticateFW_Append(&e_se_status, fw_decrypted_chunk, (int32_t)size,
                                                   fw_authenticated_chunk, (int32_t *)&fw_authenticated_chunk_size);
#endif /* (SECBOOT_CRYPTO_SCHEME != SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) */
          fw_index += size;
        }
      }
    }
  }

  /* Do the Finalization, check the authentication TAG
     ================================================= */
  if ((se_ret_status == SE_SUCCESS) && (e_ret_status == SFU_SUCCESS))
  {
    e_ret_status = SFU_ERROR;

    /* Finalization stage */
    fw_tag_len = sizeof(fw_tag_output);
    se_ret_status = SE_Decrypt_Finish(&e_se_status, fw_tag_output, (int32_t *)&fw_tag_len);
    if (se_ret_status == SE_SUCCESS)
    {
#if (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM)
      e_ret_status = SFU_SUCCESS;
#else
      fw_tag_len = sizeof(fw_tag_output);
      se_ret_status = SE_AuthenticateFW_Finish(&e_se_status, fw_tag_output, (int32_t *)&fw_tag_len);
      if ((se_ret_status == SE_SUCCESS) && (fw_tag_len == SE_TAG_LEN))
      {
        /* Firmware tag verification */
        if (MemoryCompare(fw_tag_output, pFwImageHeader->FwTag, SE_TAG_LEN) == SFU_SUCCESS)
        {
          e_ret_status = SFU_SUCCESS;
        }
      }
#endif /* (SECBOOT_CRYPTO_SCHEME == SECBOOT_AES128_GCM_AES128_GCM_AES128_GCM) */
    }
  }
  else
  {
    e_ret_status = SFU_ERROR;
  }

  return e_ret_status;
}

/**
  * @brief  Decrypt Image from dwl slot to active slot
  * @param  ActiveSlot identification of the active image
  * @param  DwlSlot identification of the downloaded area
  * @param  pFwImageHeader pointer to fw header
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
static SFU_ErrorStatus DecryptImageFromDwlSlotToActiveSlot(uint32_t ActiveSlot, uint32_t DwlSlot,
                                                           SE_FwRawHeaderTypeDef *pFwImageHeader)
{
  SFU_ErrorStatus  e_ret_status = SFU_ERROR;
  SE_StatusTypeDef e_se_status;
  SE_ErrorStatus   se_ret_status;
  SFU_FLASH_StatusTypeDef flash_if_status;
  uint8_t fw_encrypted_chunk[SFU_IMG_CHUNK_SIZE] __attribute__((aligned(8)));
  uint8_t fw_decrypted_chunk[SFU_IMG_CHUNK_SIZE] __attribute__((aligned(8)));
  uint32_t fw_index;
  uint32_t size;
  uint32_t fw_decrypted_chunk_size;
  uint32_t fw_tag_len;
  uint8_t fw_tag_output[SE_TAG_LEN];

  TRACE("\r\n\t  Image preparation done.\r\n\t  Installation started ...");

  /* Verify header presence */
  if ((pFwImageHeader == NULL))
  {
    return e_ret_status;
  }

  /* Control the firmware size vs slot size */
  if ((pFwImageHeader->FwSize + SFU_IMG_IMAGE_OFFSET) > SLOT_SIZE(ActiveSlot))
  {
    return e_ret_status;
  }

  /* Decryption process initialization
     ================================= */
  se_ret_status = SE_Decrypt_Init(&e_se_status, pFwImageHeader, SE_FW_IMAGE_COMPLETE);
  if (se_ret_status == SE_SUCCESS)
  {

    /* Erasing operation
     ================= */

    /*
     *  The header may not be contiguous with the FW image : 2 erasing operations needed
     */
    e_ret_status = SFU_LL_FLASH_Erase_Size(&flash_if_status, (uint8_t *) SlotHeaderAdd[ActiveSlot],
                                           SFU_IMG_IMAGE_OFFSET);
    if (e_ret_status == SFU_SUCCESS)
    {
      e_ret_status = SFU_LL_FLASH_Erase_Size(&flash_if_status,
                                             (uint8_t *)(SlotStartAdd[ActiveSlot] + SFU_IMG_IMAGE_OFFSET),
                                             SLOT_SIZE(ActiveSlot) - SFU_IMG_IMAGE_OFFSET);
    }
    STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_ERASE_FAILED);

    /*
     * Save immediately the new active FW header
     */
    if (e_ret_status == SFU_SUCCESS)
    {
      e_ret_status = SFU_IMG_Validation(ActiveSlot, pFwImageHeader);
      if (e_ret_status != SFU_SUCCESS)
      {
#if defined(SFU_VERBOSE_DEBUG_MODE)
        TRACE("\r\n\t  Header writing failure!");
#endif /* SFU_VERBOSE_DEBUG_MODE */
      }
    }

    /*
     * Skip header : no decryption needed
     */
    fw_index = SFU_IMG_IMAGE_OFFSET;

    while ((e_ret_status == SFU_SUCCESS) && (se_ret_status == SE_SUCCESS) &&
           (fw_index < (pFwImageHeader->FwSize + SFU_IMG_IMAGE_OFFSET)))
    {
      /* Refresh Watchdog every 10 iterations */
      if ((fw_index % 10U) != 0U)
      {
        (void) SFU_LL_SECU_IWDG_Refresh();
      }

      /* Set size of reading/decryption */
      size = SFU_IMG_CHUNK_SIZE;

      /* Last pass ? */
      if (((pFwImageHeader->FwSize + SFU_IMG_IMAGE_OFFSET) - fw_index) < size)
      {
        /* Default chunk size can be truncated at end of file */
        size = pFwImageHeader->FwSize + SFU_IMG_IMAGE_OFFSET - fw_index;
      }

      /* read then decrypt phase
       ======================= */
      e_ret_status = SFU_LL_FLASH_Read(fw_encrypted_chunk, (uint8_t *)(SlotStartAdd[DwlSlot] + fw_index), size);
      if (e_ret_status == SFU_SUCCESS)
      {
        fw_decrypted_chunk_size = size;
        se_ret_status = SE_Decrypt_Append(&e_se_status, fw_encrypted_chunk, (int32_t)size, fw_decrypted_chunk,
                                          (int32_t *)&fw_decrypted_chunk_size);

        /*
        * writing phase
        * =============== */
        if (se_ret_status == SE_SUCCESS)
        {
          /* Destination has to be in internal flash to keep confidentiality : SFU_LL_FLASH_INT_Write() */
          e_ret_status = SFU_LL_FLASH_INT_Write(&flash_if_status, (uint8_t *)(SlotStartAdd[ActiveSlot] + fw_index),
                                            fw_decrypted_chunk, size);
          STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_WRITE_FAILED);

          /* Update pointer */
          fw_index += size;
        }
      }
    }
  }

#if (SFU_IMAGE_PROGRAMMING_TYPE == SFU_ENCRYPTED_IMAGE)
#if defined(SFU_VERBOSE_DEBUG_MODE)
  TRACE("\r\n\t  %d bytes of ciphertext decrypted.", fw_index);
#endif /* SFU_VERBOSE_DEBUG_MODE */
#endif /* SFU_ENCRYPTED_IMAGE */

  if ((se_ret_status == SE_SUCCESS) && (e_ret_status == SFU_SUCCESS))
  {
    /* Do the Finalization, check the authentication TAG*/
    fw_tag_len = sizeof(fw_tag_output);
    se_ret_status = SE_Decrypt_Finish(&e_se_status, fw_tag_output, (int32_t *)&fw_tag_len);
    if (se_ret_status != SE_SUCCESS)
    {
      e_ret_status = SFU_ERROR;
#if defined(SFU_VERBOSE_DEBUG_MODE)
      TRACE("\r\n\t  Decrypt fails at Finalization stage.");
#endif /* SFU_VERBOSE_DEBUG_MODE */
    }
  }
  else
  {
    e_ret_status = SFU_ERROR;
  }
  return e_ret_status;
}


/**
  * @brief  Install the new version
  * @param  ActiveSlot identification of the active image
  * @param  DwlSlot identification of the downloaded area
  * @param  pValidHeader: pointer in ram to header of valid fw to backup
  * @param  pTestHeader: pointer in ram to header of fw to test
  * @retval SFU_SUCCESS if successful,SFU_ERROR error otherwise.
  */
static SFU_ErrorStatus InstallNewVersion(uint32_t ActiveSlot, uint32_t DwlSlot, SE_FwRawHeaderTypeDef *pValidHeader,
                                         SE_FwRawHeaderTypeDef *pTestHeader)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SFU_FLASH_StatusTypeDef x_flash_info;

  /*
   * Verify FW Image signature in dwl slot
   */
  e_ret_status = VerifyFwSignatureBeforeDecrypt(DwlSlot, pTestHeader);
  if (e_ret_status != SFU_SUCCESS)
  {
    SFU_EXCPT_SetError(SFU_EXCPT_SIGNATURE_ERR);
    return e_ret_status;
  }

  /*
   * Control if there is no additional code beyond the firmware image (malicious SW)
   */
  e_ret_status = VerifySlot((uint8_t *) SlotStartAdd[DwlSlot], SLOT_SIZE(DwlSlot), pTestHeader->FwSize);
  if (e_ret_status != SFU_SUCCESS)
  {
    SFU_EXCPT_SetError(SFU_EXCPT_ADDITIONAL_CODE_ERR);
    return e_ret_status;
  }

  /*
   * Write trailer : memorize installation process started
   */
  e_ret_status = WriteTrailerHeader(DwlSlot, pValidHeader, pTestHeader);
  if (e_ret_status != SFU_SUCCESS)
  {
    SFU_EXCPT_SetError(SFU_EXCPT_TRAILER_UPDATE_ERR);
    return e_ret_status;
  }

  /*
   * Decrypt the FW image : from dwl slot to active slot
   */
  e_ret_status = DecryptImageFromDwlSlotToActiveSlot(ActiveSlot, DwlSlot, pTestHeader);
  if (e_ret_status != SFU_SUCCESS)
  {
    SFU_EXCPT_SetError(SFU_EXCPT_DECRYPT_ERR);
    return e_ret_status;
  }


  /*
   * erase downloaded FW after installation
   */
  e_ret_status = SFU_LL_FLASH_Erase_Size(&x_flash_info, (uint8_t *) SlotStartAdd[DwlSlot], SLOT_SIZE(DwlSlot));
  if (e_ret_status != SFU_SUCCESS)
  {
    STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_ERASE_FAILED);
    SFU_EXCPT_SetError(SFU_EXCPT_DWL_SLOT_ERASING_ERR);
    return e_ret_status;
  }

  return (e_ret_status);
}

/**
  * @brief  Check that there is an Image to Install
  * @param  DwlSlot identification of the downloaded area
  * @param  pFwImageHeader pointer to fw header to be populated
  * @retval SFU_SUCCESS if Image can be installed, a SFU_ERROR  otherwise.
  */
static SFU_ErrorStatus FirmwareToInstall(uint32_t DwlSlot, SE_FwRawHeaderTypeDef *pFwImageHeader)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  uint32_t trailer_begin = (uint32_t) TRAILER_BEGIN(DwlSlot);
  uint32_t end_of_test_image;

  /* Verify FW Image Header tag in dwl slot */
  e_ret_status = CheckAndGetFWHeader(DwlSlot, pFwImageHeader);
  if (e_ret_status == SFU_SUCCESS)
  {
    end_of_test_image = (SlotStartAdd[DwlSlot] + pFwImageHeader->FwSize +
                         SFU_IMG_IMAGE_OFFSET);

    /* Check if there is enough room for the trailers */
    if (trailer_begin < end_of_test_image)
    {
      /*
       * This error causes is not memorized in the BootInfo area because there won't be any error handling
       * procedure.
       * If this function returns that no new firmware can be installed (as this may be a normal case).
       */
      e_ret_status = SFU_ERROR;

#if defined(SFU_VERBOSE_DEBUG_MODE)
      TRACE("\r\n\t  The binary image to be installed overlap with the trailer area!");
#endif /* SFU_VERBOSE_DEBUG_MODE */
    }
    else
    {
      e_ret_status = SFU_SUCCESS;
    }
  }

  return e_ret_status;
}

#endif /* (SFU_NO_SWAP) */



/* Functions Definition : services ------------------------------------------- */

#if  defined(SFU_NO_SWAP)
/**
  * @brief Checks if there is a pending firmware installation.
  *        2 situations can occur:
  *        A. Pending firmware installation: a firmware is ready to be installed.
  *        B. No firmware installation pending
  * @note  The anti-rollback check is not taken into account at this stage.
  * @note  This function populates the FWIMG module variables: fw_image_header_to_test
  * @param  pDwlSlotToInstall identification of the downloaded slot to install
  * @param  pActiveSlotToResume identification of the active slot to resume
  * @param  pActiveSlotToRollback identification of the active slot to rollback
  * @retval SFU_IMG_ImgInstallStateTypeDef Pending Installation status (pending install, no pending action)
  */
SFU_IMG_ImgInstallStateTypeDef SFU_IMG_CheckPendingInstallation(uint32_t *pDwlSlotToInstall, \
                                                                uint32_t *pActiveSlotToResume, \
                                                                uint32_t *pActiveSlotToRollback)
{
  SFU_IMG_ImgInstallStateTypeDef e_ret_state = SFU_IMG_NO_FWUPDATE;
  uint32_t i;
  uint32_t j;

  /* No rollback capability in "NO_SWAP" configuration */
  *pActiveSlotToResume = SLOT_INACTIVE;
  *pActiveSlotToRollback = SLOT_INACTIVE;
  *pDwlSlotToInstall = SLOT_INACTIVE;

  /* Check image installation stopped
     ================================ */
  for (i = 0U; i < SFU_NB_MAX_ACTIVE_IMAGE; i++)
  {
    /* Slot configured ? */
    if (SlotStartAdd[SLOT_ACTIVE_1 + i] != 0U)
    {
      for (j = 0U; j < SFU_NB_MAX_DWL_AREA; j++)
      {
        /* Slot configured ? */
        if (SlotStartAdd[SLOT_DWL_1 + j] != 0U)
        {
          if (SFU_SUCCESS == FirmwareToResume(SLOT_ACTIVE_1 + i, SLOT_DWL_1 + j, &fw_image_header_validated,
                                               &fw_image_header_to_test))
          {
            /* A Firmware Update has been stopped
             * fw_image_header_to_test & fw_image_header_validated have been populated
             * Stop at first occurrence : next ones if any will be resumed after reset
             */
            *pActiveSlotToResume = SLOT_ACTIVE_1 + i;
            *pDwlSlotToInstall = SLOT_DWL_1 + j;
            return SFU_IMG_FWUPDATE_STOPPED;
          }
        }
      }
    }
  }

  /* Install new firmware if requested
     ================================= */
  for (i = 0U; i < SFU_NB_MAX_DWL_AREA; i++)
  {
    /* Slot configured ? */
    if (SlotStartAdd[SLOT_DWL_1 + i] != 0U)
    {
      if (SFU_SUCCESS == FirmwareToInstall(SLOT_DWL_1 + i, &fw_image_header_to_test))
      {
        /*
         * A Firmware is available for installation:
         * fw_image_header_to_test has been populated
         * Stop at first occurrence : next ones if any will be installed after reset
         */
        *pDwlSlotToInstall = SLOT_DWL_1 + i;
        return SFU_IMG_FWIMAGE_TO_INSTALL;
      }
    }
  }

  return e_ret_state;
}

/**
  * @brief Resume installation : not required but present for compatibility with swap process
  *        If installation procedure is interrupted (e.g.: switch off) it will be restarted from begin at next reboot.
  * @param  ActiveSlot identification of the active image
  * @param  DwlSlot identification of the downloaded area
  * @retval SFU_ErrorStatus SFU_SUCCESS
  */
SFU_ErrorStatus SFU_IMG_TriggerResumeInstallation(uint32_t ActiveSlot, uint32_t DwlSlot)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SFU_FLASH_StatusTypeDef x_flash_info;

  /*
   * Decrypt the FW image : from dwl slot to active slot
   */
  e_ret_status = DecryptImageFromDwlSlotToActiveSlot(ActiveSlot, DwlSlot, &fw_image_header_to_test);
  if (e_ret_status != SFU_SUCCESS)
  {
    SFU_EXCPT_SetError(SFU_EXCPT_DECRYPT_ERR);
  }

  /*
   * erase downloaded FW after installation
   */
  e_ret_status = SFU_LL_FLASH_Erase_Size(&x_flash_info, (uint8_t *) SlotStartAdd[DwlSlot], SLOT_SIZE(DwlSlot));
  if (e_ret_status != SFU_SUCCESS)
  {
    STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_ERASE_FAILED);
    SFU_EXCPT_SetError(SFU_EXCPT_DWL_SLOT_ERASING_ERR);
  }

#if defined(SFU_VERBOSE_DEBUG_MODE)
  if (e_ret_status == SFU_SUCCESS)
  {
    TRACE("\r\n\t  Resume procedure completed.");
  }
  else
  {
    TRACE("\r\n\t  Resume procedure cannot be finalized!");
  }
#endif /* SFU_VERBOSE_DEBUG_MODE */

  return (e_ret_status);
}

/**
  * @brief Installs a new firmware, performs the post-installation checks and sets the metadata to tag this firmware as
  *        valid if the checks are successful.
  *        Cryptographic operations are used (if enabled): the firmware is decrypted and its authenticity is checked
  *        afterwards if the cryptographic scheme allows this (signature check for instance).
  *        The detailed errors are memorized in bootinfo area to be processed as critical errors if needed.
  *        This function modifies the FLASH content.
  *        If this procedure is interrupted (e.g.: switch off) installation will be restarted from begin at next reboot.
  * @param  DwlSlot identification of the downloaded area
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_TriggerImageInstallation(uint32_t DwlSlot)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  uint8_t *pbuffer;
  uint32_t active_slot;

  active_slot = SFU_IMG_GetFwImageSlot(&fw_image_header_to_test);

  /*
   * Check the active slot is big enough
   */
  if (fw_image_header_to_test.FwSize + SFU_IMG_IMAGE_OFFSET <= SLOT_SIZE(active_slot))
  {
    e_ret_status = SFU_SUCCESS;
  }

  /*
   * Populate fw_image_header_validated
   */
  if (SFU_SUCCESS == e_ret_status)
  {
    pbuffer = (uint8_t *) SlotHeaderAdd[active_slot];
    e_ret_status = SFU_LL_FLASH_Read((uint8_t *) &fw_image_header_validated, pbuffer,
                                     sizeof(fw_image_header_validated));
  }

  /*
   * Installing the new firmware image consists in decrypting the candidate firmware image.
   * fw_image_header_to_test is already populated after SFU_IMG_CheckPendingInstallation() call.
   */
  if (SFU_SUCCESS == e_ret_status)
  {
    e_ret_status = InstallNewVersion(active_slot, DwlSlot, &fw_image_header_validated, &fw_image_header_to_test);
  }

  if (SFU_SUCCESS != e_ret_status)
  {
    /* Erase downloaded FW in case of installation  error */
    (void) SFU_IMG_EraseDownloadedImg(DwlSlot);
  }

#if defined(SFU_VERBOSE_DEBUG_MODE)
  if (e_ret_status == SFU_SUCCESS)
  {
    TRACE("\r\n\t  Installation procedure completed.");
  }
  else
  {
    TRACE("\r\n\t  Installation procedure cannot be finalized!");
  }
#endif /* SFU_VERBOSE_DEBUG_MODE */

  /* return the installation result */
  return (e_ret_status);
}
#endif /* (SFU_NO_SWAP) */


#if   defined(SFU_NO_SWAP)
/**
  * @brief  Erase downloaded firmware in case of decryption/authentication/integrity error
  * @param  DwlSlot identification of the downloaded area
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus SFU_IMG_EraseDownloadedImg(uint32_t DwlSlot)
{
  SFU_FLASH_StatusTypeDef flash_if_status;
  SFU_ErrorStatus e_ret_status_dwl = SFU_ERROR;

  e_ret_status_dwl = SFU_LL_FLASH_Erase_Size(&flash_if_status, (uint8_t *) SlotStartAdd[DwlSlot],
                                             SLOT_SIZE(DwlSlot));
  STATUS_FWIMG(e_ret_status_dwl == SFU_ERROR, SFU_IMG_FLASH_ERASE_FAILED);

  return ((e_ret_status_dwl != SFU_SUCCESS) ?  SFU_ERROR : SFU_SUCCESS);
}

#endif /* (SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER) || (SFU_NO_SWAP) */