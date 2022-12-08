/**
  @page LocalNetwork_Concentrator Readme file

  @verbatim
  ******************************************************************************
  * @file    Demonstrations/LocalNetwork/LocalNetwork_Concentrator/readme.txt
  * @author  MCD Application Team
  * @brief   This application is a demo software of a Concentrator sending
  *          Beacon to  administrate a Network and receiving Sensor data
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

This directory contains a set of source files that implements a Concentrator
sending Beacon to administrate a Network of up to 14 sensors and receives
each connected Sensor's data.

This application is targeting the STM32WLxx Nucleo board embedding the STM32WLxx.
  ******************************************************************************

@par Keywords

Demonstrations, SubGHz_Phy, LocalNetwork, Concentrator

@par Directory contents


  - LocalNetwork_Concentrator/Common/Inc/demo_data_format.h                     Header with data format definitions for STM32WL Concentrator Demo.
  - LocalNetwork_Concentrator/Common/Inc/demo_packet_format.h                   Header with packet format definitions for STM32WL Concentrator Demo.
  - LocalNetwork_Concentrator/Common/Inc/demo_prnd.h                            Pseudorandom function for STM32WL demo.
  - LocalNetwork_Concentrator/Common/Inc/demo_radio_utils.h                     Module for various utilities common to Concentrator and Sensor of STM32WL demo.
  - LocalNetwork_Concentrator/Common/Inc/demo_regions.h                         Region specific definitions for STM32WL Sensor Concentrator Demo.
  - LocalNetwork_Concentrator/Common/Inc/demo_utils.h                           Module for various utilities common to Concentrator and Sensor of STM32WL demo.
  - LocalNetwork_Concentrator/Common/Inc/version.h                              Header with version of STM32WL Sensor Concentrator Demo.
  - LocalNetwork_Concentrator/Core/Inc/adc.h                                    This file contains all the function prototypes for
                                                                                the adc.c file
  - LocalNetwork_Concentrator/Core/Inc/adc_if.h                                 Header for ADC interface configuration
  - LocalNetwork_Concentrator/Core/Inc/dma.h                                    This file contains all the function prototypes for
                                                                                the dma.c file
  - LocalNetwork_Concentrator/Core/Inc/gpio.h                                   This file contains all the function prototypes for
                                                                                the gpio.c file
  - LocalNetwork_Concentrator/Core/Inc/main.h                                   : Header for main.c file.
                                                                                This file contains the common defines of the application.
  - LocalNetwork_Concentrator/Core/Inc/platform.h                               Header for General HW instances configuration
  - LocalNetwork_Concentrator/Core/Inc/rtc.h                                    This file contains all the function prototypes for
                                                                                the rtc.c file
  - LocalNetwork_Concentrator/Core/Inc/stm32wlxx_hal_conf.h                     HAL configuration file.
  - LocalNetwork_Concentrator/Core/Inc/stm32wlxx_it.h                           This file contains the headers of the interrupt handlers.
  - LocalNetwork_Concentrator/Core/Inc/stm32wlxx_nucleo_conf.h                  STM32WLxx_Nucleo board configuration file.
  - LocalNetwork_Concentrator/Core/Inc/stm32_lpm_if.h                           Header for Low Power Manager interface configuration
  - LocalNetwork_Concentrator/Core/Inc/subghz.h                                 This file contains all the function prototypes for
                                                                                the subghz.c file
  - LocalNetwork_Concentrator/Core/Inc/sys_app.h                                Function prototypes for sys_app.c file
  - LocalNetwork_Concentrator/Core/Inc/sys_conf.h                               Applicative configuration, e.g. : debug, trace, low power, sensors
  - LocalNetwork_Concentrator/Core/Inc/sys_debug.h                              Configuration of the debug.c instances
  - LocalNetwork_Concentrator/Core/Inc/timer_if.h                               configuration of the timer_if.c instances
  - LocalNetwork_Concentrator/Core/Inc/usart.h                                  This file contains all the function prototypes for
                                                                                the usart.c file
  - LocalNetwork_Concentrator/Core/Inc/usart_if.h                               Header for USART interface configuration
  - LocalNetwork_Concentrator/Core/Inc/utilities_conf.h                         Header for configuration file to utilities
  - LocalNetwork_Concentrator/Core/Inc/utilities_def.h                          Definitions for modules requiring utilities
  - LocalNetwork_Concentrator/SubGHz_Phy/App/demo_at.h                          Header for driver at.c module
  - LocalNetwork_Concentrator/SubGHz_Phy/App/demo_command.h                     Header for driver command.c module
  - LocalNetwork_Concentrator/SubGHz_Phy/App/demo_concentrator.h                Header for concentrator behavior module for STM32WL Concentrator Demo.
  - LocalNetwork_Concentrator/SubGHz_Phy/App/demo_report.h                      Report various things to printf console.
  - LocalNetwork_Concentrator/SubGHz_Phy/Target/mw_log_conf.h                   Configure (enable/disable) traces for CM0
  - LocalNetwork_Concentrator/SubGHz_Phy/Target/radio_board_if.h                Header for Radio interface configuration
  - LocalNetwork_Concentrator/SubGHz_Phy/Target/radio_conf.h                    Header of Radio configuration
  - LocalNetwork_Concentrator/SubGHz_Phy/Target/timer.h                         Wrapper to timer server

  - LocalNetwork_Concentrator/Common/Src/demo_radio_utils.c                     Module for various utilities common to Concentrator and Sensor of STM32WL demo.
  - LocalNetwork_Concentrator/Common/Src/demo_regions.c                         Region specific definitions for STM32WL Sensor Concentrator Demo.
  - LocalNetwork_Concentrator/Common/Src/demo_utils.c                           Module for various utilities common to Concentrator and Sensor of STM32WL demo.
  - LocalNetwork_Concentrator/Core/Src/adc.c                                    This file provides code for the configuration
                                                                                of the ADC instances.
  - LocalNetwork_Concentrator/Core/Src/adc_if.c                                 Read status related to the chip (battery level, VREF, chip temperature)
  - LocalNetwork_Concentrator/Core/Src/dma.c                                    This file provides code for the configuration
                                                                                of all the requested memory to memory DMA transfers.
  - LocalNetwork_Concentrator/Core/Src/gpio.c                                   This file provides code for the configuration
                                                                                of all used GPIO pins.
  - LocalNetwork_Concentrator/Core/Src/main.c                                   : Main program body
  - LocalNetwork_Concentrator/Core/Src/rtc.c                                    This file provides code for the configuration
                                                                                of the RTC instances.
  - LocalNetwork_Concentrator/Core/Src/stm32wlxx_hal_msp.c                      This file provides code for the MSP Initialization
                                                                                and de-Initialization codes.
  - LocalNetwork_Concentrator/Core/Src/stm32wlxx_it.c                           Interrupt Service Routines.
  - LocalNetwork_Concentrator/Core/Src/stm32_lpm_if.c                           Low layer function to enter/exit low power modes (stop, sleep)
  - LocalNetwork_Concentrator/Core/Src/subghz.c                                 This file provides code for the configuration
                                                                                of the SUBGHZ instances.
  - LocalNetwork_Concentrator/Core/Src/system_stm32wlxx.c                       CMSIS Cortex Device Peripheral Access Layer System Source File
  - LocalNetwork_Concentrator/Core/Src/sys_app.c                                Initializes HW and SW system entities (not related to the radio)
  - LocalNetwork_Concentrator/Core/Src/sys_debug.c                              Configure probes pins RealTime debugging and JTAG/SerialWires for LowPower
  - LocalNetwork_Concentrator/Core/Src/timer_if.c                               Configure RTC Alarm, Tick and Calendar manager
  - LocalNetwork_Concentrator/Core/Src/usart.c                                  This file provides code for the configuration
                                                                                of the USART instances.
  - LocalNetwork_Concentrator/Core/Src/usart_if.c                               Configuration of UART driver interface for hyperterminal communication
  - LocalNetwork_Concentrator/STM32CubeIDE/Application/Core/syscalls.c          STM32CubeIDE Minimal System calls file
  - LocalNetwork_Concentrator/STM32CubeIDE/Application/Core/sysmem.c            STM32CubeIDE System Memory calls file
  - LocalNetwork_Concentrator/SubGHz_Phy/App/demo_at.c                          at command API
  - LocalNetwork_Concentrator/SubGHz_Phy/App/demo_command.c                     Main command driver dedicated to command AT
  - LocalNetwork_Concentrator/SubGHz_Phy/App/demo_concentrator.c                Concentrator behavior module for STM32WL Concentrator Demo.
  - LocalNetwork_Concentrator/SubGHz_Phy/App/demo_report.c                      Report various things to printf console.
  - LocalNetwork_Concentrator/SubGHz_Phy/Target/radio_board_if.c                This file provides an interface layer between MW and Radio Board


@par Hardware and Software environment

  - This example runs on the STM32WLxx Nucleo boards. Both NUCLEO-WL55JC1 (HIGH-BAND) and NUCLEO-WL55JC2 (LOW-BAND) are suitable.

  - STM32WLxx Nucleo board Set-up
    - Connect the Nucleo board to your PC with a USB cable type A to micro-B
      to ST-LINK connector.
    - Please ensure that the ST-LINK connector jumpers are fitted.

  - Configure the software via the configuration files:
    - sys_conf.h, radio_conf.h, mw_log_conf.h, main.h, etc

  -Set Up:

             --------------------------  V    V  --------------------------
             |    Demo/Sensor         |  |    |  |   Demo/Concentrator    |
             |                        |  |    |  |                        |
   ComPort<--|                        |--|    |--|                        |-->ComPort
             |                        |          |                        |
             --------------------------          --------------------------

@par How to use it ?
In order to make the program work, you must do the following :
  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Run the example
  - Open a Terminal, connected the Demo/Sensor
  - UART Config = 9600, 8b, 1 stopbit, no parity, no flow control
  - Terminal Config: Select 'CR+LF' for Transmit New-Line and switch 'Local echo' on
  - Type AT? to get the all ATcommand.
  - Type AT+LIST_REGIONS to list all supported regions and AT+REGION and AT+SUBREGION to set the region where user is located
  - Type AT+BEACON_ON (default beacon frequency is EU)
  - the Demo/Concentrator will automatically start to send beacon every 16 seconds.
  - the Demo/Sensor will automatically acquire the beacon and a time slot will be reserved.
     -Blinks green every seconds and blinks red every red every beacon
  - the Demo/Sensor will send sensor data at reserved time slot.
  - the Demo/Sensors data will be logged onto the concentrator's terminal.
  - Any time the concentrator receives one-s sensor packet it will displayed as:
      AT+RCV=UID, COUNTER, VERSION_MAJOR, VERSION_MINOR, RSSI, SNR, TEMP, Voltage
        UID is a 32bits lsb of IEEE64 number. It may be 0xFFFFFFFF for early version of STM32WLxx
        COUNTER is the uplink counter of a sensor, and can be used for PER count

@par How to debug ?
  - make sure the flag DEBUGGER_ENABLED to 1 in sys_conf.h
  - simpler to define the flag LOW_POWER_DISABLE to 1 as well
  - compile, download and attach

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
