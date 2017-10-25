/*
 * Copyright (C) Techfaith 2002-2005.
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






#include "We_Lib.h"    
#include "We_Cfg.h"    
#include "We_Def.h"     
#include "We_Log.h"     
#include "We_Mem.h"    
#include "We_Core.h"   

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Msig.h"       
#include "Mtimer.h"     
#include "Mmem.h"       




typedef struct MmsTimerStruct
{
    struct MmsTimerStruct   *next;
    struct MmsTimerStruct   *prev;
    MmsStateMachine         dst;
    long                    param;
    WE_UINT32              wid;
} MmsTimer;

typedef struct 
{
    MmsTimer *first;
    MmsTimer *last;
} MmsAllTimers;

 

 

 

 

 

 



static MmsAllTimers myTimers;
static WE_UINT32 nextFreeTimerId;

 
static void removeTimer(MmsTimer *t);









void mTimerEmergencyAbort(void)
{
    

    mTimerTerminate();
} 





void mTimerInit(void)
{
    myTimers.first = NULL;
    myTimers.last = NULL;
    nextFreeTimerId = 0;
    
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, "MTIMER: initialized\n"));
} 





void mTimerTerminate(void)
{
    MmsTimer *t = myTimers.first;
        
    while (t) 
    {
        myTimers.first = t->next;
        WE_TIMER_RESET(WE_MODID_MMS, (int) t->wid);
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MTIMER: cancelling timer %d\n", t->wid));

        M_FREE(t);
        t = myTimers.first;
    } 

    myTimers.first = NULL;
    myTimers.last = NULL;

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, "MTIMER: terminated\n"));
} 






void mTimerExpired(WE_UINT32 timerId)
{
    MmsTimer *t  = myTimers.first;
    
    if (t == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Expired timer not found %d\n", __FILE__, __LINE__, timerId));
        return;
    } 

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "**** MMS timer %d expired. Dst=%d Param=%d\n", 
        timerId, t->dst, t->param));

    
    while (t != NULL && t->wid != timerId) 
    {
        t = t->next;
    } 
    
    
    if (t != NULL) 
    {
        M_SIGNAL_SENDTO_I( t->dst, (int)MMS_SIG_COMMON_TIMER_EXPIRED, t->param);
        
        removeTimer(t);
    } 
} 





void mTimerSet(MmsStateMachine dst, WE_UINT32 dsecs, long param)
{
    MmsTimer *t;
    MmsTimer *newTimer;

    newTimer            = (MmsTimer *)M_CALLOC(sizeof(MmsTimer));

    if (newTimer) 
    {
    
        newTimer->dst       = dst;
        newTimer->param     = param;
        newTimer->wid        = ++nextFreeTimerId;
        newTimer->next      = NULL;
        newTimer->prev      = NULL;
        
        
        t = myTimers.last;
        myTimers.last = newTimer;
        if (t != NULL)
        {
            t->next = newTimer;
            newTimer->prev = t;
        } 
        
        if (myTimers.first == NULL)
        {
            myTimers.first = newTimer;
        } 
        
        WE_TIMER_SET( WE_MODID_MMS, (int) newTimer->wid, dsecs*100);

    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Failed to allocate timer!\n", __FILE__, __LINE__));
        
    }
} 





void mTimerReset(MmsStateMachine dst, long param)
{
    MmsTimer *t = myTimers.first;
    
    
    while (t && (t->dst != dst || t->param != param))	
    {
        t = t->next;
    } 

    if (t == NULL)
    {
        return;
    } 

    WE_TIMER_RESET( WE_MODID_MMS, (int) t->wid);

    removeTimer(t);

    

    mSignalFlush( dst, MMS_SIG_COMMON_TIMER_EXPIRED, param, 0, 0, NULL);
} 





void mTimerRestart(MmsStateMachine dst, WE_UINT32 dsecs, long param)
{
    MmsTimer *t = myTimers.first;
    
    
    while (t && (t->dst != dst || t->param != param))	
    {
        t = t->next;
    } 

    if (t == NULL)
    {
        return;
    } 

    WE_TIMER_SET( WE_MODID_MMS, (int) t->wid, dsecs*100);
    
    

    mSignalFlush( dst, MMS_SIG_COMMON_TIMER_EXPIRED, param, 0, 0, NULL);
} 







static void removeTimer(MmsTimer *t)
{
    if (t == NULL)
    {
        return;
    } 

        
    if (myTimers.first == t) 
    {
        myTimers.first = t->next;
    } 

        
    if (myTimers.last == t) 
    {
        myTimers.last = t->prev;
    } 

    if (t->prev != NULL)
    {
        t->prev->next = t->next;
    } 

    if (t->next != NULL)
    {
        t->next->prev = t->prev;
    } 
    
    M_FREE(t);
} 
