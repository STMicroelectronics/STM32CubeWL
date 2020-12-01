/**
  @page USART_SlaveMode USART Slave Mode example

  @verbatim
  ******************************************************************************
  * @file    USART/USART_SlaveMode/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the USART_SlaveMode example.
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

This example describes an USART-SPI communication (transmit/receive) between two
boards where the USART is configured as a slave.

Boards: NUCLEO-WL55JC RevC (embeds a STM32WL55JCIx device)

   _________________________                        _________________________
  |          _______________|                      |_______________          |
  |         |      SPI1     |                      |    USART1     |         |
  |         |               |                      |               |         |
  |         | PB3 (CN9.4)|SCK_________________CK|(CN9.5)  PB5 |         |
  |         |               |                      |               |         |
  |         | PA11 (CN5.9)|MISO________________TX|(CN9.2)  PB6 |         |
  |         |               |                      |               |         |
  |         | PA12 (CN5.10)|MOSI________________RX|(CN9.1) PB7 |         |
  |         |               |                      |               |         |
  |         |_______________|                      |_______________|         |
  |      __                 |                      |                         |
  |     |__|                |                      |                         |
  |     USER                |                      |                         |
  |                      GND|______________________|GND                      |
  |                         |                      |                         |
  |_STM32WLxx_Master________|                      |_STM32WLxx_Slave_________|


Two identical boards are connected as shown on the picture above.
Board 1: SPI Master board
Board 2: USART Slave board

In a first step, after the user presses the User push-button, Master (SPI) starts
the communication by sending aTxBuffer and receiving aRxBuffer through
HAL_SPI_TransmitReceive(), at the same time Slave (USART) transmits aTxBuffer
and receives aRxBuffer through HAL_USART_TransmitReceive().
Finally, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to
check buffers correctness.

WARNING: as both boards do not behave the same way, "SLAVE_BOARD" compilation
switch is defined in Src/main.c and must be enabled at compilation time before
loading the executable in the USART slave board.
The SPI master board needs to be loaded with an executable software obtained
with SLAVE_BOARD disabled.

NUCLEO-WL55JC RevC board LEDs are used to monitor the transfer status:
- While board 1 is waiting for the user to press the User push-button, its
  LED1 is blinking rapidly (100 ms period).
- After the transmission/reception processes begin LED1 is turned off.
- When the test passes, LED1 on both boards is turned on, otherwise the
  test has failed.
- If there is an initialization or transfer error, LED3 blinks slowly (500 ms).

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32WL55JCIx Devices.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application needs to ensure that the SysTick time base is always set to
      1 millisecond to have correct HAL operation.

@par Keywords

Connectivity, UART/USART, SPI interface, full-duplex, Polling, Receiver, Transmitter, Synchronous, Slave,

@par Directory contents

  - USART/USART_SlaveMode/Inc/stm32wlxx_nucleo_conf.h     BSP configuration file
  - USART/USART_SlaveMode/Src/main.c                 Main program
  - USART/USART_SlaveMode/Src/system_stm32wlxx.c     STM32WLxx system clock configuration file
  - USART/USART_SlaveMode/Src/stm32wlxx_it.c         Interrupt handlers
  - USART/USART_SlaveMode/Src/stm32wlxx_hal_msp.c    HAL MSP module
  - USART/USART_SlaveMode/Inc/main.h                 Main program header file
  - USART/USART_SlaveMode/Inc/stm32wlxx_hal_conf.h   HAL Configuration file
  - USART/USART_SlaveMode/Inc/stm32wlxx_it.h         Interrupt handlers header file


@par Hardware and Software environment

  - This example runs on STM32WL55JCIx devices.

  - This example has been tested with two NUCLEO-WL55JC RevC boards embedding
    a STM32WL55JCIx device and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-WL55JC RevC set-up
    - Connect Master board PB3 (SCK) to Slave Board PB5 (USART CK)
    - Connect Master board PA11 (MISO) to Slave Board PB6 (USART TX)
    - Connect Master board PA12 (MOSI) to Slave Board PB7 (USART RX)
    - Connect Master board GND to Slave Board GND

@par How to use it ?

In order to make the program work, you must do the following:
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
   - Uncomment "#define SLAVE_BOARD" and load the project in Slave Board
   - Comment "#define SLAVE_BOARD" and load the project in Master Board
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
