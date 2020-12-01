/**
  @page PWR_SMPS PWR SMPS example

  @verbatim
  ******************************************************************************
  * @file    PWR/PWR_SMPS/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PWR SMPS example.
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

How to configure and use SMPS through the HAL API.

The program enable SMPS and check back the effective state of SMPS.
At each press on user button (B1), SMPS state is toggled between step-down
and bypass mode.
Effective state of SMPS is monitored by LED2 toggle frequency:
- SMPS is in mode step-down (switching enable): LED2 toggles at fast speed (10Hz).
- SMPS is in mode bypass (switching disable): LED2 toggles at slow speed (1Hz).

The program also enable SMPS clock detection: it is recommended to be enabled
before enable SMPS in switching mode. In case of clock failure, it will
automaticcaly switch off the SMPS (refer to reference manual).
SMPS clock detection is controlled through HAL SUBGHZ peripheral, due to
SMPS and radio related to the same HSE clock.

In case of error, LED2 remains on.

Note: In this example, SMPS effective state will match to requested state, but
      in some cases they can differ (radio and low-power modes can change SMPS
      operating mode).

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

PWR, SMPS, step-down

@par Directory contents
  - PWR/PWR_SMPS/Inc/stm32wlxx_nucleo_conf.h BSP configuration file
  - PWR/PWR_SMPS/Inc/stm32wlxx_hal_conf.h    HAL configuration file
  - PWR/PWR_SMPS/Inc/stm32wlxx_it.h          Interrupt handlers header file
  - PWR/PWR_SMPS/Inc/main.h                  Header for main.c module
  - PWR/PWR_SMPS/Src/stm32wlxx_it.c          Interrupt handlers
  - PWR/PWR_SMPS/Src/stm32wlxx_hal_msp.c     HAL MSP file
  - PWR/PWR_SMPS/Src/main.c                  Main program
  - PWR/PWR_SMPS/Src/system_stm32wlxx.c      STM32WLxx system source file


@par Hardware and Software environment
  - This example runs on STM32WL55JCIx devices.
  - This example has been tested with STMicroelectronics NUCLEO-WL55JC RevC
    board and can be easily tailored to any other supported device
    and development board.

@par How to use it ?
In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */

