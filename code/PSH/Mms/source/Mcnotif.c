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

#include "Mconfig.h"    
#include "Mmsrpl.h"     
#include "Msig.h"       
#include "Mmem.h"       
#include "Mcpdu.h"      
#include "Mcnotif.h"    
#include "Mcpost.h"     
#include "Mnotify.h"    
#include "Mcpdu.h"      
#include "Mlpduc.h"     
#include "Mcwsp.h"      
   

#define INDEX_MSG_TYPE_VALUE    1   
















static void cohPushReceiveMain(MmsSignal *sig);



#ifdef WE_LOG_MODULE 



const char *fsmCOHPushSigName(WE_UINT32 sigType)
{
    switch (sigType)
    {
    case MMS_SIG_COH_PUSH:
        return "MMS_SIG_COH_PUSH";
    case MMS_SIG_COH_NOTIFY_RESP:
        return "MMS_SIG_COH_NOTIFY_RESP";
    case MMS_SIG_COH_NOTIFY_POST_RSP:
        return "MMS_SIG_COH_NOTIFY_POST_RSP";
    default:
        return 0;
    }

} 
#endif










void cohPushReceiveEmergencyAbort(void)
{
    mSignalDeregister(M_FSM_COH_PUSH_RECEIVE);
} 




void cohPushReceiveInit(void)
{
    mSignalRegisterDst(M_FSM_COH_PUSH_RECEIVE, cohPushReceiveMain);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH PUSH RECEIVE: initialized\n"));
} 






static void cohPushReceiveMain(MmsSignal *sig)
{
    unsigned char *pdu;
    MmsSigCohPostParam *post;
    MmsNotifyRespInd    notifyResp;
    WE_UINT32  length;
    WE_BOOL    isPduOk = FALSE;
    WE_BOOL    isRoaming = FALSE;
    MmsNotifIndInfo *notifIndInfo = NULL;

    switch (sig->type)
    {
    case MMS_SIG_COH_PUSH : 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH PUSH RECEIVE: Received MMS_SIG_COH_PUSH\n"));

        
        pdu = (unsigned char *)sig->p_param;
        if ( mmsPduUnrecognized( pdu, sig->u_param1) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Unrecognized PDU!\n", __FILE__, __LINE__));
        }
        else if ( !mmsPduSanityCheck( pdu, sig->u_param1) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Badly formatted PDU!\n", __FILE__, __LINE__));
        }
        else if (pdu[INDEX_MSG_TYPE_VALUE] != MMS_M_NOTIFICATION_IND && 
            pdu[INDEX_MSG_TYPE_VALUE] != MMS_M_DELIVERY_IND && 
            pdu[INDEX_MSG_TYPE_VALUE] != MMS_M_READ_ORIG_IND)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Not a valid pushed PDU!\n", __FILE__, __LINE__));
        }
        else
        {
            M_SIGNAL_SENDTO_IUUP( M_FSM_MSR_NOTIFY, MMS_SIG_MSR_NOTIFICATION, 
                sig->i_param, (unsigned long)pdu[INDEX_MSG_TYPE_VALUE], 
                sig->u_param1, sig->p_param); 

            isPduOk = TRUE;
        } 

        if ( !isPduOk )
        {
            if (sig->p_param != NULL)
            {
                M_FREE(sig->p_param);
            } 

            mmsNotifyError(MMS_RESULT_COMM_ILLEGAL_PDU);
        } 
        break;        
    case MMS_SIG_COH_NOTIFY_RESP : 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH PUSH RECEIVE: Received MMS_SIG_COH_NOTIFY_RESP\n"));

        notifIndInfo = (MmsNotifIndInfo *)sig->p_param;

        


        if (cfgGetInt(MMS_CFG_NO_SMS_NOTIFY_RESP) == TRUE && 
            notifIndInfo->isSmsBearer)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Configured not to send NotifyResp.ind for notifications via SMS\n",
                __FILE__, __LINE__));
            if (notifIndInfo != NULL)
            {
                M_FREE(notifIndInfo->transactionId);
                M_FREE(notifIndInfo);
            } 
            break;
        } 

        isRoaming = (cfgGetInt(MMS_CFG_NETWORK_STATUS_ROAMING) == MMS_ROAMING);

        if (isRoaming &&
            cfgGetInt(MMS_CFG_ROAMING_MODE) == MMS_ROAM_RESTRICTED)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Configured not to send NotifyResp.ind for notifications in restricted mode\n",
                __FILE__, __LINE__));
            if (notifIndInfo != NULL)
            {
                M_FREE(notifIndInfo->transactionId);
                M_FREE(notifIndInfo);
            } 
            break;
        } 
        
        
        notifyResp.transactionId = notifIndInfo->transactionId;
        notifyResp.status = notifIndInfo->msgStatus;
        notifyResp.allowed = notifIndInfo->deliveryReportFlag;

        post = (MmsSigCohPostParam *)M_CALLOC(sizeof(MmsSigCohPostParam));
        if (post == NULL) 
        {
            break;
        }
        post->isResponseRequested = TRUE;
        post->type = MMS_M_NOTIFY_RESP; 
        post->data.pdu.packet = createWspNotifyRespIndMsg( &notifyResp, &length, 
            (MmsVersion)cfgGetInt(MMS_CFG_PROXY_RELAY_VERSION));
        post->data.pdu.length = length;
        M_FREE(notifIndInfo->transactionId);
        M_FREE(notifIndInfo);

        if (post->data.pdu.packet == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Couldn't create NotifyResp.ind!\n",
                __FILE__, __LINE__));
            M_FREE(post);
        }
        else
        {
            M_SIGNAL_SENDTO_IUUP( 
                M_FSM_COH_POST,             
                (int)MMS_SIG_COH_POST,           
                0,                          
                M_FSM_COH_PUSH_RECEIVE,     
                MMS_SIG_COH_NOTIFY_POST_RSP,
                post);                      
        } 
        break;
    case MMS_SIG_COH_NOTIFY_POST_RSP : 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH PUSH RECEIVE: Received MMS_SIG_COH_NOTIFY_POST_RSP\n"));

        if (sig->u_param1 != MMS_RESULT_OK)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "MMS FSM COH PUSH RECEIVE: Post failed (%d)\n", sig->u_param1));
        } 
        break;
    default:
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM COH PUSH RECEIVE: received unknown signal\n"));
        break;
    } 
    
    mSignalDelete(sig);
} 




void cohPushReceiveTerminate(void)
{
    mSignalDeregister(M_FSM_COH_PUSH_RECEIVE);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM COH PUSH RECEIVE: terminated\n"));
} 
