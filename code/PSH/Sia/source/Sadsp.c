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
#include "We_Mem.h"

/* SIS */
#include "Sis_cfg.h"
#include "Sis_if.h"
#include "Sis_def.h"

/* SMA */
#include "Samem.h"
#include "Satypes.h"
#include "Satimer.h"
#include "Saintsig.h"
#include "Sadsp.h"
#include "Saintsig.h"

/******************************************************************************
 * Data-structures
 *****************************************************************************/

/******************************************************************************
 * Static data-structures
 *****************************************************************************/

static SiaTimerAction *actionList;
static SiaTimerAction *currentAction;
static SiaObjectActionCallback actionCallback;
static WE_UINT32 slideDuration;
static SiaStateMachine returnFsm;
static int returnSig;


/******************************************************************************
 * Function implementation
 *****************************************************************************/

/*!\brief Init timer handler
 *****************************************************************************/
void smaDispatchInit(void)
{
    actionList = NULL;
    currentAction = NULL;
    actionCallback = NULL;
    slideDuration = 0;
    returnFsm = (SiaStateMachine)0;
    returnSig = 0;
}

/*!
 * \brief Cleans up timer handler
 *****************************************************************************/
void smaDispatchTerminate(void)
{
    smaDeleteDispatcher();
}


/*!
 * \brief Adds a media object to the list of pointers.
 * \param objs The list to add the object to.
 * \param obj The object to add
 *****************************************************************************/
static void addMo(SiaDispatchList **objs, WE_UINT32 objId)
{
    SiaDispatchList *tmp;
    if (!objs) 
    {
        return;
    }
    tmp = (SiaDispatchList*)SIA_ALLOC(sizeof(SiaDispatchList));
    tmp->objId = objId;
    tmp->next = *objs;
    (*objs) = tmp;
}

/*!
 * \brief Inserts the start or stop of a media object. The function will expand
 * the action list as necessary.
 * \param list The action list
 * \param time The time of the event.
 * \param obj The object to add the action for
 * \param isBegin TRUE if the object #obj should be shown, otherwise FALSE
 *****************************************************************************/
static void insertObj(SiaTimerAction **list, WE_UINT32 time, 
    WE_UINT32 objId, WE_BOOL isBegin)
{
    SiaTimerAction *l = *list;
    SiaTimerAction *tmp = NULL;
    SiaTimerAction *prev = NULL;
    /* Find insert position */
    while (l) 
    {
        if (time > l->time) 
        {
            prev = l;
            l = l->next;
        }
        else if (time == l->time) 
        {
            /* Already allocated, use it */
            tmp = l;
            break;
        }
        else
        {
            /* Insert before item */
            /* Allocate new action */
            tmp = (SiaTimerAction*)SIA_ALLOC(sizeof(SiaTimerAction));
            memset(tmp, 0, sizeof(SiaTimerAction));
            tmp->next = l;
            if (prev) 
            {
                prev->next = tmp;
            }
            else
            {
                *list = tmp;
            }
            break;
        }
    }
    if (!*list) /* The list is empty */
    {
        /* Allocate new action */
        tmp = (SiaTimerAction*)SIA_ALLOC(sizeof(SiaTimerAction));
        memset(tmp, 0, sizeof(SiaTimerAction));
        *list = tmp;
        tmp->next = NULL;
    }
    if (!tmp && prev) /* The insert position is last */
    {
        /* Insert last */
        /* Allocate new action */
        tmp = (SiaTimerAction*)SIA_ALLOC(sizeof(SiaTimerAction));
        memset(tmp, 0, sizeof(SiaTimerAction));
        prev->next = tmp;
        tmp->next = NULL;
    }
    /* Add the data to the new element */
    if (tmp) 
    {
        tmp->time = time;
        if (isBegin) 
        {
            addMo(&tmp->beginMos, objId);
        }
        else
        {
            addMo(&tmp->endMos, objId);
        }
    }
}


/*!
 * \brief 
 *****************************************************************************/
static void actionChainCallback(void)
{
    SiaDispatchList *obj;
    SiaTimerAction *action;
    WE_UINT32 dur = 0;

    action = currentAction;
    if (action && action->time < slideDuration) 
    {
        /* Loop over all objects to stop first */
        obj = action->endMos;
        while (obj) 
        {
            actionCallback(obj->objId , SIA_MO_STOP);
            obj = obj->next;
        }
        /* Then loop over all objects to start */
        obj = action->beginMos;
        while (obj) 
        {
            actionCallback(obj->objId, SIA_MO_START);
            obj = obj->next;
        }
        /* Calculate next time */
        if (action->next) 
        {
            dur = action->next->time - action->time;
        }
        else
        {
            if (action->time < slideDuration) 
            {
                /* There was some blank displaying in the end */
                dur = slideDuration - action->time;
            }
            else
            {
                /* This was the last action */
                dur = 0;
            }
        }
        /* Skip to next action */
        currentAction = currentAction->next;
    }
    else
    {
        dur = 0;
    }
    
    /* Start timer again */
    if (dur > 0) 
    {
        smaSetTimer(SIA_TIMER_CHAIN_TIMER_ID, dur);
    }
    else
    {
        /* send signal back */;
        /* set to first item again */
        currentAction = actionList;
        (void)SIA_SIGNAL_SENDTO(returnFsm, returnSig);
    } /* if */
}

/*!
 * \brief 
 *****************************************************************************/
static void freeDispatchList(SiaDispatchList **list)
{
    SiaDispatchList *obj;
    if (list) 
    {
        while (*list) 
        {
            obj = *list;
            *list = (*list)->next;
            SIA_FREE(obj);
        }
    }
}

/*!
 * \brief Frees the timer chain
 *****************************************************************************/
void smaStopDispatcher(void)
{
    currentAction = actionList;
    smaResetTimer(SIA_TIMER_CHAIN_TIMER_ID);
}

/*!
 * \brief Execute a timer chain i.e. plays a message from the given slide.
 * \param slideNo The slide to start playing from
 *****************************************************************************/
void smaStartDispatcher(WE_BOOL doRun)
{
    actionChainCallback();
    if (doRun)
    {
        smaRegisterTimer(SIA_TIMER_CHAIN_TIMER_ID, actionChainCallback);
    } /* if */
}


/*!
 * \brief Frees the timer chain
 *****************************************************************************/
void smaDeleteDispatcher(void)
{
    SiaTimerAction *tmp;

    if (actionList) 
    {
        while (actionList) 
        {
            tmp = actionList;
            freeDispatchList(&tmp->beginMos);
            freeDispatchList(&tmp->endMos);
            actionList = actionList->next;
            SIA_FREE(tmp);
        }
        SIA_FREE(actionList);
        actionList = NULL;
    }
}


void smaAddDispatchItem(WE_UINT32 objId, WE_UINT32 beginTime, 
    WE_UINT32 endTime)
{
    if (endTime == 0)
    {
        /* no timeing for this object, set to slide duration */
        if (beginTime <= slideDuration)
        {
            endTime = slideDuration - beginTime;
        } 
        else
        {
            /* the object will never be shown */
            endTime = 0;
        }/* if */
    } /* if */
    
    insertObj(&actionList, beginTime, objId, TRUE);
    insertObj(&actionList, endTime, objId, FALSE);
} /* smaAddDispatchItem */

WE_BOOL smaCreateDispatcher(SiaObjectActionCallback callback, 
     WE_UINT32 slideDur, SiaStateMachine retFsm, int retSig)
{
    if (actionList) 
    {
        /* Instance busy */
        return FALSE;
    }
    returnFsm = retFsm;
    returnSig = retSig;
    slideDuration = slideDur;

    actionCallback = callback;
    actionList = SIA_CALLOC(sizeof(SiaTimerAction));
    currentAction = actionList;

    return TRUE;
} /* smaCreateDispatcher */

