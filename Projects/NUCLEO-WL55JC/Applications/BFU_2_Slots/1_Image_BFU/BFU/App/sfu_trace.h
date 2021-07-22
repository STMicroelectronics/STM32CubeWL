/**
  ******************************************************************************
  * @file    sfu_trace.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for SFU_TRACE functionalities.
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
#ifndef SFU_TRACE_H
#define SFU_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_sfu.h"
#if defined(SFU_DEBUG_MODE) || defined(SFU_TEST_PROTECTION)
#include "sfu_com_trace.h"
#endif /* defined(SFU_DEBUG_MODE) || defined(SFU_TEST_PROTECTION) */

/* External variables --------------------------------------------------------*/
/**
  * This variable indicates if at boot-up the FW presence check has already been performed or not:
  * \li It is reset when the FW status has already been checked once and no FW is present
  * \li (see @ref SFU_BOOT_SM_CheckUserFwStatus)
  * \li It is used to display some messages only once in the teraterm console
  * \li It is used to determine if the user button must be pressed before waiting for a local download to start
  * \li This is used at LocalDownload and CheckUserFwStatus stages.
  */
extern uint8_t initialDeviceStatusCheck; /* used also in sfu_fwimg_swap.c */

/* Exported macros -----------------------------------------------------------*/
#ifdef SFU_DEBUG_MODE
#define TRACE_IRQ(pbuffer) (void) SFU_COM_Serial_PutString(pbuffer); /* Printf is not used inside interrupt to avoid
                                                                        faults. printf is not rieentrant */
#define TRACE   (void) printf
#else
#define TRACE(...)
#define TRACE_IRQ(pbuffer)
#endif /* SFU_DEBUG_MODE */


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


#ifdef __cplusplus
}
#endif

#endif /* SFU_TRACE_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

