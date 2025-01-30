/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sigfox_mbwrapper.c
  * @author  MCD Application Team
  * @brief   This file implements the CM4 side wrapper of the Sigfox interface
  *          shared between M0 and M4.
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
#include "platform.h"
#include "sigfox_mbwrapper.h"
#include "platform.h"
#include "mbmux.h"
#include "mbmuxif_sys.h"
#include "msg_id.h"
#include "mbmuxif_sigfox.h"
#include "stm32_mem.h"
#include "sys_app.h"
#include "st_sigfox_api.h"
#include "sigfox_monarch_api.h"
#include "sgfx_app.h"
#include "sigfox_info.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#ifndef MAX
#define MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )
#endif /* MAX */
#define NB_ELEMENTS_MAX 3

#define SGFX_OPEN_SIZE       (sizeof(sfx_rc_t))
#define SGFX_SEND_SIZE       ((SGFX_MAX_UL_PAYLOAD_SIZE * sizeof(sfx_u8)) + (SGFX_MAX_DL_PAYLOAD_SIZE * sizeof(sfx_u8)))
#define SGFX_SET_CONFIG_SIZE (NB_ELEMENTS_MAX * sizeof(sfx_u32))
#define SGFX_START_TX_SIZE   (sizeof(sfx_modulation_type_t))
#define SGFX_GET_VER_SIZE    (sizeof(sfx_u32) + sizeof(sfx_u8))
#define SGFX_GET_PAC_SIZE    (PAC_LENGTH * sizeof(sfx_u8))
#define SGFX_MBWRAP_SHBUF_SIZE MAX(MAX(MAX(SGFX_OPEN_SIZE, SGFX_SEND_SIZE), MAX(SGFX_SET_CONFIG_SIZE, SGFX_START_TX_SIZE)), MAX(SGFX_GET_VER_SIZE, SGFX_GET_PAC_SIZE))

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief Sigfox buffer to exchange data between from CM4 to CM0+
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint8_t aSigfoxMbWrapShareBuffer[SGFX_MBWRAP_SHBUF_SIZE];

static sfx_u8(* app_cb)(sfx_u8 rc_bit_mask, sfx_s16 rssi);

static SigfoxCallback_t *callback_wrap;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
sfx_error_t SIGFOX_API_open(sfx_rc_t *rc)
{
  /* USER CODE BEGIN SIGFOX_API_open_1 */

  /* USER CODE END SIGFOX_API_open_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  if (rc == NULL)
  {
    return SFX_ERR_API_OPEN;
  }

  /* copy data from Cm4 stack memory to shared memory */
  UTIL_MEM_cpy_8((uint8_t *) aSigfoxMbWrapShareBuffer, (uint8_t *) rc, sizeof(sfx_rc_t));

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_START_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) aSigfoxMbWrapShareBuffer;
  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  /* copy back data from shared memory to Cm4 stack private memory */
  UTIL_MEM_cpy_8((uint8_t *) rc, (uint8_t *) aSigfoxMbWrapShareBuffer, sizeof(sfx_rc_t));

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_API_open_2 */

  /* USER CODE END SIGFOX_API_open_2 */
}

sfx_error_t SIGFOX_API_close(void)
{
  /* USER CODE BEGIN SIGFOX_API_close_1 */

  /* USER CODE END SIGFOX_API_close_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t ret;

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_STOP_ID;
  com_obj->ParamCnt = 0;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */
  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_API_close_2 */

  /* USER CODE END SIGFOX_API_close_2 */
}

sfx_error_t SIGFOX_API_send_frame(sfx_u8 *customer_data, sfx_u8 customer_data_length,
                                  sfx_u8 *customer_response, sfx_u8 tx_mode,
                                  sfx_bool initiate_downlink_flag)
{
  /* USER CODE BEGIN SIGFOX_API_send_frame_1 */

  /* USER CODE END SIGFOX_API_send_frame_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  if (customer_data == NULL)
  {
    return SFX_ERR_API_SEND_FRAME_DATA_PTR;
  }
  else if (customer_response == NULL)
  {
    return SFX_ERR_API_SEND_FRAME_RESPONSE_PTR;
  }

  /* copy data from Cm4 stack memory to shared memory */
  UTIL_MEM_cpy_8((uint8_t *) aSigfoxMbWrapShareBuffer, (uint8_t *) customer_data,
                 SGFX_MAX_UL_PAYLOAD_SIZE * sizeof(sfx_u8));
  UTIL_MEM_cpy_8((uint8_t *)(aSigfoxMbWrapShareBuffer + SGFX_MAX_UL_PAYLOAD_SIZE * sizeof(sfx_u8)),
                 (uint8_t *)customer_response, SGFX_MAX_DL_PAYLOAD_SIZE * sizeof(sfx_u8));

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_SEND_FRAME_DWNL_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) aSigfoxMbWrapShareBuffer;
  com_buffer[i++] = (uint32_t) customer_data_length;
  com_buffer[i++] = ((uint32_t) aSigfoxMbWrapShareBuffer + SGFX_MAX_UL_PAYLOAD_SIZE * sizeof(sfx_u8));
  com_buffer[i++] = (uint32_t) tx_mode;
  com_buffer[i++] = (uint32_t) initiate_downlink_flag;

  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  /* copy back data from shared memory to Cm4 stack memory */
  UTIL_MEM_cpy_8((uint8_t *) customer_data, (uint8_t *) aSigfoxMbWrapShareBuffer,
                 SGFX_MAX_UL_PAYLOAD_SIZE * sizeof(sfx_u8));
  UTIL_MEM_cpy_8((uint8_t *)customer_response,
                 (uint8_t *)(aSigfoxMbWrapShareBuffer + SGFX_MAX_UL_PAYLOAD_SIZE * sizeof(sfx_u8)),
                 SGFX_MAX_DL_PAYLOAD_SIZE * sizeof(sfx_u8));

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_API_send_frame_2 */

  /* USER CODE END SIGFOX_API_send_frame_2 */
}

sfx_error_t SIGFOX_API_send_bit(sfx_bool bit_value,
                                sfx_u8 *customer_response,
                                sfx_u8 tx_mode,
                                sfx_bool initiate_downlink_flag)
{
  /* USER CODE BEGIN SIGFOX_API_send_bit_1 */

  /* USER CODE END SIGFOX_API_send_bit_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  if (customer_response == NULL)
  {
    return SFX_ERR_API_SEND_FRAME_RESPONSE_PTR;
  }

  /* copy data from Cm4 stack memory to shared memory */
  UTIL_MEM_cpy_8((uint8_t *)aSigfoxMbWrapShareBuffer, (uint8_t *)customer_response,
                 SGFX_MAX_DL_PAYLOAD_SIZE * sizeof(sfx_u8));

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_SEND_BIT_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) bit_value;
  com_buffer[i++] = (uint32_t) aSigfoxMbWrapShareBuffer;
  com_buffer[i++] = (uint32_t) tx_mode;
  com_buffer[i++] = (uint32_t) initiate_downlink_flag;

  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  /* copy back data from shared memory to Cm4 stack memory */
  UTIL_MEM_cpy_8((uint8_t *)customer_response, (uint8_t *) aSigfoxMbWrapShareBuffer,
                 SGFX_MAX_DL_PAYLOAD_SIZE * sizeof(sfx_u8));

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_API_send_bit_2 */

  /* USER CODE END SIGFOX_API_send_bit_2 */
}

sfx_error_t SIGFOX_API_send_outofband(sfx_oob_enum_t oob_type)
{
  /* USER CODE BEGIN SIGFOX_API_send_outofband_1 */

  /* USER CODE END SIGFOX_API_send_outofband_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_SEND_OOB_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) oob_type;
  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_API_send_outofband_2 */

  /* USER CODE END SIGFOX_API_send_outofband_2 */
}

sfx_error_t SIGFOX_API_set_std_config(sfx_u32 config_words[NB_ELEMENTS_MAX],
                                      sfx_bool timer_enable)
{
  /* USER CODE BEGIN SIGFOX_API_set_std_config_1 */

  /* USER CODE END SIGFOX_API_set_std_config_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  /* copy data from Cm4 stack memory to shared memory */
  UTIL_MEM_cpy_8((uint8_t *) aSigfoxMbWrapShareBuffer, (uint8_t *) config_words, NB_ELEMENTS_MAX * sizeof(sfx_u32));

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_SET_STD_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) aSigfoxMbWrapShareBuffer;
  com_buffer[i++] = (uint32_t) timer_enable;

  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  /* copy back data from shared memory to Cm4 stack private memory */
  UTIL_MEM_cpy_8((uint8_t *) config_words, (uint8_t *) aSigfoxMbWrapShareBuffer, NB_ELEMENTS_MAX * sizeof(sfx_u32));

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_API_set_std_config_2 */

  /* USER CODE END SIGFOX_API_set_std_config_2 */
}

sfx_error_t SIGFOX_API_start_continuous_transmission(sfx_u32 frequency, sfx_modulation_type_t type)
{
  /* USER CODE BEGIN SIGFOX_API_start_continuous_transmission_1 */

  /* USER CODE END SIGFOX_API_start_continuous_transmission_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  /* copy data from Cm4 stack memory to shared memory */
  UTIL_MEM_cpy_8((uint8_t *) aSigfoxMbWrapShareBuffer, (uint8_t *) type, sizeof(sfx_modulation_type_t));

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_START_CONTINUOUS_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) frequency;
  com_buffer[i++] = (uint32_t) aSigfoxMbWrapShareBuffer;

  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  /* copy back data from shared memory to Cm4 stack private memory */
  UTIL_MEM_cpy_8((uint8_t *) type, (uint8_t *) aSigfoxMbWrapShareBuffer, sizeof(sfx_modulation_type_t));

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_API_start_continuous_transmission_2 */

  /* USER CODE END SIGFOX_API_start_continuous_transmission_2 */
}

sfx_error_t SIGFOX_API_stop_continuous_transmission(void)
{
  /* USER CODE BEGIN SIGFOX_API_stop_continuous_transmission_1 */

  /* USER CODE END SIGFOX_API_stop_continuous_transmission_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t ret;

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_STOP_CONTINUOUS_ID;
  com_obj->ParamCnt = 0;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */
  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_API_stop_continuous_transmission_2 */

  /* USER CODE END SIGFOX_API_stop_continuous_transmission_2 */
}

sfx_error_t SIGFOX_API_get_version(sfx_u8 **version, sfx_u8 *size, sfx_version_type_t type)
{
  /* USER CODE BEGIN SIGFOX_API_get_version_1 */

  /* USER CODE END SIGFOX_API_get_version_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  if (version == NULL)
  {
    return SFX_ERR_API_GET_VERSION;
  }

  /* copy data from Cm4 stack memory to shared memory */
  UTIL_MEM_cpy_8((uint8_t *) aSigfoxMbWrapShareBuffer, (uint8_t *) version, sizeof(sfx_u32));
  UTIL_MEM_cpy_8((uint8_t *)(aSigfoxMbWrapShareBuffer + sizeof(sfx_u32)), (uint8_t *) size, sizeof(sfx_u8));

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_GET_VERSION_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) aSigfoxMbWrapShareBuffer;
  com_buffer[i++] = (uint32_t)(aSigfoxMbWrapShareBuffer + sizeof(sfx_u32));
  com_buffer[i++] = (uint32_t) type;
  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  /* copy back data from shared memory to Cm4 stack private memory */
  UTIL_MEM_cpy_8((uint8_t *) version, (uint8_t *) aSigfoxMbWrapShareBuffer, sizeof(sfx_u32));
  UTIL_MEM_cpy_8((uint8_t *) size, (uint8_t *)(aSigfoxMbWrapShareBuffer + sizeof(sfx_u32)), sizeof(sfx_u8));

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_API_get_version_2 */

  /* USER CODE END SIGFOX_API_get_version_2 */
}

sfx_error_t SIGFOX_API_get_device_id(sfx_u8 *dev_id)
{
  /* USER CODE BEGIN SIGFOX_API_get_device_id_1 */

  /* USER CODE END SIGFOX_API_get_device_id_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  if (dev_id == NULL)
  {
    return SFX_ERR_INT_GET_DEVICE_ID;
  }

  /* copy data from Cm4 stack memory to shared memory */
  UTIL_MEM_cpy_8((uint8_t *) aSigfoxMbWrapShareBuffer, (uint8_t *) dev_id, sizeof(sfx_u32));

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_GET_DEVICE_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) aSigfoxMbWrapShareBuffer;
  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  /* copy back data from shared memory to Cm4 stack private memory */
  UTIL_MEM_cpy_8((uint8_t *) dev_id, (uint8_t *) aSigfoxMbWrapShareBuffer, sizeof(sfx_u32));

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_API_get_device_id_2 */

  /* USER CODE END SIGFOX_API_get_device_id_2 */
}

sfx_error_t SIGFOX_API_get_initial_pac(sfx_u8 *initial_pac)
{
  /* USER CODE BEGIN SIGFOX_API_get_initial_pac_1 */

  /* USER CODE END SIGFOX_API_get_initial_pac_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  if (initial_pac == NULL)
  {
    return SFX_ERR_API_GET_INITIAL_PAC;
  }

  /* copy data from Cm4 stack memory to shared memory */
  UTIL_MEM_cpy_8((uint8_t *) aSigfoxMbWrapShareBuffer, (uint8_t *) initial_pac, PAC_LENGTH * sizeof(sfx_u8));

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_GET_INIT_PAC_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) aSigfoxMbWrapShareBuffer;
  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  /* copy back data from shared memory to Cm4 stack private memory */
  UTIL_MEM_cpy_8((uint8_t *) initial_pac, (uint8_t *) aSigfoxMbWrapShareBuffer, PAC_LENGTH * sizeof(sfx_u8));

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_API_get_initial_pac_2 */

  /* USER CODE END SIGFOX_API_get_initial_pac_2 */
}

sfx_error_t SIGFOX_API_set_rc_sync_period(sfx_u16 rc_sync_period)
{
  /* USER CODE BEGIN SIGFOX_API_set_rc_sync_period_1 */

  /* USER CODE END SIGFOX_API_set_rc_sync_period_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_SET_RCSYNC_PERIOD_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) rc_sync_period;
  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_API_set_rc_sync_period_2 */

  /* USER CODE END SIGFOX_API_set_rc_sync_period_2 */
}

sfx_error_t ADDON_SIGFOX_RF_PROTOCOL_API_test_mode(sfx_rc_enum_t rc_enum, sfx_test_mode_t test_mode)
{
  /* USER CODE BEGIN ADDON_SIGFOX_RF_PROTOCOL_API_test_mode_1 */

  /* USER CODE END ADDON_SIGFOX_RF_PROTOCOL_API_test_mode_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_API_TEST_MODE_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) rc_enum;
  com_buffer[i++] = (uint32_t) test_mode;
  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN ADDON_SIGFOX_RF_PROTOCOL_API_test_mode_2 */

  /* USER CODE END ADDON_SIGFOX_RF_PROTOCOL_API_test_mode_2 */
}

sfx_error_t ST_ADDON_SIGFOX_RF_PROTOCOL_API_monarch_test_mode(sfx_rc_enum_t rc_enum, sfx_test_mode_t test_mode,
                                                              sfx_u8 rc_capabilities)
{
  /* USER CODE BEGIN ST_ADDON_SIGFOX_RF_PROTOCOL_API_monarch_test_mode_1 */

  /* USER CODE END ST_ADDON_SIGFOX_RF_PROTOCOL_API_monarch_test_mode_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_API_MONARCH_TEST_MODE_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) rc_enum;
  com_buffer[i++] = (uint32_t) test_mode;
  com_buffer[i++] = (uint32_t) rc_capabilities;
  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN ST_ADDON_SIGFOX_RF_PROTOCOL_API_monarch_test_mode_2 */

  /* USER CODE END ST_ADDON_SIGFOX_RF_PROTOCOL_API_monarch_test_mode_2 */
}

sfx_error_t SIGFOX_MONARCH_API_execute_rc_scan(sfx_u8 rc_capabilities_bit_mask, sfx_u16 timer,
                                               sfx_timer_unit_enum_t unit, sfx_u8(* app_callback_handler)(sfx_u8 rc_bit_mask, sfx_s16 rssi))
{
  /* USER CODE BEGIN SIGFOX_MONARCH_API_execute_rc_scan_1 */

  /* USER CODE END SIGFOX_MONARCH_API_execute_rc_scan_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t *com_buffer;
  uint16_t i = 0;
  uint32_t ret;

  if (app_callback_handler == NULL)
  {
    return SFX_ERR_MONARCH_API_EXECUTE_RC_SCAN_NULL_CALLBACK;
  }

  /*Init callback on M4 side*/
  app_cb = app_callback_handler;

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_API_MONARCH_TEST_EXEC_RC_SCAN_ID;
  com_buffer = com_obj->ParamBuf;
  com_buffer[i++] = (uint32_t) rc_capabilities_bit_mask;
  com_buffer[i++] = (uint32_t) timer;
  com_buffer[i++] = (uint32_t) unit;
  com_buffer[i++] = (uint32_t) app_cb;
  com_obj->ParamCnt = i;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */

  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_MONARCH_API_execute_rc_scan_2 */

  /* USER CODE END SIGFOX_MONARCH_API_execute_rc_scan_2 */
}

sfx_error_t SIGFOX_MONARCH_API_stop_rc_scan(void)
{
  /* USER CODE BEGIN SIGFOX_MONARCH_API_stop_rc_scan_1 */

  /* USER CODE END SIGFOX_MONARCH_API_stop_rc_scan_1 */
  MBMUX_ComParam_t *com_obj;
  uint32_t ret;

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_API_MONARCH_STOP_RC_SCAN_ID;
  com_obj->ParamCnt = 0;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */
  ret = com_obj->ReturnVal;
  return (sfx_error_t) ret;
  /* USER CODE BEGIN SIGFOX_MONARCH_API_stop_rc_scan_2 */

  /* USER CODE END SIGFOX_MONARCH_API_stop_rc_scan_2 */
}

void SigfoxInfo_Init(void)
{
  /* USER CODE BEGIN SigfoxInfo_Init_1 */

  /* USER CODE END SigfoxInfo_Init_1 */
  MBMUX_ComParam_t *com_obj;

  com_obj = MBMUXIF_GetSigfoxFeatureCmdComPtr();
  com_obj->MsgId = SIGFOX_INFO_INIT_ID;
  com_obj->ParamCnt = 0;
  MBMUXIF_SigfoxSendCmd();
  /* waiting for event */
  /* once event is received and semaphore released: */
  return;
  /* USER CODE BEGIN SigfoxInfo_Init_2 */

  /* USER CODE END SigfoxInfo_Init_2 */
}

SigfoxInfo_t *SigfoxInfo_GetPtr(void)
{
  /* USER CODE BEGIN SigfoxInfo_GetPtr_1 */

  /* USER CODE END SigfoxInfo_GetPtr_1 */
  FEAT_INFO_Param_t  *p_feature;

  p_feature = MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_SIGFOX_ID);
  return (SigfoxInfo_t *) p_feature->Feat_Info_Config_Ptr;
  /* USER CODE BEGIN SigfoxInfo_GetPtr_2 */

  /* USER CODE END SigfoxInfo_GetPtr_2 */
}

void Sigfox_Register(SigfoxCallback_t *SigfoxCallback)
{
  /* USER CODE BEGIN Sigfox_Register_1 */

  /* USER CODE END Sigfox_Register_1 */
  callback_wrap = SigfoxCallback;
  /* USER CODE BEGIN Sigfox_Register_2 */

  /* USER CODE END Sigfox_Register_2 */
}

void Process_Sigfox_Notif(MBMUX_ComParam_t *ComObj)
{
  /* USER CODE BEGIN Process_Sigfox_Notif_1 */

  /* USER CODE END Process_Sigfox_Notif_1 */
  uint32_t    cb_ret;

  APP_LOG(TS_ON, VLEVEL_H, "CM4 - Sigfox Notif received \n\r");

  /* process Command */
  switch (ComObj->MsgId)
  {
    /* callbacks */
    case SIGFOX_GET_BATTERY_LEVEL_CB_ID:
      if (callback_wrap->GetBatteryLevel != NULL)
      {
        cb_ret = (uint32_t) callback_wrap->GetBatteryLevel();
      }
      else
      {
        while (1) {}
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  cb_ret; /* */
      break;

    case SIGFOX_GET_TEMPERATURE_LEVEL_CB_ID:
      if (callback_wrap->GetTemperatureLevel != NULL)
      {
        cb_ret = (uint32_t) callback_wrap->GetTemperatureLevel();
      }
      else
      {
        while (1) {}
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  cb_ret; /* */
      break;

    case SIGFOX_MONARCH_RC_SCAN_CB_ID:
      if (app_cb != NULL)
      {
        cb_ret = (sfx_u8) app_cb((sfx_u8)ComObj->ParamBuf[0], (sfx_s16) ComObj->ParamBuf[1]);
      }
      else
      {
        while (1) {}
      }
      /* prepare response buffer */
      ComObj->ParamCnt = 0; /* reset ParamCnt */
      ComObj->ReturnVal =  cb_ret; /* */
      break;

    default:
      break;
  }

  /* send Ack */
  APP_LOG(TS_ON, VLEVEL_H,  "CM4 - Sigfox sending ack \n\r");
  MBMUX_AcknowledgeSnd(FEAT_INFO_SIGFOX_ID);
  /* USER CODE BEGIN Process_Sigfox_Notif_2 */

  /* USER CODE END Process_Sigfox_Notif_2 */
}

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
