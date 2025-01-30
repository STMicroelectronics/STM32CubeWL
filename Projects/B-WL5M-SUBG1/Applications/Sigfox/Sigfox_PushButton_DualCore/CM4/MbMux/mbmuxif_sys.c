/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    mbmuxif_sys.c
  * @author  MCD Application Team
  * @brief   allows CM4 application to handle the SYSTEM MBMUX channel.
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
#include "mbmuxif_sys.h"
#include "stm32_mem.h"
#include "stm32_seq.h"
#include "stm32_timer.h"
#include "sys_app.h"
#include "msg_id.h"
#include "utilities_def.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static FLASH_OBProgramInitTypeDef OptionsBytesStruct;
static __IO uint8_t AllowSequencerForSysCmd = 0;
static __IO uint8_t MbSystemRespRcvFlag;

UTIL_MEM_PLACE_IN_SECTION("MAPPING_TABLE") static  MBMUX_ComTable_t MBSYS_RefTable UTIL_MEM_ALIGN(16);

MBMUX_ComTable_t *pMb_RefTable = &MBSYS_RefTable;
MBMUX_ComParam_t *SystemComObj;

UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aSystemCmdRespBuff[MAX_PARAM_OF_SYS_CMD_FUNCTIONS];/*shared*/
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aSystemNotifAckBuff[MAX_PARAM_OF_SYS_NOTIF_FUNCTIONS];/*shared*/
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aSystemPrioACmdRespBuff[MAX_PARAM_OF_SYS_PRIOA_CMD_FUNCTIONS];/*shared*/
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t
aSystemPrioANotifAckBuff[MAX_PARAM_OF_SYS_PRIOA_NOTIF_FUNCTIONS];/*shared*/
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t aSystemPrioBCmdRespBuff[MAX_PARAM_OF_SYS_PRIOB_CMD_FUNCTIONS];/*shared*/
UTIL_MEM_PLACE_IN_SECTION("MB_MEM1") uint32_t
aSystemPrioBNotifAckBuff[MAX_PARAM_OF_SYS_PRIOB_NOTIF_FUNCTIONS];/*shared*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  SYSTEM response callbacks: set event to release waiting task
  * @param  ComObj pointer to the SYSTEM com param buffer
  */
static void MBMUXIF_IsrSystemRespRcvCb(void *ComObj);

/**
  * @brief  SYSTEM notification callbacks: schedules a task in order to quit the ISR
  * @param  ComObj pointer to the SYSTEM com param buffer
  */
static void MBMUXIF_IsrSystemNotifRcvCb(void *ComObj);

/**
  * @brief  SYSTEM task to process the notification
  */
static void MBMUXIF_TaskSystemNotifRcv(void);

/**
  * @brief  SYSTEM PRIO_A response callbacks: available for new usage
  * @param  ComObj pointer to the SYSTEM com param buffer
  */
static void MBMUXIF_IsrSystemPrioARespRcvCb(void *ComObj);

/**
  * @brief  SYSTEM PRIO_A notification callbacks: used by RTC
  * @param  ComObj pointer to the SYSTEM com param buffer
  */
static void MBMUXIF_IsrSystemPrioANotifRcvCb(void *ComObj);

/**
  * @brief  SYSTEM PRIO_B response callbacks: available for new usage
  * @param  ComObj pointer to the SYSTEM com param buffer
  */
static void MBMUXIF_IsrSystemPrioBRespRcvCb(void *ComObj);

/**
  * @brief  SYSTEM PRIO_B notification callbacks: available for new usage
  * @param  ComObj pointer to the SYSTEM com param buffer
  */
static void MBMUXIF_IsrSystemPrioBNotifRcvCb(void *ComObj);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
int8_t MBMUXIF_SystemInit(void)
{
  int8_t ret = 0;
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* USER CODE BEGIN MBMUXIF_SystemInit_1 */

  /* USER CODE END MBMUXIF_SystemInit_1 */

  /* Get Chip Revision Identifier (cut) */
  MBSYS_RefTable.ChipRevId = LL_DBGMCU_GetRevisionID();
  /* Get the Dual boot configuration status */
  HAL_FLASHEx_OBGetConfig(&OptionsBytesStruct);

  if (OptionsBytesStruct.IPCCdataBufAddr != (uint32_t) pMb_RefTable)
  {
    APP_PPRINTF("There is a difference between the MAPPING_TABLE placement in memory: 0x%X \r\n",
                (uint32_t) pMb_RefTable);
    APP_PPRINTF("and the address calculated according to the IPCCDBA option byte: 0x%X \r\n",
                OptionsBytesStruct.IPCCdataBufAddr);
    APP_PPRINTF("IPCCDBA is automatically updated\n\rSystem restarting...\r\n\r\n");
    APP_PPRINTF("Please check the CM4\\MbMux\\mbmuxif_sys.c for more info \r\n\r\n");

    while (1 != UTIL_ADV_TRACE_IsBufferEmpty())
    {
      /* Wait that all printfs are completed*/
    }

    /* Next code reprograms the IPCCDBA option byte which automatically resets the chip. */
    /* It might be that this is not what you want. In that case check your linker file. */
    /* en.STM32CubeWL.zip example places MAPPING_TABLE in SRAM2 (specifically to 0x20008000) */
    /* due to its memory retention properties requested for Low Power STANDBY mode. */
    /* This address also matches the factory default value of IPCCDBA (i.e. 0x800) */
    /* In fact IPCCdataBufAddr = RAM_BASE_ADDR + IPCCDBA<<0x800 is 0x20008000. */
    /* On the other hand STM32CubeMX generates standard linker files which don't define MAPPING_TABLE */
    /* You can get inspired for your linker file by the ones in the Repository at the correspondent Apps: */
    /*    C:\Users\<username>\STM32Cube\Repository\STM32Cube_FW_WL_V1.0.0\Projects\<board>\Applications\... */
    /* When using "repository linker file" next code is not supposed to be executed */

    if (MBSYS_RefTable.ChipRevId == 0x1001)
    {
      /* w.a. for cut 1.1 : problems to write on FLASH when MSI clock with frq > 16Mhz*/
      HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
      RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_8;
      if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
      {
        Error_Handler();
      }
    }

    /* write FLASH->IPCCBR option byte */
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();
    /* Clear OPTVERR bit set on virgin samples */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    /* Unlock the Options Bytes *************************************************/
    HAL_FLASH_OB_Unlock();

    /*Write IPCCDBA Option Byte based on (FLASH_IPCC_data_buffer - SRAM1_BASE) >> 4*/
    OptionsBytesStruct.OptionType = OPTIONBYTE_IPCC_BUF_ADDR;
    OptionsBytesStruct.IPCCdataBufAddr = (uint32_t) pMb_RefTable;

    if (HAL_FLASHEx_OBProgram(&OptionsBytesStruct) != HAL_OK)
    {
      /*
      Error occurred while setting option bytes configuration.
      User can add here some code to deal with this error.
      To know the code error, user can call function 'HAL_FLASH_GetError()'
      */
      Error_Handler();
    }

    /* Start the Option Bytes programming process */
    if (HAL_FLASH_OB_Launch() != HAL_OK)
    {
      /*
      Error occurred while reloading option bytes configuration.
      User can add here some code to deal with this error.
      To know the code error, user can call function 'HAL_FLASH_GetError()'
      */
      Error_Handler();
    }

    /* Notice: After HAL_FLASH_OB_Launch() the chip is reset so next code is not executed */
    /* Implemented anyway to be aligned with other projects (SBSFU, etc) */
    /* Lock the Options Bytes ***************************************************/
    (void) HAL_FLASH_OB_Lock();
    /* Lock the Flash to disable the flash control register access  *********/
    (void) HAL_FLASH_Lock();
  }
  if (ret >= 0)
  {
    /* Init MailBoxMultiplexer */
    MBMUX_Init(pMb_RefTable);

    ret = MBMUX_RegisterFeature(FEAT_INFO_SYSTEM_ID, MBMUX_CMD_RESP,
                                MBMUXIF_IsrSystemRespRcvCb,
                                aSystemCmdRespBuff, sizeof(aSystemCmdRespBuff));
  }

  if (ret >= 0)
  {
    ret = MBMUX_RegisterFeature(FEAT_INFO_SYSTEM_ID, MBMUX_NOTIF_ACK,
                                MBMUXIF_IsrSystemNotifRcvCb,
                                aSystemNotifAckBuff, sizeof(aSystemNotifAckBuff));
  }

  if (ret >= 0)
  {
    ret = 0;
    UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_MbSystemNotifRcv), UTIL_SEQ_RFU, MBMUXIF_TaskSystemNotifRcv);
  }
  /* USER CODE BEGIN MBMUXIF_SystemInit_2 */

  /* USER CODE END MBMUXIF_SystemInit_2 */

  /* hold Cm0 from executing MBMUXIF_SystemInit after booting (after PWR_CR4_C2BOOT is set by Cm4)*/
  /* it allows Cm0 debugging step by step its own MBMUXIF_SystemInit */
  MBMUXIF_SetCpusSynchroFlag(CPUS_BOOT_SYNC_PREVENT_CPU2_TO_START);

  /* USER CODE BEGIN MBMUXIF_SystemInit_Last */

  /* USER CODE END MBMUXIF_SystemInit_Last */
  return ret;
}

int8_t MBMUXIF_SystemPrio_Add(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  int8_t ret = 0;

  /* USER CODE BEGIN MBMUXIF_SystemPrio_Add_1 */

  /* USER CODE END MBMUXIF_SystemPrio_Add_1 */

  switch (SystemPrioFeat)
  {
    case FEAT_INFO_SYSTEM_CMD_PRIO_A_ID:
    {
      ret = MBMUX_RegisterFeature(SystemPrioFeat, MBMUX_CMD_RESP,
                                  MBMUXIF_IsrSystemPrioARespRcvCb,
                                  aSystemPrioACmdRespBuff, sizeof(aSystemPrioACmdRespBuff));
      break;
    }
    case FEAT_INFO_SYSTEM_NOTIF_PRIO_A_ID:
    {
      ret = MBMUX_RegisterFeature(SystemPrioFeat, MBMUX_NOTIF_ACK,
                                  MBMUXIF_IsrSystemPrioANotifRcvCb,
                                  aSystemPrioANotifAckBuff, sizeof(aSystemPrioANotifAckBuff));
      break;
    }
    case FEAT_INFO_SYSTEM_CMD_PRIO_B_ID:
    {
      ret = MBMUX_RegisterFeature(SystemPrioFeat, MBMUX_CMD_RESP,
                                  MBMUXIF_IsrSystemPrioBRespRcvCb,
                                  aSystemPrioBCmdRespBuff, sizeof(aSystemPrioBCmdRespBuff));
      break;
    }
    case FEAT_INFO_SYSTEM_NOTIF_PRIO_B_ID:
    {
      ret = MBMUX_RegisterFeature(SystemPrioFeat, MBMUX_NOTIF_ACK,
                                  MBMUXIF_IsrSystemPrioBNotifRcvCb,
                                  aSystemPrioBNotifAckBuff, sizeof(aSystemPrioBNotifAckBuff));
      break;
    }
    default:
      break;
  }

  if (ret >= 0)
  {
    /* Note: trace works on ISR, no need to associate a SEQ_TASK */

    ret = MBMUXIF_SystemSendCm0plusRegistrationCmd(SystemPrioFeat);
    if (ret < 0)
    {
      ret = -3;
    }
  }

  /* USER CODE BEGIN MBMUXIF_SystemPrio_Add_Last */

  /* USER CODE END MBMUXIF_SystemPrio_Add_Last */
  return ret;
}

void MBMUXIF_SetCpusSynchroFlag(uint16_t flag)
{
  pMb_RefTable->SynchronizeCpusAtBoot = flag;
  /* USER CODE BEGIN MBMUXIF_SetCpusSynchroFlag */

  /* USER CODE END MBMUXIF_SetCpusSynchroFlag */
}

void MBMUXIF_WaitCm0MbmuxIsInitialized(void)
{
  while (pMb_RefTable->SynchronizeCpusAtBoot != CPUS_BOOT_SYNC_CPU2_INIT_COMPLETED)
  {
  }
  /* USER CODE BEGIN MBMUXIF_WaitCm0MbmuxIsInitialized */

  /* USER CODE END MBMUXIF_WaitCm0MbmuxIsInitialized */
}

MBMUX_ComParam_t *MBMUXIF_GetSystemFeatureCmdComPtr(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  MBMUX_ComParam_t *com_param_ptr = MBMUX_GetFeatureComPtr(SystemPrioFeat, MBMUX_CMD_RESP);
  /* USER CODE BEGIN MBMUXIF_GetSystemFeatureCmdComPtr */

  /* USER CODE END MBMUXIF_GetSystemFeatureCmdComPtr */
  if (com_param_ptr == NULL)
  {
    Error_Handler(); /* feature isn't registered */
  }
  return com_param_ptr;
}

void MBMUXIF_SystemAllowSequencerForSysCmd(void)
{
  AllowSequencerForSysCmd = 1;
  /* USER CODE BEGIN MBMUXIF_SystemAllowSequencerForSysCmd */

  /* USER CODE END MBMUXIF_SystemAllowSequencerForSysCmd */
}

void MBMUXIF_SystemSendCmd(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  /* USER CODE BEGIN MBMUXIF_SystemSendCmd_1 */

  /* USER CODE END MBMUXIF_SystemSendCmd_1 */

  if (SystemPrioFeat == FEAT_INFO_SYSTEM_ID)
  {

    MbSystemRespRcvFlag = 0;  /* To avoid using Sequencer during Init sequence for SysCmd  */
    if (MBMUX_CommandSnd(FEAT_INFO_SYSTEM_ID) == 0)
    {
      if (AllowSequencerForSysCmd)
      {
        UTIL_SEQ_WaitEvt(1 << CFG_SEQ_Evt_MbSystemRespRcv);
      }
      else
      {
        /* To avoid using Sequencer (& LowPower) during Init sequence for SysCmd  */
        while (!MbSystemRespRcvFlag) {}
      }
    }
    else
    {
      Error_Handler();
    }
  }
  else
  {
    /* USER CODE BEGIN MBMUXIF_SystemSendCmd_2 */

    /* USER CODE END MBMUXIF_SystemSendCmd_2 */
  }
  /* USER CODE BEGIN MBMUXIF_SystemSendCmd_Last */

  /* USER CODE END MBMUXIF_SystemSendCmd_Last */
}

void MBMUXIF_SystemSendAck(FEAT_INFO_IdTypeDef SystemPrioFeat)
{
  /* USER CODE BEGIN MBMUXIF_SystemSendAck_1 */

  /* USER CODE END MBMUXIF_SystemSendAck_1 */
  if (MBMUX_AcknowledgeSnd(SystemPrioFeat) != 0)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN MBMUXIF_SystemSendAck_Last */

  /* USER CODE END MBMUXIF_SystemSendAck_Last */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Exported services --------------------------------------------------------*/
FEAT_INFO_List_t *MBMUXIF_SystemSendCm0plusInfoListReq(void)
{
  MBMUX_ComParam_t *com_obj;
  uint32_t ret = 0;
  /* USER CODE BEGIN MBMUXIF_SystemSendCm0plusInfoListReq_1 */

  /* USER CODE END MBMUXIF_SystemSendCm0plusInfoListReq_1 */

  com_obj = MBMUXIF_GetSystemFeatureCmdComPtr(FEAT_INFO_SYSTEM_ID);
  if (com_obj != NULL)
  {
    com_obj->MsgId = SYS_GET_INFO_LIST_MSG_ID;
    com_obj->ParamCnt = 0;
    MBMUXIF_SystemSendCmd(FEAT_INFO_SYSTEM_ID);
    /* waiting for ack event */

    /* once event is received and semaphore released: */
    ret = com_obj->ReturnVal;
  }

  /* USER CODE BEGIN MBMUXIF_SystemSendCm0plusInfoListReq_Last */

  /* USER CODE END MBMUXIF_SystemSendCm0plusInfoListReq_Last */
  return (FEAT_INFO_List_t *) ret;   /* need to verify */
}

FEAT_INFO_Param_t *MBMUXIF_SystemGetFeatCapabInfoPtr(FEAT_INFO_IdTypeDef e_featID)
{
  FEAT_INFO_List_t *p_cm0plus_supported_features_list = NULL;
  FEAT_INFO_Param_t  *p_feature = NULL;
  uint8_t i;
  uint8_t cm0plus_nr_of_supported_features;
  uint8_t found = 0;

  /* USER CODE BEGIN MBMUXIF_SystemGetFeatCapabInfoPtr_1 */

  /* USER CODE END MBMUXIF_SystemGetFeatCapabInfoPtr_1 */

  p_cm0plus_supported_features_list = MBMUXIF_SystemSendCm0plusInfoListReq();

  if (p_cm0plus_supported_features_list != NULL)
  {
    cm0plus_nr_of_supported_features = p_cm0plus_supported_features_list->Feat_Info_Cnt;

    for (i = 0; i < cm0plus_nr_of_supported_features;  i++)
    {
      p_feature = i + p_cm0plus_supported_features_list->Feat_Info_TableAddress;
      if (p_feature->Feat_Info_Feature_Id == e_featID)
      {
        found = 1;
        break;
      }
    }
  }

  if (!found)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN MBMUXIF_SystemGetFeatCapabInfoPtr_Last */

  /* USER CODE END MBMUXIF_SystemGetFeatCapabInfoPtr_Last */
  return  p_feature;
}

int8_t MBMUXIF_SystemSendCm0plusRegistrationCmd(FEAT_INFO_IdTypeDef e_featID)
{
  MBMUX_ComParam_t *com_obj;
  uint32_t ret = 0;
  uint32_t *com_buffer;
  uint16_t i = 0;
  /* USER CODE BEGIN MBMUXIF_SystemSendCm0plusRegistrationCmd_1 */

  /* USER CODE END MBMUXIF_SystemSendCm0plusRegistrationCmd_1 */

  com_obj = MBMUXIF_GetSystemFeatureCmdComPtr(FEAT_INFO_SYSTEM_ID);
  if (com_obj != NULL)
  {
    com_obj->MsgId = SYS_REGISTER_FEATURE_MSG_ID;
    com_buffer = com_obj->ParamBuf;
    com_buffer[i++] = (uint32_t) e_featID;
    com_obj->ParamCnt = i;
    MBMUXIF_SystemSendCmd(FEAT_INFO_SYSTEM_ID);
    /* waiting for event */
    /* once event is received and semaphore released: */
    ret = com_obj->ReturnVal;
  }

  /* USER CODE BEGIN MBMUXIF_SystemSendCm0plusRegistrationCmd_Last */

  /* USER CODE END MBMUXIF_SystemSendCm0plusRegistrationCmd_Last */
  return (int8_t) ret;
}

/* USER CODE BEGIN ExpoS */

/* USER CODE END ExpoS */

/* Private functions ---------------------------------------------------------*/
static void MBMUXIF_IsrSystemRespRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemRespRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSystemRespRcvCb_1 */

  /* During "SystemInit sequence" Flag is used instead of Sequencer */
  MbSystemRespRcvFlag = 1;
  if (AllowSequencerForSysCmd) /* To avoid using Sequencer during Init sequence */
  {
    UTIL_SEQ_SetEvt(1 << CFG_SEQ_Evt_MbSystemRespRcv);
  }
  /* USER CODE BEGIN MBMUXIF_IsrSystemRespRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSystemRespRcvCb_Last */
}

static void MBMUXIF_IsrSystemNotifRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemNotifRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSystemNotifRcvCb_1 */
  SystemComObj = (MBMUX_ComParam_t *) ComObj;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_MbSystemNotifRcv), CFG_SEQ_Prio_0);
  /* USER CODE BEGIN MBMUXIF_IsrSystemNotifRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSystemNotifRcvCb_Last */
}

static void MBMUXIF_TaskSystemNotifRcv(void)
{
  /* USER CODE BEGIN MBMUXIF_TaskSystemNotifRcv_1 */

  /* USER CODE END MBMUXIF_TaskSystemNotifRcv_1 */
  Process_Sys_Notif(SystemComObj);
  /* USER CODE BEGIN MBMUXIF_TaskSystemNotifRcv_Last */

  /* USER CODE END MBMUXIF_TaskSystemNotifRcv_Last */
}

static void MBMUXIF_IsrSystemPrioARespRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioARespRcvCb */

  /* USER CODE END MBMUXIF_IsrSystemPrioARespRcvCb */
}

static void MBMUXIF_IsrSystemPrioANotifRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioANotifRcvCb_1 */

  /* USER CODE END MBMUXIF_IsrSystemPrioANotifRcvCb_1 */
  UTIL_TIMER_IRQ_Handler();
  /* clear the IPCC flag (necessary even if this Notif is not blocking */
  MBMUXIF_SystemSendAck(FEAT_INFO_SYSTEM_NOTIF_PRIO_A_ID);
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioANotifRcvCb_Last */

  /* USER CODE END MBMUXIF_IsrSystemPrioANotifRcvCb_Last */
}

static void MBMUXIF_IsrSystemPrioBRespRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioBRespRcvCb */

  /* USER CODE END MBMUXIF_IsrSystemPrioBRespRcvCb */
}

static void MBMUXIF_IsrSystemPrioBNotifRcvCb(void *ComObj)
{
  /* USER CODE BEGIN MBMUXIF_IsrSystemPrioBNotifRcvCb */

  /* USER CODE END MBMUXIF_IsrSystemPrioBNotifRcvCb */
}

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
