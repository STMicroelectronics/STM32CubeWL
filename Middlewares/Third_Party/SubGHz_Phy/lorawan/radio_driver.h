/*!
 * \file      radio_driver.h
 *
 * \brief     STM32WL_SubGHz_Phy driver implementation
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
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    radio_driver.h
  * @author  MCD Application Team
  * @brief   Header for driver radio interface
  ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RADIO_DRIVER_H__
#define __RADIO_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include "radio_conf.h"

#if defined( SX128X )
#include "sx128x.h"
#include "sx128x_regs.h"
#elif defined( SX126X )
#include "sx126x.h"
#include "sx126x_regs.h"
#elif defined( LR11XX )
#include "lr11xx.h"
#include "lr11xx_regs.h"
#elif defined( SX127X )
#include "sx127x.h"
#include "sx127x_regs.h"
#endif

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/


/*!
 * ============================================================================
 * Public functions prototypes
 * ============================================================================
 */





#ifdef __cplusplus
}
#endif

#endif // __RADIO_DRIVER_H__
