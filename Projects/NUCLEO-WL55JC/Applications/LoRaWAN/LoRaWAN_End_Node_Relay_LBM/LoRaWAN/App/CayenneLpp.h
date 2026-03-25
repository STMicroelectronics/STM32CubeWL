/*!
  * @file      CayenneLpp.h
  *
  * @brief     Implements the Cayenne Low Power Protocol
  *
  * @copyright Revised BSD License, see section \ref LICENSE.
  *
  * @code
  *                ______                              _
  *               / _____)             _              | |
  *              ( (____  _____ ____ _| |_ _____  ____| |__
  *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
  *               _____) ) ____| | | || |_| ____( (___| | | |
  *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
  *              (C)2013-2018 Semtech
  *
  * @endcode
  *
  * @author    Miguel Luis ( Semtech )
  */
#ifndef __CAYENNE_LPP_H__
#define __CAYENNE_LPP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void CayenneLppInit(void);

void CayenneLppReset(void);

uint8_t CayenneLppGetSize(void);

uint8_t *CayenneLppGetBuffer(void);

uint8_t CayenneLppCopy(uint8_t *buffer);

uint8_t CayenneLppAddDigitalInput(uint8_t channel, uint8_t value);

uint8_t CayenneLppAddDigitalOutput(uint8_t channel, uint8_t value);

uint8_t CayenneLppAddAnalogInput( uint8_t channel, float value );

uint8_t CayenneLppAddAnalogOutput( uint8_t channel, float value );

uint8_t CayenneLppAddLuminosity(uint8_t channel, uint16_t lux);

uint8_t CayenneLppAddPresence(uint8_t channel, uint8_t value);

uint8_t CayenneLppAddTemperature( uint8_t channel, float celsius );

uint8_t CayenneLppAddRelativeHumidity( uint8_t channel, float rh );

uint8_t CayenneLppAddAccelerometer( uint8_t channel, float x, float y, float z );

uint8_t CayenneLppAddBarometricPressure( uint8_t channel, float hpa );

uint8_t CayenneLppAddGyrometer( uint8_t channel, float x, float y, float z );

uint8_t CayenneLppAddGps( uint8_t channel, float latitude, float longitude, float meters );

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __CAYENNE_LPP_H__ */
