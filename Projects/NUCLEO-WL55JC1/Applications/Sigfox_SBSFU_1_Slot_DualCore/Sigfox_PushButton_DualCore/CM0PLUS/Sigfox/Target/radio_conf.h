/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    radio_conf.h
  * @author  MCD Application Team
  * @brief   Header of Radio configuration
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RADIO_CONF_H__
#define __RADIO_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "subghz.h"
#include "stm32_mem.h"       /* RADIO_MEMSET8 def in this file */
#include "stm32_seq.h"
#include "mw_log_conf.h"     /* mw trace conf */
#include "radio_board_if.h"  /* low layer api (bsp) */
#include "utilities_def.h"  /* low layer api (bsp) */
#include "mbmuxif_sys.h"
#include "sys_debug.h"
/* USER CODE BEGIN include */

/* USER CODE END include */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/**
  * @brief Max payload buffer size
  */
#define RADIO_RX_BUF_SIZE          255

/**
  * @brief Get the chip revision ID from Mbmux table
  */
#define LL_DBGMCU_GetRevisionID() MBMUXIF_ChipRevId()

/**
  * @brief drive value used anytime radio is NOT in TX low power mode
  * @note override the default configuration of radio_driver.c
  */
#define SMPS_DRIVE_SETTING_DEFAULT  SMPS_DRV_40

/**
  * @brief drive value used anytime radio is in TX low power mode
  *        TX low power mode is the worst case because the PA sinks from SMPS
  *        while in high power mode, current is sunk directly from the battery
  * @note override the default configuration of radio_driver.c
  */
#define SMPS_DRIVE_SETTING_MAX      SMPS_DRV_60

/**
  * @brief Provides the frequency of the chip running on the radio and the frequency step
  * @remark These defines are used for computing the frequency divider to set the RF frequency
  * @note override the default configuration of radio_driver.c
  */
#define XTAL_FREQ                   ( 32000000UL )

/**
  * @brief in XO mode, set internal capacitor (from 0x00 to 0x2F starting 11.2pF with 0.47pF steps)
  * @note override the default configuration of radio_driver.c
  */
#define XTAL_DEFAULT_CAP_VALUE      ( 0x20UL )

/**
  * @brief voltage of vdd tcxo.
  * @note override the default configuration of radio_driver.c
  */
#define TCXO_CTRL_VOLTAGE           TCXO_CTRL_1_7V

/**
  * @brief Radio maximum wakeup time (in ms)
  * @note override the default configuration of radio_driver.c
  */
#define RF_WAKEUP_TIME              ( 15UL )

/**
  * @brief DCDC is enabled
  * @remark this define is only used if the DCDC is present on the board
  * @note override the default configuration of radio_driver.c
  */
#define DCDC_ENABLE                 ( 1UL )

#define RADIO_IRQ_PROCESS_INIT()   do{ UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_RadioIrq_Process), UTIL_SEQ_RFU, RadioIrqProcess); \
                                       UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_RadioRxTimeout_Process), UTIL_SEQ_RFU, RadioOnRxTimeoutProcess);\
                                       UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_RadioTxTimeout_Process), UTIL_SEQ_RFU, RadioOnTxTimeoutProcess);} while(0)

#define RADIO_IRQ_PROCESS()        do{ UTIL_SEQ_SetTask(1 << CFG_SEQ_Task_RadioIrq_Process, CFG_SEQ_Prio_0); } while(0)
#define RADIO_RX_TIMEOUT_PROCESS() do{ UTIL_SEQ_SetTask(1 << CFG_SEQ_Task_RadioRxTimeout_Process, CFG_SEQ_Prio_0); } while(0)
#define RADIO_TX_TIMEOUT_PROCESS() do{ UTIL_SEQ_SetTask(1 << CFG_SEQ_Task_RadioTxTimeout_Process, CFG_SEQ_Prio_0); } while(0)

/* USER CODE BEGIN EC */
/**
  * @brief Set RX pin to high or low level
  */
#define DBG_GPIO_RADIO_RX(set_rst) PROBE_GPIO_##set_rst##_LINE(PROBE_LINE1_PORT, PROBE_LINE1_PIN);

/**
  * @brief Set TX pin to high or low level
  */
#define DBG_GPIO_RADIO_TX(set_rst) PROBE_GPIO_##set_rst##_LINE(PROBE_LINE2_PORT, PROBE_LINE2_PIN);

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
#ifndef CRITICAL_SECTION_BEGIN
/**
  * @brief macro used to enter the critical section
  */
#define CRITICAL_SECTION_BEGIN( )      UTILS_ENTER_CRITICAL_SECTION( )
#endif /* !CRITICAL_SECTION_BEGIN */
#ifndef CRITICAL_SECTION_END
/**
  * @brief macro used to exit the critical section
  */
#define CRITICAL_SECTION_END( )        UTILS_EXIT_CRITICAL_SECTION( )
#endif /* !CRITICAL_SECTION_END */

/* Function mapping */
/**
  * @brief SUBGHZ interface init to radio Middleware
  */
#define RADIO_INIT                              MX_SUBGHZ_Init

/**
  * @brief Delay interface to radio Middleware
  */
#define RADIO_DELAY_MS                          HAL_Delay

/**
  * @brief Memset utilities interface to radio Middleware
  */
#define RADIO_MEMSET8( dest, value, size )      UTIL_MEM_set_8( dest, value, size )

/**
  * @brief Memcpy utilities interface to radio Middleware
  */
#define RADIO_MEMCPY8( dest, src, size )        UTIL_MEM_cpy_8( dest, src, size )

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __RADIO_CONF_H__*/
