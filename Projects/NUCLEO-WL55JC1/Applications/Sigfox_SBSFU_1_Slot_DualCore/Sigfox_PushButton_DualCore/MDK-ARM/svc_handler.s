;*******************************************************************************
;* File Name          : svc_handler.s
;* Author             : MCD Application Team
;* Description        : Wrapper for SE isolation with MPU.
;*******************************************************************************
;*  @attention
;*
;* Copyright (c) 2020 STMicroelectronics. All rights reserved.
;*
;*  This software component is licensed by ST under Ultimate Liberty license
;*  SLA0044, the "License"; You may not use this file except in compliance with
;*  the License. You may obtain a copy of the License at:
;*                              www.st.com/SLA0044
;*
;*******************************************************************************
;
;
;
; Cortex-M version
;
      PRESERVE8
      THUMB
      AREA    |.text|, CODE, READONLY
 
      EXPORT SVC_Handler
SVC_Handler
        IMPORT SVC_APP_Handler
        MRS r0, PSP
        PUSH {LR}
        BL SVC_APP_Handler
        POP {PC}
        END

;************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****
