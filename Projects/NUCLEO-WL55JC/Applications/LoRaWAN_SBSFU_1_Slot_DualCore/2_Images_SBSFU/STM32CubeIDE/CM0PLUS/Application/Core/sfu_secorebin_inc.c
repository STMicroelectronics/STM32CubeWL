/**
  ******************************************************************************
  * @file    sfu_secorebin_inc.c
  * @author  MCD Application Team
  * @brief   Include SECoreBin binary.
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

/* Includes ------------------------------------------------------------------*/
#include "string.h"

#if defined (__GNUC__)
asm(".section SE_CORE_Bin,\"a\";"
                             ".incbin \"../../2_Images_SECoreBin/STM32CubeIDE/Debug/SECoreBin.bin\";"
);
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

/**
  * @}
  */

