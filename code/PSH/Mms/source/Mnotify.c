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
#include "We_Url.h"    
#include "We_Dcvt.h"   

#include "Mms_Def.h"    
#include "Mms_Cfg.h"    
#include "Mms_If.h"     
#include "Mmsrpl.h"     


#include "Msig.h"       
#include "Mmem.h"       
#include "Mcpdu.h"      
#include "Mconfig.h"    
#include "Mutils.h"     
#include "Mnotify.h"    
#include "Mreceive.h"   
#include "Mcnotif.h"    
#include "Fldmgr.h"     
#include "Masync.h"     

#include "Mlpdup.h"      







typedef struct StNotifFsmData
{
   WE_UINT32 msgId;                 
   unsigned char *pdu;                
   WE_UINT32 len;                   
   MmsVersion version;               
   MmsNotification *mmsNotification;  
   WE_BOOL isSmsBearer;             
   MmsNotificationReason reason;     
   MmsFileType suffix;               
   MmsFolderType location;           
   struct StNotifFsmData *next;      
   AsyncOperationHandle asyncOper;    
   WE_UINT32 origId;                
} NotifFsmData; 












static WE_BOOL msrMmsRunning;      
static NotifFsmData *currentNotif;  


static void msrNotifyMain(MmsSignal *sig);


#ifdef WE_LOG_MODULE 



const char *fsmMsrNotifSigName(WE_UINT32 sigType)
{
    switch (sigType)
    {
    case MMS_SIG_MSR_NOTIFICATION:
        return "MMS_SIG_MSR_NOTIFICATION";
    case MMS_SIG_MSR_NOTIFY_FILE_CREATED:
        return "MMS_SIG_MSR_NOTIFY_FILE_CREATED";
    case MMS_SIG_MSR_NOTIFY_ASYNC_WRITE_FINISHED:
        return "MMS_SIG_MSR_NOTIFY_ASYNC_WRITE_FINISHED";
    default:
        return 0;
    }
} 
#endif







void msrNotifyEmergencyAbort(void)
{
    msrMmsRunning = FALSE;
    mSignalDeregister(M_FSM_MSR_NOTIFY);
} 






static void freeNotifFsmData(NotifFsmData *notifFsmData)
{
    if (NULL != notifFsmData->asyncOper)
    {  
        asyncOperationStop(&notifFsmData->asyncOper);
        notifFsmData->asyncOper = NULL;
    } 
    
    if (NULL != notifFsmData->pdu) 
    {    
        M_FREE(notifFsmData->pdu);
        notifFsmData->pdu = NULL;
    }  
    
    if (notifFsmData->mmsNotification != NULL) 
    {
        freeMmsNotification(WE_MODID_MMS , notifFsmData->mmsNotification); 
        M_FREE( notifFsmData->mmsNotification);
        notifFsmData->mmsNotification = NULL;
    }     
} 







void msrNotifyInit(void)
{
     
    if (msrMmsRunning == TRUE) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR NOTIFY: M_FSM_MSR_NOTIFY is already initialized\n"));
        return;
    }
    else 
    {
        msrMmsRunning = TRUE;
        currentNotif = NULL;

        mSignalRegisterDst(M_FSM_MSR_NOTIFY, msrNotifyMain);
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR NOTIFY: initialized\n"));
    } 
} 







void msrNotifyTerminate(void)
{
    NotifFsmData *oldNotif;

    if ( !msrMmsRunning)
    {
        return;  
    } 
    
    msrMmsRunning = FALSE;

    
    while (NULL != currentNotif)
    {
        oldNotif = currentNotif;
        currentNotif = currentNotif->next;
        freeNotifFsmData(oldNotif);
        M_FREE( oldNotif );
    }

    mSignalDeregister(M_FSM_MSR_NOTIFY);
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM MSR NOTIFY: MSR_notifyTerminate\n"));
} 








static char *getTransactionId(unsigned char *pdu, WE_UINT32 len)
{
    MmsHeaderValue trId;

    if (!mmsPduGet( pdu, len, X_MMS_TRANSACTION_ID, &trId) || 
        trId.transactionId == NULL)
    {
         WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
             "%s(%d): No X_MMS_TRANSACTION_ID tag\n", __FILE__, __LINE__));
         return NULL; 
    }   

    return (char *)trId.transactionId; 
} 





static void startNotificationWrite(void)
{
    st_MmsFmgCreateParam *param = NULL;
    NotifFsmData *oldNotif = NULL;       

    if (NULL != currentNotif)
    { 

        



        param = M_ALLOC( sizeof(st_MmsFmgCreateParam));
        if (NULL == param)
        {
           WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
               "%s(%d): No memory. I'll die!! please restart\n", 
               __FILE__, __LINE__));

           
           while (NULL != currentNotif)
           {
               oldNotif = currentNotif;
               currentNotif = currentNotif->next;
               freeNotifFsmData( oldNotif );
               M_FREE( oldNotif );
           }
           return ;
        }
        param->eSuffix = currentNotif->suffix;
        param->eLocation = currentNotif->location;
        param->uiSize = currentNotif->len;

        M_SIGNAL_SENDTO_IUUP( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_CREATE_MSG,
           0, M_FSM_MSR_NOTIFY, MMS_SIG_MSR_NOTIFY_FILE_CREATED, param);

        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
           "MMS FSM MSR NOTIFY: reserve disk space, %d bytes in folder %d\n",
           currentNotif->len, currentNotif->location));
    }
} 





static void popNotificationWrite(void)
{
    NotifFsmData *oldNotif;       
   
    if (NULL == currentNotif)
    { 
        return ;
    }

    
    oldNotif = currentNotif;
    currentNotif = currentNotif->next;
    freeNotifFsmData( oldNotif );
    M_FREE( oldNotif );

    if (NULL != currentNotif)
    { 
        startNotificationWrite();
    }
} 













  
static void pushNotificationWrite(WE_BOOL isSmsBearer,
    WE_UINT32 len, unsigned char *pdu, MmsVersion version,
    MmsNotificationReason reason,
    MmsFileType suffix, MmsFolderType location,
    MmsNotification *mmsNotification)
{
    NotifFsmData *newFsm = NULL;
    NotifFsmData *ptrFsm = NULL;

    newFsm = M_ALLOC(sizeof(NotifFsmData));
    if (NULL == newFsm)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No memory. I'll die!! please restart\n", 
            __FILE__, __LINE__));
        return ;
    }
    
    
    newFsm->isSmsBearer = isSmsBearer;
    newFsm->mmsNotification = mmsNotification; 
    newFsm->pdu = pdu; 
    newFsm->len = len; 
    newFsm->version = version;
    newFsm->reason = reason;
    newFsm->suffix = suffix; 
    newFsm->location = location;
    newFsm->msgId = 0;
    newFsm->asyncOper = NULL;
    newFsm->next = NULL;
    
    if (currentNotif == NULL)
    { 
        currentNotif = newFsm;
        
        startNotificationWrite();
    }
    else
    { 
        ptrFsm = currentNotif;
        while (ptrFsm->next != NULL)
        {
            ptrFsm = ptrFsm->next;
        }
        
        
        ptrFsm->next = newFsm;
    }
} 

WE_BOOL notificationUpdate()
{    
    currentNotif->origId = 0;
    
    switch (currentNotif->suffix)
    {
    case MMS_SUFFIX_NOTIFICATION:
    case MMS_SUFFIX_IMMEDIATE:
        {
             
            fldrMgrSetNotify(currentNotif->msgId, currentNotif->mmsNotification, currentNotif->len);            
        } 
        break; 
    case MMS_SUFFIX_READREPORT: 
        {
            MmsReadOrigInd *mmsOrigInd = NULL;  
            
            
            mmsOrigInd = parseReadReport( WE_MODID_MMS, currentNotif->pdu, currentNotif->len); 
            if (mmsOrigInd != NULL)
            {
                 
                currentNotif->origId = fldrUpdateMmtReadReport(currentNotif->msgId,
                    mmsOrigInd,
                    currentNotif->len);
                
                freeMmsReadOrigInd( WE_MODID_MMS, mmsOrigInd);
                M_FREE( mmsOrigInd);
                mmsOrigInd = NULL;
            } 
            else 
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Received read report is corrupted\n", 
                    __FILE__, __LINE__));
                
                mmsNotifyError(MMS_RESULT_MESSAGE_CORRUPT);                
                return FALSE;
            } 
        }
        break; 
    case MMS_SUFFIX_DELIVERYREPORT: 
        {
            MmsDeliveryInd *mmsDelivery;
            
            

 
            mmsDelivery = (MmsDeliveryInd *)M_CALLOC(sizeof(MmsDeliveryInd));
            if ( parseDeliveryReport( WE_MODID_MMS, 
                currentNotif->pdu, currentNotif->len, mmsDelivery) == FALSE)
            {
                
                M_FREE( mmsDelivery);
                mmsDelivery = NULL;
                
                mmsNotifyError(MMS_RESULT_MESSAGE_CORRUPT);                
                return FALSE;
            } 
            
             
            currentNotif->origId = fldrUpdateMmtDelReport (currentNotif->msgId,
                mmsDelivery->messageId,
                mmsDelivery->to->address,
                mmsDelivery->date,
                currentNotif->len);
            
            freeMmsDeliveryInd( WE_MODID_MMS, mmsDelivery);
            M_FREE(mmsDelivery);
            mmsDelivery = NULL;
        }
        break;
        
    case MMS_SUFFIX_TEMPLATE:
    case MMS_SUFFIX_SEND_REQ:
    case MMS_SUFFIX_INFO:  
    case MMS_SUFFIX_MSG:
    case MMS_SUFFIX_ERROR: 
    default:  
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Internal error wrong pdu type\n", 
            __FILE__, __LINE__));
        return FALSE;
    } 
    
    return TRUE;
} 

void notificationFinished(MmsResult result)
{    
    if ((MmsResult)result != MMS_RESULT_OK)
    {
        
        popNotificationWrite();

        



        return ;
    }
    
    switch (currentNotif->suffix)
    {
    case MMS_SUFFIX_IMMEDIATE:
        {           
            if (NULL == getTransactionId(currentNotif->pdu, currentNotif->len))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): PDU is corrupt X_MMS_TRANSACTION_ID tag\n", __FILE__, __LINE__));            
                
                mmsNotifyError(MMS_RESULT_COMM_ILLEGAL_PDU);
            }
            else 
            {
                if (cfgGetInt(MMS_CFG_NETWORK_STATUS_ROAMING) == MMS_ROAMING)
                {
                    if (cfgGetInt(MMS_CFG_NETWORK_STATUS_ROAMING) == MMS_ROAM_RESTRICTED)
                    {                
                        replyNotificationReceived(MMS_NOTIFICATION_IMMEDIATE, 
                            currentNotif->msgId,
                            &currentNotif->mmsNotification->from, 
                            &currentNotif->mmsNotification->subject, 
                            currentNotif->mmsNotification->length,
                            currentNotif->reason); 
                    }
                }
                
                mmsImmediateRetrievalGet();
            }
        }
        break;
    case MMS_SUFFIX_NOTIFICATION: 
        {
             char *trId;

             if ((trId = getTransactionId(currentNotif->pdu, currentNotif->len)) == NULL)
             {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                        "%s(%d): PDU is corrupt X_MMS_TRANSACTION_ID tag\n", __FILE__, __LINE__));            
                        
                    
             }
             else 
             {
                sendNotifyIndRsp( currentNotif->isSmsBearer,
                    trId ,
                    MMS_STATUS_DEFERRED, 
                    currentNotif->mmsNotification->msgClass.classIdentifier,
                    currentNotif->version);

                


                if (currentNotif->mmsNotification->msgClass.classIdentifier != MMS_MESSAGE_CLASS_AUTO &&
                    (unsigned int)cfgGetInt(MMS_CFG_REPORT_ALLOWED) != MMS_DELIVERY_REPORT_ALLOWED_NO)
                {
                    


                    fldrMgrSetDRsent(currentNotif->msgId);
                }            

    
                replyNotificationReceived(MMS_NOTIFICATION_NORMAL, 
                    currentNotif->msgId,
                    &currentNotif->mmsNotification->from, 
                    &currentNotif->mmsNotification->subject, 
                    currentNotif->mmsNotification->length,
                    currentNotif->reason); 
             } 
        } 
        break; 
     case MMS_SUFFIX_READREPORT: 
        {
            MmsReadOrigInd *mmsOrigInd = NULL;  

            
            mmsOrigInd = parseReadReport( WE_MODID_MMS, currentNotif->pdu, currentNotif->len); 
        
            if (mmsOrigInd != NULL)
            {            
                 
                notifyReadReportIndication(mmsOrigInd, currentNotif->origId, currentNotif->msgId);

           
                freeMmsReadOrigInd( WE_MODID_MMS, mmsOrigInd);
                M_FREE( mmsOrigInd);
             } 
             else 
             {
                 WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                     "%s(%d): Received read report is corrupted %d\n", 
                     __FILE__, __LINE__, result));
                 mmsNotifyError(MMS_RESULT_MESSAGE_CORRUPT);    
             } 
        }
        break; 
     case MMS_SUFFIX_DELIVERYREPORT: 
        {
            MmsDeliveryInd *mmsDelivery;
        
            

 
            mmsDelivery = (MmsDeliveryInd *)M_CALLOC(sizeof(MmsDeliveryInd));
            if ( parseDeliveryReport( WE_MODID_MMS, 
                currentNotif->pdu, currentNotif->len, mmsDelivery) == FALSE)
            {
                
                result = MMS_RESULT_ERROR;
                M_FREE( mmsDelivery);
                mmsDelivery = NULL;
                break;
            }                     

             
            notifyDeliveryReportReceived(mmsDelivery->messageId, mmsDelivery->to, 
                    mmsDelivery->date, mmsDelivery->status,
                    currentNotif->origId, currentNotif->msgId);
                     
            freeMmsDeliveryInd( WE_MODID_MMS, mmsDelivery);
            M_FREE(mmsDelivery);
        }
        break;

     case MMS_SUFFIX_TEMPLATE:
     case MMS_SUFFIX_SEND_REQ:
     case MMS_SUFFIX_INFO:  
     case MMS_SUFFIX_MSG:
     case MMS_SUFFIX_ERROR: 
     default:  
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Internal error wrong pdu type  %d\n", 
                __FILE__, __LINE__, result));
        break;
    } 
    
    
    popNotificationWrite();
} 






MmsResult mmsPromoteNotifToDelayed(WE_UINT32 notifId)
{
    MmsResult res;

    
    res = mmsChangeMsgType(notifId, MMS_SUFFIX_NOTIFICATION);
    if (MMS_RESULT_OK != res)
    {
        return res;
    }

    res = mmsFldrMgrSetFolderQuiet(notifId, MMS_NOTIFY);
        
#if (MMS_FLDRMGR_NOTIF == 1)
    if (MMS_RESULT_OK == res)
    {
        

 
       M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_WRITE_MMT);   
    }
#endif

    return res;
} 











  
static void handleDelayedRetrieval(MmsNotification *mmsNotification, 
    WE_BOOL isSmsBearer, WE_UINT32 len, unsigned char *pdu, MmsVersion version,
    MmsNotificationReason reason)
{   
    if ( getTransactionId(pdu, len) == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): PDU is corrupt X_MMS_TRANSACTION_ID tag\n", __FILE__, __LINE__));

        M_FREE(pdu);
        if (mmsNotification != NULL) 
        {
            freeMmsNotification(WE_MODID_MMS , mmsNotification); 
            M_FREE( mmsNotification);
        }     
        
    }
    else  
    { 
        pushNotificationWrite(isSmsBearer,
            len, pdu, version,
            reason,
            MMS_SUFFIX_NOTIFICATION, MMS_NOTIFY,
            mmsNotification);
    }   
} 










  
static void handleImmediateRetrieval(MmsNotification *mmsNotification, 
    WE_BOOL isSmsBearer, WE_UINT32 len, unsigned char *pdu, MmsVersion version,
    WE_BOOL isRoaming)
{   
    if ( getTransactionId(pdu, len) == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): PDU is corrupt X_MMS_TRANSACTION_ID tag\n", __FILE__, __LINE__));

        M_FREE(pdu);
        if (mmsNotification != NULL) 
        {
            freeMmsNotification(WE_MODID_MMS , mmsNotification); 
            M_FREE( mmsNotification);
        }     
        
    }
    else  
    { 
        if (isRoaming)
        {
            pushNotificationWrite(isSmsBearer,
                len, pdu, version,
                MMS_NOTIFICATION_REASON_NORMAL,
                MMS_SUFFIX_IMMEDIATE, MMS_NOTIFY,
                mmsNotification);
        }
        else
        {
            pushNotificationWrite(isSmsBearer,
                len, pdu, version,
                MMS_NOTIFICATION_REASON_NORMAL,
                MMS_SUFFIX_IMMEDIATE, MMS_HIDDEN,
                mmsNotification);            
        }
    }   
} 










static void handleDeliveryReport(WE_UINT32 len, unsigned char *pdu)
{
    pushNotificationWrite(FALSE, 
        len, pdu, MMS_VERSION_10,
        MMS_NOTIFICATION_REASON_NORMAL,
        MMS_SUFFIX_DELIVERYREPORT, MMS_INBOX,
        NULL);
} 









  
static void handleReadReport(WE_UINT32 len, unsigned char *pdu)
{
    pushNotificationWrite(FALSE, 
        len, pdu, MMS_VERSION_10,
        MMS_NOTIFICATION_REASON_NORMAL,
        MMS_SUFFIX_READREPORT, MMS_INBOX,
        NULL);
} 











static WE_BOOL isImmediateRetrievalAllowed(const MmsNotification *mmsNotification, WE_BOOL isRoaming)
{
    WE_BOOL ret = FALSE;  
    char *tmpStr; 
    int len = 0; 
    char *hostName; 

    if (mmsNotification == NULL)
    {
        return FALSE; 
    } 

    


    if (isRoaming)
    {
        switch ( (MmsRoamingMode)cfgGetInt(MMS_CFG_ROAMING_MODE))
        {
            case MMS_ROAM_DELAYED:
                
                
                return FALSE;
            case MMS_ROAM_DEFAULT:
                
                break;
            case MMS_ROAM_RESTRICTED:
            case MMS_ROAM_REJECT:
                break; 
            default:
                
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Faulty roaming mode set!\n", __FILE__, __LINE__));
                break;
        } 
    }
        
     
    if (mmsNotification->length > (unsigned)cfgGetInt(MMS_CFG_MAX_SIZE_OF_MSG))
    {
        return FALSE;  
    }  

    
    if ((tmpStr = cfgGetStr(MMS_CFG_IMMEDIATE_RETRIEVAL_SERVER)) != NULL && 
        strlen(tmpStr) > 0) 
    {
         
        if ((hostName = we_url_get_host(WE_MODID_MMS, 
            mmsNotification->contentLocation)) == NULL)
        {
            return FALSE; 
        }  
      
        len = we_cmmn_strcmp_nc(tmpStr, hostName); 
        M_FREE(hostName); 
   
        if (len)
        {
            return FALSE; 
        }  
    }      

    
    if ((mmsNotification->from.address == NULL) && !cfgGetInt(MMS_CFG_ANONYMOUS_SENDER))
    {
        return FALSE; 
    }  

     
    switch (mmsNotification->msgClass.classIdentifier) 
    {
    case MMS_MESSAGE_CLASS_PERSONAL:  
        if (cfgGetInt(MMS_CFG_CLASS_PERSONAL))
        {
            ret = TRUE; 
        }  
        break; 
    case MMS_MESSAGE_CLASS_ADVERTISEMENT:  
        if (cfgGetInt(MMS_CFG_CLASS_ADVERTISEMENT))
        {
            ret = TRUE; 
        }  
        break; 
    case MMS_MESSAGE_CLASS_INFORMATIONAL: 
        if (cfgGetInt(MMS_CFG_CLASS_INFORMATIONAL))
        {
            ret = TRUE; 
        }     
        break; 
    case MMS_MESSAGE_CLASS_AUTO:           
        if (cfgGetInt(MMS_CFG_CLASS_AUTO))
        {
            ret = TRUE; 
        }     
        break;  
    case MMS_MESSAGE_CLASS_IS_TEXT:         
        



        if (mmsNotification->msgClass.textString != NULL)
        {
            if (we_cmmn_strcmp_nc(mmsNotification->msgClass.textString, 
                cfgGetStr(MMS_CFG_CLASS_STRING)) == 0) 
            {
                ret = TRUE;
            }  
        }     
        break; 
    case MMS_MESSAGE_CLASS_NOT_SET:        
    default:
        ret = FALSE;   
    }      

    return ret; 
} 







  
static WE_BOOL canBeImmediateRetrieval(const MmsNotification *mmsNotification, WE_BOOL isRoaming)
{
    WE_BOOL isComplete = FALSE;
    
    if (cfgGetInt(MMS_CFG_IMMEDIATE_RETRIEVAL)) 
    { 
        if (isImmediateRetrievalAllowed(mmsNotification, isRoaming))
        {   
            isComplete = TRUE;
        } 
    } 
    
    return isComplete;
} 











void sendNotifyIndRsp( WE_BOOL isSmsBearer, const char *trId, MmsStatus msgStatus,
    MmsClassIdentifier msgClass, MmsVersion version) 
{
    unsigned int len = 0;
    MmsNotifIndInfo *notifIndInfo = NULL;

     
    if (trId == NULL) 
    {
        
       WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
           "%s(%d): No X_MMS_TRANSACTION_ID passed\n", __FILE__, __LINE__));
       return; 
    }  

    notifIndInfo = M_CALLOC(sizeof(MmsNotifIndInfo));

     
    len = strlen(trId) + 1;
    notifIndInfo->transactionId = M_ALLOC(len);
    strcpy( notifIndInfo->transactionId, trId);

    


    if (msgClass == MMS_MESSAGE_CLASS_AUTO) 
    {
        notifIndInfo->deliveryReportFlag = MMS_DELIVERY_REPORT_ALLOWED_NO; 
    } 
    else 
    {
        notifIndInfo->deliveryReportFlag = 
            (MmsDeliveryReportAllowed) cfgGetInt(MMS_CFG_REPORT_ALLOWED);  
    } 

    notifIndInfo->isSmsBearer = isSmsBearer;
    notifIndInfo->msgStatus = msgStatus;
    notifIndInfo->version = version;
    
    M_SIGNAL_SENDTO_P( M_FSM_COH_PUSH_RECEIVE, (int)MMS_SIG_COH_NOTIFY_RESP,
        notifIndInfo); 
} 










static void handleNotification(WE_BOOL isSmsBearer, WE_UINT32 len, unsigned char *pdu)
{
     MmsNotification *mmsNotification;
     MmsVersion version;
     static char *trId = NULL;
     WE_BOOL isRoaming = FALSE;
     MmsResult result;
     
     mmsNotification = (MmsNotification *)M_CALLOC( sizeof(MmsNotification));
     if (NULL == mmsNotification)
     {
        

         WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Memmory allocation failed!\n", __FILE__, __LINE__));

         
         M_FREE(pdu);
         
         return ;
     }

     
     result = parseMmsNotification( WE_MODID_MMS,
                     pdu, len, mmsNotification, &version);
     if (result != MMS_RESULT_OK)
     {
         


         WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Parse Notification failed %d!\n", 
            __FILE__, __LINE__, (int) result));

         if (result != MMS_RESULT_INSUFFICIENT_MEMORY)
         {
             



             trId = getTransactionId(pdu, len);
             if (NULL != trId)
             {
                 sendNotifyIndRsp( isSmsBearer, trId, MMS_STATUS_UNRECOGNIZED,
                     MMS_MESSAGE_CLASS_AUTO, version);
             } 
         } 

         
        freeMmsNotification( WE_MODID_MMS, mmsNotification);
        M_FREE(mmsNotification);
        
        
        M_FREE(pdu);

        return ;
     } 

     isRoaming = (cfgGetInt(MMS_CFG_NETWORK_STATUS_ROAMING) == MMS_ROAMING);
     
     
     trId = getTransactionId(pdu, len);

     


     if (isRoaming && cfgGetInt(MMS_CFG_ROAMING_MODE) == MMS_ROAM_REJECT)
     {
          
         WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
             "%s(%d): Notification rejected in roaming!\n", __FILE__, __LINE__)); 
         
         sendNotifyIndRsp( isSmsBearer, trId, MMS_STATUS_REJECTED,
             mmsNotification->msgClass.classIdentifier, version);
         
         freeMmsNotification( WE_MODID_MMS, mmsNotification);
         M_FREE(mmsNotification);
         
         
         M_FREE(pdu);

         return;
     }
     
     if (cfgGetInt(MMS_CFG_IMMEDIATE_RETRIEVAL) == FALSE ||
         isRoaming && cfgGetInt(MMS_CFG_ROAMING_MODE) == MMS_ROAM_DELAYED)
     {
         handleDelayedRetrieval(mmsNotification, isSmsBearer, len, pdu, version,
             MMS_NOTIFICATION_REASON_NORMAL);         
     }
     else if (canBeImmediateRetrieval(mmsNotification, isRoaming))
     {
         




         if (cfgGetInt(MMS_CFG_MAX_RETRIEVAL_SIZE) > 0 && 
             (mmsNotification->length) > (WE_UINT32)cfgGetInt(MMS_CFG_MAX_RETRIEVAL_SIZE))
         {
             
             handleDelayedRetrieval( mmsNotification, isSmsBearer, len, pdu, version,
                 MMS_NOTIFICATION_REASON_MAX_RETRIEVAL_SIZE_EXCEEDED);
         }
         else
         {
             
             
             


             handleImmediateRetrieval( mmsNotification, isSmsBearer, len, pdu, version, isRoaming);
         }
     }
     else if (cfgGetInt(MMS_CFG_TREAT_AS_DELAYED_RETRIEVAL))
     {
         handleDelayedRetrieval(mmsNotification, isSmsBearer, len, pdu, version,
             MMS_NOTIFICATION_REASON_CONDITION_NOT_FULFILLED);
     }
     else
     {
          
         WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
             "%s(%d): Notification rejected!\n", __FILE__, __LINE__)); 
         
         sendNotifyIndRsp( isSmsBearer, trId, MMS_STATUS_REJECTED,
             mmsNotification->msgClass.classIdentifier, version);
         
         freeMmsNotification( WE_MODID_MMS, mmsNotification);
         M_FREE(mmsNotification);
         
         
         M_FREE(pdu);
     }
} 







static void msrNotifyMain(MmsSignal *sig)
{
	MmsMessageType notifType;  

	switch (sig->type)
    { 
	case MMS_SIG_MSR_NOTIFICATION:
		WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MSR Notify FSM, Received MMS_SIG_MSR_NOTIFICATION \n"));
		 
	      
        notifType = (MmsMessageType)sig->u_param1;
       
        if (fldrGetNoOfFreeMsg() <= 1)   
        { 
            


            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MSR Notify FSM, Disk is full, notification is not handled \n"));
            M_FREE(sig->p_param); 
            mmsNotifyError(MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE); 
        }
        
        else if (notifType == MMS_M_DELIVERY_IND) 
        {
            handleDeliveryReport(sig->u_param2, (unsigned char *)sig->p_param);
        }  
        else if (notifType == MMS_M_NOTIFICATION_IND) 
        {   
            handleNotification( sig->i_param, sig->u_param2, 
                (unsigned char *)sig->p_param);
        }
        else if (notifType == MMS_M_READ_ORIG_IND)
        {
            handleReadReport(sig->u_param2, sig->p_param);
        }
        else   
        {
           
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                  "MSR Notify FSM, Received unsupported notification \n"));  
            M_FREE(sig->p_param);
        } 
	 	break;
    case MMS_SIG_MSR_NOTIFY_FILE_CREATED :
        




        if ((MmsResult)sig->u_param1 == MMS_RESULT_OK)   
        {
            
            currentNotif->msgId = sig->u_param2;

            
            if (FALSE == notificationUpdate())
            {
                
                popNotificationWrite();
            }
            else
            {                
                 
                currentNotif->asyncOper = asyncOperationStart( 
                      Mms_FldrMsgIdToName(currentNotif->msgId, currentNotif->suffix),
                      M_FSM_MSR_NOTIFY, 0, 
                      MMS_SIG_MSR_NOTIFY_ASYNC_WRITE_FINISHED, ASYNC_MODE_WRITE);

                if (currentNotif->asyncOper == NULL)
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                          "%s(%d): No memory. I'll die!! please restart\n", 
                          __FILE__, __LINE__));

                    
                    popNotificationWrite();
                }
                else
                {
                    asyncWrite( currentNotif->asyncOper, 0,
                                currentNotif->pdu, currentNotif->len);
                } 
            } 
        } 
        else 
        {
            
            popNotificationWrite();

             
            mmsNotifyError((MmsResult)sig->u_param1);
        }  

        M_FREE(sig->p_param);
        break;
    case MMS_SIG_MSR_NOTIFY_ASYNC_WRITE_FINISHED :
        



       
        asyncOperationStop(&currentNotif->asyncOper);
        if ((MmsResult)sig->u_param1 != MMS_RESULT_OK)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Async operation failed %d\n", 
                __FILE__, __LINE__, sig->u_param1));
             
            mmsNotifyError((MmsResult)sig->u_param1);
            
            if (currentNotif->msgId > 0)
            {
                if (MMS_RESULT_OK != fldrMgrDeleteMessage(currentNotif->msgId,FALSE))
                {
                    


                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                        "MSR Notify FSM, Couldn't remove file\n"));
                }
            }
        } 
        
#if (MMS_FLDRMGR_NOTIF == 1)
        

 
        M_SIGNAL_SENDTO( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_NOTIF_WRITE_MMT);         
#else
        


        notificationFinished((MmsResult) sig->u_param1);
#endif
        break;
 
    case MMS_SIG_MSR_NOTIFY_MMT_RSP:
        


        notificationFinished((MmsResult) sig->u_param1);
        break;
	default :
		 WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
             "MSR Notify FSM, Error: Received unsupported sig \n"));
		break;
	} 

    mSignalDelete(sig);
} 

