/**
  ******************************************************************************
  * @file    fw_update_app.c
  * @author  MCD Application Team
  * @brief   Firmware Update module.
  *          This file provides set of firmware functions to manage Firmware
  *          Update functionalities.
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

/* Includes ------------------------------------------------------------------*/

#include "se_def.h"
#include "com.h"
#include "common.h"
#include "flash_if.h"
#include "stm32wlxx_hal.h"
#include "stm32wlxx_nucleo.h"
#include "fw_update_app.h"
#include "se_interface_application.h"
#include "sfu_fwimg_regions.h"
#include "string.h"
#include "mpu.h"
#if defined(__ARMCC_VERSION)
#include "mapping_sbsfu.h"
#elif defined (__ICCARM__) || defined(__GNUC__)
#include "mapping_export.h"
#elif defined(__CC_ARM)
#include "mapping_fwimg.h"
#include "mapping_sbsfu.h"
#endif /* __ARMCC_VERSION */
#include "ymodem.h"

/* Private defines -----------------------------------------------------------*/

#define ALL_SLOTS 255U                   /*!< Validate all new firmware installed in a single request */
/* Global variables ----------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/


/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Run FW Update process.
  * @param  None
  * @retval HAL Status.
  */
#if defined(EXTERNAL_LOADER)
void FW_UPDATE_Run(void)
{
  /* Print Firmware Update welcome message */
  printf("\r\n================ New Fw Download =========================\r\n\n");

  /* Standalone loader communication : execution requested */
  STANDALONE_LOADER_STATE = STANDALONE_LOADER_DWL_REQ;

  /*  use virtual HAL API allowing the access to Privileged register */
  SVC_NVIC_SystemReset();
}

#else
void FW_UPDATE_Run(void)
{
  printf("  Feature not supported ! \r\n");
}

#endif


#if defined(EXTERNAL_LOADER)
/**
  * @brief  Run Multiple FW Update process.
  * @param  None
  * @retval None.
  */
void FW_UPDATE_MULTIPLE_RunMenu(void)
{
  printf("  Feature not supported ! \r\n");
}

#else
/**
  * @brief  Run Multiple FW Update process.
  * @param  None
  * @retval None.
  */
void FW_UPDATE_MULTIPLE_RunMenu(void)
{
  printf("  Feature not supported ! \r\n");
}

#endif


#if defined(EXTERNAL_LOADER)
/**
  * @brief  Run validation of a FW image menu.
  * @param  None
  * @retval None.
  */
void FW_VALIDATE_RunMenu(void)
{
  printf("  Feature not supported ! \r\n");
}

#else
/**
  * @brief  Run validation of a FW image menu.
  * @param  None
  * @retval None.
  */
void FW_VALIDATE_RunMenu(void)
{
  printf("  Feature not supported ! \r\n");
}

#endif



/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
