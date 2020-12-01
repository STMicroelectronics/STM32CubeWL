/**
  @page PWR_OptimizedRunMode PWR PWR_OptimizedRunMode
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/PWR/PWR_OptimizedRunMode/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PWR Optimized Run Mode example.
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

How to increase/decrease frequency and VCORE and how to enter/exit the
Low-power run mode.

In the associated software, the system clock is set to 48MHz, an EXTI line
is connected to the User push-button (B1) through PA.00 and configured to generate an 
interrupt on falling edge upon key press.

After start-up LED2 is toggling FAST (100ms blinking period), indicates that device 
is running at 48MHz.

LED2 toggling speed is controlled by variable "uhLedBlinkSpeed".

The User push-button (B1) can be pressed at any time to change Frequency, VCore(VOS) 
and Low Power Run mode. 

Initial STATE: 
--> Freq: 48MHz, VCore 1.2V, Core in Run Mode
--> LED2 toggling FAST (100ms)   - wait User push-button (B1) action

STATE 2: 
User push-button (B1) pressed:
--> Freq: 16MHz, VCore 1.0V, Core in Run Mode
--> LED2 toggling MEDIUM (200ms) - wait User push-button (B1) action

STATE 3: 
User push-button (B1) pressed:
--> Freq:  100KHz, VCore 1.0V, Core in Low Power Run Mode
--> LED2 toggling SLOW (400ms)   - wait User push-button (B1) action

STATE 4: 
User push-button (B1) pressed:
--> Freq: 16MHz, VCore 1.0V, Core in Run Mode
--> LED2 toggling MEDIUM (200ms) - wait User push-button (B1) action

Final STATE: 
User push-button (B1) pressed:
--> Freq: 48MHz, VCore 1.2V, Core in Run Mode
--> LED2 toggling FAST (100ms) in infinite loop


@note To measure MCU current consumption on board STM32WL Nucleo,
      board configuration must be applied:
      - remove all jumpers on connector JP8 to avoid leakages between ST-Link circuitry and STM32WL device.
      - remove jumper JP1 and connect an amperemeter to measure current between the 2 connectors of the jumper.
      NB: LED2 has an impact on power consumption. 
          Remove LED2 blinking to have a constant power consumption, 
          comment line  "#define USE_LED" in main.c file  

@note This example may not be used in debug mode depending on IDE and debugger
      configuration selected, due to stsem low frequency and low power mode
      constraints.

@par Keywords

Power, PWR, Low-power run mode, Interrupt, VCORE, Low Power

@par Directory contents 

  - PWR/PWR_OptimizedRunMode/Inc/stm32wlxx_it.h          Interrupt handlers header file
  - PWR/PWR_OptimizedRunMode/Inc/main.h                  Header for main.c module
  - PWR/PWR_OptimizedRunMode/Inc/stm32_assert.h          Template file to include assert_failed function
  - PWR/PWR_OptimizedRunMode/Src/stm32wlxx_it.c          Interrupt handlers
  - PWR/PWR_OptimizedRunMode/Src/main.c                  Main program
  - PWR/PWR_OptimizedRunMode/Src/system_stm32wlxx.c      STM32WLxx system source file


@par Hardware and Software environment

  - This example runs on STM32WL55JCIx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-WL55JC RevC
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-WL55JC RevC Set-up
    - LED2 connected to pin PB.09
    - User push-button connected to pin PA.00

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
