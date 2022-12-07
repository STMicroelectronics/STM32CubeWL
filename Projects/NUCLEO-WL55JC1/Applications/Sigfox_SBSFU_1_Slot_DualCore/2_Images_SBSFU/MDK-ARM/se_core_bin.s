;/******************************************************************************
;* File Name          : se_core_bin.s
;* Author             : MCD Application Team
;* Description        : Include SECoreBin binary.
;********************************************************************************
;* @attention
;*
;* Copyright (c) 2017 STMicroelectronics.
;* All rights reserved.
;*
;* This software is licensed under terms that can be found in the LICENSE file in
;* the root directory of this software component.
;* If no LICENSE file comes with this software, it is provided AS-IS.
;*
;******************************************************************************
;
;
 AREA SE_CORE_Bin, CODE, READONLY
 INCBIN .\..\..\2_Images_SECoreBin\MDK-ARM\STM32WL55JC_Nucleo\Exe\SECoreBin.bin
 END
