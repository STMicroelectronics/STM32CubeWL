/**
  @page SubGHz_Phy_Per Readme file

  @verbatim
  ******************************************************************************
  * @file    Applications/SubGHz_Phy/SubGHz_Phy_Per/readme.txt
  * @author  MCD Application Team
  * @brief   This application is a simple demo application software between 2
  *          LoRa Objects: a STM32WL Nucleo board and whatever other LoRa Radio
  *          board embedding SubGHz_Phy_Per application too
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

This directory contains a set of source files that implements a PER (Packet Error Rate) tests application
with IBM whitening between one Tx device and one Rx device.

Tx device:
   - in /SubGHz_Phy/App/subghz_phy_app.c update #define TEST_MODE to RADIO_TX, compile and load
   - packet content is : preamble | sync | payload length | payload | crc
     + crc calculated using payload length and payload
     + whitening is calculated over payload length | payload | crc
   - the transmission starts forever in GFSK 50kbit/s with a payload of 64 bytes
   - user button 1 increments "packet length" by 16 bytes
   - user button 2 increments "packet length" by 1 bytes
   - user button 3 toggles packet "payload mode" from ramp (0x00, 0x01..) to prbs9
   - LED blue on while radio in Tx
Rx device:
   - in /SubGHz_Phy/App/subghz_phy_app.c update #define TEST_MODE to RADIO_RX, compile and load
   - LED green on when Rx OK, LED red when Rx KO

  ******************************************************************************

@par Keywords

Applications, SubGHz_Phy, PER, SingleCore

@par Directory contents


  - SubGHz_Phy_Per/Core/Inc/dma.h                                               This file contains all the function prototypes for
                                                                                the dma.c file
  - SubGHz_Phy_Per/Core/Inc/gpio.h                                              This file contains all the function prototypes for
                                                                                the gpio.c file
  - SubGHz_Phy_Per/Core/Inc/main.h                                              : Header for main.c file.
                                                                                This file contains the common defines of the application.
  - SubGHz_Phy_Per/Core/Inc/platform.h                                          Header for General HW instances configuration
  - SubGHz_Phy_Per/Core/Inc/rtc.h                                               This file contains all the function prototypes for
                                                                                the rtc.c file
  - SubGHz_Phy_Per/Core/Inc/stm32wlxx_hal_conf.h                                HAL configuration file.
  - SubGHz_Phy_Per/Core/Inc/stm32wlxx_it.h                                      This file contains the headers of the interrupt handlers.
  - SubGHz_Phy_Per/Core/Inc/stm32wlxx_nucleo_conf.h                             STM32WLxx_Nucleo board configuration file.
  - SubGHz_Phy_Per/Core/Inc/stm32_lpm_if.h                                      Header for Low Power Manager interface configuration
  - SubGHz_Phy_Per/Core/Inc/subghz.h                                            This file contains all the function prototypes for
                                                                                the subghz.c file
  - SubGHz_Phy_Per/Core/Inc/sys_app.h                                           Function prototypes for sys_app.c file
  - SubGHz_Phy_Per/Core/Inc/sys_conf.h                                          Applicative configuration, e.g. : debug, trace, low power, sensors
  - SubGHz_Phy_Per/Core/Inc/sys_debug.h                                         Configuration of the debug.c instances
  - SubGHz_Phy_Per/Core/Inc/timer_if.h                                          configuration of the timer_if.c instances
  - SubGHz_Phy_Per/Core/Inc/usart.h                                             This file contains all the function prototypes for
                                                                                the usart.c file
  - SubGHz_Phy_Per/Core/Inc/usart_if.h                                          Header for USART interface configuration
  - SubGHz_Phy_Per/Core/Inc/utilities_conf.h                                    Header for configuration file to utilities
  - SubGHz_Phy_Per/Core/Inc/utilities_def.h                                     Definitions for modules requiring utilities
  - SubGHz_Phy_Per/SubGHz_Phy/App/app_subghz_phy.h                              Header of application of the SubGHz_Phy Middleware
  - SubGHz_Phy_Per/SubGHz_Phy/App/app_version.h                                 Definition the version of the application
  - SubGHz_Phy_Per/SubGHz_Phy/App/subghz_phy_app.h                              Header of application of the SubGHz_Phy Middleware
  - SubGHz_Phy_Per/SubGHz_Phy/Target/mw_log_conf.h                              Configure (enable/disable) traces
  - SubGHz_Phy_Per/SubGHz_Phy/Target/radio_board_if.h                           Header for Radio interface configuration
  - SubGHz_Phy_Per/SubGHz_Phy/Target/radio_conf.h                               Header of Radio configuration
  - SubGHz_Phy_Per/SubGHz_Phy/Target/timer.h                                    Wrapper to timer server

  - SubGHz_Phy_Per/Core/Src/dma.c                                               This file provides code for the configuration
                                                                                of all the requested memory to memory DMA transfers.
  - SubGHz_Phy_Per/Core/Src/gpio.c                                              This file provides code for the configuration
                                                                                of all used GPIO pins.
  - SubGHz_Phy_Per/Core/Src/main.c                                              : Main program body
  - SubGHz_Phy_Per/Core/Src/rtc.c                                               This file provides code for the configuration
                                                                                of the RTC instances.
  - SubGHz_Phy_Per/Core/Src/stm32wlxx_hal_msp.c                                 This file provides code for the MSP Initialization
                                                                                and de-Initialization codes.
  - SubGHz_Phy_Per/Core/Src/stm32wlxx_it.c                                      Interrupt Service Routines.
  - SubGHz_Phy_Per/Core/Src/stm32_lpm_if.c                                      Low layer function to enter/exit low power modes (stop, sleep)
  - SubGHz_Phy_Per/Core/Src/subghz.c                                            This file provides code for the configuration
                                                                                of the SUBGHZ instances.
  - SubGHz_Phy_Per/Core/Src/system_stm32wlxx.c                                  CMSIS Cortex Device Peripheral Access Layer System Source File
  - SubGHz_Phy_Per/Core/Src/sys_app.c                                           Initializes HW and SW system entities (not related to the radio)
  - SubGHz_Phy_Per/Core/Src/sys_debug.c                                         Configure probes pins RealTime debugging and JTAG/SerialWires for LowPower
  - SubGHz_Phy_Per/Core/Src/timer_if.c                                          Configure RTC Alarm, Tick and Calendar manager
  - SubGHz_Phy_Per/Core/Src/usart.c                                             This file provides code for the configuration
                                                                                of the USART instances.
  - SubGHz_Phy_Per/Core/Src/usart_if.c                                          Configuration of UART driver interface for hyperterminal communication
  - SubGHz_Phy_Per/STM32CubeIDE/Application/User/Core/syscalls.c                STM32CubeIDE Minimal System calls file
  - SubGHz_Phy_Per/STM32CubeIDE/Application/User/Core/sysmem.c                  STM32CubeIDE System Memory calls file
  - SubGHz_Phy_Per/SubGHz_Phy/App/app_subghz_phy.c                              Application of the SubGHz_Phy Middleware
  - SubGHz_Phy_Per/SubGHz_Phy/App/subghz_phy_app.c                              Application of the SubGHz_Phy Middleware
  - SubGHz_Phy_Per/SubGHz_Phy/Target/radio_board_if.c                           This file provides an interface layer between MW and Radio Board


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
             |         Tx Device      |  |    |  |         Rx Device      |
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
  - Open two Terminals, each connected to their respective Tx or Rx Device
  - UART Config = 115200, 8b, 1 stopbit, no parity, no flow control

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
  This SubGHz_Phy project is based on "Advanced template" (MX GUI Application). 
  USER CODE SECTION are filled to end up with "Switch_Fhss_Fsk_Lora" project.

@par How to use it with Azure ThreadX RTOS?
  This example can be combined with Azure ThreadX RTOS via STM32CubeMX. The video tutorial:
  "STM32WL - How to port an existing RF application on Azure ThreadX RTOS"
  is available on https://www.youtube.com/playlist?list=PLnMKNibPkDnE2eaR-ZGM3ZJXadyQLtTpX
  but not sufficient for the complete porting of this example.
  After following the video instructions the user shall open "subghz_phy_app.c" and manually:
  - delete "stm32_seq.h" inclusion
  - delete the line "UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), UTIL_SEQ_RFU, Per_Process);
  - call Per_Process(); function within /* USER CODE App_Main_Thread_Entry_Loop */
  - replace "UTIL_SEQ_SetTask(..) calls" with tx_thread_resume(&App_MainThread);

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
