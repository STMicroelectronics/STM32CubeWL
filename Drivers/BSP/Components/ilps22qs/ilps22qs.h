/**
  ******************************************************************************
  * @file    ilps22qs.h
  * @author  MEMS Software Solutions Team
  * @brief   ILPS22QS header driver file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ILPS22QS_H
#define ILPS22QS_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "ilps22qs_reg.h"
#include <string.h>

/** @addtogroup BSP BSP
  * @{
  */

/** @addtogroup Component Component
  * @{
  */

/** @addtogroup ILPS22QS ILPS22QS
  * @{
  */

/** @defgroup ILPS22QS_Exported_Types ILPS22QS Exported Types
  * @{
  */

typedef int32_t (*ILPS22QS_Init_Func)(void);
typedef int32_t (*ILPS22QS_DeInit_Func)(void);
typedef int32_t (*ILPS22QS_GetTick_Func)(void);
typedef int32_t (*ILPS22QS_WriteReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);
typedef int32_t (*ILPS22QS_ReadReg_Func)(uint16_t, uint16_t, uint8_t *, uint16_t);

typedef struct
{
  ILPS22QS_Init_Func          Init;
  ILPS22QS_DeInit_Func        DeInit;
  uint32_t                   BusType; /*0 means I2C, 1 means SPI 4-Wires, 2 means SPI-3-Wires */
  uint8_t                    Address;
  ILPS22QS_WriteReg_Func      WriteReg;
  ILPS22QS_ReadReg_Func       ReadReg;
  ILPS22QS_GetTick_Func       GetTick;
} ILPS22QS_IO_t;

typedef struct
{
  ILPS22QS_IO_t        IO;
  stmdev_ctx_t        Ctx;
  uint8_t             is_initialized;
  uint8_t             press_is_enabled;
  uint8_t             temp_is_enabled;
  ilps22qs_md_t        last_odr;
} ILPS22QS_Object_t;

typedef struct
{
  uint8_t Temperature;
  uint8_t Pressure;
  uint8_t Humidity;
  uint8_t LowPower;
  float   HumMaxOdr;
  float   TempMaxOdr;
  float   PressMaxOdr;
} ILPS22QS_Capabilities_t;

typedef struct
{
  int32_t (*Init)(ILPS22QS_Object_t *);
  int32_t (*DeInit)(ILPS22QS_Object_t *);
  int32_t (*ReadID)(ILPS22QS_Object_t *, uint8_t *);
  int32_t (*GetCapabilities)(ILPS22QS_Object_t *, ILPS22QS_Capabilities_t *);
} ILPS22QS_CommonDrv_t;

typedef struct
{
  int32_t (*Enable)(ILPS22QS_Object_t *);
  int32_t (*Disable)(ILPS22QS_Object_t *);
  int32_t (*GetOutputDataRate)(ILPS22QS_Object_t *, float *);
  int32_t (*SetOutputDataRate)(ILPS22QS_Object_t *, float);
  int32_t (*GetTemperature)(ILPS22QS_Object_t *, float *);
} ILPS22QS_TEMP_Drv_t;

typedef struct
{
  int32_t (*Enable)(ILPS22QS_Object_t *);
  int32_t (*Disable)(ILPS22QS_Object_t *);
  int32_t (*GetOutputDataRate)(ILPS22QS_Object_t *, float *);
  int32_t (*SetOutputDataRate)(ILPS22QS_Object_t *, float);
  int32_t (*GetPressure)(ILPS22QS_Object_t *, float *);
} ILPS22QS_PRESS_Drv_t;

typedef union
{
  int16_t i16bit[3];
  uint8_t u8bit[6];
} ilps22qs_axis3bit16_t;

typedef union
{
  int16_t i16bit;
  uint8_t u8bit[2];
} ilps22qs_axis1bit16_t;

typedef union
{
  int32_t i32bit[3];
  uint8_t u8bit[12];
} ilps22qs_axis3bit32_t;

typedef union
{
  int32_t i32bit;
  uint8_t u8bit[4];
} ilps22qs_axis1bit32_t;

/**
  * @}
  */

/** @defgroup ILPS22QS_Exported_Constants ILPS22QS Exported Constants
  * @{
  */

#define ILPS22QS_OK                0
#define ILPS22QS_ERROR            -1

#define ILPS22QS_I2C_BUS          0U
#define ILPS22QS_SPI_4WIRES_BUS   1U
#define ILPS22QS_SPI_3WIRES_BUS   2U

/**
  * @}
  */

/** @addtogroup ILPS22QS_Exported_Functions ILPS22QS Exported Functions
  * @{
  */

int32_t ILPS22QS_RegisterBusIO(ILPS22QS_Object_t *pObj, ILPS22QS_IO_t *pIO);
int32_t ILPS22QS_Init(ILPS22QS_Object_t *pObj);
int32_t ILPS22QS_DeInit(ILPS22QS_Object_t *pObj);
int32_t ILPS22QS_ReadID(ILPS22QS_Object_t *pObj, uint8_t *Id);
int32_t ILPS22QS_GetCapabilities(ILPS22QS_Object_t *pObj, ILPS22QS_Capabilities_t *Capabilities);
int32_t ILPS22QS_Get_Init_Status(ILPS22QS_Object_t *pObj, uint8_t *Status);

int32_t ILPS22QS_PRESS_Enable(ILPS22QS_Object_t *pObj);
int32_t ILPS22QS_PRESS_Disable(ILPS22QS_Object_t *pObj);
int32_t ILPS22QS_PRESS_GetOutputDataRate(ILPS22QS_Object_t *pObj, float *Odr);
int32_t ILPS22QS_PRESS_SetOutputDataRate(ILPS22QS_Object_t *pObj, float Odr);
int32_t ILPS22QS_PRESS_GetPressure(ILPS22QS_Object_t *pObj, float *Value);
int32_t ILPS22QS_PRESS_Get_DRDY_Status(ILPS22QS_Object_t *pObj, uint8_t *Status);

int32_t ILPS22QS_TEMP_Enable(ILPS22QS_Object_t *pObj);
int32_t ILPS22QS_TEMP_Disable(ILPS22QS_Object_t *pObj);
int32_t ILPS22QS_TEMP_GetOutputDataRate(ILPS22QS_Object_t *pObj, float *Odr);
int32_t ILPS22QS_TEMP_SetOutputDataRate(ILPS22QS_Object_t *pObj, float Odr);
int32_t ILPS22QS_TEMP_GetTemperature(ILPS22QS_Object_t *pObj, float *Value);
int32_t ILPS22QS_TEMP_Get_DRDY_Status(ILPS22QS_Object_t *pObj, uint8_t *Status);

int32_t ILPS22QS_Read_Reg(ILPS22QS_Object_t *pObj, uint8_t reg, uint8_t *Data);
int32_t ILPS22QS_Write_Reg(ILPS22QS_Object_t *pObj, uint8_t reg, uint8_t Data);

int32_t ILPS22QS_Set_One_Shot(ILPS22QS_Object_t *pObj);
int32_t ILPS22QS_Get_One_Shot_Status(ILPS22QS_Object_t *pObj, uint8_t *Status);

int32_t ILPS22QS_Set_AVG(ILPS22QS_Object_t *pObj, uint8_t avg);
int32_t ILPS22QS_Set_LPF(ILPS22QS_Object_t *pObj, uint8_t lpf);

/**
  * @}
  */

/** @addtogroup ILPS22QS_Exported_Variables ILPS22QS Exported Variables
  * @{
  */
extern ILPS22QS_CommonDrv_t ILPS22QS_COMMON_Driver;
extern ILPS22QS_PRESS_Drv_t ILPS22QS_PRESS_Driver;
extern ILPS22QS_TEMP_Drv_t ILPS22QS_TEMP_Driver;

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
