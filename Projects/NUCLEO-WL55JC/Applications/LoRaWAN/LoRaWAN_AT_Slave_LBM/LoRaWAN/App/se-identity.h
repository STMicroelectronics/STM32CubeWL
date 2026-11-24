/*!
 * \file      se-identity.h
 *
 * \brief     Secure Element identity and keys
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
 *              (C)2020 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 */
/**
  ******************************************************************************
  *
  *          Portions COPYRIGHT 2020 STMicroelectronics
  *
  * @file    se-identity.h
  * @author  MCD Application Team
  * @brief   Secure Element identity and keys
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SOFT_SE_IDENTITY_H__
#define __SOFT_SE_IDENTITY_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Includes --------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/

/*!
 ******************************************************************************
 ********************************** WARNING ***********************************
 ******************************************************************************
  The secure-element implementation supports both 1.0.x and 1.1.x LoRaWAN
  versions of the specification.
  Thus it has been decided to use the 1.1.x keys and EUI name definitions.
  The below table shows the names equivalence between versions:
               +---------------------+-------------------------+
               |       1.0.x         |          1.1.x          |
               +=====================+=========================+
               | LORAWAN_DEVICE_EUI  | LORAWAN_DEVICE_EUI      |
               +---------------------+-------------------------+
               | LORAWAN_APP_EUI     | LORAWAN_JOIN_EUI        |
               +---------------------+-------------------------+
               | LORAWAN_GEN_APP_KEY | LORAWAN_APP_KEY         |
               +---------------------+-------------------------+
               | LORAWAN_APP_KEY     | LORAWAN_NWK_KEY         |
               +---------------------+-------------------------+
               | LORAWAN_NWK_S_KEY   | LORAWAN_F_NWK_S_INT_KEY |
               +---------------------+-------------------------+
               | LORAWAN_NWK_S_KEY   | LORAWAN_S_NWK_S_INT_KEY |
               +---------------------+-------------------------+
               | LORAWAN_NWK_S_KEY   | LORAWAN_NWK_S_ENC_KEY   |
               +---------------------+-------------------------+
               | LORAWAN_APP_S_KEY   | LORAWAN_APP_S_KEY       |
               +---------------------+-------------------------+
 ******************************************************************************
 ******************************************************************************
 ******************************************************************************
 */
/*!
 * End-device IEEE EUI (big endian)
 * When set to 00,00,00,00,00,00,00,00 DevEui is automatically set with a value provided by MCU platform
 */
#define LORAWAN_DEVICE_EUI                                 00,00,00,00,00,00,00,00

/*!
 * App/Join server IEEE EUI (big endian)
 */
#define LORAWAN_JOIN_EUI                                   01,01,01,01,01,01,01,01

/*!
 * Device address on the network (big endian)
 * When set to 00,00,00,00 DevAddr is automatically set with a value provided by MCU platform
 */
#define LORAWAN_DEVICE_ADDRESS                             00,00,00,00

/*!
 * Application root key
 */
#define LORAWAN_GEN_APP_KEY                                2B,7E,15,16,28,AE,D2,A6,AB,F7,15,88,09,CF,4F,3C

/*!
 * Network root key
 */
#define LORAWAN_APP_KEY                                    2B,7E,15,16,28,AE,D2,A6,AB,F7,15,88,09,CF,4F,3C

/*!
 * Forwarding Network session key
 */
#define LORAWAN_NWK_S_KEY                                  2B,7E,15,16,28,AE,D2,A6,AB,F7,15,88,09,CF,4F,3C

/*!
 * Application session key
 */
#define LORAWAN_APP_S_KEY                                  2B,7E,15,16,28,AE,D2,A6,AB,F7,15,88,09,CF,4F,3C

/*!
 * Format commissioning keys
 */
#define RAW_TO_INT8A(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) {0x##a,0x##b,0x##c,0x##d,\
                                                       0x##e,0x##f,0x##g,0x##h,\
                                                       0x##i,0x##j,0x##k,0x##l,\
                                                       0x##m,0x##n,0x##o,0x##p}

#define RAW8_TO_INT8A(a,b,c,d) 0x##a##b##c##d
#define RAW32_TO_INT8A(a,b,c,d,e,f,g,h) {0x##a,0x##b,0x##c,0x##d,\
                                         0x##e,0x##f,0x##g,0x##h}

#define FORMAT_KEY(...) RAW_TO_INT8A(__VA_ARGS__)
#define FORMAT8_KEY(...) RAW8_TO_INT8A(__VA_ARGS__)
#define FORMAT32_KEY(...) RAW32_TO_INT8A(__VA_ARGS__)

/* USER CODE BEGIN EC */

/* USER CODE END EC */

#ifdef __cplusplus
}
#endif

#endif  /*  __SOFT_SE_IDENTITY_H__ */
