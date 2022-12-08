/*!
 * \file      NvmDataMgmt.h
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
 *
 * \defgroup  NVMDATAMGMT NVM context management implementation
 *            This module implements the NVM context handling
 * \{
 */
#ifndef __NVMDATAMGMT_H__
#define __NVMDATAMGMT_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum NvmDataErrorStatus_e
{
    NVM_DATA_ERROR = -1,
    NVM_DATA_NO_UPDATED_DATA = -2,
    NVM_DATA_NOT_AVAILABLE = -3,
    NVM_DATA_INCONSISTENT = -4,
    NVM_DATA_DISABLED = -5,
    NVM_DATA_OK = 0

} NvmDataErrorStatus_t;

/*!
 * \brief NVM Management event.
 *
 * \param [in] notifyFlags Bitmap which contains the information about modules that
 *                         changed.
 */
void NvmDataMgmtEvent( uint16_t notifyFlags );

/*!
 * \brief Check the NVM Flag status and LoRaMAC state
 *
 * \retval status NVM_DATA_OK, NVM_DATA_DISABLED, NVM_DATA_NOT_AVAILABLE, NVM_DATA_NO_UPDATED_DATA
 */
int32_t NvmDataMgmtStoreBegin( void );

/*!
 * \brief Clean the NVM Flag status and resume LoRaMAC process
 *
 * \retval status NVM_DATA_OK, NVM_DATA_DISABLED
 */
int32_t NvmDataMgmtStoreEnd( void );

/*! \} defgroup NVMDATAMGMT */

#ifdef __cplusplus
}
#endif

#endif /* __NVMDATAMGMT_H__ */
