/**
  @page Sigfox_PushButton Readme file

  @verbatim
  ******************** (C) COPYRIGHT 2020 STMicroelectronics *******************
  * @file    Applications/Sigfox/Sigfox_PushButton/readme.txt
  * @author  MCD Application Team
  * @brief   This application is a simple demo of a Sigfox Modem sending temperature
  *          and Battery Level to a Sigfox Network.
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

This directory contains a set of source files that implements an example of a
Sigfox object sending temperature and Battery Level to a Sigfox Network.

This application is targeting the NUCLEO-WL55JC1 (HIGH-BAND) embedding the STM32WLxx.
  ******************************************************************************

@par Keywords

Applications, SubGHz_Phy, Sigfox, PushButton, SingleCore

@par Directory contents


  - Sigfox_PushButton/Core/Inc/adc.h                                            This file contains all the function prototypes for
                                                                                the adc.c file
  - Sigfox_PushButton/Core/Inc/adc_if.h                                         Header for ADC interface configuration
  - Sigfox_PushButton/Core/Inc/dma.h                                            This file contains all the function prototypes for
                                                                                the dma.c file
  - Sigfox_PushButton/Core/Inc/flash_if.h                                       This file contains definitions for FLASH Interface functionalities.
  - Sigfox_PushButton/Core/Inc/lptim.h                                          This file contains all the function prototypes for
                                                                                the lptim.c file
  - Sigfox_PushButton/Core/Inc/main.h                                           : Header for main.c file.
                                                                                This file contains the common defines of the application.
  - Sigfox_PushButton/Core/Inc/mn_lptim_if.h                                    Header for between Sigfox monarch and lptim
  - Sigfox_PushButton/Core/Inc/platform.h                                       Header for General HW instances configuration
  - Sigfox_PushButton/Core/Inc/rtc.h                                            This file contains all the function prototypes for
                                                                                the rtc.c file
  - Sigfox_PushButton/Core/Inc/stm32wlxx_hal_conf.h                             HAL configuration file.
  - Sigfox_PushButton/Core/Inc/stm32wlxx_it.h                                   This file contains the headers of the interrupt handlers.
  - Sigfox_PushButton/Core/Inc/stm32wlxx_nucleo_conf.h                          STM32WLxx_Nucleo board configuration file.
  - Sigfox_PushButton/Core/Inc/stm32_lpm_if.h                                   Header for Low Power Manager interface configuration
  - Sigfox_PushButton/Core/Inc/subghz.h                                         This file contains all the function prototypes for
                                                                                the subghz.c file
  - Sigfox_PushButton/Core/Inc/sys_app.h                                        Function prototypes for sys_app.c file
  - Sigfox_PushButton/Core/Inc/sys_conf.h                                       Applicative configuration, e.g. : debug, trace, low power, sensors
  - Sigfox_PushButton/Core/Inc/sys_debug.h                                      Configuration of the debug.c instances
  - Sigfox_PushButton/Core/Inc/sys_sensors.h                                    Header for sensors application
  - Sigfox_PushButton/Core/Inc/timer_if.h                                       configuration of the timer_if.c instances
  - Sigfox_PushButton/Core/Inc/usart.h                                          This file contains all the function prototypes for
                                                                                the usart.c file
  - Sigfox_PushButton/Core/Inc/usart_if.h                                       Header for USART interface configuration
  - Sigfox_PushButton/Core/Inc/utilities_conf.h                                 Header for configuration file to utilities
  - Sigfox_PushButton/Core/Inc/utilities_def.h                                  Definitions for modules requiring utilities
  - Sigfox_PushButton/Sigfox/App/app_sigfox.h                                   Header of application of the Sigfox Middleware
  - Sigfox_PushButton/Sigfox/App/ee.h                                           Header of the EEPROM emulator
  - Sigfox_PushButton/Sigfox/App/sgfx_app.h                                     provides code for the application of the SIGFOX Middleware
  - Sigfox_PushButton/Sigfox/App/sgfx_app_version.h                             Definition the version of the application
  - Sigfox_PushButton/Sigfox/App/sgfx_cstimer.h                                 Interface for  cstimer.c driver
  - Sigfox_PushButton/Sigfox/App/sgfx_eeprom_if.h                               eeprom interface to the upper module.
  - Sigfox_PushButton/Sigfox/App/sigfox_data.h                                  provides encrypted data
  - Sigfox_PushButton/Sigfox/Target/mcu_api.h                                   defines the interface to mcu_api.c
  - Sigfox_PushButton/Sigfox/Target/mn_api.h                                    monarch library interface
  - Sigfox_PushButton/Sigfox/Target/mw_log_conf.h                               Configure (enable/disable) traces for CM0
  - Sigfox_PushButton/Sigfox/Target/radio_board_if.h                            Header for Radio interface configuration
  - Sigfox_PushButton/Sigfox/Target/radio_conf.h                                Header of Radio configuration
  - Sigfox_PushButton/Sigfox/Target/rf_api.h                                    interface to rf_api.c
  - Sigfox_PushButton/Sigfox/Target/se_nvm.h                                    header to emulated SE nvm data
  - Sigfox_PushButton/Sigfox/Target/sgfx_credentials.h                          interface to key manager and encryption algorithm
  - Sigfox_PushButton/Sigfox/Target/timer.h                                     Wrapper to timer server

  - Sigfox_PushButton/Core/Src/adc.c                                            This file provides code for the configuration
                                                                                of the ADC instances.
  - Sigfox_PushButton/Core/Src/adc_if.c                                         Read status related to the chip (battery level, VREF, chip temperature)
  - Sigfox_PushButton/Core/Src/dma.c                                            This file provides code for the configuration
                                                                                of all the requested memory to memory DMA transfers.
  - Sigfox_PushButton/Core/Src/flash_if.c                                       This file provides set of firmware functions to manage Flash
                                                                                Interface functionalities.
  - Sigfox_PushButton/Core/Src/lptim.c                                          This file provides code for the configuration
                                                                                of the LPTIM instances.
  - Sigfox_PushButton/Core/Src/main.c                                           : Main program body
  - Sigfox_PushButton/Core/Src/mn_lptim_if.c                                    Interface between Sigfox monarch and lptim
  - Sigfox_PushButton/Core/Src/rtc.c                                            This file provides code for the configuration
                                                                                of the RTC instances.
  - Sigfox_PushButton/Core/Src/stm32wlxx_hal_msp.c                              This file provides code for the MSP Initialization
                                                                                and de-Initialization codes.
  - Sigfox_PushButton/Core/Src/stm32wlxx_it.c                                   Interrupt Service Routines.
  - Sigfox_PushButton/Core/Src/stm32_lpm_if.c                                   Low layer function to enter/exit low power modes (stop, sleep)
  - Sigfox_PushButton/Core/Src/subghz.c                                         This file provides code for the configuration
                                                                                of the SUBGHZ instances.
  - Sigfox_PushButton/Core/Src/system_stm32wlxx.c                               CMSIS Cortex Device Peripheral Access Layer System Source File
  - Sigfox_PushButton/Core/Src/sys_app.c                                        Initializes HW and SW system entities (not related to the radio)
  - Sigfox_PushButton/Core/Src/sys_debug.c                                      Enables 4 debug pins for internal signals RealTime debugging
  - Sigfox_PushButton/Core/Src/sys_sensors.c                                    Manages the sensors on the application
  - Sigfox_PushButton/Core/Src/timer_if.c                                       Configure RTC Alarm, Tick and Calendar manager
  - Sigfox_PushButton/Core/Src/usart.c                                          This file provides code for the configuration
                                                                                of the USART instances.
  - Sigfox_PushButton/Core/Src/usart_if.c                                       Configuration of UART driver interface for hyperterminal communication
  - Sigfox_PushButton/Sigfox/App/app_sigfox.c                                   Application of the Sigfox Middleware
  - Sigfox_PushButton/Sigfox/App/ee.c                                           Implementation of the EEPROM emulator
  - Sigfox_PushButton/Sigfox/App/sgfx_app.c                                     provides code for the application of the sigfox Middleware
  - Sigfox_PushButton/Sigfox/App/sgfx_cstimer.c                                 manages carrier sense timer.
  - Sigfox_PushButton/Sigfox/App/sgfx_eeprom_if.c                               eeprom interface to the sigfox component
  - Sigfox_PushButton/Sigfox/Target/mcu_api.c                                   mcu library interface
  - Sigfox_PushButton/Sigfox/Target/mn_api.c                                    monarch library interface implementation
  - Sigfox_PushButton/Sigfox/Target/radio_board_if.c                            This file provides an interface layer between MW and Radio Board
  - Sigfox_PushButton/Sigfox/Target/rf_api.c                                    rf library interface
  - Sigfox_PushButton/Sigfox/Target/se_nvm.c                                    manages SE nvm data
  - Sigfox_PushButton/Sigfox/Target/sgfx_credentials.c                          manages keys and encryption algorithm
  - Sigfox_PushButton/STM32CubeIDE/Application/User/Core/syscalls.c             STM32CubeIDE Minimal System calls file
  - Sigfox_PushButton/STM32CubeIDE/Application/User/Core/sysmem.c               STM32CubeIDE System Memory calls file


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
             |    Sigfox Object       |  |    |  |   Sigfox  Network      |
             |                        |  |    |  |                        |
    Button-->|                        |--|    |--|                        |-->Web Server
             |                        |          |                        |
             --------------------------          --------------------------

@par How to use it ?
In order to make the program work, you must do the following :
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Run the example
  - Open a Terminal, connected the Sigfox Object
  - UART Config = 9600, 8b, 1 stopbit, no parity, no flow control
  - Terminal Config: Select 'CR+LF' for Transmit New-Line and switch 'Local echo' on
  - Push Button 1 (it will send a sigfox message, Blue LED blinking)

@par How to debug ?
  - make sure the flag DEBUGGER_ENABLED to 1 in sys_conf.h
  - simpler to define the flag LOW_POWER_DISABLE to 1 as well
  - compile, download and attach

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */