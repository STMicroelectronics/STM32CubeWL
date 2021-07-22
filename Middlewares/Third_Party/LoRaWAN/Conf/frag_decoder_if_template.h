/**
  ******************************************************************************
  * @file    frag_decoder_if_template.h
  * @author  MCD Application Team
  * @brief   Applicative interfaces of LoRa-Alliance fragmentation decoder
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#ifndef __FRAG_DECODER_IF_H__
#define __FRAG_DECODER_IF_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

#define INTEROP_TEST_MODE                           0

#if (INTEROP_TEST_MODE == 1)
/*!
  * Maximum number of fragment that can be handled.
  *
  * \remark This parameter has an impact on the memory footprint.
  */
#define FRAG_MAX_NB                                 20

/*!
  * Maximum fragment size that can be handled.
  *
  * \remark This parameter has an impact on the memory footprint.
  */
#define FRAG_MAX_SIZE                               50

/*!
  * Maximum number of extra frames that can be handled.
  *
  * \remark This parameter has an impact on the memory footprint.
  */
#define FRAG_MAX_REDUNDANCY                         5

#else /* INTEROP_TEST_MODE == 0 */
/*!
  * Maximum number of fragment that can be handled.
  *
  * \remark This parameter has an impact on the memory footprint.
  * \note FRAG_MAX_NB = (SLOT_DWL_1_END - SLOT_DWL_1_START) / FRAG_MAX_SIZE
  */
#define FRAG_MAX_NB                                  716

/*!
  * Maximum fragment size that can be handled.
  *
  * \remark This parameter has an impact on the memory footprint.
  */
#define FRAG_MAX_SIZE                               120

/*!
  * Maximum number of extra frames that can be handled.
  *
  * \remark This parameter has an impact on the memory footprint.
  * \note FRAG_MAX_NB * 0.10 (with redundancy = 10 %)
  */
#define FRAG_MAX_REDUNDANCY                         72

#endif /* INTEROP_TEST_MODE */

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __FRAG_DECODER_IF_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
