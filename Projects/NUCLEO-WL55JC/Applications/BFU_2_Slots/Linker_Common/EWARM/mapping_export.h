/**
  ******************************************************************************
  * @file    mapping_export.h
  * @author  MCD Application Team
  * @brief   This file contains the definitions exported from mapping linker files.
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
#ifndef MAPPING_EXPORT_H
#define MAPPING_EXPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/
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
extern uint32_t __ICFEDIT_SLOT_Dwl_1_start__;
#define SLOT_DWL_1_START ((uint32_t)& __ICFEDIT_SLOT_Dwl_1_start__)
extern uint32_t __ICFEDIT_SLOT_Dwl_1_end__;
#define SLOT_DWL_1_END ((uint32_t)& __ICFEDIT_SLOT_Dwl_1_end__)
extern uint32_t __ICFEDIT_SLOT_Dwl_2_start__;
#define SLOT_DWL_2_START ((uint32_t)& __ICFEDIT_SLOT_Dwl_2_start__)
extern uint32_t __ICFEDIT_SLOT_Dwl_2_end__;
#define SLOT_DWL_2_END ((uint32_t)& __ICFEDIT_SLOT_Dwl_2_end__)
extern uint32_t __ICFEDIT_SLOT_Dwl_3_start__;
#define SLOT_DWL_3_START ((uint32_t)& __ICFEDIT_SLOT_Dwl_3_start__)
extern uint32_t __ICFEDIT_SLOT_Dwl_3_end__;
#define SLOT_DWL_3_END ((uint32_t)& __ICFEDIT_SLOT_Dwl_3_end__)
extern uint32_t __ICFEDIT_SWAP_start__;
#define SWAP_START ((uint32_t)& __ICFEDIT_SWAP_start__)
extern uint32_t __ICFEDIT_SWAP_end__;
#define SWAP_END ((uint32_t)& __ICFEDIT_SWAP_end__)
#endif /* __ICCARM__ || __GNUC__ */

#ifdef __cplusplus
}
#endif

#endif /* MAPPING_EXPORT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

