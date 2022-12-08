/*!
 * \file      NvmDataMgmt.c
 *
 * \brief     NVM context management implementation
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
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    NvmDataMgmt.c
  * @author  MCD Application Team
  * @brief   NVM context management implementation
  ******************************************************************************
  */
#include "utilities.h"
#include "LoRaMac.h"
#include "NvmDataMgmt.h"
/*!
 * Enables/Disables the context storage management storage.
 * Must be enabled for LoRaWAN 1.0.4 or later.
 */
#ifndef CONTEXT_MANAGEMENT_ENABLED
#define CONTEXT_MANAGEMENT_ENABLED         0
#endif /* CONTEXT_MANAGEMENT_ENABLED */

#if ( CONTEXT_MANAGEMENT_ENABLED == 1 )
static uint16_t NvmNotifyFlags = 0;
#endif /* CONTEXT_MANAGEMENT_ENABLED == 1 */

void NvmDataMgmtEvent( uint16_t notifyFlags )
{
#if ( CONTEXT_MANAGEMENT_ENABLED == 1 )
    NvmNotifyFlags |= notifyFlags;
#endif /* CONTEXT_MANAGEMENT_ENABLED == 1 */
}

int32_t NvmDataMgmtStoreBegin( void )
{
#if ( CONTEXT_MANAGEMENT_ENABLED == 1 )
    /* Input checks */
    if( NvmNotifyFlags == LORAMAC_NVM_NOTIFY_FLAG_NONE )
    {
        /* There was no update. */
        return NVM_DATA_NO_UPDATED_DATA;
    }
    if( LoRaMacStop( ) != LORAMAC_STATUS_OK )
    {
        return NVM_DATA_NOT_AVAILABLE;
    }
    return NVM_DATA_OK;
#else
    return NVM_DATA_DISABLED;
#endif /* CONTEXT_MANAGEMENT_ENABLED */
}

int32_t NvmDataMgmtStoreEnd( void )
{
#if ( CONTEXT_MANAGEMENT_ENABLED == 1 )
    /* Reset notification flags */
    NvmNotifyFlags = LORAMAC_NVM_NOTIFY_FLAG_NONE;

    /* Resume LoRaMac */
    LoRaMacStart( );
    return NVM_DATA_OK;
#else
    return NVM_DATA_DISABLED;
#endif /* CONTEXT_MANAGEMENT_ENABLED */
}
