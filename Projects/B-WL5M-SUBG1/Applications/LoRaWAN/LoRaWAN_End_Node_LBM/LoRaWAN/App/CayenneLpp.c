/*!
  * @file      CayenneLpp.c
  *
  * @brief     Implements the Cayenne Low Power Protocol
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
/* Includes ------------------------------------------------------------------*/
#include "stm32_mem.h"
#include "CayenneLpp.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define CAYENNE_LPP_MAXBUFFER_SIZE                  242
#define LPP_DIGITAL_INPUT       0       /* 1 byte */
#define LPP_DIGITAL_OUTPUT      1       /* 1 byte */
#define LPP_ANALOG_INPUT        2       /* 2 bytes, 0.01 signed */
#define LPP_ANALOG_OUTPUT       3       /* 2 bytes, 0.01 signed */
#define LPP_LUMINOSITY          101     /* 2 bytes, 1 lux unsigned */
#define LPP_PRESENCE            102     /* 1 byte, 1 */
#define LPP_TEMPERATURE         103     /* 2 bytes, 0.1 Celsius degrees signed */
#define LPP_RELATIVE_HUMIDITY   104     /* 1 byte, 0.5% unsigned */
#define LPP_ACCELEROMETER       113     /* 2 bytes per axis, 0.001G */
#define LPP_BAROMETRIC_PRESSURE 115     /* 2 bytes 0.1 hPa Unsigned */
#define LPP_GYROMETER           134     /* 2 bytes per axis, 0.01 degrees/s */
#define LPP_GPS                 136     /* 3 byte lon/lat 0.0001 degrees, 3 bytes alt 0.01m */

/* Data ID + Data Type + Data Size */
#define LPP_DIGITAL_INPUT_SIZE       3
#define LPP_DIGITAL_OUTPUT_SIZE      3
#define LPP_ANALOG_INPUT_SIZE        4
#define LPP_ANALOG_OUTPUT_SIZE       4
#define LPP_LUMINOSITY_SIZE          4
#define LPP_PRESENCE_SIZE            3
#define LPP_TEMPERATURE_SIZE         4
#define LPP_RELATIVE_HUMIDITY_SIZE   3
#define LPP_ACCELEROMETER_SIZE       8
#define LPP_BAROMETRIC_PRESSURE_SIZE 4
#define LPP_GYROMETER_SIZE           8
#define LPP_GPS_SIZE                 11

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static uint8_t CayenneLppBuffer[CAYENNE_LPP_MAXBUFFER_SIZE];
static uint8_t CayenneLppCursor = 0;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
void CayenneLppInit(void)
{
  CayenneLppCursor = 0;
  /* USER CODE BEGIN CayenneLppCursor */

  /* USER CODE END CayenneLppCursor */
}

void CayenneLppReset(void)
{
  CayenneLppCursor = 0;
  /* USER CODE BEGIN CayenneLppReset */

  /* USER CODE END CayenneLppReset */
}

uint8_t CayenneLppGetSize(void)
{
  /* USER CODE BEGIN CayenneLppGetSize */

  /* USER CODE END CayenneLppGetSize */
  return CayenneLppCursor;
}

uint8_t *CayenneLppGetBuffer(void)
{
  /* USER CODE BEGIN CayenneLppGetBuffer */

  /* USER CODE END CayenneLppGetBuffer */
  return CayenneLppBuffer;
}

uint8_t CayenneLppCopy(uint8_t *dst)
{
  /* USER CODE BEGIN CayenneLppCopy_1 */

  /* USER CODE END CayenneLppCopy_1 */
  UTIL_MEM_cpy_8(dst, CayenneLppBuffer, CayenneLppCursor);
  /* USER CODE BEGIN CayenneLppCopy_2 */

  /* USER CODE END CayenneLppCopy_2 */
  return CayenneLppCursor;
}

uint8_t CayenneLppAddDigitalInput(uint8_t channel, uint8_t value)
{
  /* USER CODE BEGIN CayenneLppAddDigitalInput_1 */

  /* USER CODE END CayenneLppAddDigitalInput_1 */
  if ((CayenneLppCursor + LPP_DIGITAL_INPUT_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_DIGITAL_INPUT;
  CayenneLppBuffer[CayenneLppCursor++] = value;
  /* USER CODE BEGIN CayenneLppAddDigitalInput_2 */

  /* USER CODE END CayenneLppAddDigitalInput_2 */
  return CayenneLppCursor;
}

uint8_t CayenneLppAddDigitalOutput(uint8_t channel, uint8_t value)
{
  /* USER CODE BEGIN CayenneLppAddDigitalOutput_1 */

  /* USER CODE END CayenneLppAddDigitalOutput_1 */
  if ((CayenneLppCursor + LPP_DIGITAL_OUTPUT_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_DIGITAL_OUTPUT;
  CayenneLppBuffer[CayenneLppCursor++] = value;
  /* USER CODE BEGIN CayenneLppAddDigitalOutput_2 */

  /* USER CODE END CayenneLppAddDigitalOutput_2 */
  return CayenneLppCursor;
}

uint8_t CayenneLppAddAnalogInput(uint8_t channel, float value)
{
  /* USER CODE BEGIN CayenneLppAddAnalogInput_1 */

  /* USER CODE END CayenneLppAddAnalogInput_1 */
  if ((CayenneLppCursor + LPP_ANALOG_INPUT_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }

  int16_t val = (int16_t)(value * 100);
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_ANALOG_INPUT;
  CayenneLppBuffer[CayenneLppCursor++] = val >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = val;
  /* USER CODE BEGIN CayenneLppAddAnalogInput_2 */

  /* USER CODE END CayenneLppAddAnalogInput_2 */
  return CayenneLppCursor;
}

uint8_t CayenneLppAddAnalogOutput(uint8_t channel, float value)
{
  /* USER CODE BEGIN CayenneLppAddAnalogOutput_1 */

  /* USER CODE END CayenneLppAddAnalogOutput_1 */
  if ((CayenneLppCursor + LPP_ANALOG_OUTPUT_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  int16_t val = (int16_t)(value * 100);
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_ANALOG_OUTPUT;
  CayenneLppBuffer[CayenneLppCursor++] = val >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = val;
  /* USER CODE BEGIN CayenneLppAddAnalogOutput_2 */

  /* USER CODE END CayenneLppAddAnalogOutput_2 */
  return CayenneLppCursor;
}

uint8_t CayenneLppAddLuminosity(uint8_t channel, uint16_t lux)
{
  /* USER CODE BEGIN CayenneLppAddLuminosity_1 */

  /* USER CODE END CayenneLppAddLuminosity_1 */
  if ((CayenneLppCursor + LPP_LUMINOSITY_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_LUMINOSITY;
  CayenneLppBuffer[CayenneLppCursor++] = lux >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = lux;
  /* USER CODE BEGIN CayenneLppAddLuminosity_2 */

  /* USER CODE END CayenneLppAddLuminosity_2 */
  return CayenneLppCursor;
}

uint8_t CayenneLppAddPresence(uint8_t channel, uint8_t value)
{
  /* USER CODE BEGIN CayenneLppAddPresence_1 */

  /* USER CODE END CayenneLppAddPresence_1 */
  if ((CayenneLppCursor + LPP_PRESENCE_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_PRESENCE;
  CayenneLppBuffer[CayenneLppCursor++] = value;
  /* USER CODE BEGIN CayenneLppAddPresence_2 */

  /* USER CODE END CayenneLppAddPresence_2 */
  return CayenneLppCursor;
}

uint8_t CayenneLppAddTemperature(uint8_t channel, float celsius)
{
  /* USER CODE BEGIN CayenneLppAddTemperature_1 */

  /* USER CODE END CayenneLppAddTemperature_1 */
  if ((CayenneLppCursor + LPP_TEMPERATURE_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  int16_t val = (int16_t)(celsius * 10);
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_TEMPERATURE;
  CayenneLppBuffer[CayenneLppCursor++] = val >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = val;
  /* USER CODE BEGIN CayenneLppAddTemperature_2 */

  /* USER CODE END CayenneLppAddTemperature_2 */
  return CayenneLppCursor;
}

uint8_t CayenneLppAddRelativeHumidity(uint8_t channel, float rh)
{
  /* USER CODE BEGIN CayenneLppAddRelativeHumidity_1 */

  /* USER CODE END CayenneLppAddRelativeHumidity_1 */
  if ((CayenneLppCursor + LPP_RELATIVE_HUMIDITY_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_RELATIVE_HUMIDITY;
  CayenneLppBuffer[CayenneLppCursor++] = (uint8_t)(rh * 2);
  /* USER CODE BEGIN CayenneLppAddRelativeHumidity_2 */

  /* USER CODE END CayenneLppAddRelativeHumidity_2 */
  return CayenneLppCursor;
}

uint8_t CayenneLppAddAccelerometer(uint8_t channel, float x, float y, float z)
{
  /* USER CODE BEGIN CayenneLppAddAccelerometer_1 */

  /* USER CODE END CayenneLppAddAccelerometer_1 */
  if ((CayenneLppCursor + LPP_ACCELEROMETER_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  int16_t vx = (int16_t)(x * 1000);
  int16_t vy = (int16_t)(y * 1000);
  int16_t vz = (int16_t)(z * 1000);

  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_ACCELEROMETER;
  CayenneLppBuffer[CayenneLppCursor++] = vx >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = vx;
  CayenneLppBuffer[CayenneLppCursor++] = vy >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = vy;
  CayenneLppBuffer[CayenneLppCursor++] = vz >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = vz;
  /* USER CODE BEGIN CayenneLppAddAccelerometer_2 */

  /* USER CODE END CayenneLppAddAccelerometer_2 */
  return CayenneLppCursor;
}

uint8_t CayenneLppAddBarometricPressure(uint8_t channel, float hpa)
{
  /* USER CODE BEGIN CayenneLppAddBarometricPressure_1 */

  /* USER CODE END CayenneLppAddBarometricPressure_1 */
  if ((CayenneLppCursor + LPP_BAROMETRIC_PRESSURE_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  int16_t val = (int16_t)(hpa * 10);

  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_BAROMETRIC_PRESSURE;
  CayenneLppBuffer[CayenneLppCursor++] = val >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = val;
  /* USER CODE BEGIN CayenneLppAddBarometricPressure_2 */

  /* USER CODE END CayenneLppAddBarometricPressure_2 */
  return CayenneLppCursor;
}

uint8_t CayenneLppAddGyrometer(uint8_t channel, float x, float y, float z)
{
  /* USER CODE BEGIN CayenneLppAddGyrometer_1 */

  /* USER CODE END CayenneLppAddGyrometer_1 */
  if ((CayenneLppCursor + LPP_GYROMETER_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  int16_t vx = (int16_t)(x * 100);
  int16_t vy = (int16_t)(y * 100);
  int16_t vz = (int16_t)(z * 100);

  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_GYROMETER;
  CayenneLppBuffer[CayenneLppCursor++] = vx >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = vx;
  CayenneLppBuffer[CayenneLppCursor++] = vy >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = vy;
  CayenneLppBuffer[CayenneLppCursor++] = vz >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = vz;
  /* USER CODE BEGIN CayenneLppAddGyrometer_2 */

  /* USER CODE END CayenneLppAddGyrometer_2 */
  return CayenneLppCursor;
}

uint8_t CayenneLppAddGps(uint8_t channel, float latitude, float longitude, float meters)
{
  /* USER CODE BEGIN CayenneLppAddGps_1 */

  /* USER CODE END CayenneLppAddGps_1 */
  if ((CayenneLppCursor + LPP_GPS_SIZE) > CAYENNE_LPP_MAXBUFFER_SIZE)
  {
    return 0;
  }
  int32_t lat = (int32_t)(latitude * 10000);
  int32_t lon = (int32_t)(longitude * 10000);
  int32_t alt = (int32_t)(meters * 100);

  CayenneLppBuffer[CayenneLppCursor++] = channel;
  CayenneLppBuffer[CayenneLppCursor++] = LPP_GPS;

  CayenneLppBuffer[CayenneLppCursor++] = lat >> 16;
  CayenneLppBuffer[CayenneLppCursor++] = lat >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = lat;
  CayenneLppBuffer[CayenneLppCursor++] = lon >> 16;
  CayenneLppBuffer[CayenneLppCursor++] = lon >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = lon;
  CayenneLppBuffer[CayenneLppCursor++] = alt >> 16;
  CayenneLppBuffer[CayenneLppCursor++] = alt >> 8;
  CayenneLppBuffer[CayenneLppCursor++] = alt;
  /* USER CODE BEGIN CayenneLppAddGps_2 */

  /* USER CODE END CayenneLppAddGps_2 */
  return CayenneLppCursor;
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
