/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    demo_prnd.h
  * @author  MCD Application Team
  * @brief   Pseudorandom function for STM32WL demo.
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
#ifndef __DEMO_PRND_H__
#define __DEMO_PRND_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEMO_PRND_P_MERS   (31) /*Nth Mersenne prime*/
#define DEMO_PRND_P        ((1ULL << DEMO_PRND_P_MERS) - 1)     /*2^N - 1 is often a prime*/
#define DEMO_PRND_A        62089911ULL   /*Suggested by Knuth, found by Fishman and Moore*/

/**
  * @brief Generate one pseudorandom number between 0x00000001 and 0x7ffffffe.
  * @param last previous generated number (cannot be 0x00000000 and cannot be 0x7fffffff)
  * @return almost 31 bits of pseudorandom number (numbers 0x00000000 and 0x7fffffff cannot happen)
  */
static inline uint32_t DEMO_PRND_Next(uint32_t last)
{
  uint64_t x = (uint64_t)(last) * DEMO_PRND_A;          /* x = last * A */
  x = (x >> DEMO_PRND_P_MERS) + (x & DEMO_PRND_P);      /* x / w + x % w almost mod P, P = w-1 */
  if (x >= DEMO_PRND_P)
  {
    return x - DEMO_PRND_P;                             /* The rest of mod P */
  }
  else
  {
    return x;
  }
}

#ifdef __cplusplus
}
#endif

#endif /* __DEMO_PRND_H__ */
