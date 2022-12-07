/**
  ******************************************************************************
  * @file    sfu_fwimg_internal.h
  * @author  MCD Application Team
  * @brief   This file contains internal definitions (private) for SFU_FWIMG functionalities.
  *          This file should be included only by sfu_fwimg_core.c and sfu_fwimg_services.c.
  *          Nevertheless, the SFU_KMS module is allowed to include it to re-use the variable
  *          fw_image_header_to_test to install an KMS blob.
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
#ifndef SFU_FWIMG_INTERNAL_H
#define SFU_FWIMG_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "se_def.h"
#include "sfu_fwimg_regions.h"
#include "kms_blob_metadata.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief Payload Buffer descriptor.
  * This structure describes how a Firmware is split in Flash.
  * In the nominal case a Firmware is stored in 1 contiguous area.
  * But, a Firmware can be split in up to 2 areas (typically when a FW installation procedure is interrupted or after a
  * decrypt operation).
  */
typedef struct
{
  uint32_t  pPayload[2];               /*!<  table containing payload pointer*/
  uint32_t  PayloadSize[2];            /*!<  table containing Payload Size*/
} SE_Ex_PayloadDescTypeDef;

/* Exported constants --------------------------------------------------------*/
/**
  * @brief SFU_IMG Flash Status Type Definition
  * Status of a FLASH operation.
  */
typedef enum
{
  SFU_IMG_OK = 0x0U,             /*!< No problem reported */
  SFU_IMG_FLASH_ERASE_FAILED,    /*!< FLASH erase failure */
  SFU_IMG_FLASH_WRITE_FAILED,    /*!< FLASH write failure */
  SFU_IMG_FLASH_READ_FAILED,     /*!< FLASH read failure */
} SFU_IMG_StatusTypeDef;

/**
  * @brief RAM chunk used for decryption / comparison / swap
  * it is the size of RAM buffer allocated in stack and used for decrypting/moving images.
  * some function allocates 2 buffer of this size in stack.
  * As image are encrypted by 128 bits blocks, this value is 16 bytes aligned.
  */
#define CHUNK_SIZE_SIGN_VERIFICATION (1024UL)  /*!< Signature verification chunk size*/

#define SFU_IMG_CHUNK_SIZE  (512UL)
#define AES_BLOCK_SIZE (16UL)  /*!< Size of an AES block to check padding needs for decrypting */

/* Exported macros -----------------------------------------------------------*/
/**
  * @brief  Status Macro
  * This macros aims at capturing abnormal errors in the FWIMG sub-module (typically FLASH errors).
  * When SFU_FWIMG_BLOCK_ON_ABNORMAL_ERRORS_MODE is activated this macro blocks the execution.
  * When SFU_FWIMG_BLOCK_ON_ABNORMAL_ERRORS_MODE is deactivated, a log is printed in the console (if SFU_DEBUG_MODE is
  * activated) and the execution continues.
  */
#if defined(SFU_FWIMG_BLOCK_ON_ABNORMAL_ERRORS_MODE)
#define STATUS_FWIMG(B,A) if (B) { \
                                   SFU_IMG_Status=A; \
                                   SFU_IMG_Line = __LINE__; \
                                   TRACE("\r\n          Abnormal error %d at line %d in %s - BLOCK", \
                                         SFU_IMG_Status, SFU_IMG_Line, __FILE__); \
                                   while(1==1){;} \
                                 } while(0==1){;}
#else
#define STATUS_FWIMG(B,A) if (B) { \
                                   SFU_IMG_Status=A; \
                                   SFU_IMG_Line = __LINE__; \
                                   TRACE("\r\n          Abnormal error %d at line %d in %s - CONTINUE", \
                                         SFU_IMG_Status, SFU_IMG_Line, __FILE__); \
                                 } while(0==1){;}
#endif /* SFU_FWIMG_BLOCK_ON_ABNORMAL_ERRORS_MODE */

/* External variables --------------------------------------------------------*/
/**
  *  FWIMG status variables used to log errors and display debug messages.
  *  This is related to FLASH operations.
  *  This is handled with STATUS_FWIMG.
  */
extern SFU_IMG_StatusTypeDef SFU_IMG_Status;
extern uint32_t SFU_IMG_Line;

extern SE_FwRawHeaderTypeDef fw_image_header_validated;
#if   (SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER)
extern SE_FwRawHeaderTypeDef fw_image_header_to_test;
#endif /* (SECBOOT_LOADER == SECBOOT_USE_STANDALONE_LOADER) */

/* Exported functions : helper ---------------------------------------------- */
SFU_ErrorStatus MemoryCompare(uint8_t *pAdd1, uint8_t *pAdd2, uint32_t Size);
SFU_ErrorStatus VerifyHeaderSignature(SE_FwRawHeaderTypeDef *pFwImageHeader);
SFU_ErrorStatus VerifyFwSignatureScatter(SE_StatusTypeDef *pSeStatus, uint32_t SlotNumber,
                                         SE_FwRawHeaderTypeDef *pSE_Metadata,
                                         SE_Ex_PayloadDescTypeDef  *pSE_Payload, uint32_t SE_FwType);
SFU_ErrorStatus VerifyTag(SE_StatusTypeDef *pSeStatus, uint32_t SlotNumber, SE_FwRawHeaderTypeDef *pSE_Metadata,
                          uint8_t  *pPayload, uint32_t SE_FwType);
SFU_ErrorStatus ParseFWInfo(SE_FwRawHeaderTypeDef *pFwHeader, uint8_t *pBuffer);
SFU_ErrorStatus CheckAndGetFWHeader(uint32_t SlotNumber, SE_FwRawHeaderTypeDef *pFwImageHeader);
SFU_ErrorStatus VerifyFwSignature(SE_StatusTypeDef  *pSeStatus, uint32_t slot, SE_FwRawHeaderTypeDef *pFwImageHeader,
                                  uint32_t SE_FwType);
SFU_ErrorStatus VerifySlot(uint8_t *pSlotBegin, uint32_t uSlotSize, uint32_t uFwSize);
SFU_ErrorStatus CleanUpSlot(uint8_t *pSlotBegin, uint32_t uSlotSize, uint32_t uOffset);

#ifdef __cplusplus
}
#endif

#endif /* SFU_FWIMG_INTERNAL_H */
