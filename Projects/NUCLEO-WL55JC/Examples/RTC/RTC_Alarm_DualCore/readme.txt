/**
  @page RTC_Alarm_DualCore RTC Alarm Example

  @verbatim
  ******************************************************************************
  * @file    RTC/RTC_Alarm_DualCore/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the RTC Alarm example.
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
This multicore project shows how CPU1 (CM4) uses the Alarm A and CPU2 (CM0) uses Alarm B.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 48 MHz.
The RTC peripheral configuration is ensured by the HAL_RTC_Init() function.
This later is calling the HAL_RTC_MspInit()function which core is implementing
the configuration of the needed RTC resources according to the used hardware (CLOCK,
PWR, RTC clock source and BackUp). You may update this function to change RTC configuration.

LSE oscillator clock is used as RTC clock source .

Preliminary step: Flash memory must be loaded with CPU1 and CPU2 programs.
Procedure using IDE:
  1. Load CPU1 program, then execute it (it will perform CPU2 boot)
  2. Load CPU2 program
  3. Perform a system reset (or SW reset of CPU2 program only)

In this example :
 - CPU1 configures the RTC, set the Time to 00:10:00 and programs the Alarm A with interrupt at 00:10:02.
 - CPU1 boot the CPU2.
 - CPU2 programs the Alarm B with interrupt at 00:10:04.

Three GPIO are used to monitor CPU activity through LED:
- LED1 for CPU1 activity: turned on when RTC Alarm A interruption is generated correctly (2s after CPU1 boot)
- LED2 for CPU2 activity: turned on when RTC Alarm B interruption is generated correctly (4s after CPU2 boot)
- LED3 for CPU1 and CPU2 in case of error: LED toggle

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Multicore, RTC, Alarm, LSE

@par Directory contents

  - RTC/RTC_Alarm_DualCore/CM4/Inc/stm32wlxx_nucleo_conf.h         BSP configuration file
  - RTC/RTC_Alarm_DualCore/CM4/Inc/stm32wlxx_hal_conf.h            HAL configuration file
  - RTC/RTC_Alarm_DualCore/CM4/Inc/stm32wlxx_it.h                  Interrupt handlers header file
  - RTC/RTC_Alarm_DualCore/CM4/Inc/main.h                          Header for main.c module
  - RTC/RTC_Alarm_DualCore/CM4/Src/stm32wlxx_it.c                  Interrupt handlers
  - RTC/RTC_Alarm_DualCore/CM4/Src/main.c                          Main program
  - RTC/RTC_Alarm_DualCore/CM4/Src/stm32wlxx_hal_msp.c             HAL MSP module
  - RTC/RTC_Alarm_DualCore/CM0PLUS/Inc/stm32wlxx_nucleo_conf.h     BSP configuration file
  - RTC/RTC_Alarm_DualCore/CM0PLUS/Inc/stm32wlxx_hal_conf.h        HAL configuration file
  - RTC/RTC_Alarm_DualCore/CM0PLUS/Inc/stm32wlxx_it.h              Interrupt handlers header file
  - RTC/RTC_Alarm_DualCore/CM0PLUS/Inc/main.h                      Header for main.c module
  - RTC/RTC_Alarm_DualCore/CM0PLUS/Src/stm32wlxx_it.c              Interrupt handlers
  - RTC/RTC_Alarm_DualCore/CM0PLUS/Src/main.c                      Main program
  - RTC/RTC_Alarm_DualCore/CM0PLUS/Src/stm32wlxx_hal_msp.c         HAL MSP module
  - RTC/RTC_Alarm_DualCore/Common/System/system_stm32wlxx.c        STM32WLxx system source file


@par Hardware and Software environment

 - This example runs on STM32WL55JCxx devices.

 - This example has been tested with STMicroelectronics NUCLEO-WL55JC RevB
   board and can be easily tailored to any other supported device
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

