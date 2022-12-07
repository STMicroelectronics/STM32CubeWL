/**
  ******************************************************************************
  * @file    LmhpPackagesRegistration.c
  * @author  MCD Application Team
  * @brief   Implements the Packages Registration Agent
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020(-2021) STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "LmhpPackagesRegistration.h"
#include "LmhpClockSync.h"
#include "LmhpRemoteMcastSetup.h"
#include "LmhpFragmentation.h"
#include "LmhpFirmwareManagement.h"
#include "LmHandler.h"
#include "frag_decoder_if.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Exported functions ---------------------------------------------------------*/
LmHandlerErrorStatus_t LmhpPackagesRegistrationInit(void)
{
  if (LmHandlerPackageRegister(PACKAGE_ID_CLOCK_SYNC, NULL) != LORAMAC_HANDLER_SUCCESS)
  {
    return LORAMAC_HANDLER_ERROR;
  }
  else if (LmHandlerPackageRegister(PACKAGE_ID_REMOTE_MCAST_SETUP, NULL) != LORAMAC_HANDLER_SUCCESS)
  {
    return LORAMAC_HANDLER_ERROR;
  }
  else if (LmHandlerPackageRegister(PACKAGE_ID_FRAGMENTATION, (LmhpFragmentationParams_t *)&FRAG_DECODER_IF_FragmentationParams) != LORAMAC_HANDLER_SUCCESS)
  {
    return LORAMAC_HANDLER_ERROR;
  }
  else if (LmHandlerPackageRegister(PACKAGE_ID_FIRMWARE_MANAGEMENT, NULL) != LORAMAC_HANDLER_SUCCESS)
  {
    return LORAMAC_HANDLER_ERROR;
  }

  return LORAMAC_HANDLER_SUCCESS;
}

LmHandlerErrorStatus_t LmhpPackagesRegister(uint8_t id, LmhPackage_t **package)
{
  if (package == NULL)
  {
    return LORAMAC_HANDLER_ERROR;
  }
  switch (id)
  {
    case PACKAGE_ID_CLOCK_SYNC:
    {
      *package = LmhpClockSyncPackageFactory();
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
