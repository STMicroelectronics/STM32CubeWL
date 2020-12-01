/**
  @page RCC_LSIConfig RCC LSI Clock Config example
  
  @verbatim
  ******************************************************************************
  * @file    RCC/RCC_LSIConfig/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RCC LSI Clock Config example.
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

How to enable/disable the low-speed internal (LSI) RC oscillator (about 32 KHz) at run time, using the RCC HAL API.


Board: NUCLEO-WL55JC RevC (embeds a STM32WL55JC device)
Microcontroller Clock Output MCO1 Pin: PA.08

   _________________________                   _______________________
  |     _________CN10_______|                 |  _______________      |
  |    |   RCC              |                 | |               | O   |
  |    |                    |                 | |   _   _   _   | O   |
  |    |(pin 16) MCO1(PA.08)|_________________| | _| |_| |_| |_ | --  |
  |    |                    |                 | |_______________| --  |
  |    |                    |                 |                       |
  |    |                    |                 |_oscilloscope__________|
  |    |                    |
  |    |                    |
  |    |____________________|
  |                         |
  |                         |
  |_STM32_Board ____________|

In this example, after startup SYSCLK is configured to the max frequency using the PLL with
HSI as clock source, the User push-button (B1) (connected to External line 0)
is used to enable/disable LSI:
each time the User push-button (B1) is pressed, External line 0 interrupt is generated and the ISR
enables the LSI when it is off, disables it when it is on.

LED2 behaviour is as follows according to LSI activation:
when LSI is on, LED2 is toggling (every 100 ms).
when LSI is off, LED2 is on.
LSI clock waveform is available on the MCO1 pin PA.08 and can be captured
on an oscilloscope.

In case of configuration error, LED3 is on.


@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

System, RCC, LSI, PLL, PLLCLK, SYSCLK, Clock, Oscillator

@par Directory contents

  - RCC/RCC_LSIConfig/Inc/stm32wlxx_hal_conf.h    HAL configuration file
  - RCC/RCC_LSIConfig/Inc/stm32wlxx_it.h          Interrupt handlers header file
  - RCC/RCC_LSIConfig/Inc/main.h                        Header for main.c module
  - RCC/RCC_LSIConfig/Src/stm32wlxx_it.c          Interrupt handlers
  - RCC/RCC_LSIConfig/Src/main.c                        Main program
  - RCC/RCC_LSIConfig/Src/system_stm32wlxx.c      STM32WLxx system source file
  - RCC/RCC_LSIConfig/Src/stm32wlxx_hal_msp.c     HAL MSP module

@par Hardware and Software environment

  - This example runs on STM32WL55JCIx devices.

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
