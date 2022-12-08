/*!
 * \file      LmHandlerTypes.h
 *
 * \brief     Defines the types used by LmHandler
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
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    LmHandlerTypes.h
  * @author  MCD Application Team
  * @brief   Header for Global types used by LoRaMAC Layer handler
  ******************************************************************************
  */
#ifndef __LORAMAC_HANDLER_TYPES_H__
#define __LORAMAC_HANDLER_TYPES_H__

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "LoRaMacInterfaces.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/*!
 *
 */
typedef enum
{
    LORAMAC_HANDLER_ADR_OFF = 0,
    LORAMAC_HANDLER_ADR_ON = !LORAMAC_HANDLER_ADR_OFF
} LmHandlerAdrStates_t;

/*!
 *
 */
typedef enum
{
    LORAMAC_HANDLER_RESET = 0,
    LORAMAC_HANDLER_SET = !LORAMAC_HANDLER_RESET
} LmHandlerFlagStatus_t;

/*!
 *
 */
typedef enum
{
    LORAMAC_HANDLER_ERROR = -1,
    LORAMAC_HANDLER_BUSY_ERROR = -2,
    LORAMAC_HANDLER_NO_NETWORK_JOINED = -3,
    LORAMAC_HANDLER_COMPLIANCE_RUNNING = -4,
    LORAMAC_HANDLER_CRYPTO_ERROR = -5,
    LORAMAC_HANDLER_DUTYCYCLE_RESTRICTED = -6,
    LORAMAC_HANDLER_PAYLOAD_LENGTH_RESTRICTED = -7,
    LORAMAC_HANDLER_NVM_DATA_UP_TO_DATE = -8,
    LORAMAC_HANDLER_SUCCESS = 0
} LmHandlerErrorStatus_t;

/*!
 *
 */
typedef enum
{
    LORAMAC_HANDLER_UNCONFIRMED_MSG = 0,
    LORAMAC_HANDLER_CONFIRMED_MSG = !LORAMAC_HANDLER_UNCONFIRMED_MSG
} LmHandlerMsgTypes_t;

/*!
 *
 */
typedef enum
{
    LORAMAC_HANDLER_FALSE = 0,
    LORAMAC_HANDLER_TRUE = !LORAMAC_HANDLER_FALSE
} LmHandlerBoolean_t;

typedef enum
{
    LORAMAC_HANDLER_BEACON_ACQUIRING,
    LORAMAC_HANDLER_BEACON_LOST,
    LORAMAC_HANDLER_BEACON_RX,
    LORAMAC_HANDLER_BEACON_NRX
} LmHandlerBeaconState_t;

typedef enum
{
    LORAMAC_HANDLER_NVM_RESTORE,
    LORAMAC_HANDLER_NVM_STORE,
} LmHandlerNvmContextStates_t;

typedef enum
{
    LORAMAC_HANDLER_L2_VERSION,
    LORAMAC_HANDLER_REGION_VERSION,
} LmHandlerVersionType_t;
/*!
 * Commissioning parameters
 */
typedef struct CommissioningParams_s
{
    uint8_t DevEui[8];
    uint8_t JoinEui[8];
    uint32_t NetworkId;
    uint32_t DevAddr;
} CommissioningParams_t;

/*!
 * Application data structure
 */
typedef struct LmHandlerAppData_s
{
    uint8_t Port;
    uint8_t BufferSize;
    uint8_t *Buffer;
} LmHandlerAppData_t;

typedef struct LmHandlerRequestParams_s
{
    uint8_t IsMcpsRequest;
    LoRaMacStatus_t Status;
    union
    {
        Mcps_t Mcps;
        Mlme_t Mlme;
    } RequestType_t;
} LmHandlerRequestParams_t;

/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* __LORAMAC_HANDLER_TYPES_H__ */
