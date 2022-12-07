/**
  @page 2_Images_UserApp_M4 / Secure Firmware Update - M4 User Application Demo

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @brief   This application shows a User Application
  ******************************************************************************
  *
  * Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Application Description

This application demonstrates firmware download capabilities.

This very simple user application makes the blue LED blink every 100 ms.

@par Keywords

Security

@par Directory contents


   - 2_Images_UserApp_M4/Src/main.c                   Main program
   - 2_Images_UserApp_M4/Src/stm32wlxx_it.c           Interrupt handlers
   - 2_Images_UserApp_M4/Src/system_stm32wlxx.c       STM32 system file
   - 2_Images_UserApp_M4/Inc/main.h                   Header for main.c file
   - 2_Images_UserApp_M4/Inc/stm32wlxx_hal_conf.h     HAL configuration file
   - 2_Images_UserApp_M4/Inc/stm32wlxx_it.h           Header for stm32wlxx_it.c file
   - 2_Images_UserApp_M4/Inc/stm32wlxx_nucleo_conf.h  BSP configuration file

@par Hardware and Software environment

  - This example runs on STM32WL55xx devices.
  - This example has been tested with NUCLEO-WL55JC RevC board and can be
    easily tailored to any other supported device and development board.
  - An up-to-date version of ST-LINK firmware is required. Upgrading ST-LINK firmware
    is a feature provided by STM32Cube programmer available on www.st.com.
  - Microsoft Windows has a limitation whereby paths to files and directories cannot
    be longer than 256 characters. Paths to files exceeding that limits cause tools (e.g. compilers,
    shell scripts) to fail reading from or writing to such files.
    As a workaround, it is advised to use the subst.exe command from within a command prompt to set
    up a local drive out of an existing directory on the hard drive, such as:
    C:\> subst X: <PATH_TO_CUBEFW>\Firmware

@par IDE postbuild script

In order to ease the development process, a postbuild script ("postbuild.bat") is integrated in each IDE project.
This postbuild script:
 - is generated when compiling the Secure Engine Core project,
 - prepares the firmware image of the user application to be installed in the device.
 - prepares the new portion of firmware image: part of firmware image which changed vs a reference firmware image.
   The reference firmware image is provided to the postbuild script by manually copying the UserApp.bin of the running
   application into RefUserApp.bin (in folder 2_Images_UserApp_M4/IDE).
 - generates a big binary including M4 SB, M0+ SBSFU, M0+ UserApp, M4 UserApp.

   As an example, once original user application has been built, downloaded and installed, you may create the
   RefUserApp.bin, then change the LED blinking period in source file main.c, then build again the user
   application.
   At post processing stage, the portion of changed code will be identified as a difference vs
   the reference binary file and a PartialUserApp.sfb will be generated in 2_Images_UserApp_M4/Binary folder.

A known limitation of this integration occurs when you update the firmware version (parameter of
postbuild.bat script).
The IDE does not track this update so you need to force the rebuild of the project manually.

@par How to use it ?

Refer to SBSFU readme and follow steps by steps instructions.

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
