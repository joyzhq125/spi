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
#include "We_Cmmn.h"    

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_If.h"     

#include "Msig.h"       
#include "Mmem.h"       

#include "Mmsrpl.h"     

#ifdef WE_LOG_MODULE 



const char *pcFsmAyncSigName(WE_UINT32 sigType);
const char *fsmCOHGetSigName(WE_UINT32 sigType);
const char *fsmCOHPostSigName(WE_UINT32 sigType);
const char *fsmCOHPushSigName(WE_UINT32 sigType);
const char *fsmCOHSendSigName(WE_UINT32 sigType);
const char *fsmCOHWAPSigName(WE_UINT32 sigType);
const char *fsmCONTSigName(WE_UINT32 sigType);
const char *fsmCoreConfigSigName(WE_UINT32 sigType);
const char *fsmCoreMainSigName(WE_UINT32 sigType);
const char *fsmFldrMgrSigName(WE_UINT32 sigType);
const char *fsmMMHSigName(WE_UINT32 sigType);
const char *fsmMsrNotifSigName(WE_UINT32 sigType);
const char *fsmMsrReceiveSigName(WE_UINT32 sigType);
const char *fsmMsrSendSigName(WE_UINT32 sigType);
const char *mmsMRgetSigName(WE_UINT32 sigType);
#endif






#define M_SIG_MAX_FREE_LIST_LENGTH   20











static MmsSignal    *m_signal_queue_first;
static MmsSignal    *m_signal_queue_last;



static MmsSignal    *m_signal_free_list;
static int          m_signal_free_list_length;


static MSignalFunction *m_signal_func[M_FSM_MAX_REGS];


#ifdef WE_LOG_MODULE
static void logSignal(const MmsSignal *sig);
static const char *fsmName(MmsStateMachine fsm);
#endif 







void mSignalEmergencyAbort(void)
{
    
} 





void mSignalFlush(MmsStateMachine dst, MmsSignalId wid, long i_param,
    unsigned long u_param1, unsigned long u_param2, const void *p_param)
{
    MmsSignal   *sig = m_signal_queue_first;
    MmsSignal   *prev = NULL;
    MmsSignal   *current = NULL;

    while (sig) 
    {
        if (sig->dst == dst && sig->type == (WE_UINT32)wid && sig->i_param == i_param &&
            sig->u_param1 == u_param1 && sig->u_param2 == u_param2 &&
            sig->p_param == p_param)
        {
            if (prev != NULL)
            {
                prev->next = sig->next;
            }
            else
            {
                m_signal_queue_first = sig->next;
            } 

            current = sig->next;
            mSignalDelete(sig);
            sig = current;
        }
        else
        {
            sig = sig->next;
        } 

        prev = sig;
    } 
} 




void mSignalInit (void)
{
    int i;
    
    m_signal_queue_first = m_signal_queue_last = NULL;
    m_signal_free_list = NULL;
    m_signal_free_list_length = 0;
    
    for (i = 0; i < M_FSM_MAX_REGS; i++)
    {
        m_signal_func[i] = NULL;
    }
}




void mSignalTerminate (void)
{
    MmsSignal   *sig;
    int           i;
    
    
    while (!mSignalQueueIsEmpty()) 
    {
        sig = m_signal_queue_first;
        m_signal_queue_first = sig->next;
        mSignalDelete(sig);
    }
    
    
    while (m_signal_free_list != NULL) 
    {
        sig = m_signal_free_list;
        m_signal_free_list = sig->next;
        M_FREE(sig);
    }
    
    
    for (i = 0; i < M_FSM_MAX_REGS; i++)
    {
        m_signal_func[i] = NULL;
    }
}




void mSignalProcess(void)
{
    MmsSignal           *signal;
    MSignalFunction     *func;
    int                  idx;
    
    signal = m_signal_queue_first;
    if (signal != NULL) 
    {
        m_signal_queue_first = signal->next;
        if (m_signal_queue_first == NULL)
        {
            m_signal_queue_last = NULL;
        }
        idx = signal->dst;
        if ((idx >= 0) && (idx < M_FSM_MAX_REGS) &&
            ((func = m_signal_func[idx]) != NULL))
        {
#ifdef WE_LOG_MODULE
            logSignal(signal);
#endif
            func(signal);
        }
        else
        {
            mSignalDelete(signal);
        }
    }
}





int mSignalQueueIsEmpty (void)
{
    return m_signal_queue_first == NULL;
}




void mSignalDelete(MmsSignal *sig)
{
    if (sig == NULL)
    {
        return;
    }
    
    if (m_signal_free_list_length < M_SIG_MAX_FREE_LIST_LENGTH) 
    {
        sig->next = m_signal_free_list;
        m_signal_free_list = sig;
        m_signal_free_list_length++;
    }
    else
    {
        M_FREE(sig);
    }
}





void mSignalRegisterDst(MmsStateMachine dst, MSignalFunction *f)
{
    if (dst < M_FSM_MAX_REGS)
    {
        m_signal_func[dst] = f;
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "mSignalRegisterDst: FSM dst (%d) out of bounds\n", dst));
    } 
}




void mSignalDeregister(MmsStateMachine dst)
{
    if (dst < M_FSM_MAX_REGS)
    {
        m_signal_func[dst] = NULL;
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "mSignalDeregister: FSM dst (%d) out of bounds\n", dst));
    } 
}




void mSignalSendTo(MmsStateMachine dst, WE_UINT32 signum,
    long i_param, unsigned long u_param1, unsigned long u_param2,
    void *p_param)
{
    MmsSignal *signal = NULL;
    
    if (m_signal_free_list == NULL)
    {
        signal = M_ALLOCTYPE(MmsSignal);
    }
    else 
    {
        signal = m_signal_free_list;
        m_signal_free_list = signal->next;
        --m_signal_free_list_length;
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
        if (m_signal_queue_last != NULL) 
        {
            m_signal_queue_last->next = signal;
            m_signal_queue_last = signal;
        }
        else 
        {
            m_signal_queue_first = m_signal_queue_last = signal;
        } 
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d) Out of memory\n", __FILE__, __LINE__));

        mmsNotifyError(MMS_RESULT_INSUFFICIENT_MEMORY);
    } 
} 




void mSignalPrepend (MmsSignal *sig)
{
    if (sig == NULL)
    {
        return;
    }
    
    sig->next = m_signal_queue_first;
    if (m_signal_queue_last == NULL)
    {
        m_signal_queue_last = sig;
    }
    m_signal_queue_first = sig;
}

#ifdef WE_LOG_MODULE



static const char *fsmSigName(MmsStateMachine fsm, WE_UINT32 sigType)
{
    switch (fsm)
    {
    case M_FSM_ASYNC :
        return pcFsmAyncSigName(sigType);
    case M_FSM_COH_GET :
        return fsmCOHGetSigName(sigType);
    case M_FSM_COH_POST :
        return fsmCOHPostSigName(sigType);
    case M_FSM_COH_PUSH_RECEIVE :
        return fsmCOHPushSigName(sigType);
    case M_FSM_COH_SEND :
        return fsmCOHSendSigName(sigType);
    case M_FSM_COH_WAP :
        return fsmCOHWAPSigName(sigType);
    case M_FSM_CONT :
        return fsmCONTSigName(sigType);
    case M_FSM_CORE_CONFIG :
        return fsmCoreConfigSigName(sigType);
    case M_FSM_CORE_MAIN :
        return fsmCoreMainSigName(sigType);
    case M_FSM_FLDR_MANAGER :
        return fsmFldrMgrSigName(sigType);
    case M_FSM_MMH_FETCH :
        return 0; 
    case M_FSM_MMH_HANDLER :
        return fsmMMHSigName(sigType);
    case M_FSM_MSR_NOTIFY :
        return fsmMsrNotifSigName(sigType);
    case M_FSM_MSR_RECEIVE :
        return fsmMsrReceiveSigName(sigType);
    case M_FSM_MSR_SEND :
        return fsmMsrSendSigName(sigType);
    case M_FSM_MR :
        return mmsMRgetSigName(sigType);
    default :
        return 0;
    } 

} 




static const char *fsmName(MmsStateMachine fsm)
{
    switch (fsm)
    {
    case M_FSM_ASYNC :
        return "Asynch File API FSM";
    case M_FSM_COH_GET :
        return "COH Get";
    case M_FSM_COH_POST :
        return "COH Post";
    case M_FSM_COH_PUSH_RECEIVE :
        return "COH Push Receive";
    case M_FSM_COH_SEND :
        return "COH Send";
    case M_FSM_COH_WAP :
        return "COH WAP";
    case M_FSM_CONT :
        return "Content Router FSM";
    case M_FSM_CORE_CONFIG :
        return "CORE Configuration";
    case M_FSM_CORE_MAIN :
        return "CORE Main";
    case M_FSM_FLDR_MANAGER :
        return "Folder Manager";
    case M_FSM_MMH_FETCH :
        return "MMH Fetch";
    case M_FSM_MMH_HANDLER :
        return "MMH Msg Handler";
    case M_FSM_MSR_NOTIFY :
        return "MSR Notification";
    case M_FSM_MSR_RECEIVE :
        return "MSR Receive";
    case M_FSM_MSR_SEND :
        return "MSR Send";
    case M_FSM_MR:
        return "Message read";
    default :
        return "Unknown";
    } 
} 




static void logSignal(const MmsSignal *sig)
{
    const char *sigType = fsmSigName(sig->dst, sig->type);

    if (sigType == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "MMS Internal SIG to %-20s %6d %7d %8d %8d %9d\n",
            fsmName(sig->dst), sig->type,
            sig->i_param, sig->u_param1, sig->u_param2, sig->p_param));
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "MMS Internal SIG to %-20s %-35s %7d %8d %8d %9d\n",
            fsmName(sig->dst), sigType,
            sig->i_param, sig->u_param1, sig->u_param2, sig->p_param));
    }
} 

#endif 
