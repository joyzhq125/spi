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

/*! \file maintsig.c
 *  \brief The internal signal queue.
 */

/* WE */
#include "We_Def.h"
#include "We_Cfg.h"
#include "We_Mem.h"
#include "We_Log.h"

/* MSA */
#include "Msa_Intsig.h"
#include "Msa_Mem.h"


/****************************************
 * Constants:
 ****************************************/

/*! Max number of signal structs stored in the free list: */
#define MSA_SIG_MAX_FREE_LIST_LENGTH   10

/****************************************
 * Global variables:
 ****************************************/

/*lint -esym(956,m_signal_queue_first,m_signal_queue_last) */
/*lint -esym(956,m_signal_free_list,m_signal_free_list_length,m_signal_func) */
/* Non const, non volatile static or external variable. Not a problem since 
 * we're not running anything multithreaded. */

/* The global queue of signals to be delivered: */
static MsaSignal    *msa_signal_queue_first;
static MsaSignal    *msa_signal_queue_last;

/* List of available signal structs. This is used
 * to reduce the number of allocations. */
static MsaSignal   *msa_signal_free_list;
static int          msa_signal_free_list_length;

/*! The table of known FSM/destinations */
static MsaSignalFunction *msa_signal_func[MSA_FSM_MAX_REGS];

/* Logging of signals to file */
#ifdef WE_LOG_MODULE
static void logSignal(const MsaSignal *sig);
static const char *fsmName(MsaStateMachine fsm);
#endif /* WE_LOG_MODULE */

/*!
 *  \brief Remove all signals with the specified wid and data 
 *  for a state machine. Can be used to remove expired timers from the 
 *  queue when the timer is removed.
 *  \param dst      Current state machine
 *  \param wid       The signal WID
 *  \param i_param  Signal parameter
 *  \param u_param1 Signal parameter
 *  \param u_param2 Signal parameter
 *  \param p_param  Signal parameter
 */
void msaSignalFlush(MsaStateMachine dst, int wid, long i_param,
    unsigned long u_param1, unsigned long u_param2, const void *p_param)
{
    MsaSignal *sig     = msa_signal_queue_first;
    MsaSignal *prev    = NULL;
    MsaSignal *current = NULL;

    while (sig) 
    {
        if (sig->dst == dst && sig->type == wid && sig->i_param == i_param &&
            sig->u_param1 == u_param1 && sig->u_param2 == u_param2 &&
            sig->p_param == p_param)
        {
            if (prev != NULL)
            {
                prev->next = sig->next;
            }
            else
            {
                msa_signal_queue_first = sig->next;
            } /* if */

            current = sig->next;
            msaSignalDelete(sig);
            sig = current;
        }
        else
        {
            sig = sig->next;
        } /* if */

        prev = sig;
    } /* while */
} /* mSignalFlush */

/*!
 * \brief Initialize the signal module.
 */
void msaSignalInit (void)
{
    int i;
    
    msa_signal_queue_first      = msa_signal_queue_last = NULL;
    msa_signal_free_list        = NULL;
    msa_signal_free_list_length = 0;
    
    for (i = 0; i < MSA_FSM_MAX_REGS; i++)
    {
        msa_signal_func[i] = NULL;
    }
}

/*!
 * \brief Terminate the signal module.
 */
void msaSignalTerminate (void)
{
   MsaSignal   *sig;
    int           i;
    
    /* Empty the signal queue */
    while (!msaSignalQueueIsEmpty()) 
    {
        sig = msa_signal_queue_first;
        msa_signal_queue_first = sig->next;
        msaSignalDelete(sig);
    }
    
    /* Empty the list of free signal structs */
    while (msa_signal_free_list != NULL) 
    {
        sig = msa_signal_free_list;
        msa_signal_free_list = sig->next;
        MSA_FREE(sig);
    }
    
    /* Reset the function pointers */
    for (i = 0; i < MSA_FSM_MAX_REGS; i++)
    {
        msa_signal_func[i] = NULL;
    }
}

/*!
 * \brief Process the first signal in the signal queue.
 */
void msaSignalProcess(void)
{
    MsaSignal           *signal;
    MsaSignalFunction   *func;
    int                  idx;
    
    signal = msa_signal_queue_first;
    if (signal != NULL) 
    {
        msa_signal_queue_first = signal->next;
        if (msa_signal_queue_first == NULL)
        {
            msa_signal_queue_last = NULL;
        }
        idx = signal->dst;
        if ((idx >= 0) && (idx < MSA_FSM_MAX_REGS) &&
            ((func = msa_signal_func[idx]) != NULL))
        {
#ifdef WE_LOG_MODULE
            logSignal(signal);
#endif
            func(signal);
        }
        else
        {
            msaSignalDelete(signal);
        }
    }
}

/*!
 * \return TRUE if the global signal queue is empty, otherwise FALSE.
 */
int msaSignalQueueIsEmpty (void)
{
    return msa_signal_queue_first == NULL;
}

/*!
 *  \brief Delete a signal and its parameters.
 *  \param sig The signal to delete
 */
void msaSignalDelete(MsaSignal *sig)
{
    if (sig == NULL)
    {
        return;
    }
    
    if (msa_signal_free_list_length < MSA_SIG_MAX_FREE_LIST_LENGTH) 
    {
        sig->next = msa_signal_free_list;
        msa_signal_free_list = sig;
        msa_signal_free_list_length++;
    }
    else
    {
        /*lint -e{774} */
        MSA_FREE(sig);
    }
}

/*!
 *  \brief Register the function "f" to be called when a signal
 *  is delivered to destination "dst".
 *  \param dst The destination FSM
 *  \param f The function to register
 */
void msaSignalRegisterDst(MsaStateMachine dst, MsaSignalFunction *f)
{
    if ((dst < MSA_FSM_MAX_REGS) && (dst > MSA_NOTUSED_FSM))
    {
        msa_signal_func[dst] = f;
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
            "msaSignalRegisterDst: FSM dst (%d) out of bounds\n", dst));
    } /* if */
}

/*!
 *  \brief Remove the registered function for destination "dst".
 *  \param dst The FSM to deregister
 */
void msaSignalDeregister(MsaStateMachine dst)
{
    if ((dst < MSA_FSM_MAX_REGS) && (dst > MSA_NOTUSED_FSM))
    {
        msa_signal_func[dst] = NULL;
    }
    else
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA,
            "msaSignalDeregister: FSM dst (%d) out of bounds\n", dst));
    } /* if */
}

/*!
 *  \brief Add a new signal to the end of the global queue.
 *  \param dst      Current state machine
 *  \param signum   The signal WID
 *  \param i_param  Signal parameter
 *  \param u_param1 Signal parameter
 *  \param u_param2 Signal parameter
 *  \param p_param  Signal parameter
 *  \return The new signal. Could be NULL on erroneous destination FSM. 
 */
MsaSignal *msaSignalSendTo(MsaStateMachine dst, int signum,
    long i_param, unsigned long u_param1, unsigned long u_param2,
    void *p_param)
{
    MsaSignal *signal = NULL;

    if ((dst <= MSA_NOTUSED_FSM) || (dst >= MSA_FSM_MAX_REGS))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_MSA,
            "%s(%d): Broken signal: non-existant destination FSM!\n",
        __FILE__, __LINE__));
        return NULL;
    } /* if */
    
    if (msa_signal_free_list == NULL)
    {
        signal = MSA_ALLOC(sizeof(MsaSignal));
    }
    else 
    {
        signal = msa_signal_free_list;
        msa_signal_free_list = signal->next;
        --msa_signal_free_list_length;
    } /* if */
    
    signal->type = signum;
    signal->dst = dst;
    signal->i_param = i_param;
    signal->u_param1 = u_param1;
    signal->u_param2 = u_param2;
    signal->p_param = p_param;
    
    signal->next = NULL;
    if (msa_signal_queue_last != NULL) 
    {
        msa_signal_queue_last->next = signal;
        msa_signal_queue_last = signal;
    }
    else 
    {
        msa_signal_queue_first = msa_signal_queue_last = signal;
    } /* if */

    return signal;
} /* mSignalSendTo */

/*!
 *  \brief Add an old signal to the beginning of the queue.
 *  \param sig The signal to put at the front of the queue.
 */
void msaSignalPrepend (MsaSignal *sig)
{
    if (sig == NULL)
    {
        return;
    }
    
    sig->next = msa_signal_queue_first;
    if (msa_signal_queue_last == NULL)
    {
        msa_signal_queue_last = sig;
    }
    msa_signal_queue_first = sig;
}

#ifdef WE_LOG_MODULE

/*
 * Translate FSM enum to string
 */
static const char *fsmName(MsaStateMachine fsm)
{
    switch (fsm)
    {
    case MSA_CONFIG_FSM:
        return "Configuration:";
    case MSA_MV_FSM:
        return "Message viewer:";
    case MSA_SE_FSM:
        return "SMIL editor:";
    case MSA_ME_FSM :
        return "Message editor:";
    case MSA_ASYNC_FSM:
        return "Asyncronous file operations:";
    case MSA_MCR_FSM :
        return "Message creation:";
    case MSA_MR_FSM :
        return "Message reader:";
    case MSA_CRH_FSM :
        return "Content router handler:";
    case MSA_COMM_FSM :
        return "Communication handler:";
    case MSA_CORE_FSM :
        return "Core";
    case MSA_PBH_FSM :
        return "Phone book handler:";
    case MSA_CTH_FSM:
        return "Cross triggering handler:";
    case MSA_PH_FSM:
        return "Packet handler:";
    case MSA_MMV_FSM:
        return "Multipart mixed viewer:";
    case MSA_MME_FSM:
        return "Multipart mixed editor:";
    case MSA_MOB_FSM:
        return "Media object browser:";
    case MSA_DEL_FSM:
        return "Message deletion:";
    case MSA_PIPE_FSM:
        return "Pipe-handler:";
    case MSA_FSM_MAX_REGS:
    default :
        return "Unknown";
    } /* switch */
} /* fsmName */

/*
 * Logging of signals to file
 */
static void logSignal(const MsaSignal *sig)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA,
        "MSA Internal SIG to %-20s %6d %7d %8d %8d %9d\n",
        fsmName(sig->dst), sig->type,
        sig->i_param, sig->u_param1, sig->u_param2, sig->p_param));
} /* logSignal */

#endif /* WE_LOG_MODULE */
