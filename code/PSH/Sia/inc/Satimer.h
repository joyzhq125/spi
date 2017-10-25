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

/*! \file Satimer.h
 *  \brief Handle timing of media objects during playback.
 */

#ifndef SATIMER_H
#define SATIMER_H


/******************************************************************************
 * Types
 *****************************************************************************/

/*\enum Timers used by SMA */
typedef enum
{
    SIA_DIALOG_TIMER_ID = 1,
    SIA_TIMER_CHAIN_TIMER_ID,
    SIA_TIMER_COUNT /* Counter to keep track of timers */
} SiaTimer;

/******************************************************************************
 * Constants
 *****************************************************************************/

/******************************************************************************
 * Data-structures
 *****************************************************************************/
typedef void (*SiaTimerCallback)(void);

/******************************************************************************
 * Prototypes
 *****************************************************************************/
void smaTimerInit(void);
void smaTimerTerminate(void);

void smaRegisterTimer(SiaTimer id, SiaTimerCallback expiryCallback);
void smaSetTimer(SiaTimer id, WE_UINT32 duration);
void smaResetTimer(SiaTimer id);
void smaHandleTimerExpiry(SiaTimer id);

#endif /* SATIMER_H */
