 /*******************************************************************************
 * File Name          : stm32_timer_if.c
 * Description        : This file provides the ll driver for  the time server
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include <math.h>
#include "stm32_timer_if.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported variables ---------------------------------------------------------*/
const UTIL_TIMER_Driver_s UTIL_TimerDriver =
{
    PPP_Init,
    PPP_DeInit,
    
    PPP_StartTimer,
    PPP_StopTimer, 
    
    PPP_SetTimerContext,
    PPP_GetTimerContext,
    
    PPP_GetTimerElapsedTime,
    PPP_GetTimerValue,
    PPP_GetMinimumTimeout,
    
    PPP_ms2Tick,
    PPP_Tick2ms, 
};

/*!
 * @brief Initializes the PPP timer
 * @note The timer is based on the PPP
 * @param none
 * @retval none
 */
UTIL_TIMER_Status_t PPP_Init( void )
{
  return UTIL_TIMER_OK;
}

/*!
 * @brief Initializes the PPP timer
 * @note The timer is based on the PPP
 * @param none
 * @retval none
 */
UTIL_TIMER_Status_t PPP_DeInit( void )
{
  return UTIL_TIMER_OK;
}

/*!
 * @brief Set the timer
 * @note The timer is set at now (read in this function) + timeout
 * @param timeout Duration of the Timer ticks
 */
UTIL_TIMER_Status_t PPP_StartTimer( uint32_t timeout )
{
  return UTIL_TIMER_OK;
}

/*!
 * @brief Stop the timer
 * @param none
 * @retval none
 */
UTIL_TIMER_Status_t PPP_StopTimer( void )
{
  return UTIL_TIMER_OK;
}

/*!
 * @brief returns the wake up time in ticks
 * @param none
 * @retval wake up time in ticks
 */
uint32_t PPP_GetMinimumTimeout( void )
{
  return ( 0 );
}

/*!
 * @brief converts time in ms to time in ticks
 * @param [IN] time in milliseconds
 * @retval returns time in timer ticks
 */
uint32_t PPP_ms2Tick( uint32_t timeMicroSec )
{
  return ( 0 );
}

/*!
 * @brief converts time in ticks to time in ms
 * @param [IN] time in timer ticks
 * @retval returns time in milliseconds
 */
uint32_t PPP_Tick2ms( uint32_t tick )
{
  return ( 0 );
}

/*!
 * @brief Get the PPP timer elapsed time since the last timer was set
 * @param none
 * @retval PPP Elapsed time in ticks
 */
uint32_t PPP_GetTimerElapsedTime( void )
{
  return ( 0 );
}

/*!
 * @brief Get the PPP timer value
 * @param none
 * @retval PPP Timer value in ticks
 */
uint32_t PPP_GetTimerValue( void )
{
  return ( 0 );
}

/*!
 * @brief PPP IRQ Handler on the PPP timer
 * @param none
 * @retval none
 */
void PPP_IrqHandler ( void )
{
}

/*!
 * @brief set Time Reference set also the sDate and sTime
 * @param none
 * @retval Timer Value
 */
uint32_t PPP_SetTimerContext( void )
{
  return ( 0 );
}

/*!
 * @brief Get the PPP timer Reference
 * @param none
 * @retval Timer Value in  Ticks
 */
uint32_t PPP_GetTimerContext( void )
{
  return ( 0 );
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

