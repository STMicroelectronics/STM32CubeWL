/**
  @page GTZC_GlobalSecurityConfiguration_DualCore Description of the GTZC_GlobalSecurityConfiguration_DualCore example

  @verbatim
  ******************************************************************************
  * @file    GTZC_GlobalSecurityConfiguration_DualCore/readme.txt
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

How to configure Flash option bytes and GTZC to fully secure all resources from unsecured accesses.

This example runs on both CPU, each CPU executes its program independently.
CPU1 is configuring overall system clock, then start CPU2 and wait for ever.
At first power on, CPU2 configures security Flash option bytes. At any next reset GTZC is configured to protect all securable peripherals and to detect all kind of illegal access.
DMA channels are also configured to be secured and privileged.

Preliminary step:
Flash memory must be loaded with CPU1 and CPU2 programs. Procedure using IDE:
1. Load CPU1 test program, then execute it (it will perform CPU2 boot)
2. Load CPU2 test program
3. Perform Power off & on reset, as security configuration is performed at first boot only.

Example execution:
A. CPU1 code:
1. Configure LED1 as user interface output
2. Configure system clocks
3. Boot CPU2
4. Wait for ever

B. CPU2 code:
1. Check boot flag. In case of first boot detected, configure all option bytes related to security. Memory boundaries are taken from common scatter file. 
2. On all next boots, CPU2 enable all Illegal access detection, secure & privilege all securable peripherals, restrict secure memeories to privilege access only and finally locks configuration.
3. DMA channel are secured. 

This behaviour is illustrated using LEDs.
  - LED1 is blinking slowly to indicate that system has been properly configured 

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

Security, GTZC, FLASH, DMA, TZSC, TZIC, MPCWM, Secure, Privilege, Illegal access, Memory, Option bytes, Lock, Power On Reset

@par Directory contents

  - GTZC_GlobalSecurityConfiguration_DualCore/CM0PLUS/Src/main.c                 	Main program
  - GTZC_GlobalSecurityConfiguration_DualCore/CM0PLUS/Src/stm32wlxx_hal_msp.c    	HAL MSP module
  - GTZC_GlobalSecurityConfiguration_DualCore/CM0PLUS/Src/stm32wlxx_it.c         	Interrupt handlers
  - GTZC_GlobalSecurityConfiguration_DualCore/CM0PLUS/Inc/main.h                 	Main program header file
  - GTZC_GlobalSecurityConfiguration_DualCore/CM0PLUS/Inc/stm32wlxx_hal_conf.h   	HAL Configuration file
  - GTZC_GlobalSecurityConfiguration_DualCore/CM0PLUS/Inc/stm32wlxx_it.h         	Interrupt handlers header file
  - GTZC_GlobalSecurityConfiguration_DualCore/CM0PLUS/Inc/stm32wlxx_nucleo_conf.h	Nucleo Configuration file
  - GTZC_GlobalSecurityConfiguration_DualCore/CM4/Src/main.c                     	Main program
  - GTZC_GlobalSecurityConfiguration_DualCore/CM4/Src/stm32wlxx_hal_msp.c        	HAL MSP module
  - GTZC_GlobalSecurityConfiguration_DualCore/CM4/Src/stm32wlxx_it.c             	Interrupt handlers
  - GTZC_GlobalSecurityConfiguration_DualCore/CM4/Inc/main.h                     	Main program header file
  - GTZC_GlobalSecurityConfiguration_DualCore/CM4/Inc/stm32wlxx_hal_conf.h       	HAL Configuration file
  - GTZC_GlobalSecurityConfiguration_DualCore/CM4/Inc/stm32wlxx_it.h             	Interrupt handlers header file
  - GTZC_GlobalSecurityConfiguration_DualCore/CM4/Inc/stm32wlxx_nucleo_conf.h	    Nucleo Configuration file
  - GTZC_GlobalSecurityConfiguration_DualCore/common/System/system_stm32wlxx.c    STM32WLxx system clock configuration file



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
