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

/*--- Include files ---*/

#include "We_Def.h"
#include "We_Cfg.h"
#include "We_Mem.h"
#include "We_Log.h"
#include "Samem.h"
#include "Saintsig.h"


/*--- Definitions/Declarations ---*/

/*--- Types ---*/

/*--- Constants ---*/

/* Max number of signal structs stored in the free list: */
#define SIA_SIG_MAX_FREE_LIST_LENGTH   10

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Global variables ---*/

/*--- Static variables ---*/



/*! \file maintsig.c
 * \brief  <brief description>
 */

/****************************************
 * Global variables:
 ****************************************/

/*lint -esym(956,m_signal_queue_first,m_signal_queue_last) */
/*lint -esym(956,m_signal_free_list,m_signal_free_list_length,m_signal_func) */
/* Non const, non volatile static or external variable. Not a problem since 
 * we're not running anything multithreaded. */

/* The global queue of signals to be delivered: */
static SiaSignal    *Sia_signal_queue_first;
static SiaSignal    *Sia_signal_queue_last;

/* List of available signal structs. This is used
 * to reduce the number of allocations. */
static SiaSignal   *Sia_signal_free_list;
static int          Sia_signal_free_list_length;

/* The table of known FSM/destinations */

static SiaSignalFunction *Sia_signal_func[SIA_FSM_MAX_REGS];

/*
 * Initialize the signal module.
 */
void smaSignalInit (void)
{
    int i;
    
    Sia_signal_queue_first      = Sia_signal_queue_last = NULL;
    Sia_signal_free_list        = NULL;
    Sia_signal_free_list_length = 0;
    
    for (i = 0; i < SIA_FSM_MAX_REGS; i++)
    {
        Sia_signal_func[i] = NULL;
    } /* for */
} /* smaSignalInit */


/*
 * Terminate the signal module.
 */
void smaSignalTerminate (void)
{
    SiaSignal   *sig;
    int           i;
    
    /* Empty the signal queue */
    while (!smaSignalQueueIsEmpty()) 
    {
        sig = Sia_signal_queue_first;
        Sia_signal_queue_first = sig->next;
        smaSignalDelete(sig);
    }
    
    /* Empty the list of free signal structs */
    while (Sia_signal_free_list != NULL) 
    {
        sig = Sia_signal_free_list;
        Sia_signal_free_list = sig->next;
        SIA_FREE(sig);
    }
    
    /* Reset the function pointers */
    for (i = 0; i < SIA_FSM_MAX_REGS; i++)
    {
        Sia_signal_func[i] = NULL;
    }
}

/*
 * Process the first signal in the signal queue.
 */
void smaSignalProcess(void)
{
    SiaSignal           *signal;
    SiaSignalFunction   *func;
    int                  idx;
    
    signal = Sia_signal_queue_first;
    if (signal != NULL) 
    {
        Sia_signal_queue_first = signal->next;
        if (Sia_signal_queue_first == NULL)
        {
            Sia_signal_queue_last = NULL;
        }
        idx = signal->dst;
        if ((idx >= 0) && (idx < SIA_FSM_MAX_REGS) &&
            ((func = Sia_signal_func[idx]) != NULL))
        {
            func(signal);
        }
        else
        {
            smaSignalDelete(signal);
        }
    }
}

/*
 * Return TRUE if the global signal queue is empty,
 * FALSE otherwise.
 */
int smaSignalQueueIsEmpty (void)
{
    return Sia_signal_queue_first == NULL;
}

/*
 * Delete a signal and its parameters.
 */
void smaSignalDelete(SiaSignal *sig)
{
    if (sig == NULL)
    {
        return;
    }
    
    if (Sia_signal_free_list_length < SIA_SIG_MAX_FREE_LIST_LENGTH) 
    {
        sig->next = Sia_signal_free_list;
        Sia_signal_free_list = sig;
        Sia_signal_free_list_length++;
    }
    else
    {
        SIA_FREE(sig);
    }
}

/*
 * Register the function "f" to be called when a signal
 * is delivered to destination "dst".
 */
void smaSignalRegisterDst(SiaStateMachine dst, SiaSignalFunction *f)
{
    if (dst < SIA_FSM_MAX_REGS)
    {
        Sia_signal_func[dst] = f;
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "SiaSignalRegisterDst: FSM dst (%d) out of bounds\n", dst));
    } /* if */
}

/*
 * Remove the registrated function for destination "dst".
 */
void smaSignalDeregister(SiaStateMachine dst)
{
    if (dst < SIA_FSM_MAX_REGS)
    {
        Sia_signal_func[dst] = NULL;
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
            "SiaSignalDeregister: FSM dst (%d) out of bounds\n", dst));
    } /* if */
}

/*
 * Add a new signal to the end of the global queue.
 */
SiaSignal *smaSignalSendTo(SiaStateMachine dst, int signum,
    long i_param, unsigned long u_param1, unsigned long u_param2,
    void *p_param)
{
    SiaSignal *signal = NULL;
    
    if (Sia_signal_free_list == NULL)
    {
        signal = SIA_ALLOC(sizeof(SiaSignal));
    }
    else 
    {
        signal = Sia_signal_free_list;
        Sia_signal_free_list = signal->next;
        --Sia_signal_free_list_length;
    } /* if */
    
    if (signal != NULL)
    {
        signal->type = signum;
        signal->dst = dst;
        signal->i_param = i_param;
        signal->u_param1 = u_param1;
        signal->u_param2 = u_param2;
        signal->p_param = p_param;
    
        signal->next = NULL;
        if (Sia_signal_queue_last != NULL) 
        {
            Sia_signal_queue_last->next = signal;
            Sia_signal_queue_last = signal;
        }
        else 
        {
            Sia_signal_queue_first = Sia_signal_queue_last = signal;
        } /* if */
    } /* if */

    return signal;
} /* mSignalSendTo */

/*
 * Remove all signals with the specified id and data for a state machine.
 * Can be used to remove expired timers from the queue when the timer is removed.
 */
void smaSignalFlush(SiaStateMachine dst, int id, long i_param,
    unsigned long u_param1, unsigned long u_param2, const void *p_param)
{
    SiaSignal *sig     = Sia_signal_queue_first;
    SiaSignal *prev    = NULL;
    SiaSignal *current = NULL;

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
                Sia_signal_queue_first = sig->next;
            } /* if */

            current = sig->next;
            smaSignalDelete(sig);
            sig = current;
        }
        else
        {
            sig = sig->next;
        } /* if */

        prev = sig;
    } /* while */
} /* smaSignalFlush */

/*
 * Add an old signal to the beginning of the queue.
 */
void smaSignalPrepend (SiaSignal *sig)
{
    if (sig == NULL)
    {
        return;
    }
    
    sig->next = Sia_signal_queue_first;
    if (Sia_signal_queue_last == NULL)
    {
        Sia_signal_queue_last = sig;
    }
    Sia_signal_queue_first = sig;
} /* smaSignalPrepend */


