/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    features_info.c
  * @author  MCD Application Team
  * @brief   CM0PLUS supported features list
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
#include <stddef.h>
#include <stdint.h>
#include "features_info.h"
#include "sigfox_info.h"
#include "sigfox_version.h"
#include "app_version.h"
#include "subghz_phy_version.h"
#include "stm32_mem.h"

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
/**
  * @brief Feature info table
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM2") FEAT_INFO_Param_t Feat_Info_Table[] =
{
  {
    .Feat_Info_Feature_Id =   FEAT_INFO_SYSTEM_ID,
    .Feat_Info_Feature_Version = APP_VERSION,
    .Feat_Info_Config_Size =  0,
    .Feat_Info_Config_Ptr = (void *) NULL
  },
  {
    .Feat_Info_Feature_Id =   FEAT_INFO_SYSTEM_NOTIF_PRIO_A_ID,
    .Feat_Info_Feature_Version = APP_VERSION,
    .Feat_Info_Config_Size = 0,
    .Feat_Info_Config_Ptr = (void *) NULL
  },
  /* {
    .Feat_Info_Feature_Id =   FEAT_INFO_KMS_ID,
    .Feat_Info_Feature_Version = FEAT_INFO_KMS_VER,
    .Feat_Info_Config_Size = 0,
    .Feat_Info_Config_Ptr=  (void *) NULL
  }, */
  {
    .Feat_Info_Feature_Id =   FEAT_INFO_SIGFOX_ID,
    .Feat_Info_Feature_Version = SIGFOX_VERSION,
    .Feat_Info_Config_Size = 0,
    .Feat_Info_Config_Ptr = (void *) NULL
  },
  {
    .Feat_Info_Feature_Id =   FEAT_INFO_TRACE_ID,
    .Feat_Info_Feature_Version = APP_VERSION,
    .Feat_Info_Config_Size = 0,
    .Feat_Info_Config_Ptr = (void *) NULL
  },
  {
    .Feat_Info_Feature_Id =   FEAT_INFO_RADIO_ID,
    .Feat_Info_Feature_Version = SUBGHZ_PHY_VERSION,
    .Feat_Info_Config_Size = 0,
    .Feat_Info_Config_Ptr = (void *) NULL
  },
};

/**
  * @brief Feature info container
  */
UTIL_MEM_PLACE_IN_SECTION("MB_MEM2") FEAT_INFO_List_t Feat_Info_List =
{
  .Feat_Info_Cnt =  sizeof(Feat_Info_Table) / sizeof(FEAT_INFO_Param_t),
  .Feat_Info_TableAddress =  Feat_Info_Table,
};

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

void FEAT_INFO_Init(void)
{
  /* USER CODE BEGIN FEAT_INFO_Init_1 */

  /* USER CODE END FEAT_INFO_Init_1 */

  /* Set Sigfox Info_Config */
  SigfoxInfo_Init();
  /* USER CODE BEGIN FEAT_INFO_Init_2 */

  /* USER CODE END FEAT_INFO_Init_2 */
}

FEAT_INFO_List_t *FEAT_INFO_GetListPtr(void)
{
  /* USER CODE BEGIN FEAT_INFO_GetListPtr_1 */

  /* USER CODE END FEAT_INFO_GetListPtr_1 */
  return (FEAT_INFO_List_t *) &Feat_Info_List;
  /* USER CODE BEGIN FEAT_INFO_GetListPtr_2 */

  /* USER CODE END FEAT_INFO_GetListPtr_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */
