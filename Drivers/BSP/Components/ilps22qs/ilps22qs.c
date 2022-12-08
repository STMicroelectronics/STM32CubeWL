/**
  ******************************************************************************
  * @file    ilps22qs.c
  * @author  MEMS Software Solutions Team
  * @brief   ILPS22QS driver file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ilps22qs.h"

/** @addtogroup BSP BSP
  * @{
  */

/** @addtogroup Component Component
  * @{
  */

/** @defgroup ILPS22QS ILPS22QS
  * @{
  */

/** @defgroup ILPS22QS_Exported_Variables ILPS22QS Exported Variables
  * @{
  */

ILPS22QS_CommonDrv_t ILPS22QS_COMMON_Driver =
{
  ILPS22QS_Init,
  ILPS22QS_DeInit,
  ILPS22QS_ReadID,
  ILPS22QS_GetCapabilities,
};

ILPS22QS_PRESS_Drv_t ILPS22QS_PRESS_Driver =
{
  ILPS22QS_PRESS_Enable,
  ILPS22QS_PRESS_Disable,
  ILPS22QS_PRESS_GetOutputDataRate,
  ILPS22QS_PRESS_SetOutputDataRate,
  ILPS22QS_PRESS_GetPressure,
};

ILPS22QS_TEMP_Drv_t ILPS22QS_TEMP_Driver =
{
  ILPS22QS_TEMP_Enable,
  ILPS22QS_TEMP_Disable,
  ILPS22QS_TEMP_GetOutputDataRate,
  ILPS22QS_TEMP_SetOutputDataRate,
  ILPS22QS_TEMP_GetTemperature,
};

/**
  * @}
  */

/** @defgroup ILPS22QS_Private_Function_Prototypes ILPS22QS Private Function Prototypes
  * @{
  */

static int32_t ReadRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint16_t Length);
static int32_t WriteRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint16_t Length);
static int32_t ILPS22QS_GetOutputDataRate(ILPS22QS_Object_t *pObj, float *Odr);
static int32_t ILPS22QS_SetOutputDataRate_When_Enabled(ILPS22QS_Object_t *pObj, float Odr);
static int32_t ILPS22QS_SetOutputDataRate_When_Disabled(ILPS22QS_Object_t *pObj, float Odr);
static int32_t ILPS22QS_Initialize(ILPS22QS_Object_t *pObj);

/**
  * @}
  */

/** @defgroup ILPS22QS_Exported_Functions ILPS22QS Exported Functions
  * @{
  */

/**
  * @brief  Register Component Bus IO operations
  * @param  pObj the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_RegisterBusIO(ILPS22QS_Object_t *pObj, ILPS22QS_IO_t *pIO)
{
  int32_t ret = ILPS22QS_OK;

  if (pObj == NULL)
  {
    ret = ILPS22QS_ERROR;
  }
  else
  {
    pObj->IO.Init      = pIO->Init;
    pObj->IO.DeInit    = pIO->DeInit;
    pObj->IO.BusType   = pIO->BusType;
    pObj->IO.Address   = pIO->Address;
    pObj->IO.WriteReg  = pIO->WriteReg;
    pObj->IO.ReadReg   = pIO->ReadReg;
    pObj->IO.GetTick   = pIO->GetTick;

    pObj->Ctx.read_reg  = ReadRegWrap;
    pObj->Ctx.write_reg = WriteRegWrap;
    pObj->Ctx.handle    = pObj;

    if (pObj->IO.Init == NULL)
    {
      ret = ILPS22QS_ERROR;
    }
    else if (pObj->IO.Init() != ILPS22QS_OK)
    {
      ret = ILPS22QS_ERROR;
    }
    else
    {
      if (pObj->IO.BusType == ILPS22QS_SPI_3WIRES_BUS) /* SPI 3-Wires */
      {
        /* Enable the SPI 3-Wires support only the first time */
        if (pObj->is_initialized == 0U)
        {
          /* Enable SPI 3-Wires on the component */
          uint8_t data = 0x20;

          if (ILPS22QS_Write_Reg(pObj, ILPS22QS_IF_CTRL, data) != ILPS22QS_OK)
          {
            ret = ILPS22QS_ERROR;
          }
        }
      }
    }
  }

  return ret;
}

/**
  * @brief  Initialize the ILPS22QS sensor
  * @param  pObj the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_Init(ILPS22QS_Object_t *pObj)
{
  if (pObj->is_initialized == 0U)
  {
    if (ILPS22QS_Initialize(pObj) != ILPS22QS_OK)
    {
      return ILPS22QS_ERROR;
    }
  }

  pObj->is_initialized = 1U;

  return ILPS22QS_OK;
}

/**
  * @brief  Deinitialize the ILPS22QS sensor
  * @param  pObj the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_DeInit(ILPS22QS_Object_t *pObj)
{
  if (pObj->is_initialized == 1U)
  {
    if (ILPS22QS_PRESS_Disable(pObj) != ILPS22QS_OK)
    {
      return ILPS22QS_ERROR;
    }

    if (ILPS22QS_TEMP_Disable(pObj) != ILPS22QS_OK)
    {
      return ILPS22QS_ERROR;
    }
  }

  pObj->is_initialized = 0;

  return ILPS22QS_OK;
}

/**
  * @brief  Get WHO_AM_I value
  * @param  pObj the device pObj
  * @param  Id the WHO_AM_I value
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_ReadID(ILPS22QS_Object_t *pObj, uint8_t *Id)
{
  ilps22qs_id_t val;
  if (ilps22qs_id_get(&(pObj->Ctx), &val) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  *Id = val.whoami;

  return ILPS22QS_OK;
}

/**
  * @brief  Get ILPS22QS sensor capabilities
  * @param  pObj Component object pointer
  * @param  Capabilities pointer to ILPS22QS sensor capabilities
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_GetCapabilities(ILPS22QS_Object_t *pObj, ILPS22QS_Capabilities_t *Capabilities)
{
  /* Prevent unused argument(s) compilation warning */
  (void)(pObj);

  Capabilities->Humidity    = 0;
  Capabilities->Pressure    = 1;
  Capabilities->Temperature = 1;
  Capabilities->LowPower    = 0;
  Capabilities->HumMaxOdr   = 0.0f;
  Capabilities->TempMaxOdr  = 200.0f;
  Capabilities->PressMaxOdr = 200.0f;
  return ILPS22QS_OK;
}

/**
  * @brief  Get the ILPS22QS initialization status
  * @param  pObj the device pObj
  * @param  Status 1 if initialized, 0 otherwise
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_Get_Init_Status(ILPS22QS_Object_t *pObj, uint8_t *Status)
{
  if (pObj == NULL)
  {
    return ILPS22QS_ERROR;
  }

  *Status = pObj->is_initialized;

  return ILPS22QS_OK;
}

/**
  * @brief  Enable the ILPS22QS pressure sensor
  * @param  pObj the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_PRESS_Enable(ILPS22QS_Object_t *pObj)
{
  /* Check if the component is already enabled */
  if (pObj->press_is_enabled == 1U)
  {
    return ILPS22QS_OK;
  }

  /* Output data rate selection. */
  if (ilps22qs_mode_set(&(pObj->Ctx), &pObj->last_odr) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  pObj->press_is_enabled = 1;

  return ILPS22QS_OK;
}

/**
  * @brief  Disable the ILPS22QS pressure sensor
  * @param  pObj the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_PRESS_Disable(ILPS22QS_Object_t *pObj)
{
  /* Check if the component is already disabled */
  if (pObj->press_is_enabled == 0U)
  {
    return ILPS22QS_OK;
  }

  /* Check if the ILPS22QS temperature sensor is still enable. */
  /* If yes, skip the disable function, if not call disable function */
  if (pObj->temp_is_enabled == 0U)
  {
    ilps22qs_md_t val;

    /* Get current output data rate. */
    if (ilps22qs_mode_get(&(pObj->Ctx), &val) != ILPS22QS_OK)
    {
      return ILPS22QS_ERROR;
    }

    memcpy(&pObj->last_odr, &val, sizeof(ilps22qs_md_t));

    val.odr = ILPS22QS_ONE_SHOT;

    /* Output data rate selection - power down. */
    if (ilps22qs_mode_set(&(pObj->Ctx), &val) != ILPS22QS_OK)
    {
      return ILPS22QS_ERROR;
    }
  }

  pObj->press_is_enabled = 0;

  return ILPS22QS_OK;
}

/**
  * @brief  Get the ILPS22QS pressure sensor output data rate
  * @param  pObj the device pObj
  * @param  Odr pointer where the output data rate is written
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_PRESS_GetOutputDataRate(ILPS22QS_Object_t *pObj, float *Odr)
{
  return ILPS22QS_GetOutputDataRate(pObj, Odr);
}

/**
  * @brief  Set the ILPS22QS pressure sensor output data rate
  * @param  pObj the device pObj
  * @param  Odr the output data rate value to be set
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_PRESS_SetOutputDataRate(ILPS22QS_Object_t *pObj, float Odr)
{
  /* Check if the component is enabled */
  if (pObj->press_is_enabled == 1U)
  {
    return ILPS22QS_SetOutputDataRate_When_Enabled(pObj, Odr);
  }
  else
  {
    return ILPS22QS_SetOutputDataRate_When_Disabled(pObj, Odr);
  }
}

/**
  * @brief  Get the ILPS22QS pressure value
  * @param  pObj the device pObj
  * @param  Value pointer where the pressure value is written
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_PRESS_GetPressure(ILPS22QS_Object_t *pObj, float *Value)
{
  ilps22qs_data_t data;
  ilps22qs_md_t md;

  if (ilps22qs_mode_get(&(pObj->Ctx), &md) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  if (md.fs != ILPS22QS_1260hPa)  /* NOTE: Currently only 1260 hPa full scale supported */
  {
    md.fs = ILPS22QS_1260hPa;

    if (ilps22qs_mode_set(&(pObj->Ctx), &md) != ILPS22QS_OK)
    {
      return ILPS22QS_ERROR;
    }
  }

  if (ilps22qs_data_get(&(pObj->Ctx), &md, &data) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  *Value = data.pressure.hpa;

  return ILPS22QS_OK;
}

/**
  * @brief  Get the ILPS22QS pressure data ready bit value
  * @param  pObj the device pObj
  * @param  Status the status of data ready bit
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_PRESS_Get_DRDY_Status(ILPS22QS_Object_t *pObj, uint8_t *Status)
{
  ilps22qs_status_t reg;

  if (ilps22qs_read_reg(&(pObj->Ctx), ILPS22QS_STATUS, (uint8_t *) &reg, 1) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  *Status = reg.p_da;

  return ILPS22QS_OK;
}

/**
  * @brief  Enable the ILPS22QS temperature sensor
  * @param  pObj the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_TEMP_Enable(ILPS22QS_Object_t *pObj)
{
  /* Check if the component is already enabled */
  if (pObj->temp_is_enabled == 1U)
  {
    return ILPS22QS_OK;
  }

  /* Output data rate selection. */
  if (ilps22qs_mode_set(&(pObj->Ctx), &pObj->last_odr) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  pObj->temp_is_enabled = 1;

  return ILPS22QS_OK;
}

/**
  * @brief  Disable the ILPS22QS temperature sensor
  * @param  pObj the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_TEMP_Disable(ILPS22QS_Object_t *pObj)
{
  /* Check if the component is already disabled */
  if (pObj->temp_is_enabled == 0U)
  {
    return ILPS22QS_OK;
  }

  /* Check if the ILPS22QS pressure sensor is still enable. */
  /* If yes, skip the disable function, if not call disable function */
  if (pObj->press_is_enabled == 0U)
  {
    ilps22qs_md_t val;

    /* Get current output data rate. */
    if (ilps22qs_mode_get(&(pObj->Ctx), &val) != ILPS22QS_OK)
    {
      return ILPS22QS_ERROR;
    }

    memcpy(&pObj->last_odr, &val, sizeof(ilps22qs_md_t));

    val.odr = ILPS22QS_ONE_SHOT;

    /* Output data rate selection - power down. */
    if (ilps22qs_mode_set(&(pObj->Ctx), &val) != ILPS22QS_OK)
    {
      return ILPS22QS_ERROR;
    }
  }

  pObj->temp_is_enabled = 0;

  return ILPS22QS_OK;
}

/**
  * @brief  Get the ILPS22QS temperature sensor output data rate
  * @param  pObj the device pObj
  * @param  Odr pointer where the output data rate is written
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_TEMP_GetOutputDataRate(ILPS22QS_Object_t *pObj, float *Odr)
{
  return ILPS22QS_GetOutputDataRate(pObj, Odr);
}

/**
  * @brief  Set the ILPS22QS temperature sensor output data rate
  * @param  pObj the device pObj
  * @param  Odr the output data rate value to be set
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_TEMP_SetOutputDataRate(ILPS22QS_Object_t *pObj, float Odr)
{
  /* Check if the component is enabled */
  if (pObj->temp_is_enabled == 1U)
  {
    return ILPS22QS_SetOutputDataRate_When_Enabled(pObj, Odr);
  }
  else
  {
    return ILPS22QS_SetOutputDataRate_When_Disabled(pObj, Odr);
  }
}

/**
  * @brief  Get the ILPS22QS temperature value
  * @param  pObj the device pObj
  * @param  Value pointer where the temperature value is written
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_TEMP_GetTemperature(ILPS22QS_Object_t *pObj, float *Value)
{
  ilps22qs_data_t data;
  ilps22qs_md_t md;

  if (ilps22qs_mode_get(&(pObj->Ctx), &md) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  if (md.fs != ILPS22QS_1260hPa)  /* NOTE: Currently only 1260 hPa full scale supported */
  {
    md.fs = ILPS22QS_1260hPa;

    if (ilps22qs_mode_set(&(pObj->Ctx), &md) != ILPS22QS_OK)
    {
      return ILPS22QS_ERROR;
    }
  }

  if (ilps22qs_data_get(&(pObj->Ctx), &md, &data) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  *Value = data.heat.deg_c;

  return ILPS22QS_OK;
}

/**
  * @brief  Get the ILPS22QS temperature data ready bit value
  * @param  pObj the device pObj
  * @param  Status the status of data ready bit
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_TEMP_Get_DRDY_Status(ILPS22QS_Object_t *pObj, uint8_t *Status)
{
  ilps22qs_status_t reg;

  if (ilps22qs_read_reg(&(pObj->Ctx), ILPS22QS_STATUS, (uint8_t *) &reg, 1) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  *Status = reg.t_da;

  return ILPS22QS_OK;
}

/**
  * @brief  Get the ILPS22QS register value
  * @param  pObj the device pObj
  * @param  Reg address to be written
  * @param  Data value to be written
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_Read_Reg(ILPS22QS_Object_t *pObj, uint8_t Reg, uint8_t *Data)
{
  if (ilps22qs_read_reg(&(pObj->Ctx), Reg, Data, 1) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  return ILPS22QS_OK;
}

/**
  * @brief  Set the ILPS22QS register value
  * @param  pObj the device pObj
  * @param  Reg address to be written
  * @param  Data value to be written
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_Write_Reg(ILPS22QS_Object_t *pObj, uint8_t Reg, uint8_t Data)
{
  if (ilps22qs_write_reg(&(pObj->Ctx), Reg, &Data, 1) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  return ILPS22QS_OK;
}

/**
  * @}
  */

/** @defgroup ILPS22QS_Private_Functions ILPS22QS Private Functions
  * @{
  */

/**
  * @brief  Get output data rate
  * @param  pObj the device pObj
  * @param  Odr the output data rate value
  * @retval 0 in case of success, an error code otherwise
  */
static int32_t ILPS22QS_GetOutputDataRate(ILPS22QS_Object_t *pObj, float *Odr)
{
  int32_t ret = ILPS22QS_OK;
  ilps22qs_md_t val;

  if (ilps22qs_mode_get(&(pObj->Ctx), &val) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  switch (val.odr)
  {
    case ILPS22QS_ONE_SHOT:
      *Odr = 0.0f;
      break;

    case ILPS22QS_1Hz:
      *Odr = 1.0f;
      break;

    case ILPS22QS_4Hz:
      *Odr = 4.0f;
      break;

    case ILPS22QS_10Hz:
      *Odr = 10.0f;
      break;

    case ILPS22QS_25Hz:
      *Odr = 25.0f;
      break;

    case ILPS22QS_50Hz:
      *Odr = 50.0f;
      break;

    case ILPS22QS_75Hz:
      *Odr = 75.0f;
      break;

    case ILPS22QS_100Hz:
      *Odr = 100.0f;
      break;

    case ILPS22QS_200Hz:
      *Odr = 200.0f;
      break;

    default:
      ret = ILPS22QS_ERROR;
      break;
  }

  return ret;
}

/**
  * @brief  Set output data rate
  * @param  pObj the device pObj
  * @param  Odr the output data rate value to be set
  * @retval 0 in case of success, an error code otherwise
  */
static int32_t ILPS22QS_SetOutputDataRate_When_Enabled(ILPS22QS_Object_t *pObj, float Odr)
{
  ilps22qs_md_t new_val;

  if (ilps22qs_mode_get(&(pObj->Ctx), &new_val) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  new_val.odr = (Odr <=   1.0f) ? ILPS22QS_1Hz
                : (Odr <=   4.0f) ? ILPS22QS_4Hz
                : (Odr <=  10.0f) ? ILPS22QS_10Hz
                : (Odr <=  25.0f) ? ILPS22QS_25Hz
                : (Odr <=  50.0f) ? ILPS22QS_50Hz
                : (Odr <=  75.0f) ? ILPS22QS_75Hz
                : (Odr <= 100.0f) ? ILPS22QS_100Hz
                :                   ILPS22QS_200Hz;

  if (ilps22qs_mode_set(&(pObj->Ctx), &new_val) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  if (ilps22qs_mode_get(&(pObj->Ctx), &pObj->last_odr) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  return ILPS22QS_OK;
}

/**
  * @brief  Set output data rate when disabled
  * @param  pObj the device pObj
  * @param  Odr the output data rate value to be set
  * @retval 0 in case of success, an error code otherwise
  */
static int32_t ILPS22QS_SetOutputDataRate_When_Disabled(ILPS22QS_Object_t *pObj, float Odr)
{
  pObj->last_odr.odr = (Odr <=   1.0f) ? ILPS22QS_1Hz
                       : (Odr <=   4.0f) ? ILPS22QS_4Hz
                       : (Odr <=  10.0f) ? ILPS22QS_10Hz
                       : (Odr <=  25.0f) ? ILPS22QS_25Hz
                       : (Odr <=  50.0f) ? ILPS22QS_50Hz
                       : (Odr <=  75.0f) ? ILPS22QS_75Hz
                       : (Odr <= 100.0f) ? ILPS22QS_100Hz
                       :                   ILPS22QS_200Hz;

  return ILPS22QS_OK;
}

/**
  * @brief  Initialize the ILPS22QS sensor
  * @param  pObj the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
static int32_t ILPS22QS_Initialize(ILPS22QS_Object_t *pObj)
{
  ilps22qs_md_t md;
  ilps22qs_bus_mode_t bus_mode;

  /* Set bdu and if_inc recommended for driver usage */
  if (ilps22qs_init_set(&(pObj->Ctx), ILPS22QS_DRV_RDY) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  /* Select bus interface */
  if (pObj->IO.BusType == ILPS22QS_SPI_3WIRES_BUS) /* SPI 3-Wires */
  {
    bus_mode.interface = ILPS22QS_SPI_3W;
  }
  else if (pObj->IO.BusType == ILPS22QS_SPI_4WIRES_BUS) /* SPI 3-Wires */
  {
    bus_mode.interface = ILPS22QS_SPI_4W;
  }
  else
  {
    bus_mode.interface = ILPS22QS_SEL_BY_HW;
  }

  bus_mode.filter = ILPS22QS_AUTO;
  if (ilps22qs_bus_mode_set(&(pObj->Ctx), &bus_mode) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  /* Set Output Data Rate in Power Down */
  md.odr = ILPS22QS_ONE_SHOT;

  /* Configure basic parameters */
  md.avg = ILPS22QS_4_AVG;
  md.lpf = ILPS22QS_LPF_ODR_DIV_4;
  md.fs = ILPS22QS_1260hPa;

  if (ilps22qs_mode_set(&(pObj->Ctx), &md) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  if (ilps22qs_mode_get(&(pObj->Ctx), &pObj->last_odr) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  pObj->last_odr.odr = ILPS22QS_25Hz;

  return ILPS22QS_OK;
}

/**
  * @brief  Set the ILPS22QS One Shot Mode
  * @param  pObj the device pObj
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_Set_One_Shot(ILPS22QS_Object_t *pObj)
{
  ilps22qs_md_t md;

  if (ilps22qs_mode_get(&(pObj->Ctx), &md) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  /* Start One Shot Measurement */
  if (ilps22qs_trigger_sw(&(pObj->Ctx), &md) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  return ILPS22QS_OK;
}

/**
  * @brief  Get the ILPS22QS One Shot Status
  * @param  pObj the device pObj
  * @param  Status pointer to the one shot status (1 means measurements available, 0 means measurements not available yet)
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_Get_One_Shot_Status(ILPS22QS_Object_t *pObj, uint8_t *Status)
{
  uint8_t p_da;
  uint8_t t_da;

  /* Get DataReady for pressure */
  if (ILPS22QS_PRESS_Get_DRDY_Status(pObj, &p_da) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  /* Get DataReady for temperature */
  if (ILPS22QS_TEMP_Get_DRDY_Status(pObj, &t_da) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  if (p_da && t_da)
  {
    *Status = 1;
  }
  else
  {
    *Status = 0;
  }

  return ILPS22QS_OK;
}

/**
  * @brief  Set the ILPS22QS averaging selection
  * @param  pObj the device pObj
  * @param  avg averaging selection to be set
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_Set_AVG(ILPS22QS_Object_t *pObj, uint8_t avg)
{
  ilps22qs_md_t md;

  if (avg > 7)
  {
    return ILPS22QS_ERROR;
  }

  if (ilps22qs_mode_get(&(pObj->Ctx), &md) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  switch (avg)
  {
    case 0:
    default:
      md.avg = ILPS22QS_4_AVG;
      break;
    case 1:
      md.avg = ILPS22QS_8_AVG;
      break;
    case 2:
      md.avg = ILPS22QS_16_AVG;
      break;
    case 3:
      md.avg = ILPS22QS_32_AVG;
      break;
    case 4:
      md.avg = ILPS22QS_64_AVG;
      break;
    case 5:
      md.avg = ILPS22QS_128_AVG;
      break;
    case 6:
      md.avg = ILPS22QS_256_AVG;
      break;
    case 7:
      md.avg = ILPS22QS_512_AVG;
      break;
  }

  if (ilps22qs_mode_set(&(pObj->Ctx), &md) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  return ILPS22QS_OK;
}

/**
  * @brief  Set the ILPS22QS low pass filter
  * @param  pObj the device pObj
  * @param  lpf low pass filter mode to be set
  * @retval 0 in case of success, an error code otherwise
  */
int32_t ILPS22QS_Set_LPF(ILPS22QS_Object_t *pObj, uint8_t lpf)
{
  ilps22qs_md_t md;

  if (lpf != 0 && lpf != 1 && lpf != 3)
  {
    return ILPS22QS_ERROR;
  }

  if (ilps22qs_mode_get(&(pObj->Ctx), &md) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  switch (lpf)
  {
    case 0:
    default:
      md.lpf = ILPS22QS_LPF_DISABLE;
      break;
    case 1:
      md.lpf = ILPS22QS_LPF_ODR_DIV_4;
      break;
    case 3:
      md.lpf = ILPS22QS_LPF_ODR_DIV_9;
      break;
  }

  if (ilps22qs_mode_set(&(pObj->Ctx), &md) != ILPS22QS_OK)
  {
    return ILPS22QS_ERROR;
  }

  return ILPS22QS_OK;
}

/**
  * @brief  Wrap Read register component function to Bus IO function
  * @param  Handle the device handler
  * @param  Reg the register address
  * @param  pData the stored data pointer
  * @param  Length the length
  * @retval 0 in case of success, an error code otherwise
  */
static int32_t ReadRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint16_t Length)
{
  ILPS22QS_Object_t *pObj = (ILPS22QS_Object_t *)Handle;

  return pObj->IO.ReadReg(pObj->IO.Address, Reg, pData, Length);
}

/**
  * @brief  Wrap Write register component function to Bus IO function
  * @param  Handle the device handler
  * @param  Reg the register address
  * @param  pData the stored data pointer
  * @param  Length the length
  * @retval 0 in case of success, an error code otherwise
  */
static int32_t WriteRegWrap(void *Handle, uint8_t Reg, uint8_t *pData, uint16_t Length)
{
  ILPS22QS_Object_t *pObj = (ILPS22QS_Object_t *)Handle;

  return pObj->IO.WriteReg(pObj->IO.Address, Reg, pData, Length);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
