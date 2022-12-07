/**
  ******************************************************************************
  * @file    mapping_fwimg.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for firmware images mapping
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
#ifndef MAPPING_FWIMG_H
#define MAPPING_FWIMG_H

/* Slots  must be aligned on 2048 bytes (0x800) */

/* Active slot #1  (56 kbytes) */
#define SLOT_ACTIVE_1_HEADER         0x0803F800
#define SLOT_ACTIVE_1_START          0x0801C000
#define SLOT_ACTIVE_1_END            0x08029FFF

/* Active slot #2  (100 kbytes) */
#define SLOT_ACTIVE_2_HEADER         0x0803F000
#define SLOT_ACTIVE_2_START          0x08003000
#define SLOT_ACTIVE_2_END            0x0801BFFF

/* Dwl slot #1  (2 kbytes) */
#define SLOT_BLOB_DWL_1_START        0x08002800
#define SLOT_BLOB_DWL_1_END          0x08002FFF

/* Slots not configured */
#define SLOT_ACTIVE_3_HEADER         0x00000000
#define SLOT_ACTIVE_3_START          0x00000000
#define SLOT_ACTIVE_3_END            0x00000000

#endif
