/**
  @page PWR_EnterStopMode PWR standby example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/PWR/PWR_EnterStopMode/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PWR Stop 2 mode example.
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

How to enter the Stop 2 mode.

After start-up LED2 is toggling during 5 seconds, then the system automatically 
enter in Stop 2 mode (Final state).

LED2 is used to monitor the system state as follows:
 - LED2 toggling : system in Run mode
 - LED2 off : system in Stop 2 mode

This example does not implement a wake-up source from any peripheral: to wake-up the device,
press on Reset button.

@note To measure MCU current consumption on board STM32WL Nucleo,
      board configuration must be applied:
      - remove all jumpers on connector JP8 to avoid leakages between ST-Link circuitry and STM32WL device.
      - remove jumper JP1 and connect an amperemeter to measure current between the 2 connectors of the jumper.

@note This example can not be used in DEBUG mode due to the fact 
      that the Cortex-M4 core is no longer clocked during low power mode 
      so debugging features are disabled.

@par Keywords

Power, PWR, Stop mode, Interrupt, Low Power

@par Directory contents 

  - PWR/PWR_EnterStopMode/Inc/stm32wlxx_it.h          Interrupt handlers header file
  - PWR/PWR_EnterStopMode/Inc/main.h                  Header for main.c module
  - PWR/PWR_EnterStopMode/Inc/stm32_assert.h          Template file to include assert_failed function
  - PWR/PWR_EnterStopMode/Src/stm32wlxx_it.c          Interrupt handlers
  - PWR/PWR_EnterStopMode/Src/main.c                  Main program
  - PWR/PWR_EnterStopMode/Src/system_stm32wlxx.c      STM32WLxx system source file

@par Hardware and Software environment

  - This example runs on STM32WL55JCIx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-WL55JC RevC
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-WL55JC RevC Set-up
    - LED2 connected to PB.09 pin

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
