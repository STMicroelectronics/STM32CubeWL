/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fw_update_agent.c
  * @author  MCD Application Team
  * @brief   This file provides set of functions to manage Firmware Update functionalities.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#define SFU_FWIMG_COMMON_C
#include "sfu_fwimg_regions.h"

#include "fw_update_agent.h"
#include "sys_app.h"
#include "utilities.h"

#include "flash_if.h"
#include "se_def_metadata.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/**
  * Structure containing values related to the management of multi-images in Flash
  */
typedef struct
{
  uint32_t  MaxSizeInBytes;        /*!< The maximum allowed size for the FwImage in User Flash (in Bytes) */
  uint32_t  DownloadAddr;          /*!< The download address for the FwImage in UserFlash */
  uint32_t  ImageOffsetInBytes;    /*!< Image write starts at this offset */
} FwImageFlashTypeDef;

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/**
  * Size of header to write in Swap sector to trigger installation
  */
#define INSTALLED_LENGTH                            ((uint32_t)512U)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/
#if (INTEROP_TEST_MODE == 0)
void FwUpdateAgent_Run(void)
{
  /* USER CODE BEGIN FwUpdateAgent_Run_1 */

  /* USER CODE END FwUpdateAgent_Run_1 */
  int32_t status = FLASH_OK;
  uint8_t fw_header_dwl_slot[SE_FW_HEADER_TOT_LEN];
  uint32_t first_page = PAGE(FRAG_DECODER_SWAP_REGION_START);
  uint32_t nb_pages = PAGE(FRAG_DECODER_SWAP_REGION_START + SFU_IMG_IMAGE_OFFSET - 1) - first_page + 1U;

  /* Read header in slot 1 */
  UTIL_MEM_cpy_8(fw_header_dwl_slot, (void *)FRAG_DECODER_DWL_REGION_START, SE_FW_HEADER_TOT_LEN);

  /* Ask for installation at next reset */
  if (HAL_FLASH_Unlock() == HAL_OK)
  {
    status = FLASH_IF_EraseByPages(first_page, nb_pages, 0U);

    if (status == FLASH_OK)
    {
      status = FLASH_IF_Write(FRAG_DECODER_SWAP_REGION_START, fw_header_dwl_slot, SE_FW_HEADER_TOT_LEN, NULL);
    }
    HAL_FLASH_Lock();
  }
  else
  {
    status = FLASH_LOCK_ERROR;
  }

  if (status == FLASH_OK)
  {
    /* System Reboot*/
    NVIC_SystemReset();
  }
  else
  {
    APP_LOG(TS_OFF, VLEVEL_M, "FW Update Agent Run Failed\r\n");
  }
  /* USER CODE BEGIN FwUpdateAgent_Run_2 */

  /* USER CODE END FwUpdateAgent_Run_2 */
}
#endif /* INTEROP_TEST_MODE */

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
