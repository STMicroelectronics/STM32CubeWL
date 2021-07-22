/**
  @page SubGHz_Phy_PingPong_DualCore Readme file

  @verbatim
  ******************** (C) COPYRIGHT 2020 STMicroelectronics *******************
  * @file    Applications/SubGHz_Phy/SubGHz_Phy_PingPong_DualCore/readme.txt
  * @author  MCD Application Team
  * @brief   This application is a simple demo application software between 2
  *          LoRa Objects: a STM32WL Nucleo board and whatever other LoRa Radio
  *          board embedding SubGHz_Phy_PingPong application too.
  *          IPCC and MBMUX are used for multicore communication.
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

This directory contains a set of source files that implements a Dual Core (CM4 / CM0) Ping-Pong application
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

Applications, SubGHz_Phy, PingPong, DualCore

@par Directory contents


  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/ipcc.h                        This file contains all the function prototypes for
                                                                                the ipcc.c file
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/ipcc_if.h                     This file contains the interface of the ipcc driver on CM0PLUS.
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/main.h                        : Header for main.c file.
                                                                                This file contains the common defines of the application.
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/platform.h                    Header for General HW instances configuration
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/rtc.h                         This file contains all the function prototypes for
                                                                                the rtc.c file
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/stm32wlxx_hal_conf.h          HAL configuration file.
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/stm32wlxx_it.h                This file contains the headers of the interrupt handlers.
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/stm32_lpm_if.h                Header for Low Power Manager interface configuration
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/subghz.h                      This file contains all the function prototypes for
                                                                                the subghz.c file
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/sys_app.h                     Function prototypes for sys_app.c file
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/sys_conf.h                    Applicative configuration, e.g. : debug, trace, low power, sensors
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/timer_if.h                    configuration of the timer_if.c instances
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/utilities_conf.h              Header for configuration file to utilities
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Inc/utilities_def.h               Definitions for modules requiring utilities
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/MbMux/mbmux.h                          API which interfaces CM0PLUS to IPCC
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/MbMux/mbmuxif_radio.h                  API for CM0PLUS application to register and handle RADIO driver via MBMUX
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/MbMux/mbmuxif_sys.h                    API for CM0PLUS application to handle the SYSTEM MBMUX channel
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/MbMux/mbmuxif_trace.h                  API for CM0PLUS application to register and handle TRACE via MBMUX
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/MbMux/radio_mbwrapper.h                This file implements the CM0PLUS side wrapper of the Radio interface
                                                                                shared between M0 and M4.
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/SubGHz_Phy/App/app_subghz_phy.h        Header of application of the SubGHz_Phy Middleware
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/SubGHz_Phy/App/app_version.h           Definition the version of the CM0PLUS application
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/SubGHz_Phy/Target/mw_log_conf.h        Configure (enable/disable) traces for CM0
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/SubGHz_Phy/Target/radio_board_if.h     Header for Radio interface configuration
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/SubGHz_Phy/Target/radio_conf.h         Header of Radio configuration
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/SubGHz_Phy/Target/timer.h              Wrapper to timer server
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/dma.h                             This file contains all the function prototypes for
                                                                                the dma.c file
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/ipcc.h                            This file contains all the function prototypes for
                                                                                the ipcc.c file
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/ipcc_if.h                         This file contains the interface of the ipcc driver on CM4.
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/main.h                            : Header for main.c file.
                                                                                This file contains the common defines of the application.
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/platform.h                        Header for General HW instances configuration
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/stm32wlxx_hal_conf.h              HAL configuration file.
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/stm32wlxx_it.h                    This file contains the headers of the interrupt handlers.
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/stm32_lpm_if.h                    Header for Low Power Manager interface configuration
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/sys_app.h                         Function prototypes for sys_app.c file
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/sys_conf.h                        Applicative configuration, e.g. : debug, trace, low power, sensors
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/timer_if.h                        configuration of the timer_if.c instances
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/usart.h                           This file contains all the function prototypes for
                                                                                the usart.c file
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/usart_if.h                        Header for USART interface configuration
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/utilities_conf.h                  Header for configuration file to utilities
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Inc/utilities_def.h                   Definitions for modules requiring utilities
  - SubGHz_Phy_PingPong_DualCore/CM4/MbMux/mbmux.h                              API which interfaces CM4 to IPCC
  - SubGHz_Phy_PingPong_DualCore/CM4/MbMux/mbmuxif_radio.h                      API for CM4 application to register and handle RADIO driver via MBMUX
  - SubGHz_Phy_PingPong_DualCore/CM4/MbMux/mbmuxif_sys.h                        API for CM4 application to handle the SYSTEM MBMUX channel
  - SubGHz_Phy_PingPong_DualCore/CM4/MbMux/mbmuxif_trace.h                      API for CM4 application to register and handle TRACE via MBMUX
  - SubGHz_Phy_PingPong_DualCore/CM4/MbMux/radio_mbwrapper.h                    This file implements the CM4 side wrapper of the Radio interface
                                                                                shared between M0 and M4.
  - SubGHz_Phy_PingPong_DualCore/CM4/SubGHz_Phy/App/app_subghz_phy.h            Header of application of the SubGHz_Phy Middleware
  - SubGHz_Phy_PingPong_DualCore/CM4/SubGHz_Phy/App/app_version.h               Definition the version of the CM4 application
  - SubGHz_Phy_PingPong_DualCore/CM4/SubGHz_Phy/App/subghz_phy_app.h            Header of application of the SubGHz_Phy Middleware
  - SubGHz_Phy_PingPong_DualCore/Common/Board/stm32wlxx_nucleo_conf.h           STM32WLxx_Nucleo board configuration file.
  - SubGHz_Phy_PingPong_DualCore/Common/MbMux/features_info.h                   Feature list and parameters TypeDefinitions
  - SubGHz_Phy_PingPong_DualCore/Common/MbMux/mbmux_table.h                     Maps the IPCC channels to memory buffers
  - SubGHz_Phy_PingPong_DualCore/Common/MbMux/msg_id.h                          MBMUX message ID enumeration
  - SubGHz_Phy_PingPong_DualCore/Common/System/sys_debug.h                      Configuration of the debug.c instances

  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Src/ipcc.c                        This file provides code for the configuration
                                                                                of the IPCC instances.
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Src/ipcc_if.c                     Interface to IPCC: handles IRQs and abstract application from Ipcc handler and channel direction
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Src/main.c                        : Main program body
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Src/rtc.c                         This file provides code for the configuration
                                                                                of the RTC instances.
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Src/stm32wlxx_hal_msp.c           This file provides code for the MSP Initialization
                                                                                and de-Initialization codes.
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Src/stm32wlxx_it.c                Interrupt Service Routines.
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Src/stm32_lpm_if.c                Low layer function to enter/exit low power modes (stop, sleep)
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Src/subghz.c                      This file provides code for the configuration
                                                                                of the SUBGHZ instances.
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Src/sys_app.c                     Initializes HW and SW system entities (not related to the radio)
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/Core/Src/timer_if.c                    Configure RTC Alarm, Tick and Calendar manager
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/MbMux/features_info.c                  CM0PLUS supported features list
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/MbMux/mbmux.c                          Interface CPU2 to IPCC: multiplexer to map features to IPCC channels
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/MbMux/mbmuxif_radio.c                  allows CM0PLUS application to register and handle RADIO driver via MBMUX
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/MbMux/mbmuxif_sys.c                    allows CM0 application to handle the SYSTEM MBMUX channel
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/MbMux/mbmuxif_trace.c                  Interface layer CM0PLUS Trace to MBMUX (Mailbox Multiplexer)
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/MbMux/radio_mbwrapper.c                This file implements the CM0 side wrapper of the Radio interface
                                                                                shared between M0 and M4.
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/SubGHz_Phy/App/app_subghz_phy.c        Application of the SubGHz_Phy Middleware
  - SubGHz_Phy_PingPong_DualCore/CM0PLUS/SubGHz_Phy/Target/radio_board_if.c     This file provides an interface layer between MW and Radio Board
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Src/dma.c                             This file provides code for the configuration
                                                                                of all the requested memory to memory DMA transfers.
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Src/ipcc.c                            This file provides code for the configuration
                                                                                of the IPCC instances.
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Src/ipcc_if.c                         Interface to IPCC: handles IRQs and abstract application from Ipcc handler and channel direction
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Src/main.c                            : Main program body
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Src/stm32wlxx_hal_msp.c               This file provides code for the MSP Initialization
                                                                                and de-Initialization codes.
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Src/stm32wlxx_it.c                    Interrupt Service Routines.
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Src/stm32_lpm_if.c                    Low layer function to enter/exit low power modes (stop, sleep)
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Src/sys_app.c                         Initializes HW and SW system entities (not related to the radio)
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Src/timer_if.c                        Configure RTC Alarm, Tick and Calendar manager
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Src/usart.c                           This file provides code for the configuration
                                                                                of the USART instances.
  - SubGHz_Phy_PingPong_DualCore/CM4/Core/Src/usart_if.c                        Configuration of UART driver interface for hyperterminal communication
  - SubGHz_Phy_PingPong_DualCore/CM4/MbMux/mbmux.c                              CM4 side multiplexer to map features to IPCC channels
  - SubGHz_Phy_PingPong_DualCore/CM4/MbMux/mbmuxif_radio.c                      allows CM4 application to register and handle RADIO driver via MBMUX
  - SubGHz_Phy_PingPong_DualCore/CM4/MbMux/mbmuxif_sys.c                        allows CM4 application to handle the SYSTEM MBMUX channel.
  - SubGHz_Phy_PingPong_DualCore/CM4/MbMux/mbmuxif_trace.c                      allows CM4 application to receive by CM0 TRACE via MBMUX
  - SubGHz_Phy_PingPong_DualCore/CM4/MbMux/radio_mbwrapper.c                    This file implements the CM4 side wrapper of the Radio interface
                                                                                shared between M0 and M4.
  - SubGHz_Phy_PingPong_DualCore/CM4/SubGHz_Phy/App/app_subghz_phy.c            Application of the SubGHz_Phy Middleware
  - SubGHz_Phy_PingPong_DualCore/CM4/SubGHz_Phy/App/subghz_phy_app.c            Application of the SubGHz_Phy Middleware
  - SubGHz_Phy_PingPong_DualCore/Common/System/system_stm32wlxx.c               CMSIS Cortex Device Peripheral Access Layer System Source File
  - SubGHz_Phy_PingPong_DualCore/Common/System/sys_debug.c                      Enables 4 debug pins for internal signals RealTime debugging
  - SubGHz_Phy_PingPong_DualCore/STM32CubeIDE/CM0PLUS/Application/User/Core/syscalls.cSTM32CubeIDE Minimal System calls file
  - SubGHz_Phy_PingPong_DualCore/STM32CubeIDE/CM0PLUS/Application/User/Core/sysmem.cSTM32CubeIDE System Memory calls file
  - SubGHz_Phy_PingPong_DualCore/STM32CubeIDE/CM4/Application/User/Core/syscalls.cSTM32CubeIDE Minimal System calls file
  - SubGHz_Phy_PingPong_DualCore/STM32CubeIDE/CM4/Application/User/Core/sysmem.cSTM32CubeIDE System Memory calls file


@par Hardware and Software environment

  - This example runs on the STM32WLxx Nucleo board.

  - STM32WLxx Nucleo board Set-up
    - Connect the Nucleo board to your PC with a USB cable type A to micro-B
      to ST-LINK connector.
    - Please ensure that the ST-LINK connector jumpers are fitted.

  - Configure the software via the configuration files:
    - CM0PLUS (Mw and radio drivers config)
        - sys_conf.h, radio_conf.h, mw_log_conf.h, main.h, etc
    - CM4 (Subghz application)
        - sys_conf.h, nucleo_conf.h, main.h, etc

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
  - Rebuild all CM4 files and load your image into CM4 target memory
  - Rebuild all CM0PLUS files, set PWR_CR4_C2BOOT flag via CM4 and load your image into CM0PLUS target memory
  - Do the same for second board
  - Reset the two boards
  - Run the example on two boards
  - Open two Terminals, each connected to their respective PingPong Device
  - UART Config = 115200, 8b, 1 stopbit, no parity, no flow control

@par How to debug ?
  - Open your preferred toolchain and select CM4 project
  - define the flag DEBUGGER_ENABLED to 1 in CM4\Core\Inc\sys_conf.h
  - define the flag LOW_POWER_DISABLE to 1 at least on one MCU (CM4 or CM0PUS)
  - compile and download
  - put a breakpoint in sys_app.c one line after HAL_PWREx_ReleaseCore(PWR_CORE_CPU2);
  - run CM4 (the above functions sets PWR_CR4_C2BOOT flag)
  - open a second instance of your preferred toolchain and select CM0PLUS project
  - to debug CM0PLUS as well, define the flag DEBUGGER_ENABLED to 1 in CM0PLUS\Core\Inc\sys_conf.h
  - define the flag LOW_POWER_DISABLE to 1 at least on one MCU (CM4 or CM0PUS)
  - compile and download using "Download active application"
  - once CM0PLUS code is downloaded reset (system reset) the CM4
  - run CM4 to the breakpoint again
  - attach to running target CM0PLUS
         (CM0PLUS is in a while loop waiting CM4 to give green light to go further)
         (this is done to prevent CM0PLUS to execute too much code before attaching)
  - on CM4 execute MBMUXIF_SetCpusSynchroFlag(1); this will allow CM0PLUS to exit the while loop

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */