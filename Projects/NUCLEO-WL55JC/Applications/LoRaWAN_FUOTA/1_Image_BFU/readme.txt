/**
  @page 1_Image_BFU / Boot and Firmware Update Demo Application

  @verbatim
  ******************************************************************************
  * @file    readme.txt
  * @brief   This application shows Boot and Firmware Update example.
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

The Boot (B) and Firmware Update (FU) solution allows the update of the STM32 microcontroller built-in
program with new firmware versions, adding new features and correcting potential issues. The update process is performed
in a secure way to prevent unauthorized updates.
In addition, Secure Boot (Root of Trust services) checks and activates the STM32 security mechanisms (if any), and checks the
authenticity and integrity of user application code before every execution to ensure that invalid or malicious code
cannot be run. The Firmware Update application receives the encrypted firmware image, decrypts it, then checks
the authenticity and integrity of the code before installing it.

The key management services provide cryptographic services to the user application through the pkcs11 APIs
(KEY ID based APIs). User application keys are stored and can be updated in a secure way (authenticity check, data decryption and
data integrity check).
This example handles 2 firmware images located in 1 active slot and 1 download area in FLASH allowing to demonstrate
features like:
   - download a new firmware image from the application
   - resume firmware update procedure in case of power off during installation
   - rollback on the previous firmware if the new active image is not validated (selft-tests KO) at first start-up.

For more details, refer to UM2262 "Getting started with SBSFU - software expansion for STM32Cube" available from the
STMicroelectronics microcontroller website www.st.com.

@par Keywords

Security, SBSFU, MPU

@par Directory contents

   - 1_Image_BFU/Core/Src/main.c                           Main application file
   - 1_Image_BFU/Core/Src/stm32wlxx_hal_msp.c              HAL MSP module
   - 1_Image_BFU/Core/Src/stm32wlxx_it.c                   STM32 interrupt handlers
   - 1_Image_BFU/Core/Inc/app_hw.h                         Hardware definitions for application
   - 1_Image_BFU/Core/Inc/main.h                           Header file for main.c
   - 1_Image_BFU/Core/Inc/stm32wlxx_hal_conf.h             HAL configuration file
   - 1_Image_BFU/Core/Inc/stm32wlxx_it.h                   Header file for stm32wlxx_it.c
   - 1_Image_BFU/Core/Inc/stm32wlxx_nucleo_conf.h          BSP configuration file
   - 1_Image_BFU/BFU/App/sfu_boot.c                      Secure Boot (SB): entry/exit points and state machine
   - 1_Image_BFU/BFU/App/sfu_com_loader.c                BFU communication module: local loader part
   - 1_Image_BFU/BFU/App/sfu_com_trace.c                 BFU communication module: trace part
   - 1_Image_BFU/BFU/App/sfu_error.c                     BFU errors management
   - 1_Image_BFU/BFU/App/sfu_fwimg_common.c              BFU image handling: common functionalities/services
   - 1_Image_BFU/BFU/App/sfu_fwimg_swap.c                BFU image handling: FW upgrade without swap area services
   - 1_Image_BFU/BFU/App/sfu_fwimg_no_swap.c             BFU image handling: FW upgrade with swap area services
   - 1_Image_BFU/BFU/App/sfu_interface_crypto_scheme.c   Crypto scheme implementation based on KMS API
   - 1_Image_BFU/BFU/App/sfu_kms.c                       Key management service blob importation handling
   - 1_Image_BFU/BFU/App/sfu_loader.c                    BFU Local Loader
   - 1_Image_BFU/BFU/App/sfu_new_image.c                 BFU image handling: manage the new firmware image storage and installation
   - 1_Image_BFU/BFU/App/sfu_test.c                      BFU security protection automatic test
   - 1_Image_BFU/BFU/App/app_sfu.h                       Software configuration of BFU application
   - 1_Image_BFU/BFU/App/sfu_boot.h                      Header file for sfu_boot.c
   - 1_Image_BFU/BFU/App/sfu_com_loader.h                Header file for sfu_com_loader.c
   - 1_Image_BFU/BFU/App/sfu_com_trace.h                 Header file for sfu_com_trace.c
   - 1_Image_BFU/BFU/App/sfu_def.h                       General definition for BFU application
   - 1_Image_BFU/BFU/App/sfu_error.h                     Header file for sfu_error.c file
   - 1_Image_BFU/BFU/App/sfu_fsm_states.h                BFU FSM states definitions
   - 1_Image_BFU/BFU/App/sfu_fwimg_internal.h            Internal definitions for firmware image handling (sfu_fwimg_core.c and sfu_fwimg_services.c)
   - 1_Image_BFU/BFU/App/sfu_fwimg_regions.h             FLASH regions definitions for image handling
   - 1_Image_BFU/BFU/App/sfu_fwimg_services.h            Header file for sfu_fwimg_services.c
   - 1_Image_BFU/BFU/App/sfu_interface_crypto_scheme.h   Header file for sfu_interface_crypto_scheme.h
   - 1_Image_BFU/BFU/App/sfu_kms.h                       Header file for sfu_kms.c
   - 1_Image_BFU/BFU/App/sfu_loader.h                    Header file for sfu_loader.c
   - 1_Image_BFU/BFU/App/sfu_new_image.h                 Header file for sfu_new_image.c
   - 1_Image_BFU/BFU/App/sfu_test.h                      Header file for sfu_test.c
   - 1_Image_BFU/BFU/App/sfu_trace.h                     Header file for sfu_trace.c
   - 1_Image_BFU/BFU/Target/sfu_low_level.c              BFU general low level interface
   - 1_Image_BFU/BFU/Target/sfu_low_level_flash.c        BFU flash low level interface (wrapper)
   - 1_Image_BFU/BFU/Target/sfu_low_level_flash_int.c    BFU internal flash low level interface
   - 1_Image_BFU/BFU/Target/sfu_low_level_flash_ext.c    BFU external flash low level interface
   - 1_Image_BFU/BFU/Target/sfu_low_level_security.c     BFU security low level interface
   - 1_Image_BFU/BFU/Target/sfu_low_level.h              Header file for general low level interface
   - 1_Image_BFU/BFU/Target/sfu_low_level_flash.h        Header file for flash low level interface (wrapper)
   - 1_Image_BFU/BFU/Target/sfu_low_level_flash_int.h    Header file for internal flash low level interface
   - 1_Image_BFU/BFU/Target/sfu_low_level_flash_ext.h    Header file for external flash low level interface
   - 1_Image_BFU/BFU/Target/sfu_low_level_security.h     Header file for security low level interface

@par Hardware and Software environment

   - This example runs on STM32WL55xx devices.
   - This example has been tested with NUCLEO-WL55JC RevC board and can be easily tailored to any other supported device and
     development board.
   - An up-to-date version of ST-LINK firmware is required. Upgrading ST-LINK firmware is a feature provided by
     STM32Cube programmer available on www.st.com.
   - This example is linked with SE_Core binary generated by Secure Engine Core binary generation project.
   - This example needs a terminal emulator.
   - By default, SBSFU is configured in development mode. To switch in production mode, refer to AN5056 "Integration
     guide - software expansion for STM32Cube" available from the STMicroelectronics microcontroller website
     www.st.com.
   - Microsoft Windows has a limitation whereby paths to files and directories cannot be longer than 256 characters.
     Paths to files exceeding that limits cause tools (e.g. compilers, shell scripts) to fail reading from or writing
     to such files.
     As a workaround, it is advised to use the subst.exe command from within a command prompt to set up a local drive
     out of an existing directory on the hard drive, such as:
     C:\> subst X: <PATH_TO_CUBEFW>\Firmware

@par How to use it ?

Several steps to run BFU application :

1. Compile projects in the following order. This is mandatory as each project requests some objects generated by the
   compilation of the previous one:
   - 1_Image_SECoreBin (see also SECoreBin/readme.txt)
   - 1_Image_BFU
   - 1_Image_UserApp (see also UserApp/readme.txt)

2. Before loading BFU image into the target, please ensure with STM32CubeProgrammer available on www.st.com that the
   following are valid for the device:
   - RDP Level 0
   - Write Protection disabled on all FLASH pages
   - Chip has been erased

3. Use a terminal emulator (Tera Term for example, open source free software terminal emulator that can be downloaded
   from https://osdn.net/projects/ttssh2/) for UART connection with the board.
   Support of YMODEM protocol is required. Serial port configuration should be :
   - Baud rate = 115200
   - Data = 8 bits
   - Parity = none
   - Stop = 1 bit
   - Flow control = none

4. Load BFU image into target memory with your preferred toolchain or STM32CubeProgrammer.

5. Once the BFU software is downloaded, power cycle the board (unplug/plug USB cable) : the BFU
   application starts and configures the security mechanisms if some of them are enabled.

6. [If RDP is enabled] Power cycle the board a second time (unplug/plug the USB cable): the BFU application starts with
   the configured securities turned on and the Tera Term connection is possible.
   Caution: Make sure to use an up-to-date version of ST-LINK firmware else BFU may not start.

7. At startup (Power On or Reset button pushed) welcome information are displayed on terminal emulator.
   Green LED blinks every 3 seconds when a local download is waited.
   Green LED blinks every 250 ms in case of error in option bytes configuration.

8. Send the user encrypted firmware file (\2_Slots\1_Image_UserApp\Binary\UserApp.sfb) with Tera Term by using menu
   "File > Transfer > YMODEM > Send..."

Note1 : Press User push-button (B1) at reset to force a local download if an application is already installed.
Note2 : The following steps enable the protections disabled by default:
        - Comment "SECBOOT_DISABLE_SECURITY_IPS" in app_sfu.h. This will enable RDP, WRP, DAP, DMA, IWDG protections
          as well as attack surface reduction with MPU.
        - As TAMPER detection can be very sensitive, the protection remains disabled by default.
          Uncomment "SFU_TAMPER_PROTECT_ENABLE" in app_sfu.h to enable it.
        These features achieve an attack surface reduction but will not make it possible to protect secrets.
Note3 : for Linux users Minicom can be used but to do so you need to compile the BFU project with the MINICOM_YMODEM
        switch enabled (app_sfu.h)

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
