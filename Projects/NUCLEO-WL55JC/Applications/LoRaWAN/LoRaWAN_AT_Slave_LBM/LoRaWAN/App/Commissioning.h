/*!
 * \file      Commissioning.h
 *
 * \brief     End-device commissioning parameters
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2020 Semtech
 *
 * \endcode
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    Commissioning.h
  * @author  MCD Application Team
  * @brief   End-device commissioning parameters
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COMMISSIONING_H__
#define __COMMISSIONING_H__

/*!
 ******************************************************************************
 ********************************** WARNING ***********************************
 ******************************************************************************

 The LoRaWAN AES128 keys are stored and provisioned on secure-elements.

 This project provides a software emulated secure-element.
 The LoRaWAN AES128 keys SHALL be updated under
 src/peripherals/<secure-element name>-se\se-identity.h file.

 ******************************************************************************
 ******************************************************************************
 ******************************************************************************
 */
#include "se-identity.h"
#include "lorawan_version.h"

/* USER CODE BEGIN EC1 */

/* USER CODE END EC1 */

/* USER CODE BEGIN EC2 */

/* USER CODE END EC2 */

#endif /* __COMMISSIONING_H__ */
