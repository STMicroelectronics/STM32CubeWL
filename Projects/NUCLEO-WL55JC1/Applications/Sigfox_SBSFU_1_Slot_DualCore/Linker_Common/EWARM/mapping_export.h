/**
  ******************************************************************************
  * @file    mapping_export.h
  * @author  MCD Application Team
  * @brief   This file contains the definitions exported from mapping linker files.
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
#ifndef MAPPING_EXPORT_H
#define MAPPING_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
extern uint32_t __ICFEDIT_M4_SB_region_ROM_start__;
#define M4_SB_REGION_ROM_START ((uint32_t)& __ICFEDIT_M4_SB_region_ROM_start__)
extern uint32_t __ICFEDIT_M4_SB_region_ROM_end__;
#define M4_SB_REGION_ROM_END ((uint32_t)& __ICFEDIT_M4_SB_region_ROM_end__)
extern uint32_t __ICFEDIT_intvec_start__;
#define INTVECT_START ((uint32_t)& __ICFEDIT_intvec_start__)
extern uint32_t __ICFEDIT_SE_Startup_region_ROM_start__;
#define SE_STARTUP_REGION_ROM_START ((uint32_t)& __ICFEDIT_SE_Startup_region_ROM_start__)
extern uint32_t __ICFEDIT_SE_Code_region_ROM_start__;
#define SE_CODE_REGION_ROM_START ((uint32_t)& __ICFEDIT_SE_Code_region_ROM_start__)
extern uint32_t __ICFEDIT_SE_Code_region_ROM_end__;
#define SE_CODE_REGION_ROM_END ((uint32_t)& __ICFEDIT_SE_Code_region_ROM_end__)
extern uint32_t __ICFEDIT_SE_IF_region_ROM_start__;
#define SE_IF_REGION_ROM_START ((uint32_t)& __ICFEDIT_SE_IF_region_ROM_start__)
extern uint32_t __ICFEDIT_SE_IF_region_ROM_end__;
#define SE_IF_REGION_ROM_END ((uint32_t)& __ICFEDIT_SE_IF_region_ROM_end__)
extern uint32_t __ICFEDIT_SE_Key_region_ROM_start__;
#define SE_KEY_REGION_ROM_START ((uint32_t)& __ICFEDIT_SE_Key_region_ROM_start__)
extern uint32_t __ICFEDIT_SE_Key_region_ROM_end__;
#define SE_KEY_REGION_ROM_END ((uint32_t)& __ICFEDIT_SE_Key_region_ROM_end__)
extern uint32_t __ICFEDIT_SE_CallGate_region_ROM_start__;
#define SE_CALLGATE_REGION_ROM_START ((uint32_t)& __ICFEDIT_SE_CallGate_region_ROM_start__)
extern uint32_t __ICFEDIT_SB_region_ROM_start__;
#define SB_REGION_ROM_START ((uint32_t)& __ICFEDIT_SB_region_ROM_start__)
extern uint32_t __ICFEDIT_SB_region_ROM_end__;
#define SB_REGION_ROM_END ((uint32_t)& __ICFEDIT_SB_region_ROM_end__)
extern uint32_t __ICFEDIT_SE_region_RAM_start__;
#define SE_REGION_RAM_START ((uint32_t)& __ICFEDIT_SE_region_RAM_start__)
extern uint32_t __ICFEDIT_SE_region_RAM_end__ ;
#define SE_REGION_RAM_END ((uint32_t)& __ICFEDIT_SE_region_RAM_end__)
extern uint32_t __ICFEDIT_SB_region_RAM_start__ ;
#define SB_REGION_RAM_START ((uint32_t)& __ICFEDIT_SB_region_RAM_start__)
extern uint32_t __ICFEDIT_SB_region_RAM_end__ ;
#define SB_REGION_RAM_END ((uint32_t)& __ICFEDIT_SB_region_RAM_end__)
extern uint32_t __ICFEDIT_SE_region_RAM_stack_top__;
#define SE_REGION_RAM_STACK_TOP ((uint32_t)& __ICFEDIT_SE_region_RAM_stack_top__)

extern uint32_t __ICFEDIT_EE_DataStorage_start__ ;
#define EE_DATASTORAGE_START ((uint32_t)& __ICFEDIT_EE_DataStorage_start__)
extern uint32_t __ICFEDIT_EE_DataStorage_end__ ;
#define EE_DATASTORAGE_END ((uint32_t)& __ICFEDIT_EE_DataStorage_end__)
extern uint32_t __ICFEDIT_KMS_DataStorage_start__ ;
#define KMS_DATASTORAGE_START ((uint32_t)& __ICFEDIT_KMS_DataStorage_start__)
extern uint32_t __ICFEDIT_KMS_DataStorage_end__ ;
#define KMS_DATASTORAGE_END ((uint32_t)& __ICFEDIT_KMS_DataStorage_end__)


#if defined (__ICCARM__) || defined(__GNUC__)
extern uint32_t __ICFEDIT_SLOT_Active_1_header__;
#define SLOT_ACTIVE_1_HEADER ((uint32_t)& __ICFEDIT_SLOT_Active_1_header__)
extern uint32_t __ICFEDIT_SLOT_Active_1_start__;
#define SLOT_ACTIVE_1_START ((uint32_t)& __ICFEDIT_SLOT_Active_1_start__)
extern uint32_t __ICFEDIT_SLOT_Active_1_end__;
#define SLOT_ACTIVE_1_END ((uint32_t)& __ICFEDIT_SLOT_Active_1_end__)
extern uint32_t __ICFEDIT_SLOT_Active_2_header__;
#define SLOT_ACTIVE_2_HEADER ((uint32_t)& __ICFEDIT_SLOT_Active_2_header__)
extern uint32_t __ICFEDIT_SLOT_Active_2_start__;
#define SLOT_ACTIVE_2_START ((uint32_t)& __ICFEDIT_SLOT_Active_2_start__)
extern uint32_t __ICFEDIT_SLOT_Active_2_end__;
#define SLOT_ACTIVE_2_END ((uint32_t)& __ICFEDIT_SLOT_Active_2_end__)
extern uint32_t __ICFEDIT_SLOT_Active_3_header__;
#define SLOT_ACTIVE_3_HEADER ((uint32_t)& __ICFEDIT_SLOT_Active_3_header__)
extern uint32_t __ICFEDIT_SLOT_Active_3_start__;
#define SLOT_ACTIVE_3_START ((uint32_t)& __ICFEDIT_SLOT_Active_3_start__)
extern uint32_t __ICFEDIT_SLOT_Active_3_end__;
#define SLOT_ACTIVE_3_END ((uint32_t)& __ICFEDIT_SLOT_Active_3_end__)
extern uint32_t __ICFEDIT_SLOT_Blob_Dwl_1_start__;
#define SLOT_BLOB_DWL_1_START ((uint32_t)& __ICFEDIT_SLOT_Blob_Dwl_1_start__)
extern uint32_t __ICFEDIT_SLOT_Blob_Dwl_1_end__;
#define SLOT_BLOB_DWL_1_END ((uint32_t)& __ICFEDIT_SLOT_Blob_Dwl_1_end__)

/* DualCore specificities */
#if !defined(CORE_CM0PLUS)
extern uint32_t __ICFEDIT_region_ROM_end__ ;
#define C1_AREA_ADDR_END  ((uint32_t)& __ICFEDIT_region_ROM_end__)
#define C2_BOOT_BASE_ADDR ((uint32_t)& __ICFEDIT_intvec_start__)
#endif /* CORE_CM0PLUS */
extern volatile uint32_t __ICFEDIT_M4_M0PLUS_FLAG_region_RAM_start__ ;
#define M4_M0PLUS_FLAG_RAM_START ((uint32_t)& __ICFEDIT_M4_M0PLUS_FLAG_region_RAM_start__)

#endif /* __ICCARM__ || __GNUC__ */

#ifdef __cplusplus
}
#endif

#endif /* MAPPING_EXPORT_H */
