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

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    

#include "Msig.h"       
#include "Mmem.h"       
#include "Mcpdu.h"      
#include "Mcsend.h"     
#include "Mcpost.h"     
#include "Fldmgr.h"     
#include "Mutils.h"     


#define MMS_MAX_FSM_SEND_INSTANCES 1





typedef struct
{
    MmsStateMachine requestingFsm;  
    long            fsmInstance;    
    MmsSignalId     returnSig;      
    MmsMsgId        msgId;          
} CohSendInstanceData;




















static CohSendInstanceData *fsmInstance[MMS_MAX_FSM_SEND_INSTANCES];


static void cohSendMain(MmsSignal *sig);
static void deleteInstance(long instance);
static long getInstance(const MmsSignal *sig);
static long selectInstance(void);


#ifdef WE_LOG_MODULE 



const char *fsmCOHSendSigName(WE_UINT32 sigType)
{
    switch (sigType)
    {
    case MMS_SIG_COH_SEND:
        return "MMS_SIG_COH_SEND";
    case MMS_SIG_COH_SEND_POST_RSP:
        return "MMS_SIG_COH_SEND_POST_RSP";
    default:
        return 0;
    }
} 
#endif







void cohSendEmergencyAbort(void)
{
    mSignalDeregister(M_FSM_COH_SEND);
} 




void cohSendInit(void)
{
    int i;
    
    for (i = 0; i < MMS_MAX_FSM_SEND_INSTANCES; ++i)
    {
        fsmInstance[i] = NULL;
    } 

    mSignalRegisterDst(M_FSM_COH_SEND, cohSendMain);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH SEND: initialized\n"));
} 






static void cohSendMain(MmsSignal *sig)
{
    long instance = -1L;
    MmsSigCohPostParam *post;

    switch (sig->type)
    {
    case MMS_SIG_COH_SEND :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH SEND: Received MMS_SIG_COH_SEND, "));

        if (sig->p_param == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "Wrong indata, return!\n"));
            
            
            M_SIGNAL_SENDTO_IU( (MmsStateMachine)sig->u_param1, sig->u_param2, 
                sig->i_param, MMS_RESULT_ERROR); 
            break;
        }

        
        instance = selectInstance();
        
        if (instance == -1L)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "We're BUSY! Return!\n"));

            if (sig->p_param != NULL)
            {
                M_FREE(sig->p_param);
            } 

            
            M_SIGNAL_SENDTO_IU( (MmsStateMachine)sig->u_param1, sig->u_param2, 
                sig->i_param, MMS_RESULT_BUSY); 
        }
        else
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "Sending MMS_SIG_COH_POST\n"));

            
            fsmInstance[instance]->requestingFsm = (MmsStateMachine)sig->u_param1;
            fsmInstance[instance]->returnSig = (MmsSignalId)sig->u_param2;
            fsmInstance[instance]->fsmInstance = sig->i_param;
            fsmInstance[instance]->msgId = ((MmsSigCohSendParam *)sig->p_param)->msgId;

            post = M_ALLOCTYPE(MmsSigCohPostParam);
            if (post) 
            {                               
                post->isResponseRequested = TRUE;
                post->type = MMS_M_SEND_REQ;
                post->data.msgId = ((MmsSigCohSendParam *)sig->p_param)->msgId;
                M_FREE(sig->p_param);
                
                M_SIGNAL_SENDTO_IUUP( 
                    M_FSM_COH_POST,             
                    MMS_SIG_COH_POST,           
                    instance,                   
                    M_FSM_COH_SEND,             
                    MMS_SIG_COH_SEND_POST_RSP,  
                    post);                      
            }
            else
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                        "%s(%d): Allocation failed!\n",
                        __FILE__, __LINE__));
            }
        } 
        break;
    case MMS_SIG_COH_SEND_POST_RSP :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH SEND: Received MMS_SIG_COH_SEND_POST_RSP (%d)\n", 
            sig->u_param1));

        if ((instance = getInstance(sig)) != -1)
        {
            if (sig->u_param1 != MMS_RESULT_OK)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "MMS FSM COH SEND: POST failed %d!\n", sig->u_param1));

                if (sig->p_param != NULL)
                {
                    M_FREE(sig->p_param);
                } 
            }
            else if (sig->p_param == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "MMS FSM COH SEND: No Server Msg WID returned!\n"));
                sig->u_param1 = MMS_RESULT_COMM_SERVER_ERROR;
            }
            else
            {
                if ( !Mms_FldrSetServerMsgId( fsmInstance[instance]->msgId, 
                    (char *)sig->p_param) )
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                        "MMS FSM COH SEND: Couldn't store Server Msg WID (%s)\n", 
                        sig->p_param));
                    sig->u_param1 = MMS_RESULT_ERROR;
                } 

                M_FREE(sig->p_param);
            } 

            

            M_SIGNAL_SENDTO_IU(fsmInstance[instance]->requestingFsm, 
                fsmInstance[instance]->returnSig, 
                fsmInstance[instance]->fsmInstance,
                sig->u_param1);

            deleteInstance(instance);
        } 
        break;
    default:
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH SEND: received unknown signal\n"));
        break;
    } 
    
    mSignalDelete(sig);
} 




void cohSendTerminate(void)
{
    mSignalDeregister(M_FSM_COH_SEND);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH SEND: terminated\n"));
} 






static void deleteInstance(long instance)
{
    if (fsmInstance[instance] != NULL)
    {
        M_FREE(fsmInstance[instance]);
        fsmInstance[instance] = NULL;
    } 
} 







static long getInstance(const MmsSignal *sig)
{
    long instance = -1L;

    instance = sig->i_param;

    if (instance < 0L || instance >= MMS_MAX_FSM_SEND_INSTANCES ||
        fsmInstance[instance] == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "SEND getInstance: instance out of bounds (%d)\n", instance));
        instance = -1L;
    } 

    return instance;
} 







static long selectInstance(void)
{
    long i;
    
    for (i = 0L; i < MMS_MAX_FSM_SEND_INSTANCES; i++) 
    {
        if (fsmInstance[i] == NULL)
        {
            
            fsmInstance[i] = M_ALLOC(sizeof(CohSendInstanceData));
            
            memset( fsmInstance[i], 0, sizeof(CohSendInstanceData));
            fsmInstance[i]->fsmInstance = 0;
            fsmInstance[i]->requestingFsm = M_FSM_MAX_REGS;
            fsmInstance[i]->returnSig = MMS_SIG_COMMON_BASE;
            
            return i;
        } 
    } 

    return -1L;
} 
