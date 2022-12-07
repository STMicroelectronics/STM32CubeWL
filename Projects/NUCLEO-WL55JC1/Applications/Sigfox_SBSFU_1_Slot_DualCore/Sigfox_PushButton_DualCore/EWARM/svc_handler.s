;*******************************************************************************
;* File Name          : svc_handler.s
;* Author             : MCD Application Team
;* Description        : Wrapper for SE isolation with MPU.
;*******************************************************************************
;* @attention
;*
;* Copyright (c) 2017 STMicroelectronics.
;* All rights reserved.
;*
;* This software is licensed under terms that can be found in the LICENSE file in
;* the root directory of this software component.
;* If no LICENSE file comes with this software, it is provided AS-IS.
;*******************************************************************************
;
;
;
; Cortex-M version
;
      SECTION .text:CODE:NOROOT:REORDER(2)
      EXPORT SVC_Handler
SVC_Handler
        IMPORT SVC_APP_Handler
        MRS r0, PSP
        PUSH {LR}
        BL SVC_APP_Handler
        POP {PC}
        END
