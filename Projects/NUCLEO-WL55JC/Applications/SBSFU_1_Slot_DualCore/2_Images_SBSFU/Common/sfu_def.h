/**
  ******************************************************************************
  * @file    sfu_def.h
  * @author  MCD Application Team
  * @brief   This file contains the general definitions for SBSFU application.
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
#ifndef SFU_DEF_H
#define SFU_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if defined(__ARMCC_VERSION)
#include "mapping_sbsfu.h"
#elif defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#endif /* __ARMCC_VERSION */
#include "app_sfu.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  SFU Error Typedef
  */
typedef enum
{
  SFU_ERROR = 0x00001FE1U,
  SFU_SUCCESS = 0x00122F11U
} SFU_ErrorStatus;

/* Exported constants --------------------------------------------------------*/

#define SFU_RAM_BASE             ((uint32_t) SE_REGION_RAM_START)
#define SFU_RAM_END              ((uint32_t) SB_REGION_RAM_END)
#define SFU_SB_RAM_BASE          ((uint32_t) SB_REGION_RAM_START)
#define SFU_SB_RAM_END           ((uint32_t) SB_REGION_RAM_END)


#ifdef CORE_CM0PLUS
#define SFU_BOOT_BASE_ADDR       ((uint32_t) INTVECT_START)           /* SFU Boot Address */
#else /* CORE_CM0PLUS */
#define SFU_C2_BOOT_BASE_ADDR    ((uint32_t) INTVECT_START)           /* SFU Boot Address */
#define SFU_C2_AREA_ADDR_END     ((uint32_t) SE_KEY_REGION_ROM_END)   /* SBSFU end Address (covering all the SBSFU
                                                                         executable code and the related keys) */
#define SFU_C1_BOOT_BASE_ADDR    ((uint32_t) M4_SB_REGION_ROM_START)  /* SB Boot Address */
#define SFU_C1_AREA_ADDR_END     ((uint32_t) M4_SB_REGION_ROM_END)    /* SB end Address (covering all the SB
                                                                         executable code */
#endif /* CORE_CM0PLUS */
#define SFU_ROM_ADDR_START       ((uint32_t) SE_IF_REGION_ROM_START)  /* SBSFU base Address */

#define SFU_ROM_ADDR_END         ((uint32_t) SE_KEY_REGION_ROM_END)   /* SBSFU end Address (covering all the SBSFU
                                                                         executable code and the related keys) */

#define SFU_SENG_ROM_ADDR_START  ((uint32_t) SE_CODE_REGION_ROM_START)/* Secure Engine area address START */
#define SFU_SENG_ROM_ADDR_END    ((uint32_t) SE_CODE_REGION_ROM_END)  /* Secure Engine area address END - SE includes
                                                                         everything up to the License */
#define SFU_SENG_ROM_SIZE        ((uint32_t) (SFU_SENG_ROM_ADDR_END - \
                                              SFU_SENG_ROM_ADDR_START + 1U)) /* Secure Engine area size */

#define SFU_KEYS_ROM_ADDR_START  ((uint32_t) SE_KEY_REGION_ROM_START) /* Keys Area (Keys + Keys Retrieve function)
                                                                         START. This is the PCRoP Area */
#define SFU_KEYS_ROM_ADDR_END    ((uint32_t) SE_KEY_REGION_ROM_END)   /* Keys Area (Keys + Keys Retrieve function) END.
                                                                         This is the PCRoP Area */

#define SFU_SENG_RAM_ADDR_START  ((uint32_t) SE_REGION_RAM_START)   /* Secure Engine reserved RAM2 area START
                                                                       address */
#define SFU_SENG_RAM_ADDR_END    ((uint32_t) SE_REGION_RAM_END)     /* Secure Engine reserved RAM2 area END address */
#define SFU_SENG_RAM_SIZE        ((uint32_t) (SFU_SENG_RAM_ADDR_END - \
                                              SFU_SENG_RAM_ADDR_START + 1U)) /* Secure Engine reserved RAM area SIZE */


#ifdef __cplusplus
}
#endif

#endif /* SFU_DEF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

