/**
  ******************************************************************************
  * @file    LmhpDataDistribution.c
  * @author  MCD Application Team
  * @brief   Implements the Data Distribution Agent
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "LmhpDataDistribution.h"
#include "LmhpClockSync.h"
#include "LmhpRemoteMcastSetup.h"
#include "LmhpFragmentation.h"
#include "LmhpFirmwareManagement.h"
#include "LmHandler.h"
#include "mw_log_conf.h"
#include "utilities.h"

#include "flash_if.h"
#include "se_def_metadata.h"

#define SFU_FWIMG_COMMON_C
#include "sfu_fwimg_regions.h"

/* Private typedef -----------------------------------------------------------*/
/*! 
 * Structure containing values related to the management of multi-images in Flash
 */
typedef struct
{
  uint32_t  MaxSizeInBytes;        /*!< The maximum allowed size for the FwImage in User Flash (in Bytes) */
  uint32_t  DownloadAddr;          /*!< The download address for the FwImage in UserFlash */
  uint32_t  ImageOffsetInBytes;    /*!< Image write starts at this offset */
  uint32_t  ExecutionAddr;         /*!< The execution address for the FwImage in UserFlash */
} FwImageFlashTypeDef;

/* Private define ------------------------------------------------------------*/
/*!
 * Defines the maximum size for the buffer receiving the fragmentation result.
 *
 * \remark By default FragDecoder.h defines:
 *         \ref FRAG_MAX_NB   313
 *         \ref FRAG_MAX_SIZE 216
 *
 *         In interop test mode will be
 *         \ref FRAG_MAX_NB   21
 *         \ref FRAG_MAX_SIZE 50
 *
 *         FileSize = FRAG_MAX_NB * FRAG_MAX_SIZE
 *
 *         If bigger file size is to be received or is fragmented differently
 *         one must update those parameters.
 *
 * \remark  Memory allocation is done at compile time. Several options have to be foreseen
 *          in order to optimize the memory. Will depend of the Memory management used
 *          Could be Dynamic allocation --> malloc method
 *          Variable Length Array --> VLA method
 *          pseudo dynamic allocation --> memory pool method
 *          Other option :
 *          In place of using the caching memory method we can foreseen to have a direct
 *          flash memory copy. This solution will depend of the SBSFU constraint
 *
 */
#define UNFRAGMENTED_DATA_SIZE                      ( FRAG_MAX_NB * FRAG_MAX_SIZE )

/*!
 * Starting offset to add to the  first address
 */
#define SFU_IMG_IMAGE_OFFSET                        ((uint32_t)512U)

/*!
 * Size of header to write in Swap sector to trigger installation
 */
#define INSTALLED_LENGTH                            ((uint32_t)512U)

#define SFU_IMG_SWAP_REGION_SIZE                    ((uint32_t)(SWAP_END - SWAP_START + 1U))

#define SFU_IMG_SWAP_REGION_BEGIN_VALUE             ((uint32_t)SWAP_START)

#define SFU_IMG_SLOT_DWL_REGION_BEGIN_VALUE         ((uint32_t)SLOT_DWL_1_START)

#define SFU_IMG_SLOT_DWL_REGION_SIZE                ((uint32_t)(SLOT_DWL_1_END - SLOT_DWL_1_START + 1U))

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  Init `data` buffer of `size` starting at address `addr`
  * @param  addr Address start index to erase.
  * @param  size number of bytes.
  * @retval status Init operation status [0: Success, -1 Fail]
  */
static uint8_t FragDecoderErase(uint32_t addr, uint32_t size);
/**
  * @brief  Write `data` buffer of `size` starting at address `addr`
  * @param  addr Address start index to write to.
  * @param  data Data buffer to be written.
  * @param  size Size of data buffer to be written.
  * @retval status Write operation status [0: Success, -1 Fail]
  */
static uint8_t FragDecoderWrite(uint32_t addr, uint8_t *data, uint32_t size);

/**
  * @brief  Reads `data` buffer of `size` starting at address `addr`
  * @param  addr Address start index to read from.
  * @param  data Data buffer to be read.
  * @param  size Size of data buffer to be read.
  * @retval status Read operation status [0: Success, -1 Fail]
  */
static uint8_t FragDecoderRead(uint32_t addr, uint8_t *data, uint32_t size);

/**
  * @brief  Callback to get the current progress status of the fragmentation session
  * @param  fragCounter fragment counter
  * @param  fragNb number of fragments
  * @param  fragSize size of fragments
  * @param  fragNbLost number of lost fragments
  * @retval None
  */
static void OnFragProgress(uint16_t fragCounter, uint16_t fragNb, uint8_t fragSize, uint16_t fragNbLost);

/**
  * @brief  Callback to notify when the fragmentation session is finished
  * @param  status status of the fragmentation process
  * @param  size size of the fragmented data block
  * @retval None
  */
static void OnFragDone(int32_t status, uint32_t size);

#if (INTEROP_TEST_MODE == 0)
/**
  * @brief  Run FW Update process.
  * @param  None
  * @retval None
  */
static void FwUpdateAgentRun(void);

/**
  * @brief  Provide the area descriptor to write a FW image in Flash.
  *         This function is used by the User Application to know where to store
  *          a new Firmware Image before asking for its installation.
  * @param  pArea pointer to area descriptor
  * @retval HAL_OK if successful, otherwise HAL_ERROR
  */
static uint32_t FwUpdateAgentGetDownloadAreaInfo(FwImageFlashTypeDef *pArea);

/**
  * @brief  Write in Flash the next header image to install.
  *         This function is used by the User Application to request a Firmware installation (at next reboot).
  * @param  fw_header FW header of the FW to be installed
  * @retval HAL_OK if successful, otherwise HAL_ERROR
  */
static uint32_t FwUpdateAgentInstallAtNextReset(uint8_t *fw_header);
#endif /* INTEROP_TEST_MODE == 0 */
/* Private variables ---------------------------------------------------------*/
static LmhpFragmentationParams_t FragmentationParams =
{
  .DecoderCallbacks =
  {
    .FragDecoderErase = FragDecoderErase,
    .FragDecoderWrite = FragDecoderWrite,
    .FragDecoderRead = FragDecoderRead,
  },
  .OnProgress = OnFragProgress,
  .OnDone = OnFragDone
};

/*
 * Indicates if the file transfer is done
 */
static volatile bool IsFileTransferDone = false;

#if (INTEROP_TEST_MODE == 1)  /*write fragment in RAM - Caching mode*/
/*
 * Un-fragmented data storage.
 */
static uint8_t UnfragmentedData[UNFRAGMENTED_DATA_SIZE];
#endif /* INTEROP_TEST_MODE == 1 */

/* Exported functions ---------------------------------------------------------*/
LmHandlerErrorStatus_t LmhpDataDistributionInit(void)
{
  if (LmHandlerPackageRegister(PACKAGE_ID_CLOCK_SYNC, NULL) != LORAMAC_HANDLER_SUCCESS)
  {
    return LORAMAC_HANDLER_ERROR;
  }
  else if (LmHandlerPackageRegister(PACKAGE_ID_REMOTE_MCAST_SETUP, NULL) != LORAMAC_HANDLER_SUCCESS)
  {
    return LORAMAC_HANDLER_ERROR;
  }
  else if (LmHandlerPackageRegister(PACKAGE_ID_FRAGMENTATION, &FragmentationParams) != LORAMAC_HANDLER_SUCCESS)
  {
    return LORAMAC_HANDLER_ERROR;
  }
  else if (LmHandlerPackageRegister(PACKAGE_ID_FIRMWARE_MANAGEMENT, NULL) != LORAMAC_HANDLER_SUCCESS)
  {
    return LORAMAC_HANDLER_ERROR;
  }

  return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmhpDataDistributionPackageRegister(uint8_t id, LmhPackage_t **package)
{
  if (package == NULL)
  {
    return LORAMAC_HANDLER_ERROR;
  }
  switch (id)
  {
    case PACKAGE_ID_CLOCK_SYNC:
    {
      *package = LmphClockSyncPackageFactory();
      break;
    }
    case PACKAGE_ID_REMOTE_MCAST_SETUP:
    {
      *package = LmhpRemoteMcastSetupPackageFactory();
      break;
    }
    case PACKAGE_ID_FRAGMENTATION:
    {
      *package = LmhpFragmentationPackageFactory();
      break;
    }
    case PACKAGE_ID_FIRMWARE_MANAGEMENT:
    {
      *package = LmhpFirmwareManagementPackageFactory();
      break;
    }
  }

  return LORAMAC_HANDLER_SUCCESS;
}

/* Private  functions ---------------------------------------------------------*/
static uint8_t FragDecoderErase(uint32_t addr, uint32_t size)
{
#if (INTEROP_TEST_MODE == 1)
  for (uint32_t i = 0; i < size; i++)
  {
    UnfragmentedData[addr + i] = 0xFF;
  }
#else /* INTEROP_TEST_MODE == 0 */
  if (FLASH_Erase((void *)(SlotStartAdd[SLOT_DWL_1] + addr), size) != HAL_OK)
  {
    return -1;
  }
#endif /* INTEROP_TEST_MODE */
  return 0; /* Success */
}

static uint8_t FragDecoderWrite(uint32_t addr, uint8_t *data, uint32_t size)
{
  if (size >= UNFRAGMENTED_DATA_SIZE)
  {
    return (uint8_t) - 1; /* Fail */
  }
#if (INTEROP_TEST_MODE == 1)  /*write fragment in RAM - Caching mode*/
  for (uint32_t i = 0; i < size; i++)
  {
    UnfragmentedData[addr + i] = data[i];
  }
#else /* INTEROP_TEST_MODE == 0 */
  if (FLASH_Write(SlotStartAdd[SLOT_DWL_1] + addr, data, size) != HAL_OK)
  {
    return -1;
  }
#endif /* INTEROP_TEST_MODE */

  return 0; /* Success */
}

static uint8_t FragDecoderRead(uint32_t addr, uint8_t *data, uint32_t size)
{
  if (size >= UNFRAGMENTED_DATA_SIZE)
  {
    return (uint8_t) - 1; /* Fail */
  }

#if (INTEROP_TEST_MODE == 1)   /*Read fragment in RAM - Caching mode*/
  for (uint32_t i = 0; i < size; i++)
  {
    data[i] = UnfragmentedData[addr + i];
  }
#else /* INTEROP_TEST_MODE == 0 */
  if (FLASH_Read((void *)(SlotStartAdd[SLOT_DWL_1] + addr), data, size) != HAL_OK)
  {
    return -1;
  }
#endif /* INTEROP_TEST_MODE */

  return 0; /* Success */
}

static void OnFragProgress(uint16_t fragCounter, uint16_t fragNb, uint8_t fragSize, uint16_t fragNbLost)
{
#if (INTEROP_TEST_MODE == 1)
  /* BSP_LED_On(LED_BLUE); */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
#endif /* INTEROP_TEST_MODE == 1 */

  MW_LOG(TS_OFF, VLEVEL_M, "\r\n....... FRAG_DECODER in Progress .......\r\n");
  MW_LOG(TS_OFF, VLEVEL_M, "RECEIVED    : %5d / %5d Fragments\r\n", fragCounter, fragNb);
  MW_LOG(TS_OFF, VLEVEL_M, "              %5d / %5d Bytes\r\n", fragCounter * fragSize, fragNb * fragSize);
  MW_LOG(TS_OFF, VLEVEL_M, "LOST        :       %7d Fragments\r\n\r\n", fragNbLost);
}

static void OnFragDone(int32_t status, uint32_t size)
{
  IsFileTransferDone = true;
#if (INTEROP_TEST_MODE == 0)
  /* Do a request to Run the Secure boot - The file is already in flash */
  FwUpdateAgentRun();
#else
  /* BSP_LED_Off(LED_BLUE); */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
#endif /* INTEROP_TEST_MODE == 1 */
  MW_LOG(TS_OFF, VLEVEL_M, "\r\n....... FRAG_DECODER Finished .......\r\n");
  MW_LOG(TS_OFF, VLEVEL_M, "STATUS      : %d\r\n", status);
}

#if (INTEROP_TEST_MODE == 0)
static void FwUpdateAgentRun(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  uint8_t fw_header_dwl_slot[SE_FW_HEADER_TOT_LEN];
  FwImageFlashTypeDef fw_image_dwl_area;

  /* Get Info about the download area */
  if (FwUpdateAgentGetDownloadAreaInfo(&fw_image_dwl_area) != HAL_ERROR)
  {
    /* Read header in slot 1 */
    ret = FLASH_Read(fw_header_dwl_slot, (void *) fw_image_dwl_area.DownloadAddr, SE_FW_HEADER_TOT_LEN);

    /* Ask for installation at next reset */
    (void)FwUpdateAgentInstallAtNextReset((uint8_t *) fw_header_dwl_slot);

    /* System Reboot*/
    NVIC_SystemReset();
  }
  if (ret != HAL_OK)
  {
    MW_LOG(TS_OFF, VLEVEL_M, "  --  Operation Failed  \r\n");
  }
}

static uint32_t FwUpdateAgentGetDownloadAreaInfo(FwImageFlashTypeDef *pArea)
{
  uint32_t ret;
  if (pArea != NULL)
  {
    pArea->DownloadAddr = SFU_IMG_SLOT_DWL_REGION_BEGIN_VALUE;
    pArea->MaxSizeInBytes = (uint32_t)SFU_IMG_SLOT_DWL_REGION_SIZE;
    pArea->ImageOffsetInBytes = SFU_IMG_IMAGE_OFFSET;
    ret =  HAL_OK;
  }
  else
  {
    ret = HAL_ERROR;
  }
  return ret;
}

static uint32_t FwUpdateAgentInstallAtNextReset(uint8_t *pfw_header)
{
  uint32_t ret = HAL_OK;

  if (pfw_header == NULL)
  {
    return HAL_ERROR;
  }

  ret = FLASH_Erase((void *) SFU_IMG_SWAP_REGION_BEGIN_VALUE, SFU_IMG_IMAGE_OFFSET);
  if (ret == HAL_OK)
  {
    ret = FLASH_Write(SFU_IMG_SWAP_REGION_BEGIN_VALUE, pfw_header, SE_FW_HEADER_TOT_LEN);
  }
  return ret;
}
#endif /* INTEROP_TEST_MODE == 0 */
