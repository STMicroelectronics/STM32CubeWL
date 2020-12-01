/**
  @page HSEM_Sync_DualCore HSEM Sync DualCore example

  @verbatim
  ******************************************************************************
  * @file    HSEM/HSEM_Sync_DualCore/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the HSEM Sync DualCore example.
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

How to synchronise 2 cpus using HSEM peripherals to access safely a shared resource.

In this example, the CPU1 represents an application using a thread ID1. The CPU2 represents an application without any thread. This permits to illustrate the usage of HAL_HSEM_Take(HSEM_ID, HSEM_PROCESS) for CPU1 and HAL_HSEM_FastTake(HSEM_ID) for CPU2.

Preliminary step: Flash memory must be loaded with CPU1 and CPU2 programs.
Procedure using IDE:
  1. Load CPU1 test program, then execute it (it will perform CPU2 boot)
  2. Load CPU2 test program

Example execution:
1. CPU1 reserves the shared resource for ID1.
2. CPU1 boot CPU2
3. CPU2 tries to reserve the shared resource and fails. It then set a callback to be notified by interrupt when the shared resource become available again.
4. CPU1 emulates a process execution during 5 sec, then frees the shared resource.
5. CPU2 receives the notification and reserves the shared resource.

Connections required:
None (LED are already connected on Nucleo board)

Peripherals used:
HSEM and 3 GPIO to monitor CPU activity through LED:
- LED1 for CPU1 activity: turned on when CPU1 holds the semaphore
- LED2 for CPU2 activity: toggle while waiting for semaphore release, turned on when CPU2 holds the semaphore
- LED3 for CPU1 and CPU2 error handler

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Hardware semaphore, Semaphore, HSEM, EXTI, synchronize, Lock, Unlock, Take, Release, Process, Dual core

@par Directory contents

  - HSEM/HSEM_Sync_DualCore/CM0PLUS/Src/main.c                  Main program
  - HSEM/HSEM_Sync_DualCore/CM0PLUS/Src/stm32wlxx_hal_msp.c     HAL MSP module
  - HSEM/HSEM_Sync_DualCore/CM0PLUS/Src/stm32wlxx_it.c          Interrupt handlers
  - HSEM/HSEM_Sync_DualCore/CM0PLUS/Inc/main.h                  Main program header file
  - HSEM/HSEM_Sync_DualCore/CM0PLUS/Inc/stm32wlxx_hal_conf.h    HAL Configuration file
  - HSEM/HSEM_Sync_DualCore/CM0PLUS/Inc/stm32wlxx_it.h          Interrupt handlers header file
  - HSEM/HSEM_Sync_DualCore/CM0PLUS/Inc/stm32wlxx_nucleo_conf.h BSP Driver Configuration file
  - HSEM/HSEM_Sync_DualCore/CM4/Src/main.c                      Main program
  - HSEM/HSEM_Sync_DualCore/CM4/Src/stm32wlxx_hal_msp.c         HAL MSP module
  - HSEM/HSEM_Sync_DualCore/CM4/Src/stm32wlxx_it.c              Interrupt handlers
  - HSEM/HSEM_Sync_DualCore/CM4/Inc/main.h                      Main program header file
  - HSEM/HSEM_Sync_DualCore/CM4/Inc/stm32wlxx_hal_conf.h        HAL Configuration file
  - HSEM/HSEM_Sync_DualCore/CM4/Inc/stm32wlxx_it.h              Interrupt handlers header file
  - HSEM/HSEM_Sync_DualCore/CM4/Inc/stm32wlxx_nucleo_conf.h     BSP Driver Configuration file
  - HSEM/HSEM_Sync_DualCore/Common/system_stm32wlxx.c           STM32WLxx system clock configuration file

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
