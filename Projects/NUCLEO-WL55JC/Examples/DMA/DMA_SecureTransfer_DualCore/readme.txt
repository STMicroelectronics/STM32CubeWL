/**
  @page DMA_SecureTransfer_DualCore DMA secure to non secure transfer Example
  
  @verbatim
  ******************************************************************************
  * @file    DMA/DMA_SecureTransfer_DualCore/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the DMA secure to non secure transfer example.
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

How to configure DMA to perform a data transfer from secured area to unsecure area.

This example runs on both CPU, each CPU executes its program independently.
CPU1 is configuring overall system clock, then waits for CPU2 to be ready. 
Once CPU2 is ready, CPU1 checks unsecure specific SRAM1 address content.
CPU2 configures GTZC for security, then configure DMA channel to perform a 
secure memory to unsecure memory transfer. Once done CPU2 warns CPU2.

Preliminary step:
1. Flash memory must be loaded with CPU1 and CPU2 programs.
   Procedure using IDE:
  1.1. Load CPU1 test program, then execute it (it will perform CPU2 boot)
  1.2. Load CPU2 test program
2. Option bytes have to be modified to meet following conditions:
  2.1. Flash security is enabled (FSD = 0) so overall product security is enable. 
       Flash security start address (SFSA) has to be defined to half of the Flash (SFSA = 0x40)
  2.2. SRAM1 (Non-backup SRAM1) has to be configured to have second half secured (SNBRSA = 0x10, NBRSD = 0)

Example execution:
A. CPU1 code:
1. Configure LED1 as user interface output
2. Configure system clocks
3. Boot CPU2
4. wait for CPU2 to be ready
5. check DMA transfer with expected value

B. CPU2 code:
1. Configure security through GTZC
2. Configure DMA channel to be a memory to memory, and secure to unsecure data transfer. 
3. Configure push button B1 as user interface input
4. On user event, DMA starts transfer. 
5. On transfer complete interrupt, CPU2 warns CPU1 

This behaviour is illustrated using LEDs. 
  - LED1 is blinking to indicate that system is being configured or waiting for user action.
  - LED2 is set on to indicate the successful transfer from secured area to unsecure area.
  - LED3 is kept on to indicate any error.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

DMA, Security, Transfer, Secure, Securable, Illegal access

@par Directory contents 

  - DMA/DMA_SecureToNonSecureTransfer/CM4/Inc/stm32wlxx_nucleo_conf.h         BSP configuration file
  - DMA/DMA_SecureToNonSecureTransfer/CM4/Inc/stm32wlxx_hal_conf.h            HAL configuration file
  - DMA/DMA_SecureToNonSecureTransfer/CM4/Inc/stm32wlxx_it.h                  Interrupt handlers header file
  - DMA/DMA_SecureToNonSecureTransfer/CM4/Inc/main.h                          Header for main.c module  
  - DMA/DMA_SecureToNonSecureTransfer/CM4/Src/stm32wlxx_it.c                  Interrupt handlers
  - DMA/DMA_SecureToNonSecureTransfer/CM4/Src/main.c                          Main program
  - DMA/DMA_SecureToNonSecureTransfer/CM4/Src/stm32wlxx_hal_msp.c             HAL MSP module
  - DMA/DMA_SecureToNonSecureTransfer/CM0PLUS/Inc/stm32wlxx_nucleo_conf.h     BSP configuration file
  - DMA/DMA_SecureToNonSecureTransfer/CM0PLUS/Inc/stm32wlxx_hal_conf.h        HAL configuration file
  - DMA/DMA_SecureToNonSecureTransfer/CM0PLUS/Inc/stm32wlxx_it.h              Interrupt handlers header file
  - DMA/DMA_SecureToNonSecureTransfer/CM0PLUS/Inc/main.h                      Header for main.c module  
  - DMA/DMA_SecureToNonSecureTransfer/CM0PLUS/Src/stm32wlxx_it.c              Interrupt handlers
  - DMA/DMA_SecureToNonSecureTransfer/CM0PLUS/Src/main.c                      Main program
  - DMA/DMA_SecureToNonSecureTransfer/CM0PLUS/Src/stm32wlxx_hal_msp.c         HAL MSP module
  - DMA/DMA_SecureToNonSecureTransfer/Common/System/system_stm32wlxx.c        STM32WLxx system source file


@par Hardware and Software environment

  - This example runs on STM32WL55JCxx devices.

  - This example has been tested with STMicroelectronics NUCLEO-WL55JC RevB
    board and can be easily tailored to any other supported device 
    and development board.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
 
