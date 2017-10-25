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
#include "We_Cmmn.h"    
#include "We_File.h"    
#include "We_Dcvt.h"   

#include "Mms_Def.h"    
#include "Mms_Cfg.h"    
#include "Mms_If.h"     
#include "Mmsrpl.h"     

#include "Msig.h"       
#include "Mreceive.h"   
#include "Mcget.h"      
#include "Mnotify.h"    
#include "Mmem.h"       
#include "Mutils.h"     
#include "Mconfig.h"    
#include "Fldmgr.h"     
#include "Mhandler.h"   
#include "Mcpdu.h"      
#include "Mcpost.h"     
#include "Mcwsp.h"      








typedef enum
{
    STATE_IDLE,     
    STATE_DELAYED_RETRIEVAL_ACTIVE,
    STATE_IMMEDIATE_RETRIEVAL_ACTIVE
} RetrievalState;


typedef struct
{   
    WE_UINT8 orderer;                  
    MmsMsgId notifId;                   
    MmsMsgId msgId;                     
         
    char *acknowledgeId;                
    MmsNotification *mmsNotification;    
    WE_BOOL isSmsBearer;               
    MmsVersion version;                 
    WE_BOOL retrieveCancelled;         
} RetrievalData;
















static RetrievalData current;               
static RetrievalState retrievalState;       


static void cleanup(void);
static void deleteUnusedFile(MmsMsgId msgId);
static void immediateRetrievalDone(MmsMsgId msgId, MmsResult result);
static void msrReceiveMain(MmsSignal *sig);
static void reserveDiskSpace(MmsFolderType location, WE_UINT32 size);
static void startNextImmediateRetrieve(void);


#ifdef WE_LOG_MODULE 



const char *fsmMsrReceiveSigName(WE_UINT32 sigType)
{
    switch (sigType)
    {
    case MMS_SIG_MSR_RECEIVE_CANCEL:
        return "MMS_SIG_MSR_RECEIVE_CANCEL";
    case MMS_SIG_MSR_RECEIVE_DELAYED:
        return "MMS_SIG_MSR_RECEIVE_DELAYED";
    case MMS_SIG_MSR_RECEIVE_DELAYED_GET_HEADER_RSP:
        return "MMS_SIG_MSR_RECEIVE_DELAYED_GET_HEADER_RSP";
    case MMS_SIG_MSR_RECEIVE_GET_RSP:
        return "MMS_SIG_MSR_RECEIVE_GET_RSP";
    case MMS_SIG_MSR_RECEIVE_FILE_CREATED:
        return "MMS_SIG_MSR_RECEIVE_FILE_CREATED";
    case MMS_SIG_MSR_MSG_DONE_RSP:
        return "MMS_SIG_MSR_MSG_DONE_RSP";
    case MMS_SIG_MSR_RECEIVE_ACK_RSP:
        return "MMS_SIG_MSR_RECEIVE_ACK_RSP";
    case MMS_SIG_MSR_RECEIVE_FILE_DELETED:
        return "MMS_SIG_MSR_RECEIVE_FILE_DELETED";
    default:
        return 0;
    }
} 
#endif












static void setMsgFileSize(WE_UINT32 msgId, WE_INT32 size)
{
    char *fileName = Mms_FldrMsgIdToName(msgId, MMS_SUFFIX_MSG);
    int fileHandle = 0; 
            
    if ((fileName != NULL) && (size > 0))
    {
        fileHandle = WE_FILE_OPEN(WE_MODID_MMS, fileName, 
            WE_FILE_SET_RDWR, 0);
                
        if (fileHandle >= 0 ) 
        {
            if (WE_FILE_SETSIZE(fileHandle, size) < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "MMS FSM MSR RECEIVE Failed to set file size to msgId (%d).\n",
                     msgId));
            } 
                    
            if (WE_FILE_CLOSE (fileHandle) < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "MMS FSM MSR RECEIVE Failed close file handle(%d).\n",
                     msgId));
            }  
        } 
        else 
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "MMS FSM MSR RECEIVE Failed to open msgId (%d).\n",
                msgId));
        } 
    } 
} 









void msrReceiveEmergencyAbort(void)
{
    mSignalDeregister(M_FSM_MSR_RECEIVE);
} 





static void cleanup(void)
{
    current.msgId = 0;
    current.notifId = 0;
    current.isSmsBearer = FALSE;
    current.retrieveCancelled = FALSE;
    current.orderer = 0;

    if (NULL != current.acknowledgeId)
    {
        M_FREE(current.acknowledgeId);
    }
    
    if (current.mmsNotification != NULL) 
    {
        freeMmsNotification( WE_MODID_MMS, current.mmsNotification);
        M_FREE(current.mmsNotification);
    } 
} 




void msrReceiveInit(void)
{
    retrievalState = STATE_IDLE;
    current.mmsNotification = NULL;
    current.acknowledgeId = NULL;
    cleanup();

    mSignalRegisterDst(M_FSM_MSR_RECEIVE, msrReceiveMain);
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM MSR RECEIVE: initialized\n"));
} 

  



static WE_BOOL postAcknowledgeInd(char *transId)
{
    WE_UINT32      length;
    MmsSigCohPostParam *post;
    MmsAcknowledgeInd   ack;
    

    if (NULL == transId)
    {
        return FALSE;
    }

    







    
    
    ack.transactionId = transId;
    ack.allowed = (MmsDeliveryReportAllowed)cfgGetInt(MMS_CFG_REPORT_ALLOWED);
    
    post = M_ALLOCTYPE(MmsSigCohPostParam);
    if (NULL == post)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Couldn't allocate memory\n", __FILE__, __LINE__));
        return FALSE;
    }
    post->isResponseRequested = TRUE;
    post->type = MMS_M_ACKNOWLEDGE_IND;
    post->data.pdu.packet = createWspAcknowledgeIndMsg( &ack, 
        &length, (MmsVersion)cfgGetInt(MMS_CFG_PROXY_RELAY_VERSION));
    post->data.pdu.length = length;
    
    if (post->data.pdu.packet == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Couldn't create PDU\n", __FILE__, __LINE__));
        return FALSE;
    }
    else
    {        
        M_SIGNAL_SENDTO_IUUP( 
            M_FSM_COH_POST,              
            (int)MMS_SIG_COH_POST,       
            0,                           
            M_FSM_MSR_RECEIVE,           
            MMS_SIG_MSR_RECEIVE_ACK_RSP, 
            post);                       
    } 
    
    return TRUE;
} 









static void deleteUnusedFile(MmsMsgId msgId)
{
    st_MmsFmgDeleteParam *param = M_CALLOC(sizeof(st_MmsFmgDeleteParam));

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM MSR RECEIVE: delete unused file %d\n", msgId));

    param->uiMsgId = msgId;
    M_SIGNAL_SENDTO_IUUP( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_DELETE_MSG,
        0, M_FSM_MSR_RECEIVE, MMS_SIG_MSR_RECEIVE_FILE_DELETED, param);
} 








static void reserveDiskSpace(MmsFolderType location, WE_UINT32 size)
{
    st_MmsFmgCreateParam *param = M_CALLOC( sizeof(st_MmsFmgCreateParam));

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM MSR RECEIVE: reserve disk space, %d bytes in folder %d\n",
        size, location));

    param->eSuffix = MMS_SUFFIX_MSG;
    param->eLocation = location;
    param->uiSize = size;
    M_SIGNAL_SENDTO_IUUP( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_CREATE_MSG,
        0, M_FSM_MSR_RECEIVE, MMS_SIG_MSR_RECEIVE_FILE_CREATED, param);
} 










static void immediateRetrievalDone( MmsMsgId msgId, MmsResult result)
{
    if (result == MMS_RESULT_OK)
    { 
        


        deleteUnusedFile(current.notifId);

        


        sendNotifyIndRsp( current.isSmsBearer, 
            current.mmsNotification->transactionId, MMS_STATUS_RETRIEVED, 
            current.mmsNotification->msgClass.classIdentifier, 
            current.version);

        


        if (current.mmsNotification->msgClass.classIdentifier != MMS_MESSAGE_CLASS_AUTO &&
            (unsigned int)cfgGetInt(MMS_CFG_REPORT_ALLOWED) != MMS_DELIVERY_REPORT_ALLOWED_NO)
        {
            


            fldrMgrSetDRsent(current.msgId);
        }            

        replyNotificationReceived(MMS_NOTIFICATION_MESSAGE, msgId, 
            &current.mmsNotification->from, 
            &current.mmsNotification->subject, 
            current.mmsNotification->length,
            MMS_NOTIFICATION_REASON_NORMAL);
        
        cleanup();
        retrievalState = STATE_IDLE;
        mmsImmediateRetrievalGet();
    }
    else
    { 

        


        mmsNotifyError(result);
        
        


        if (MMS_RESULT_OK != (result = mmsPromoteNotifToDelayed(current.notifId)))
        {
            


            mmsNotifyError(result);
        }
        else
        {
            sendNotifyIndRsp( current.isSmsBearer,
                current.mmsNotification->transactionId,
                MMS_STATUS_DEFERRED, 
                current.mmsNotification->msgClass.classIdentifier,
                current.version);

            


            if (current.mmsNotification->msgClass.classIdentifier != MMS_MESSAGE_CLASS_AUTO &&
                (unsigned int)cfgGetInt(MMS_CFG_REPORT_ALLOWED) != MMS_DELIVERY_REPORT_ALLOWED_YES)
            {
                


                fldrMgrSetDRsent(current.msgId);
            }     
            
            replyNotificationReceived(MMS_NOTIFICATION_NORMAL, 
                current.notifId,
                &current.mmsNotification->from, 
                &current.mmsNotification->subject, 
                current.mmsNotification->length,
                MMS_NOTIFICATION_REASON_RETIEVAL_FAILED);             
        }

        cleanup();
        retrievalState = STATE_IDLE;
        mmsImmediateRetrievalGet();
    } 
} 










static void delayedRetrievalDone( MmsMsgId msgId, MmsResult result)
{
        if (result == MMS_RESULT_OK)
        {
            



            deleteUnusedFile(current.notifId);

            


            if(postAcknowledgeInd(current.acknowledgeId))
            {
                


                return;
            }

            


            replyRetrieveMessageResponse(current.orderer, MMS_RESULT_OK, msgId);
        }
        else
        {
            replyRetrieveMessageResponse(current.orderer, result, 0);
        }

        cleanup();
        retrievalState = STATE_IDLE;
        mmsImmediateRetrievalGet();
} 







static void msrReceiveMain(MmsSignal *sig)
{
    MmsSigCohGetParam *mmsSigCohGetPar; 
    MmsSigCohGetResultParam *getResult; 
    MmsResult ret = MMS_RESULT_OK;       
    MmsNotification *mNotif = NULL;
    
    switch (sig->type)
    {
    case MMS_SIG_MSR_RECEIVE_CANCEL :
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR RECEIVE: MMS_SIG_MSR_RECEIVE_CANCEL\n"));
        
        

        if (retrievalState == STATE_DELAYED_RETRIEVAL_ACTIVE)
        {
            if ((MmsMsgId)sig->u_param1 == current.notifId ||
                (MmsMsgId)sig->u_param1 == 0) 
            {
                current.retrieveCancelled = TRUE;
                if (current.msgId > 0)
                {
                    cohGetCancel(current.msgId);
                }
            } 
        } 
        else if (retrievalState == STATE_IMMEDIATE_RETRIEVAL_ACTIVE)
        {
            if ((MmsMsgId)sig->u_param1 == current.notifId ||
                (MmsMsgId)sig->u_param1 == 0) 
            {
                current.retrieveCancelled = TRUE;
                if (current.msgId > 0)
                {
                    cohGetCancel(current.msgId);
                }
            } 
        } 
        break;

    case MMS_SIG_MSR_RECEIVE_DELAYED : 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR RECEIVE: MMS_SIG_MSR_RECEIVE_DELAYED\n"));
       
        
        if (retrievalState != STATE_IDLE)
        {
            replyRetrieveMessageResponse((WE_UINT8) sig->u_param2, MMS_RESULT_BUSY,(MmsMsgId)0);
        }
        else
        {    
            retrievalState = STATE_DELAYED_RETRIEVAL_ACTIVE;
            cleanup();
            current.notifId = (MmsMsgId)sig->u_param1;
            current.orderer = (WE_UINT8) sig->u_param2;

            
            M_SIGNAL_SENDTO_IUU( M_FSM_MMH_HANDLER,
                (int)M_FSM_MMH_GET_HEADER, M_FSM_MSR_RECEIVE,
                current.notifId, MMS_SIG_MSR_RECEIVE_DELAYED_GET_HEADER_RSP);
        }  
        break;

    case MMS_SIG_MSR_RECEIVE_DELAYED_GET_HEADER_RSP:
        



        mNotif = (MmsNotification *)sig->p_param;
        if (current.retrieveCancelled && retrievalState == STATE_DELAYED_RETRIEVAL_ACTIVE)
        {
            

            M_SIGNAL_SENDTO_IU( M_FSM_MSR_RECEIVE, (int)MMS_SIG_MSR_MSG_DONE_RSP, 
                (WE_INT32)current.msgId, MMS_RESULT_CANCELLED_BY_USER);

        }
        else if (current.retrieveCancelled && retrievalState == STATE_IMMEDIATE_RETRIEVAL_ACTIVE)
        {
            

            M_SIGNAL_SENDTO_IU( M_FSM_MSR_RECEIVE, (int)MMS_SIG_MSR_MSG_DONE_RSP, 
                (WE_INT32)current.msgId, MMS_RESULT_CANCELLED_BY_SYSTEM);
        }
        else if ((MmsResult)sig->i_param != MMS_RESULT_OK || mNotif == NULL)
        {  

            

            M_SIGNAL_SENDTO_IU( M_FSM_MSR_RECEIVE, (int)MMS_SIG_MSR_MSG_DONE_RSP, 
                (WE_INT32)current.msgId, (MmsResult)sig->i_param);            
        }
        else if (cfgGetInt(MMS_CFG_MAX_RETRIEVAL_SIZE) > 0 &&
                mNotif->length > (WE_UINT32) cfgGetInt(MMS_CFG_MAX_RETRIEVAL_SIZE))
        {
            

            M_SIGNAL_SENDTO_IU( M_FSM_MSR_RECEIVE, (int)MMS_SIG_MSR_MSG_DONE_RSP, 
                (WE_INT32)current.msgId, MMS_RESULT_MAX_RETRIEVAL_SIZE_EXCEEDED);            
        }
        else
        {
            current.mmsNotification = mNotif;
            mNotif = NULL;
            reserveDiskSpace( MMS_HIDDEN, current.mmsNotification->length);
        } 

        if (mNotif)
        {
            freeMmsNotification( WE_MODID_MMS, mNotif); 
            M_FREE(mNotif); 
        } 
        break; 

    case MMS_SIG_MSR_RECEIVE_FILE_CREATED :
        current.msgId = sig->u_param2;  
        
        if (sig->u_param1 == MMS_RESULT_OK)
        {
            if (current.retrieveCancelled)
            {
                


                if (retrievalState == STATE_IMMEDIATE_RETRIEVAL_ACTIVE)
                {
                    M_SIGNAL_SENDTO_IU( M_FSM_MSR_RECEIVE, (int)MMS_SIG_MSR_MSG_DONE_RSP, 
                        (WE_INT32)current.msgId, MMS_RESULT_CANCELLED_BY_SYSTEM); 
                }
                else
                {
                    M_SIGNAL_SENDTO_IU( M_FSM_MSR_RECEIVE, (int)MMS_SIG_MSR_MSG_DONE_RSP, 
                        (WE_INT32)current.msgId, MMS_RESULT_CANCELLED_BY_USER);
                }
                break;
            }

            mmsSigCohGetPar = (MmsSigCohGetParam*)M_CALLOC(sizeof(MmsSigCohGetParam));
            mmsSigCohGetPar->path = sig->p_param;   
            mmsSigCohGetPar->msgId = current.msgId; 
            mmsSigCohGetPar->size = current.mmsNotification->length;
            mmsSigCohGetPar->isImmediate = (retrievalState == STATE_IMMEDIATE_RETRIEVAL_ACTIVE);

            
            mmsSigCohGetPar->uri = we_cmmn_strdup( WE_MODID_MMS, current.mmsNotification->contentLocation);
            mmsSigCohGetPar->transactionId = we_cmmn_strdup( WE_MODID_MMS, 
                current.mmsNotification->transactionId);
            
             
            M_SIGNAL_SENDTO_IUUP( M_FSM_COH_GET, (int)MMS_SIG_COH_GET_MSG,
                0, M_FSM_MSR_RECEIVE, MMS_SIG_MSR_RECEIVE_GET_RSP, 
                mmsSigCohGetPar);
        }
        else 
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Folder Manager failed %d.\n", 
                __FILE__, __LINE__, sig->u_param1));

            

            M_SIGNAL_SENDTO_IU( M_FSM_MSR_RECEIVE, (int)MMS_SIG_MSR_MSG_DONE_RSP, 
                (WE_INT32)current.msgId, sig->u_param1);

            if (sig->p_param != NULL)
            {
                M_FREE(sig->p_param);
            } 
        } 
        break;

    case MMS_SIG_MSR_RECEIVE_GET_RSP:
        


        if (sig->p_param == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "MMS FSM MSR RECEIVE: Signal error, received no result.\n"));
            cleanup();
            retrievalState = STATE_IDLE;
            break;
        } 

        getResult = (MmsSigCohGetResultParam *)sig->p_param;
        ret = getResult->result;

        if (ret == MMS_RESULT_OK)
        {  
            
            if (getResult->size != current.mmsNotification->length) 
            {
                setMsgFileSize(getResult->msgId, (WE_INT32)getResult->size);
            }  

            
            current.acknowledgeId = getResult->transactionId;            

             
            M_SIGNAL_SENDTO_IUU( M_FSM_MMH_HANDLER,
                (int)M_FSM_MMH_INT_CREATE_INFO_LIST, M_FSM_MSR_RECEIVE,
                getResult->msgId, MMS_SIG_MSR_MSG_DONE_RSP);
        }
        else 
        {            
            

            M_SIGNAL_SENDTO_IU( M_FSM_MSR_RECEIVE, (int)MMS_SIG_MSR_MSG_DONE_RSP, 
                (WE_INT32)current.msgId, ret);
        } 

        M_FREE(sig->p_param);
        break;

        
    case MMS_SIG_MSR_MSG_DONE_RSP:     
        ret = (MmsResult)sig->u_param1;  

        if (ret != MMS_RESULT_OK)
        {
            
            if (current.msgId != 0)
            {                
                deleteUnusedFile(current.msgId);
                current.msgId = 0;
            }
        }
        else
        {
            


            if (current.msgId != 0)
            {                
                if (MMS_RESULT_OK != mmsFldrMgrSetFolderQuiet(current.msgId, MMS_INBOX))
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                        "%s(%d): Message move failed, message will remain invisible %d\n", __FILE__, __LINE__, sig->u_param1));                    
                }
            }
        }
        
        if (retrievalState == STATE_IMMEDIATE_RETRIEVAL_ACTIVE)
        {   
            immediateRetrievalDone(current.msgId, ret);
        }
        else 
        {
            delayedRetrievalDone(current.msgId, ret);
        } 
        break;

    case MMS_SIG_MSR_RECEIVE_ACK_RSP:
        if (sig->u_param1 != MMS_RESULT_OK)
        {
            



            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "MMS FSM MSR RECEIVE: Post of M-Acknowledge.ind failed %d\n",
                sig->u_param1));
        }
        else
        {
            


            if (current.mmsNotification->deliveryReport == MMS_DELIVERY_REPORT_YES &&
                (unsigned int)cfgGetInt(MMS_CFG_REPORT_ALLOWED) != MMS_DELIVERY_REPORT_ALLOWED_NO)
            {
                


                fldrMgrSetDRsent(current.msgId);
            }
                
        } 

        


        replyRetrieveMessageResponse(current.orderer, MMS_RESULT_OK, current.msgId);
        
        cleanup();
        retrievalState = STATE_IDLE;
        mmsImmediateRetrievalGet();
        break;

    case MMS_SIG_MSR_RECEIVE_NEXT_IMMEDIATE:
        startNextImmediateRetrieve();
        break;
    case MMS_SIG_MSR_RECEIVE_FILE_DELETED :
        if (sig->u_param1 != MMS_RESULT_OK)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Deletion failed %d\n", __FILE__, __LINE__, sig->u_param1));
        } 
        break;

    default:
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR RECEIVE: received unknown signal %d\n", sig->type));
        break;
    } 
    
    mSignalDelete(sig);
} 




void msrReceiveTerminate(void)
{
    


    if (retrievalState == STATE_DELAYED_RETRIEVAL_ACTIVE)
    {
        if (current.msgId != 0)
        {
            
            deleteUnusedFile(current.msgId);
        }
    }
    
    mSignalDeregister(M_FSM_MSR_RECEIVE);
    cleanup();
    retrievalState = STATE_IDLE;

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM MSR RECEIVE: terminated\n"));
} 








static void startNextImmediateRetrieve(void)
{
    MmsMsgId notifId; 
    
    if (retrievalState != STATE_IDLE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No immediate retrieval is started while doing "
            "another retrieval.\n", __FILE__, __LINE__));

        return ;
    }

    if (cfgGetInt(MMS_CFG_NETWORK_STATUS_BEARER) == MMS_BEARER_NOT_AVAILABLE)
    {
        


        retrievalState = STATE_IDLE;
        return ;        
    }

    if (cfgGetInt(MMS_CFG_ROAMING_MODE) == MMS_ROAMING)
    {
        switch (cfgGetInt(MMS_CFG_NETWORK_STATUS_ROAMING))
        {
        case MMS_ROAM_DEFAULT:
            break;
        case MMS_ROAM_DELAYED:
        case MMS_ROAM_RESTRICTED:
        case MMS_ROAM_REJECT:
        default:
            


            retrievalState = STATE_IDLE;
            return ;        
        }
    }

    


    if (cfgGetInt(MMS_CFG_IMMEDIATE_RETRIEVAL) == 0)
    {
        



        mmsShowAllImmediateNotif();
        retrievalState = STATE_IDLE;
        return ;
    }

    notifId = fldrMgrSearchImmNotif();
    if (notifId != 0) 
    {
        cleanup();
        retrievalState = STATE_IMMEDIATE_RETRIEVAL_ACTIVE;
        current.notifId = notifId;
        
        
        M_SIGNAL_SENDTO_IUU( M_FSM_MMH_HANDLER,
            (int)M_FSM_MMH_GET_HEADER, M_FSM_MSR_RECEIVE,
            current.notifId, MMS_SIG_MSR_RECEIVE_DELAYED_GET_HEADER_RSP);
    }
    else    
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No immediate retrievals"
            "Retrieval.\n", __FILE__, __LINE__));
        retrievalState = STATE_IDLE;
    } 
} 







void mmsImmediateRetrievalGet(void)
{
    
    M_SIGNAL_SENDTO( M_FSM_MSR_RECEIVE,
        MMS_SIG_MSR_RECEIVE_NEXT_IMMEDIATE);
} 
