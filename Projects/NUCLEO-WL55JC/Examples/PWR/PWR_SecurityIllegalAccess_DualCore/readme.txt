/**
  @page PWR_SecurityIllegalAccess_DualCore Description of the PWR_SecurityIllegalAccess_DualCore example
  
  @verbatim
  ******************************************************************************
  * @file    Examples/PWR/PWR_SecurityIllegalAccess_DualCore/readme.txt 
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

How to manage illegal access in multicore program and low-power modes.

This example runs on both CPU and demonstrates how to manage the two
boot/wake up sources of CPU2: normal boot/wake up (from CPU1 or peripheral) and illegal access boot/wake up (ILAC).

Preliminary step:
1. Flash memory must be loaded with CPU1 and CPU2 programs.
   Procedure using IDE:
  1.1. Load CPU1 program, then execute it (it will perform CPU2 boot)
       Note: LED3 indicating error will turn on, indicating that system setup is not yet fulfilled to run the example.
  1.2. Load CPU2 program
  1.3. Perform a system reset (or SW reset of CPU2 program only)
2. Option bytes have to be modified to meet following conditions:
  2.1. Flash security is enabled (FSD = 0) so overall product security is enable. Flash security start address (SFSA) has to be defined to half of the Flash (SFSA = 0x40)
  2.2. SRAM2 (backup SRAM2) security start address has to be configured to half of the SRAM2 (SBRSA = 0x10, BRSD = 0) to match linker file configuration.
       Note: Status of system security enable will be set automatically by device after option bytes application (expected status: ESE = 1)

Example execution:
1. CPU1 program:
1.1. Initial state: CPU1 only boot, CPU2 remains in reset state.
1.2. CPU1 configures system clock, GPIO of components allocated to CPU1 (LED1 and button B1, additionally LED3 in case of error),
1.3. Check whether system security is activated:
     if security activated, toggle LED1 quickly (10Hz) during 2 sec. Else, error case: turn on LED3.
1.4. CPU1 toggle LED1 slowly (1Hz), waiting for user action.
1.5. Illegal access event: Press on user push-button (B1) makes CPU1 perform an illegal access (attempt to write in GTZC register)
     LED1 is turned on during 2 sec
1.6. Steps 1.4 and 1.5 are repeated indefinitely.

2. CPU2 program:
2.1. CPU2 boot by GTZC peripheral or by CPU1
2.2. CPU2 configures GPIO of components allocated to CPU2 (LED2)
2.3. Check CPU2 boot source:
     - CPU2 boot from normal boot request, by CPU1: intermediate phase during setup of this example, while system security not yet activated.
       Go in error handler to not continue program execution (toggle LED2 slowly (1Hz) indefinitely).
     - CPU2 boot from illegal access, by GTZC peripheral
       Use GTZC to process illegal access event: retrieve illegal access source.
       If expected source (illegal access to GTZC peripheral), then toggle LED quickly (10Hz) during 2 sec. Else, go to error handler (toggle LED2 slowly (1Hz) indefinitely).
2.4. Enter in CPU Stop mode (CStop)
     Note: Since CPU1 is in CRun mode, system remains in Run mode.
     Note: Case of CPU2 boot from illegal access: CPU2 entering in DeepSleep mode
     makes CPU2 exit from illegal access mode and set back in reset state.
2.7. At next illegal access occurrence, start from steps 2.1.


Connections required:
None

Peripherals used:
- GPIO for LED1: CPU1 activity
- GPIO for user button B1: illegal access generation
- GPIO for LED2: CPU2 activity (boot source), error CPU2
- GPIO for LED3: error CPU1

@note How to disable security (set back device to initial state):
Option bytes have to be modified following this sequence:
1. Set RDP level to 0xBB
2. Program option bytes
3. Set RDP level to 0xAA, disable system security (ESE=0)
4. Program option bytes

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

PWR, dual core, dual boot, security

@par Directory contents 

  - Examples/PWR/PWR_SecurityIllegalAccess_DualCore/Src/main.c                 Main program
  - Examples/PWR/PWR_SecurityIllegalAccess_DualCore/Src/system_stm32wlxx.c     STM32WLxx system clock configuration file
  - Examples/PWR/PWR_SecurityIllegalAccess_DualCore/Src/stm32wlxx_it.c         Interrupt handlers 
  - Examples/PWR/PWR_SecurityIllegalAccess_DualCore/Src/stm32wlxx_hal_msp.c    HAL MSP module
  - Examples/PWR/PWR_SecurityIllegalAccess_DualCore/Inc/main.h                 Main program header file
  - Examples/PWR/PWR_SecurityIllegalAccess_DualCore/Inc/stm32wlxx_hal_conf.h   HAL Configuration file
  - Examples/PWR/PWR_SecurityIllegalAccess_DualCore/Inc/stm32wlxx_it.h         Interrupt handlers header file


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
