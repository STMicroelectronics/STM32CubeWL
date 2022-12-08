/*!
 * \file      LmhpRemoteMcastSetup.h
 *
 * \brief     Implements the LoRa-Alliance remote multicast setup package
 *            Specification V1.0.0: https://resources.lora-alliance.org/technical-specifications/lorawan-remote-multicast-setup-specification-v1-0-0
 *            Specification V2.0.0: https://resources.lora-alliance.org/technical-specifications/ts005-2-0-0-remote-multicast-setup
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
#ifndef __LMHP_REMOTE_MCAST_SETUP_H__
#define __LMHP_REMOTE_MCAST_SETUP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "LmhPackage.h"

/*!
 * Remote multicast setup package identifier.
 *
 * \remark This value must be unique amongst the packages
 */
#define PACKAGE_ID_REMOTE_MCAST_SETUP               2

LmhPackage_t *LmhpRemoteMcastSetupPackageFactory( void );

#ifdef __cplusplus
}
#endif

#endif /* __LMHP_REMOTE_MCAST_SETUP_H__ */
