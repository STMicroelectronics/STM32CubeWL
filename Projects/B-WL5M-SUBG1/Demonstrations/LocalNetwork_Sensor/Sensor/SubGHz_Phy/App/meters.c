/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    meter.c
  * @author  MCD Application Team
  * @brief   Metering module. Measures sensor data.
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
#include <math.h>
#include <string.h>
#include <meters.h>
#include <stm32_timer.h>
#include <utilities_conf.h>
#include <stm32_seq.h>
#include "adc_if.h"
#include "demo_data_format.h"
#include <motion_ar.h>
#include <motion_ec.h>
#include <motion_mc.h>
#include <motion_pm.h>

#include <b_wl5m_subg_motion_sensors_ex.h>
#include <b_wl5m_subg_env_sensors_ex.h>
#include <b_wl5m_subg_bus.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
  * @brief  Load the calibration parameters from storage
  * @param  data_size  size of data
  * @param  data  pointer of data
  * @retval Will return 0 the if it is success and 1 if it is failure
  */
char MotionMC_LoadCalFromNVM(unsigned short int datasize, unsigned int *data);

/**
  * @brief Wrapper to read I2C register.
  * @param handle user defined data, pointer to device address
  * @param reg register address
  * @param data data to write
  * @param size size of data in bytes
  * @return nonzero on error
  */
char MotionMC_SaveCalInNVM(unsigned short int datasize, unsigned int *data);

/**
  * @brief Init temperature sensor.
  * @return nonzero on error
  */
static uint32_t Meters_STTS22HInit(void);

/**
  * @brief Init pressure sensor.
  * @return nonzero on error
  */
static uint32_t Meters_ILPS22QSInit(void);

/**
  * @brief Init accelerometer sensor.
  * @return nonzero on error
  */
static uint32_t Meters_ISM330DHCXInit(void);

/**
  * @brief Init magnetometer sensor.
  * @return nonzero on error
  */
static uint32_t Meters_IIS2MDCInit(void);

/**
  * @brief Start measurement of the slow environment sensors.
  * @return nonzero on error
  */
static void Meters_TriggerSlow(void *context);

/**
  * @brief Measure slow sensors.
  * @param context ignored
  */
static void Meters_MeasureSlow(void *context);

/**
  * @brief Measure fast sensors and tell sequencer to process the data.
  * @param context ignored
  */
static void Meters_MeasureFast(void *context);

/**
  * @brief Task complementary to Meters_MeasureFast() timer calculates motion data.
  */
static void Meters_MeasureFastTask(void);

/**
  * @brief Reinit sensor when in error
  */
static void Meters_Errors(void);

/**
  * @brief  Load the calibration parameters from storage
  * @param  data_size  size of data
  * @param  data  pointer of data
  * @retval Will return 0 the if it is success and 1 if it is failure
  */
char MotionMC_LoadCalFromNVM(unsigned short int datasize, unsigned int *data)
{
  return (char)1; /* FAILURE: Read from NVM not implemented. */
}

/**
  * @brief  Save the calibration parameters in storage
  * @param  data_size  size of data
  * @param  data  pointer of data
  * @retval Will return 0 the if it is success and 1 if it is failure
  */
char MotionMC_SaveCalInNVM(unsigned short int datasize, unsigned int *data)
{
  return (char)1; /* FAILURE: Write to NVM not implemented. */
}

/**
  * @brief Global structure with data for this module.
  */
typedef struct
{
  uint32_t acc_sum;     /**<Sum of acceleration for average [mg]*/
  uint32_t acc_n;       /**<Number of samples in acc_sum [mg]*/
  uint16_t acc_max;     /**<Maximal acceleration between GetData calls [mg]*/
  int16_t temperature;  /**<Last measured temperature [0.01 `C]*/
  uint16_t pressure;    /**<Last measured pressure [0.1 hPa]*/
  uint8_t humidity;     /**<Last measured humidity [0.5 %]*/
  uint8_t voltage;      /**<MCU voltage [0.05 V]*/
  uint8_t activity;     /**<Recognized activity enum as in DEMO_data_x_x_t*/
  int euler[3];         /**<Orientation result from sensor fusion [degree]*/
  uint16_t n_steps;     /**<Number of steps done*/
  uint16_t distance;    /**<Distance from the ranging sensor [mm]*/  /*note ToF not supported*/
  uint16_t n_swipes;    /**<Number of swipes detected */ /*note ToF not supported*/

  MOTION_SENSOR_AxesRaw_t data_raw_magnetic;       /**<Raw magnetometer output*/
  MOTION_SENSOR_AxesRaw_t data_raw_acceleration;   /**<Raw accelerometer output*/

  MEC_input_t  mec_input;       /**<MotionEC input*/
  MEC_output_t mec_output;      /**<MotionEC output, this needs to be persistent*/
  MMC_Input_t  mmc_input;       /**<MotionMC input*/
  MMC_Output_t mmc_output;      /**<MotionMC output*/
  MAR_input_t  mar_input;       /**<MotionAR input*/
  MAR_output_t mar_output;      /**<MotionAR output*/
  MPM_input_t  mpm_input;       /**<MotionPM input*/
  MPM_output_t mpm_output;      /**<MotionPM output*/


  int timestamp;                /**<Timestamp of the measured sample as in MMC_Input_t::timestamp [ms]*/
  int last_timestamp;           /**<To calculate delta for various calculations [as timestamp]*/
  bool calculation_ongoing;     /**<True when there is a motion calculation ongoing*/

  UTIL_TIMER_Object_t fast_timer;      /**<Timer measuring fast sensors continuously*/
  UTIL_TIMER_Object_t trigger_timer;   /**<Timer to trigger slow sensors just before GetData call*/
  UTIL_TIMER_Object_t measure_timer;   /**<Timer to get measured values of slow sensors*/

  bool temperature_sensor_good;     /**<True if the sensor is good to measure, false if something failed*/
  bool pressure_sensor_good;    /**<True if the sensor is good to measure, false if something failed*/
  bool accelero_sensor_good;  /**<True if the sensor is good to measure, false if something failed*/
  bool magneto_sensor_good;    /**<True if the sensor is good to measure, false if something failed*/
} Meters_t;
Meters_t Meters;

/**
  * @brief Initialize sensors.
  * @return nonzero on error
  */
uint32_t Meters_Init(void)
{
  UTILS_ENTER_CRITICAL_SECTION();       /*EXTI might be triggered immediately*/

  /*Clear measured data*/
  Meters.acc_max = 0;
  Meters.acc_sum = 0;
  Meters.acc_n = 0;
  Meters.activity = DEMO_DATA_1_1_ERROR_ACTIVITY;
  Meters.euler[0] = DEMO_DATA_1_1_ERROR_YAW;
  Meters.euler[1] = DEMO_DATA_1_1_ERROR_PITCH;
  Meters.euler[2] = DEMO_DATA_1_1_ERROR_ROLL;
  Meters.distance = DEMO_DATA_1_1_ERROR_DISTANCE;
  Meters.humidity = DEMO_DATA_1_1_ERROR_HUMIDITY;
  Meters.n_steps = 0;
  Meters.n_swipes = 0;
  Meters.pressure = DEMO_DATA_1_1_ERROR_PRESSURE;
  Meters.temperature = DEMO_DATA_1_1_ERROR_TEMPERATURE;
  Meters.voltage = DEMO_DATA_1_1_ERROR_VOLTAGE;

  /*Init Motion libraries*/
  __HAL_RCC_CRC_CLK_ENABLE();
  MotionMC_Initialize(METERS_FAST_PERIOD, 1);
  float freq = 1000 / METERS_FAST_PERIOD;
  MotionEC_Initialize(MEC_MCU_STM32, &freq);
  MotionEC_SetOrientationEnable(MEC_ENABLE);
  MotionEC_SetLinearAccEnable(MEC_ENABLE);
  MotionAR_Initialize();
  MotionAR_SetOrientation_Acc("nwu");
  MotionPM_Initialize();

  /*Init sensors*/
  Meters.temperature_sensor_good = (Meters_STTS22HInit() == 0) ? true : false;
  Meters.pressure_sensor_good = (Meters_ILPS22QSInit() == 0) ? true : false;
  Meters.accelero_sensor_good = (Meters_ISM330DHCXInit() == 0) ? true : false;
  Meters.magneto_sensor_good = (Meters_IIS2MDCInit() == 0) ? true : false;

  /*Delay to change datarates*/
  HAL_Delay(3 * METERS_FAST_PERIOD);

  /*Init timer and task for fast sensors continuous measuring*/
  UTIL_TIMER_Create(&Meters.fast_timer, METERS_FAST_PERIOD, UTIL_TIMER_PERIODIC, Meters_MeasureFast, NULL);
  UTIL_SEQ_RegTask(CFG_SEQ_Task_Meters, UTIL_SEQ_RFU, Meters_MeasureFastTask);

  /*Init timers for slow sensors wakeup and measurement*/
  UTIL_TIMER_Create(&Meters.trigger_timer, METERS_SLOW_MAX_DELAY, UTIL_TIMER_ONESHOT, Meters_TriggerSlow, NULL);
  UTIL_TIMER_Create(&Meters.measure_timer, METERS_SLOW_ENABLE_DELAY, UTIL_TIMER_ONESHOT, Meters_MeasureSlow, NULL);

  /*Start measuring*/
  UTIL_TIMER_Start(&Meters.fast_timer);
  UTIL_TIMER_Start(&Meters.measure_timer);

  UTILS_EXIT_CRITICAL_SECTION();

  return (Meters.temperature_sensor_good   ? 0x0 : 0x01)
         | (Meters.magneto_sensor_good   ? 0x0 : 0x02)
         | (Meters.accelero_sensor_good ? 0x0 : 0x04)
         | (Meters.pressure_sensor_good   ? 0x0 : 0x08);
}

void Meters_DeInit(void)
{
  BSP_ENV_SENSOR_DeInit(ENV_SENSOR_STTS22H_0);
  BSP_ENV_SENSOR_DeInit(ENV_SENSOR_ILPS22QS_0);
  BSP_MOTION_SENSOR_DeInit(MOTION_SENSOR_ISM330DHCX_0);
  BSP_MOTION_SENSOR_DeInit(MOTION_SENSOR_IIS2MDC_0);
  BSP_I2C2_DeInit();
  BSP_I2C2_DeInit();
  BSP_I2C2_DeInit();
  BSP_I2C2_DeInit();
}
/**
  * @brief Init temperature sensor.
  * @return nonzero on error
  */
static uint32_t Meters_STTS22HInit(void)
{
  if (BSP_ENV_SENSOR_Init(ENV_SENSOR_STTS22H_0, ENV_TEMPERATURE))
  {
    return 1;
  }
  if (BSP_ENV_SENSOR_Enable(ENV_SENSOR_STTS22H_0, ENV_TEMPERATURE))
  {
    return 2;
  }
  if (BSP_ENV_SENSOR_Set_One_Shot(ENV_SENSOR_STTS22H_0))
  {
    return 4;
  }
  return 0;
}

/**
  * @brief Init pressure sensor.
  * @return nonzero on error
  */
static uint32_t Meters_ILPS22QSInit(void)
{
  if (BSP_ENV_SENSOR_Init(ENV_SENSOR_ILPS22QS_0, ENV_PRESSURE))
  {
    return 1;
  }
  if (BSP_ENV_SENSOR_Enable(ENV_SENSOR_ILPS22QS_0, ENV_PRESSURE))
  {
    return 2;
  }
  if (BSP_ENV_SENSOR_Set_One_Shot(ENV_SENSOR_ILPS22QS_0))
  {
    return 3;
  }
  return 0;
}

/**
  * @brief Init accelerometer sensor.
  * @return nonzero on error
  */
static uint32_t Meters_ISM330DHCXInit(void)
{
  if (BSP_MOTION_SENSOR_Init(MOTION_SENSOR_ISM330DHCX_0, MOTION_ACCELERO))
  {
    return 1;
  }
  if (BSP_MOTION_SENSOR_SetFullScale(MOTION_SENSOR_ISM330DHCX_0, MOTION_ACCELERO, 16))
  {
    return 2;
  }
  if (BSP_MOTION_SENSOR_SetOutputDataRate(MOTION_SENSOR_ISM330DHCX_0, MOTION_ACCELERO, 1000 / METERS_FAST_PERIOD))
  {
    return 3;
  }
  return 0;
}

/**
  * @brief Init magnetometer sensor.
  * @return nonzero on error
  */
static uint32_t Meters_IIS2MDCInit(void)
{
  if (BSP_MOTION_SENSOR_Init(MOTION_SENSOR_IIS2MDC_0, MOTION_MAGNETO))
  {
    return 1;
  }
  if (BSP_MOTION_SENSOR_SetOutputDataRate(MOTION_SENSOR_IIS2MDC_0, MOTION_MAGNETO, 1000 / METERS_FAST_PERIOD))
  {
    return 2;
  }
  return 0;
}


/**
  * @brief Do last minute calculations and get measured data ready to be sent.
  * @param Data pointer to data to be written
  * @param NextGet after how long will the next call of this functions be (relative to now) [ms]
  */
void Meters_GetData(DEMO_data_1_1_t *Data, uint32_t NextGet)
{
  /*Start timer to enable sensors before next call*/
  UTIL_TIMER_Stop(&Meters.trigger_timer);
  UTIL_TIMER_SetPeriod(&Meters.trigger_timer, NextGet - METERS_SLOW_ENABLE_DELAY - METERS_SLOW_PREMEASURE);
  UTIL_TIMER_Start(&Meters.trigger_timer);

  UTILS_ENTER_CRITICAL_SECTION();       /*Copy values in critical section so they are consistent*/

  Data->temperature      = Meters.temperature;
  Data->pressure         = Meters.pressure;
  Data->yaw              = Meters.euler[0];
  Data->pitch            = Meters.euler[1];
  Data->roll             = Meters.euler[2];
  Data->humidity         = Meters.humidity;
  Data->activity         = Meters.activity;
  Data->distance         = Meters.distance;
  Data->step_count       = Meters.n_steps;
  Data->swipe_count      = Meters.n_swipes;
  if (Meters.acc_n == 0)
  {
    Data->acceleration_avg = DEMO_DATA_1_1_ERROR_ACCELERATION;
  }
  else
  {
    Data->acceleration_avg = Meters.acc_sum / Meters.acc_n;
  }
  Data->acceleration_max = Meters.acc_max;
  Data->voltage          = Meters.voltage;

  /*Clear sum and maximum for new period*/
  Meters.acc_sum = 0;
  Meters.acc_n = 0;
  Meters.acc_max = 0;

  UTILS_EXIT_CRITICAL_SECTION();
}

/**
  * @brief Start measurement of the slow environment sensors.
  * @return nonzero on error
  */
static void Meters_TriggerSlow(void *context)
{
  /*Trigger temperature sensor*/
  if (Meters.temperature_sensor_good == true)
  {
    if (BSP_ENV_SENSOR_Enable(ENV_SENSOR_STTS22H_0, ENV_TEMPERATURE))
    {
      Meters.temperature_sensor_good = false;
    }
    else if (BSP_ENV_SENSOR_Set_One_Shot(ENV_SENSOR_STTS22H_0))
    {
      Meters.temperature_sensor_good = false;
    }
  }

  /*Trigger Pressure sensor*/
  if (Meters.pressure_sensor_good == true)
  {
    if (BSP_ENV_SENSOR_Enable(ENV_SENSOR_ILPS22QS_0, ENV_PRESSURE))
    {
      Meters.pressure_sensor_good = false;
    }
    else if (BSP_ENV_SENSOR_Set_One_Shot(ENV_SENSOR_ILPS22QS_0))
    {
      Meters.pressure_sensor_good = false;
    }
  }

  /*Enable again in a long time to keep values fresh when GetData is not called*/
  UTIL_TIMER_SetPeriod(&Meters.trigger_timer, METERS_SLOW_MAX_DELAY);
  UTIL_TIMER_Start(&Meters.trigger_timer);

  /*Start timer to get results*/
  UTIL_TIMER_Start(&Meters.measure_timer);
}

/**
  * @brief Measure slow sensors.
  * @param context ignored
  */
static void Meters_MeasureSlow(void *context)
{
  /*Temperature*/
  if (Meters.temperature_sensor_good == true)
  {
    float temperature = 0;
    if (BSP_ENV_SENSOR_GetValue(ENV_SENSOR_STTS22H_0, ENV_TEMPERATURE, &temperature))
    {
      Meters.temperature_sensor_good = false;
    }
    else if (BSP_ENV_SENSOR_Disable(ENV_SENSOR_STTS22H_0, ENV_TEMPERATURE))
    {
      Meters.temperature_sensor_good = false;
    }
    else
    {
      Meters.temperature = (int16_t)(100 * temperature);    /*[0.01 `C]*/
    }
  }
  else
  {
    Meters_Errors();
    Meters.temperature = DEMO_DATA_1_1_ERROR_TEMPERATURE;
  }

  /*Pressure*/
  if (Meters.pressure_sensor_good == true)
  {
    float pressure = 0;
    if (BSP_ENV_SENSOR_GetValue(ENV_SENSOR_ILPS22QS_0, ENV_PRESSURE, &pressure))
    {
      Meters.pressure_sensor_good = false;
    }
    else if (BSP_ENV_SENSOR_Disable(ENV_SENSOR_ILPS22QS_0, ENV_PRESSURE))
    {
      Meters.pressure_sensor_good = false;
    }
    else
    {
      Meters.pressure = (uint16_t)(10 * pressure);    /*[0.1 hPa]*/
    }
  }
  else
  {
    Meters_Errors();
    Meters.pressure = DEMO_DATA_1_1_ERROR_PRESSURE;
  }

  /*Battery Voltage*/
  Meters.voltage = SYS_GetBatteryLevel() / 50;
}

/**
  * @brief Measure fast sensors and tell sequencer to process the data.
  * @param context ignored
  */
static void Meters_MeasureFast(void *context)
{
  Meters.timestamp += METERS_FAST_PERIOD;

  if ((Meters.accelero_sensor_good == true)  /*Accelerometer is working*/
      && (Meters.calculation_ongoing == false)) /*Main thread finished the calculation in time*/
  {
    /*Get accelerometer data*/
    if (BSP_MOTION_SENSOR_GetAxesRaw(MOTION_SENSOR_ISM330DHCX_0, MOTION_ACCELERO, &(Meters.data_raw_acceleration)))
    {
      Meters.accelero_sensor_good = false;
    }
    else if (Meters.magneto_sensor_good == true)     /*Magnetometer is working*/
    {
      HAL_Delay(3); /*delay between to read for i2c*/
      /*Get magnetometer data*/
      if (BSP_MOTION_SENSOR_GetAxesRaw(MOTION_SENSOR_IIS2MDC_0, MOTION_MAGNETO, &(Meters.data_raw_magnetic)))
      {
        Meters.magneto_sensor_good = false;
      }
    }
  }

  if ((Meters.accelero_sensor_good == true)  /*Accelerometer is still working*/
      && (Meters.calculation_ongoing == false)) /*Main thread finished the calculation in time*/
  {
    /*Start task to do the calculation*/
    Meters.calculation_ongoing = true;
    UTIL_SEQ_SetTask(CFG_SEQ_Task_Meters, CFG_SEQ_Prio_1);
  }
  else
  {
    /*No data can be calculated*/
    UTILS_ENTER_CRITICAL_SECTION();       /*Clear values in critical section so they are consistent*/
    Meters.acc_max = DEMO_DATA_1_1_ERROR_ACCELERATION;
    Meters.acc_sum = DEMO_DATA_1_1_ERROR_ACCELERATION;
    Meters.acc_n = 0;
    Meters.activity = DEMO_DATA_1_1_ERROR_ACTIVITY;
    Meters.euler[0] = DEMO_DATA_1_1_ERROR_YAW;
    Meters.euler[1] = DEMO_DATA_1_1_ERROR_PITCH;
    Meters.euler[2] = DEMO_DATA_1_1_ERROR_ROLL;
    UTILS_EXIT_CRITICAL_SECTION();
  }
  if (Meters.accelero_sensor_good == false)
  {
    Meters_Errors();
  }
}

/**
  * @brief Task complementary to Meters_MeasureFast() timer calculates motion data.
  */
static void Meters_MeasureFastTask(void)
{
  /*Clone timestamp early to avoid interrupt overwriting this*/
  Meters.last_timestamp = 0;
  Meters.mec_input.deltatime_s = (Meters.timestamp - Meters.last_timestamp) / (float)1000;
  Meters.mmc_input.TimeStamp = Meters.last_timestamp = Meters.timestamp;

  /*Convert accelerometer, milli g to g*/
  Meters.mec_input.acc[0] = ism330dhcx_from_fs16g_to_mg(Meters.data_raw_acceleration.x) / (float)1000;
  Meters.mar_input.acc_x = Meters.mec_input.acc[0];
  Meters.mpm_input.AccX = Meters.mec_input.acc[0];
  Meters.mec_input.acc[1] = ism330dhcx_from_fs16g_to_mg(Meters.data_raw_acceleration.y) / (float)1000;
  Meters.mar_input.acc_y = Meters.mec_input.acc[1];
  Meters.mpm_input.AccY = Meters.mec_input.acc[1];
  Meters.mec_input.acc[2] = ism330dhcx_from_fs16g_to_mg(Meters.data_raw_acceleration.z) / (float)1000;
  Meters.mar_input.acc_z = Meters.mec_input.acc[2];
  Meters.mpm_input.AccZ = Meters.mec_input.acc[2];

  /*Activity*/
  MotionAR_Update(&(Meters.mar_input), &(Meters.mar_output), Meters.mmc_input.TimeStamp);
  Meters.activity = (uint8_t)Meters.mar_output;

  /*Step counter*/
  MotionPM_Update(&(Meters.mpm_input), &(Meters.mpm_output));
  Meters.n_steps = Meters.mpm_output.Nsteps;

  if (Meters.magneto_sensor_good == true) /*Only if magnetometer is working*/
  {
    /*Convert Magnetometer data, milligauss to microtesla*/
    Meters.mmc_input.Mag[0] = iis2mdc_from_lsb_to_mgauss(Meters.data_raw_magnetic.x) / (float)10;
    Meters.mmc_input.Mag[1] = iis2mdc_from_lsb_to_mgauss(Meters.data_raw_magnetic.y) / (float)10;
    Meters.mmc_input.Mag[2] = iis2mdc_from_lsb_to_mgauss(Meters.data_raw_magnetic.z) / (float)10;

    /*Calibrate Magnetometer*/
    MotionMC_Update(&(Meters.mmc_input));
    MotionMC_GetCalParams(&(Meters.mmc_output));
    Meters.mec_input.mag[0] = ((Meters.mmc_input.Mag[0] - Meters.mmc_output.HI_Bias[0])
                               * Meters.mmc_output.SF_Matrix[0][0]
                               + (Meters.mmc_input.Mag[1] - Meters.mmc_output.HI_Bias[1]) * Meters.mmc_output.SF_Matrix[0][1]
                               + (Meters.mmc_input.Mag[2] - Meters.mmc_output.HI_Bias[2]) * Meters.mmc_output.SF_Matrix[0][2]);
    Meters.mec_input.mag[1] = ((Meters.mmc_input.Mag[0] - Meters.mmc_output.HI_Bias[0])
                               * Meters.mmc_output.SF_Matrix[1][0]
                               + (Meters.mmc_input.Mag[1] - Meters.mmc_output.HI_Bias[1]) * Meters.mmc_output.SF_Matrix[1][1]
                               + (Meters.mmc_input.Mag[2] - Meters.mmc_output.HI_Bias[2]) * Meters.mmc_output.SF_Matrix[1][2]);
    Meters.mec_input.mag[2] = ((Meters.mmc_input.Mag[0] - Meters.mmc_output.HI_Bias[0])
                               * Meters.mmc_output.SF_Matrix[2][0]
                               + (Meters.mmc_input.Mag[1] - Meters.mmc_output.HI_Bias[1]) * Meters.mmc_output.SF_Matrix[2][1]
                               + (Meters.mmc_input.Mag[2] - Meters.mmc_output.HI_Bias[2]) * Meters.mmc_output.SF_Matrix[2][2]);

    /*Sensor fusion*/
    {
      MotionEC_Run(&(Meters.mec_input), &(Meters.mec_output));
      UTILS_ENTER_CRITICAL_SECTION()
      ; /*Copy values in critical section so they are consistent*/
      Meters.euler[0] = (int)Meters.mec_output.euler[0];
      Meters.euler[1] = (int)Meters.mec_output.euler[1];
      Meters.euler[2] = (int)Meters.mec_output.euler[2];
      UTILS_EXIT_CRITICAL_SECTION();
    }

    /*Average and maximal acceleration*/
    {
      int acc = (int)(1000
                      * sqrtf(
                        Meters.mec_output.linear[0] * Meters.mec_output.linear[0]
                        + Meters.mec_output.linear[1] * Meters.mec_output.linear[1]
                        + Meters.mec_output.linear[2] * Meters.mec_output.linear[2])); /*Euclidean norm*/

      UTILS_ENTER_CRITICAL_SECTION(); /*Calculate values in critical section so they are consistent*/
      if ((Meters.acc_sum != DEMO_DATA_1_1_ERROR_ACCELERATION) && (Meters.acc_max != DEMO_DATA_1_1_ERROR_ACCELERATION))
      {
        Meters.acc_sum += acc; /*Sum all acceleration*/
        Meters.acc_n++; /*Number of summed elements*/
        if (acc > Meters.acc_max)
        {
          Meters.acc_max = acc; /*Track maximum*/
        }
      }
      UTILS_EXIT_CRITICAL_SECTION();
    }
  }
  else
  {
    /*Sensor fusion is not possible*/
    UTILS_ENTER_CRITICAL_SECTION();       /*Clear values in critical section so they are consistent*/
    Meters.euler[0] = DEMO_DATA_1_1_ERROR_YAW;
    Meters.euler[1] = DEMO_DATA_1_1_ERROR_PITCH;
    Meters.euler[2] = DEMO_DATA_1_1_ERROR_ROLL;
    UTILS_EXIT_CRITICAL_SECTION();
  }

  /*Reset flag to let timer know that the calculation ended*/
  Meters.calculation_ongoing = false;
}

static void Meters_Errors(void)
{
  UTILS_ENTER_CRITICAL_SECTION();
  Meters_DeInit();
  Meters_Init();
  UTILS_EXIT_CRITICAL_SECTION();
}

#ifdef __cplusplus
}
#endif
