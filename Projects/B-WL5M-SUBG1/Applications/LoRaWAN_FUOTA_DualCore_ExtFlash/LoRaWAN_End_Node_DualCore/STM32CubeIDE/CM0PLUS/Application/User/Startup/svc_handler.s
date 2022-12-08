/**
  ******************************************************************************
  * @file      svc_handler.s
  * @author    MCD Application Team
  * @brief     Wrapper for SE isolation with MPU.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

        .section  .text,"ax",%progbits
        .syntax unified
        .weak  SVC_APP_Handler
        .global SVC_Handler
        .type  SVC_Handler, %function
SVC_Handler:
        MRS r0, PSP
        PUSH {LR}
        BL SVC_APP_Handler
        POP {PC}
.end
