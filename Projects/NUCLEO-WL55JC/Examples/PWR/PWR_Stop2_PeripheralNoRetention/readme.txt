/**
  @page PWR_Stop2_PeripheralNoRetention PWR SMPS example

  @verbatim
  ******************************************************************************
  * @file    PWR/PWR_Stop2_PeripheralNoRetention/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the PWR_Stop2_PeripheralNoRetention example.
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

How to use low-power mode Stop2 and peripherals not featuring retention
of configuration.

STM32WL is featuring differences on low-power modes Stop0/Stop1 versus Stop2:
on Stop2, some peripherals have no retention of their configuration.
This example demonstrates how to use a peripheral not retained in Stop2: RNG
(reconfiguration at system wake-up from Stop2).
This example runs on CPU1 only.

Execution:
The program configures system, GPIO used by LED and user button and RNG peripheral.
Then, program performs the sequence indefinitely:
1. Perform a random number generation using RNG peripheral
2. Wait for press on user button (B1). During this step, toggle LED2 (10Hz).
3. System enter in Stop2 mode
4. Wait for press on user button. During this step, LED2 remains off.
5. At system wake up, turn on LED2 during 1sec
6. Reconfigure RNG peripheral
7. Go to step "1."

Peripherals used:
- GPIO for LED1 "CPU1 activity"
- GPIO for user button B1 "CPU1 wake up"

In case of error, LED2 remains on.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

PWR, Low Power, Stop2 mode, Wakeup, Interrupt, EXTI

@par Directory contents
  - PWR/PWR_Stop2_PeripheralNoRetention/Inc/stm32wlxx_nucleo_conf.h BSP configuration file
  - PWR/PWR_Stop2_PeripheralNoRetention/Inc/stm32wlxx_hal_conf.h    HAL configuration file
  - PWR/PWR_Stop2_PeripheralNoRetention/Inc/stm32wlxx_it.h          Interrupt handlers header file
  - PWR/PWR_Stop2_PeripheralNoRetention/Inc/main.h                  Header for main.c module
  - PWR/PWR_Stop2_PeripheralNoRetention/Src/stm32wlxx_it.c          Interrupt handlers
  - PWR/PWR_Stop2_PeripheralNoRetention/Src/stm32wlxx_hal_msp.c     HAL MSP file
  - PWR/PWR_Stop2_PeripheralNoRetention/Src/main.c                  Main program
  - PWR/PWR_Stop2_PeripheralNoRetention/Src/system_stm32wlxx.c      STM32WLxx system source file


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

