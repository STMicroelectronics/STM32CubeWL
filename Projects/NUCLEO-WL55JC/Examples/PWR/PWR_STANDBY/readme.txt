/**
  @page PWR_STANDBY PWR STANDBY example

  @verbatim
  ******************************************************************************
  * @file    PWR/PWR_STANDBY/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the PWR Standby example.
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

How to enter the Standby mode and wake up from this mode by using an external
reset or the WKUP pin.

In the associated software, the system clock is set to 48 MHz, an EXTI line
is connected to the User push-button (B1) thrum PA0 and configured to generate an
interrupt on falling edge.
The SysTick is programmed to generate an interrupt each 1 ms and in the SysTick
interrupt handler, LED2 is toggled in order to indicate whether the MCU is in Standby or Run mode.

When a FALLING edge is detected on the EXTI line, an interrupt is generated and the system wake up,
the program then checks and clears the standby flag.
After clearing the standby flag, the software enables wake-up pin PWR_WAKEUP_PIN1 connected to PA0, then 
the corresponding flag indicating that a wakeup event was received from the PWR_WAKEUP_PIN1 is cleared.
Finally, the system enters again Standby mode causing LED2 to stop toggling.

A FALLING edge on WKUP pin will wake-up the system from Standby.
Alternatively, an external RESET of the board will lead to a system wake-up as well.

After wake-up from Standby mode, program execution restarts in the same way as after
a RESET and LED2 restarts toggling.

Two leds LED2 and LED3 are used to monitor the system state as follows:
 - LED3 ON: configuration failed (system will go to an infinite loop)
 - LED2 toggling: system in Run mode
 - LED2 off : system in Standby mode

@note To measure the current consumption in Standby mode, remove JP1 jumper
      and connect an amperemeter to JP1 to measure IDD current.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Power, PWR, Standby mode, Interrupt, EXTI, Wakeup, Low Power, External reset

@par Directory contents

  - PWR/PWR_STANDBY/Inc/stm32wlxx_nucleo_conf.h     BSP configuration file
  - PWR/PWR_STANDBY/Inc/stm32wlxx_conf.h         HAL Configuration file
  - PWR/PWR_STANDBY/Inc/stm32wlxx_it.h           Header for stm32wlxx_it.c
  - PWR/PWR_STANDBY/Inc/main.h                         Header file for main.c
  - PWR/PWR_STANDBY/Src/system_stm32wlxx.c       STM32WLxx system clock configuration file
  - PWR/PWR_STANDBY/Src/stm32wlxx_it.c           Interrupt handlers
  - PWR/PWR_STANDBY/Src/main.c                         Main program
  - PWR/PWR_STANDBY/Src/stm32wlxx_hal_msp.c      HAL MSP module

@par Hardware and Software environment

  - This example runs on STM32WL55JCIx devices

  - This example has been tested with STMicroelectronics NUCLEO-WL55JC RevC
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-WL55JC RevC Set-up
    - Use LED2 and LED3 connected respectively to PB.09 and PB.11 pins
    - User push-button (B1) connected to pin PA.00 (External line 0)
    - WakeUp Pin PWR_WAKEUP_PIN1 connected to PA0

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
