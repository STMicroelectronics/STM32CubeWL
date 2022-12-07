/**
  ******************************************************************************
  * @file    mapping_sbsfu.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for SBSFU mapping
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
#ifndef MAPPING_SBSFU_H
#define MAPPING_SBSFU_H

/*****************************************************************************/
/*                                ROM section                                */
/*****************************************************************************/

/* Vector table  */
#define VECTOR_SIZE                     0x200
#define INTVECT_START                   0x08000000

/* SE Code region protected by MPU isolation */
#define SE_CODE_REGION_ROM_START        (INTVECT_START + VECTOR_SIZE)
#define SE_CALLGATE_REGION_ROM_START    (SE_CODE_REGION_ROM_START + 4)
#define SE_CALLGATE_REGION_ROM_END      (SE_CODE_REGION_ROM_START + 0x1FF)

/* SE Embedded Keys */
#define SE_KEY_REGION_ROM_START         (SE_CALLGATE_REGION_ROM_END + 0x1)
#define SE_KEY_REGION_ROM_END           (SE_KEY_REGION_ROM_START + 0x2FF)

/* SE Startup                               */
#define SE_STARTUP_REGION_ROM_START     (SE_KEY_REGION_ROM_END + 1)
#define SE_CODE_NOKEY_REGION_ROM_START  (SE_STARTUP_REGION_ROM_START + 0x100)
#define SE_CODE_REGION_ROM_END          (SE_STARTUP_REGION_ROM_START + 0x75FF)

/* SE IF ROM: used to locate Secure Engine interface code */
#define SE_IF_REGION_ROM_START          (SE_CODE_REGION_ROM_END + 0x1)
#define SE_IF_REGION_ROM_END            (SE_IF_REGION_ROM_START + 0xAFF)

/* SBSFU Code region */
#define SB_REGION_ROM_START             (SE_IF_REGION_ROM_END + 1)
#define SB_REGION_ROM_END               0x0800C7FF

/* KMS Data Storage (NVMS) region protected area */
/* KMS Data Storage need for 2 images : 4 kbytes * 2 ==> 8 kbytes */
#define KMS_DATASTORAGE_START           0x0800C800
#define KMS_DATASTORAGE_END             0x0800E7FF

/* ROM regions size definition */
#define SE_CALLGATE_REGION_ROM_SIZE     (SE_CALLGATE_REGION_ROM_END - SE_CALLGATE_REGION_ROM_START + 0x1)
#define SE_KEY_REGION_ROM_SIZE          (SE_KEY_REGION_ROM_END - SE_KEY_REGION_ROM_START + 0x1)
#define SE_ROM_REGION_SIZE              (SE_CODE_REGION_ROM_END - SE_CODE_NOKEY_REGION_ROM_START + 0x1)
#define SE_IF_ROM_REGION_SIZE           (SE_IF_REGION_ROM_END - SE_IF_REGION_ROM_START + 0x1)
#define SB_ROM_REGION_SIZE              (SB_REGION_ROM_END - SB_REGION_ROM_START + 0x1)
#define KMS_DATASTORAGE_SIZE            (KMS_DATASTORAGE_END - KMS_DATASTORAGE_START + 0x1)

/******************************************************************************/
/*                              RAM section                                   */
/*                                                                            */
/******************************************************************************/

/* SE RAM region */
#define SE_REGION_RAM_START             0x20000000
#define SE_REGION_RAM_STACK_TOP         0x20000C00                /* Secure Engine's private stack */
#define SE_REGION_RAM_END               0x200033FF

/* SBSFU RAM region */
#define SB_REGION_RAM_START             (SE_REGION_RAM_END + 0x1)
#define SB_REGION_RAM_END               0x20007FFF

/* RAM regions definition */
#define SE_RAM_REGION_SIZE              (SE_REGION_RAM_END - SE_REGION_RAM_STACK_TOP + 0x1)
#define SB_RAM_REGION_SIZE              (SB_REGION_RAM_END - SB_REGION_RAM_START + 0x1)

#endif

