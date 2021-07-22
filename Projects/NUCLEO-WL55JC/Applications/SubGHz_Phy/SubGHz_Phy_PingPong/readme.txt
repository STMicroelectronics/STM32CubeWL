/**
  @page SubGHz_Phy_PingPong Readme file

  @verbatim
  ******************** (C) COPYRIGHT 2020 STMicroelectronics *******************
  * @file    Applications/SubGHz_Phy/SubGHz_Phy_PingPong/readme.txt
  * @author  MCD Application Team
  * @brief   This application is a simple demo application software between 2
  *          LoRa Objects: a STM32WL Nucleo board and whatever other LoRa Radio
  *          board embedding SubGHz_Phy_PingPong application too
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  @endverbatim

@par Description

This directory contains a set of source files that implements a Ping-Pong application
between two PingPong Devices.

The PingPong Device can be
   - a STM32WL Nucleo board
   - a LoRa Radio expansion board or a B-L072Z-LRWAN1
The applications aims to show a simple RX/TX RF link between the two PingPong devices,
one will be called Ping the other will be called Pong.
By default, each PingPong Device starts as a master and will transmit a "Ping" message, and then wait for an answer.
At start-up, each PingPong Device has its two LEDs blinking.
When boards will synchronize (Tx window of one board aligned with Rx window of the other board)
the Ping Device (board receiving "Ping" msg) will blink green LED and the Pong Device (board receiving "Pong" msg) will blink red LED.
The first PingPong Device receiving a "Ping" message will become a slave and answers the master with a "Pong" message.

Logs via hyperterminal complement LEDs indicators.

  ******************************************************************************

@par Keywords

Applications, SubGHz_Phy, PingPong, SingleCore

@par Directory contents


  - SubGHz_Phy_PingPong/Core/Inc/dma.h                                          This file contains all the function prototypes for
                                                                                the dma.c file
  - SubGHz_Phy_PingPong/Core/Inc/main.h                                         : Header for main.c file.
                                                                                This file contains the common defines of the application.
  - SubGHz_Phy_PingPong/Core/Inc/platform.h                                     Header for General HW instances configuration
  - SubGHz_Phy_PingPong/Core/Inc/rtc.h                                          This file contains all the function prototypes for
                                                                                the rtc.c file
  - SubGHz_Phy_PingPong/Core/Inc/stm32wlxx_hal_conf.h                           HAL configuration file.
  - SubGHz_Phy_PingPong/Core/Inc/stm32wlxx_it.h                                 This file contains the headers of the interrupt handlers.
  - SubGHz_Phy_PingPong/Core/Inc/stm32wlxx_nucleo_conf.h                        STM32WLxx_Nucleo board configuration file.
  - SubGHz_Phy_PingPong/Core/Inc/stm32_lpm_if.h                                 Header for Low Power Manager interface configuration
  - SubGHz_Phy_PingPong/Core/Inc/subghz.h                                       This file contains all the function prototypes for
                                                                                the subghz.c file
  - SubGHz_Phy_PingPong/Core/Inc/sys_app.h                                      Function prototypes for sys_app.c file
  - SubGHz_Phy_PingPong/Core/Inc/sys_conf.h                                     Applicative configuration, e.g. : debug, trace, low power, sensors
  - SubGHz_Phy_PingPong/Core/Inc/sys_debug.h                                    Configuration of the debug.c instances
  - SubGHz_Phy_PingPong/Core/Inc/timer_if.h                                     configuration of the timer_if.c instances
  - SubGHz_Phy_PingPong/Core/Inc/usart.h                                        This file contains all the function prototypes for
                                                                                the usart.c file
  - SubGHz_Phy_PingPong/Core/Inc/usart_if.h                                     Header for USART interface configuration
  - SubGHz_Phy_PingPong/Core/Inc/utilities_conf.h                               Header for configuration file to utilities
  - SubGHz_Phy_PingPong/Core/Inc/utilities_def.h                                Definitions for modules requiring utilities
  - SubGHz_Phy_PingPong/SubGHz_Phy/App/app_subghz_phy.h                         Header of application of the SubGHz_Phy Middleware
  - SubGHz_Phy_PingPong/SubGHz_Phy/App/app_version.h                            Definition the version of the application
  - SubGHz_Phy_PingPong/SubGHz_Phy/App/subghz_phy_app.h                         Header of application of the SubGHz_Phy Middleware
  - SubGHz_Phy_PingPong/SubGHz_Phy/Target/mw_log_conf.h                         Configure (enable/disable) traces for CM0
  - SubGHz_Phy_PingPong/SubGHz_Phy/Target/radio_board_if.h                      Header for Radio interface configuration
  - SubGHz_Phy_PingPong/SubGHz_Phy/Target/radio_conf.h                          Header of Radio configuration
  - SubGHz_Phy_PingPong/SubGHz_Phy/Target/timer.h                               Wrapper to timer server

  - SubGHz_Phy_PingPong/Core/Src/dma.c                                          This file provides code for the configuration
                                                                                of all the requested memory to memory DMA transfers.
  - SubGHz_Phy_PingPong/Core/Src/main.c                                         : Main program body
  - SubGHz_Phy_PingPong/Core/Src/rtc.c                                          This file provides code for the configuration
                                                                                of the RTC instances.
  - SubGHz_Phy_PingPong/Core/Src/stm32wlxx_hal_msp.c                            This file provides code for the MSP Initialization
                                                                                and de-Initialization codes.
  - SubGHz_Phy_PingPong/Core/Src/stm32wlxx_it.c                                 Interrupt Service Routines.
  - SubGHz_Phy_PingPong/Core/Src/stm32_lpm_if.c                                 Low layer function to enter/exit low power modes (stop, sleep)
  - SubGHz_Phy_PingPong/Core/Src/subghz.c                                       This file provides code for the configuration
                                                                                of the SUBGHZ instances.
  - SubGHz_Phy_PingPong/Core/Src/system_stm32wlxx.c                             CMSIS Cortex Device Peripheral Access Layer System Source File
  - SubGHz_Phy_PingPong/Core/Src/sys_app.c                                      Initializes HW and SW system entities (not related to the radio)
  - SubGHz_Phy_PingPong/Core/Src/sys_debug.c                                    Enables 4 debug pins for internal signals RealTime debugging
  - SubGHz_Phy_PingPong/Core/Src/timer_if.c                                     Configure RTC Alarm, Tick and Calendar manager
  - SubGHz_Phy_PingPong/Core/Src/usart.c                                        This file provides code for the configuration
                                                                                of the USART instances.
  - SubGHz_Phy_PingPong/Core/Src/usart_if.c                                     Configuration of UART driver interface for hyperterminal communication
  - SubGHz_Phy_PingPong/STM32CubeIDE/Application/User/Core/syscalls.c           STM32CubeIDE Minimal System calls file
  - SubGHz_Phy_PingPong/STM32CubeIDE/Application/User/Core/sysmem.c             STM32CubeIDE System Memory calls file
  - SubGHz_Phy_PingPong/SubGHz_Phy/App/app_subghz_phy.c                         Application of the SubGHz_Phy Middleware
  - SubGHz_Phy_PingPong/SubGHz_Phy/App/subghz_phy_app.c                         Application of the SubGHz_Phy Middleware
  - SubGHz_Phy_PingPong/SubGHz_Phy/Target/radio_board_if.c                      This file provides an interface layer between MW and Radio Board


@par Hardware and Software environment

  - This example runs on the STM32WLxx Nucleo board.

  - STM32WLxx Nucleo board Set-up
    - Connect the Nucleo board to your PC with a USB cable type A to micro-B
      to ST-LINK connector.
    - Please ensure that the ST-LINK connector jumpers are fitted.

  - Configure the software via the configuration files:
    - sys_conf.h, radio_conf.h, mw_log_conf.h, main.h, etc

  -Set Up:

             --------------------------  V    V  --------------------------
             |   PingPong Device      |  |    |  |   PingPong Device      |
             |                        |  |    |  |                        |
   ComPort<--|                        |--|    |--|                        |-->ComPort
             |                        |          |                        |
             --------------------------          --------------------------

@par How to use it ?
In order to make the program work, you must do the following :
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Do the same for second board
  - Reset the two boards
  - Run the example on two boards
  - Open two Terminals, each connected to their respective PingPong Device
  - UART Config = 115200, 8b, 1 stopbit, no parity, no flow control

@par How to debug ?
  - make sure the flag DEBUGGER_ENABLED to 1 in sys_conf.h
  - simpler to define the flag LOW_POWER_DISABLE to 1 as well
  - compile, download and attach

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
