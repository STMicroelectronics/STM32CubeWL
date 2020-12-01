/**
  @page RCC_LSEConfig RCC Clock Config example

  @verbatim
  ******************************************************************************
  * @file    RCC/RCC_LSEConfig/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the RCC Clock Config example.
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

Enabling/disabling of the low-speed external(LSE) RC oscillator (about 32 KHz) at run time, using the RCC HAL API.


Board: NUCLEO-WL55JC RevC (embeds a STM32WL55JC device)
Microcontroller Clock Output MCO1 Pin: PA.08

   _________________________                   _______________________
  |     _________CN10_______|                 |  _______________      |
  |    |   RCC              |                 | |               | O   |
  |    |                    |                 | |   _   _   _   | O   |
  |    |(pin 16) MCO1(PA.08)|_________________| | _| |_| |_| |_ | --  |
  |    |                    |                 | |_______________| --  |
  |    |                    |                 |                       |
  |    |                    |                 |_Oscilloscope__________|
  |    |                    |
  |    |                    |
  |    |____________________|
  |                         |
  |                         |
  |_STM32_Board ____________|

In this example, after startup SYSCLK is configured to the max frequency using the PLL with
HSI as clock source, the User push-button (B1) (connected to External line 0)
is used to enable/disable LSE:
each time the User push-button (B1) is pressed, External line 0 interrupt is generated and the ISR
enables the LSE when it is off, disables it when it is on.


- when LSE is off, LED2 is toggling (every 300 ms)
- when LSE is on, LED1 is toggling (every 300 ms)

LSE clock waveform is available on the MCO1 pin PA.08 and can be captured
on an oscilloscope.


In case of configuration error, LED3 is On.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

System, RCC, PLL, PLLCLK, SYSCLK, LSE, Clock, Oscillator,

@par Directory contents

  - RCC/RCC_LSEConfig/Inc/stm32wlxx_nucleo_conf.h     BSP configuration file
  - RCC/RCC_LSEConfig/Inc/stm32wlxx_hal_conf.h    HAL configuration file
  - RCC/RCC_LSEConfig/Inc/stm32wlxx_it.h          Interrupt handlers header file
  - RCC/RCC_LSEConfig/Inc/main.h                        Header for main.c module
  - RCC/RCC_LSEConfig/Src/stm32wlxx_it.c          Interrupt handlers
  - RCC/RCC_LSEConfig/Src/main.c                        Main program
  - RCC/RCC_LSEConfig/Src/system_stm32wlxx.c      STM32WLxx system source file
  - RCC/RCC_LSEConfig/Src/stm32wlxx_hal_msp.c     HAL MSP module

@par Hardware and Software environment

  - This example runs on   devices.

  - This example has been tested with NUCLEO-WL55JC RevC
    board and can be easily tailored to any other supported device
    and development board.

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
