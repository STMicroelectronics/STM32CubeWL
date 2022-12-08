/**
  @page FreeRTOS_ThreadFlags FreeRTOS Thread Flags example

  @verbatim
  ******************************************************************************
  * @file    FreeRTOS/FreeRTOS_ThreadFlags/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the FreeRTOS Thread Flags example.
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

How to use thread flags with CMSIS_RTOS2 API.

This application creates three threads.

   + A "GenThread" that :
      - Waits 500ms before setting the thread flags for the first Rx thread (using osThreadFlagsSet () ). 
      - Waits 500ms before setting the thread flags for the second Rx thread (using osThreadFlagsSet () ). 

   + A "RxThread1" which waits for the thread flags setting (using osThreadFlagsWait() ) before setting 
     on the LED1.

	 
   + A "RxThread2" which waits for the thread flags setting (using osThreadFlagsGet() ) before setting 
     off the LED1 and clearing the thread flags (using osThreadFlagsClear() ).

Observed behaviour:
  - Each 500ms, the LED1 is toggling.
  - The LED3 is on if any error occurs.

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
	  

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

@par Keywords

RTOS, FreeRTOS, Threading, ThreadFlags

@par Directory contents
    - FreeRTOS/FreeRTOS_ThreadFlags/Src/app_freertos.c               FreeRTOS application file
    - FreeRTOS/FreeRTOS_ThreadFlags/Src/main.c                       Main program
    - FreeRTOS/FreeRTOS_ThreadFlags/Src/stm32wlxx_hal_timebase_tim.c HAL timebase file
    - FreeRTOS/FreeRTOS_ThreadFlags/Src/stm32wlxx_it.c               Interrupt handlers
    - FreeRTOS/FreeRTOS_ThreadFlags/Src/system_stm32wlxx.c           STM32WLxx system clock configuration file
    - FreeRTOS/FreeRTOS_ThreadFlags/Inc/main.h                       Main program header file
    - FreeRTOS/FreeRTOS_ThreadFlags/Inc/stm32wlxx_hal_conf.h         HAL Library Configuration file
    - FreeRTOS/FreeRTOS_ThreadFlags/Inc/stm32wlxx_it.h               Interrupt handlers header file
    - FreeRTOS/FreeRTOS_ThreadFlags/Inc/FreeRTOSConfig.h             FreeRTOS Configuration file

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
