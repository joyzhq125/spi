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



#include "We_Def.h"
#include "We_Cfg.h"
#include "We_Mem.h"
#include "We_Log.h"


#include "Sis_Mem.h"
#include "Sis_Isig.h"









#define SIS_SIG_MAX_FREE_LIST_LENGTH   10



























static SlsSignal    *Sis_signal_queue_first;
static SlsSignal    *Sis_signal_queue_last;



static SlsSignal   *Sis_signal_free_list;
static int          Sis_signal_free_list_length;



static SlsSignalFunction *Sis_signal_func[SIS_FSM_MAX_REGS];




void slsSignalInit (void)
{
    int i;
    
    Sis_signal_queue_first      = Sis_signal_queue_last = NULL;
    Sis_signal_free_list        = NULL;
    Sis_signal_free_list_length = 0;
    
    for (i = 0; i < SIS_FSM_MAX_REGS; i++)
    {
        Sis_signal_func[i] = NULL;
    } 
} 





void slsSignalTerminate (void)
{
    SlsSignal   *sig;
    int           i;
    
    
    while (!slsSignalQueueIsEmpty()) 
    {
        sig = Sis_signal_queue_first;
        Sis_signal_queue_first = sig->next;
        slsSignalDelete(sig);
    }
    
    
    while (Sis_signal_free_list != NULL) 
    {
        sig = Sis_signal_free_list;
        Sis_signal_free_list = sig->next;
        SIS_FREE(sig);
    }
    
    
    for (i = 0; i < SIS_FSM_MAX_REGS; i++)
    {
        Sis_signal_func[i] = NULL;
    }
}




void slsSignalProcess(void)
{
    SlsSignal           *signal;
    SlsSignalFunction   *func;
    int                  idx;
    
    signal = Sis_signal_queue_first;
    if (signal != NULL) 
    {
        Sis_signal_queue_first = signal->next;
        if (Sis_signal_queue_first == NULL)
        {
            Sis_signal_queue_last = NULL;
        }
        idx = signal->dst;
        if ((idx >= 0) && (idx < SIS_FSM_MAX_REGS) &&
            ((func = Sis_signal_func[idx]) != NULL))
        {
            func(signal);
        }
        else
        {
            slsSignalDelete(signal);
        }
    }
}





int slsSignalQueueIsEmpty (void)
{
    return Sis_signal_queue_first == NULL;
}




void slsSignalDelete(SlsSignal *sig)
{
    if (sig == NULL)
    {
        return;
    }
    
    if (Sis_signal_free_list_length < SIS_SIG_MAX_FREE_LIST_LENGTH) 
    {
        sig->next = Sis_signal_free_list;
        Sis_signal_free_list = sig;
        Sis_signal_free_list_length++;
    }
    else
    {
        SIS_FREE(sig);
    }
}





void slsSignalRegisterDst(SlsStateMachine dst, SlsSignalFunction *f)
{
    if (dst < SIS_FSM_MAX_REGS)
    {
        Sis_signal_func[dst] = f;
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "SlsSignalRegisterDst: FSM dst (%d) out of bounds\n", dst));
    } 
}




void slsSignalDeregister(SlsStateMachine dst)
{
    if (dst < SIS_FSM_MAX_REGS)
    {
        Sis_signal_func[dst] = NULL;
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS,
            "SlsSignalDeregister: FSM dst (%d) out of bounds\n", dst));
    } 
}




SlsSignal *slsSignalSendTo(SlsStateMachine dst, int signum,
    long i_param, unsigned long u_param1, unsigned long u_param2,
    void *p_param)
{
    SlsSignal *signal = NULL;
    
    if (Sis_signal_free_list == NULL)
    {
        signal = SIS_ALLOC(sizeof(SlsSignal));
    }
    else 
    {
        signal = Sis_signal_free_list;
        Sis_signal_free_list = signal->next;
        --Sis_signal_free_list_length;
    } 
    
    if (signal != NULL)
    {
        signal->type = signum;
        signal->dst = dst;
        signal->i_param = i_param;
        signal->u_param1 = u_param1;
        signal->u_param2 = u_param2;
        signal->p_param = p_param;
    
        signal->next = NULL;
        if (Sis_signal_queue_last != NULL) 
        {
            Sis_signal_queue_last->next = signal;
            Sis_signal_queue_last = signal;
        }
        else 
        {
            Sis_signal_queue_first = Sis_signal_queue_last = signal;
        } 
    } 

    return signal;
} 





void slsSignalFlush(SlsStateMachine dst, int id, long i_param,
    unsigned long u_param1, unsigned long u_param2, const void *p_param)
{
    SlsSignal *sig     = Sis_signal_queue_first;
    SlsSignal *prev    = NULL;
    SlsSignal *current = NULL;

    while (sig) 
    {
        if (sig->dst == dst && sig->type == id && sig->i_param == i_param &&
            sig->u_param1 == u_param1 && sig->u_param2 == u_param2 &&
            sig->p_param == p_param)
        {
            if (prev != NULL)
            {
                prev->next = sig->next;
            }
            else
            {
                Sis_signal_queue_first = sig->next;
            } 

            current = sig->next;
            slsSignalDelete(sig);
            sig = current;
        }
        else
        {
            sig = sig->next;
        } 

        prev = sig;
    } 
} 




void slsSignalPrepend (SlsSignal *sig)
{
    if (sig == NULL)
    {
        return;
    }
    
    sig->next = Sis_signal_queue_first;
    if (Sis_signal_queue_last == NULL)
    {
        Sis_signal_queue_last = sig;
    }
    Sis_signal_queue_first = sig;
} 


