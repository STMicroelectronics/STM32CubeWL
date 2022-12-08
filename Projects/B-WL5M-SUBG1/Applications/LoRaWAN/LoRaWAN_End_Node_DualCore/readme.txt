/**
  @page LoRaWAN_End_Node_DualCore Readme file

  @verbatim
  ******************************************************************************
  * @file    Applications/LoRaWAN/LoRaWAN_End_Node_DualCore/readme.txt
  * @author  MCD Application Team
  * @brief   This application is a simple demo application software of a LoRa
  *          modem connecting to Network server. Data sent can be checked on
  *          Network server for eg Loriot. Traces are displayed over UART
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

This directory contains a set of source files that implements a Dual Core (CM4 / CM0) LoRa application
device sending sensors data to LoRa Network server.
Data are sent periodically on timer event or on "user button 1" depending on the configuration.

This application is targeting the B-WL5M-SUBG1 Connectivity Expansion Board embedding the STM32WL5Mxx.
  ******************************************************************************

@par Keywords

Applications, SubGHz_Phy, LoRaWAN, End_Node, DualCore

@par Directory contents


  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/ca_conf.h                        This file contains configuration for Cryptographic API (CA)
                                                                                module functionalities.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/ca_low_level.h                   This file contains the low level definitions of the Cryptographic API (CA) module.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/gpio.h                           This file contains all the function prototypes for
                                                                                the gpio.c file
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/ipcc.h                           This file contains all the function prototypes for
                                                                                the ipcc.c file
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/ipcc_if.h                        This file contains the interface of the ipcc driver on CM0PLUS.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/kms_config.h                     This file contains configuration for Key Management Services (KMS)
                                                                                module functionalities.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/kms_if.h                         This file contains kms interfaces with middleware
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/kms_low_level.h                  This file contains definitions for Key Management Services (KMS)
                                                                                module Low Level access to Flash, CRC...
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/kms_mem_pool_def.h               This file contains definitions for Key Management Services (KMS)
                                                                                memory pool definition
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/kms_platf_objects_config.h       This file contains definitions for Key Management Services (KMS)
                                                                                module platform objects management configuration
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/kms_platf_objects_interface.h    This file contains definitions for Key Management Services (KMS)
                                                                                module platform objects management configuration interface
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/main.h                           : Header for main.c file.
                                                                                This file contains the common defines of the application.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/mbedtls_config.h                 !!! No description found !!!
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/nvms_low_level.h                 This file contains definitions for Key Management Services (KMS)
                                                                                module NVM Low Level access to physical storage (Flash...)
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/platform.h                       Header for General HW instances configuration
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/rtc.h                            This file contains all the function prototypes for
                                                                                the rtc.c file
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/stm32wlxx_hal_conf.h             HAL configuration file.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/stm32wlxx_it.h                   This file contains the headers of the interrupt handlers.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/stm32_lpm_if.h                   Header for Low Power Manager interface configuration
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/subghz.h                         This file contains all the function prototypes for
                                                                                the subghz.c file
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/sys_app.h                        Function prototypes for sys_app.c file
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/sys_conf.h                       Applicative configuration, e.g. : debug, trace, low power, sensors
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/timer_if.h                       configuration of the timer_if.c instances
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/utilities_conf.h                 Header for configuration file to utilities
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Inc/utilities_def.h                  Definitions for modules requiring utilities
  - LoRaWAN_End_Node_DualCore/CM0PLUS/KMS/App/app_kms.h                         !!! No description found !!!
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/App/app_lorawan.h                 Header of application of the LRWAN Middleware
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/App/app_version.h                 Definition the version of the CM0PLUS application
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/App/Commissioning.h               End-device commissioning parameters
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/App/lora_app.h                    Header of application of the LRWAN Middleware
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/App/se-identity.h                 Secure Element identity and keys
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/Target/lorawan_conf.h             Header for LoRaWAN middleware instances
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/Target/mw_log_conf.h              Configure (enable/disable) traces for CM0
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/Target/radio_board_if.h           Header for Radio interface configuration
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/Target/radio_conf.h               Header of Radio configuration
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/Target/systime.h                  Map middleware systime
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/Target/timer.h                    Wrapper to timer server
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/LmHandler_mbwrapper.h               This file implements the CM0PLUS side wrapper of the LmHandler
                                                                                interface shared between M0 and M4.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/mbmux.h                             API which interfaces CM0PLUS to IPCC
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/mbmuxif_lora.h                      API provided to CM0PLUS application to register and handle LoraWAN via MBMUX
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/mbmuxif_radio.h                     API for CM0PLUS application to register and handle RADIO driver via MBMUX
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/mbmuxif_sys.h                       API for CM0PLUS application to handle the SYSTEM MBMUX channel
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/mbmuxif_trace.h                     API for CM0PLUS application to register and handle TRACE via MBMUX
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/radio_mbwrapper.h                   This file implements the CM0PLUS side wrapper of the Radio interface
                                                                                shared between M0 and M4.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/adc.h                                This file contains all the function prototypes for
                                                                                the adc.c file
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/adc_if.h                             Header for ADC interface configuration
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/b_wl5m_subg_conf.h                   B-WL5M-SUBG board configuration file.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/dma.h                                This file contains all the function prototypes for
                                                                                the dma.c file
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/flash_if.h                           This file contains definitions for FLASH Interface functionalities.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/gpio.h                               This file contains all the function prototypes for
                                                                                the gpio.c file
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/ipcc.h                               This file contains all the function prototypes for
                                                                                the ipcc.c file
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/ipcc_if.h                            This file contains the interface of the ipcc driver on CM4.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/main.h                               : Header for main.c file.
                                                                                This file contains the common defines of the application.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/platform.h                           Header for General HW instances configuration
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/stm32wlxx_hal_conf.h                 HAL configuration file.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/stm32wlxx_it.h                       This file contains the headers of the interrupt handlers.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/stm32_lpm_if.h                       Header for Low Power Manager interface configuration
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/sys_app.h                            Function prototypes for sys_app.c file
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/sys_conf.h                           Applicative configuration, e.g. : debug, trace, low power, sensors
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/sys_sensors.h                        Header for sensors application
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/timer_if.h                           configuration of the timer_if.c instances
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/usart.h                              This file contains all the function prototypes for
                                                                                the usart.c file
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/usart_if.h                           Header for USART interface configuration
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/utilities_conf.h                     Header for configuration file to utilities
  - LoRaWAN_End_Node_DualCore/CM4/Core/Inc/utilities_def.h                      Definitions for modules requiring utilities
  - LoRaWAN_End_Node_DualCore/CM4/LoRaWAN/App/app_lorawan.h                     Header of application of the LRWAN Middleware
  - LoRaWAN_End_Node_DualCore/CM4/LoRaWAN/App/app_version.h                     Definition the version of the CM4 application
  - LoRaWAN_End_Node_DualCore/CM4/LoRaWAN/App/CayenneLpp.h                      Implements the Cayenne Low Power Protocol
  - LoRaWAN_End_Node_DualCore/CM4/LoRaWAN/App/lora_app.h                        Header of application of the LRWAN Middleware
  - LoRaWAN_End_Node_DualCore/CM4/LoRaWAN/Target/lorawan_conf.h                 Header for LoRaWAN middleware instances
  - LoRaWAN_End_Node_DualCore/CM4/LoRaWAN/Target/systime.h                      Map middleware systime
  - LoRaWAN_End_Node_DualCore/CM4/LoRaWAN/Target/timer.h                        Wrapper to timer server
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/LmHandler_mbwrapper.h                   This file implements the CM4 side wrapper of the LmHandler
                                                                                interface shared between M0 and M4.
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/mbmux.h                                 API which interfaces CM4 to IPCC
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/mbmuxif_lora.h                          API provided to CM4 application to register and handle LoraWAN via MBMUX
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/mbmuxif_radio.h                         API for CM4 application to register and handle RADIO driver via MBMUX
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/mbmuxif_sys.h                           API for CM4 application to handle the SYSTEM MBMUX channel
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/mbmuxif_trace.h                         API for CM4 application to register and handle TRACE via MBMUX
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/radio_mbwrapper.h                       This file implements the CM4 side wrapper of the Radio interface
                                                                                shared between M0 and M4.
  - LoRaWAN_End_Node_DualCore/Common/MbMux/features_info.h                      Feature list and parameters TypeDefinitions
  - LoRaWAN_End_Node_DualCore/Common/MbMux/lora_info.h                          To give info to the application about LoRaWAN configuration
  - LoRaWAN_End_Node_DualCore/Common/MbMux/mbmux_table.h                        Maps the IPCC channels to memory buffers
  - LoRaWAN_End_Node_DualCore/Common/MbMux/msg_id.h                             MBMUX message ID enumeration
  - LoRaWAN_End_Node_DualCore/Common/System/sys_debug.h                         Configuration of the debug.c instances

  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/ca_low_level.c                   This file contains the low level implementations of the Cryptographic API (CA) module.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/gpio.c                           This file provides code for the configuration
                                                                                of all used GPIO pins.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/ipcc.c                           This file provides code for the configuration
                                                                                of the IPCC instances.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/ipcc_if.c                        Interface to IPCC: handles IRQs and abstract application from Ipcc handler and channel direction
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/kms_low_level.c                  This file contains implementations for Key Management Services (KMS)
                                                                                module Low Level access to Flash, CRC...
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/main.c                           : Main program body
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/nvms_low_level.c                 This file contains definitions for Key Management Services (KMS)
                                                                                module NVM Low Level access to physical storage (Flash...)
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/rtc.c                            This file provides code for the configuration
                                                                                of the RTC instances.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/stm32wlxx_hal_msp.c              This file provides code for the MSP Initialization
                                                                                and de-Initialization codes.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/stm32wlxx_it.c                   Interrupt Service Routines.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/stm32_lpm_if.c                   Low layer function to enter/exit low power modes (stop, sleep)
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/subghz.c                         This file provides code for the configuration
                                                                                of the SUBGHZ instances.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/sys_app.c                        Initializes HW and SW system entities (not related to the radio)
  - LoRaWAN_End_Node_DualCore/CM0PLUS/Core/Src/timer_if.c                       Configure RTC Alarm, Tick and Calendar manager
  - LoRaWAN_End_Node_DualCore/CM0PLUS/KMS/App/app_kms.c                         !!! No description found !!!
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/App/app_lorawan.c                 Application of the LRWAN Middleware
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/App/lora_app.c                    Application of the LRWAN Middleware
  - LoRaWAN_End_Node_DualCore/CM0PLUS/LoRaWAN/Target/radio_board_if.c           This file provides an interface layer between MW and Radio Board
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/features_info.c                     CM0PLUS supported features list
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/LmHandler_mbwrapper.c               This file implements the CM0PLUS side wrapper of the LoraMac interface
                                                                                shared between M0 and M4.
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/lora_info.c                         To give info to the application about LoRaWAN configuration
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/mbmux.c                             Interface CPU2 to IPCC: multiplexer to map features to IPCC channels
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/mbmuxif_lora.c                      allows CM0 application to register and handle LoraWAN to MBMUX
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/mbmuxif_radio.c                     allows CM0PLUS application to register and handle RADIO driver via MBMUX
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/mbmuxif_sys.c                       allows CM0 application to handle the SYSTEM MBMUX channel
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/mbmuxif_trace.c                     Interface layer CM0PLUS Trace to MBMUX (Mailbox Multiplexer)
  - LoRaWAN_End_Node_DualCore/CM0PLUS/MbMux/radio_mbwrapper.c                   This file implements the CM0 side wrapper of the Radio interface
                                                                                shared between M0 and M4.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/adc.c                                This file provides code for the configuration
                                                                                of the ADC instances.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/adc_if.c                             Read status related to the chip (battery level, VREF, chip temperature)
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/dma.c                                This file provides code for the configuration
                                                                                of all the requested memory to memory DMA transfers.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/flash_if.c                           This file provides set of firmware functions to manage Flash
                                                                                Interface functionalities.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/gpio.c                               This file provides code for the configuration
                                                                                of all used GPIO pins.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/ipcc.c                               This file provides code for the configuration
                                                                                of the IPCC instances.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/ipcc_if.c                            Interface to IPCC: handles IRQs and abstract application from Ipcc handler and channel direction
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/main.c                               : Main program body
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/stm32wlxx_hal_msp.c                  This file provides code for the MSP Initialization
                                                                                and de-Initialization codes.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/stm32wlxx_it.c                       Interrupt Service Routines.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/stm32_lpm_if.c                       Low layer function to enter/exit low power modes (stop, sleep)
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/sys_app.c                            Initializes HW and SW system entities (not related to the radio)
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/sys_sensors.c                        Manages the sensors on the application
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/timer_if.c                           Configure RTC Alarm, Tick and Calendar manager
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/usart.c                              This file provides code for the configuration
                                                                                of the USART instances.
  - LoRaWAN_End_Node_DualCore/CM4/Core/Src/usart_if.c                           Configuration of UART driver interface for hyperterminal communication
  - LoRaWAN_End_Node_DualCore/CM4/LoRaWAN/App/app_lorawan.c                     Application of the LRWAN Middleware
  - LoRaWAN_End_Node_DualCore/CM4/LoRaWAN/App/CayenneLpp.c                      Implements the Cayenne Low Power Protocol
  - LoRaWAN_End_Node_DualCore/CM4/LoRaWAN/App/lora_app.c                        Application of the LRWAN Middleware
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/LmHandler_mbwrapper.c                   This file implements the CM4 side wrapper of the LoraMac interface
                                                                                shared between M0 and M4.
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/mbmux.c                                 CM4 side multiplexer to map features to IPCC channels
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/mbmuxif_lora.c                          allows CM4 application to register and handle LoraWAN via MBMUX
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/mbmuxif_radio.c                         allows CM4 application to register and handle RADIO driver via MBMUX
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/mbmuxif_sys.c                           allows CM4 application to handle the SYSTEM MBMUX channel.
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/mbmuxif_trace.c                         allows CM4 application to receive by CM0 TRACE via MBMUX
  - LoRaWAN_End_Node_DualCore/CM4/MbMux/radio_mbwrapper.c                       This file implements the CM4 side wrapper of the Radio interface
                                                                                shared between M0 and M4.
  - LoRaWAN_End_Node_DualCore/Common/System/system_stm32wlxx.c                  CMSIS Cortex Device Peripheral Access Layer System Source File
  - LoRaWAN_End_Node_DualCore/Common/System/sys_debug.c                         Configure probes pins RealTime debugging and JTAG/SerialWires for LowPower
  - LoRaWAN_End_Node_DualCore/STM32CubeIDE/CM0PLUS/Application/User/Core/syscalls.cSTM32CubeIDE Minimal System calls file
  - LoRaWAN_End_Node_DualCore/STM32CubeIDE/CM0PLUS/Application/User/Core/sysmem.cSTM32CubeIDE System Memory calls file
  - LoRaWAN_End_Node_DualCore/STM32CubeIDE/CM4/Application/User/Core/syscalls.c STM32CubeIDE Minimal System calls file
  - LoRaWAN_End_Node_DualCore/STM32CubeIDE/CM4/Application/User/Core/sysmem.c   STM32CubeIDE System Memory calls file


@par Hardware and Software environment

  - This example runs on the B-WL5M-SUBG1 Connectivity Expansion Board.

  - B-WL5M-SUBG1 Connectivity Expansion Board Set-up
    - Connect the Connectivity Expansion Board to your PC with an external STLINK-V3SET.
    - Please ensure that the B-WL5M-SUBG1 is supplied.

  - Configure the software via the configuration files:
    - CM0PLUS (Mw and radio drivers config)
        - sys_conf.h, radio_conf.h, lorawan_conf.h, Commissioning.h, se-identity.h, mw_log_conf.h, main.h, etc
    - CM4 (Lora application)
        - sys_conf.h, lora_app.c, lora_app.h, nucleo_conf.h, main.h, etc
    - Careful:
        - the region and class chosen on CM4/LoRaWAN/App/lora_app.h shall be compatible with CM0PLUS/LoRaWAN/Target/lorawan_conf.h list

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
  - Rebuild all CM4 files and load your image into CM4 target memory
  - Rebuild all CM0PLUS files, set PWR_CR4_C2BOOT flag via CM4 and load your image into CM0PLUS target memory
  - Reset the system (two cores)
  - Run the example
  - Open a Terminal, connected the LoRa Object
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
  This example can be combined with Azure ThreadX RTOS via STM32CubeMX (CM4 core only). 
  The video tutorial: "STM32WL - How to port an existing RF application on Azure ThreadX RTOS"
  is available on https://www.youtube.com/playlist?list=PLnMKNibPkDnE2eaR-ZGM3ZJXadyQLtTpX

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */