/**
  ******************************************************************************
  * @file      sfu_se_mpu.s
  * @author    MCD Application Team
  * @brief     Wrapper for SE isolation with MPU.
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
        .section  .text,"ax",%progbits
        .syntax unified
        .weak  MPU_SVC_Handler
        .global SVC_Handler
        .type  SVC_Handler, %function
SVC_Handler:
        MRS r0, PSP
        PUSH {LR}
        BL MPU_SVC_Handler
        POP {PC}

        .section  .RamFunc,"ax",%progbits       
        .global jump_to_function
jump_to_function:
        /* assume R1 R2 are useless */
        LDR R1, [R0]
        LDR R2, [R0,#4]
        MOV SP, R1
        BX  R2

        .global launch_application
launch_application:
        /* return from exception to application launch function */
        /* R0: application vector address */
        /* R1: exit function address */
        /* push interrupt context R0 R1 R2 R3 R12 LR PC xPSR */
        /* MOV R2, #0x01000000 ; xPSR activate Thumb bit */
        MOVS R2, #0x1
        LSLS R2, R2, #24
        PUSH {R2} /* FLAGS=0 */
        MOVS R2, #1
        BICS R1, R1, R2  /* clear least significant bit of exit function */
        PUSH {R1}  /* return address = application entry point */
        MOVS R1, #0 /* clear other context registers */
        PUSH {R1} /* LR =0 */
        PUSH {R1} /* R12 =0 */
        PUSH {R1} /* R3 = 0 */
        PUSH {R1} /* R2 = 0 */
        PUSH {R1} /* R1 = 0 */
        /* set R0 to application entry point */
        PUSH {R0} /* R0 = application entry point */
        /* set LR to return to thread mode with main stack */
        /* MOV LR, #0xFFFFFFF9 */
        MOVS R2, #6
        MVNS R2, R2
        MOV LR, R2
        /* return from interrupt */
        BX LR
.end
