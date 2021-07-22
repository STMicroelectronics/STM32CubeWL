/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    test_rf.h
  * @author  MCD Application Team
  * @brief   Header for test_rf.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TEST_RF_H__
#define __TEST_RF_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  BW_7kHz = 0,
  BW_12kHz = 1,
  BW_31kHz = 2,
  BW_62kHz = 3,
  BW_125kHz = 4,
  BW_250kHz = 5,
  BW_500kHz = 6,
} Lora_BandWidth_t;

typedef struct
{
  uint32_t modulation;         /* 0: FSK, 1: Lora, 2:BPSK(Tx) */
  uint32_t freq;               /* in Hz */
  int32_t power;               /* [-9 :22]dBm */
  uint32_t bandwidth;          /* Lora [0:7.8125, 1: 15.625, 2: 31.25, 3: 62.5, 4: 125, 5: 250, 6: 500]kHz
                                  FSK : [4800Hz :467000 Hz] */
  uint32_t loraSf_datarate;    /* Lora[SF5..SF12] FSK [600..300000 bits/s] */
  uint32_t codingRate;         /* Lora Only [1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8] */
  uint32_t lna;                /* 0:off 1:On */
  uint32_t paBoost;            /* 0:off 1:On */
  uint32_t payloadLen;         /* [1:256] */
  uint32_t fskDev;             /* FSK only [4800:467000]
                                  Note: no check applied wrt bandwidth. Common practice is to have bandwidth>1,5*fskDev */
  uint32_t lowDrOpt;           /* Lora Only 0: off, 1:On, 2: Auto (1 when SF11 or SF12, 0 otherwise) */
  uint32_t BTproduct;          /* FSK only [0 no Gaussian Filter Applied, 1: BT=0,3, 2: BT=0,5, 3: BT=0,7, 4: BT=1] */
} testParameter_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
#define TEST_FSK                           0
#define TEST_LORA                          1
#define TEST_BPSK                          2

/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions ------------------------------------------------------- */
/**
  * @brief RF Tone test command
  * @retval status 0 ok, -1 ko
  */
int32_t TST_TxTone(void);

/**
  * @brief RF Receive  test command
  * @retval status 0 ok, -1 ko
  */
int32_t TST_RxRssi(void);

/**
  * @brief RF Transmit LORA test command
  * @note Transmits [0x00, 0x11, 0x22, ... 0xFF]
  * @param[in] nb_packet nb of packets
  * @retval status 0 ok, -1 ko
  */
int32_t TST_TX_Start(int32_t nb_packet);

/**
  * @brief RF Receive LORA test command
  * @param[in] nb_packet nb of packets
  * @retval status 0 ok, -1 ko
  */
int32_t TST_RX_Start(int32_t nb_packet);

/**
  * @brief RF Set Radio Configuration test command
  * @param[in] Param Pointer config param
  * @retval status 0 ok, -1 ko
  */
int32_t TST_set_config(testParameter_t *Param);

/**
  * @brief RF Get Radio Configuration test command
  * @param[in] Param Pointer config param
  * @retval LoRa status
  */
int32_t TST_get_config(testParameter_t *Param);

/**
  * @brief RF test stop
  * @retval LoRa status
  */
int32_t TST_stop(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __TEST_RF_H__*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
