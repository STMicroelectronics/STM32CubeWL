/**
  ******************************************************************************
  * @file    sfu_fwimg_common.c
  * @author  MCD Application Team
  * @brief   This file provides set of firmware functions to manage the Firmware Images.
  *          This file contains the "common" functionalities of the image handling.
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

#define SFU_FWIMG_COMMON_C

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "sfu_fsm_states.h" /* needed for sfu_error.h */
#include "sfu_error.h"
#include "sfu_low_level.h"
#include "sfu_low_level_flash_int.h"
#include "sfu_low_level_security.h"
#include "se_interface_bootloader.h"
#include "sfu_interface_crypto_scheme.h"
#include "sfu_fwimg_regions.h"
#include "sfu_fwimg_services.h"
#include "sfu_fwimg_internal.h"
#include "sfu_trace.h"
#include "sfu_boot.h"
#include "sfu_mpu_isolation.h"        /* Secure Engine isolated thanks to MPU */



/* Private variables ---------------------------------------------------------*/
static uint8_t fw_tag_validated[SFU_NB_MAX_ACTIVE_IMAGE][SE_TAG_LEN];

/* Global variables ----------------------------------------------------------*/
/**
  *  FWIMG status variables used to log errors and display debug messages.
  *  This is related to FLASH operations.
  *  This is handled with STATUS_FWIMG.
  */
SFU_IMG_StatusTypeDef SFU_IMG_Status;
uint32_t SFU_IMG_Line;

/**
  * FW header (metadata) of the active FW in active slot: structured format (access by fields)
  */
SE_FwRawHeaderTypeDef fw_image_header_validated;

#if   (SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER)
/**
  * FW header (metadata) of the candidate FW in dwl slot: structured format (access by fields)
  */
SE_FwRawHeaderTypeDef fw_image_header_to_test;
#endif /* (SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER) */

/* Functions Definition : helper ---------------------------------------------*/
/**
  * @brief  Memory compare with constant time execution.
  * @note   Objective is to avoid basic attacks based on time execution
  * @param  pAdd1 Address of the first buffer to compare
  * @param  pAdd2 Address of the second buffer to compare
  * @param  Size Size of the comparison
  * @retval SFU_ SUCCESS if equal, a SFU_error otherwise.
  */
SFU_ErrorStatus MemoryCompare(uint8_t *pAdd1, uint8_t *pAdd2, uint32_t Size)
{
  uint8_t result = 0x00U;
  uint32_t i;

  for (i = 0U; i < Size; i++)
  {
    result |= pAdd1[i] ^ pAdd2[i];
  }

  if (result == 0x00U)
  {
    return SFU_SUCCESS;
  }
  else
  {
    return SFU_ERROR;
  }
}


/**
  * @brief  Check header signature
  * @param  pFwImageHeader: pointer to raw header.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus VerifyHeaderSignature(SE_FwRawHeaderTypeDef *pFwImageHeader)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SE_StatusTypeDef se_status;
  uint8_t result = 0x00U;
  uint32_t i;

  /* Check if the FW header is authentic : SFU1 / SFU2 / SFU3 */
  if (SFU_IMG_GetFwImageSlot(pFwImageHeader) != SLOT_INACTIVE)
  {
    /* Unused bytes should be set at 0x00 value
       Comparison with constant time execution */
    for (i = 0U; i < sizeof(pFwImageHeader->Reserved); i++)
    {
      result |= pFwImageHeader->Reserved[i] ^ 0x00U;
    }

    if (result == 0x00U)
    {
      /* Check signature of the FW header */
      if (SE_VerifyHeaderSignature(&se_status, pFwImageHeader) == SE_SUCCESS)
      {
        FLOW_STEP(uFlowCryptoValue, FLOW_STEP_AUTHENTICATE);
        e_ret_status = SFU_SUCCESS;
      }
    }
  }

  return e_ret_status;
}


/**
  * @brief Secure Engine Firmware signature verification (FW in non contiguous area).
  *        It handles Firmware TAG verification of a complete buffer by calling
  *        SE_AuthenticateFW_Init, SE_AuthenticateFW_Append and SE_AuthenticateFW_Finish inside the firewall.
  * @note: AES_GCM tag: In order to verify the TAG of a buffer, the function will re-encrypt it
  *        and at the end compare the obtained TAG with the one provided as input
  *        in pSE_GMCInit parameter.
  * @note: SHA-256 tag: a hash of the firmware is performed and compared with the digest stored in the Firmware header.
  * @param pSE_Status: Secure Engine Status.
  *        This parameter can be a value of @ref SE_Status_Structure_definition.
  * @param SlotNumber index of the slot in the list
  * @param pSE_Metadata: Firmware metadata.
  * @param pSE_Payload: pointer to Payload Buffer descriptor.
  * @param SE_FwType: Type of Fw Image.
  *        This parameter can be SE_FW_IMAGE_COMPLETE or SE_FW_IMAGE_PARTIAL.
  * @retval SE_ErrorStatus SE_SUCCESS if successful, SE_ERROR otherwise.
  */
SFU_ErrorStatus VerifyFwSignatureScatter(SE_StatusTypeDef *pSeStatus, uint32_t SlotNumber,
                                         SE_FwRawHeaderTypeDef *pSE_Metadata,
                                         SE_Ex_PayloadDescTypeDef  *pSE_Payload, uint32_t SE_FwType)
{
  SE_ErrorStatus se_ret_status = SE_ERROR;
  SFU_ErrorStatus sfu_ret_status = SFU_SUCCESS;
  /* Loop variables */
  uint32_t i;
  uint32_t j;
  /* Variables to handle the FW image chunks to be injected in the verification procedure and the result */
  uint32_t fw_tag_len;                 /* length of the authentication tag to be verified */
  uint32_t fw_verified_total_size = 0; /* number of bytes that have been processed during authentication check */
  uint32_t fw_chunk_size;              /* size of a FW chunk to be verified */
  /* Authentication tag computed in this procedure (to be compared with the one stored in the FW metadata) */
  uint8_t fw_tag_output[SE_TAG_LEN] __attribute__((aligned(8)));
  /* FW chunk produced by the verification procedure if any   */
  uint8_t fw_chunk[CHUNK_SIZE_SIGN_VERIFICATION] __attribute__((aligned(8)));
  /* FW chunk provided as input to the verification procedure */
  uint8_t fw_image_chunk[CHUNK_SIZE_SIGN_VERIFICATION] __attribute__((aligned(8)));
  /* Variables to handle the FW image (this will be split in chunks) */
  uint32_t payloadsize;
  uint32_t ppayload;
  uint32_t scatter_nb;
  /* Variables to handle FW image size and tag */
  uint32_t fw_size;
  uint8_t *fw_tag;

  /* Check the pointers allocation */
  if ((pSeStatus == NULL) || (pSE_Metadata == NULL) || (pSE_Payload == NULL))
  {
    return SFU_ERROR;
  }
  if ((pSE_Payload->pPayload[0] == 0U) || ((pSE_Payload->pPayload[1] == 0U) && (pSE_Payload->PayloadSize[1] != 0U)))
  {
    return SFU_ERROR;
  }

  /* Check the parameters value and set fw_size and fw_tag to check */
  if (SE_FwType == SE_FW_IMAGE_COMPLETE)
  {
    fw_size = pSE_Metadata->FwSize;
    fw_tag = pSE_Metadata->FwTag;
  }
  else if (SE_FwType == SE_FW_IMAGE_PARTIAL)
  {
    fw_size = pSE_Metadata->PartialFwSize;
    fw_tag = pSE_Metadata->PartialFwTag;
  }
  else
  {
    return SFU_ERROR;
  }

  if ((pSE_Payload->PayloadSize[0] + pSE_Payload->PayloadSize[1]) != fw_size)
  {
    return SFU_ERROR;
  }

  /*  fix number of scatter block */
  if (pSE_Payload->PayloadSize[1] != 0U)
  {
    scatter_nb = 2U;
  }
  else
  {
    scatter_nb = 1U;
  }


  /* Encryption process*/
  se_ret_status = SE_AuthenticateFW_Init(pSeStatus, pSE_Metadata, SE_FwType);

  /* check for initialization errors */
  if ((se_ret_status == SE_SUCCESS) && (*pSeStatus == SE_OK))
  {
    for (j = 0; j < scatter_nb; j++)
    {
      payloadsize = pSE_Payload->PayloadSize[j];
      ppayload = pSE_Payload->pPayload[j];
      i = 0;
      fw_chunk_size = CHUNK_SIZE_SIGN_VERIFICATION;

      while ((i < (payloadsize / CHUNK_SIZE_SIGN_VERIFICATION)) && (*pSeStatus == SE_OK) &&
             (sfu_ret_status == SFU_SUCCESS))
      {

        sfu_ret_status = SFU_LL_FLASH_Read(fw_image_chunk, (uint8_t *)ppayload, fw_chunk_size) ;
        if (sfu_ret_status == SFU_SUCCESS)
        {
          se_ret_status = SE_AuthenticateFW_Append(pSeStatus, fw_image_chunk, (int32_t)fw_chunk_size,
                                                   fw_chunk, (int32_t *)&fw_chunk_size);
        }
        else
        {
          *pSeStatus = SE_ERR_FLASH_READ;
          se_ret_status = SE_ERROR;
          sfu_ret_status = SFU_ERROR;
        }
        ppayload += fw_chunk_size;
        fw_verified_total_size += fw_chunk_size;
        i++;
      }
      /* this the last path , size can be smaller */
      fw_chunk_size = pSE_Payload->pPayload[j] + pSE_Payload->PayloadSize[j] - ppayload;
      if ((fw_chunk_size != 0U) && (se_ret_status == SE_SUCCESS) && (*pSeStatus == SE_OK))
      {
        sfu_ret_status = SFU_LL_FLASH_Read(fw_image_chunk, (uint8_t *)ppayload, fw_chunk_size) ;
        if (sfu_ret_status == SFU_SUCCESS)
        {

          se_ret_status = SE_AuthenticateFW_Append(pSeStatus, fw_image_chunk,
                                                   (int32_t)(payloadsize - (i * CHUNK_SIZE_SIGN_VERIFICATION)),
                                                   fw_chunk, (int32_t *)&fw_chunk_size);
        }
        else
        {
          *pSeStatus = SE_ERR_FLASH_READ;
          se_ret_status = SE_ERROR;
          sfu_ret_status = SFU_ERROR;
        }
        fw_verified_total_size += fw_chunk_size;
      }
    }
  }

  if ((sfu_ret_status == SFU_SUCCESS) && (se_ret_status == SE_SUCCESS) && (*pSeStatus == SE_OK))
  {
    if (fw_verified_total_size <= fw_size)
    {
      /* Do the Finalization, check the authentication TAG*/
      fw_tag_len = sizeof(fw_tag_output);
      se_ret_status =   SE_AuthenticateFW_Finish(pSeStatus, fw_tag_output, (int32_t *)&fw_tag_len);

      if ((se_ret_status == SE_SUCCESS) && (*pSeStatus == SE_OK) && (fw_tag_len == SE_TAG_LEN))
      {
        /* Firmware tag verification */
        if (MemoryCompare(fw_tag_output, fw_tag, SE_TAG_LEN) != SFU_SUCCESS)
        {
          *pSeStatus = SE_SIGNATURE_ERR;
          se_ret_status = SE_ERROR;
          sfu_ret_status = SFU_ERROR;

          /* Save result for active slot :
             to avoid single fault attack the signature will be double checked before UserApp execution */
          if ((SlotNumber >= SLOT_ACTIVE_1) && (SlotNumber < SLOT_DWL_1))
          {
            (void) memset(&fw_tag_validated[SlotNumber - SLOT_ACTIVE_1][0U], 0x00, SE_TAG_LEN);
          }
        }
        else
        {
          FLOW_STEP(uFlowCryptoValue, FLOW_STEP_INTEGRITY);

          /* Save result for active slot :
             to avoid single fault attack the signature will be double checked before UserApp execution */
          if ((SlotNumber >= SLOT_ACTIVE_1) && (SlotNumber < SLOT_DWL_1))
          {
            (void) memcpy(&fw_tag_validated[SlotNumber - SLOT_ACTIVE_1][0U], fw_tag_output, SE_TAG_LEN);
          }
        }
      }
      else
      {
        sfu_ret_status = SFU_ERROR;
      }
    }
    else
    {
      sfu_ret_status = SFU_ERROR;
    }
  }
  else
  {
    sfu_ret_status = SFU_ERROR;
  }
  return sfu_ret_status;
}

/**
  * @brief Fill authenticated info in SE_FwImage.
  * @param SFU_APP_Status
  * @param pBuffer
  * @param BufferSize
  * @retval SFU_SUCCESS if successful, a SFU_ERROR otherwise.
  */
SFU_ErrorStatus ParseFWInfo(SE_FwRawHeaderTypeDef *pFwHeader, uint8_t *pBuffer)
{
  /* Check the pointers allocation */
  if ((pFwHeader == NULL) || (pBuffer == NULL))
  {
    return SFU_ERROR;
  }
  (void) memcpy(pFwHeader, pBuffer, sizeof(*pFwHeader));
  return SFU_SUCCESS;
}

/**
  * @brief  Verify Image Header in the slot given as a parameter
  * @param  SlotNumber index of the slot in the list
  * @param  pFwImageHeader pointer to a structure to handle the header info (filled by this function)
  * @note   Not used in Alpha version -
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus CheckAndGetFWHeader(uint32_t SlotNumber, SE_FwRawHeaderTypeDef *pFwImageHeader)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* use api read to detect possible ECC error */
  e_ret_status = SFU_LL_FLASH_Read((uint8_t *) pFwImageHeader, (uint8_t *) SlotHeaderAdd[SlotNumber],
                                   sizeof(SE_FwRawHeaderTypeDef));
  if (e_ret_status == SFU_SUCCESS)
  {
    /* Verify signature */
    e_ret_status = VerifyHeaderSignature(pFwImageHeader);
  }
  return e_ret_status;
}

/**
  * @brief  Verify Fw signature of binary contiguous in flash
  * @param  pSeStatus pointer giving the SE status result
  * @param  SlotNumber flash slot to check
  * @param  pFwImageHeader pointer to fw header
  * @param  SlotNumber flash slot to check
  * @param SE_FwType: Type of Fw Image.
  *        This parameter can be SE_FW_IMAGE_COMPLETE or SE_FW_IMAGE_PARTIAL.
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus VerifyFwSignature(SE_StatusTypeDef  *pSeStatus, uint32_t SlotNumber,
                                  SE_FwRawHeaderTypeDef *pFwImageHeader,
                                  uint32_t SE_FwType)
{
  SE_Ex_PayloadDescTypeDef  pse_payload;
  uint32_t fw_size;
  uint32_t fw_offset;

  /*  put it OK, to discriminate error in SFU FWIMG parts */
  *pSeStatus = SE_OK;

  /* Check the parameters value */
  if (pFwImageHeader == NULL)
  {
    return SFU_ERROR;
  }
  if ((SE_FwType != SE_FW_IMAGE_PARTIAL) && (SE_FwType != SE_FW_IMAGE_COMPLETE))
  {
    return SFU_ERROR;
  }

  /* Check SE_FwType parameter, and fix size and offset accordingly */
  switch (SE_FwType)
  {
    case SE_FW_IMAGE_COMPLETE :
      fw_size = pFwImageHeader->FwSize;
      fw_offset = 0U;
      break;
    case SE_FW_IMAGE_PARTIAL :
      fw_size = pFwImageHeader->PartialFwSize;
      fw_offset = pFwImageHeader->PartialFwOffset;
      break;
    default :
      /* error */
      fw_size = 0U;
      fw_offset = 0U;
      break;
  }

  pse_payload.pPayload[0] = SlotStartAdd[SlotNumber] + fw_offset + SFU_IMG_IMAGE_OFFSET;
  pse_payload.PayloadSize[0] = fw_size;
  pse_payload.pPayload[1] = 0U;
  pse_payload.PayloadSize[1] = 0U;

  return  VerifyFwSignatureScatter(pSeStatus, SlotNumber, pFwImageHeader, &pse_payload, SE_FwType);
}

/**
  * @brief Verifies the validity of a slot.
  * @note  Control if there is no additional code beyond the firmware image (malicious SW).
  * @param pSlotBegin Start address of a slot.
  * @param uSlotSize Size of a slot.
  * @param uFwSize Size of the firmware image.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus VerifySlot(uint8_t *pSlotBegin, uint32_t uSlotSize, uint32_t uFwSize)
{
  uint8_t *pdata;
  uint32_t length;
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Check is already clean */
  pdata = pSlotBegin + SFU_IMG_IMAGE_OFFSET + uFwSize;
  length = uSlotSize - SFU_IMG_IMAGE_OFFSET - uFwSize;
  e_ret_status = SFU_LL_FLASH_Compare(pdata, 0x00000000U, 0xFFFFFFFFU, length);

  return e_ret_status;
}

/**
  * @brief  This function clean-up the flash
  * @note   Not designed to clean-up area inside secure engine isolation.
  * @note   To clean the slot without erasing the header we write "0x00" except if already written
  * @param  pSlotBegin Start address of a slot.
  * @param  uSlotSize Size of a slot.
  * @param  uOffset Offset in the slot to start cleanup, until end of the slot.
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, SFU_ERROR otherwise.
  */
SFU_ErrorStatus CleanUpSlot(uint8_t *pSlotBegin, uint32_t uSlotSize, uint32_t uOffset)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  uint32_t pdata;
  uint32_t slot_begin;
  SFU_LL_FLASH_write_t pattern;
  SFU_LL_FLASH_write_t value;
  SFU_FLASH_StatusTypeDef flash_if_info;

  /* Clean-up pattern initialization */
  (void) memset((void *)&pattern, 0x00, sizeof(SFU_LL_FLASH_write_t));

  /* Cleaning operation */
  slot_begin = (uint32_t) pSlotBegin;
  for (pdata = slot_begin + uOffset; pdata < (slot_begin + uSlotSize); pdata += sizeof(SFU_LL_FLASH_write_t))
  {
    e_ret_status =  SFU_LL_FLASH_Read((uint8_t *) &value, (uint8_t *) pdata, sizeof(SFU_LL_FLASH_write_t));

    if (e_ret_status == SFU_SUCCESS)
    {
      /* If read succeed, then the pattern write is performed only if pattern is not already present */
      if (memcmp((void *) &value, (void *) &pattern, sizeof(SFU_LL_FLASH_write_t)) != 0)
      {
        e_ret_status = SFU_LL_FLASH_Write(&flash_if_info, (uint8_t *) pdata, (uint8_t *) &pattern,
                                          sizeof(SFU_LL_FLASH_write_t));
      }
    }
    else
    {
      /* If read fail, then the write is performed unconditionally */
      e_ret_status = SFU_LL_FLASH_Write(&flash_if_info, (uint8_t *) pdata, (uint8_t *) &pattern,
                                        sizeof(SFU_LL_FLASH_write_t));
    }

    /* Reload Watchdog every 4Kbytes */
    if ((pdata % 0x1000U) == 0U)
    {
      (void) SFU_LL_SECU_IWDG_Refresh();
    }
  }
  return e_ret_status;
}

/* Functions Definition : services ------------------------------------------- */

/**
  * @brief FW Image Handling (FWIMG) initialization function.
  *        Checks the validity of the settings related to image handling (slots size and alignments...).
  * @note  The system initialization must have been performed before calling this function (flash driver ready to be
  *        used...etc...).
  *        Must be called first (and once) before calling the other Image handling services.
  * @param  None.
  * @retval SFU_IMG_InitStatusTypeDef SFU_IMG_INIT_OK if successful, an error code otherwise.
  */
SFU_IMG_InitStatusTypeDef SFU_IMG_InitImageHandling(void)
{
  uint32_t i;
  uint32_t mpu_size;
  uint32_t mpu_max_size;
  uint32_t mpu_sub_size;
  SFU_IMG_InitStatusTypeDef e_ret_status = SFU_IMG_INIT_OK;

  /*
   * When there is no valid FW in active, the fw_image_header_validated structure is filled with 0s.
   * When installing a first FW (after local download) this means that WRITE_TRAILER_MAGIC will write a SWAP magic
   * starting with 0s.
   * This causes an issue when calling CLEAN_TRAILER_MAGIC (because of this we added an erase that generated
   * side-effects).
   * To avoid all these problems we can initialize fw_image_header_validated with a non-0  value.
   */
  (void) memset((uint8_t *) &fw_image_header_validated, 0xFE, sizeof(fw_image_header_validated));

  /*
   * Sanity check: let's make sure all actives slots are properly aligned with regards to flash constraints
   */
  for (i = 0U; i < SFU_NB_MAX_ACTIVE_IMAGE; i++)
  {
#if defined(__GNUC__)
    __IO uint32_t start_add;
    start_add = SlotStartAdd[SLOT_ACTIVE_1 + i];
    if (start_add != 0U)
#else
    if (SlotStartAdd[SLOT_ACTIVE_1 + i] != 0U)
#endif /* (__GNUC__) */
    {
      if (!IS_ALIGNED(SlotStartAdd[SLOT_ACTIVE_1 + i]))
      {
        e_ret_status = SFU_IMG_INIT_FLASH_CONSTRAINTS_ERROR;
        TRACE("\r\n= [FWIMG] SLOT_ACTIVE_%d (%x) is not properly aligned\r\n",
              i + 1U, SlotStartAdd[SLOT_ACTIVE_1 + i]);
      } /* else active slot is properly aligned */
    }
  }


  /*
   * Sanity check: let's make sure the MAGIC patterns used by the internal algorithms match the FLASH constraints.
   */
  if (0U != (uint32_t)(MAGIC_LENGTH % (uint32_t)sizeof(SFU_LL_FLASH_write_t)))
  {
    e_ret_status = SFU_IMG_INIT_FLASH_CONSTRAINTS_ERROR;
    TRACE("\r\n= [FWIMG] magic size (%d) is not matching the FLASH constraints\r\n", MAGIC_LENGTH);
  } /* else the MAGIC patterns size is fine with regards to FLASH constraints */

  /*
   * Sanity check: let's make sure the Firmware Header Length is fine with regards to FLASH constraints
   */
  if (0U != (uint32_t)(SE_FW_HEADER_TOT_LEN % (uint32_t)sizeof(SFU_LL_FLASH_write_t)))
  {
    /* The code writing the FW header in FLASH requires the FW Header length to match the FLASH constraints */
    e_ret_status = SFU_IMG_INIT_FLASH_CONSTRAINTS_ERROR;
    TRACE("\r\n= [FWIMG] FW Header size (%d) is not matching the FLASH constraints\r\n", SE_FW_HEADER_TOT_LEN);
  } /* else the FW Header Length is fine with regards to FLASH constraints */


  /*
   * Sanity check: let's make sure the active slot headers do not overlap SB code area protected by WRP
   */
  for (i = 0U; i < SFU_NB_MAX_ACTIVE_IMAGE; i++)
  {
#if defined(__GNUC__)
    __IO uint32_t start_add;
    start_add = SlotStartAdd[SLOT_ACTIVE_1 + i];
    if (start_add != 0U)
#else
    if (SlotStartAdd[SLOT_ACTIVE_1 + i] != 0U)
#endif /* (__GNUC__) */
    {
      if(!((((SlotStartAdd[SLOT_ACTIVE_1 + i]   - FLASH_BASE) / FLASH_PAGE_SIZE) > SFU_PROTECT_WRP_PAGE_END_1 )
        || (((SlotEndAdd[SLOT_ACTIVE_1 + i] + 1 - FLASH_BASE) / FLASH_PAGE_SIZE) < SFU_PROTECT_WRP_PAGE_START_1)))
      {
        TRACE("\r\n= [FWIMG] SLOT_ACTIVE_%d overlaps SBSFU code area protected by WRP\r\n", i + 1U);
        e_ret_status = SFU_IMG_INIT_FLASH_CONSTRAINTS_ERROR;
      }
    }
  }



  /*
   * Sanity check: let's make sure headers are under the MPU protection
   */
  /* Calculate size of image header MPU region */
  mpu_max_size = 2U;
  for (i = 0U; i < SFU_PROTECT_MPU_HEADER_SIZE; i++)
  {
    mpu_max_size = mpu_max_size * 2U;
  }
  /* Take into account 8 sub-regions (assuming they are continuous) */
  mpu_sub_size = mpu_max_size / 8U;
  mpu_size = 0U;
  for (i = 0U; i < 8U; i++)
  {
    if (((~SFU_PROTECT_MPU_HEADER_SREG) & (0x1U << i)) != 0U)
    {
      mpu_size = mpu_size + mpu_sub_size;
    }
  }

  for (i = 0U; i < SFU_NB_MAX_ACTIVE_IMAGE; i++)
  {
#if defined(__GNUC__)
    __IO uint32_t start_add;
    start_add = SlotStartAdd[SLOT_ACTIVE_1 + i];
    if (start_add != 0U)
#else
    if (SlotStartAdd[SLOT_ACTIVE_1 + i] != 0U)
#endif /* (__GNUC__) */
    {
      if (!((SlotHeaderAdd[SLOT_ACTIVE_1 + i] >= SFU_PROTECT_MPU_HEADER_START)
            && ((SlotHeaderAdd[SLOT_ACTIVE_1 + i] + SE_FW_HEADER_TOT_LEN) <=
                (SFU_PROTECT_MPU_HEADER_START + mpu_size))))
      {
        TRACE("\r\n= [FWIMG] Header of SLOT_ACTIVE_%d is not under the MPU protection\r\n", i + 1U);
        e_ret_status = SFU_IMG_INIT_FLASH_CONSTRAINTS_ERROR;
      }
    }
  }


  /*
   * Sanity check: let's make sure the KMS NVM area does not overlap SB code area
   */
  if (!(((SB_REGION_ROM_START) > KMS_DATASTORAGE_END)
        || ((SB_REGION_ROM_END) < (KMS_DATASTORAGE_START))))
  {
    TRACE("\r\n= [FWIMG] KMS NVM area overlaps SB Code area\r\n");
    e_ret_status = SFU_IMG_INIT_FLASH_CONSTRAINTS_ERROR;
  }

  /*
   * Sanity check: let's make sure the KMS NVM area does not overlap active slots area
   */
  for (i = 0U; i < SFU_NB_MAX_ACTIVE_IMAGE; i++)
  {
    if (SlotStartAdd[SLOT_ACTIVE_1 + i] != 0U)
    {
      if (!(((SlotStartAdd[SLOT_ACTIVE_1 + i]) > KMS_DATASTORAGE_END)
            || ((SlotEndAdd[SLOT_ACTIVE_1 + i]) < (KMS_DATASTORAGE_START))))
      {
        TRACE("\r\n= [FWIMG] KMS NVM area overlaps SLOT_ACTIVE_%d area\r\n", i + 1U);
        e_ret_status = SFU_IMG_INIT_FLASH_CONSTRAINTS_ERROR;
      }
    }
  }




  return e_ret_status;
}

/**
  * @brief This function makes sure the current active firmware will not be considered as valid any more.
  *        This function alters the FLASH content.
  * @note It is up to the caller to make sure the conditions to call this primitive are met (no check performed before
  *       running the procedure).
  * @param  SlotNumber index of the slot in the list
  * @retval SFU_SUCCESS if successful,SFU_ERROR error otherwise.
  */
SFU_ErrorStatus SFU_IMG_InvalidateCurrentFirmware(uint32_t SlotNumber)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;

  /* Reload Watchdog */
  (void) SFU_LL_SECU_IWDG_Refresh();

  /* erase active slot except Header (under SECoreBin protection) for the anti-rollback check during next Fw update */
  e_ret_status = CleanUpSlot((uint8_t *) SlotStartAdd[SlotNumber], SLOT_SIZE(SlotNumber), SFU_IMG_IMAGE_OFFSET);

  return (e_ret_status);
}

/**
  * @brief Verifies the validity of the active firmware image metadata.
  * @note This function relies on cryptographic procedures and it is up to the caller to make sure the required
  *       elements have been configured.
  * @note This function populates the FWIMG module variable: fw_image_header_validated
  * @param  SlotNumber index of the slot in the list
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyActiveImgMetadata(uint32_t SlotNumber)
{
  /*
   * If the metadata is valid then 'fw_image_header_validated' is filled with the metadata.
   */
  return (CheckAndGetFWHeader(SlotNumber, &fw_image_header_validated));
}

/**
  * @brief Verifies the validity of the active firmware image.
  * @note This function relies on cryptographic procedures and it is up to the caller to make sure the required
  *       elements have been configured.
  *       Typically, SFU_IMG_VerifyActiveImgMetadata() must have been called first to populate
  *       fw_image_header_validated.
  * @param  SlotNumber index of the slot in the list
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyActiveImg(uint32_t SlotNumber)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SE_StatusTypeDef e_se_status = SE_KO;

  /*
   * fw_image_header_validated MUST have been populated with valid metadata first,
   */
  e_ret_status = VerifyFwSignature(&e_se_status, SlotNumber, &fw_image_header_validated, SE_FW_IMAGE_COMPLETE);
#if defined(SFU_VERBOSE_DEBUG_MODE)
  if (SFU_ERROR == e_ret_status)
  {
    /* We do not memorize any specific error, the FSM state is already providing the info */
    TRACE("\r\n=         SFU_IMG_VerifyActiveImg failure with se_status=%d!", e_se_status);
  }
#endif /* SFU_VERBOSE_DEBUG_MODE */

  return (e_ret_status);
}

/**
  * @brief Verifies if no malicious code beyond Fw Image in an active slot.
  * @note SFU_IMG_VerifyActiveImgMetadata() must have been called first to populate fw_image_header_validated.
  * @param  SlotNumber index of the slot in the list
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyActiveSlot(uint32_t SlotNumber)
{
  /*
   * fw_image_header_validated MUST have been populated with valid metadata first,
   */
  return (VerifySlot((uint8_t *) SlotStartAdd[SlotNumber], SLOT_SIZE(SlotNumber), fw_image_header_validated.FwSize));
}

/**
  * @brief Verifies if no malicious code in active slot.
  * @param  SlotNumber index of the slot in the list
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_VerifyEmptyActiveSlot(uint32_t SlotNumber)
{
  return (VerifySlot((uint8_t *) SlotStartAdd[SlotNumber], SLOT_SIZE(SlotNumber), 0U));
}

/**
  * @brief  Control firmware signature
  * @note   This control will be done twice for security reasons (first control done in VerifyFwSignatureScatter)
  * @param  SlotNumber index of the slot in the list
  * @retval SFU_SUCCESS if successful,SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_ControlActiveImgTag(uint32_t SlotNumber)
{
  /*
   * fw_image_header_validated MUST have been populated with valid metadata first,
   */
  if (MemoryCompare(fw_tag_validated[SlotNumber - SLOT_ACTIVE_1], fw_image_header_validated.FwTag,
                    SE_TAG_LEN) != SFU_SUCCESS)
  {
    return SFU_ERROR;
  }
  else
  {
    FLOW_STEP(uFlowCryptoValue, FLOW_STEP_INTEGRITY);
    return SFU_SUCCESS;
  }

}

/**
  * @brief Launches the user application.
  *        The caller must be prepared to never get the hand back after calling this function.
  *        If a problem occurs, it must be memorized in the bootinfo area.
  *        If the caller gets the hand back then this situation must be handled as a critical error.
  * @note It is up to the caller to make sure the conditions to call this primitive are met
  *       (typically: no security check performed before launching the firmware).
  * @note This function only handles the "system" aspects.
  *       It is up to the caller to manage any security related action (enable ITs, disengage MPU, clean RAM...).
  *       Nevertheless, cleaning-up the stack and heap used by SB_SFU is part of the system actions handled by this
  *       function (as this needs to be done just before jumping into the user application).
  * @param  SlotNumber index of the slot in the list
  * @retval SFU_ErrorStatus Does not return if successful, returns SFU_ERROR otherwise.
  */
SFU_ErrorStatus SFU_IMG_LaunchActiveImg(uint32_t SlotNumber)
{
  SFU_ErrorStatus e_ret_status = SFU_SUCCESS;

  /* Verify if authentication/integrity/lock SE service performed */
  FLOW_CONTROL_CHECK(uFlowCryptoValue, FLOW_CTRL_LOCK_SERVICE);

  /* this function does not return */
  /* first image identified as master image */
  SFU_MPU_SysCall((uint32_t)SB_SYSCALL_LAUNCH_APP, SlotStartAdd[SlotNumber] + SFU_IMG_IMAGE_OFFSET);

  /* The point below should NOT be reached */
  return (e_ret_status);
}

/**
  * @brief Get the version of the active FW
  * @note It is up to the caller to make sure the active slot contains a valid active FW.
  * @note In the current implementation the header is checked (authentication) and no version is returned if this check
  *       fails.
  * @param  SlotNumber index of the slot in the list
  * @retval the FW version if the header is valid (coded on uint16_t), 0 otherwise
  */
uint16_t SFU_IMG_GetActiveFwVersion(uint32_t SlotNumber)
{
  SE_FwRawHeaderTypeDef fw_image_header;
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  uint16_t version = 0;

  /* check the header of the active FW */
  e_ret_status = CheckAndGetFWHeader(SlotNumber, &fw_image_header);
  if (e_ret_status == SFU_SUCCESS)
  {
    /* retrieve the version from the header without any further check */
    version = fw_image_header.FwVersion;
  }

  return (version);
}

/**
  * @brief Detect if a FW is present in a slot.
  * @note  Detection done based on MAGIC header + FW signature. Header verification
  *        not done at this stage for timing optimization.
  * @param  SlotNumber index of the slot in the list
  * @retval SFU_SUCCESS if successful, error code otherwise
  */
SFU_ErrorStatus SFU_IMG_DetectFW(uint32_t SlotNumber)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  uint8_t *pbuffer;
  uint8_t  buffer[SE_FW_HEADER_TOT_LEN];
  SE_FwRawHeaderTypeDef *p_header;

  /* Read header */
  pbuffer = (uint8_t *) SlotHeaderAdd[SlotNumber];
  if (SFU_LL_FLASH_Read(buffer, pbuffer, sizeof(buffer)) == SFU_SUCCESS)
  {
    /* Check if the FW header is authentic : SFU1 / SFU2 / SFU3 */
    p_header = (SE_FwRawHeaderTypeDef *)(uint32_t)buffer;
    if (SFU_IMG_GetFwImageSlot(p_header) != SLOT_INACTIVE)
    {
      /*
       * Control if the first 32 bytes (could be more but time consuming ...) are significant
       * At this location we should find the vector table.
       * Objective is to detect if the FW image has been erased
       * ==> this is the case after SFU_IMG_InvalidateCurrentFirmware() (could be an attack attempt)
       */
      if (VerifySlot((uint8_t *) SlotStartAdd[SlotNumber], SFU_IMG_IMAGE_OFFSET + 0x20U, 0U) != SFU_SUCCESS)
      {
        e_ret_status = SFU_SUCCESS;
      }
    }
  }
  return e_ret_status;
}

/**
  * @brief  Validate the active FW image in active slot by installing the header
  * @param  SlotNumber index of the slot in the list
  * @param  pFwImageHeader pointer in ram to the header to install
  * @retval SFU_SUCCESS if successful, a SFU_ErrorStatus error otherwise.
  */

SFU_ErrorStatus SFU_IMG_Validation(uint32_t SlotNumber, SE_FwRawHeaderTypeDef *pFWImageHeader)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SFU_FLASH_StatusTypeDef flash_if_status;

  /* Header erasing */
  e_ret_status = SFU_LL_FLASH_Erase_Size(&flash_if_status, (uint8_t *) SlotHeaderAdd[SlotNumber], SFU_IMG_IMAGE_OFFSET);
  STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_ERASE_FAILED);

  /* Header writing: encrypted part with signature */
  if (e_ret_status == SFU_SUCCESS)
  {
    e_ret_status = SFU_LL_FLASH_Write(&flash_if_status, (uint8_t *) SlotHeaderAdd[SlotNumber], (uint8_t *) pFWImageHeader,
                                      SE_FW_AUTH_LEN + SE_FW_HEADER_SIGN_LEN);
    STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_WRITE_FAILED);
  }

  /* Header writing : Do not update image state part */

  /* Header writing: previous FW image fingerprint */
  if (e_ret_status == SFU_SUCCESS)
  {
    e_ret_status = SFU_LL_FLASH_Write(&flash_if_status, (uint8_t *)(SlotHeaderAdd[SlotNumber] + SE_FW_HEADER_TOT_LEN -
                                                                    SE_FW_HEADER_FINGERPRINT_LEN),
                                      (uint8_t *) pFWImageHeader->PrevHeaderFingerprint, SE_FW_HEADER_FINGERPRINT_LEN);
    STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_WRITE_FAILED);
  }


  return e_ret_status;
}

/**
  * @brief  Get firmware image ID from header.
  * @param  pFwImageHeader pointer to a structure to handle the header info
  * @retval Fw Id : SLOT_ACTIVE_1 / 2 /... / SFU_NB_MAX_ACTIVE_IMAGE
  */
uint32_t SFU_IMG_GetFwImageSlot(SE_FwRawHeaderTypeDef *pFwImageHeader)
{
  uint32_t active_slot;

  if (memcmp(pFwImageHeader->SFUMagic, SFUM_1, strlen(SFUM_1)) == 0)
  {
    active_slot = SLOT_ACTIVE_1;
  }
#if (SFU_NB_MAX_ACTIVE_IMAGE > 1)
  else if (memcmp(pFwImageHeader->SFUMagic, SFUM_2, strlen(SFUM_2)) == 0)
  {
    active_slot = SLOT_ACTIVE_2;
  }
#endif  /* (SFU_NB_MAX_ACTIVE_IMAGE > 1) */
#if (SFU_NB_MAX_ACTIVE_IMAGE > 2)
  else if (memcmp(pFwImageHeader->SFUMagic, SFUM_3, strlen(SFUM_3)) == 0)
  {
    active_slot = SLOT_ACTIVE_3;
  }
#endif  /* (SFU_NB_MAX_ACTIVE_IMAGE > 2) */
  else
  {
    active_slot = SLOT_INACTIVE;
  }

  return active_slot;
}

/**
  * @brief  Check candidate image version is allowed.
  * @param  ActiveSlot Slot identification used in case of secure counters usage
  * @param  CurrentVersion Version of currently installed image if any
  * @param  CandidateVersion Version of candidate image
  * @retval SFU_SUCCESS if candidate image version is allowed, SFU_ErrorStatus error otherwise.
  */
SFU_ErrorStatus SFU_IMG_CheckFwVersion(uint32_t ActiveSlot, uint16_t CurrentVersion, uint16_t CandidateVersion)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  UNUSED(ActiveSlot);

  /* If the header of the active firmware is not valid, the CurrentVersion is 0 */
  if (CurrentVersion == 0U)
  {
    /*
     * If the header of the active firmware is not valid (could come from an attack attempt, we authorize only the
     * installation of a specific version identified by SFU_FW_VERSION_INIT_NUM.
     */
    if (CandidateVersion == SFU_FW_VERSION_INIT_NUM)
    {
      /* Candidate version is allowed */
      e_ret_status = SFU_SUCCESS;
    }
#if defined(SFU_VERBOSE_DEBUG_MODE)
    else
    {
      TRACE("\r\n\t  Init version:%d - Current version:%d - Candidate version:%d : Installation not allowed!", SFU_FW_VERSION_INIT_NUM, CurrentVersion, CandidateVersion);
    }
#endif /* SFU_VERBOSE_DEBUG_MODE */
  }
  else
  {
    /*
     * It is not allowed to install a Firmware with a lower version than the active firmware.
     * But we authorize the re-installation of the current firmware version.
     * We also check that the candidate version is at least the min. allowed version for this device.
     */
    if ((CandidateVersion >= CurrentVersion) && (CandidateVersion >= SFU_FW_VERSION_START_NUM))
    {
      /* Candidate version is allowed */
      e_ret_status = SFU_SUCCESS;
    }
#if defined(SFU_VERBOSE_DEBUG_MODE)
    else
    {
      TRACE("\r\n\t  Init version:%d - Current version:%d - Candidate version:%d : Installation not allowed!", SFU_FW_VERSION_INIT_NUM, CurrentVersion, CandidateVersion);
    }
#endif /* SFU_VERBOSE_DEBUG_MODE */

  }

  return e_ret_status;
}

#if   (SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER)
/**
  * @brief Verifies the validity of the metadata associated to a candidate image.
  *        The anti-rollback check is performed.
  *        Errors can be memorized in the bootinfo area but no action is taken in this procedure.
  * @note It is up to the caller to make sure the conditions to call this primitive are met (no check performed before
  *       running the procedure):
  *       SFU_IMG_CheckPendingInstallation should be called first.
  * @note For the local loader (@ref SECBOOT_USE_LOCAL_LOADER), the candidate metadata is verified by:
  *       @ref SFU_APP_VerifyFwHeader.
  *       But, SFU_IMG_CheckCandidateVersion will be called when the installation of the new image is triggered.
  * @param  DwlSlot identification of the downloaded area
  * @retval SFU_ErrorStatus SFU_SUCCESS if successful, error code otherwise
  */

/* anti-rollback mechanism is tested twice to avoid basic hardware attack
   ==> compiler optimization is disabled for this reason */
#if defined(__ICCARM__)
#pragma optimize=none
#elif defined(__CC_ARM)
#pragma O0
#elif defined(__ARMCC_VERSION)
__attribute__((optnone))
#elif defined(__GNUC__)
__attribute__((optimize("O0")))
#endif /* __ICCARM__ */

SFU_ErrorStatus SFU_IMG_CheckCandidateVersion(uint32_t DwlSlot)
{
  SFU_ErrorStatus e_ret_status = SFU_ERROR;
  SFU_FLASH_StatusTypeDef flash_if_status;
  uint32_t active_slot;
  uint16_t cur_ver;
  uint16_t verif_ver;

  /*
   * - Candidate FW Header Authentication
   *       An authentication check is already performed with SFU_IMG_CheckPendingInstallation().
   *       As a consequence: fw_image_header_to_test is already populated
   *
   * - Check version is allowed
   *      SFU_IMG_GetActiveFwVersion() returns 0 if the header of active firmware is not valid
   */

  /* Header analysis to find slot based on number with magic tag */
  active_slot = SFU_IMG_GetFwImageSlot(&fw_image_header_to_test);

  /* Get the version of active_slot */
  cur_ver = SFU_IMG_GetActiveFwVersion(active_slot);

  if (SFU_IMG_CheckFwVersion(active_slot, cur_ver, fw_image_header_to_test.FwVersion) != SFU_SUCCESS)
  {
    /* The installation is forbidden */
    /* Memorize this error as this will be handled as a critical failure */
    SFU_EXCPT_SetError(SFU_EXCPT_VERSION_ERR);

    /* We would enter an infinite loop of installation attempts if we do not clean-up the dwl slot header */
    e_ret_status = SFU_LL_FLASH_Erase_Size(&flash_if_status, (uint8_t *)SlotStartAdd[DwlSlot], SFU_IMG_IMAGE_OFFSET);
    STATUS_FWIMG(e_ret_status == SFU_ERROR, SFU_IMG_FLASH_ERASE_FAILED);

    /* leave now to handle the critical failure in the appropriate FSM state */
    e_ret_status = SFU_ERROR;
  }
  else
  {
    /* the anti-rollback check succeeds: the version is fine */
    /* double check to avoid basic hardware attack attack */
    verif_ver = SFU_IMG_GetActiveFwVersion(active_slot);
    e_ret_status = SFU_IMG_CheckFwVersion(active_slot, verif_ver, fw_image_header_to_test.FwVersion);
  }

  /* Return the result */
  return (e_ret_status);
}
#endif /* SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER */
