# STM32CubeWL MCU Firmware Package

![tag](https://img.shields.io/badge/tag-v1.7.0-brightgreen.svg)
[![release note](https://img.shields.io/badge/release_note-view_html-gold.svg)](https://htmlpreview.github.io/?https://github.com/STMicroelectronics/STM32CubeWL/blob/main/Release_Notes.html)
[![offer](https://img.shields.io/badge/stm32cube_mcu_offer-browse-orange.svg)](https://github.com/STMicroelectronics/STM32Cube_MCU_Overall_Offer)

> [!IMPORTANT]
> This repository contains **submodules**. Please refer to the ["Get software"](README.md#get-software) section for more details.

## Overview

This repository provides the **STM32CubeWL** MCU firmware, including **all** software components (Drivers, Middleware, Projects, Utilities).

> [!NOTE]
> ## Some middleware libraries and projects are unavailable in this repository
> 
> In this repository, the middleware libraries listed below **along with** [this](Projects/README.md#list-of-unavailable-projects) list of projects (demos, applications, and examples) using them, are **not available** as they (the middleware libraries) are subject to some restrictive license terms requiring the user's approval via a "click thru" procedure.
> * `./Middlewares/ST/STM32_Key_Management_Services`
> * `./Middlewares/ST/STM32_Secure_Engine`
> * `./Middlewares/Third_Party/Sigfox`
> 
> If needed, they can be found inside the full firmware package available on our website `st.com` and downloadable from [here](https://www.st.com/en/embedded-software/stm32cubewl.html#get-software). You will be prompted to login or to register in case you have no account.

## Get software

This repository contains **submodules**.
* To **clone** it, run the command below
```
git clone --recursive https://github.com/STMicroelectronics/STM32CubeWL.git
```
* To **pull** the latest updates, run the commands below from the root of the repository
```
git pull; git submodule update --init --recursive
```

> [!CAUTION]
> If the "Download ZIP" option is used instead of the `git clone` command, then the different **submodules** have to be collected and added **manually**.

> [!NOTE]
> The latest version of this firmware available on GitHub may be **ahead** of the one available on [st.com](https://www.st.com/en/microcontrollers-microprocessors/stm32-32-bit-arm-cortex-mcus.html) or via [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html). This is due to the **rolling release**. Please refer to [this](https://github.com/STMicroelectronics/STM32Cube_MCU_Overall_Offer/discussions/21) post for more details.

## Feedback and contributions

We value and appreciate feedback and contributions from the community. Please refer to the [CONTRIBUTING.md](CONTRIBUTING.md) file for details on how to participate effectively.