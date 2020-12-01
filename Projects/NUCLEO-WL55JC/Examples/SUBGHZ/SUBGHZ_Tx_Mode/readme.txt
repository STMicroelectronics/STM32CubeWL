/**
  @page SUBGHZ_Tx_Mode SUBGHZ Tx Mode example

  @verbatim
  ******************************************************************************
  * @file    SUBGHZ/SUBGHZ_Tx_Mode/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the SUBGHZ Tx Mode example.
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

This example shows how to configure the SUBGHZ peripheral to set the SUBGHZ 
Radio in Tx Mode.

Starting from RADIO_MODE_STANDBY_RC mode, when the PA ramping is completed the 
packet handler starts the packet transmission and SUBGHZ Radio switch in 
RADIO_MODE_TX Mode. 

When the last bit of the packet has been sent, status is updated to 
RADIO_COMMAND_TX_DONE. The PA regulator is ramped down, the PA is switched OFF 
and the SUBGHZ Radio goes back to RADIO_MODE_STANDBY_RC mode.

STM32 board's LEDs can be used to monitor the Tx Done status:
 - LED2 turns ON when the Transmission process is complete (RADIO_COMMAND_TX_DONE).
 - LED3 toggles when there is an error in transmission process.  

@note  PA regulator ramping parameter are not filled in this example. 
       This can be done with the command RADIO_SET_TXPARAMS. 
       Also the Packet is not filled. Aims of example is to switch SUBGHZ Radio
       in Tx Mode and then go back to standby.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Connectivity, SUBGHZ, Transmission

@par Directory contents 

  - SUBGHZ/SUBGHZ_Tx_Mode/Inc/stm32wlxx_nucleo_conf.h     BSP configuration file
  - SUBGHZ/SUBGHZ_Tx_Mode/Inc/stm32wlxx_hal_conf.h        HAL configuration file
  - SUBGHZ/SUBGHZ_Tx_Mode/Inc/stm32wlxx_it.h              Interrupt handlers header file
  - SUBGHZ/SUBGHZ_Tx_Mode/Inc/main.h                      Header for main.c module  
  - SUBGHZ/SUBGHZ_Tx_Mode/Src/stm32wlxx_it.c              Interrupt handlers
  - SUBGHZ/SUBGHZ_Tx_Mode/Src/main.c                      Main program
  - SUBGHZ/SUBGHZ_Tx_Mode/Src/system_stm32wlxx.c          stm32wlxx system source file
  - SUBGHZ/SUBGHZ_Tx_Mode/Src/stm32wlxx_hal_msp.c         HAL MSP file

@par Hardware and Software environment

  - This example runs on STM32WLE5JCIx devices.

  - This example has been tested with NUCLEO-WL55JC RevC board and can be
    easily tailored to any other supported device and development board.

@par How to use it ? 

In order to make the program work, you must do the following:
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
 
