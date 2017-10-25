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
#include "We_Def.h"     
#include "We_Core.h"    
#include "We_Dcvt.h"    
#include "We_Cfg.h"    
#include "We_Log.h"     
#include "We_Cfg.h"     
#include "We_Errc.h"   

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_Env.h"    
#include "Mms_If.h"     

#include "Mms_Int.h"    
#include "Mmsrpl.h"     

#include "Msig.h"       
#include "Fldmgr.h"     

#include "Mmsui.h"      

#define MMS_DUMMY_APPL 42











 












void replyDeleteMessageResponse(WE_UINT8 reciever, MmsResult result)
{   
    MmsResultSig data;

    WE_LOG_SIG_BEGIN("MMS_SIG_DELETE_MSG_REPLY", LS_SEND, WE_MODID_MMS, reciever);
    WE_LOG_SIG_UINT32("Result",result);
    WE_LOG_SIG_END();

    if (reciever == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): No receiving module.\n", __FILE__, __LINE__));
        return;  
    }  

    data.result = result;
    if ( !mmsSendSignalExt( WE_MODID_MMS, reciever, MMS_SIG_DELETE_MSG_REPLY,
        &data, (MmsIfConvertFunction *)mms_cvt_MmsResult))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } 
} 






void replyCreateMessageResponse(WE_UINT8 reciever, MmsResult result, MmsMsgId msgId)								
{
	MmsMsgReplySig data;

    WE_LOG_SIG_BEGIN("MMS_SIG_CREATE_MSG_REPLY", LS_SEND, WE_MODID_MMS, reciever);
    WE_LOG_SIG_UINT32("Result",result);
    WE_LOG_SIG_UINT32("MsgId",msgId);
    WE_LOG_SIG_END();

    if (reciever == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): No sender module.\n", __FILE__, __LINE__));
        return;  
    }  

    data.result = result;
    data.msgId = msgId;
    if ( !mmsSendSignalExt( WE_MODID_MMS, reciever, MMS_SIG_CREATE_MSG_REPLY,
        &data, (MmsIfConvertFunction *)mms_cvt_MmsMsgReply))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } 
}  






void replyCreateMessageDoneResponse(WE_UINT8 reciever, MmsResult result, MmsMsgId msgId)
{
	MmsMsgReplySig data;

    WE_LOG_SIG_BEGIN("MMS_SIG_CREATE_MSG_DONE_REPLY", LS_SEND, WE_MODID_MMS, reciever);
    WE_LOG_SIG_UINT32("Result",result);
    WE_LOG_SIG_UINT32("MsgId",msgId);
    WE_LOG_SIG_END();
    
    if (reciever == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): No sender module.\n", __FILE__, __LINE__));
        return;  
    }  
    
    data.result = result;
    data.msgId = msgId;
    if ( !mmsSendSignalExt( WE_MODID_MMS, reciever, MMS_SIG_MSG_DONE_REPLY,
        &data, (MmsIfConvertFunction *)mms_cvt_MmsMsgReply))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } 
}  






void replyMoveMessageResponse(WE_UINT8 reciever, MmsResult result)
{
    MmsResultSig data;

    WE_LOG_SIG_BEGIN("MMS_SIG_MOV_MSG_REPLY", LS_SEND, WE_MODID_MMS, reciever);
    WE_LOG_SIG_UINT32("Result",result);
    WE_LOG_SIG_END();

    if (reciever == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): No sender module.\n", __FILE__, __LINE__));
        return;  
    }  

    data.result = result;
    if ( !mmsSendSignalExt( WE_MODID_MMS, reciever, MMS_SIG_MOV_MSG_REPLY, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsResult))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } 
} 






void replyGetNumberOfMessagesResponse(WE_UINT8 reciever, MmsResult result, WE_UINT32 number, 
    MmsFolderType folder)
{
    MmsMsgReplyNoOfMsg data;

    WE_LOG_SIG_BEGIN("MMS_SIG_GET_NUM_OF_MSG_REPLY", LS_SEND, WE_MODID_MMS, reciever);
    WE_LOG_SIG_UINT32("Result",result);
    WE_LOG_SIG_UINT32("Number",number);
    WE_LOG_SIG_UINT32("Folder",folder);
    WE_LOG_SIG_END();

    if (reciever == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): No sender module.\n", __FILE__, __LINE__));
        return;  
    }  

    data.result = result;
    data.number = number;
    data.folder = folder; 
    if ( !mmsSendSignalExt( WE_MODID_MMS, reciever, MMS_SIG_GET_NUM_OF_MSG_REPLY,
        &data, (MmsIfConvertFunction *)mms_cvt_MmsMsgReplyNoOfMsg))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } 
} 






void replyNotificationReceived(MmsNotificationType type, MmsMsgId notifId,
                               MmsAddress *from, MmsEncodedText *subject, WE_UINT32 size,
                               MmsNotificationReason reason
                               )
{
    MmsMsgReplyNotification data;
    WE_UINT32 numOfMsg = 0;
    WE_UINT32 numOfUnreadMsg = 0;
    WE_UINT32 numOfNotif = 0;
    WE_UINT32 numOfUnreadNotif = 0;
    WE_UINT32 numOfRR = 0;
    WE_UINT32 numOfUnreadRR = 0;
    WE_UINT32 numOfDR = 0;
    WE_UINT32 numOfUnreadDR = 0;
    
    


    if (type == MMS_NOTIFICATION_NORMAL) 
    {           
         
        fldrGetInboxStatus(&numOfMsg, &numOfUnreadMsg,
            &numOfNotif, &numOfUnreadNotif,
            &numOfRR, &numOfUnreadRR,
            &numOfDR, &numOfUnreadDR);                        
        MMSa_newMessage( MMS_SUFFIX_NOTIFICATION, notifId, from, subject, size,
            numOfMsg, numOfUnreadMsg,
            numOfNotif, numOfUnreadNotif,
            numOfRR, numOfUnreadRR,
            numOfDR, numOfUnreadDR,
            reason); 
#ifdef MMS_UBS_IN_USE
        mmsNotifyUBSMsgNew(notifId);
#endif
    }
    else if (type == MMS_NOTIFICATION_IMMEDIATE ) 
    {           
         
        fldrGetInboxStatus(&numOfMsg, &numOfUnreadMsg,
            &numOfNotif, &numOfUnreadNotif,
            &numOfRR, &numOfUnreadRR,
            &numOfDR, &numOfUnreadDR);                        
        MMSa_newMessage( MMS_SUFFIX_IMMEDIATE, notifId, from, subject, size,
            numOfMsg, numOfUnreadMsg,
            numOfNotif, numOfUnreadNotif,
            numOfRR, numOfUnreadRR,
            numOfDR, numOfUnreadDR,
            reason); 
#ifdef MMS_UBS_IN_USE
        mmsNotifyUBSMsgNew(notifId);
#endif
    }
    else if (type ==  MMS_NOTIFICATION_MESSAGE )
    {
         
        fldrGetInboxStatus(&numOfMsg, &numOfUnreadMsg,
            &numOfNotif, &numOfUnreadNotif,
            &numOfRR, &numOfUnreadRR,
            &numOfDR, &numOfUnreadDR);                        
        MMSa_newMessage( MMS_SUFFIX_MSG, notifId, from, subject, size,
            numOfMsg, numOfUnreadMsg,
            numOfNotif, numOfUnreadNotif,
            numOfRR, numOfUnreadRR,
            numOfDR, numOfUnreadDR,
            reason);
#ifdef MMS_UBS_IN_USE
        mmsNotifyUBSMsgNew(notifId);
#endif        
    } 

    memset( &data, 0, sizeof(data));
    data.type = type;
    data.notifId = notifId;
    data.size = size;
    if (from != NULL)
    {
        data.fromCharset = from->name.charset;
        data.fromName = from->name.text;
        data.fromAddress = from->address;
        data.fromAddrType = from->addrType;
    } 

    if (subject != NULL)
    {
        data.subjectCharset = subject->charset;
        data.subjectText = subject->text;
    } 

    mmsSendApplSubscription( WE_MODID_MMS, MMS_SIG_NOTIFICATION,
        &data, (MmsIfConvertFunction *)mms_cvt_MmsMsgNotificationMsg);
} 






void notifyReadReportIndication(MmsReadOrigInd *mmsOrigInd, WE_UINT32 msgId,
                               WE_UINT32 msgIdFile)
{
    WE_UINT32 numOfMsg = 0;
    WE_UINT32 numOfUnreadMsg = 0;
    WE_UINT32 numOfNotif = 0;
    WE_UINT32 numOfUnreadNotif = 0;
    WE_UINT32 numOfRR = 0;
    WE_UINT32 numOfUnreadRR = 0;
    WE_UINT32 numOfDR = 0;
    WE_UINT32 numOfUnreadDR = 0;
    
    MmsMsgReadReport data;

    WE_LOG_SIG_BEGIN("MMS_SIG_NOTIFICATION", LS_SEND, WE_MODID_MMS, MMS_DUMMY_APPL);
    WE_LOG_SIG_STR("Address",mmsOrigInd->from.address);
    WE_LOG_SIG_UINT32("AddressType",mmsOrigInd->from.addrType);
    WE_LOG_SIG_UINT32("Charset",mmsOrigInd->from.name.charset);
    WE_LOG_SIG_STR("Text",mmsOrigInd->from.name.text);
    WE_LOG_SIG_UINT32("Date",mmsOrigInd->date);
    WE_LOG_SIG_UINT32("ServerMsgId",mmsOrigInd->serverMessageId);
    WE_LOG_SIG_UINT32("ReadStatus",mmsOrigInd->readStatus);
    WE_LOG_SIG_UINT32("Originated MsgId",msgId);
    WE_LOG_SIG_UINT32("MsgId",msgIdFile);
    WE_LOG_SIG_END();


     
    fldrGetInboxStatus(&numOfMsg, &numOfUnreadMsg,
        &numOfNotif, &numOfUnreadNotif,
        &numOfRR, &numOfUnreadRR,
        &numOfDR, &numOfUnreadDR);
    
    MMSa_newMessage( MMS_SUFFIX_READREPORT, msgIdFile,
        &mmsOrigInd->from, NULL, 0,
        numOfMsg, numOfUnreadMsg,
        numOfNotif, numOfUnreadNotif,
        numOfRR, numOfUnreadRR,
        numOfDR, numOfUnreadDR,
        MMS_NOTIFICATION_REASON_NORMAL);
#ifdef MMS_UBS_IN_USE
    mmsNotifyUBSMsgNew(msgIdFile);
#endif        

    data.address = mmsOrigInd->from.address;
    data.addrType = mmsOrigInd->from.addrType;
    data.charset = mmsOrigInd->from.name.charset;
    data.text = mmsOrigInd->from.name.text;
    data.date = mmsOrigInd->date;
    data.serverMessageId = mmsOrigInd->serverMessageId;
    data.readStatus = mmsOrigInd->readStatus;
    data.msgId = msgId;
    data.msgIdFile = msgIdFile;
    
    mmsSendApplSubscription( WE_MODID_MMS, MMS_SIG_READ_REPORT_IND,
        &data, (MmsIfConvertFunction *)mms_cvt_MmsMsgReadReport);
} 






void replyReadReportIndication(MmsReadOrigInd *mmsOrigInd, WE_UINT32 msgId,
                               WE_UINT32 msgIdFile)
{   
    MmsMsgReadReport data;

    WE_LOG_SIG_BEGIN("MMS_SIG_NOTIFICATION", LS_SEND, WE_MODID_MMS, MMS_DUMMY_APPL);
    WE_LOG_SIG_STR("Address",mmsOrigInd->from.address);
    WE_LOG_SIG_UINT32("AddressType",mmsOrigInd->from.addrType);
    WE_LOG_SIG_UINT32("Charset",mmsOrigInd->from.name.charset);
    WE_LOG_SIG_STR("Text",mmsOrigInd->from.name.text);
    WE_LOG_SIG_UINT32("Date",mmsOrigInd->date);
    WE_LOG_SIG_UINT32("ServerMsgId",mmsOrigInd->serverMessageId);
    WE_LOG_SIG_UINT32("ReadStatus",mmsOrigInd->readStatus);
    WE_LOG_SIG_UINT32("Originated MsgId",msgId);
    WE_LOG_SIG_UINT32("MsgId",msgIdFile);
    WE_LOG_SIG_END();
    
    data.address = mmsOrigInd->from.address;
    data.addrType = mmsOrigInd->from.addrType;
    data.charset = mmsOrigInd->from.name.charset;
    data.text = mmsOrigInd->from.name.text;
    data.date = mmsOrigInd->date;
    data.serverMessageId = mmsOrigInd->serverMessageId;
    data.readStatus = mmsOrigInd->readStatus;
    data.msgId = msgId;
    data.msgIdFile = msgIdFile;
    
    mmsSendApplSubscription( WE_MODID_MMS, MMS_SIG_READ_REPORT_IND,
        &data, (MmsIfConvertFunction *)mms_cvt_MmsMsgReadReport);
} 







void notifyDeliveryReportReceived(char *serverMsgId, MmsAddress *to, 
    MmsTimeSec date, MmsStatus status, WE_UINT32 msgId, WE_UINT32 msgIdFile)
{
    WE_UINT32 numOfMsg = 0;
    WE_UINT32 numOfUnreadMsg = 0;
    WE_UINT32 numOfNotif = 0;
    WE_UINT32 numOfUnreadNotif = 0;
    WE_UINT32 numOfRR = 0;
    WE_UINT32 numOfUnreadRR = 0;
    WE_UINT32 numOfDR = 0;
    WE_UINT32 numOfUnreadDR = 0;
    
    MmsMsgDeliveryReportInd data;

    WE_LOG_SIG_BEGIN("MMS_SIG_NOTIFICATION", LS_SEND, WE_MODID_MMS, MMS_DUMMY_APPL);
    WE_LOG_SIG_UINT32("serverMsgId", serverMsgId);
    WE_LOG_SIG_STR("Address", to->address);
    WE_LOG_SIG_UINT32("AddressType",to->addrType);
    WE_LOG_SIG_UINT32("Charset",to->name.charset);
    WE_LOG_SIG_STR("Text",to->name.text);
    WE_LOG_SIG_UINT32("Date",date);
    WE_LOG_SIG_UINT32("Status",status);
    WE_LOG_SIG_UINT32("Orig message Id:", msgId); 
    WE_LOG_SIG_UINT32("MsgId:", msgIdFile);
    WE_LOG_SIG_END();

    
    fldrGetInboxStatus(&numOfMsg, &numOfUnreadMsg,
        &numOfNotif, &numOfUnreadNotif,
        &numOfRR, &numOfUnreadRR,
        &numOfDR, &numOfUnreadDR);
    MMSa_newMessage(MMS_SUFFIX_DELIVERYREPORT,
        msgIdFile,
        to, 
        NULL,
        0,
        numOfMsg, numOfUnreadMsg,
        numOfNotif, numOfUnreadNotif,
        numOfRR, numOfUnreadRR,
        numOfDR, numOfUnreadDR,
        MMS_NOTIFICATION_REASON_NORMAL);
#ifdef MMS_UBS_IN_USE
    mmsNotifyUBSMsgNew(msgIdFile);
#endif    

    data.serverMsgId = serverMsgId; 
    data.address = to->address;
    data.addrType = to->addrType;
    data.charset = to->name.charset;
    data.text = to->name.text;
    data.date = date;
    data.status = status;
    data.msgId = msgId;   
    data.msgId = msgId;  
    data.msgIdFile = msgIdFile;
    
    mmsSendApplSubscription( WE_MODID_MMS, MMS_SIG_DELIVERY_REPORT,
        &data, (MmsIfConvertFunction *)mms_cvt_MmsMsgDeliveryReport);
} 






void replyDeliveryReportReceived(char *serverMsgId, MmsAddress *to, 
    MmsTimeSec date, MmsStatus status, WE_UINT32 msgId, WE_UINT32 msgIdFile)
{    
    MmsMsgDeliveryReportInd data;

    WE_LOG_SIG_BEGIN("MMS_SIG_NOTIFICATION", LS_SEND, WE_MODID_MMS, MMS_DUMMY_APPL);
    WE_LOG_SIG_UINT32("serverMsgId", serverMsgId);
    WE_LOG_SIG_STR("Address", to->address);
    WE_LOG_SIG_UINT32("AddressType",to->addrType);
    WE_LOG_SIG_UINT32("Charset",to->name.charset);
    WE_LOG_SIG_STR("Text",to->name.text);
    WE_LOG_SIG_UINT32("Date",date);
    WE_LOG_SIG_UINT32("Status",status);
    WE_LOG_SIG_UINT32("Orig message Id:", msgId); 
    WE_LOG_SIG_UINT32("MsgId:", msgIdFile);
    WE_LOG_SIG_END();

    data.serverMsgId = serverMsgId; 
    data.address = to->address;
    data.addrType = to->addrType;
    data.charset = to->name.charset;
    data.text = to->name.text;
    data.date = date;
    data.status = status;
    data.msgId = msgId;   
    data.msgId = msgId;  
    data.msgIdFile = msgIdFile;
    
    mmsSendApplSubscription( WE_MODID_MMS, MMS_SIG_DELIVERY_REPORT,
        &data, (MmsIfConvertFunction *)mms_cvt_MmsMsgDeliveryReport);
} 






void replyRetrieveMessageResponse(WE_UINT8 reciever, MmsResult result, MmsMsgId msgId)
{
	MmsMsgReplySig data;

    WE_LOG_SIG_BEGIN("MMS_SIG_RETRIEVE_MSG_REPLY", LS_SEND, WE_MODID_MMS, reciever);
    WE_LOG_SIG_UINT32("Result",result);
    WE_LOG_SIG_UINT32("MsgId",msgId);
    WE_LOG_SIG_END();

    if (result == MMS_RESULT_OK)
    {
#ifdef MMS_UBS_IN_USE
        mmsNotifyUBSMsgNew(msgId);
#endif
    }

    if (reciever == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): No sender module.\n", __FILE__, __LINE__));
        return;  
    }  

    data.result = result;
    data.msgId = msgId;

    if ( !mmsSendSignalExt( WE_MODID_MMS, reciever, MMS_SIG_RETRIEVE_MSG_REPLY,
        &data, (MmsIfConvertFunction *)mms_cvt_MmsMsgReply))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } 
} 






void replySendMessageResponse(WE_UINT8 reciever, MmsResult result)
{
    MmsResultSig data;

    WE_LOG_SIG_BEGIN("MMS_SIG_SEND_MSG_REPLY", LS_SEND, WE_MODID_MMS, reciever);
    WE_LOG_SIG_UINT32("Result",result);
    WE_LOG_SIG_END();

    if (reciever == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): No sender module.\n", __FILE__, __LINE__));
        return;  
    }  

    data.result = result;
    if ( !mmsSendSignalExt( WE_MODID_MMS, reciever, MMS_SIG_SEND_MSG_REPLY, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsResult))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } 
} 






void replySendReadReportResponse(WE_UINT8 reciever, MmsResult result, MmsMsgId rrId, MmsMsgId msgId)
{
    MmsRRReplySig data;

    WE_LOG_SIG_BEGIN("MMS_SIG_SEND_RR_REPLY", LS_SEND, WE_MODID_MMS, reciever);
    WE_LOG_SIG_UINT32("Result",result);
    WE_LOG_SIG_UINT32("rrId",rrId);
    WE_LOG_SIG_UINT32("msgId",msgId);
    WE_LOG_SIG_END();

    if (reciever == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): No sender module.\n", __FILE__, __LINE__));
        return;  
    }  

    data.result = result;
    data.rrId = rrId;
    data.msgId = msgId;
    if ( !mmsSendSignalExt( WE_MODID_MMS, reciever, MMS_SIG_SEND_RR_REPLY, &data, 
        (MmsIfConvertFunction *)mms_cvt_MmsRRReplySig))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } 
} 






void mmsNotifyError(MmsResult result)
{
    MmsResultSig data;

    WE_LOG_SIG_BEGIN("MMS_SIG_ERROR", LS_SEND, WE_MODID_MMS, MMS_DUMMY_APPL);
    WE_LOG_SIG_UINT32("Result",result);
    WE_LOG_SIG_END();

    



    if (mmsNoOfRegisteredApps() > 0)
    {
        data.result = result;
        mmsSendApplSubscription( WE_MODID_MMS, MMS_SIG_ERROR, &data, 
            (MmsIfConvertFunction *)mms_cvt_MmsResult);
    }
    else
    {
        MMSa_error(result);
    }
} 






void replyForwardReqResponse(WE_UINT8 reciever, MmsResult result)
{
    MmsResultSig data;

    WE_LOG_SIG_BEGIN("MMS_SIG_FORWARD_REQ_REPLY", LS_SEND, WE_MODID_MMS, reciever);
    WE_LOG_SIG_UINT32("Result",result);
    WE_LOG_SIG_END();
    
    if (reciever == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): No sender module.\n", __FILE__, __LINE__));
        return;  
    }  

    data.result = result;
    if ( !mmsSendSignalExt( WE_MODID_MMS, reciever, MMS_SIG_FORWARD_REQ_REPLY, 
        &data, (MmsIfConvertFunction *)mms_cvt_MmsResult))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): signal send failed.\n", __FILE__, __LINE__));
    } 
} 






void replyFolderContent(WE_UINT8 reciever, MmsListOfProperties *properties)
{
    we_dcvt_t cvtObj;   
    WE_UINT16 length;     
    void *signalBuffer;  
    void *userBuffer;   

    if (reciever == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): No sender module.\n", __FILE__, __LINE__));
        return;  
    }  
    
        
    we_dcvt_init(&cvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);

    if ( !mms_cvt_MmsListOfProperties(&cvtObj, properties))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", 
            __FILE__, __LINE__));
        return;
    } 

    length = (WE_UINT16)cvtObj.pos;
    
    signalBuffer = WE_SIGNAL_CREATE( MMS_SIG_GET_FLDR_REPLY, 
        WE_MODID_MMS, reciever, length); 

    if (signalBuffer == NULL) 
    {
        WE_ERROR( WE_MODID_MMS, WE_ERR_SYSTEM_FATAL);
    }
    else
    {
        userBuffer = WE_SIGNAL_GET_USER_DATA (signalBuffer, &length);
        we_dcvt_init(&cvtObj, WE_DCVT_ENCODE, userBuffer, length, 
            WE_MODID_MMS); 
        if ( !mms_cvt_MmsListOfProperties(&cvtObj, properties))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", 
                __FILE__, __LINE__));
            
            WE_SIGNAL_DESTRUCT( WE_MODID_MMS, MMS_SIG_GET_FLDR_REPLY, 
                signalBuffer);
        }
        else
        {
            WE_SIGNAL_SEND(signalBuffer); 
        } 
    }  
} 






void replyMsgInfo(WE_UINT8 reciever, MmsListOfProperties *ptr)
{
    we_dcvt_t cvtObj;   
    WE_UINT16 length;     
    void *signalBuffer;  
    void *userBuffer;   
    
    MmsListOfProperties* properties = (MmsListOfProperties*)ptr; 

    if (reciever == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): No sender module.\n", __FILE__, __LINE__));
        return;  
    }  
    
        
    we_dcvt_init(&cvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    
    if ( !mms_cvt_MmsListOfProperties(&cvtObj, properties))
    {
        WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
            "%s(%d): Conversion failed.\n", 
            __FILE__, __LINE__));
        return;
    } 

    length = (WE_UINT16)cvtObj.pos;
    
    signalBuffer = WE_SIGNAL_CREATE( MMS_SIG_GET_MSG_INFO_REPLY, 
        WE_MODID_MMS, reciever, length); 

    if (signalBuffer == NULL) 
    {
        WE_ERROR( WE_MODID_MMS, WE_ERR_SYSTEM_FATAL);
    }
    else
    {
        userBuffer = WE_SIGNAL_GET_USER_DATA (signalBuffer, &length);
        we_dcvt_init(&cvtObj, WE_DCVT_ENCODE, userBuffer, length, 
            WE_MODID_MMS); 
        if ( !mms_cvt_MmsListOfProperties(&cvtObj, properties))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): Conversion failed.\n", 
                __FILE__, __LINE__));
            
            WE_SIGNAL_DESTRUCT( WE_MODID_MMS, MMS_SIG_GET_MSG_INFO_REPLY, 
                signalBuffer);
        }
        else
        {
            WE_SIGNAL_SEND(signalBuffer); 
        } 
    }  
} 






void replyProgressStatus(WE_UINT8 operation, WE_UINT8 state,
    MmsMsgId msgId,
    WE_UINT32 progress, WE_UINT32 totalLength,
    WE_UINT32 *lastTime)
{
    MmsProgressStatus data;
    WE_UINT8 queueLength = 0;
    WE_UINT32 time;
    WE_UINT32 threshold = 1;

    WE_LOG_SIG_BEGIN("MMS_SIG_PROGRESS_STATUS_REPLY", LS_SEND, WE_MODID_MMS, MMS_DUMMY_APPL);
    WE_LOG_SIG_UINT32("MsgId", msgId);
    WE_LOG_SIG_UINT32("Progress", progress);
    WE_LOG_SIG_UINT32("Total", totalLength);
    WE_LOG_SIG_END();

    
    if (progress>totalLength)
    {
        totalLength = progress;
    }

    


    time = WE_TIME_GET_CURRENT();
    if (state == MMS_PROGRESS_STATUS_PROGRESS && time < (*lastTime)+threshold)
    {
        return ;
    }
    *lastTime = time;

    
    queueLength = (WE_UINT8) mmsCountNotifInQueue();


    memset( &data, 0, sizeof(data));
    data.operation = operation;
    data.state = state;
    data.msgId = msgId;
    data.progress = progress;
    data.totalLength = totalLength;
    data.queueLength = queueLength;

    
    MMSa_progressStatus(&data);
    
    mmsSendApplSubscription( WE_MODID_MMS, MMS_SIG_PROGRESS_STATUS_REPLY,
        &data, (MmsIfConvertFunction *)mms_cvt_MmsProgressStatus);
} 























