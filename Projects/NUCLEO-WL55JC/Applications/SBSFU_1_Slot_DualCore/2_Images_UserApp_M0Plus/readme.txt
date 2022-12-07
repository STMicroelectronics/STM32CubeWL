/**
  @page 2_Images_UserApp_M0Plus / Secure Firmware Update - M0+ User Application Demo

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @brief   This application shows a User Application
  ******************************************************************************
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file in
  * the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Application Description

This application demonstrates firmware download capabilities and provides a set of functions to test the active
protections offered by Secure Boot and Secure Engine.
It also demonstrates key management services through basic examples and specific boot
mechanism (secure firmware update decryption, verification...)
A terminal connected with the board via VCOM is needed to communicate with the board and to select which feature
to demonstrate.

For more details, refer to UM2262 "Getting started with SBSFU - software expansion for STM32Cube" available from
the STMicroelectronics microcontroller website www.st.com.

@par Keywords

Security, KMS, SBSFU

@par Directory contents

   - 2_Images_UserApp_M0Plus/Src/com.c                    Communication module file
   - 2_Images_UserApp_M0Plus/Src/common.c                 Common module file
   - 2_Images_UserApp_M0Plus/Src/flash_if.c               Flash interface file
   - 2_Images_UserApp_M0Plus/Src/fw_update_app.c          Firmware update application
   - 2_Images_UserApp_M0Plus/Src/kms_object_update_app.c  KMS Blob update application
   - 2_Images_UserApp_M0Plus/Src/main.c                   Main program
   - 2_Images_UserApp_M0Plus/Src/mpu.c                    Secure Engine isolation with MPU (when no Firewall is available)
   - 2_Images_UserApp_M0Plus/Src/se_user_code.c           Call user defined services running in Secure Engine
   - 2_Images_UserApp_M0Plus/Src/stm32wlxx_it.c           Interrupt handlers
   - 2_Images_UserApp_M0Plus/Src/system_stm32wlxx.c       STM32 system file
   - 2_Images_UserApp_M0Plus/Src/test_protections.c       Protection test
   - 2_Images_UserApp_M0Plus/Src/tkms_app.c               tKMS tests
   - 2_Images_UserApp_M0Plus/Src/tkms_app_derive_key.c    tKMS key derivation tests
   - 2_Images_UserApp_M0Plus/Src/tkms_app_encrypt_decrypt.c tKMS encryption/decryption tests
   - 2_Images_UserApp_M0Plus/Src/tkms_app_sign_verify.c   tKMS signature/verification tests
   - 2_Images_UserApp_M0Plus/Src/ymodem.c                 Ymodem communication module
   - 2_Images_UserApp_M0Plus/Inc/com.h                    Header for com.c file
   - 2_Images_UserApp_M0Plus/Inc/common.h                 Header for common.c file
   - 2_Images_UserApp_M0Plus/Inc/flash_if.h               Header for flash_if.c file
   - 2_Images_UserApp_M0Plus/Inc/fw_update_app.h          Header for fw_update_app.c file
   - 2_Images_UserApp_M0Plus/Inc/main.h                   Header for main.c file
   - 2_Images_UserApp_M0Plus/Inc/mpu.h                    Header file for mpu.c
   - 2_Images_UserApp_M0Plus/Inc/se_user_code.h           Header file for se_user_code.c
   - 2_Images_UserApp_M0Plus/Inc/sfu_app_new_image.h      Definition for the new blob storage and installation
   - 2_Images_UserApp_M0Plus/Inc/stm32wlxx_hal_conf.h     HAL configuration file
   - 2_Images_UserApp_M0Plus/Inc/stm32wlxx_it.h           Header for stm32wlxx_it.c file
   - 2_Images_UserApp_M0Plus/Inc/stm32wlxx_nucleo_conf.h  BSP configuration file
   - 2_Images_UserApp_M0Plus/Inc/test_protections.h       Header for test_protections.c file
   - 2_Images_UserApp_M0Plus/Inc/tkms_app.h               Header for tkms_app.c file
   - 2_Images_UserApp_M0Plus/Inc/ymodem.h                 Header for ymodem.c file

@par Hardware and Software environment

  - This example runs on STM32WL55xx devices.
  - This example has been tested with NUCLEO-WL55JC RevC board and can be
    easily tailored to any other supported device and development board.
  - An up-to-date version of ST-LINK firmware is required. Upgrading ST-LINK firmware
    is a feature provided by STM32Cube programmer available on www.st.com.
  - This example is based on se_interface_application.o module exported by SBSFU project.
  - This example needs a terminal emulator.
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

A known limitation of this integration occurs when you update the firmware version (parameter of postbuild.bat script).
The IDE does not track this update so you need to force the rebuild of the project manually.

@par How to use it ?

Refer to SBSFU readme and follow steps by steps instructions.

Once executed, this user application gives access to a menu which allows:
  1 - to download a new firmware : feature not supported in 1 firmware image example
  2 - to test protections (GTZC isolation, HDP, WRP, IWDG, TAMPER, MPU isolation)
  3 - to demonstrate how to call user defined services running in Secure Engine
  4 - to provide access to multiple images feature (not supported)
  5 - to validate a firmware image at first start-up (not supported)
  a - to demonstrate usage of key management services

1. Pressing 1 allows to download a new firmware.
Feature not supported in 1 firmware image example

2. Pressing 2 allows to test protections.

 - CORRUPT IMAGE test (#1): causes a signature verification failure at next boot
 - Isolation tests (#2, #3): cause a reset trying to access protected code or data (either in RAM or FLASH)
 - Secure user memory protection test (#4): cause an error trying to access protected code
 - WRP test (#5): causes an error trying to erase write protected code
 - IWDG test (#6): causes a reset simulating a deadlock by not refreshing the watchdog.
 - TAMPER test (#7): causes a reset if a tamper event is detected. In order to generate a tamper event,
   user has to connect PC13 (CN7.23) to GND (It may be enough to put your finger close to PC13 (CN7.23)).
   By default, the feature is disabled (see the SBSFU readme.txt).
 - GTZC tests (#8): causes an error trying to access to protected IP, code or data (either in RAM or FLASH)
 - KMS data storage test (#9): causes an error trying to access KMS data storage (NVM data)

3. Pressing 3 allows to call user defined services running in Secure Engine.
 - As an example, SE_APP_GetActiveFwInfo (#1, #2) service is called in order to display the firmware information
   located in the protected area such as version and size.
 - The IRQ handling tests (#a, #b) show how to disable and to enable IRQ services.
   Each interface must be called once (no consecutive calls of the same interface).
   Presses on the User Button generate interrupts (handled or not depending on whether the interrupts are enabled).

4. This menu is dedicated to multiple images feature.
Feature not available as there is no firmware download area.

5. This menu is dedicated to image validation.
Feature available under ENABLE_IMAGE_STATE_HANDLING compilation switch, not available in this example.

a. Pressing a allows to enter specific key management services test menu.
With this menu, you should be able to try encryption/decryption, signature/verification or key derivation mechanisms.


Note1 : There is only 1 active slot configured in this example.
Note2 : for Linux users Minicom can be used but to do so you need to compile the UserApp project with the MINICOM_YMODEM
        switch enabled (ymodem.h)


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
