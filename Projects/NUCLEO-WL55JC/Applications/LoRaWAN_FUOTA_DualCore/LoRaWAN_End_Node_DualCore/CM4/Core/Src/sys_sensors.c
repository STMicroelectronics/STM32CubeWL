/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sys_sensors.c
  * @author  MCD Application Team
  * @brief   Manages the sensors on the application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "platform.h"
#include "sys_conf.h"
#include "sys_sensors.h"
#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 0)
#include "adc_if.h"
#endif /* SENSOR_ENABLED */

/* USER CODE BEGIN Includes */
#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 1)
#if defined (X_NUCLEO_IKS01A2)
#warning "IKS drivers are today available for several families but not stm32WL"
#warning "up to the user adapt IKS low layer to map it on WL board driver"
#warning "this code would work only if user provide necessary IKS and BSP layers"
#include "iks01a2_env_sensors.h"
#elif defined (X_NUCLEO_IKS01A3)

/*
## How to add IKS01A3 to STM32CubeWL
   Note that LoRaWAN_End_Node Example is used as an example for steps below.
 1. Open the LoRaWAN_End_Node CubeMX project by double-clicking on the LoRaWAN_End_Node.ioc under "STM32Cube_FW_WL_V1.x.x\Projects\NUCLEO-WL55JC\Applications\LoRaWAN\LoRaWAN_End_Node"
 2. From the CubeMX project, click on "Software Packs"->"Manage Software Packs" to open the Embedded Software Packages Manager. Then, click on the "STMicroelectronics" tab, expand the X-CUBE-MEMS1, check the latest version of this pack (i.e. 9.0.0), and install. Then, close the Embedded Software Packages Manager.
 3. From the CubeMX project, click on "Software Packs"->"Select Components" to open the Software Packs Component Selector, expand the X-CUBE-MEMS1 pack and select the "Board Extension IKS01A3" component by checking the respective box, and click OK.
 4. From the CubeMX project, expand the "Connectivity" category and enable I2C2 on pins PA11 (I2C2_SDA) and PA12 (I2C2_SCK).
 5. From the CubeMX project, expand the "Software Packs" category and enable the "Board Extension IKS01A3" by checking the box, and choose I2C2 under the "Found Solutions" menu.
 6. From the CubeMX project, click the "Project Manager" section
    - From the "Project Settings" section, select your Toolchain/IDE of choice (if CubeIDE, uncheck the "Generator Under Root" option).
    - From the "Code Generator" section, select "Copy only the necessary library files".
 7. Click "GENERATE CODE" to generate the code project with the MEMS drivers integrated.
 8. From the code project, find and open the sys_conf.h and make the following edits
    - Set the #define SENSOR_ENABLED to 1
    - Set the #define LOW_POWER_DISABLE to 1 to prevent the device from entering low power mode. This is needed, since the I2C2 requires handling when exiting low power modes, so to prevent issues, best is to disable low power mode, however, if low power mode is desired, you'll have to re-initialize the I2C2 from PWR_ExitStopMode() in stm32_lpm_if.c, so you can just call HAL_I2C_Init() from there.
 9. From the code project, find and open lora_app.h, and uncomment the following line
    #define CAYENNE_LPP
 10. From the code project properties, add X_NUCLEO_IKS01A3 Pre-processor Defined symbol.
 11. Save all changes and build project
 12. Connect the X-NUCLEO-IKS01A3 expansion board on the NUCLEO-WL55JC1
 13. Load and run the code
*/
#warning "IKS drivers are today available for several families but not stm32WL, follow steps defined in sys_sensors.c"
#include "iks01a3_env_sensors.h"
#else  /* not X_IKS01xx */
#error "user to include its sensor drivers"
#endif  /* X_NUCLEO_IKS01xx */
#elif !defined (SENSOR_ENABLED)
#error SENSOR_ENABLED not defined
#endif  /* SENSOR_ENABLED */
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/

/* USER CODE BEGIN PD */
#define STSOP_LATTITUDE           ((float) 43.618622 )  /*!< default latitude position */
#define STSOP_LONGITUDE           ((float) 7.051415  )  /*!< default longitude position */
#define MAX_GPS_POS               ((int32_t) 8388607 )  /*!< 2^23 - 1 */
#define HUMIDITY_DEFAULT_VAL      50.0f                 /*!< default humidity */
#define TEMPERATURE_DEFAULT_VAL   18.0f                 /*!< default temperature */
#define PRESSURE_DEFAULT_VAL      1000.0f               /*!< default pressure */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 1)
#if defined (X_NUCLEO_IKS01A2)
#warning "IKS drivers are today available for several families but not stm32WL"
#warning "up to the user adapt IKS low layer to map it on WL board driver"
#warning "this code would work only if user provide necessary IKS and BSP layers"
IKS01A2_ENV_SENSOR_Capabilities_t EnvCapabilities;
#elif defined (X_NUCLEO_IKS01A3)
IKS01A3_ENV_SENSOR_Capabilities_t EnvCapabilities;
#else  /* not X_IKS01Ax */
#error "user to include its sensor drivers"
#endif  /* X_NUCLEO_IKS01 */
#elif !defined (SENSOR_ENABLED)
#error SENSOR_ENABLED not defined
#endif  /* SENSOR_ENABLED */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int32_t EnvSensors_Read(sensor_t *sensor_data)
{
  /* USER CODE BEGIN EnvSensors_Read */
  float HUMIDITY_Value = HUMIDITY_DEFAULT_VAL;
  float TEMPERATURE_Value = TEMPERATURE_DEFAULT_VAL;
  float PRESSURE_Value = PRESSURE_DEFAULT_VAL;

#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 1)
#if (USE_IKS01A2_ENV_SENSOR_HTS221_0 == 1)
  IKS01A2_ENV_SENSOR_GetValue(HTS221_0, ENV_HUMIDITY, &HUMIDITY_Value);
  IKS01A2_ENV_SENSOR_GetValue(HTS221_0, ENV_TEMPERATURE, &TEMPERATURE_Value);
#endif /* USE_IKS01A2_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A2_ENV_SENSOR_LPS22HB_0 == 1)
  IKS01A2_ENV_SENSOR_GetValue(LPS22HB_0, ENV_PRESSURE, &PRESSURE_Value);
  IKS01A2_ENV_SENSOR_GetValue(LPS22HB_0, ENV_TEMPERATURE, &TEMPERATURE_Value);
#endif /* USE_IKS01A2_ENV_SENSOR_LPS22HB_0 */
#if (USE_IKS01A3_ENV_SENSOR_HTS221_0 == 1)
  IKS01A3_ENV_SENSOR_GetValue(IKS01A3_HTS221_0, ENV_HUMIDITY, &HUMIDITY_Value);
  IKS01A3_ENV_SENSOR_GetValue(IKS01A3_HTS221_0, ENV_TEMPERATURE, &TEMPERATURE_Value);
#endif /* USE_IKS01A3_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A3_ENV_SENSOR_LPS22HH_0 == 1)
  IKS01A3_ENV_SENSOR_GetValue(IKS01A3_LPS22HH_0, ENV_PRESSURE, &PRESSURE_Value);
  IKS01A3_ENV_SENSOR_GetValue(IKS01A3_LPS22HH_0, ENV_TEMPERATURE, &TEMPERATURE_Value);
#endif /* USE_IKS01A3_ENV_SENSOR_LPS22HH_0 */
#else
  TEMPERATURE_Value = (SYS_GetTemperatureLevel() >> 8);
#endif  /* SENSOR_ENABLED */

  sensor_data->humidity    = HUMIDITY_Value;
  sensor_data->temperature = TEMPERATURE_Value;
  sensor_data->pressure    = PRESSURE_Value;

  sensor_data->latitude  = (int32_t)((STSOP_LATTITUDE  * MAX_GPS_POS) / 90);
  sensor_data->longitude = (int32_t)((STSOP_LONGITUDE  * MAX_GPS_POS) / 180);

  return 0;
  /* USER CODE END EnvSensors_Read */
}

int32_t EnvSensors_Init(void)
{
  int32_t ret = 0;
  /* USER CODE BEGIN EnvSensors_Init */
#if defined (SENSOR_ENABLED) && (SENSOR_ENABLED == 1)
  /* Init */
#if (USE_IKS01A2_ENV_SENSOR_HTS221_0 == 1)
  ret = IKS01A2_ENV_SENSOR_Init(HTS221_0, ENV_TEMPERATURE | ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A2_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A2_ENV_SENSOR_LPS22HB_0 == 1)
  ret = IKS01A2_ENV_SENSOR_Init(LPS22HB_0, ENV_TEMPERATURE | ENV_PRESSURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A2_ENV_SENSOR_LPS22HB_0 */
#if (USE_IKS01A3_ENV_SENSOR_HTS221_0 == 1)
  ret = IKS01A3_ENV_SENSOR_Init(IKS01A3_HTS221_0, ENV_TEMPERATURE | ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A3_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A3_ENV_SENSOR_LPS22HH_0 == 1)
  ret = IKS01A3_ENV_SENSOR_Init(IKS01A3_LPS22HH_0, ENV_TEMPERATURE | ENV_PRESSURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A3_ENV_SENSOR_LPS22HH_0 */

  /* Enable */
#if (USE_IKS01A2_ENV_SENSOR_HTS221_0 == 1)
  ret = IKS01A2_ENV_SENSOR_Enable(HTS221_0, ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  ret = IKS01A2_ENV_SENSOR_Enable(HTS221_0, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A2_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A2_ENV_SENSOR_LPS22HB_0 == 1)
  ret = IKS01A2_ENV_SENSOR_Enable(LPS22HB_0, ENV_PRESSURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  ret = IKS01A2_ENV_SENSOR_Enable(LPS22HB_0, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A2_ENV_SENSOR_LPS22HB_0 */
#if (USE_IKS01A3_ENV_SENSOR_HTS221_0 == 1)
  ret = IKS01A3_ENV_SENSOR_Enable(IKS01A3_HTS221_0, ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  ret = IKS01A3_ENV_SENSOR_Enable(IKS01A3_HTS221_0, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A3_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A3_ENV_SENSOR_LPS22HH_0 == 1)
  ret = IKS01A3_ENV_SENSOR_Enable(IKS01A3_LPS22HH_0, ENV_PRESSURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  ret = IKS01A3_ENV_SENSOR_Enable(IKS01A3_LPS22HH_0, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A3_ENV_SENSOR_LPS22HH_0 */

  /* Get capabilities */
#if (USE_IKS01A2_ENV_SENSOR_HTS221_0 == 1)
  ret = IKS01A2_ENV_SENSOR_GetCapabilities(HTS221_0, &EnvCapabilities);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A2_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A2_ENV_SENSOR_LPS22HB_0 == 1)
  ret = IKS01A2_ENV_SENSOR_GetCapabilities(LPS22HB_0, &EnvCapabilities);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A2_ENV_SENSOR_LPS22HB_0 */
#if (USE_IKS01A3_ENV_SENSOR_HTS221_0 == 1)
  ret = IKS01A3_ENV_SENSOR_GetCapabilities(IKS01A3_HTS221_0, &EnvCapabilities);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A3_ENV_SENSOR_HTS221_0 */
#if (USE_IKS01A3_ENV_SENSOR_LPS22HH_0 == 1)
  ret = IKS01A3_ENV_SENSOR_GetCapabilities(IKS01A3_LPS22HH_0, &EnvCapabilities);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
#endif /* USE_IKS01A3_ENV_SENSOR_LPS22HH_0 */

#elif !defined (SENSOR_ENABLED)
#error SENSOR_ENABLED not defined
#endif /* SENSOR_ENABLED  */
  /* USER CODE END EnvSensors_Init */
  return ret;
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
