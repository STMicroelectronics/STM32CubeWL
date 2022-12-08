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

/******************************************************************************/
/*                               M4 ROM section                               */
/******************************************************************************/

/* M4 SB Code region */
#define M4_SB_REGION_ROM_START          0x08000000
#define M4_SB_REGION_ROM_END            0x080027FF

/* ROM regions definition */
#define M4_SB_ROM_REGION_SIZE          (M4_SB_REGION_ROM_END - M4_SB_REGION_ROM_START + 0x1)

/******************************************************************************/
/*                               M0+ ROM section                              */
/******************************************************************************/
/* EE Data Storage (NVMS) region protected area */
/* EE Data Storage need for 2 images : 2 kbytes * 2 ==> 4 kbytes */
#define EE_DATASTORAGE_START            0x0802A000
#define EE_DATASTORAGE_END              0x0802AFFF

/* KMS Data Storage (NVMS) region protected area */
/* KMS Data Storage need for 2 images : 4 kbytes * 2 ==> 8 kbytes */
#define KMS_DATASTORAGE_START           0x0802B000
#define KMS_DATASTORAGE_END             0x0802CFFF

/* SE IF ROM: used to locate Secure Engine interface code out of MPU isolation     */
#define SE_IF_REGION_ROM_START          (KMS_DATASTORAGE_END + 0x1)
#define SE_IF_REGION_ROM_END            (SE_IF_REGION_ROM_START + 0x15FF)

/* SBSFU Code region */
#define SB_REGION_ROM_START             (SE_IF_REGION_ROM_END + 1)
#define SB_REGION_ROM_END               0x080367FF

/* M0 Vector table with alignment constraint on VECTOR_SIZE */
#define VECTOR_SIZE                     0x200
#define INTVECT_START                   (SB_REGION_ROM_END + 0x1)

/* SE Code region protected by MPU isolation */
#define SE_CODE_REGION_ROM_START        (INTVECT_START + VECTOR_SIZE)
#define SE_CALLGATE_REGION_ROM_START    (SE_CODE_REGION_ROM_START + 4)
#define SE_CALLGATE_REGION_ROM_END      (SE_CODE_REGION_ROM_START + 0xFF)

/* SE Startup                               */
#define SE_STARTUP_REGION_ROM_START     (SE_CALLGATE_REGION_ROM_END + 1)
#define SE_CODE_NOKEY_REGION_ROM_START  (SE_STARTUP_REGION_ROM_START + 0x100)

/* User Embedded Keys */
#define USER_KEY_REGION_ROM_START       0x0803E500
#define USER_KEY_REGION_ROM_END         0x0803E7FF

/* SE Embedded Keys */
#define SE_KEY_REGION_ROM_START         0x0803E800
#define SE_KEY_REGION_ROM_END           0x0803EFFF
#define SE_CODE_REGION_ROM_END          SE_KEY_REGION_ROM_END

/* ROM regions size definition */
#define KMS_DATASTORAGE_SIZE            (KMS_DATASTORAGE_END - KMS_DATASTORAGE_START + 0x1)
#define SE_IF_ROM_REGION_SIZE           (SE_IF_REGION_ROM_END - SE_IF_REGION_ROM_START + 0x1)
#define SB_ROM_REGION_SIZE              (SB_REGION_ROM_END - SB_REGION_ROM_START + 0x1)
#define SE_CALLGATE_REGION_ROM_SIZE     (SE_CALLGATE_REGION_ROM_END - SE_CALLGATE_REGION_ROM_START + 0x1)
#define SE_CODE_NOKEY_REGION_ROM_SIZE   (SE_KEY_REGION_ROM_START - SE_CODE_NOKEY_REGION_ROM_START)
#define USER_KEY_REGION_ROM_SIZE        (USER_KEY_REGION_ROM_START - USER_KEY_REGION_ROM_END + 0x1)
#define SE_KEY_REGION_ROM_SIZE          (SE_KEY_REGION_ROM_END - SE_KEY_REGION_ROM_START + 0x1)
#define SE_ROM_REGION_SIZE              (SE_CODE_REGION_ROM_END - SE_CODE_REGION_ROM_START + 0x1)

/******************************************************************************/
/*                              RAM section                                   */
/*                                                                            */
/******************************************************************************/

/* M4 SB */
#define M4_SB_REGION_RAM_START          0x20000000
#define M4_SB_REGION_RAM_END            (M4_SB_REGION_RAM_START + 0xCDF)

/* M0+/M4 Synchronization flag */
#define M4_M0PLUS_FLAG_RAM_START        (M4_SB_REGION_RAM_END + 0x1)
#define M4_M0PLUS_FLAG_RAM_END          (M4_M0PLUS_FLAG_RAM_START + 0x1F)

/* M4 UserApp : allocation limited to half RAM size */
#define M4_APP_REGION_RAM_START         (M4_M0PLUS_FLAG_RAM_END + 0x1)
#define M4_APP_REGION_RAM_END           0x20007FFF

/* M4 UserApp shared area */
#define M4_SHARED_REGION_RAM_START      (M4_APP_REGION_RAM_END + 0x1)
#define M4_SHARED_REGION_RAM_END        0x200083FF

/* M0PLUS UserApp shared area */
#define M0PLUS_SHARED_REGION_RAM_START  (M4_SHARED_REGION_RAM_END + 0x1)
#define M0PLUS_SHARED_REGION_RAM_END    0x20008FFF

/* SBSFU RAM region */
#define SB_REGION_RAM_START             (M0PLUS_SHARED_REGION_RAM_END + 0x1)
#define SB_REGION_RAM_END               0x2000C3FF

/* KMS Data Storage (VMS) region */
/* WARNING: Symbols not used */
/* The VMS address is defined in vms_low_level.h: VMS_KMS_DATA_STORAGE_START */
#define VM_KMS_DATASTORAGE_RAM_START    0x2000C400
#define VM_KMS_DATASTORAGE_RAM_END      0x2000CBFF

/* SE RAM region protected area with 1 kBytes alignment constraint (TZIC) ==> 0x2000D400 */
#define SE_REGION_RAM_START             (VM_KMS_DATASTORAGE_RAM_END +0x1)
#define SE_REGION_RAM_STACK_TOP         0x2000DB00                /* Secure Engine's private stack */
#define SE_REGION_RAM_END               0x2000FFFF

/* RAM regions definition */
#define M4_SB_RAM_REGION_SIZE           (M4_SB_REGION_RAM_END - M4_SB_REGION_RAM_START + 0x1)
#define M4_APP_RAM_REGION_SIZE          (M4_APP_REGION_RAM_END - M4_APP_REGION_RAM_START + 0x1)
#define M4_SHARED_RAM_REGION_SIZE       (M4_SHARED_REGION_RAM_END - M4_SHARED_REGION_RAM_START + 0x1)
#define M0PLUS_SHARED_RAM_REGION_SIZE   (M0PLUS_SHARED_REGION_RAM_END - M0PLUS_SHARED_REGION_RAM_START + 0x1)
#define SB_RAM_REGION_SIZE              (SB_REGION_RAM_END - SB_REGION_RAM_START + 0x1)
#define VM_KMS_DATASTORAGE_RAM_REGION_SIZE  (VM_KMS_DATASTORAGE_RAM_END - VM_KMS_DATASTORAGE_RAM_START + 0x1)
#define SE_RAM_REGION_SIZE              (SE_REGION_RAM_END - SE_REGION_RAM_STACK_TOP + 0x1)

#endif

