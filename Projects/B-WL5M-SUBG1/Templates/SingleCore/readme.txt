/**
  @page Templates  Description of the Templates example
  
  @verbatim
  ******************************************************************************
  * @file    Templates/SingleCore/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the Templates example.
  ******************************************************************************
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Example Description

This projects provides a reference template that can be used to build any Single Core (CM4) firmware application.

This directory provides a reference template project that can be used to build any firmware application for
STM32WL5Mxx devices using STM32CubeWL HAL and running on B-WL5M-SUBG board from STMicroelectronics. 

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Directory contents 

  - Templates/SingleCore/Src/main.c                 Main program
  - Templates/SingleCore/Src/system_stm32wlxx.c     STM32WLxx system clock configuration file
  - Templates/SingleCore/Src/stm32wlxx_it.c         Interrupt handlers 
  - Templates/SingleCore/Src/stm32wlxx_hal_msp.c    HAL MSP module
  - Templates/SingleCore/Inc/main.h                 Main program header file  
  - Templates/SingleCore/Inc/stm32wlxx_hal_conf.h   HAL Configuration file
  - Templates/SingleCore/Inc/stm32wlxx_it.h         Interrupt handlers header file


@par Hardware and Software environment

  - This example runs on STM32WL5Mxx devices.
    
  - This example has been tested with STMicroelectronics B-WL5M-SUBG1
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
