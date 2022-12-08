/**
  @page Sensor Readme file

  @verbatim
  ******************************************************************************
  * @file    Demonstrations/LocalNetwork_Sensor/Sensor/readme.txt
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
data (based on all sensors available on the B-WL5M-SUBG1 board) to the demo concentrator.

This application is targeting the B-WL5M-SUBG1 Connectivity Expansion Board embedding the STM32WL5Mxx.
  ******************************************************************************

@par Keywords

Demonstrations, SubGHz_Phy, LocalNetwork, Sensor

@par Directory contents


  - Sensor/Core/Inc/adc.h                                                       This file contains all the function prototypes for
                                                                                the adc.c file
  - Sensor/Core/Inc/adc_if.h                                                    Header for ADC interface configuration
  - Sensor/Core/Inc/b_wl5m_subg_conf.h                                          configuration file.
                                                                                This file is a copy of template b_wl5m_subg_conf_template.h
  - Sensor/Core/Inc/dma.h                                                       This file contains all the function prototypes for
                                                                                the dma.c file
  - Sensor/Core/Inc/gpio.h                                                      This file contains all the function prototypes for
                                                                                the gpio.c file
  - Sensor/Core/Inc/main.h                                                      : Header for main.c file.
                                                                                This file contains the common defines of the application.
  - Sensor/Core/Inc/mx25l4006_conf.h                                            This file contains all the description of the
                                                                                MX25L4006 CMOS Serial Flash.
  - Sensor/Core/Inc/platform.h                                                  Header for General HW instances configuration
  - Sensor/Core/Inc/rtc.h                                                       This file contains all the function prototypes for
                                                                                the rtc.c file
  - Sensor/Core/Inc/stm32wlxx_hal_conf.h                                        HAL configuration file.
  - Sensor/Core/Inc/stm32wlxx_it.h                                              This file contains the headers of the interrupt handlers.
  - Sensor/Core/Inc/stm32_lpm_if.h                                              Header for Low Power Manager interface configuration
  - Sensor/Core/Inc/subghz.h                                                    This file contains all the function prototypes for
                                                                                the subghz.c file
  - Sensor/Core/Inc/sys_app.h                                                   Function prototypes for sys_app.c file
  - Sensor/Core/Inc/sys_conf.h                                                  Applicative configuration, e.g. : debug, trace, low power, sensors
  - Sensor/Core/Inc/sys_debug.h                                                 Configuration of the debug.c instances
  - Sensor/Core/Inc/timer_if.h                                                  configuration of the timer_if.c instances
  - Sensor/Core/Inc/usart.h                                                     This file contains all the function prototypes for
                                                                                the usart.c file
  - Sensor/Core/Inc/usart_if.h                                                  Header for USART interface configuration
  - Sensor/Core/Inc/utilities_conf.h                                            Header for configuration file to utilities
  - Sensor/Core/Inc/utilities_def.h                                             Definitions for modules requiring utilities
  - Sensor/Middlewares/ST/STM32_MotionAR_Library/Inc/motion_ar.h                Header for motion_ar module
  - Sensor/Middlewares/ST/STM32_MotionEC_Library/Inc/motion_ec.h                Header for motion_ec module
  - Sensor/Middlewares/ST/STM32_MotionMC_Library/Inc/motion_mc.h                Header for motion_mc module
  - Sensor/Middlewares/ST/STM32_MotionMC_Library/Inc/motion_mc_cm0p.h           Header for motion_mc_cm0p module
  - Sensor/Middlewares/ST/STM32_MotionPM_Library/Inc/motion_pm.h                Header for motion_pm module
  - Sensor/SubGHz_Phy/App/demo_sensor.h                                         Header for concentrator behavior module for STM32WL Concentrator Demo.
  - Sensor/SubGHz_Phy/App/meters.h                                              Header for metering module.
  - Sensor/SubGHz_Phy/App/meters_conf.h                                         Configuration for metering module.
  - Sensor/SubGHz_Phy/App/stm32wl55c_discovery_conf.h                           configuration file.
  - Sensor/SubGHz_Phy/App/version.h                                             Header with version of STM32WL Sensor Concentrator Demo.
  - Sensor/SubGHz_Phy/Target/mw_log_conf.h                                      Configure (enable/disable) traces for CM0
  - Sensor/SubGHz_Phy/Target/radio_board_if.h                                   Header for Radio interface configuration
  - Sensor/SubGHz_Phy/Target/radio_conf.h                                       Header of Radio configuration
  - Sensor/SubGHz_Phy/Target/timer.h                                            Wrapper to timer server

  - Sensor/Core/Src/adc.c                                                       This file provides code for the configuration
                                                                                of the ADC instances.
  - Sensor/Core/Src/adc_if.c                                                    Read status related to the chip (battery level, VREF, chip temperature)
  - Sensor/Core/Src/dma.c                                                       This file provides code for the configuration
                                                                                of all the requested memory to memory DMA transfers.
  - Sensor/Core/Src/gpio.c                                                      This file provides code for the configuration
                                                                                of all used GPIO pins.
  - Sensor/Core/Src/main.c                                                      : Main program body
  - Sensor/Core/Src/rtc.c                                                       This file provides code for the configuration
                                                                                of the RTC instances.
  - Sensor/Core/Src/stm32wlxx_hal_msp.c                                         This file provides code for the MSP Initialization
                                                                                and de-Initialization codes.
  - Sensor/Core/Src/stm32wlxx_it.c                                              Interrupt Service Routines.
  - Sensor/Core/Src/stm32_lpm_if.c                                              Low layer function to enter/exit low power modes (stop, sleep)
  - Sensor/Core/Src/subghz.c                                                    This file provides code for the configuration
                                                                                of the SUBGHZ instances.
  - Sensor/Core/Src/system_stm32wlxx.c                                          CMSIS Cortex Device Peripheral Access Layer System Source File
  - Sensor/Core/Src/sys_app.c                                                   Initializes HW and SW system entities (not related to the radio)
  - Sensor/Core/Src/sys_debug.c                                                 Configure probes pins RealTime debugging and JTAG/SerialWires for LowPower
  - Sensor/Core/Src/timer_if.c                                                  Configure RTC Alarm, Tick and Calendar manager
  - Sensor/Core/Src/usart.c                                                     This file provides code for the configuration
                                                                                of the USART instances.
  - Sensor/Core/Src/usart_if.c                                                  Configuration of UART driver interface for hyperterminal communication
  - Sensor/STM32CubeIDE/Application/Core/syscalls.c                             STM32CubeIDE Minimal System calls file
  - Sensor/STM32CubeIDE/Application/Core/sysmem.c                               STM32CubeIDE System Memory calls file
  - Sensor/SubGHz_Phy/App/demo_sensor.c                                         Concentrator behavior module for STM32WL Concentrator Demo.
  - Sensor/SubGHz_Phy/App/meters.c                                              Metering module. Measures sensor data.
  - Sensor/SubGHz_Phy/Target/radio_board_if.c                                   This file provides an interface layer between MW and Radio Board


@par Hardware and Software environment

  - This example runs on the B-WL5M-SUBG1 Connectivity Expansion Board.

  - B-WL5M-SUBG1 Connectivity Expansion Board Set-up
    - Connect the Connectivity Expansion Board to your PC with an external STLINK-V3SET.
    - Please ensure that the B-WL5M-SUBG1 is supplied.

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