/*
 * Copyright (C) Techfaith, 2002-2005.
 * All rights reserved.
 *
 * This software is covered by the license agreement between
 * the end user and Techfaith, and may be 
 * used and copied only in accordance with the terms of the 
 * said agreement.
 *
 * Techfaith assumes no responsibility or 
 * liability for any errors or inaccuracies in this software, 
 * or any consequential, incidental or indirect damage arising
 * out of the use of the software.
 *
 */

/*!\file satimer.c
 */

/* WE */
#include "We_Def.h"
#include "We_Log.h"
#include "We_Lib.h"
#include "We_Core.h"

/* SMA */
#include "Satimer.h"

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/******************************************************************************
 * Static data-structures
 *****************************************************************************/

static SiaTimerCallback timerTable[SIA_TIMER_COUNT - 1] = {0};


/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!\brief Init timer handler
 *****************************************************************************/
void smaTimerInit(void)
{
    memset(timerTable, 0, (sizeof(SiaTimerCallback) * (SIA_TIMER_COUNT - 1)));
}

/*!
 * \brief Cleans up timer handler
 *****************************************************************************/
void smaTimerTerminate(void)
{
}

/*!
 * \brief Registers a timer with its callback function.
 * \param id The timer id, see #SiaTimer
 * \param expiryCallback A function that will be called each timer the timer
 *  expires.
 *****************************************************************************/
void smaRegisterTimer(SiaTimer id, SiaTimerCallback expiryCallback)
{
    if (id) 
    {
        timerTable[id - 1] = expiryCallback;
    }
}

/*!
 * \brief Unregisters a timer.
 * \param id The id of the timer to unregister.
 *****************************************************************************/
void smaUnRegisterTimer(SiaTimer id)
{
    if (id) 
    {
        timerTable[id - 1] = NULL;
    }
}

/*!
 * \brief Starts a timer.
 * \param id The timer id
 * \param duration The time in milliseconds to set the timer to.
 *****************************************************************************/
void smaSetTimer(SiaTimer id, WE_UINT32 duration)
{
    WE_TIMER_SET (WE_MODID_SIA, (int)id, duration);    
}

/*!
 * \brief Stops a timer.
 * \param id The timer id
 * \param duration The time in milliseconds to set the timer to.
 *****************************************************************************/
void smaResetTimer(SiaTimer id)
{
    WE_TIMER_RESET(WE_MODID_SIA, (int)id);    
}

/*!
 * \brief Handles expiry of timer.
 * \param id The timer id
 *****************************************************************************/
void smaHandleTimerExpiry(SiaTimer id)
{
    SiaTimerCallback callback = timerTable[id - 1];
    if (callback) 
    {
        callback();
    }
}
