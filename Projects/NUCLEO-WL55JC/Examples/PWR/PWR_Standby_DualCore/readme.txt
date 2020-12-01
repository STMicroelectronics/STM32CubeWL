/**
  @page PWR_Standby_DualCore Description of the PWR_Standby_DualCore example
  
  @verbatim
  ******************************************************************************
  * @file    Examples/PWR/PWR_Standby_DualCore/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the Templates example.
  ******************************************************************************
  *
  * Copyright (c) 2020 STMicroelectronics. All rights reserved.
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                       opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  @endverbatim

@par Example Description

How to manage system low-power mode Standby and each CPU low-power modes in multicore program.

This example runs on both CPU, each CPU executes its program independently: different modes selection (CRun, CStandby), asynchronous low-power modes entry request and wake-up.
The system automatically applies the deepest low-power mode compliant with both CPU low-power mode requests.

Preliminary step: Flash memory must be loaded with CPU1 and CPU2 programs.
Procedure using IDE:
  1. Load CPU1 test program, then execute it (it will perform CPU2 boot)
  2. Load CPU2 test program
  3. Perform a system reset (or SW reset of CPU2 program only)

Example execution:
Configuration:
  - CPU1 makes CPU2 boot
  - Each CPU configures GPIO for LED and user push-button, allocated on their respective bus domain:
    - CPU1: LED1 and button B1, additionally LED3 in case of error
    - CPU2: LED2 and button B2
Execution:
  Each user push-button makes the corresponding CPU toggle between CRun and CStandby mode:
  - if program is in Run mode: request to enter in CStandby mode
  - if program is in low power mode: wake-up from low-power mode
  
  Each LED indicates the corresponding CPU state: toggle slowly (1Hz) infinitively for CRun mode, toggle quickly (10Hz) at wake-up, off for CStandby mode.
  3 cases are differentiated, for each CPU:
  - Wake-up a CPU from system Standby (the other CPU is in Standby or Shutdown mode): toggle quickly LED during 4s
    Wake-up state: CPU and peripherals context reset and RAM content lost, program restart from main
  - Wake-up a CPU from system reset or Run after a phase of system Standby (system was in Standby mode, the other CPU woke-up first and is in CRun mode): toggle quickly LED during 2s
    Wake-up state: CPU and peripherals context reset and RAM content lost, program restart from main
  - Wake-up a CPU from system Run, system never entered in Standby mode (the other CPU is in CRun mode (and remained in Run mode during low-power phase of this CPU)): toggle quickly LED during 1s
    Wake-up state: CPU and peripherals context retained and RAM content retained, program restart from WFE/WFI instruction
    Note: In this case, CPU requested low-power mode is CStandby but CPU effective low-power mode is equivalent to CStop mode. Its wake up sources are the one of Stop mode (any EXTI line), not Standby mode (specific direct wake up lines).

Note: A CPU request to enter in CStandby mode does not guarantee that system will enter in Standby mode.
      To enter in system Standby mode, the other CPU must be in CStandby mode.
      Otherwise, system will enter in the deepest low power mode possible (if the other CPU is in CRun/CStop mode, system will enter in Run/Stop mode).


Connections required:
Connect User push-button (B2) (pin PA1, Morpho connector CN10 pin 36) to wake-up pin 2 (pin PC13 Morpho connector CN7 pin 23).
Note: User push-button (B1) is already connected to wake-up pin 1 on board.

Peripherals used:
5 GPIO used for:
- LED1: "CPU1 activity"
- User push-button (B1): "CPU1 wake up and request to enter in low power mode"
- LED3: "error CPU1"
- LED2: "CPU2 activity"
- User push-button (B2): "CPU2 wake up and request to enter in low power mode"

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

PWR, low power, Standby, dual core

@par Directory contents 

  - Examples/PWR/PWR_Standby_DualCore/Src/main.c                 Main program
  - Examples/PWR/PWR_Standby_DualCore/Src/system_stm32wlxx.c     STM32WLxx system clock configuration file
  - Examples/PWR/PWR_Standby_DualCore/Src/stm32wlxx_it.c         Interrupt handlers 
  - Examples/PWR/PWR_Standby_DualCore/Src/stm32wlxx_hal_msp.c    HAL MSP module
  - Examples/PWR/PWR_Standby_DualCore/Inc/main.h                 Main program header file
  - Examples/PWR/PWR_Standby_DualCore/Inc/stm32wlxx_hal_conf.h   HAL Configuration file
  - Examples/PWR/PWR_Standby_DualCore/Inc/stm32wlxx_it.h         Interrupt handlers header file


@par Hardware and Software environment

  - This example runs on STM32WL55xx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-WL55JC
    boards and can be easily tailored to any other supported device
    and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Load CPU1 test program, then execute it (it will perform CPU2 boot)
 - Load CPU2 test program
 - Perform a system reset (or SW reset of CPU2 program only)
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
