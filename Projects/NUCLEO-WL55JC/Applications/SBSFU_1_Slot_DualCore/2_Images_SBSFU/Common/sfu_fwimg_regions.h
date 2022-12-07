/**
  ******************************************************************************
  * @file    sfu_fwimg_regions.h
  * @author  MCD Application Team
  * @brief   This file contains FLASH regions definitions for SFU_FWIMG functionalities
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
#ifndef SFU_FWIMG_REGIONS_H
#define SFU_FWIMG_REGIONS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_sfu.h"
#if defined(__ARMCC_VERSION)
#include "mapping_fwimg.h"
#elif defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#endif /* __ARMCC_VERSION */
#include "se_crypto_config.h"


/* Exported constants --------------------------------------------------------*/
/**
  *  Slot list : 2 slots per image configuration + swap
  */
#define NB_SLOTS      6U
#define SLOT_INACTIVE 0U     /* this index should not be used ==> no tag found in the header */
#define SLOT_ACTIVE_1 1U
#define SLOT_ACTIVE_2 2U
#define SLOT_DWL_1    3U
#define SLOT_DWL_2    4U
#define SLOT_SWAP     5U

/* Calculation of the size of a slot */
#define SLOT_SIZE(a) (SlotEndAdd[a] - SlotStartAdd[a] + 1U)

/*
 * Design constraint: the image slot size must be a multiple of the swap area size.
 * And of course both image slots must have the same size.
 */
#define SFU_IMG_REGION_IS_MULTIPLE(a,b) ((a / b * b) == a)

/*
 * Checking that the slot sizes are consistent with the .icf file
 * Sizes expressed in bytes (+1 because the end address belongs to the slot)
 * The checks are executed at runtime in the SFU_Img_Init() function.
 */
#define SFU_IMG_REGION_IS_SAME_SIZE(a,b) ((a) == (b))

/**
  * Image starting offset to add to the  address of 1st block
  */
#define SFU_IMG_IMAGE_OFFSET ((uint32_t)512U)


/* External variables --------------------------------------------------------*/
extern const uint32_t  SlotHeaderAdd[NB_SLOTS];
extern const uint32_t  SlotStartAdd[NB_SLOTS];
extern const uint32_t  SlotEndAdd[NB_SLOTS];

#if  (defined(SFU_FWIMG_COMMON_C) || defined(SE_LOW_LEVEL_C) || defined(TEST_PROTECTIONS_C))

const uint32_t  SlotHeaderAdd[NB_SLOTS] = { 0U,
                                            SLOT_ACTIVE_1_HEADER,
                                            SLOT_ACTIVE_2_HEADER,
                                            SLOT_BLOB_DWL_1_START,
                                            0U,
                                            0U,
                                          };
/* List of slot start address */
const uint32_t  SlotStartAdd[NB_SLOTS]  = { 0U,
                                            SLOT_ACTIVE_1_START,
                                            SLOT_ACTIVE_2_START,
                                            SLOT_BLOB_DWL_1_START,
                                            0U,
                                            0U,
                                          };
/* List of slot end address */
const uint32_t  SlotEndAdd[NB_SLOTS]    = { 0U,
                                            SLOT_ACTIVE_1_END,
                                            SLOT_ACTIVE_2_END,
                                            SLOT_BLOB_DWL_1_END,
                                            0U,
                                            0U,
                                          };
#endif /* SFU_FWIMG_COMMON_C  || SE_LOW_LEVEL_C || TEST_PROTECTIONS_C || SFU_LOADER_C */

#ifdef __cplusplus
}
#endif

#endif /* SFU_FWIMG_REGIONS_H */
