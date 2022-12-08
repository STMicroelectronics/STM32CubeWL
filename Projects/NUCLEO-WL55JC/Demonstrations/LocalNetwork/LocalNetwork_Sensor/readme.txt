/**
  @page LocalNetwork_Sensor Readme file

  @verbatim
  ******************************************************************************
  * @file    Demonstrations/LocalNetwork/LocalNetwork_Sensor/readme.txt
  * @author  MCD Application Team
  * @brief   This application is a demo software of a Sensor sending
  *          Sensor data to the demo concentrator modem
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

This directory contains a set of source files that implements Sensor sending Sensor
data (based on the Internal Temperature Sensor and VBat information) to the demo concentrator.

This application is targeting the STM32WLxx Nucleo board embedding the STM32WLxx.
  ******************************************************************************

@par Keywords

Demonstrations, SubGHz_Phy, LocalNetwork, Sensor

@par Directory contents


  - LocalNetwork_Sensor/Core/Inc/adc.h                                          This file contains all the function prototypes for
                                                                                the adc.c file
  - LocalNetwork_Sensor/Core/Inc/adc_if.h                                       Header for ADC interface configuration
  - LocalNetwork_Sensor/Core/Inc/dma.h                                          This file contains all the function prototypes for
                                                                                the dma.c file
  - LocalNetwork_Sensor/Core/Inc/gpio.h                                         This file contains all the function prototypes for
                                                                                the gpio.c file
  - LocalNetwork_Sensor/Core/Inc/main.h                                         : Header for main.c file.
                                                                                This file contains the common defines of the application.
  - LocalNetwork_Sensor/Core/Inc/platform.h                                     Header for General HW instances configuration
  - LocalNetwork_Sensor/Core/Inc/rtc.h                                          This file contains all the function prototypes for
                                                                                the rtc.c file
  - LocalNetwork_Sensor/Core/Inc/stm32wlxx_hal_conf.h                           HAL configuration file.
  - LocalNetwork_Sensor/Core/Inc/stm32wlxx_it.h                                 This file contains the headers of the interrupt handlers.
  - LocalNetwork_Sensor/Core/Inc/stm32wlxx_nucleo_conf.h                        STM32WLxx_Nucleo board configuration file.
  - LocalNetwork_Sensor/Core/Inc/stm32_lpm_if.h                                 Header for Low Power Manager interface configuration
  - LocalNetwork_Sensor/Core/Inc/subghz.h                                       This file contains all the function prototypes for
                                                                                the subghz.c file
  - LocalNetwork_Sensor/Core/Inc/sys_app.h                                      Function prototypes for sys_app.c file
  - LocalNetwork_Sensor/Core/Inc/sys_conf.h                                     Applicative configuration, e.g. : debug, trace, low power, sensors
  - LocalNetwork_Sensor/Core/Inc/sys_debug.h                                    Configuration of the debug.c instances
  - LocalNetwork_Sensor/Core/Inc/timer_if.h                                     configuration of the timer_if.c instances
  - LocalNetwork_Sensor/Core/Inc/usart.h                                        This file contains all the function prototypes for
                                                                                the usart.c file
  - LocalNetwork_Sensor/Core/Inc/usart_if.h                                     Header for USART interface configuration
  - LocalNetwork_Sensor/Core/Inc/utilities_conf.h                               Header for configuration file to utilities
  - LocalNetwork_Sensor/Core/Inc/utilities_def.h                                Definitions for modules requiring utilities
  - LocalNetwork_Sensor/SubGHz_Phy/App/demo_sensor.h                            Header for concentrator behavior module for STM32WL Concentrator Demo.
  - LocalNetwork_Sensor/SubGHz_Phy/App/meters_conf.h                            Configuration for metering module.
  - LocalNetwork_Sensor/SubGHz_Phy/Target/mw_log_conf.h                         Configure (enable/disable) traces for CM0
  - LocalNetwork_Sensor/SubGHz_Phy/Target/radio_board_if.h                      Header for Radio interface configuration
  - LocalNetwork_Sensor/SubGHz_Phy/Target/radio_conf.h                          Header of Radio configuration
  - LocalNetwork_Sensor/SubGHz_Phy/Target/timer.h                               Wrapper to timer server

  - LocalNetwork_Sensor/Core/Src/adc.c                                          This file provides code for the configuration
                                                                                of the ADC instances.
  - LocalNetwork_Sensor/Core/Src/adc_if.c                                       Read status related to the chip (battery level, VREF, chip temperature)
  - LocalNetwork_Sensor/Core/Src/dma.c                                          This file provides code for the configuration
                                                                                of all the requested memory to memory DMA transfers.
  - LocalNetwork_Sensor/Core/Src/gpio.c                                         This file provides code for the configuration
                                                                                of all used GPIO pins.
  - LocalNetwork_Sensor/Core/Src/main.c                                         : Main program body
  - LocalNetwork_Sensor/Core/Src/rtc.c                                          This file provides code for the configuration
                                                                                of the RTC instances.
  - LocalNetwork_Sensor/Core/Src/stm32wlxx_hal_msp.c                            This file provides code for the MSP Initialization
                                                                                and de-Initialization codes.
  - LocalNetwork_Sensor/Core/Src/stm32wlxx_it.c                                 Interrupt Service Routines.
  - LocalNetwork_Sensor/Core/Src/stm32_lpm_if.c                                 Low layer function to enter/exit low power modes (stop, sleep)
  - LocalNetwork_Sensor/Core/Src/subghz.c                                       This file provides code for the configuration
                                                                                of the SUBGHZ instances.
  - LocalNetwork_Sensor/Core/Src/system_stm32wlxx.c                             CMSIS Cortex Device Peripheral Access Layer System Source File
  - LocalNetwork_Sensor/Core/Src/sys_app.c                                      Initializes HW and SW system entities (not related to the radio)
  - LocalNetwork_Sensor/Core/Src/sys_debug.c                                    Configure probes pins RealTime debugging and JTAG/SerialWires for LowPower
  - LocalNetwork_Sensor/Core/Src/timer_if.c                                     Configure RTC Alarm, Tick and Calendar manager
  - LocalNetwork_Sensor/Core/Src/usart.c                                        This file provides code for the configuration
                                                                                of the USART instances.
  - LocalNetwork_Sensor/Core/Src/usart_if.c                                     Configuration of UART driver interface for hyperterminal communication
  - LocalNetwork_Sensor/STM32CubeIDE/Application/Core/syscalls.c                STM32CubeIDE Minimal System calls file
  - LocalNetwork_Sensor/STM32CubeIDE/Application/Core/sysmem.c                  STM32CubeIDE System Memory calls file
  - LocalNetwork_Sensor/SubGHz_Phy/App/demo_sensor.c                            Concentrator behavior module for STM32WL Concentrator Demo.
  - LocalNetwork_Sensor/SubGHz_Phy/Target/radio_board_if.c                      This file provides an interface layer between MW and Radio Board


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
  - Use another Nucleo with Demo/Concentrator binary (please see Demo/Concentrator project)  and start it with AT+BEACON_ON (default beacon frequency is EU)
  - the Demo/Sensor will automatically acquire the beacon and a time slot will be reserved.
  - the Demo/Sensor will send sensor data at reserved time slot.

@par How to debug ?
  - make sure the flag DEBUGGER_ENABLED to 1 in sys_conf.h
  - simpler to define the flag LOW_POWER_DISABLE to 1 as well
  - compile, download and attach

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */