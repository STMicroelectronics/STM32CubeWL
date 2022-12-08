/*!
 * \file      LmhpClockSync.h
 *
 * \brief     Implements the LoRa-Alliance clock synchronization package
 *            Specification V1.0.0: https://resources.lora-alliance.org/technical-specifications/lorawan-application-layer-clock-synchronization-specification-v1-0-0
 *            Specification V2.0.0: https://resources.lora-alliance.org/technical-specifications/ts003-2-0-0-application-layer-clock-synchronization
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
 *              (C)2013-2018 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 */
#ifndef __LMHP_CLOCK_SYNC_H__
#define __LMHP_CLOCK_SYNC_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "LmhPackage.h"

/*!
 * Clock sync package identifier.
 *
 * \remark This value must be unique amongst the packages
 */
#define PACKAGE_ID_CLOCK_SYNC                       1

LmhPackage_t *LmhpClockSyncPackageFactory( void );

LmHandlerErrorStatus_t LmhpClockSyncAppTimeReq( void );

#ifdef __cplusplus
}
#endif

#endif /* __LMHP_CLOCK_SYNC_H__ */
