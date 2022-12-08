/**
  @page LoRaWAN_AT_Slave Readme file

  @verbatim
  ******************************************************************************
  * @file    Applications/LoRaWAN/LoRaWAN_AT_Slave/readme.txt
  * @author  MCD Application Team
  * @brief   This application is a simple demo application software of a LoRa
  *          modem controlled though AT command interface over UART by an
  *          external host
  ******************************************************************************
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Description

This directory contains a set of source files that implements a LoRa application
modem that is controlled though AT command interface over UART by an external host,
like a computer executing a terminal.

This application is targeting the B-WL5M-SUBG1 Connectivity Expansion Board embedding the STM32WL5Mxx.
  ******************************************************************************

@par Keywords

Applications, SubGHz_Phy, LoRaWAN, AT_Slave, SingleCore

@par Directory contents


  - LoRaWAN_AT_Slave/Core/Inc/adc.h                                             This file contains all the function prototypes for
                                                                                the adc.c file
  - LoRaWAN_AT_Slave/Core/Inc/adc_if.h                                          Header for ADC interface configuration
  - LoRaWAN_AT_Slave/Core/Inc/dma.h                                             This file contains all the function prototypes for
                                                                                the dma.c file
  - LoRaWAN_AT_Slave/Core/Inc/flash_if.h                                        This file contains definitions for FLASH Interface functionalities.
  - LoRaWAN_AT_Slave/Core/Inc/gpio.h                                            This file contains all the function prototypes for
                                                                                the gpio.c file
  - LoRaWAN_AT_Slave/Core/Inc/main.h                                            : Header for main.c file.
                                                                                This file contains the common defines of the application.
  - LoRaWAN_AT_Slave/Core/Inc/platform.h                                        Header for General HW instances configuration
  - LoRaWAN_AT_Slave/Core/Inc/rtc.h                                             This file contains all the function prototypes for
                                                                                the rtc.c file
  - LoRaWAN_AT_Slave/Core/Inc/stm32wlxx_hal_conf.h                              HAL configuration file.
  - LoRaWAN_AT_Slave/Core/Inc/stm32wlxx_it.h                                    This file contains the headers of the interrupt handlers.
  - LoRaWAN_AT_Slave/Core/Inc/stm32_lpm_if.h                                    Header for Low Power Manager interface configuration
  - LoRaWAN_AT_Slave/Core/Inc/subghz.h                                          This file contains all the function prototypes for
                                                                                the subghz.c file
  - LoRaWAN_AT_Slave/Core/Inc/sys_app.h                                         Function prototypes for sys_app.c file
  - LoRaWAN_AT_Slave/Core/Inc/sys_conf.h                                        Applicative configuration, e.g. : debug, trace, low power, sensors
  - LoRaWAN_AT_Slave/Core/Inc/sys_debug.h                                       Configuration of the debug.c instances
  - LoRaWAN_AT_Slave/Core/Inc/timer_if.h                                        configuration of the timer_if.c instances
  - LoRaWAN_AT_Slave/Core/Inc/usart.h                                           This file contains all the function prototypes for
                                                                                the usart.c file
  - LoRaWAN_AT_Slave/Core/Inc/usart_if.h                                        Header for USART interface configuration
  - LoRaWAN_AT_Slave/Core/Inc/utilities_conf.h                                  Header for configuration file to utilities
  - LoRaWAN_AT_Slave/Core/Inc/utilities_def.h                                   Definitions for modules requiring utilities
  - LoRaWAN_AT_Slave/LoRaWAN/App/app_lorawan.h                                  Header of application of the LRWAN Middleware
  - LoRaWAN_AT_Slave/LoRaWAN/App/app_version.h                                  Definition the version of the application
  - LoRaWAN_AT_Slave/LoRaWAN/App/Commissioning.h                                End-device commissioning parameters
  - LoRaWAN_AT_Slave/LoRaWAN/App/lora_app.h                                     Header of application of the LRWAN Middleware
  - LoRaWAN_AT_Slave/LoRaWAN/App/lora_at.h                                      Header for driver at.c module
  - LoRaWAN_AT_Slave/LoRaWAN/App/lora_command.h                                 Header for driver command.c module
  - LoRaWAN_AT_Slave/LoRaWAN/App/lora_info.h                                    To give info to the application about LoRaWAN configuration
  - LoRaWAN_AT_Slave/LoRaWAN/App/se-identity.h                                  Secure Element identity and keys
  - LoRaWAN_AT_Slave/LoRaWAN/App/test_rf.h                                      Header for test_rf.c
  - LoRaWAN_AT_Slave/LoRaWAN/Target/lorawan_conf.h                              Header for LoRaWAN middleware instances
  - LoRaWAN_AT_Slave/LoRaWAN/Target/mw_log_conf.h                               Configure (enable/disable) traces
  - LoRaWAN_AT_Slave/LoRaWAN/Target/radio_board_if.h                            Header for Radio interface configuration
  - LoRaWAN_AT_Slave/LoRaWAN/Target/radio_conf.h                                Header of Radio configuration
  - LoRaWAN_AT_Slave/LoRaWAN/Target/systime.h                                   Map middleware systime
  - LoRaWAN_AT_Slave/LoRaWAN/Target/timer.h                                     Wrapper to timer server

  - LoRaWAN_AT_Slave/Core/Src/adc.c                                             This file provides code for the configuration
                                                                                of the ADC instances.
  - LoRaWAN_AT_Slave/Core/Src/adc_if.c                                          Read status related to the chip (battery level, VREF, chip temperature)
  - LoRaWAN_AT_Slave/Core/Src/dma.c                                             This file provides code for the configuration
                                                                                of all the requested memory to memory DMA transfers.
  - LoRaWAN_AT_Slave/Core/Src/flash_if.c                                        This file provides set of firmware functions to manage Flash
                                                                                Interface functionalities.
  - LoRaWAN_AT_Slave/Core/Src/gpio.c                                            This file provides code for the configuration
                                                                                of all used GPIO pins.
  - LoRaWAN_AT_Slave/Core/Src/main.c                                            : Main program body
  - LoRaWAN_AT_Slave/Core/Src/rtc.c                                             This file provides code for the configuration
                                                                                of the RTC instances.
  - LoRaWAN_AT_Slave/Core/Src/stm32wlxx_hal_msp.c                               This file provides code for the MSP Initialization
                                                                                and de-Initialization codes.
  - LoRaWAN_AT_Slave/Core/Src/stm32wlxx_it.c                                    Interrupt Service Routines.
  - LoRaWAN_AT_Slave/Core/Src/stm32_lpm_if.c                                    Low layer function to enter/exit low power modes (stop, sleep)
  - LoRaWAN_AT_Slave/Core/Src/subghz.c                                          This file provides code for the configuration
                                                                                of the SUBGHZ instances.
  - LoRaWAN_AT_Slave/Core/Src/system_stm32wlxx.c                                CMSIS Cortex Device Peripheral Access Layer System Source File
  - LoRaWAN_AT_Slave/Core/Src/sys_app.c                                         Initializes HW and SW system entities (not related to the radio)
  - LoRaWAN_AT_Slave/Core/Src/sys_debug.c                                       Configure probes pins RealTime debugging and JTAG/SerialWires for LowPower
  - LoRaWAN_AT_Slave/Core/Src/timer_if.c                                        Configure RTC Alarm, Tick and Calendar manager
  - LoRaWAN_AT_Slave/Core/Src/usart.c                                           This file provides code for the configuration
                                                                                of the USART instances.
  - LoRaWAN_AT_Slave/Core/Src/usart_if.c                                        Configuration of UART driver interface for hyperterminal communication
  - LoRaWAN_AT_Slave/LoRaWAN/App/app_lorawan.c                                  Application of the LRWAN Middleware
  - LoRaWAN_AT_Slave/LoRaWAN/App/lora_app.c                                     Application of the LRWAN Middleware
  - LoRaWAN_AT_Slave/LoRaWAN/App/lora_at.c                                      AT command API
  - LoRaWAN_AT_Slave/LoRaWAN/App/lora_command.c                                 Main command driver dedicated to command AT
  - LoRaWAN_AT_Slave/LoRaWAN/App/lora_info.c                                    To give info to the application about LoRaWAN configuration
  - LoRaWAN_AT_Slave/LoRaWAN/App/test_rf.c                                      manages tx tests
  - LoRaWAN_AT_Slave/LoRaWAN/Target/radio_board_if.c                            This file provides an interface layer between MW and Radio Board
  - LoRaWAN_AT_Slave/STM32CubeIDE/Application/User/Core/syscalls.c              STM32CubeIDE Minimal System calls file
  - LoRaWAN_AT_Slave/STM32CubeIDE/Application/User/Core/sysmem.c                STM32CubeIDE System Memory calls file


@par Hardware and Software environment

  - This example runs on the B-WL5M-SUBG1 Connectivity Expansion Board.

  - B-WL5M-SUBG1 Connectivity Expansion Board Set-up
    - Connect the Connectivity Expansion Board to your PC with an external STLINK-V3SET.
    - Please ensure that the B-WL5M-SUBG1 is supplied.

  - Configure the software via the configuration files:
    - sys_conf.h, radio_conf.h, lorawan_conf.h, lora_app.h, Commissioning.h, se-identity.h, mw_log_conf.h, main.h, etc
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
  - UART Config = 9600, 8b, 1 stopbit, no parity, no flow control
  - Terminal Config: Select 'CR+LF' for Transmit New-Line and switch 'Local echo' on
  - Reset the board
  - type AT? to get all available commands
  - Send your AT commands by typing them in the terminal

@par How to debug ?
  - make sure the flag DEBUGGER_ENABLED to 1 in sys_conf.h
  - simpler to define the flag LOW_POWER_DISABLE to 1 as well
  - compile, download and attach

@par How to use MX to modify some RF middleware and application settings
  This example is compatible (with some problems/limitations) with STM32CubeMX 
  and the RF application and middleware configuration can be modified via GUI. Few warnings and guidelines:
  - ioc file is provided in the project directory and can be opened with STM32CubeMX v6.7.0 or higher.
  - warning: when regenerating with the provided ioc file, 
    the IDE projects are regenerated and paths to HAL and MWs files from STM32Cube/Repository location are erroneously added; to avoid that, 
    user shall uncheck the "Use Default Firmware Location" in the GUI "Project-Manager" panel
    and shall replace "Firmware Relative Path" with the root directory of the STM32CubeWL firmware package  (e.g. C:\myDir\STM32Cube_FW_WL_V1.3.0\);
    problem will be fixed in next STM32CubeMX version.
  - .extSettings file allows to add to the generated IDE projects additional files not generated natively by MX (e.g. BSP files).
  - when regenerating on existing code and existing linker files:
    - STM32CubeMX updates the existing project content and preserves linker files.
    - STM32CubeMX does not update the USER CODE sections (lines between /* USER CODE BEGIN Xxx */ and /* USER CODE END Xxx */).
  - when regenerating after copying only the ioc file in an empty directory:
    - STM32CubeMX generates default project files and default linker files. (Please check original linker file from project directory)
    - it is up to the user to to fill the USER CODE sections with his application code.

@par How to use it with Azure ThreadX RTOS?
  This example can be combined with Azure ThreadX RTOS via STM32CubeMX. The video tutorial:
  "STM32WL - How to port an existing RF application on Azure ThreadX RTOS"
  is available on https://www.youtube.com/playlist?list=PLnMKNibPkDnE2eaR-ZGM3ZJXadyQLtTpX

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */