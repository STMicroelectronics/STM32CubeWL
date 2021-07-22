/**
  @page GTZC_MemoryWatermarkProtection_DualCore Description of the GTZC_MemoryWatermarkProtection_DualCore example

  @verbatim
  ******************************************************************************
  * @file    GTZC_MemoryWatermarkProtection_DualCore/readme.txt
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

How to protect secure memory area from unprivileged access and set up illegal 
access notification.

This example runs on both CPU, each CPU executes its program independently.
CPU1 is configuring overall system clock, then start CPU2 and wait forever.
CPU2 setup Backup SRAM2 privilege protection starting from half of the SRAM2
and enable Illegal Exception detection on SRAM2. On user action, CPU2 is 
turning itself in unprivileged mode, then attempt to read protected address.
Illegal access interrupt is raised. Then CPU2 turns back in privileged
mode and attempt successfully to access secured privileged address. User is
informed that overall test is successful.

Preliminary step:
1. Flash memory must be loaded with CPU1 and CPU2 programs.
   Procedure using IDE:
  1.1. Load CPU1 test program, then execute it (it will perform CPU2 boot)
  1.2. Load CPU2 test program
2. Option bytes have to be modified to meet following conditions:
  2.1. Flash security is enabled (FSD = 0) so overall product security is enable. Flash security start address (SFSA) has to be defined to half of the Flash (SFSA = 0x40)
  2.2. SRAM1 (Non-backup SRAM1) has to be configured to have second half secured (SNBRSA = 0x10, NBRSD = 0)
  2.3. SRAM2 (backup SRAM2) has to be configured to have second half secured (SBRSA = 0x10, BRSD = 0)

Example execution:
A. CPU1 code:
1. Configure LED1 as user interface output
2. Configure system clocks
3. Boot CPU2
4. Wait forever

B. CPU2 code:
1. Enable illegal access detection linked to SRAM2
2. Enable privileged SRAM2 area protection from half of SRAM2
3. Change CPU mode to privileged, and try to access half of SRAM2 address
4. Check Illegal Access interrupt occurs
5. Change CPU mode back to privileged and try again to read memory., warn user through LED1 fast blink
6. Check Illegal Access interrupt does not occur and warn user example is successful

This behaviour is illustrated using LEDs.
  - LED1 is blinking slowly to indicate that system is being configured or waiting for user action
  - LED1 is blinking rapidly to indicate an illegal access event occurred.
  - LED1 is kept on to indicate any error

@note If Illegal Exception occurred, CPU2 will boot alone a next System reset. As a consequence,
      a power off / on sequence has to be performed to replay example.

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

Security, GTZC, TZSC, TZIC, Secure, Privilege, Watermark, Illegal access, Memory

@par Directory contents

  - GTZC_MemoryWatermarkProtection_DualCore/CM0PLUS/Src/main.c                 	Main program
  - GTZC_MemoryWatermarkProtection_DualCore/CM0PLUS/Src/stm32wlxx_hal_msp.c    	HAL MSP module
  - GTZC_MemoryWatermarkProtection_DualCore/CM0PLUS/Src/stm32wlxx_it.c         	Interrupt handlers
  - GTZC_MemoryWatermarkProtection_DualCore/CM0PLUS/Inc/main.h                 	Main program header file
  - GTZC_MemoryWatermarkProtection_DualCore/CM0PLUS/Inc/stm32wlxx_hal_conf.h   	HAL Configuration file
  - GTZC_MemoryWatermarkProtection_DualCore/CM0PLUS/Inc/stm32wlxx_it.h         	Interrupt handlers header file
  - GTZC_MemoryWatermarkProtection_DualCore/CM0PLUS/Inc/stm32wlxx_nucleo_conf.h	Nucleo Configuration file
  - GTZC_MemoryWatermarkProtection_DualCore/CM4/Src/main.c                     	Main program
  - GTZC_MemoryWatermarkProtection_DualCore/CM4/Src/stm32wlxx_hal_msp.c        	HAL MSP module
  - GTZC_MemoryWatermarkProtection_DualCore/CM4/Src/stm32wlxx_it.c             	Interrupt handlers
  - GTZC_MemoryWatermarkProtection_DualCore/CM4/Inc/main.h                     	Main program header file
  - GTZC_MemoryWatermarkProtection_DualCore/CM4/Inc/stm32wlxx_hal_conf.h       	HAL Configuration file
  - GTZC_MemoryWatermarkProtection_DualCore/CM4/Inc/stm32wlxx_it.h             	Interrupt handlers header file
  - GTZC_MemoryWatermarkProtection_DualCore/CM4/Inc/stm32wlxx_nucleo_conf.h	    Nucleo Configuration file
  - GTZC_MemoryWatermarkProtection_DualCore/common/system_stm32wlxx.c          	STM32WLxx system clock configuration file



@par Hardware and Software environment

  - This example runs on STM32WL55JCxx devices.

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
