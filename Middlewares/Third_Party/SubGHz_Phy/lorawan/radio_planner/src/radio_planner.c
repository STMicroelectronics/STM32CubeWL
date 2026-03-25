/*!
 * \file      radio_planner.c
 *
 * \brief     Radio planner implementation
 *
 * The Clear BSD License
 * Copyright Semtech Corporation 2021. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdlib.h>
#include <stdio.h>
#include "timer.h"
#include "radio_planner.h"
#include "smtc_duty_cycle.h"
#include "smtc_modem_hal_dbg_trace.h"
#include "smtc_modem_hal.h"

#if defined( ADD_LBM_GEOLOCATION )
#include "lr11xx_system.h"  // For blocking command abort
#endif

#include <string.h>

static radio_planner_t rp_modem_radio_planner;
radio_planner_t* rp_modem_radio_planner_p = &rp_modem_radio_planner;

TimerEvent_t nextAlarmTimer;

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS ----------------------------------------------------------
 */

#define TARGET_RAL &( rp->radio_target_attached_to_this_hook[rp->radio_task_id]->ral )
#define TARGET_RADIO rp->radio_target_attached_to_this_hook[rp->radio_task_id]
#define TARGET_RAL_FOR_HOOK_ID  (&( rp->radio_target_attached_to_this_hook[rp->radio_task_id]->ral ))

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE DEFINES ---------------------------------------------------------
 */
 
/*can be overridden in radio_conf.h*/
#ifndef DBG_GPIO_RADIO_RX
#define DBG_GPIO_RADIO_RX(set_rst)
#endif

/*can be overridden in radio_conf.h*/
#ifndef DBG_GPIO_RADIO_TX
#define DBG_GPIO_RADIO_TX(set_rst)
#endif

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DECLARATION -------------------------------------------
 */

/**
 * @brief rp_task_free to free a task
 *
 * @param rp  pointer to the radioplanner object itself
 * @param task pointer to the task that function free
 */
static void rp_task_free( const radio_planner_t* rp, rp_task_t* task );

/**
 * @brief rp_task_update_time update task time
 *
 * @param rp pointer to the radioplanner object itself
 * @param now the current time in ms
 */
static void rp_task_update_time( radio_planner_t* rp, uint32_t now );

/**
 * @brief rp_task_arbiter the core of the radio planner
 *
 * @param rp pointer to the radioplanner object itself
 * @param caller_func_name input just for print
 */
static void rp_task_arbiter( radio_planner_t* rp, const char* caller_func_name );

/**
 * @brief rp_task_compute_ranking compute the ranking of the different tasks inside the radio planner
 *
 * @param rp pointer to the radioplanner object itself
 */
static void rp_task_compute_ranking( radio_planner_t* rp );

/**
 * @brief rp_task_launch_current call  the launch callback of the new running task
 *
 * @param rp  pointer to the radioplanner object itself
 */
static void rp_task_launch_current( radio_planner_t* rp );

/**
 * @brief rp_task_select_next select the next most priority task
 *
 * @param rp pointer to the radioplanner object itself
 * @param now the current time in ms
 * @return uint8_t
 */
static uint8_t rp_task_select_next( radio_planner_t* rp, const uint32_t now );

/**
 * @brief rp_task_get_next return if there is a task to schedule or no more task
 *
 * @param rp pointer to the radioplanner object itself
 * @param duration return the delay to the next task (to set the timer)
 * @param task_id return the task id of the next task to launch
 * @param now the current time
 * @return rp_next_state_status_t return if it have to set a timer or if there is no more task in rp
 */
static rp_next_state_status_t rp_task_get_next( radio_planner_t* rp, uint32_t* duration, uint8_t* task_id,
                                                const uint32_t now );
/**
 * @brief rp_task_find_highest_priority utilities to classify priority task (value 0 is the highest priority)
 *
 * @param vector a vector of value to classify from the lowest to the highest
 * @param length size of the vector
 * @return uint8_t index of the vector to indicate the position inside the vector of the minimum value
 */
static uint8_t rp_task_find_highest_priority( uint8_t* vector, uint8_t length );

/**
 * @brief rp_get_pkt_payload get the receive payload
 *
 * @param rp pointer to the radioplanner object itself
 * @param task the task containing the received payload
 * @return rp_hook_status_t
 */
rp_hook_status_t rp_get_pkt_payload( radio_planner_t* rp, const rp_task_t* task );

/**
 * @brief rp_set_alarm configure the radio planner timer
 *
 * @param rp pointer to the radioplanner object itself
 * @param alarm_in_ms delay in ms (relative value)
 */
static void rp_set_alarm( radio_planner_t* rp, const uint32_t alarm_in_ms );

/**
 * @brief rp_timer_irq function call by the timer callback
 *
 * @param rp pointer to the radioplanner object itself
 */
static void rp_timer_irq( radio_planner_t* rp );

/**
 * @brief rp_task_call_aborted excute the callback of the aborted tasks
 *
 * @param rp pointer to the radioplanner object itself
 */
static void rp_task_call_aborted( radio_planner_t* rp );
/**
 * @brief rp_consumption_statistics_updated compute the statistic (power consumption)
 *
 * @param rp pointer to the radioplanner object itself
 * @param hook_id hook id on which statistics are perform
 * @param time the current time in ms
 */
static void rp_consumption_statistics_updated( radio_planner_t* rp, const uint8_t hook_id, const uint32_t time );

/**
 * @brief rp_timer_irq_callback timer callback
 *
 * @param obj pointer to the radioplanner object itself
 */
static void rp_timer_irq_callback( void* obj );

/**
 * @brief rp_hook_callback call the callback associated to the id
 *
 * @param rp pointer to the radioplanner object itself
 * @param id target hook id
 */
static void rp_hook_callback( radio_planner_t* rp, uint8_t id );

/**
 * @brief rp_task_print debug print function for rp
 *
 * @param rp pointer to the radioplanner object itself
 * @param task target task to print
 */
static void rp_task_print( const radio_planner_t* rp, const rp_task_t* task );

/*!
 * \brief radio IRQ callback
 *
 * \param [in] radioIrq       mask of radio irq
 */
static void RadioOnDioIrq( irq_mask_t radioIrq );

/*!
 * \brief Process radio irq
 */
static void RadioIrqProcess( irq_mask_t radioIrq );

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION ---------------------------------------------
 */

void rp_init( radio_planner_t* rp, const ralf_t* radio )
{
    memset( rp, 0, sizeof( radio_planner_t ) );
    rp->radio = radio;

    for( int32_t i = 0; i < RP_NB_HOOKS; i++ )
    {
        rp->tasks[i].hook_id                      = i;
        rp->tasks[i].type                         = RP_TASK_TYPE_NONE;
        rp->tasks[i].launch_task_callbacks        = NULL;
        rp->tasks[i].state                        = RP_TASK_STATE_FINISHED;
        rp->tasks[i].schedule_task_low_priority   = false;
        rp->radio_target_attached_to_this_hook[i] = rp->radio;
        rp->hooks[i]                              = NULL;
        rp->tasks[i].launch_task_callbacks        = NULL;
        rp->hook_callbacks[i]                     = NULL;
        rp->status[i]                             = RP_STATUS_TASK_INIT;
    }
    rp->priority_task.type  = RP_TASK_TYPE_NONE;
    rp->priority_task.state = RP_TASK_STATE_FINISHED;
    rp_stats_init( &rp->stats );
    rp->next_state_status = RP_STATUS_NO_MORE_TASK_SCHEDULE;
    rp->margin_delay      = RP_MARGIN_DELAY;
    rp->disable_failsafe  = 0;

    //Pointer to context for radio IRQ
    rp_modem_radio_planner_p = rp;
    ral_init_irq(&(rp->radio->ral), (void*)RadioOnDioIrq);

    TimerInit( &nextAlarmTimer, rp_timer_irq_callback );
    TimerStop( &nextAlarmTimer );
}
rp_hook_status_t rp_attach_new_radio( radio_planner_t* rp, const ralf_t* radio, const uint8_t hook_id )
{
    if( hook_id >= RP_NB_HOOKS )
    {
        SMTC_MODEM_HAL_PANIC( );
        return RP_HOOK_STATUS_ID_ERROR;
    }
    if( radio ==
        rp->radio_target_attached_to_this_hook[hook_id] )  // radio shall not be the same radio as the initial one
    {
        SMTC_MODEM_HAL_PANIC( );
        return RP_HOOK_STATUS_ID_ERROR;
    }
    rp->radio_target_attached_to_this_hook[hook_id] = radio;
    return RP_HOOK_STATUS_OK;
}
rp_hook_status_t rp_hook_init( radio_planner_t* rp, const uint8_t id, void ( *callback )( void* context ), void* hook )
{
    if( id >= RP_NB_HOOKS )
    {
        SMTC_MODEM_HAL_PANIC( );
        return RP_HOOK_STATUS_ID_ERROR;
    }
    if( ( rp->hook_callbacks[id] != NULL ) || ( callback == NULL ) )
    {
        SMTC_MODEM_HAL_PANIC( );
        return RP_HOOK_STATUS_ID_ERROR;
    }
    rp->status[id]         = RP_STATUS_TASK_INIT;
    rp->hook_callbacks[id] = callback;
    rp->hooks[id]          = hook;
    return RP_HOOK_STATUS_OK;
}

rp_hook_status_t rp_release_hook( radio_planner_t* rp, uint8_t id )
{
    if( id >= RP_NB_HOOKS )
    {
        SMTC_MODEM_HAL_PANIC( );
        return RP_HOOK_STATUS_ID_ERROR;
    }

    rp->hook_callbacks[id]                   = NULL;
    rp->tasks[id].schedule_task_low_priority = false;
    return RP_HOOK_STATUS_OK;
}

rp_hook_status_t rp_hook_get_id( const radio_planner_t* rp, const void* hook, uint8_t* id )
{
    for( int32_t i = 0; i < RP_NB_HOOKS; i++ )
    {
        if( hook == rp->hooks[i] )
        {
            *id = i;
            return RP_HOOK_STATUS_OK;
        }
    }
    SMTC_MODEM_HAL_PANIC( );
    return RP_HOOK_STATUS_ID_ERROR;
}

rp_hook_status_t rp_task_enqueue( radio_planner_t* rp, const rp_task_t* task, uint8_t* payload,
                                  uint16_t payload_buffer_size, const rp_radio_params_t* radio_params )
{
    uint8_t hook_id = task->hook_id;
    if( hook_id >= RP_NB_HOOKS )
    {
        SMTC_MODEM_HAL_PANIC( );
        return RP_HOOK_STATUS_ID_ERROR;
    }
    if( ( task->launch_task_callbacks == NULL ) || ( rp->hook_callbacks[hook_id] == NULL ) )
    {
        SMTC_MODEM_HAL_PANIC( );
        return RP_HOOK_STATUS_ID_ERROR;
    }
    if( ( task->state ) > RP_TASK_STATE_ASAP )
    {
        SMTC_MODEM_HAL_PANIC( " task invalid\n" );
        return RP_HOOK_STATUS_ID_ERROR;
    }
    uint32_t now = SysTimeToMs(SysTimeGet());

    if( ( task->state == RP_TASK_STATE_SCHEDULE ) && ( ( ( int32_t ) ( task->start_time_ms - now ) <= 0 ) ) )
    {
        SMTC_MODEM_HAL_TRACE_PRINTF( " RP: Task enqueue impossible. Task is in past start_time_ms:%d - now:%d = %d\n",
                                     task->start_time_ms, now, task->start_time_ms - now );
        return RP_TASK_STATUS_SCHEDULE_TASK_IN_PAST;
    }

    if( ( ( task->start_time_ms - now ) > 36000000 ) &&
        ( task->start_time_ms > now ) )  // this task is too far in the future, improve robustness to wrapping issue   
    {
        SMTC_MODEM_HAL_TRACE_PRINTF(
            " RP: Task enqueue impossible. Task is too far in future start_time_ms:%d - now:%d = %d\n",
            task->start_time_ms, now, task->start_time_ms - now );
        return RP_TASK_STATUS_TASK_TOO_FAR_IN_FUTURE;
    }

    if( rp->tasks[hook_id].state == RP_TASK_STATE_RUNNING )
    {
        SMTC_MODEM_HAL_TRACE_PRINTF( " RP: Task enqueue impossible. Task is already running\n" );
        return RP_TASK_STATUS_ALREADY_RUNNING;
    }
    if( rp->tasks[hook_id].state != RP_TASK_STATE_FINISHED )
    {
    	MW_LOG( TS_ON, VLEVEL_M, " RP: WARNING Task is already running\r\n" );
    }
    rp->status[hook_id]              = RP_STATUS_TASK_INIT;
    rp->tasks[hook_id]               = *task;
    rp->radio_params[hook_id]        = *radio_params;
    rp->payload[hook_id]             = payload;
    rp->payload_buffer_size[hook_id] = payload_buffer_size;
    if( rp->tasks[hook_id].schedule_task_low_priority == true )
    {
        rp->tasks[hook_id].priority = ( (uint8_t)RP_TASK_STATE_ASAP * RP_NB_HOOKS ) + hook_id;
    }
    else
    {
        rp->tasks[hook_id].priority = ( (uint8_t)rp->tasks[hook_id].state * RP_NB_HOOKS ) + hook_id;
    }
    rp->tasks[hook_id].start_time_init_ms = rp->tasks[hook_id].start_time_ms;
    MW_LOG( TS_ON, VLEVEL_H, "RP: Task #%u enqueue with #%u priority\n", hook_id, rp->tasks[hook_id].priority );
    rp_task_compute_ranking( rp );
    if( rp->radio_irq_flag == false )
    {
        rp_task_arbiter( rp, __func__ );
    }
    return RP_HOOK_STATUS_OK;
}

rp_hook_status_t rp_task_abort( radio_planner_t* rp, const uint8_t hook_id )
{
    SMTC_MODEM_HAL_RP_TRACE_PRINTF( "RP: rp_task_abort #%d\n", hook_id );
    if( hook_id >= RP_NB_HOOKS )
    {
        SMTC_MODEM_HAL_PANIC( );
        return RP_HOOK_STATUS_ID_ERROR;
    }

    if( rp->tasks[hook_id].state > RP_TASK_STATE_ABORTED )
    {
        return RP_HOOK_STATUS_OK;
    }

    if( rp->tasks[hook_id].state == RP_TASK_STATE_RUNNING )
    {
#if defined( ADD_LBM_GEOLOCATION )
        if( ( hook_id == RP_HOOK_ID_DIRECT_RP_ACCESS_GNSS ) ||
            ( hook_id == RP_HOOK_ID_DIRECT_RP_ACCESS_GNSS_ALMANAC ) || ( hook_id == RP_HOOK_ID_DIRECT_RP_ACCESS_WIFI ) )
        {
            SMTC_MODEM_HAL_PANIC_ON_FAILURE(
                lr11xx_system_abort_blocking_cmd( rp->radio_target_attached_to_this_hook[hook_id]->ral.context ) ==
                LR11XX_STATUS_OK );
        }
#endif

        rp->radio_irq_flag                         = true;  // Fake soft IRQ to call user callback
#ifdef RELAY
        rp->irq_timestamp_100us[rp->radio_task_id] = UTIL_TIMER_GetCurrentTime() * 10000;
#endif //RELAY
        rp->irq_timestamp_ms[rp->radio_task_id]    = SysTimeToMs(SysTimeGet());
        smtc_modem_hal_user_lbm_irq( );
    }
    else
    {
        rp->tasks[hook_id].state = RP_TASK_STATE_ABORTED;

        if( rp->radio_irq_flag == false )
        {
            rp_task_arbiter( rp, __func__ );
        }
    }
    return RP_HOOK_STATUS_OK;
}

void rp_get_status( const radio_planner_t* rp, const uint8_t id, uint32_t* irq_timestamp_ms, rp_status_t* status )
{
    if( id >= RP_NB_HOOKS )
    {
        SMTC_MODEM_HAL_PANIC( );
        return;
    }
    *irq_timestamp_ms = rp->irq_timestamp_ms[id];
    *status           = rp->status[id];
}

void rp_get_and_clear_raw_radio_irq( radio_planner_t* rp, const uint8_t id, ral_irq_t* raw_radio_irq )
{
    if( id >= RP_NB_HOOKS )
    {
        SMTC_MODEM_HAL_PANIC( );
        return;
    }
    *raw_radio_irq        = rp->raw_radio_irq[id];
    rp->raw_radio_irq[id] = 0;
}

rp_stats_t rp_get_stats( const radio_planner_t* rp )
{
    return rp->stats;
}

void rp_callback( radio_planner_t* rp )
{
    if( ( rp->tasks[rp->radio_task_id].state == RP_TASK_STATE_RUNNING ) &&
        ( rp->disable_failsafe != RP_DISABLE_FAILSAFE_KEY ) &&
        ( ( int32_t ) ( rp->tasks[rp->radio_task_id].start_time_ms + 128000 - SysTimeToMs(SysTimeGet())) < 0 ) )
    {
    	rp->tasks[rp->radio_task_id].state = RP_TASK_STATE_ABORTED;
    	rp_task_call_aborted( rp );

    	SMTC_MODEM_HAL_PANIC( "RP_FAILSAFE - #%d\n", rp->radio_task_id );
    }
    if( ( rp->radio_irq_flag == false ) && ( rp->timer_irq_flag == false ) )
    {
        return;
    }
    if( ( rp->radio_irq_flag == false ) && ( rp->timer_irq_flag == true ) )
    {
        rp->timer_irq_flag = false;
        rp_timer_irq( rp );
        return;
    }
    if( rp->radio_irq_flag == true )
    {
        rp->radio_irq_flag = false;
        if( rp->tasks[rp->radio_task_id].state < RP_TASK_STATE_ABORTED )
        {
            SMTC_MODEM_HAL_RP_TRACE_PRINTF( " RP: INFO - Radio IRQ received for hook #%u\n", rp->radio_task_id );

            if( rp->status[rp->radio_task_id] == RP_STATUS_LR_FHSS_HOP )
            {
                return;
            }

            // Compute statistics after LR-FHSS hopping interrupts to have the correct TOA and not only by hop
            rp_consumption_statistics_updated( rp, rp->radio_task_id, rp->irq_timestamp_ms[rp->radio_task_id] );

            // Tx can be performed only if no activity detected on channel
            if( ( rp->status[rp->radio_task_id] == RP_STATUS_CAD_NEGATIVE ) &&
                ( rp->tasks[rp->radio_task_id].type == RP_TASK_TYPE_CAD_TO_TX ) )
            {
                rp->radio = TARGET_RADIO;
                rp_hook_callback( rp, rp->radio_task_id );
                return;
            }

            // Rx can be performed if activity detected on channel
            if( ( rp->status[rp->radio_task_id] == RP_STATUS_CAD_POSITIVE ) &&
                ( rp->tasks[rp->radio_task_id].type == RP_TASK_TYPE_CAD_TO_RX ) )
            {
                rp->radio = TARGET_RADIO;
                rp_hook_callback( rp, rp->radio_task_id );
                return;
            }

            if( rp->tasks[rp->radio_task_id].type == RP_TASK_TYPE_RX_BLE_SCAN )
            {
                if( ( rp->status[rp->radio_task_id] == RP_STATUS_RX_PACKET ) ||
                    ( rp->status[rp->radio_task_id] == RP_STATUS_RX_CRC_ERROR ) )
                {
                    rp->radio = TARGET_RADIO;
                    rp_hook_callback( rp, rp->radio_task_id );
                    return;
                }
            }

            // Have to call rp_task_free before rp_hook_callback because the callback can enqueued a task and so call
            // the arbiter
            rp_task_free( rp, &rp->tasks[rp->radio_task_id] );
            SMTC_MODEM_HAL_PANIC_ON_FAILURE( ral_set_sleep( TARGET_RAL, true ) == RAL_STATUS_OK );
            rp->radio = TARGET_RADIO;
            rp_hook_callback( rp, rp->radio_task_id );

            rp_task_call_aborted( rp );

            rp_task_arbiter( rp, __func__ );
        }
        else
        {
            // A radio irq happened and no rp task is on going, put radio to sleep
            // even in case multiple radio put only main radio in sleep
            for( int i = 0; i < RP_NB_HOOKS; i++ )
            {
                SMTC_MODEM_HAL_PANIC_ON_FAILURE(
                    ral_set_sleep( &( rp->radio_target_attached_to_this_hook[i]->ral ), true ) == RAL_STATUS_OK );
            }
            SMTC_MODEM_HAL_TRACE_PRINTF( " radio planner it but no more task activated\n" );
        }

        ral_set_ant_switch( TARGET_RAL, false );
        // Shut Down the TCXO
        smtc_modem_hal_stop_radio_tcxo( );
    }
}

bool rp_get_irq_flag( void* obj )
{
    radio_planner_t* rp = ( ( radio_planner_t* ) obj );
    if( ( rp->timer_irq_flag == true ) || ( rp->radio_irq_flag == true ) )
    {
        return true;
    }
    else
    {
        return false;
    }
}
void rp_disable_failsafe( radio_planner_t* rp, bool disable )
{
    if( disable == true )
    {
        rp->disable_failsafe = RP_DISABLE_FAILSAFE_KEY;
    }
    else
    {
        rp->disable_failsafe = 0;
    }
}
/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE FUNCTIONS DEFINITION --------------------------------------------
 */

static void rp_task_free( const radio_planner_t* rp, rp_task_t* task )
{
    task->hook_id            = RP_NB_HOOKS;
    task->start_time_ms      = 0;
    task->start_time_init_ms = 0;
    task->duration_time_ms   = 0;
    //   task->type               = RP_TASK_TYPE_NONE; doesn't clear for suspend feature
    task->state                      = RP_TASK_STATE_FINISHED;
    task->schedule_task_low_priority = false;
}

static void rp_task_update_time( radio_planner_t* rp, uint32_t now )
{
    for( int32_t i = 0; i < RP_NB_HOOKS; i++ )
    {
        if( rp->tasks[i].state == RP_TASK_STATE_ASAP )
        {
            if( ( int32_t ) ( now - rp->tasks[i].start_time_ms ) > 0 )
            {
                rp->tasks[i].start_time_ms = now + RP_MCU_FAIRNESS_DELAY_MS;
            }

            // An asap task is automatically switch in schedule task after RP_TASK_ASAP_TO_SCHEDULE_TRIG_TIME ms
            if( ( int32_t ) ( now - rp->tasks[i].start_time_init_ms ) > RP_TASK_ASAP_TO_SCHEDULE_TRIG_TIME )
            {
                rp->tasks[i].state = RP_TASK_STATE_SCHEDULE;
                // Schedule the task at (now + RP_TASK_RE_SCHEDULE_OFFSET_TIME) seconds
                rp->tasks[i].start_time_ms = now + RP_TASK_RE_SCHEDULE_OFFSET_TIME;
                rp->tasks[i].priority      = ( (uint8_t)rp->tasks[i].state * RP_NB_HOOKS ) + i;
                SMTC_MODEM_HAL_TRACE_WARNING(
                    "RP: SWITCH TASK #%d FROM ASAP TO SCHEDULED (start_time_init_ms:%u, now:%u, diff:%d)\n", i,
                    rp->tasks[i].start_time_init_ms, now, ( int32_t ) ( now - rp->tasks[i].start_time_init_ms ) );
                rp_task_compute_ranking( rp );
            }
        }
    }

    if( ( rp->tasks[rp->radio_task_id].state == RP_TASK_STATE_RUNNING ) &&
        ( ( rp->tasks[rp->radio_task_id].type == RP_TASK_TYPE_RX_LORA ) ||
          ( rp->tasks[rp->radio_task_id].type == RP_TASK_TYPE_RX_FSK ) ||
          ( rp->tasks[rp->radio_task_id].type == RP_TASK_TYPE_RX_BLE ) ||
          ( rp->tasks[rp->radio_task_id].type == RP_TASK_TYPE_RX_BLE_SCAN ) ) )
    {
        rp->tasks[rp->radio_task_id].duration_time_ms =
            now + rp->margin_delay + 2 - rp->tasks[rp->radio_task_id].start_time_ms;
        if( rp->tasks[rp->radio_task_id].schedule_task_low_priority == true )
        {
            rp->tasks[rp->radio_task_id].schedule_task_low_priority = false;
            rp->tasks[rp->radio_task_id].priority = ( (uint8_t)RP_TASK_STATE_SCHEDULE * RP_NB_HOOKS ) + rp->radio_task_id;
            rp_task_compute_ranking( rp );
        }
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " RP: Extended duration of radio task #%u time to %lu ms\n", rp->radio_task_id,
                                        now );
    }
}

static void rp_task_arbiter( radio_planner_t* rp, const char* caller_func_name )
{
    uint32_t now = SysTimeToMs(SysTimeGet());
    // Update time for ASAP task to now. But, also extended duration in case of running task is a RX task
    rp_task_update_time( rp, now );

    // Select the high priority task
    if( rp_task_select_next( rp, now ) == RP_SOMETHING_TO_DO )
    {  // Next task exists
    	MW_LOG( TS_ON, VLEVEL_H,  "RP: start= %08d, now= %08d \r\n", rp->priority_task.start_time_ms, now );
        int32_t delay = ( int32_t ) ( rp->priority_task.start_time_ms - now );
        MW_LOG( TS_ON, VLEVEL_H,
            " RP: Arbiter has been called by %s and priority-task #%d, timer hook #%d, delay %d, now %d\n ",
            caller_func_name, rp->priority_task.hook_id, rp->timer_hook_id, delay, now );

        // Case where the high priority task is in the past, error case
        if( delay < 0 )
        {  // The high priority task is in the past, error case
            if( rp->priority_task.state != RP_TASK_STATE_RUNNING )
            {
                rp->stats.rp_error++;
                MW_LOG( TS_ON, VLEVEL_M,  " RP: ERROR - delay #%d - hook #%d\n", delay, rp->priority_task.hook_id );

                rp->tasks[rp->priority_task.hook_id].state = RP_TASK_STATE_ABORTED;
            }
        }
        // Case where the high priority task is in the future
        else if( ( uint32_t ) delay > rp->margin_delay )
        {  // The high priority task is in the future
        	MW_LOG( TS_ON, VLEVEL_H,  " RP: High priority task is in the future\n" );
        }
        // Case where the high priority task is now
        else
        {
            if( rp->tasks[rp->radio_task_id].state == RP_TASK_STATE_RUNNING )
            {  // Radio is already running
                if( rp->tasks[rp->radio_task_id].hook_id != rp->priority_task.hook_id )
                {  // priority task not equal to radio task => abort radio task if the priority task is SCHEDULED
                    rp->tasks[rp->radio_task_id].state = RP_TASK_STATE_ABORTED;
                    //MW_LOG( TS_ON, VLEVEL_H, "RP: Abort running #%u for priority #%u\r\n", rp->radio_task_id, rp->priority_task.hook_id );
#if defined( ADD_LBM_GEOLOCATION )
                    if( ( rp->radio_task_id == RP_HOOK_ID_DIRECT_RP_ACCESS_GNSS ) ||
                        ( rp->radio_task_id == RP_HOOK_ID_DIRECT_RP_ACCESS_GNSS_ALMANAC ) ||
                        ( rp->radio_task_id == RP_HOOK_ID_DIRECT_RP_ACCESS_WIFI ) )
                    {
                        SMTC_MODEM_HAL_PANIC_ON_FAILURE(
                            lr11xx_system_abort_blocking_cmd(
                                rp->radio_target_attached_to_this_hook[rp->radio_task_id]->ral.context ) ==
                            LR11XX_STATUS_OK );
                    }
#endif
                    SMTC_MODEM_HAL_PANIC_ON_FAILURE( ral_set_standby( TARGET_RAL, RAL_STANDBY_CFG_RC ) ==
                                                     RAL_STATUS_OK );
                    SMTC_MODEM_HAL_PANIC_ON_FAILURE( ral_clear_irq_status( TARGET_RAL, RAL_IRQ_ALL ) == RAL_STATUS_OK );

                    SMTC_MODEM_HAL_PANIC_ON_FAILURE( ral_set_sleep( TARGET_RAL, true ) == RAL_STATUS_OK );

                    rp->radio_irq_flag = false;

                    ral_set_ant_switch( TARGET_RAL, false );
                    // Shut Down the TCXO
                    smtc_modem_hal_stop_radio_tcxo( );

                    rp_consumption_statistics_updated( rp, rp->radio_task_id, SysTimeToMs(SysTimeGet()));

                    rp->radio_task_id                  = rp->priority_task.hook_id;
                    rp->tasks[rp->radio_task_id].state = RP_TASK_STATE_RUNNING;
                    rp_task_launch_current( rp );
                }  // else case already managed during enqueue task
            }
            else
            {  // Radio is sleeping start priority task on radio
                rp->radio_task_id                  = rp->priority_task.hook_id;
                rp->tasks[rp->radio_task_id].state = RP_TASK_STATE_RUNNING;
                rp_task_launch_current( rp );
            }
        }
        // Timer has expired on a not priority task => Have to abort this task
        int32_t tmp = ( int32_t ) ( rp->tasks[rp->timer_hook_id].start_time_ms - now );

        if( tmp > 0 )
        {
            if( ( ( uint32_t ) tmp < rp->margin_delay ) && ( rp->next_state_status == RP_STATUS_HAVE_TO_SET_TIMER ) &&
                ( rp->timer_hook_id != rp->priority_task.hook_id ) &&
                ( rp->tasks[rp->timer_hook_id].state == RP_TASK_STATE_SCHEDULE ) )
            {
            	MW_LOG( TS_ON, VLEVEL_H,  " RP: Aborted task with hook #%u - not a priority task\r\n ",
                                              rp->timer_hook_id );
                rp->tasks[rp->timer_hook_id].state = RP_TASK_STATE_ABORTED;
            }
        }
        // Execute the garbage collection if the radio isn't running
        if( rp->tasks[rp->radio_task_id].state != RP_TASK_STATE_RUNNING )
        {
            rp_task_call_aborted( rp );
        }

        // Set the Timer to the next Task
        rp->next_state_status =
            rp_task_get_next( rp, &rp->timer_value, &rp->timer_hook_id, SysTimeToMs(SysTimeGet()));

        if( rp->next_state_status == RP_STATUS_HAVE_TO_SET_TIMER )
        {
            if( rp->timer_value > rp->margin_delay )
            {
                rp_set_alarm( rp, rp->timer_value - rp->margin_delay );
            }
            else
            {
                rp->timer_irq_flag = true;
            }
        }
    }
    else
    {  // No more tasks in the radio planner
        rp_task_call_aborted( rp );
        //MW_LOG( TS_OFF, VLEVEL_H, " RP: No more active tasks\n" );
    }
}

static void rp_task_compute_ranking( radio_planner_t* rp )
{
    uint8_t rank;
    uint8_t ranks_temp[RP_NB_HOOKS];

    for( int32_t i = 0; i < RP_NB_HOOKS; i++ )
    {
        ranks_temp[i] = rp->tasks[i].priority;
    }
    for( int32_t i = 0; i < RP_NB_HOOKS; i++ )
    {
        rank             = rp_task_find_highest_priority( &( ranks_temp[0] ), RP_NB_HOOKS );
        ranks_temp[rank] = 0xFF;
        rp->rankings[i]  = rank;
    }
}

static void rp_task_launch_current( radio_planner_t* rp )
{
    uint8_t id = rp->radio_task_id;
    SMTC_MODEM_HAL_RP_TRACE_PRINTF( " RP: Launch task #%u and start radio state %u, type %u\n", id, rp->tasks[id].state,
                                    rp->tasks[id].type );
    if( rp->tasks[id].launch_task_callbacks == NULL )
    {
    	MW_LOG( TS_ON, VLEVEL_M, " RP: ERROR - launch_task_callbacks == NULL \r\n" );
    }
    else
    {
        rp_task_print( rp, &rp->tasks[id] );
        rp->radio = TARGET_RADIO;
        rp->tasks[id].launch_task_callbacks( ( void* ) rp );
    }
}

static uint8_t rp_task_select_next( radio_planner_t* rp, const uint32_t now )
{
    uint8_t  hook_to_exe_tmp      = 0xFF;
    uint32_t hook_time_to_exe_tmp = 0;
    uint32_t hook_duration_tmp    = 0;
    uint32_t time_tmp             = 0;
    uint8_t  rank                 = 0;
    uint8_t  hook_id              = 0;

    for( hook_id = 0; hook_id < RP_NB_HOOKS; hook_id++ )
    {  // Garbage collector
        if( ( rp->tasks[hook_id].state == RP_TASK_STATE_SCHEDULE ) &&
            ( ( ( int32_t ) ( rp->tasks[hook_id].start_time_ms - now ) < 0 ) ) )
        {
            rp->tasks[hook_id].state = RP_TASK_STATE_ABORTED;
        }
    }
    for( hook_id = 0; hook_id < RP_NB_HOOKS; hook_id++ )
    {
        rank = rp->rankings[hook_id];
        if( ( ( rp->tasks[rank].state < RP_TASK_STATE_RUNNING ) &&
              ( ( int32_t ) ( rp->tasks[rank].start_time_ms - now ) >= 0 ) ) ||
            ( rp->tasks[rank].state == RP_TASK_STATE_RUNNING ) )
        {
            hook_to_exe_tmp      = rp->tasks[rank].hook_id;
            hook_time_to_exe_tmp = rp->tasks[rank].start_time_ms;
            hook_duration_tmp    = rp->tasks[rank].duration_time_ms;
            break;
        }
    }
    if( hook_id == RP_NB_HOOKS )
    {
        return RP_NO_MORE_TASK;
    }

    for( int32_t i = hook_id + 1; i < RP_NB_HOOKS; i++ )
    {
        rank = rp->rankings[i];
        if( ( ( rp->tasks[rank].state < RP_TASK_STATE_RUNNING ) &&
              ( ( int32_t ) ( rp->tasks[rank].start_time_ms - now ) >= 0 ) ) ||
            ( rp->tasks[rank].state == RP_TASK_STATE_RUNNING ) )
        {
            time_tmp = rp->tasks[rank].start_time_ms + rp->tasks[rank].duration_time_ms;

            int32_t tmp = ( int32_t ) ( time_tmp - hook_time_to_exe_tmp );
            if( ( tmp < 0 ) && ( ( int32_t ) ( time_tmp - now ) >= 0 ) )
            {
                hook_to_exe_tmp      = rp->tasks[rank].hook_id;
                hook_time_to_exe_tmp = rp->tasks[rank].start_time_ms;
                hook_duration_tmp    = rp->tasks[rank].duration_time_ms;
            }
            if( ( tmp > 0 ) && ( tmp < ( int32_t ) hook_duration_tmp ) )
            {
                if( rp->tasks[rank].state == RP_TASK_STATE_ASAP )
                {
#if defined (ENDNODE) || defined (ENDNODE_RELAY)
                    rp->tasks[rank].start_time_ms = hook_time_to_exe_tmp + hook_duration_tmp + RP_MCU_FAIRNESS_DELAY_MS;
#else
					rp->tasks[rank].start_time_ms = hook_time_to_exe_tmp + hook_duration_tmp + 100;
#endif //RELAY
                }
            }
        }
    }
    rp->priority_task = rp->tasks[hook_to_exe_tmp];
    return RP_SOMETHING_TO_DO;
}

static rp_next_state_status_t rp_task_get_next( radio_planner_t* rp, uint32_t* duration, uint8_t* task_id,
                                                const uint32_t now )
{
    uint8_t  hook_id  = 0;
    uint8_t  index    = 0;
    uint32_t time_tmp = now;

    for( hook_id = 0; hook_id < RP_NB_HOOKS; hook_id++ )
    {  // Garbage collector
        if( ( rp->tasks[hook_id].state == RP_TASK_STATE_SCHEDULE ) &&
            ( ( ( int32_t ) ( rp->tasks[hook_id].start_time_ms - time_tmp ) < 0 ) ) )
        {
            rp->tasks[hook_id].state = RP_TASK_STATE_ABORTED;
        }
    }
    for( hook_id = 0; hook_id < RP_NB_HOOKS; hook_id++ )
    {
    	//MW_LOG( TS_OFF, VLEVEL_M,  "RP: next task id= %d, state= %d, start_time=%08d, time_temp=%08d\r\n", hook_id, rp->tasks[hook_id].state, rp->tasks[hook_id].start_time_ms, time_tmp );
        if( ( rp->tasks[hook_id].state < RP_TASK_STATE_RUNNING ) &&
            ( ( ( int32_t ) ( rp->tasks[hook_id].start_time_ms - time_tmp ) >= 0 ) ) )
        {
            time_tmp = rp->tasks[hook_id].start_time_ms;
            index    = hook_id;
            break;
        }
    }
    if( hook_id == RP_NB_HOOKS )
    {
        return RP_STATUS_NO_MORE_TASK_SCHEDULE;
    }

    for( uint8_t i = hook_id; i < RP_NB_HOOKS; i++ )
    {
        if( ( rp->tasks[i].state < RP_TASK_STATE_RUNNING ) &&
            ( ( int32_t ) ( rp->tasks[i].start_time_ms - time_tmp ) < 0 ) &&
            ( ( int32_t ) ( rp->tasks[i].start_time_ms - now ) >= 0 ) )
        {
            time_tmp = rp->tasks[i].start_time_ms;
            index    = i;
        }
    }
    *task_id  = index;
    *duration = time_tmp - now;
    return RP_STATUS_HAVE_TO_SET_TIMER;
}

static uint8_t rp_task_find_highest_priority( uint8_t* vector, uint8_t length )
{
    uint8_t priority_high = 0xFF;
    uint8_t index         = 0;

    for( int32_t i = 0; i < length; i++ )
    {
        if( vector[i] <= priority_high )
        {
            priority_high = vector[i];
            index         = i;
        }
    }
    return index;
}

rp_hook_status_t rp_get_pkt_payload( radio_planner_t* rp, const rp_task_t* task )
{
    rp_hook_status_t status = RP_HOOK_STATUS_OK;
    uint8_t          id     = task->hook_id;

    if( ( task->type == RP_TASK_TYPE_USER ) || ( task->type == RP_TASK_TYPE_NONE ) )
    {
        return status;  // don't catch the payload in case of user task
    }

    SMTC_MODEM_HAL_PANIC_ON_FAILURE( ral_get_pkt_payload( &( rp->radio_target_attached_to_this_hook[id]->ral ),
                                                          rp->payload_buffer_size[id], rp->payload[id],
                                                          &rp->rx_payload_size[id] ) == RAL_STATUS_OK );

    MW_LOG( TS_ON, VLEVEL_M,  " RP: RX size %d\r\n", rp->rx_payload_size[id] );
    if( ( task->type == RP_TASK_TYPE_RX_LORA ) || ( task->type == RP_TASK_TYPE_CAD_TO_RX ) )
    {
        rp->radio_params[id].pkt_type = RAL_PKT_TYPE_LORA;
        status                        = RP_HOOK_STATUS_OK;

        SMTC_MODEM_HAL_PANIC_ON_FAILURE(
            ral_get_lora_rx_pkt_status( &( rp->radio_target_attached_to_this_hook[id]->ral ),
                                        &rp->radio_params[id].rx.lora_pkt_status ) == RAL_STATUS_OK );
    }
    else if( task->type == RP_TASK_TYPE_RX_FSK )
    {
        rp->radio_params[id].pkt_type = RAL_PKT_TYPE_GFSK;
        status                        = RP_HOOK_STATUS_OK;

        SMTC_MODEM_HAL_PANIC_ON_FAILURE(
            ral_get_gfsk_rx_pkt_status( &( rp->radio_target_attached_to_this_hook[id]->ral ),
                                        &rp->radio_params[id].rx.gfsk_pkt_status ) == RAL_STATUS_OK );
    }
    else if( ( task->type == RP_TASK_TYPE_RX_BLE ) || ( task->type == RP_TASK_TYPE_RX_BLE_SCAN ) )
    {
        rp->radio_params[id].pkt_type = RAL_PKT_TYPE_GFSK;
        status                        = RP_HOOK_STATUS_OK;

        // BLE using GFSK
        SMTC_MODEM_HAL_PANIC_ON_FAILURE(
            ral_get_gfsk_rx_pkt_status( &( rp->radio_target_attached_to_this_hook[id]->ral ),
                                        &rp->radio_params[id].rx.gfsk_pkt_status ) == RAL_STATUS_OK );
    }
    else
    {
        status = RP_HOOK_STATUS_ID_ERROR;
    }

    return status;
}

static void rp_set_alarm( radio_planner_t* rp, const uint32_t alarm_in_ms )
{
	TimerStop( &nextAlarmTimer );
	TimerSetValue(&nextAlarmTimer, alarm_in_ms);
	TimerStart(&nextAlarmTimer );
}

static void rp_timer_irq( radio_planner_t* rp )
{
    rp_task_arbiter( rp, __func__ );
}

static void rp_task_call_aborted( radio_planner_t* rp )
{
    for( int32_t i = 0; i < RP_NB_HOOKS; i++ )
    {
        if( rp->tasks[i].state == RP_TASK_STATE_ABORTED )
        {
        	MW_LOG( TS_ON, VLEVEL_H,  "RP INFO: task id %d aborted\r\n", i );
            rp->stats.task_hook_aborted_nb[i]++;
            rp_task_free( rp, &rp->tasks[i] );
            rp->status[i] = RP_STATUS_TASK_ABORTED;
            rp->radio     = rp->radio_target_attached_to_this_hook[i];
            rp_hook_callback( rp, i );
        }
    }
}

static void rp_task_print( const radio_planner_t* rp, const rp_task_t* task )
{
    SMTC_MODEM_HAL_RP_TRACE_PRINTF( "\nRP- INFO - Radio task #%u  running - Timer task #%u running  - Hook ID #%u -",
                                    rp->radio_task_id, rp->timer_hook_id, task->hook_id );
    switch( task->type )
    {
    case RP_TASK_TYPE_RX_LORA:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_RX_LORA " );
        break;
    case RP_TASK_TYPE_RX_FSK:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_RX_FSK " );
        break;
    case RP_TASK_TYPE_TX_LORA:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_TX_LORA " );
        break;
    case RP_TASK_TYPE_TX_FSK:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_TX_FSK " );
        break;
    case RP_TASK_TYPE_TX_LR_FHSS:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_TX_LR_FHSS " );
        break;
    case RP_TASK_TYPE_CAD:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_CAD " );
        break;
    case RP_TASK_TYPE_CAD_TO_TX:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_CAD_TO_TX " );
        break;
    case RP_TASK_TYPE_CAD_TO_RX:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_CAD_TO_RX " );
        break;
    case RP_TASK_TYPE_TX_BLE:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_TX_BLE " );
        break;
    case RP_TASK_TYPE_RX_BLE:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_RX_BLE " );
        break;
    case RP_TASK_TYPE_RX_BLE_SCAN:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_RX_BLE_SCAN " );
        break;
    case RP_TASK_TYPE_NONE:
    case RP_TASK_TYPE_GNSS_SNIFF:
    case RP_TASK_TYPE_WIFI_SNIFF:
    case RP_TASK_TYPE_GNSS_RSSI:
    case RP_TASK_TYPE_WIFI_RSSI:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_EMPTY " );
        break;
    default:
        SMTC_MODEM_HAL_RP_TRACE_PRINTF( " TASK_ERROR " );
        break;
    };
    SMTC_MODEM_HAL_RP_TRACE_PRINTF( " - start time @%lu - priority #%u\n", task->start_time_ms, task->priority );
}

static void rp_consumption_statistics_updated( radio_planner_t* rp, const uint8_t hook_id, const uint32_t time )
{
    uint32_t micro_ampere_radio = 0, micro_ampere_process = 0;
    uint32_t radio_t = 0, process_t = 0;
    uint32_t tx_freq_hz = 0;

    // TODO RP_TASK_TYPE_CAD_TO_TX, RP_TASK_TYPE_USER, RP_TASK_TYPE_NONE,

    if( ( rp->tasks[hook_id].type == RP_TASK_TYPE_RX_LORA ) || ( rp->tasks[hook_id].type == RP_TASK_TYPE_CAD ) ||
        ( rp->tasks[hook_id].type == RP_TASK_TYPE_CAD_TO_RX ) )
    {
        bool enable_boost_mode = false;
        ral_cfg_rx_boosted( TARGET_RAL_FOR_HOOK_ID, enable_boost_mode );
        ral_get_lora_rx_consumption_in_ua( TARGET_RAL_FOR_HOOK_ID, rp->radio_params[hook_id].rx.lora.mod_params.bw,
                                           enable_boost_mode, &micro_ampere_radio );
    }
    else if( ( rp->tasks[hook_id].type == RP_TASK_TYPE_RX_FSK ) || ( rp->tasks[hook_id].type == RP_TASK_TYPE_LBT ) )
    {
        bool enable_boost_mode = false;
        ral_cfg_rx_boosted( TARGET_RAL_FOR_HOOK_ID, enable_boost_mode );
        ral_get_gfsk_rx_consumption_in_ua(
            TARGET_RAL_FOR_HOOK_ID, rp->radio_params[hook_id].rx.gfsk.mod_params.br_in_bps,
            rp->radio_params[hook_id].rx.gfsk.mod_params.bw_dsb_in_hz, enable_boost_mode, &micro_ampere_radio );
    }
    else if( rp->tasks[hook_id].type == RP_TASK_TYPE_TX_LORA )
    {
        ral_get_tx_consumption_in_ua( TARGET_RAL_FOR_HOOK_ID, rp->radio_params[hook_id].tx.lora.output_pwr_in_dbm,
                                      rp->radio_params[hook_id].tx.lora.rf_freq_in_hz, &micro_ampere_radio );
        tx_freq_hz = rp->radio_params[hook_id].tx.lora.rf_freq_in_hz;
    }
    else if( rp->tasks[hook_id].type == RP_TASK_TYPE_TX_FSK )
    {
        ral_get_tx_consumption_in_ua( TARGET_RAL_FOR_HOOK_ID, rp->radio_params[hook_id].tx.gfsk.output_pwr_in_dbm,
                                      rp->radio_params[hook_id].tx.gfsk.rf_freq_in_hz, &micro_ampere_radio );
        tx_freq_hz = rp->radio_params[hook_id].tx.gfsk.rf_freq_in_hz;
    }
    else if( rp->tasks[hook_id].type == RP_TASK_TYPE_TX_LR_FHSS )
    {
        // The power and duty-cycle are computed only on the center_frequency_in_hz and not on all hopping
        ral_get_tx_consumption_in_ua( TARGET_RAL_FOR_HOOK_ID, rp->radio_params[hook_id].tx.lr_fhss.output_pwr_in_dbm,
                                      rp->radio_params[hook_id].tx.lr_fhss.ral_lr_fhss_params.center_frequency_in_hz,
                                      &micro_ampere_radio );

        tx_freq_hz = rp->radio_params[hook_id].tx.lr_fhss.ral_lr_fhss_params.center_frequency_in_hz;
    }

    if( ( rp->tasks[hook_id].type == RP_TASK_TYPE_GNSS_SNIFF ) ||
        ( rp->tasks[hook_id].type == RP_TASK_TYPE_GNSS_RSSI ) ||
        ( rp->tasks[hook_id].type == RP_TASK_TYPE_WIFI_SNIFF ) ||
        ( rp->tasks[hook_id].type == RP_TASK_TYPE_WIFI_RSSI ) )
    {
        rp_stats_sniff_update( &rp->stats, time, radio_t, process_t, hook_id, micro_ampere_radio,
                               micro_ampere_process );
    }
    else if( ( rp->tasks[hook_id].type == RP_TASK_TYPE_TX_BLE ) || ( rp->tasks[hook_id].type == RP_TASK_TYPE_RX_BLE ) ||
             ( rp->tasks[hook_id].type == RP_TASK_TYPE_RX_BLE_SCAN ) )
    {
        // TODO for this kind of task
    }
    else
    {
        uint32_t tx_timestamp_tmp = rp->stats.tx_timestamp;
        rp_stats_update( &rp->stats, time, hook_id, micro_ampere_radio );
        if( tx_timestamp_tmp != 0 )
        {
            smtc_duty_cycle_sum( tx_freq_hz, rp->stats.tx_last_toa_ms[hook_id] );
        }
    }
}

/*
 * -----------------------------------------------------------------------------
 * --- CALLBACK FUNCTIONS DEFINITION -------------------------------------------
 */

static void RadioOnDioIrq( irq_mask_t radioIrq )
{
    RadioIrqProcess(radioIrq);
}

static void RadioIrqProcess( irq_mask_t radioIrq )
{
    radio_planner_t* rp = rp_modem_radio_planner_p;
    rp->radio_irq_flag = true;
#ifdef RELAY
    rp->irq_timestamp_100us[rp->radio_task_id] = UTIL_TIMER_GetCurrentTime() * 10000;
#endif // RELAY
    rp->irq_timestamp_ms[rp->radio_task_id]    = SysTimeToMs(SysTimeGet());

    if( rp->tasks[rp->radio_task_id].type != RP_TASK_TYPE_LBT )
    {
		switch( radioIrq )
		{
		case IRQ_RADIO_TX_DONE:
			DBG_GPIO_RADIO_TX( RST );
			MW_LOG( TS_ON, VLEVEL_M,  " TX DONE\r\n" );
			rp->status[rp->radio_task_id] = RP_STATUS_TX_DONE;

			if( rp->priority_task.type == RP_TASK_TYPE_TX_LR_FHSS )
			{
				ral_lr_fhss_handle_tx_done( TARGET_RAL_FOR_HOOK_ID,
						&rp->radio_params[rp->radio_task_id].tx.lr_fhss.ral_lr_fhss_params, NULL );

				// fix issue with delay between the end of the transmission and the generation of txdone irq
				// get delay according to lr-fhss params
				uint16_t bit_delay_us = 0;
				ral_lr_fhss_get_bit_delay_in_us( TARGET_RAL_FOR_HOOK_ID, &rp->radio_params[rp->radio_task_id].tx.lr_fhss.ral_lr_fhss_params,
										rp->payload_buffer_size[rp->radio_task_id], &bit_delay_us );

				rp->irq_timestamp_ms[rp->radio_task_id] = rp->irq_timestamp_ms[rp->radio_task_id] - ( ( uint32_t ) bit_delay_us / 1000 );
			}

			//!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
			SMTC_MODEM_HAL_PANIC_ON_FAILURE( ral_set_standby( TARGET_RAL, RAL_STANDBY_CFG_RC ) == RAL_STATUS_OK );

			break;

		case IRQ_RADIO_RX_DONE:
			DBG_GPIO_RADIO_RX( RST );
			MW_LOG( TS_ON, VLEVEL_M,  " RX DONE\r\n" );
			rp->status[rp->radio_task_id] = RP_STATUS_RX_PACKET;

			if( rp_get_pkt_payload( rp, &rp->tasks[rp->radio_task_id] ) == RP_HOOK_STATUS_ID_ERROR )
			{
				SMTC_MODEM_HAL_PANIC( );
				return;
			}

      //!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
      SMTC_MODEM_HAL_PANIC_ON_FAILURE( ral_set_standby( TARGET_RAL, RAL_STANDBY_CFG_RC ) == RAL_STATUS_OK );

      ral_handle_rx_done(TARGET_RAL_FOR_HOOK_ID);
			break;

		case IRQ_RADIO_CAD_DONE:  //SX126X_IRQ_CAD_DONE
			MW_LOG( TS_ON, VLEVEL_H,  " CAD DONE\r\n" );
			rp->status[rp->radio_task_id] = RP_STATUS_CAD_NEGATIVE;

			//!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
			SMTC_MODEM_HAL_PANIC_ON_FAILURE( ral_set_standby( TARGET_RAL, RAL_STANDBY_CFG_RC ) == RAL_STATUS_OK );


			break;
		case IRQ_RADIO_CAD_DETECTED:  // it is also named as CAD_OK
			MW_LOG( TS_ON, VLEVEL_M,  " CAD OK\r\n" );
			rp->status[rp->radio_task_id] = RP_STATUS_CAD_POSITIVE;

			// Rx can be performed if activity detected on channel
			if( rp->tasks[rp->radio_task_id].type == RP_TASK_TYPE_CAD_TO_RX )
			{
				rp->radio = TARGET_RADIO;
				rp_hook_callback( rp, rp->radio_task_id );
				return;
			}

			//!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
			SMTC_MODEM_HAL_PANIC_ON_FAILURE( ral_set_standby( TARGET_RAL, RAL_STANDBY_CFG_RC ) == RAL_STATUS_OK );

			break;

		case IRQ_RADIO_TIMEOUT:
			MW_LOG( TS_ON, VLEVEL_M,  " IRQ_RADIO_RX_TX_TIMEOUT\r\n" );

			if(( rp->tasks[rp->radio_task_id].type == RP_TASK_TYPE_TX_LORA ) ||
				( rp->tasks[rp->radio_task_id].type == RP_TASK_TYPE_TX_FSK ) ||
					( rp->tasks[rp->radio_task_id].type == RP_TASK_TYPE_TX_LR_FHSS ))
			{
				rp->status[rp->radio_task_id] = RP_STATUS_TX_TIMEOUT;
			}
			else
			{
				rp->status[rp->radio_task_id] = RP_STATUS_RX_TIMEOUT;
			}

			//!< Update operating mode state to a value lower than \ref MODE_STDBY_XOSC
			SMTC_MODEM_HAL_PANIC_ON_FAILURE( ral_set_standby( TARGET_RAL, RAL_STANDBY_CFG_RC ) == RAL_STATUS_OK );

			break;
		case IRQ_RADIO_PREAMBLE_DETECTED:
			MW_LOG( TS_ON, VLEVEL_H,  " PRE OK\r\n" );
			break;

		case IRQ_RADIO_SYNC_WORD_VALID:
			MW_LOG( TS_ON, VLEVEL_H,  " SYNC OK\r\n" );
			break;

		case IRQ_RADIO_HEADER_VALID:
			MW_LOG( TS_ON, VLEVEL_H,  " HDR OK\r\n" );
			break;

		case IRQ_RADIO_HEADER_ERROR:
			MW_LOG( TS_ON, VLEVEL_M,  " HDR KO\r\n" );
			rp->status[rp->radio_task_id] = RP_STATUS_RX_CRC_ERROR;
			break;

		case IRQ_RADIO_CRC_ERROR:
			MW_LOG( TS_ON, VLEVEL_M,  " IRQ_RADIO_CRC_ERROR\r\n" );
			rp->status[rp->radio_task_id] = RP_STATUS_RX_CRC_ERROR;
			break;
	//#if( RADIO_LR_FHSS_IS_ON == 1 )
		case IRQ_RADIO_LR_FHSS_HOP:
		{
			MW_LOG( TS_ON, VLEVEL_M,  " HOP\r\n" );
			rp->status[rp->radio_task_id] = RP_STATUS_LR_FHSS_HOP;
			SMTC_MODEM_HAL_PANIC_ON_FAILURE(ral_lr_fhss_handle_hop(
					TARGET_RAL_FOR_HOOK_ID, &rp->radio_params[rp->radio_task_id].tx.lr_fhss.ral_lr_fhss_params,
					( ral_lr_fhss_memory_state_t ) rp->radio_params[rp->radio_task_id].lr_fhss_state ) == RAL_STATUS_OK );
			//( void ) wl_lr_fhss_handle_hop( &SubgRf.lr_fhss.lr_fhss_params, &SubgRf.lr_fhss.lr_fhss_state );

			break;
		}
	//#endif /* RADIO_LR_FHSS_IS_ON == 1 */
		default:
			break;
		}
    }
}


void rp_radio_irq_callback( void* obj )
{
    radio_planner_t* rp                        = ( ( radio_planner_t* ) obj );
    rp->radio_irq_flag                         = true;
#ifdef RELAY
    rp->irq_timestamp_100us[rp->radio_task_id] = UTIL_TIMER_GetCurrentTime() * 10000;
#endif // RELAY
    rp->irq_timestamp_ms[rp->radio_task_id]    = SysTimeToMs(SysTimeGet());
    smtc_modem_hal_user_lbm_irq( );
}

static void rp_timer_irq_callback( void* obj )
{
	radio_planner_t* rp = rp_modem_radio_planner_p;
  rp->timer_irq_flag  = true;
  smtc_modem_hal_user_lbm_irq( );
}

static void rp_hook_callback( radio_planner_t* rp, uint8_t id )
{
    if( id >= RP_NB_HOOKS )
    {
        SMTC_MODEM_HAL_PANIC( );
        return;
    }
    if( rp->hook_callbacks[id] == NULL )
    {
        SMTC_MODEM_HAL_PANIC( );
        return;
    }
    rp->hook_callbacks[id]( rp->hooks[id] );
}

/* --- EOF ------------------------------------------------------------------ */
