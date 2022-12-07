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
extern uint32_t INTVECT_start;
#define INTVECT_START ((uint32_t)& INTVECT_start)
extern uint32_t SE_Startup_region_ROM_start;
#define SE_STARTUP_REGION_ROM_START ((uint32_t)& SE_Startup_region_ROM_start)
extern uint32_t SE_Code_region_ROM_start;
#define SE_CODE_REGION_ROM_START ((uint32_t)& SE_Code_region_ROM_start)
extern uint32_t SE_Code_region_ROM_end;
#define SE_CODE_REGION_ROM_END ((uint32_t)& SE_Code_region_ROM_end)
extern uint32_t SE_IF_region_ROM_start;
#define SE_IF_REGION_ROM_START ((uint32_t)& SE_IF_region_ROM_start)
extern uint32_t SE_IF_region_ROM_end;
#define SE_IF_REGION_ROM_END ((uint32_t)& SE_IF_region_ROM_end)
extern uint32_t SE_Key_region_ROM_start;
#define SE_KEY_REGION_ROM_START ((uint32_t)& SE_Key_region_ROM_start)
extern uint32_t SE_Key_region_ROM_end;
#define SE_KEY_REGION_ROM_END ((uint32_t)& SE_Key_region_ROM_end)
extern uint32_t SE_CallGate_region_ROM_start;
#define SE_CALLGATE_REGION_ROM_START ((uint32_t)& SE_CallGate_region_ROM_start)
extern uint32_t SB_region_ROM_start;
#define SB_REGION_ROM_START ((uint32_t)& SB_region_ROM_start)
extern uint32_t SB_region_ROM_end;
#define SB_REGION_ROM_END ((uint32_t)& SB_region_ROM_end)
extern uint32_t SE_region_RAM_start;
#define SE_REGION_RAM_START ((uint32_t)& SE_region_RAM_start)
extern uint32_t SE_region_RAM_end;
#define SE_REGION_RAM_END ((uint32_t)& SE_region_RAM_end)
extern uint32_t SB_region_RAM_start;
#define SB_REGION_RAM_START ((uint32_t)& SB_region_RAM_start)
extern uint32_t SB_region_RAM_end;
#define SB_REGION_RAM_END ((uint32_t)& SB_region_RAM_end)
extern uint32_t SE_region_RAM_stack_top;
#define SE_REGION_RAM_STACK_TOP ((uint32_t)& SE_region_RAM_stack_top)

extern uint32_t KMS_DataStorage_start;
#define KMS_DATASTORAGE_START ((uint32_t)& KMS_DataStorage_start)
extern uint32_t KMS_DataStorage_end;
#define KMS_DATASTORAGE_END ((uint32_t)& KMS_DataStorage_end)

extern uint32_t SLOT_Active_1_header;
#define SLOT_ACTIVE_1_HEADER ((uint32_t)& SLOT_Active_1_header)
extern uint32_t SLOT_Active_1_start;
#define SLOT_ACTIVE_1_START ((uint32_t)& SLOT_Active_1_start)
extern uint32_t SLOT_Active_1_end;
#define SLOT_ACTIVE_1_END ((uint32_t)& SLOT_Active_1_end)
extern uint32_t SLOT_Active_2_header;
#define SLOT_ACTIVE_2_HEADER ((uint32_t)& SLOT_Active_2_header)
extern uint32_t SLOT_Active_2_start;
#define SLOT_ACTIVE_2_START ((uint32_t)& SLOT_Active_2_start)
extern uint32_t SLOT_Active_2_end;
#define SLOT_ACTIVE_2_END ((uint32_t)& SLOT_Active_2_end)
extern uint32_t SLOT_Active_3_header;
#define SLOT_ACTIVE_3_HEADER ((uint32_t)& SLOT_Active_3_header)
extern uint32_t SLOT_Active_3_start;
#define SLOT_ACTIVE_3_START ((uint32_t)& SLOT_Active_3_start)
extern uint32_t SLOT_Active_3_end;
#define SLOT_ACTIVE_3_END ((uint32_t)& SLOT_Active_3_end)
extern uint32_t SLOT_Dwl_1_start;
#define SLOT_DWL_1_START ((uint32_t)& SLOT_Dwl_1_start)
extern uint32_t SLOT_Dwl_1_end;
#define SLOT_DWL_1_END ((uint32_t)& SLOT_Dwl_1_end)
extern uint32_t SLOT_Dwl_2_start;
#define SLOT_DWL_2_START ((uint32_t)& SLOT_Dwl_2_start)
extern uint32_t SLOT_Dwl_2_end;
#define SLOT_DWL_2_END ((uint32_t)& SLOT_Dwl_2_end)
extern uint32_t SLOT_Dwl_3_start;
#define SLOT_DWL_3_START ((uint32_t)& SLOT_Dwl_3_start)
extern uint32_t SLOT_Dwl_3_end;
#define SLOT_DWL_3_END ((uint32_t)& SLOT_Dwl_3_end)
extern uint32_t SWAP_start;
#define SWAP_START ((uint32_t)& SWAP_start)
extern uint32_t SWAP_end;
#define SWAP_END ((uint32_t)& SWAP_end)


#ifdef __cplusplus
}
#endif

#endif /* MAPPING_EXPORT_H */
