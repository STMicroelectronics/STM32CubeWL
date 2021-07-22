/**
  @page LoRaWAN_End_Node Readme file

  @verbatim
  ******************** (C) COPYRIGHT 2020 STMicroelectronics *******************
  * @file    Applications/LoRaWAN/LoRaWAN_End_Node/readme.txt
  * @author  MCD Application Team
  * @brief   This application is a simple demo application software of a LoRa
  *          modem connecting to Network server. Data sent can be checked on
  *          Network server for eg Loriot. Traces are displayed over UART
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

This directory contains a set of source files that implements a LoRa application
device sending sensors data to LoRa Network server.
Data are sent periodically on timer event or on "user button 1" depending on the configuration.

This application is targeting the STM32WLxx Nucleo board embedding the STM32WLxx.
  ******************************************************************************

@par Keywords

Applications, SubGHz_Phy, LoRaWAN, End_Node, SingleCore

@par Directory contents


  - LoRaWAN_End_Node/Core/Inc/adc.h                                             This file contains all the function prototypes for
                                                                                the adc.c file
  - LoRaWAN_End_Node/Core/Inc/adc_if.h                                          Header for ADC interface configuration
  - LoRaWAN_End_Node/Core/Inc/dma.h                                             This file contains all the function prototypes for
                                                                                the dma.c file
  - LoRaWAN_End_Node/Core/Inc/main.h                                            : Header for main.c file.
                                                                                This file contains the common defines of the application.
  - LoRaWAN_End_Node/Core/Inc/platform.h                                        Header for General HW instances configuration
  - LoRaWAN_End_Node/Core/Inc/rtc.h                                             This file contains all the function prototypes for
                                                                                the rtc.c file
  - LoRaWAN_End_Node/Core/Inc/stm32wlxx_hal_conf.h                              HAL configuration file.
  - LoRaWAN_End_Node/Core/Inc/stm32wlxx_it.h                                    This file contains the headers of the interrupt handlers.
  - LoRaWAN_End_Node/Core/Inc/stm32wlxx_nucleo_conf.h                           STM32WLxx_Nucleo board configuration file.
  - LoRaWAN_End_Node/Core/Inc/stm32_lpm_if.h                                    Header for Low Power Manager interface configuration
  - LoRaWAN_End_Node/Core/Inc/subghz.h                                          This file contains all the function prototypes for
                                                                                the subghz.c file
  - LoRaWAN_End_Node/Core/Inc/sys_app.h                                         Function prototypes for sys_app.c file
  - LoRaWAN_End_Node/Core/Inc/sys_conf.h                                        Applicative configuration, e.g. : debug, trace, low power, sensors
  - LoRaWAN_End_Node/Core/Inc/sys_debug.h                                       Configuration of the debug.c instances
  - LoRaWAN_End_Node/Core/Inc/sys_sensors.h                                     Header for sensors application
  - LoRaWAN_End_Node/Core/Inc/timer_if.h                                        configuration of the timer_if.c instances
  - LoRaWAN_End_Node/Core/Inc/usart.h                                           This file contains all the function prototypes for
                                                                                the usart.c file
  - LoRaWAN_End_Node/Core/Inc/usart_if.h                                        Header for USART interface configuration
  - LoRaWAN_End_Node/Core/Inc/utilities_conf.h                                  Header for configuration file to utilities
  - LoRaWAN_End_Node/Core/Inc/utilities_def.h                                   Definitions for modules requiring utilities
  - LoRaWAN_End_Node/LoRaWAN/App/app_lorawan.h                                  Header of application of the LRWAN Middleware
  - LoRaWAN_End_Node/LoRaWAN/App/CayenneLpp.h                                   Implements the Cayenne Low Power Protocol
  - LoRaWAN_End_Node/LoRaWAN/App/Commissioning.h                                End-device commissioning parameters
  - LoRaWAN_End_Node/LoRaWAN/App/lora_app.h                                     Header of application of the LRWAN Middleware
  - LoRaWAN_End_Node/LoRaWAN/App/lora_app_version.h                             Definition the version of the application
  - LoRaWAN_End_Node/LoRaWAN/App/lora_info.h                                    To give info to the application about LoRaWAN configuration
  - LoRaWAN_End_Node/LoRaWAN/App/se-identity.h                                  Secure Element identity and keys
  - LoRaWAN_End_Node/LoRaWAN/Target/lorawan_conf.h                              Header for LoRaWAN middleware instances
  - LoRaWAN_End_Node/LoRaWAN/Target/mw_log_conf.h                               Configure (enable/disable) traces for CM0
  - LoRaWAN_End_Node/LoRaWAN/Target/radio_board_if.h                            Header for Radio interface configuration
  - LoRaWAN_End_Node/LoRaWAN/Target/radio_conf.h                                Header of Radio configuration
  - LoRaWAN_End_Node/LoRaWAN/Target/systime.h                                   Map middleware systime
  - LoRaWAN_End_Node/LoRaWAN/Target/timer.h                                     Wrapper to timer server

  - LoRaWAN_End_Node/Core/Src/adc.c                                             This file provides code for the configuration
                                                                                of the ADC instances.
  - LoRaWAN_End_Node/Core/Src/adc_if.c                                          Read status related to the chip (battery level, VREF, chip temperature)
  - LoRaWAN_End_Node/Core/Src/dma.c                                             This file provides code for the configuration
                                                                                of all the requested memory to memory DMA transfers.
  - LoRaWAN_End_Node/Core/Src/main.c                                            : Main program body
  - LoRaWAN_End_Node/Core/Src/rtc.c                                             This file provides code for the configuration
                                                                                of the RTC instances.
  - LoRaWAN_End_Node/Core/Src/stm32wlxx_hal_msp.c                               This file provides code for the MSP Initialization
                                                                                and de-Initialization codes.
  - LoRaWAN_End_Node/Core/Src/stm32wlxx_it.c                                    Interrupt Service Routines.
  - LoRaWAN_End_Node/Core/Src/stm32_lpm_if.c                                    Low layer function to enter/exit low power modes (stop, sleep)
  - LoRaWAN_End_Node/Core/Src/subghz.c                                          This file provides code for the configuration
                                                                                of the SUBGHZ instances.
  - LoRaWAN_End_Node/Core/Src/system_stm32wlxx.c                                CMSIS Cortex Device Peripheral Access Layer System Source File
  - LoRaWAN_End_Node/Core/Src/sys_app.c                                         Initializes HW and SW system entities (not related to the radio)
  - LoRaWAN_End_Node/Core/Src/sys_debug.c                                       Enables 4 debug pins for internal signals RealTime debugging
  - LoRaWAN_End_Node/Core/Src/sys_sensors.c                                     Manages the sensors on the application
  - LoRaWAN_End_Node/Core/Src/timer_if.c                                        Configure RTC Alarm, Tick and Calendar manager
  - LoRaWAN_End_Node/Core/Src/usart.c                                           This file provides code for the configuration
                                                                                of the USART instances.
  - LoRaWAN_End_Node/Core/Src/usart_if.c                                        Configuration of UART driver interface for hyperterminal communication
  - LoRaWAN_End_Node/LoRaWAN/App/app_lorawan.c                                  Application of the LRWAN Middleware
  - LoRaWAN_End_Node/LoRaWAN/App/CayenneLpp.c                                   Implements the Cayenne Low Power Protocol
  - LoRaWAN_End_Node/LoRaWAN/App/lora_app.c                                     Application of the LRWAN Middleware
  - LoRaWAN_End_Node/LoRaWAN/App/lora_info.c                                    To give info to the application about LoRaWAN configuration
  - LoRaWAN_End_Node/LoRaWAN/Target/radio_board_if.c                            This file provides an interface layer between MW and Radio Board
  - LoRaWAN_End_Node/STM32CubeIDE/Application/User/Core/syscalls.c              STM32CubeIDE Minimal System calls file
  - LoRaWAN_End_Node/STM32CubeIDE/Application/User/Core/sysmem.c                STM32CubeIDE System Memory calls file


@par Hardware and Software environment

  - This example runs on the STM32WLxx Nucleo board.

  - STM32WLxx Nucleo board Set-up
    - Connect the Nucleo board to your PC with a USB cable type A to micro-B
      to ST-LINK connector.
    - Please ensure that the ST-LINK connector jumpers are fitted.

  - Configure the software via the configuration files:
    - sys_conf.h, radio_conf.h, lorawan_conf.h, lora_app.c, lora_app.h, Commissioning.h, se-identity.h, mw_log_conf.h, main.h, etc
    - Careful:
        - the region and class chosen on LoRaWAN/App/lora_app.h shall be compatible with LoRaWAN/Target/lorawan_conf.h list

  -Set Up:

             --------------------------  V    V  --------------------------
             |      LoRa Object       |  |    |  |      LoRa Network      |
             |                        |  |    |  |                        |
   ComPort<--|                        |--|    |--|                        |-->Web Server
             |                        |          |                        |
             --------------------------          --------------------------

@par How to use it ?
In order to make the program work, you must do the following :
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Run the example
  - Open a Terminal, connected the LoRa Object
  - UART Config = 115200, 8b, 1 stopbit, no parity, no flow control

@par How to debug ?
  - make sure the flag DEBUGGER_ENABLED to 1 in sys_conf.h
  - simpler to define the flag LOW_POWER_DISABLE to 1 as well
  - compile, download and attach

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */