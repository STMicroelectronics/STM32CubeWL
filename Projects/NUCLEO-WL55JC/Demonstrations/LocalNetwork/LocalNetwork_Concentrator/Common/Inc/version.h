/**
  ******************************************************************************
  * @file    version.h
  * @author  MCD Application Team
  * @brief   Header with version of STM32WL Sensor Concentrator Demo.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#ifndef __VERSION_H__
#define __VERSION_H__

/**
  * @brief Major version indicates major changes in behavior.
  * Used when regions/subregions are changed or incompatible behavior.
  * Only 0 ~ 7 fit inside packets.
  */
#define VERSION_MAJOR          1

/**
  * @brief Minor version indicated minor changes in behavior.
  * Used to change sensor data format or compatible changes.
  * Only 0 ~ 31 fit inside packets.
  */
#define VERSION_MINOR          0

#define VERSION_STR_CONVERT(a) VERSION_STR_HELPER(a)    /*Preprocessor trick to convert constant to string*/
#define VERSION_STR_HELPER(a)  #a

#define VERSION_STR            "v" VERSION_STR_CONVERT(VERSION_MAJOR) ":" VERSION_STR_CONVERT(VERSION_MINOR)

#endif /* __VERSION_H__ */
