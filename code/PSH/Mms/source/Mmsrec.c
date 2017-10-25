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
#include "We_Errc.h"   
#include "We_Cmmn.h"    

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_If.h"     

#include "Mmsrpl.h"     

#include "Msig.h"       
#include "Mmem.h"       

#include "Msend.h"      
#include "Mreceive.h"   
#include "Mcnotif.h"    
#include "Mconfig.h"    
#include "Fldmgr.h"     
#include "Mhandler.h"   
#include "Mms_Cont.h"   
#include "Mms_If.h"     





void recContentRouting(const we_content_data_t* incoming)
{
    we_content_data_t* pOurContent;

    if ( !incoming)
    {
        return;
    } 

    pOurContent = M_ALLOCTYPE(we_content_data_t);

    memset(pOurContent, 0, sizeof(we_content_data_t));
    if (incoming->routingIdentifier)
    {
        pOurContent->routingIdentifier = M_ALLOC(sizeof(char) * 
            (strlen(incoming->routingIdentifier) + 1));
        strcpy(pOurContent->routingIdentifier, incoming->routingIdentifier);
    } 

    if (incoming->contentUrl)
    {
        pOurContent->contentUrl = M_ALLOC(sizeof(char) * 
            (strlen(incoming->contentUrl) + 1));
        strcpy(pOurContent->contentUrl, incoming->contentUrl);
    } 

    if (incoming->contentSource)
    {
        pOurContent->contentSource = M_ALLOC(sizeof(char) *
            (strlen(incoming->contentSource) + 1));
        strcpy(pOurContent->contentSource, incoming->contentSource);
    } 

    if (incoming->contentDataType == WE_CONTENT_DATA_RAW &&
        incoming->contentData != NULL)
    {
        pOurContent->contentData = M_ALLOC(incoming->contentDataLength);
        memcpy( pOurContent->contentData, incoming->contentData, 
            incoming->contentDataLength);
    } 

    pOurContent->routingFormat = incoming->routingFormat;
    pOurContent->contentDataType = incoming->contentDataType;
    pOurContent->contentDataLength = incoming->contentDataLength;

    contQueue(pOurContent);
        
} 






void recCreateMessage(WE_UINT8 orderer, MmsFileType fileType)
{
    if (fileType == 0)
    {
        fileType = MMS_SUFFIX_SEND_REQ;
    }

    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recCreateMessage(void)\n"));

    M_SIGNAL_SENDTO_IU( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_CREATE_MSG, fileType, orderer);
} 







void recCreateMessageDone(WE_UINT8 source, MmsMsgId msgId, MmsFolderType folder)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recCreateMessageDone(%d)\n", msgId));
    
    M_SIGNAL_SENDTO_IUU( M_FSM_MMH_HANDLER, (int)M_FSM_MMH_CREATE_INFO_LIST, (long) source, msgId, folder);
} 






void recDeleteMessage(WE_UINT8 orderer, MmsMsgId msgId)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recDeleteMessage(%d)\n", msgId));

    M_SIGNAL_SENDTO_UU( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_DELETE_MSG, msgId, orderer);
} 






void recForwardReq(WE_UINT8 source, const unsigned char *pdu, WE_UINT32 length)
{   
    unsigned char* fsmPdu; 

    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recForwardReq - sending forward req called\n"));
  
    if (pdu == NULL || length == 0) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d) Resulting message empty\n", __FILE__, __LINE__));
         
        replyForwardReqResponse(source, MMS_RESULT_MESSAGE_EMPTY); 
        return; 
    }  
   
    

 
    fsmPdu = M_CALLOC(length); 
    
    if (fsmPdu == NULL) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d) Out of memory\n", __FILE__, __LINE__));
         
        mmsNotifyError(MMS_RESULT_INSUFFICIENT_MEMORY);
        return;
    }
    
    memcpy(fsmPdu, pdu, length); 

    
    M_SIGNAL_SENDTO_UUP(M_FSM_MSR_SEND, (int)MMS_SIG_MSR_SEND_FORWARD, 
        source, length, fsmPdu);
} 






void recGetMessageIdList(WE_UINT8 orderer, MmsFolderType folder, WE_UINT32 startPos, 
    WE_UINT32 maxIds)
{
    WE_UINT32 module = orderer;

    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recGetMessageIdList(%d,%d,%d)\n", folder, startPos, maxIds));

    M_SIGNAL_SENDTO_IUUP(M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_GET_LIST,
                folder, startPos, maxIds, (void*) module);
} 







void recGetMessageInfo(WE_UINT8 source, WE_UINT32 msgId)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recGetMessageInfo(%d)\n", msgId));

    M_SIGNAL_SENDTO_UU(M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_GET_MSG_INFO,
                        msgId, source);
} 






void recGetNumberOfMessages(WE_UINT8 orderer, MmsFolderType folder)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recGetNumberOfMessages(%d)\n", folder));

    M_SIGNAL_SENDTO_IU(M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_GET_NO_MSG, folder, orderer);
} 






void recMoveMessage(WE_UINT8 orderer, MmsMsgId msgId, MmsFolderType toFolder)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recMoveMessage(%d,%d)\n", msgId, toFolder));

    M_SIGNAL_SENDTO_IUU( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_MOVE_MSG,toFolder, msgId, orderer);
} 






void recPush( WE_BOOL isSmsBearer, unsigned long size, void *pdu)
{
    void *myPdu;

    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recPush( %ld, <data>)\n", size));

    if (size == 0 || pdu == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "MMSc_push: No data received, size %d, ptr %d .\n", size, pdu));
        return; 
    } 

    myPdu = M_ALLOC( size);
    if (myPdu == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d) Out of memory\n", __FILE__, __LINE__));
        mmsNotifyError(MMS_RESULT_INSUFFICIENT_MEMORY);
        return;
    } 

    memcpy( myPdu, pdu, size);

    M_SIGNAL_SENDTO_IUP( M_FSM_COH_PUSH_RECEIVE, (int)MMS_SIG_COH_PUSH, 
        isSmsBearer, size, myPdu);
} 


void recRegResponse(we_registry_response_t* p)
{
    MmsRegistryEntry*       pFirst = NULL;
    MmsRegistryEntry*       pCurr  = NULL;
    we_registry_param_t    param;
    int                     hasMore;

    hasMore = WE_REGISTRY_RESPONSE_GET_NEXT (p, &param);

    if (hasMore)
    {
        pFirst = M_ALLOCTYPE(MmsRegistryEntry);
        pCurr = pFirst;

        
        while (hasMore) 
        {
            memset(pCurr, 0, sizeof(MmsRegistryEntry));

            if (param.path)
            {
                pCurr->path = M_ALLOC(sizeof(char) * (strlen(param.path) + 1));
                strcpy(pCurr->path, param.path);
            }

            if (param.key)
            {
                pCurr->key = M_ALLOC(sizeof(char) * (strlen(param.key) + 1));
                strcpy(pCurr->key, param.key);
            }

            if (param.value_bv_length > 0)
            {
                pCurr->value_bv = M_ALLOC(param.value_bv_length);
                memcpy(pCurr->value_bv, param.value_bv, param.value_bv_length);
            }

            pCurr->type             = param.type;
            pCurr->value_i          = param.value_i;
            pCurr->value_bv_length  = param.value_bv_length;
            pCurr->next             = NULL;

            hasMore = WE_REGISTRY_RESPONSE_GET_NEXT (p, &param);

            if (hasMore)
            {
                pCurr->next = M_ALLOCTYPE(MmsRegistryEntry);
                pCurr = pCurr->next;
            }
        } 
    }
    
    



    M_SIGNAL_SENDTO_P(M_FSM_CORE_CONFIG, (int)MMS_SIG_CORE_REG_RESPONSE, 
                            pFirst);
} 







void recRetrieveCancel(MmsMsgId notifId)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recRetrieveCancel(%d)\n", notifId));

    M_SIGNAL_SENDTO_U( M_FSM_MSR_RECEIVE, (int)MMS_SIG_MSR_RECEIVE_CANCEL, notifId);
} 






void recRetrieveMessage(WE_UINT8 orderer, MmsMsgId msgId)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recRetrieveMessage(%d)\n", msgId));

    if (fldrGetNoOfFreeMsg() == 0)   
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
              "The disk is full, retrieve op. is not handled \n"));
        replyRetrieveMessageResponse(orderer, MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE,0);
        return;       
    } 

    M_SIGNAL_SENDTO_UU( M_FSM_MSR_RECEIVE, (int)MMS_SIG_MSR_RECEIVE_DELAYED, 
                msgId, orderer);
} 







void recSendCancel(MmsMsgId msgId)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recSendCancel(%d)\n", msgId));

    M_SIGNAL_SENDTO_U( M_FSM_MSR_SEND, (int)MMS_SIG_MSR_SEND_CANCEL, msgId);
} 






void recSendMessage(WE_UINT8 source, MmsMsgId msgId)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recSendMessage(%d)\n", msgId));
    
    M_SIGNAL_SENDTO_UU( M_FSM_MSR_SEND, (int)MMS_SIG_MSR_SEND_MSG, msgId, source);
} 






void recSendReadReport(WE_UINT8 source, MmsMsgId msgId, MmsReadStatus readStatus)
{
    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recSendReadReport(%d)\n", msgId));
    
    M_SIGNAL_SENDTO_IUU( M_FSM_MSR_SEND, (int)MMS_SIG_MSR_CREATE_RR, 
        readStatus, msgId, source);
} 





void recSetReadMark(MmsMsgId msgId, WE_UINT8 value)
{
   fldrMgrSetReadMark(msgId, value);
} 






void recGetReport(MmsMsgId msgId)
{
    MmsFileType fileType = getMsgType(msgId); 

    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "recGetReport(%d)\n", msgId));
    
    if ((fileType !=  MMS_SUFFIX_READREPORT) && 
        (fileType !=  MMS_SUFFIX_DELIVERYREPORT)) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d) Wrong file type for get report\n", __FILE__, __LINE__));
        sendReportWithError(fileType, MMS_RESULT_WRONG_FILE_TYPE);
        return;
    } 

    M_SIGNAL_SENDTO_IUU( M_FSM_MMH_HANDLER, (int)M_FSM_MMH_GET_HEADER, \
        M_FSM_MMH_HANDLER, msgId, M_FSM_MMH_GOT_REPORT);   
} 







void getFolderStatus (WE_UINT8 source, WE_UINT32 folderMask)
{   
    WE_UINT32 usedMsgBytes;
    WE_UINT32 numOfMsg;
    WE_UINT32 totalFolderSize;
    MmsFolderStatus data;
    we_dcvt_t cvtObj;   
    WE_UINT16 length;     
    void *signalBuffer;  
    void *userBuffer;   
    
    fldrGetFolderStatus(&usedMsgBytes, &numOfMsg, &totalFolderSize, folderMask);
    
    WE_LOG_SIG_BEGIN("MMS_SIG_FOLDER_STATUS_REPLY", LS_SEND, WE_MODID_MMS, source);
    WE_LOG_SIG_UINT32("usedMsgBytes",usedMsgBytes);
    WE_LOG_SIG_UINT32("numOfMsg",numOfMsg);
    WE_LOG_SIG_UINT32("totalFolderSize",totalFolderSize);
    WE_LOG_SIG_END();

    data.usedMsgBytes = usedMsgBytes;
    data.numOfMsg = numOfMsg;
    data.totalFolderSize = totalFolderSize; 

    we_dcvt_init(&cvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    (void) mms_cvt_MmsFolderStatus(&cvtObj, &data);
    length = (WE_UINT16)cvtObj.pos;
     
    signalBuffer = WE_SIGNAL_CREATE (MMS_SIG_FOLDER_STATUS_REPLY, 
        WE_MODID_MMS, source, length);
    
    if (signalBuffer == NULL) 
    {
        WE_ERROR( WE_MODID_MMS, WE_ERR_SYSTEM_FATAL);
    }
    else
    {
        userBuffer = WE_SIGNAL_GET_USER_DATA (signalBuffer, &length);
        we_dcvt_init (&cvtObj, WE_DCVT_ENCODE, userBuffer, length, 
            WE_MODID_MMS); 
        (void) mms_cvt_MmsFolderStatus(&cvtObj, &data);
        WE_SIGNAL_SEND (signalBuffer); 
    }         
} 

