/**
  ******************************************************************************
  * @file    stm32_tiny_sscanf.h
  * @author  MCD Application Team
  * @brief   Header for driver tiny_sscanf.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_TINY_SSCANF_H__
#define __STM32_TINY_SSCANF_H__

#ifdef __cplusplus
extern "C"
{
#endif
/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/**
  * @brief  Read formatted data from string
  *
  * Reads data from s and stores them according to parameter format into the
  * locations given by the additional arguments, as if scanf was used, but
  * reading from s instead of the standard input (stdin).
  *
  * The additional arguments should point to already allocated objects of the
  * type specified by their corresponding format specifier within the format string.
  *
  * @param  C string that the function processes as its source to retrieve the data.
  * @param  C string that contains a format string that follows the same specifications
  *         as format in scanf (see scanf for details).
  * @param  Depending on the format string, the function may expect a sequence of
  *         additional arguments, each containing a pointer to allocated storage
  *         where the interpretation of the extracted characters is stored with
  *         the appropriate type.
  *         There should be at least as many of these arguments as the number of
  *         values stored by the format specifiers. Additional arguments are
  *         ignored by the function.
  * @retval The number of items in the argument list successfully filled. This
  *         count can match the expected number of items or be less (even zero)
  *         in the case of a matching failure
  * @note   Current supported formats are %hx, %hhx, %ul, %d,...
  */
int tiny_sscanf(const char *str, const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* __STM32_TINY_SSCANF_H__ */
