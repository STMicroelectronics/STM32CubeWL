/**
  @page USART_HardwareFlowControl_Init USART example (IT Mode with HW Flow Control)
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/USART/USART_HardwareFlowControl_Init/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the USART_HardwareFlowControl_Init example.
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

Configuration of GPIO and  peripheral 
to receive characters asynchronously from an HyperTerminal (PC) in Interrupt mode 
with the Hardware Flow Control feature enabled. This example is based on STM32WLxx 
USART LL API. The peripheral initialization 
uses LL unitary service functions for optimization purposes (performance and size).

USART1 Peripheral is configured in asynchronous mode (115200 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
HW flow control is used.
GPIO associated to User push-button (B1) is linked with EXTI.

Example execution:
After startup from reset and system configuration, LED2 is blinking quickly.
On press on User push-button (B1), USART TX Empty interrupt is enabled.
First character of buffer to be transmitted is written into USART Transmit Data Register (TDR)
in order to initialise transfer procedure.
When character is sent from TDR, a TXE interrupt occurs.
USART1 IRQ Handler routine is sending next character on USART Tx line.
IT will be raised until last byte is to be transmitted : Then, Transmit Complete (TC) interrupt is enabled 
instead of TX Empty (TXE).
When character is received on USART Rx line, a RXNE interrupt occurs.
USART1 IRQ Handler routine is then checking received character value.
On a specific value ('S' or 's'), LED2 is turned On.
Received character is echoed on Tx line.
In case of errors, LED2 is blinking.

@par Keywords

Connectivity, UART/USART, Asynchronous, RS-232, baud rate, Interrupt, HyperTerminal, 
hardware flow control, CTS/RTS, Transmitter, Receiver

@par Directory contents 

  - USART/USART_HardwareFlowControl_Init/Inc/stm32wlxx_it.h          Interrupt handlers header file
  - USART/USART_HardwareFlowControl_Init/Inc/main.h                  Header for main.c module
  - USART/USART_HardwareFlowControl_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - USART/USART_HardwareFlowControl_Init/Src/stm32wlxx_it.c          Interrupt handlers
  - USART/USART_HardwareFlowControl_Init/Src/main.c                  Main program
  - USART/USART_HardwareFlowControl_Init/Src/system_stm32wlxx.c      STM32WLxx system source file

@par Hardware and Software environment

  - This example runs on STM32WLxx devices.

  - This example has been tested with NUCLEO-WL55JC RevC board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WL55JC RevC Set-up
    GPIOs connected to USART1 TX/RX/CTS/RTS signals should be wired
    to corresponding pins on PC UART (could be done through a USB to UART adapter).
    - Connect STM32 MCU board USART1 TX pin (PB.06 connected to pin 35 of connector CN10)
      to PC COM port RX signal
    - Connect STM32 MCU board USART1 RX pin (PB.07 connected to pin 37 of connector CN10)
      to PC COM port TX signal
    - Connect STM32 MCU board USART1 CTS pin (PA.11 connected to pin 5 of Morpho connector CN10)
      to PC COM port RTS signal
    - Connect STM32 MCU board USART1 RTS pin (PA.12 connected to pin 3 of Morpho connector CN10)
      to PC COM port CTS signal
    - Connect STM32 MCU board GND to PC COM port GND signal

  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration 
    (115200 bauds, 8 bits data, 1 stop bit, no parity, HW flow control enabled).

  - Launch the program. Enter characters on PC communication SW side.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
