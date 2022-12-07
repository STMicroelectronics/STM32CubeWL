/**
  ******************************************************************************
  * @file    sfu_com_trace.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for Secure Firmware Update COM module
  *          functionalities  for the trace.
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
#ifndef SFU_COM_TRACE_H
#define SFU_COM_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sfu_def.h"
#include <stdio.h>

/* Exported functions ------------------------------------------------------- */
SFU_ErrorStatus SFU_COM_Init(void);
SFU_ErrorStatus SFU_COM_DeInit(void);
SFU_ErrorStatus SFU_COM_Serial_PutString(uint8_t *pString);


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

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* SFU_COM_TRACE_H */
