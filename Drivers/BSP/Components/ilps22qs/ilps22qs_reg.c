/*
 ******************************************************************************
 * @file    ilps22qs_reg.c
 * @author  Sensors Software Solution Team
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

#include "ilps22qs_reg.h"

/**
  * @defgroup    ILPS22QS
  * @brief       This file provides a set of functions needed to drive the
  *              ilps22qs nano pressure sensor.
  * @{
  *
  */

/**
  * @defgroup    Interfaces_Functions
  * @brief       This section provide a set of functions used to read and
  *              write a generic register of the device.
  *              MANDATORY: return 0 -> no Error.
  * @{
  *
  */

/**
  * @brief  Read generic device register
  *
  * @param  ctx   read / write interface definitions(ptr)
  * @param  reg   register to read
  * @param  data  pointer to buffer that store the data read(ptr)
  * @param  len   number of consecutive register to read
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_read_reg(stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data,
                          uint16_t len)
{
  int32_t ret;
  ret = ctx->read_reg(ctx->handle, reg, data, len);
  return ret;
}

/**
  * @brief  Write generic device register
  *
  * @param  ctx   read / write interface definitions(ptr)
  * @param  reg   register to write
  * @param  data  pointer to data to write in register reg(ptr)
  * @param  len   number of consecutive register to write
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_write_reg(stmdev_ctx_t *ctx, uint8_t reg, uint8_t *data,
                           uint16_t len)
{
  int32_t ret;
  ret = ctx->write_reg(ctx->handle, reg, data, len);
  return ret;
}

/**
  * @}
  *
  */

/**
  * @defgroup  Private_functions
  * @brief     Section collect all the utility functions needed by APIs.
  * @{
  *
  */

static void bytecpy(uint8_t *target, uint8_t *source)
{
  if ((target != NULL) && (source != NULL))
  {
    *target = *source;
  }
}

/**
  * @}
  *
  */

/**
  * @defgroup    Sensitivity
  * @brief       These functions convert raw-data into engineering units.
  * @{
  *
  */

float_t ilps22qs_from_fs1260_to_hPa(int32_t lsb)
{
  return ((float_t)lsb / 1048576.0f);   /* 4096.0f * 256 */
}

float_t ilps22qs_from_fs4000_to_hPa(int32_t lsb)
{
  return ((float_t)lsb /  524288.0f);   /* 2048.0f * 256 */
}

float_t ilps22qs_from_lsb_to_celsius(int16_t lsb)
{
  return ((float_t)lsb / 100.0f);
}

/**
  * @}
  *
  */

/**
  * @defgroup    Basic functions
  * @brief       This section groups all the functions concerning device basic
  *              configuration.
  * @{
  *
  */

/**
  * @brief  Device "Who am I".[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   ID values.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_id_get(stmdev_ctx_t *ctx, ilps22qs_id_t *val)
{
  uint8_t reg;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_WHO_AM_I, &reg, 1);
  val->whoami = reg;

  return ret;
}

/**
  * @brief  Configures the bus operating mode.[set]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   configures the bus operating mode.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_bus_mode_set(stmdev_ctx_t *ctx, ilps22qs_bus_mode_t *val)
{
  ilps22qs_i3c_if_ctrl_add_t i3c_if_ctrl_add;
  ilps22qs_if_ctrl_t if_ctrl;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_IF_CTRL, (uint8_t *)&if_ctrl, 1);
  if (ret == 0)
  {
    if_ctrl.i2c_i3c_dis = ((uint8_t)val->interface & 0x02U) >> 1;
    if_ctrl.en_spi_read = ((uint8_t)val->interface & 0x01U);
    ret = ilps22qs_write_reg(ctx, ILPS22QS_IF_CTRL, (uint8_t *)&if_ctrl, 1);
  }
  if (ret == 0)
  {
    ret = ilps22qs_read_reg(ctx, ILPS22QS_I3C_IF_CTRL_ADD,
                            (uint8_t *)&i3c_if_ctrl_add, 1);
  }
  if (ret == 0)
  {
    i3c_if_ctrl_add.asf_on = (uint8_t)val->filter & 0x01U;
    ret = ilps22qs_write_reg(ctx, ILPS22QS_I3C_IF_CTRL_ADD,
                             (uint8_t *)&i3c_if_ctrl_add, 1);
  }
  return ret;
}

/**
  * @brief  Configures the bus operating mode.[set]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   configures the bus operating mode.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_bus_mode_get(stmdev_ctx_t *ctx, ilps22qs_bus_mode_t *val)
{
  ilps22qs_i3c_if_ctrl_add_t i3c_if_ctrl_add;
  ilps22qs_if_ctrl_t if_ctrl;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_IF_CTRL, (uint8_t *)&if_ctrl, 1);
  if (ret == 0)
  {
    ret = ilps22qs_read_reg(ctx, ILPS22QS_I3C_IF_CTRL_ADD,
                            (uint8_t *)&i3c_if_ctrl_add, 1);

    switch (if_ctrl.i2c_i3c_dis << 1)
    {
      case ILPS22QS_SEL_BY_HW:
        val->interface = ILPS22QS_SEL_BY_HW;
        break;
      case ILPS22QS_SPI_3W:
        val->interface = ILPS22QS_SPI_3W;
        break;
      case ILPS22QS_SPI_4W:
        val->interface = ILPS22QS_SPI_4W;
        break;
      default:
        val->interface = ILPS22QS_SEL_BY_HW;
        break;
    }

    switch (i3c_if_ctrl_add.asf_on)
    {
      case ILPS22QS_AUTO:
        val->filter = ILPS22QS_AUTO;
        break;
      case ILPS22QS_ALWAYS_ON:
        val->filter = ILPS22QS_ALWAYS_ON;
        break;
      default:
        val->filter = ILPS22QS_AUTO;
        break;
    }
  }
  return ret;
}

/**
  * @brief  Configures the bus operating mode.[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   configures the bus operating mode.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_init_set(stmdev_ctx_t *ctx, ilps22qs_init_t val)
{
  ilps22qs_ctrl_reg2_t ctrl_reg2;
  ilps22qs_ctrl_reg3_t ctrl_reg3;
  uint8_t reg[2];
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_CTRL_REG2, reg, 2);

  bytecpy((uint8_t *)&ctrl_reg2, &reg[0]);
  bytecpy((uint8_t *)&ctrl_reg3, &reg[1]);

  switch (val)
  {
    case ILPS22QS_BOOT:
      ctrl_reg2.boot = PROPERTY_ENABLE;
      ret = ilps22qs_write_reg(ctx, ILPS22QS_CTRL_REG2,
                               (uint8_t *)&ctrl_reg2, 1);
      break;
    case ILPS22QS_RESET:
      ctrl_reg2.swreset = PROPERTY_ENABLE;
      ret = ilps22qs_write_reg(ctx, ILPS22QS_CTRL_REG2,
                               (uint8_t *)&ctrl_reg2, 1);
      break;
    case ILPS22QS_DRV_RDY:
      ctrl_reg2.bdu = PROPERTY_ENABLE;
      ctrl_reg3.if_add_inc = PROPERTY_ENABLE;
      bytecpy(&reg[0], (uint8_t *)&ctrl_reg2);
      bytecpy(&reg[1], (uint8_t *)&ctrl_reg3);
      ret = ilps22qs_write_reg(ctx, ILPS22QS_CTRL_REG2, reg, 2);
      break;
    default:
      ctrl_reg2.swreset = PROPERTY_ENABLE;
      ret = ilps22qs_write_reg(ctx, ILPS22QS_CTRL_REG2,
                               (uint8_t *)&ctrl_reg2, 1);
      break;
  }
  return ret;
}

/**
  * @brief  Get the status of the device.[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   the status of the device.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_status_get(stmdev_ctx_t *ctx, ilps22qs_stat_t *val)
{
  ilps22qs_interrupt_cfg_t interrupt_cfg;
  ilps22qs_int_source_t int_source;
  ilps22qs_ctrl_reg2_t ctrl_reg2;
  ilps22qs_status_t status;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_CTRL_REG2,
                          (uint8_t *)&ctrl_reg2, 1);
  if (ret == 0)
  {
    ret = ilps22qs_read_reg(ctx, ILPS22QS_INT_SOURCE, (uint8_t *)&int_source, 1);
  }
  if (ret == 0)
  {
    ret = ilps22qs_read_reg(ctx, ILPS22QS_STATUS, (uint8_t *)&status, 1);
  }
  if (ret == 0)
  {
    ret = ilps22qs_read_reg(ctx, ILPS22QS_INTERRUPT_CFG,
                            (uint8_t *)&interrupt_cfg, 1);
  }
  val->sw_reset  = ctrl_reg2.swreset;
  val->boot      = int_source.boot_on;
  val->drdy_pres = status.p_da;
  val->drdy_temp = status.t_da;
  val->ovr_pres  = status.p_or;
  val->ovr_temp  = status.t_or;
  val->end_meas  = ~ctrl_reg2.oneshot;
  val->ref_done = ~interrupt_cfg.autozero;

  return ret;
}

/**
  * @brief  Electrical pin configuration.[set]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   the electrical settings for the configurable pins.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_pin_conf_set(stmdev_ctx_t *ctx, ilps22qs_pin_conf_t *val)
{
  ilps22qs_if_ctrl_t if_ctrl;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_IF_CTRL, (uint8_t *)&if_ctrl, 1);

  if (ret == 0)
  {
    if_ctrl.sda_pu_en = val->sda_pull_up;
    if_ctrl.cs_pu_dis = ~val->cs_pull_up;
    ret = ilps22qs_write_reg(ctx, ILPS22QS_IF_CTRL, (uint8_t *)&if_ctrl, 1);
  }

  return ret;
}

/**
  * @brief  Electrical pin configuration.[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   the electrical settings for the configurable pins.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_pin_conf_get(stmdev_ctx_t *ctx, ilps22qs_pin_conf_t *val)
{
  ilps22qs_if_ctrl_t if_ctrl;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_IF_CTRL, (uint8_t *)&if_ctrl, 1);

  val->sda_pull_up  = if_ctrl.sda_pu_en;
  val->cs_pull_up = ~if_ctrl.cs_pu_dis;

  return ret;
}

/**
  * @brief  Get the status of all the interrupt sources.[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   the status of all the interrupt sources.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_all_sources_get(stmdev_ctx_t *ctx,
                                 ilps22qs_all_sources_t *val)
{
  ilps22qs_fifo_status2_t fifo_status2;
  ilps22qs_int_source_t int_source;
  ilps22qs_status_t status;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_STATUS, (uint8_t *)&status, 1);
  if (ret == 0)
  {
    ret = ilps22qs_read_reg(ctx, ILPS22QS_INT_SOURCE,
                            (uint8_t *)&int_source, 1);
  }
  if (ret == 0)
  {
    ret = ilps22qs_read_reg(ctx, ILPS22QS_FIFO_STATUS2,
                            (uint8_t *)&fifo_status2, 1);
  }

  val->drdy_pres        = status.p_da;
  val->drdy_temp        = status.t_da;
  val->over_pres        = int_source.ph;
  val->under_pres       = int_source.pl;
  val->thrsld_pres      = int_source.ia;
  val->fifo_full        = fifo_status2.fifo_full_ia;
  val->fifo_ovr         = fifo_status2.fifo_ovr_ia;
  val->fifo_th          = fifo_status2.fifo_wtm_ia;

  return ret;
}


/**
  * @brief  Sensor conversion parameters selection.[set]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   set the sensor conversion parameters.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_mode_set(stmdev_ctx_t *ctx, ilps22qs_md_t *val)
{
  ilps22qs_ctrl_reg1_t ctrl_reg1;
  ilps22qs_ctrl_reg2_t ctrl_reg2;
  uint8_t reg[2];
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_CTRL_REG1, reg, 2);

  if (ret == 0)
  {
    bytecpy((uint8_t *)&ctrl_reg1, &reg[0]);
    bytecpy((uint8_t *)&ctrl_reg2, &reg[1]);

    ctrl_reg1.odr = (uint8_t)val->odr;
    ctrl_reg1.avg = (uint8_t)val->avg;
    ctrl_reg2.en_lpfp = (uint8_t)val->lpf & 0x01U;
    ctrl_reg2.lfpf_cfg = ((uint8_t)val->lpf & 0x02U) >> 2;
    ctrl_reg2.fs_mode = (uint8_t)val->fs;

    bytecpy(&reg[0], (uint8_t *)&ctrl_reg1);
    bytecpy(&reg[1], (uint8_t *)&ctrl_reg2);
    ret = ilps22qs_write_reg(ctx, ILPS22QS_CTRL_REG1, reg, 2);
  }

  return ret;
}

/**
  * @brief  Sensor conversion parameters selection.[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   get the sensor conversion parameters.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_mode_get(stmdev_ctx_t *ctx, ilps22qs_md_t *val)
{
  ilps22qs_ctrl_reg1_t ctrl_reg1;
  ilps22qs_ctrl_reg2_t ctrl_reg2;
  uint8_t reg[2];
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_CTRL_REG1, reg, 2);

  if (ret == 0)
  {
    bytecpy((uint8_t *)&ctrl_reg1, &reg[0]);
    bytecpy((uint8_t *)&ctrl_reg2, &reg[1]);

    switch (ctrl_reg2.fs_mode)
    {
      case ILPS22QS_1260hPa:
        val->fs = ILPS22QS_1260hPa;
        break;
      case ILPS22QS_4000hPa:
        val->fs = ILPS22QS_4000hPa;
        break;
      default:
        val->fs = ILPS22QS_1260hPa;
        break;
    }

    switch (ctrl_reg1.odr)
    {
      case ILPS22QS_ONE_SHOT:
        val->odr = ILPS22QS_ONE_SHOT;
        break;
      case ILPS22QS_1Hz:
        val->odr = ILPS22QS_1Hz;
        break;
      case ILPS22QS_4Hz:
        val->odr = ILPS22QS_4Hz;
        break;
      case ILPS22QS_10Hz:
        val->odr = ILPS22QS_10Hz;
        break;
      case ILPS22QS_25Hz:
        val->odr = ILPS22QS_25Hz;
        break;
      case ILPS22QS_50Hz:
        val->odr = ILPS22QS_50Hz;
        break;
      case ILPS22QS_75Hz:
        val->odr = ILPS22QS_75Hz;
        break;
      case ILPS22QS_100Hz:
        val->odr = ILPS22QS_100Hz;
        break;
      case ILPS22QS_200Hz:
        val->odr = ILPS22QS_200Hz;
        break;
      default:
        val->odr = ILPS22QS_ONE_SHOT;
        break;
    }

    switch (ctrl_reg1.avg)
    {
      case ILPS22QS_4_AVG:
        val->avg = ILPS22QS_4_AVG;
        break;
      case ILPS22QS_8_AVG:
        val->avg = ILPS22QS_8_AVG;
        break;
      case ILPS22QS_16_AVG:
        val->avg = ILPS22QS_16_AVG;
        break;
      case ILPS22QS_32_AVG:
        val->avg = ILPS22QS_32_AVG;
        break;
      case ILPS22QS_64_AVG:
        val->avg = ILPS22QS_64_AVG;
        break;
      case ILPS22QS_128_AVG:
        val->avg = ILPS22QS_128_AVG;
        break;
      case ILPS22QS_256_AVG:
        val->avg = ILPS22QS_256_AVG;
        break;
      case ILPS22QS_512_AVG:
        val->avg = ILPS22QS_512_AVG;
        break;
      default:
        val->avg = ILPS22QS_4_AVG;
        break;
    }

    switch ((ctrl_reg2.lfpf_cfg << 2) | ctrl_reg2.en_lpfp)
    {
      case ILPS22QS_LPF_DISABLE:
        val->lpf = ILPS22QS_LPF_DISABLE;
        break;
      case ILPS22QS_LPF_ODR_DIV_4:
        val->lpf = ILPS22QS_LPF_ODR_DIV_4;
        break;
      case ILPS22QS_LPF_ODR_DIV_9:
        val->lpf = ILPS22QS_LPF_ODR_DIV_9;
        break;
      default:
        val->lpf = ILPS22QS_LPF_DISABLE;
        break;
    }
  }
  return ret;
}

/**
  * @brief  Software trigger for One-Shot.[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  md    the sensor conversion parameters.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_trigger_sw(stmdev_ctx_t *ctx, ilps22qs_md_t *md)
{
  ilps22qs_ctrl_reg2_t ctrl_reg2;
  int32_t ret = 0;

  if (md->odr == ILPS22QS_ONE_SHOT)
  {
    ret = ilps22qs_read_reg(ctx, ILPS22QS_CTRL_REG2, (uint8_t *)&ctrl_reg2, 1);
    ctrl_reg2.oneshot = PROPERTY_ENABLE;
    if (ret == 0)
    {
      ret = ilps22qs_write_reg(ctx, ILPS22QS_CTRL_REG2, (uint8_t *)&ctrl_reg2, 1);
    }
  }
  return ret;
}

/**
  * @brief  AH/QVAR function enable.[set]
  *
  * @param  ctx    Read / write interface definitions
  * @param  val    Change the value of ah_qvar_en in reg CTRL_REG3
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
int32_t ilps22qs_ah_qvar_en_set(stmdev_ctx_t *ctx, uint8_t val)
{
  ilps22qs_ctrl_reg3_t ctrl_reg3;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_CTRL_REG3, (uint8_t *)&ctrl_reg3, 1);

  if (ret == 0)
  {
    ctrl_reg3.ah_qvar_en = val;
    ret = ilps22qs_write_reg(ctx, ILPS22QS_CTRL_REG3, (uint8_t *)&ctrl_reg3, 1);
  }

  return ret;
}

/**
  * @brief  AH/QVAR function enable.[get]
  *
  * @param  ctx    Read / write interface definitions
  * @param  val    Return the value of ah_qvar_en in reg CTRL_REG3
  * @retval        Interface status (MANDATORY: return 0 -> no Error).
  *
  */
int32_t ilps22qs_ah_qvar_en_get(stmdev_ctx_t *ctx, uint8_t *val)
{
  ilps22qs_ctrl_reg3_t ctrl_reg3;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_CTRL_REG3, (uint8_t *)&ctrl_reg3, 1);
  *val = ctrl_reg3.ah_qvar_en;

  return ret;
}

/**
  * @brief  Software trigger for One-Shot.[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  md    the sensor conversion parameters.(ptr)
  * @param  data  data retrieved from the sensor.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_data_get(stmdev_ctx_t *ctx, ilps22qs_md_t *md,
                          ilps22qs_data_t *data)
{
  uint8_t buff[5];
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_PRESS_OUT_XL, buff, 5);

  /* pressure conversion */
  data->pressure.raw = (int32_t)buff[2];
  data->pressure.raw = (data->pressure.raw * 256) + (int32_t) buff[1];
  data->pressure.raw = (data->pressure.raw * 256) + (int32_t) buff[0];
  data->pressure.raw = data->pressure.raw * 256;

  switch (md->fs)
  {
    case ILPS22QS_1260hPa:
      data->pressure.hpa = ilps22qs_from_fs1260_to_hPa(data->pressure.raw);
      break;
    case ILPS22QS_4000hPa:
      data->pressure.hpa = ilps22qs_from_fs4000_to_hPa(data->pressure.raw);
      break;
    default:
      data->pressure.hpa = 0.0f;
      break;
  }

  /* temperature conversion */
  data->heat.raw = (int16_t)buff[4];
  data->heat.raw = (data->heat.raw * 256) + (int16_t) buff[3];
  data->heat.deg_c = ilps22qs_from_lsb_to_celsius(data->heat.raw);

  return ret;
}

/**
  * @brief  AH/QVAR data read.[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  md    the sensor conversion parameters.(ptr)
  * @param  data  data retrieved from the sensor.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_ah_qvar_data_get(stmdev_ctx_t *ctx,
                                  ilps22qs_ah_qvar_data_t *data)
{
  uint8_t buff[5];
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_PRESS_OUT_XL, buff, 3);

  /* QVAR conversion */
  data->raw = (int32_t)buff[2];
  data->raw = (data->raw * 256) + (int32_t) buff[1];
  data->raw = (data->raw * 256) + (int32_t) buff[0];
  data->raw = (data->raw * 256);
  data->lsb = (data->raw / 256); /* shift 8bit left */

  return ret;
}

/**
  * @}
  *
  */

/**
  * @defgroup     FIFO functions
  * @brief        This section groups all the functions concerning the
  *               management of FIFO.
  * @{
  *
  */

/**
  * @brief  FIFO operation mode selection.[set]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   set the FIFO operation mode.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_fifo_mode_set(stmdev_ctx_t *ctx, ilps22qs_fifo_md_t *val)
{
  ilps22qs_fifo_ctrl_t fifo_ctrl;
  ilps22qs_fifo_wtm_t fifo_wtm;
  uint8_t reg[2];
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_FIFO_CTRL, reg, 2);
  if (ret == 0)
  {
    bytecpy((uint8_t *)&fifo_ctrl, &reg[0]);
    bytecpy((uint8_t *)&fifo_wtm, &reg[1]);

    fifo_ctrl.f_mode = (uint8_t)val->operation & 0x03U;
    fifo_ctrl.trig_modes = ((uint8_t)val->operation & 0x04) >> 2;

    if (val->watermark != 0x00U)
    {
      fifo_ctrl.stop_on_wtm = PROPERTY_ENABLE;
    }
    else
    {
      fifo_ctrl.stop_on_wtm = PROPERTY_DISABLE;
    }

    fifo_wtm.wtm = val->watermark;

    bytecpy(&reg[0], (uint8_t *)&fifo_ctrl);
    bytecpy(&reg[1], (uint8_t *)&fifo_wtm);

    ret = ilps22qs_write_reg(ctx, ILPS22QS_FIFO_CTRL, reg, 2);
  }
  return ret;
}

/**
  * @brief  FIFO operation mode selection.[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   get the FIFO operation mode.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_fifo_mode_get(stmdev_ctx_t *ctx, ilps22qs_fifo_md_t *val)
{
  ilps22qs_fifo_ctrl_t fifo_ctrl;
  ilps22qs_fifo_wtm_t fifo_wtm;
  uint8_t reg[2];
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_FIFO_CTRL, reg, 2);

  bytecpy((uint8_t *)&fifo_ctrl, &reg[0]);
  bytecpy((uint8_t *)&fifo_wtm, &reg[1]);

  switch ((fifo_ctrl.trig_modes << 2) | fifo_ctrl.f_mode)
  {
    case ILPS22QS_BYPASS:
      val->operation = ILPS22QS_BYPASS;
      break;
    case ILPS22QS_FIFO:
      val->operation = ILPS22QS_FIFO;
      break;
    case ILPS22QS_STREAM:
      val->operation = ILPS22QS_STREAM;
      break;
    case ILPS22QS_STREAM_TO_FIFO:
      val->operation = ILPS22QS_STREAM_TO_FIFO;
      break;
    case ILPS22QS_BYPASS_TO_STREAM:
      val->operation = ILPS22QS_BYPASS_TO_STREAM;
      break;
    case ILPS22QS_BYPASS_TO_FIFO:
      val->operation = ILPS22QS_BYPASS_TO_FIFO;
      break;
    default:
      val->operation = ILPS22QS_BYPASS;
      break;
  }

  val->watermark = fifo_wtm.wtm;

  return ret;
}

/**
  * @brief  Get the number of samples stored in FIFO.[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  md    the sensor conversion parameters.(ptr)
  * @param  val   number of samples stored in FIFO.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_fifo_level_get(stmdev_ctx_t *ctx, uint8_t *val)
{
  ilps22qs_fifo_status1_t fifo_status1;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_FIFO_STATUS1,
                          (uint8_t *)&fifo_status1, 1);

  *val = fifo_status1.fss;

  return ret;
}

/**
  * @brief  Software trigger for One-Shot.[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  md    the sensor conversion parameters.(ptr)
  * @param  fmd   get the FIFO operation mode.(ptr)
  * @param  samp  number of samples stored in FIFO.(ptr)
  * @param  data  data retrieved from FIFO.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_fifo_data_get(stmdev_ctx_t *ctx, uint8_t samp,
                               ilps22qs_md_t *md, ilps22qs_fifo_data_t *data)
{
  uint8_t fifo_data[3];
  uint8_t i;
  int32_t ret = 0;

  for (i = 0U; i < samp; i++)
  {
    ret = ilps22qs_read_reg(ctx, ILPS22QS_FIFO_DATA_OUT_PRESS_XL, fifo_data, 3);
    data[i].raw = (int16_t)fifo_data[2];
    data[i].raw = (data[i].raw * 256) + fifo_data[1];
    data[i].raw = (data[i].raw * 256) + fifo_data[0];
    data[i].raw = (data[i].raw * 256);

    switch (md->fs)
    {
      case ILPS22QS_1260hPa:
        data[i].hpa = ilps22qs_from_fs1260_to_hPa(data[i].raw);
        break;
      case ILPS22QS_4000hPa:
        data[i].hpa = ilps22qs_from_fs4000_to_hPa(data[i].raw);
        break;
      default:
        data[i].hpa = 0.0f;
        break;
    }

  }

  return ret;
}

/**
  * @}
  *
  */

/**
  * @defgroup     Interrupt signals
  * @brief        This section groups all the functions concerning
  *               the management of interrupt signals.
  * @{
  *
  */

/**
  * @brief  Interrupt pins hardware signal configuration.[set]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   the pins hardware signal settings.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_interrupt_mode_set(stmdev_ctx_t *ctx,
                                    ilps22qs_int_mode_t *val)
{
  ilps22qs_interrupt_cfg_t interrupt_cfg;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_INTERRUPT_CFG,
                          (uint8_t *)&interrupt_cfg, 1);
  if (ret == 0)
  {
    interrupt_cfg.lir = val->int_latched ;
    ret = ilps22qs_write_reg(ctx, ILPS22QS_INTERRUPT_CFG,
                             (uint8_t *)&interrupt_cfg, 1);
  }
  return ret;
}

/**
  * @brief  Interrupt pins hardware signal configuration.[get]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   the pins hardware signal settings.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_interrupt_mode_get(stmdev_ctx_t *ctx,
                                    ilps22qs_int_mode_t *val)
{
  ilps22qs_interrupt_cfg_t interrupt_cfg;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_INTERRUPT_CFG,
                          (uint8_t *)&interrupt_cfg, 1);

  val->int_latched = interrupt_cfg.lir;

  return ret;
}

/**
  * @brief  AH function disable
  *
  * @param  ctx   communication interface handler.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_ah_qvar_disable(stmdev_ctx_t *ctx)
{
  uint32_t val = 0;
  int32_t ret;

  ret = ilps22qs_write_reg(ctx, ILPS22QS_ANALOGIC_HUB_DISABLE, (uint8_t *)&val, 1);

  return ret;
}

/**
  * @}
  *
  */

/**
  * @defgroup     Interrupt on threshold functions
  * @brief        This section groups all the functions concerning
  *               the wake up functionality.
  * @{
  *
  */

/**
  * @brief  Configuration of Wake-up and Wake-up to Sleep .[set]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   parameters of configuration.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_int_on_threshold_mode_set(stmdev_ctx_t *ctx,
                                           ilps22qs_int_th_md_t *val)
{
  ilps22qs_interrupt_cfg_t interrupt_cfg;
  ilps22qs_ths_p_l_t ths_p_l;
  ilps22qs_ths_p_h_t ths_p_h;
  uint8_t reg[3];
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_INTERRUPT_CFG, reg, 3);
  if (ret == 0)
  {
    bytecpy((uint8_t *)&interrupt_cfg, &reg[0]);
    bytecpy((uint8_t *)&ths_p_l, &reg[1]);
    bytecpy((uint8_t *)&ths_p_h, &reg[2]);

    interrupt_cfg.phe = val->over_th;
    interrupt_cfg.ple = val->under_th;
    ths_p_h.ths = (uint8_t)(val->threshold / 256U);
    ths_p_l.ths = (uint8_t)(val->threshold - (ths_p_h.ths * 256U));

    bytecpy(&reg[0], (uint8_t *)&interrupt_cfg);
    bytecpy(&reg[1], (uint8_t *)&ths_p_l);
    bytecpy(&reg[2], (uint8_t *)&ths_p_h);

    ret = ilps22qs_read_reg(ctx, ILPS22QS_INTERRUPT_CFG, reg, 3);
  }
  return ret;
}

/**
  * @brief  Configuration of Wake-up and Wake-up to Sleep .[set]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   parameters of configuration.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_int_on_threshold_mode_get(stmdev_ctx_t *ctx,
                                           ilps22qs_int_th_md_t *val)
{
  ilps22qs_interrupt_cfg_t interrupt_cfg;
  ilps22qs_ths_p_l_t ths_p_l;
  ilps22qs_ths_p_h_t ths_p_h;
  uint8_t reg[3];
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_INTERRUPT_CFG, reg, 3);

  bytecpy((uint8_t *)&interrupt_cfg, &reg[0]);
  bytecpy((uint8_t *)&ths_p_l, &reg[1]);
  bytecpy((uint8_t *)&ths_p_h, &reg[2]);

  val->over_th = interrupt_cfg.phe;
  val->under_th = interrupt_cfg.ple;
  val->threshold = ths_p_h.ths;
  val->threshold = (val->threshold * 256U)  + ths_p_l.ths;

  return ret;
}

/**
  * @}
  *
  */

/**
  * @defgroup     Reference value of pressure
  * @brief        This section groups all the functions concerning
  *               the wake up functionality.
  * @{
  *
  */

/**
  * @brief  Configuration of Wake-up and Wake-up to Sleep .[set]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   parameters of configuration.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_reference_mode_set(stmdev_ctx_t *ctx, ilps22qs_ref_md_t *val)
{
  ilps22qs_interrupt_cfg_t interrupt_cfg;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_INTERRUPT_CFG,
                          (uint8_t *)&interrupt_cfg, 1);
  if (ret == 0)
  {

    interrupt_cfg.autozero = val->get_ref;
    interrupt_cfg.autorefp = (uint8_t)val->apply_ref & 0x01U;

    interrupt_cfg.reset_az  = ((uint8_t)val->apply_ref & 0x02U) >> 1;
    interrupt_cfg.reset_arp = ((uint8_t)val->apply_ref & 0x02U) >> 1;

    ret = ilps22qs_read_reg(ctx, ILPS22QS_INTERRUPT_CFG,
                            (uint8_t *)&interrupt_cfg, 1);
  }
  return ret;
}

/**
  * @brief  Configuration of Wake-up and Wake-up to Sleep .[set]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   parameters of configuration.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_reference_mode_get(stmdev_ctx_t *ctx, ilps22qs_ref_md_t *val)
{
  ilps22qs_interrupt_cfg_t interrupt_cfg;
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_INTERRUPT_CFG,
                          (uint8_t *)&interrupt_cfg, 1);

  switch ((interrupt_cfg.reset_az << 1) |
          interrupt_cfg.autorefp)
  {
    case ILPS22QS_OUT_AND_INTERRUPT:
      val->apply_ref = ILPS22QS_OUT_AND_INTERRUPT;
      break;
    case ILPS22QS_ONLY_INTERRUPT:
      val->apply_ref = ILPS22QS_ONLY_INTERRUPT;
      break;
    default:
      val->apply_ref = ILPS22QS_RST_REFS;
      break;
  }
  val->get_ref = interrupt_cfg.autozero;

  return ret;
}


/**
  * @brief  Configuration of Wake-up and Wake-up to Sleep .[set]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   parameters of configuration.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_opc_set(stmdev_ctx_t *ctx, int16_t val)
{
  uint8_t reg[2];
  int32_t ret;

  reg[1] = (val & 0xFF00U) / 256;
  reg[0] = val & 0x00FFU ;

  ret = ilps22qs_write_reg(ctx, ILPS22QS_RPDS_L, reg, 2);

  return ret;
}

/**
  * @brief  Configuration of Wake-up and Wake-up to Sleep .[set]
  *
  * @param  ctx   communication interface handler.(ptr)
  * @param  val   parameters of configuration.(ptr)
  * @retval       interface status (MANDATORY: return 0 -> no Error)
  *
  */
int32_t ilps22qs_opc_get(stmdev_ctx_t *ctx, int16_t *val)
{
  uint8_t reg[2];
  int32_t ret;

  ret = ilps22qs_read_reg(ctx, ILPS22QS_RPDS_L, reg, 2);

  *val = reg[1];
  *val = *val * 256 + reg[0];

  return ret;
}


/**
  * @}
  *
  */

/**
  * @}
  *
  */
