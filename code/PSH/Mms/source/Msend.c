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

#include "Mmsrpl.h"     
#include "Msig.h"       
#include "Mmem.h"       
#include "Mcpdu.h"      
#include "Msend.h"      
#include "Mcsend.h"     
#include "Mcpost.h"     
#include "Fldmgr.h"      

#include "Mhandler.h"    
#include "Mreport.h"     




typedef struct {
    WE_UINT8   orderer;            
    WE_BOOL    onStateSending;     
    WE_BOOL    isSendingReadReport;
    WE_BOOL    sendCancelled;      
    MmsMsgId    msgIdToSend;         
    MmsMsgId    readOrigMsgId;       
} MsrState;
















static MsrState g_msrState;


static void msrSendMain(MmsSignal *sig);
static void sendOneMsg(MmsMsgId msgId, unsigned char *buffer, WE_UINT32 len, 
    MmsMessageType type);
static void moveMsgToFolder(WE_UINT32 filedId, WE_UINT32 folderType);


#ifdef WE_LOG_MODULE 



const char *fsmMsrSendSigName(WE_UINT32 sigType)
{
    switch (sigType)
    {
    case MMS_SIG_MSR_SEND_CANCEL:
        return "MMS_SIG_MSR_SEND_CANCEL";
    case MMS_SIG_MSR_SEND_MSG:
        return "MMS_SIG_MSR_SEND_MSG";
    case MMS_SIG_MSR_SEND_RSP:
        return "MMS_SIG_MSR_SEND_RSP";
    case MMS_SIG_MSR_CREATE_RR:
        return "MMS_SIG_MSR_CREATE_RR";
    case MMS_SIG_MSR_CREATE_RR_RSP:
        return "MMS_SIG_MSR_CREATE_RR_RSP";
    case MMS_SIG_MSR_SEND_RR_RSP:
        return "MMS_SIG_MSR_SEND_RR_RSP";
    case MMS_SIG_MSR_SEND_FORWARD:
        return "MMS_SIG_MSR_SEND_FORWARD";
    case MMS_SIG_MSR_SEND_PDU:
        return "MMS_SIG_MSR_SEND_PDU";
    case MMS_SIG_MSR_SEND_FORWARD_RSP:
        return "MMS_SIG_MSR_SEND_FORWARD_RSP";
    default:
        return 0;
    }
} 
#endif







static void msrCleanUp(MsrState *msrState)
{
    
    msrState->onStateSending = FALSE;
    msrState->isSendingReadReport = FALSE;
    msrState->sendCancelled = FALSE;
    msrState->orderer = 0;
    msrState->msgIdToSend = 0;
    msrState->readOrigMsgId = 0;
}







void msrSendEmergencyAbort(void)
{
    mSignalDeregister(M_FSM_MSR_SEND);
} 




void msrSendInit(void)
{
    mSignalRegisterDst(M_FSM_MSR_SEND, msrSendMain);
    
    msrCleanUp(&g_msrState);
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM MSR SEND: initialized\n"));
} 




void msrSendTerminate(void)
{
    mSignalDeregister(M_FSM_MSR_SEND);
    
    
    msrCleanUp(&g_msrState);
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM MSR SEND: terminated\n"));
} 





static void msrSigCreateReadReport(WE_UINT8 orderer, MmsMsgId msgId, MmsReadStatus status)
{
    MmsReadReportInfo *readReportInfo = NULL;
    MsrState *state = &g_msrState;
    
    
    if (state->onStateSending == TRUE) 
    {
        replySendReadReportResponse(orderer, MMS_RESULT_BUSY, 0, msgId);
    }
    else if (msgId > 0) 
    {    
        state->orderer = orderer;
        state->onStateSending = TRUE;
        state->isSendingReadReport = TRUE;
        state->readOrigMsgId = msgId;
        state->msgIdToSend = 0;
        
        
        readReportInfo = M_CALLOC(sizeof(MmsReadReportInfo)); 
        readReportInfo->msgId = state->readOrigMsgId;
        readReportInfo->readStatus = status;
        
        
        M_SIGNAL_SENDTO_IUUP(M_FSM_MMH_HANDLER, (int)M_FSM_MMH_CREATE_RR,
            M_FSM_MSR_SEND, MMS_SIG_MSR_CREATE_RR_RSP, MMS_SIG_MSR_SEND_PDU, 
            readReportInfo);
    }
    else 
    {
        replySendReadReportResponse(orderer, MMS_RESULT_MESSAGE_ID_NOT_FOUND, 0, msgId);
    } 
} 

static void msrSigSendCancel(MmsMsgId msgId)
{
    MsrState *state = &g_msrState;
    
    

    if (state->onStateSending &&
        (msgId == state->msgIdToSend || msgId == 0))
    {
        cohPostCancel(state->msgIdToSend);
        state->sendCancelled = TRUE;
    } 
} 

static void msrSigSendMsg(WE_UINT8 orderer, MmsMsgId msgId)
{
    MsrState *state = &g_msrState;

    
    if (state->onStateSending == TRUE) 
    {
        if (state->isSendingReadReport)
        {
            replySendReadReportResponse(orderer, MMS_RESULT_BUSY, msgId, state->readOrigMsgId);
        }
        else
        {
            replySendMessageResponse(orderer, MMS_RESULT_BUSY);
        } 
    }
    else if (checkMsgId(msgId)) 
    {    
        
        if ( fldrGetFileSuffix(msgId) == MMS_SUFFIX_SEND_REQ)
        {
            state->sendCancelled = FALSE;
            state->onStateSending = TRUE;                    
            state->msgIdToSend = msgId;
            state->orderer = orderer;
            
            
            moveMsgToFolder(state->msgIdToSend, MMS_OUTBOX);        
            sendOneMsg(state->msgIdToSend, NULL, 0, MMS_M_SEND_REQ);
        }
        else
        {
            
            if (state->isSendingReadReport)
            {
                replySendReadReportResponse(orderer, MMS_RESULT_WRONG_FILE_TYPE, msgId, state->readOrigMsgId);
            }
            else
            {
                replySendMessageResponse(orderer, MMS_RESULT_WRONG_FILE_TYPE);                
            } 
        } 
    }
    else 
    {
        if (state->isSendingReadReport)
        {
            replySendReadReportResponse(orderer, MMS_RESULT_MESSAGE_ID_NOT_FOUND, 0, state->readOrigMsgId);
        }
        else
        {
            replySendMessageResponse(orderer, MMS_RESULT_MESSAGE_ID_NOT_FOUND);                
        } 
    }      
} 

static void msrSigSendReadReport(WE_UINT8 orderer, MmsMsgId rrId, WE_BOOL contOperation)
{
    MsrState *state = &g_msrState;
    
    if (!contOperation)
    {
        


        state->readOrigMsgId = 0; 
    }

    
    if (!contOperation && state->onStateSending == TRUE) 
    {
        if (state->isSendingReadReport)
        {
            replySendReadReportResponse(orderer, MMS_RESULT_BUSY, rrId, state->readOrigMsgId);
        }
        else
        {
            replySendMessageResponse(orderer, MMS_RESULT_BUSY);
        } 
    }
    else if (checkMsgId(rrId)) 
    {    
        
        if ( fldrGetFileSuffix(rrId) == MMS_SUFFIX_SEND_REQ)
        {
            state->sendCancelled = FALSE;
            state->onStateSending = TRUE;                    
            state->msgIdToSend = rrId;
            
            
            moveMsgToFolder(state->msgIdToSend, MMS_OUTBOX);        
            sendOneMsg(state->msgIdToSend, NULL, 0, MMS_M_SEND_REQ);
        }
        else
        {
            
            replySendReadReportResponse(orderer, MMS_RESULT_WRONG_FILE_TYPE, rrId, state->readOrigMsgId);
        } 
    }
    else 
    {
        replySendReadReportResponse(orderer, MMS_RESULT_MESSAGE_ID_NOT_FOUND, 0, state->readOrigMsgId);
    }      
} 

static void msrSigSendRsp(MmsResult result)
{
    MsrState *state = &g_msrState;

    
    
    if (result == MMS_RESULT_OK) 
    {
         
        moveMsgToFolder(state->msgIdToSend, MMS_SENT);
    } 
    
    if (state->isSendingReadReport) 
    {
        replySendReadReportResponse(state->orderer, result, state->msgIdToSend, state->readOrigMsgId);
        state->msgIdToSend = 0;
        state->readOrigMsgId = 0;
        state->isSendingReadReport = FALSE;
    }
    else 
    {
        replySendMessageResponse(state->orderer, result);              
    } 
    
    state->onStateSending = FALSE;  
} 

static void msrSigCreateReadReportRsp(MmsResult result, MmsMsgId readReport)
{
    MsrState *state = &g_msrState;

    if (state->sendCancelled)
    {
        replySendReadReportResponse(state->orderer, MMS_RESULT_CANCELLED_BY_USER, readReport, state->readOrigMsgId);

        msrCleanUp(state);
    }
    else if (result == MMS_RESULT_OK)
    {
        state->onStateSending = FALSE; 
        
        

        msrSigSendReadReport(state->orderer, readReport, TRUE);
    }
    else 
    {
         
        replySendReadReportResponse(state->orderer, result, 0, state->readOrigMsgId); 

        msrCleanUp(state);
    } 
} 

static void msrSigSendResdReportResp(MmsResult result)
{
    MsrState *state = &g_msrState;
    
    replySendReadReportResponse(state->orderer, result, state->msgIdToSend, state->readOrigMsgId);
    msrCleanUp(state);
} 



static void msrSigSendPDU(unsigned char *pdu, WE_UINT32 len, MmsMessageType msgType, MmsResult result)
{
    MsrState *state = &g_msrState;

    if (state->sendCancelled)
    {
        if (msgType == MMS_M_FORWARD_REQ)
        {
            replyForwardReqResponse(state->orderer, MMS_RESULT_CANCELLED_BY_USER);
        }
        else
        {
            replySendReadReportResponse(state->orderer, MMS_RESULT_CANCELLED_BY_USER, 0, state->readOrigMsgId);
        } 

        if (pdu != NULL)
        {
            M_FREE(pdu);
        } 

        msrCleanUp(state);
    }
    else if (result == MMS_RESULT_OK && pdu != NULL)
    {
       



        state->msgIdToSend = 0;
        sendOneMsg(0, pdu, len, msgType);
    }
    else 
    { 
         
        if (pdu != NULL)
        {
            M_FREE(pdu);
        } 

        if (msgType == MMS_M_FORWARD_REQ)
        {
            replyForwardReqResponse(state->orderer, result); 
        } 
        else
        {
            replySendReadReportResponse(state->orderer, result, 0, state->readOrigMsgId);
        } 
        state->msgIdToSend = 0;
        state->readOrigMsgId = 0;
    } 
    state->onStateSending = FALSE;
    state->isSendingReadReport = FALSE; 
    state->sendCancelled = FALSE;
} 

static void msrSigSendForward(WE_UINT8 orderer, unsigned char* pdu, WE_UINT32 len)
{
    MsrState *state = &g_msrState;

    
    if (state->onStateSending == TRUE) 
    {
        M_FREE(pdu); 
        replyForwardReqResponse(orderer, MMS_RESULT_BUSY);   
    }
    else if (pdu != NULL) 
    {
        


        state->onStateSending = TRUE;
        state->orderer = orderer;

        msrSigSendPDU(pdu, len, MMS_M_FORWARD_REQ, MMS_RESULT_OK);
    }   
} 

static void msrSigSendForwardRsp(MmsResult result)
{
    MsrState *state = &g_msrState;

    replyForwardReqResponse(state->orderer, result);
    msrCleanUp(state);
} 






static void msrSendMain(MmsSignal *sig)
{
    switch (sig->type)
    {
    case MMS_SIG_MSR_SEND_CANCEL :      
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR SEND: MMS_SIG_MSR_SEND_CANCEL\n"));
        
        msrSigSendCancel((MmsMsgId)sig->u_param1);
        break;

    case MMS_SIG_MSR_SEND_MSG:         
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR SEND: MMS_SIG_MSR_SEND_MSG\n"));
        
        


        msrSigSendMsg((WE_UINT8)sig->u_param2, (MmsMsgId)sig->u_param1);
        break;

    case MMS_SIG_MSR_SEND_RSP:          
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR SEND received: MMS_SIG_MSR_SEND_RSP\n"));

        msrSigSendRsp((MmsResult)sig->u_param1);
        break; 
    
    case  MMS_SIG_MSR_CREATE_RR:  
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR CREATE received: MMS_SIG_MSR_CREATE_RR\n"));

        msrSigCreateReadReport((WE_UINT8) (MmsMsgId)sig->u_param2, (MmsMsgId)sig->u_param1, (MmsReadStatus)sig->i_param);
        break; 

    case MMS_SIG_MSR_CREATE_RR_RSP:  
        msrSigCreateReadReportRsp((MmsResult)sig->i_param, (MmsMsgId) sig->u_param1);
        break;

    case MMS_SIG_MSR_SEND_RR_RSP:
        msrSigSendResdReportResp((MmsResult)sig->u_param1);
        break;

    case MMS_SIG_MSR_SEND_FORWARD: 
        


        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR SEND received: MMS_SIG_MSR_SEND_FORWARD\n"));

        msrSigSendForward((WE_UINT8) sig->u_param2, (unsigned char*) sig->p_param, (WE_UINT32) sig->u_param1);
        
        break;

    case MMS_SIG_MSR_SEND_PDU:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR SEND received: MMS_SIG_MSR_SEND_PDU\n"));
        
        msrSigSendPDU((unsigned char*) sig->p_param, (WE_UINT32) sig->u_param1,
                      (MmsMessageType)sig->u_param2, (MmsResult) sig->i_param);
        break; 

    case MMS_SIG_MSR_SEND_FORWARD_RSP:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR SEND received: MMS_SIG_MSR_SEND_FORWARD_RSP\n"));

        msrSigSendForwardRsp((MmsResult)sig->u_param1);
        break;

    default:
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR MSR SEND: received unknown signal\n"));
        break;

    } 
    
    mSignalDelete(sig);
} 








static void sendOneMsg(MmsMsgId msgId, unsigned char *buffer, WE_UINT32 len, 
    MmsMessageType type)
{
    MmsSigCohSendParam *param = NULL;
    MmsSigCohPostParam *post = NULL;
    MmsMsrSendSignalId  retSignal;

    if (buffer == NULL)
    {
        param = M_ALLOCTYPE(MmsSigCohSendParam);
        param->msgId = msgId;
        
        
        M_SIGNAL_SENDTO_UUP(M_FSM_COH_SEND, 
            (int)MMS_SIG_COH_SEND,          
            M_FSM_MSR_SEND,                 
            MMS_SIG_MSR_SEND_RSP,            
            param);

    }
    else
    {
        post = M_ALLOCTYPE(MmsSigCohPostParam);
        post->isResponseRequested = TRUE;
        post->type = type;
        post->data.pdu.packet = buffer;
        post->data.pdu.length = len;
        if (type == MMS_M_FORWARD_REQ)
        {
            retSignal = MMS_SIG_MSR_SEND_FORWARD_RSP;
        }
        else
        {
            retSignal = MMS_SIG_MSR_SEND_RR_RSP;
        } 

        
        M_SIGNAL_SENDTO_IUUP( 
            M_FSM_COH_POST,                 
            (int)MMS_SIG_COH_POST,          
            0,                              
            M_FSM_MSR_SEND,                 
            retSignal,                      
            post);                          
    } 
} 







static void moveMsgToFolder(WE_UINT32 filedId, WE_UINT32 folderType)
{
    

    if  ( moveMessage(folderType, filedId) != MMS_RESULT_OK )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR SEND internal parameter fault\n"));
    } 

} 



