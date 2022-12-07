/**
  @page FreeRTOS_HSEM_DualCore FreeRTOS Stop Mode example

  @verbatim
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_HSEM_DualCore/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FreeRTOS HSEM DualCore example.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Application Description

How to synchronise 2 cpus using HSEM peripherals and CMSISOS2.0 RTOS API application.

CM4 : This application creates two threads.

   + A "RxThread" that blocks on a the "osqueue" message queue to wait for data. As soon as
     data is available in the "osqueue" the LED2 blinks.
	 
   + A "TxThread" that waits for a hardware semaphore. Once the semaphore is released,
     the "TxThread" puts message in "osqueue".

CM0PLUS : This application relesase the hardware semaphore each 5s.

Observed behaviour:
Each 5s, the LED2 is toggling as sign for wakeup from "Stop Mode". In fact, when the threads
are not running, i.e "Idle Mode", the FreeRTOS through the PreSleepProcessing(), that 
calls HAL_PWR_EnterHSEM(), makes the system enter in "STOP mode" (power consumption as low as 2mA).

Preliminary step: Flash memory must be loaded with CPU1 and CPU2 programs.
Procedure using IDE:
  1. Load CPU1 test program, then execute it (it will perform CPU2 boot)
  2. Load CPU2 test program

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in HAL time base ISR.
      This implies that if HAL_Delay() is called from a peripheral ISR process, then
      the HAL time base interrupt must have higher priority (numerically lower) than
      the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority()
      function.

@note The application needs to ensure that the HAL time base is always set to 1 millisecond
      to have correct HAL operation.

@note The FreeRTOS heap size configTOTAL_HEAP_SIZE defined in FreeRTOSConfig.h is set accordingly to the
      OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.
	  
@note In "Stop Mode" the MCU isn't accessible by the debugger.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

@par Keywords

FreeRTOS, RTOS, Hardware semaphore, Semaphore, HSEM

@par Directory contents
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM4/Src/app_freertos.c                            FreeRTOS application file
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM4/Src/main.c                                       Main program
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM4/Src/stm32wlxx_hal_timebase_tim.c     HAL timebase file
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM4/Src/stm32wlxx_it.c                            Interrupt handlers
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM4/Inc/main.h                                       Main program header file
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM4/Inc/stm32wlxx_hal_conf.h                  HAL Library Configuration file
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM4/Inc/stm32wlxx_it.h                            Interrupt handlers header file
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM4/Inc/FreeRTOSConfig.h                        FreeRTOS Configuration file
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM0PLUS/Src/main.c                                 Main program
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM0PLUS/Src/stm32wlxx_it.c                     Interrupt handlers
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM0PLUS/Inc/main.h                                Main program header file
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM0PLUS/Inc/stm32wlxx_hal_conf.h           HAL Library Configuration file
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/CM0PLUS/Inc/stm32wlxx_it.h                     Interrupt handlers header file
    - FreeRTOS/FreeRTOS_HSEM_DualCore_CM0PLUS/Common/system_stm32wlxx.c                   STM32WLxx system clock configuration file

@par Hardware and Software environment

  - This application runs on STM32WL55JCIx devices.

  - This application has been tested with NUCLEO-WL55JC RevC board and can be
    easily tailored to any other supported device and development board.

@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 */
