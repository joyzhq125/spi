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

#include "Mmem.h"           
#include "Msig.h"       
#include "Mhandler.h"    
#include "Mcpdu.h"      
#include "Mreport.h"     
#include "Mconfig.h"     
#include "Mcwsp.h"      
#include "Mutils.h"     
#include "Fldmgr.h"     
#include "Mlpduc.h"      
#include "Masync.h"     
#include "Mutils.h"      
#include "Mmsrpl.h"     


#define SIZE_OF_DATE           30


















static WE_BOOL fsmBusy;


static WE_UINT32 callingSM;


static MmsReadReportInfo *info;


static WE_UINT32 returnSignal;


static WE_UINT32 altReturnSignal;


static unsigned char *saveBuf;


static MmsGetHeader *saveMsgHeader;


static MmsReadStatus readStatus;


static MmsMsgId readRepMsgId;


static MmsMsgId newMsgId; 


static AsyncOperationHandle asyncOper;



static void create11report(const MmsSignal *sig);
static MmsSetHeader *createRrHeader( const MmsGetHeader *header, const WE_UINT32 date);
static unsigned char *createRrBody( const MmsGetHeader *header, WE_UINT32 date,
    WE_UINT32 msgId, WE_UINT32 *length);
static MmsResult createRrMessage( const MmsGetHeader *header, WE_UINT32 date, 
    WE_UINT32 msgId);

#if (MMS_READ_REPORT_MULTIPART_MIXED == 1)
static unsigned char *createRrMultipartMixedBody( unsigned char *pdu,
    WE_UINT32 *length);
#endif 








void readReportInitData(void)
{
    fsmBusy = FALSE;
    callingSM = 0;
    info = NULL;
    returnSignal = 0;
    altReturnSignal = 0; 
    saveBuf =  NULL;
    saveMsgHeader = NULL;
    readStatus = MMS_READ_STATUS_READ;
    readRepMsgId = 0;
    newMsgId  = 0; 
    asyncOper = NULL;
} 





void freeReportGlobalData(void)
{
    fsmBusy = FALSE;
    callingSM = 0;
    
    if (info != NULL) 
    {
        M_FREE(info);
        info = NULL;
    } 

    returnSignal = 0;
    altReturnSignal = 0;
   
    if (saveBuf != NULL) 
    {
        M_FREE(saveBuf); 
        saveBuf =  NULL;
    }  

    if (saveMsgHeader != NULL)
    {
        freeMmsGetHeader(WE_MODID_MMS, saveMsgHeader);
        M_FREE(saveMsgHeader);
        saveMsgHeader = NULL;
    }  

    readStatus = MMS_READ_STATUS_READ;
    readRepMsgId = 0;
    newMsgId  = 0; 

    if (asyncOper != NULL)  
    {
       asyncOperationStop(&asyncOper); 
    }  
} 






void deleteNewReadReport(void)
{
    st_MmsFmgDeleteParam *param;

    if (newMsgId == 0)
    {
        return;  
    }  

    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS,
        "Deleting a read report, that could not be built (%d)\n", newMsgId));
    
    param = (st_MmsFmgDeleteParam *)M_CALLOC(sizeof(st_MmsFmgDeleteParam));

    param->uiMsgId = newMsgId;

         
    M_SIGNAL_SENDTO_IUUP( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_DELETE_MSG,
                              0, 0, 0, param);
} 






void startReadReportCreation(const MmsSignal *sig )
{    
    if (fsmBusy)
    {
       M_SIGNAL_SENDTO_IU( (MmsStateMachine)sig->i_param, sig->u_param2, 
           MMS_RESULT_BUSY, 0);
       fsmBusy = FALSE;
    }
    else
    {   
        
        callingSM = (unsigned long)sig->i_param;    
        returnSignal = sig->u_param1;
        altReturnSignal = sig->u_param2;
        info = (MmsReadReportInfo *)sig->p_param;
        readStatus = info->readStatus;
        readRepMsgId = info->msgId;
        fsmBusy = TRUE; 
        
         
        M_SIGNAL_SENDTO_IUU( M_FSM_MMH_HANDLER, (int)M_FSM_MMH_GET_HEADER, \
                M_FSM_MMH_HANDLER, readRepMsgId, M_FSM_MMH_CREATE_RR_GOT_HEADER);      
     } 
} 







void createReadReport(const MmsSignal *sig)
{     
    MmsGetHeader *header;
    MmsVersion version = (MmsVersion)cfgGetInt(MMS_CFG_PROXY_RELAY_VERSION);
    
    


    if ((MmsResult)sig->i_param != MMS_RESULT_OK || sig->p_param == NULL)
    {  
       WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "Failed to parse header for the read report: MMS_SIG_MSR_CREATE_RR\n"));

       
       M_SIGNAL_SENDTO_IU( (MmsStateMachine)callingSM, returnSignal, 
           (MmsResult)sig->i_param, sig->u_param1);

       header = (MmsGetHeader*) sig->p_param;
       if (header != NULL)
       {
            freeMmsGetHeader(WE_MODID_MMS, header);
            M_FREE(header);
            header = NULL;
       } 

       freeReportGlobalData(); 
       return;
    } 


    header = (MmsGetHeader*) sig->p_param;
   
    


    if (header->msgClass.classIdentifier == MMS_MESSAGE_CLASS_AUTO 
            || header->readReply != MMS_READ_REPLY_YES)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "Not allowed to create Read Report for this message\n"));

        if ( header)
        {
            freeMmsGetHeader(WE_MODID_MMS, header);
            M_FREE(header);
            header = NULL;
        } 
                
        M_SIGNAL_SENDTO_IU( (MmsStateMachine)callingSM, returnSignal, 
            MMS_RESULT_NO_REPORT, sig->u_param1);
        freeReportGlobalData();
        return;  
    } 
    


    else if (version == MMS_VERSION_10)
    {
       st_MmsFmgCreateParam *param = M_ALLOC( sizeof(st_MmsFmgCreateParam));
         
       param->eSuffix = MMS_SUFFIX_SEND_REQ; 
       param->eLocation = MMS_DRAFTS;
       param->uiSize = 0; 
       saveMsgHeader = (MmsGetHeader*) sig->p_param;
      
        
       M_SIGNAL_SENDTO_IUUP( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_CREATE_MSG,
             0, M_FSM_MMH_HANDLER, M_FSM_MMH_MSG_RR_CREATED, param);
       
       

    }
    


    else
    {  
       create11report(sig);
    }         
} 







static void create11report(const MmsSignal *sig)
{
    MmsGetHeader *msgHeader = NULL;
    MmsResult result = MMS_RESULT_OK;
    MmsVersion version = MMS_VERSION_10;
    unsigned char *pdu = NULL;
    WE_UINT32 messageSize = 0;

    msgHeader = (MmsGetHeader *)sig->p_param;

    if ((MmsResult)sig->i_param != MMS_RESULT_OK)
    {
        result = (MmsResult)sig->i_param;
    } 
    else if (msgHeader->readReply != MMS_READ_REPLY_YES)
    {
        result = MMS_RESULT_NO_REPORT;
    }
    else if ( sig->i_param == MMS_RESULT_OK)
    {
        version = (MmsVersion)cfgGetInt(MMS_CFG_PROXY_RELAY_VERSION);

#if (MMS_READ_REPORT_DATE == 1)
        
        msgHeader->date = mmsGetGMTtime(TRUE);
#else
        msgHeader->date = 0;
#endif
        pdu = createWspReadReport(msgHeader, &messageSize, &result, 
            readStatus, version);   
    }
    else
    {
       result = (MmsResult)sig->i_param;
    } 

    
    if (msgHeader != NULL)
    {
        freeMmsGetHeader(WE_MODID_MMS, msgHeader);
        M_FREE(msgHeader);
        msgHeader = NULL;
    } 

    if (result == MMS_RESULT_OK)
    {
        fldrMgrSetRRsent(sig->u_param1);
    }

    M_SIGNAL_SENDTO_IUUP( (MmsStateMachine)callingSM, altReturnSignal, result, 
        messageSize, MMS_M_READ_REC_IND, pdu);
  
    freeReportGlobalData();
} 









void create10report(MmsSignal *sig)
{
    MmsResult result = MMS_RESULT_OK;
    WE_UINT32 date; 

    
    M_FREE( sig->p_param );
    sig->p_param = NULL;
    
    if ((saveMsgHeader == NULL) ||  
        ((MmsResult)sig->i_param != MMS_RESULT_OK)) 
    {
        
         WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "Failed to create a file for the read report: MMS_SIG_MSR_CREATE_RR\n"));

        
         M_SIGNAL_SENDTO_IU( (MmsStateMachine)callingSM, returnSignal, 
             (MmsResult)sig->i_param, sig->u_param1);
 
           
         freeReportGlobalData();
        
         return;     
    }  
    
    newMsgId = sig->u_param2;  

    if ( sig->i_param == MMS_RESULT_OK)
    {
        


        if ( saveMsgHeader->msgClass.classIdentifier != MMS_MESSAGE_CLASS_AUTO 
                    && saveMsgHeader->readReply == MMS_READ_REPLY_YES)
        {
#if (MMS_READ_REPORT_DATE == 1)
            
            date = mmsGetGMTtime(TRUE);
#else
            date = 0;
#endif      
            result = createRrMessage( saveMsgHeader, date, readRepMsgId);
        } 
        else
        {
            result = MMS_RESULT_NO_REPORT;
        } 
     } 
     else
     {
        result = (MmsResult)sig->i_param;
     } 

    

     if (saveMsgHeader != NULL)
     {
        freeMmsGetHeader(WE_MODID_MMS, saveMsgHeader);
        M_FREE(saveMsgHeader);
        saveMsgHeader = NULL;
     }  
        
     

 
     if (result != MMS_RESULT_OK)
     {
         M_SIGNAL_SENDTO_IU(  (MmsStateMachine)callingSM, returnSignal, 
             result, readRepMsgId);
         deleteNewReadReport();
         freeReportGlobalData(); 
     } 
   
      
} 











static MmsResult createRrMessage( const MmsGetHeader *header, WE_UINT32 date, 
    WE_UINT32 msgId)
{
    MmsResult ret = MMS_RESULT_OK;
    MmsSetHeader *newHeader = NULL;
    WE_UINT32 headerLength = 0;
    WE_UINT32 bodyLength = 0;
    unsigned char *pduData = NULL;
    unsigned char *pduBody = NULL;
    unsigned char *pduMsg = NULL; 
    MmsVersion version;
 
    
     
    version = (MmsVersion)cfgGetInt(MMS_CFG_PROXY_RELAY_VERSION);

    if( (header != NULL))     
    {
        
        if( (newHeader = createRrHeader( header, date)) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Could not message file for the read report",
                    __FILE__, __LINE__));  
            
            ret = MMS_RESULT_ERROR;
        } 
        

        else if (( pduData = createWspSendHeader(WE_MODID_MMS, newHeader, &headerLength, 
            &ret, version, MMS_READ_REPORT_MULTIPART_MIXED)) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Unable to encode Read report header",
                    __FILE__, __LINE__));          
        } 
        
         
        if ( newHeader != NULL)
        {
            freeMmsSetHeader(WE_MODID_MMS, newHeader);
            M_FREE( newHeader);
            newHeader = NULL;
        } 
     
        if ((ret != MMS_RESULT_OK) || (pduData == NULL)) 
        {         
           WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Failed to create a WSP coded header.\n",
            __FILE__, __LINE__));
        } 
        
        else if ((pduBody = createRrBody( header, date, msgId, &bodyLength)) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Unable to create Read report body",
                    __FILE__, __LINE__));      
                       
            ret = MMS_RESULT_ERROR;
        }
        else 
        {
            

 
             pduMsg = (unsigned char*) M_ALLOC(headerLength + bodyLength);
            
             if (pduMsg != NULL)
             {
                memcpy(pduMsg, pduData, headerLength); 
                memcpy(pduMsg + headerLength, pduBody, bodyLength);     
             } 
             else 
             {
                 return MMS_RESULT_INSUFFICIENT_MEMORY; 
             }
        } 
      
        
        if (pduBody != NULL)
        {
            M_FREE( pduBody);
            pduBody = NULL;
        }  

        if ( pduData != NULL)
        {
            M_FREE( pduData);
            pduData = NULL;
        } 

        
        if ( ret != MMS_RESULT_OK )
        {
           
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Failed to WSP code the RR message!",
                     __FILE__, __LINE__));       
        } 
        else
        {
            saveBuf = pduMsg; 
             
            
            asyncOper = asyncOperationStart( 
            Mms_FldrMsgIdToName(newMsgId, MMS_SUFFIX_SEND_REQ), M_FSM_MMH_HANDLER, 
                    0, M_FSM_MMH_RR_WRITE, ASYNC_MODE_WRITE);
            
            if ( asyncOper == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Open the file for write Read Report Data",
                     __FILE__, __LINE__));      

                M_SIGNAL_SENDTO_IU( (MmsStateMachine)callingSM, returnSignal, 
                    MMS_RESULT_FILE_WRITE_ERROR, newMsgId);
                deleteNewReadReport();
                freeReportGlobalData();
            }          
            else 
            {
                
                asyncWrite(asyncOper, 0, saveBuf,headerLength + bodyLength);
                
            }
        }        
    } 
    else 
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Invalied pointer to parsed data",
                     __FILE__, __LINE__));      
        ret = MMS_RESULT_MESSAGE_CORRUPT;       
    }
    return ret;
} 







void createInfoFileForRR (const MmsSignal *sig )
{
    
   if ((MmsResult)sig->i_param != MMS_RESULT_OK)
   {
       WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Failed to write a file for the new read report\n",
            __FILE__, __LINE__));
      
       M_SIGNAL_SENDTO_IU( (MmsStateMachine)callingSM, returnSignal, 
           (MmsResult)sig->i_param, newMsgId);

       deleteNewReadReport();
       freeReportGlobalData();
   } 
   else 
   {       
       if (asyncOper != NULL)  
       {
           asyncOperationStop(&asyncOper);
           asyncOper = NULL;
       }  
       
        
       M_SIGNAL_SENDTO_IUU(M_FSM_MMH_HANDLER, (int)M_FSM_MMH_INT_CREATE_INFO_LIST, 
            M_FSM_MMH_HANDLER, newMsgId, M_FSM_MMH_RR_INFO_FILE_CREATED);
   }  

} 







void readReportDone(const MmsSignal *sig)
{    
    if ((MmsResult)sig->u_param1 != MMS_RESULT_OK)
    {
        st_MmsFmgDeleteParam *param = M_CALLOC(sizeof(st_MmsFmgDeleteParam));

        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM MSR RECEIVE: delete unused file %d\n", sig->u_param1));

        param->uiMsgId = (MmsMsgId)sig->i_param;
         
        M_SIGNAL_SENDTO_IUUP( M_FSM_FLDR_MANAGER, (int)MMS_SIG_FMG_INT_DELETE_MSG,
                              0, 0, 0, param);
    }
    else
    {
        fldrMgrSetRRsent((MmsMsgId)sig->i_param);
        
    } 
   
    
    M_SIGNAL_SENDTO_IU( (MmsStateMachine)callingSM, returnSignal, 
        (MmsResult)sig->u_param1, (WE_UINT32) sig->i_param);
     
    freeReportGlobalData();
} 








static MmsSetHeader *createRrHeader(const MmsGetHeader *header, 
    const WE_UINT32 date)
{
    unsigned char *tmpPtr   = NULL;
    MmsSetHeader *newHeader    = M_CALLOC( sizeof( MmsSetHeader));
    char *textRead          = NULL;
    WE_UINT32 tmpVal           = 0;
    
    
    newHeader->fromType = (MmsFromType)cfgGetInt( MMS_CFG_FROM_ADDRESS_INSERT_TYPE);
    newHeader->from.addrType = (MmsAddressType) cfgGetInt(MMS_CFG_FROM_ADDRESS_TYPE);
    newHeader->from.address = NULL;
    newHeader->from.name.text = NULL;
    
    
    if ( newHeader->fromType == MMS_FROM_ADDRESS_PRESENT) 
    {
        char *addr = NULL;
        WE_UINT32 len = 0; 

        addr = cfgGetStr(MMS_CFG_FROM_ADDRESS); 

        if (addr != NULL) 
        {
            len = strlen(addr) + 1; 
            newHeader->from.address = M_ALLOC(len); 
            strcpy(newHeader->from.address, addr); 
        } 
        
        addr = cfgGetStr(MMS_CFG_FROM_NAME); 

        if (addr != NULL) 
        {
            len = strlen(addr) + 1; 
            newHeader->from.name.text = M_ALLOC(len); 
            strcpy(newHeader->from.name.text, addr);
            newHeader->from.name.charset = MMS_UTF8;
        }      
    } 
    else 
    {   
        newHeader->fromType =  MMS_FROM_INSERT_ADDRESS; 
    } 

    
    tmpVal = sizeof(MmsAddressList);
    newHeader->to = M_ALLOC(tmpVal);
   
    if (copyMmsAddress(WE_MODID_MMS, &newHeader->to->current, &header->from)
        == FALSE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): createRrHeader: unable to copy To address.",
                    __FILE__, __LINE__));  
        
       if ( newHeader)
       {
           freeMmsSetHeader(WE_MODID_MMS, newHeader);
           M_FREE(newHeader);
           newHeader = NULL;
       } 
       return NULL;
    } 

    newHeader->to->next = NULL;
    
    
    tmpVal = sizeof(MmsContentType);
    newHeader->contentType = M_CALLOC(tmpVal);
    
#if (MMS_READ_REPORT_MULTIPART_MIXED == 1)
    newHeader->contentType->knownValue = MMS_VND_WAP_MULTIPART_MIXED;
#else 
    newHeader->contentType->knownValue = MMS_TYPE_TEXT_PLAIN;
#endif 

    newHeader->contentType->strValue = NULL;
    newHeader->contentType->params = NULL;
    
    
    newHeader->subject.charset = header->subject.charset;
    
    if ((( textRead = cfgGetStr(MMS_CFG_TEXT_READ)) != NULL) &&
        ( header->subject.text != NULL))
    {
        
        tmpVal = strlen( textRead) + strlen( header->subject.text) + 1;
        newHeader->subject.text = M_ALLOC( tmpVal);
        strcpy( newHeader->subject.text, textRead);
        tmpPtr = (unsigned char *)newHeader->subject.text + strlen( textRead);
        strcpy( (char *)tmpPtr, header->subject.text);
    }
    else if ( header->subject.text != NULL)
    {
        
        tmpVal = strlen( header->subject.text) + 1;
        newHeader->subject.text = M_ALLOC( tmpVal);
        strcpy( newHeader->subject.text, header->subject.text);
    }
    else
    {
        
        if ( textRead != NULL)
        {
            tmpVal = strlen( textRead) + 1;
            newHeader->subject.text = M_ALLOC( tmpVal);
            strcpy( newHeader->subject.text, textRead);
        } 
    }  
    newHeader->subject.charset = MMS_UTF8;

    

    


    newHeader->msgClass.classIdentifier = MMS_MESSAGE_CLASS_AUTO;
    newHeader->readReply = MMS_READ_REPLY_NO;
    newHeader->deliveryReport = MMS_DELIVERY_REPORT_NO; 
    newHeader->date = date; 
    newHeader->priority = MMS_PRIORITY_NOT_SET;
    newHeader->visibility = MMS_SENDER_VISIBILITY_NOT_SET;
    
    return newHeader;
}  









static unsigned char *createRrBody( const MmsGetHeader *header, WE_UINT32 date,
    WE_UINT32 msgId, WE_UINT32 *length)
{
    unsigned char *pduBody      = NULL;
    unsigned char *pduSave      = NULL;
    unsigned char *serverMsgId  = NULL;
    char *textYourMsg           = NULL;
    char *textFrom              = NULL;
    char *textTo                = NULL;
    char *textMsgId             = NULL;
    char *textSent              = NULL;
    char *textWasReadOn         = NULL;
    char *textSubject           = NULL;
    unsigned int  nsize         = 0;
    WE_UINT32 size             = 0;
    WE_UINT32 serverMsgIdSize  = 0;
    
    nsize = strlen("\n");
    
    
    if ( (textYourMsg = cfgGetStr( MMS_CFG_TEXT_YOUR_MSG)) != NULL)
    {
        size += strlen( textYourMsg);
        size += nsize; 
    } 

    if ( (textFrom = cfgGetStr(MMS_CFG_TEXT_FROM)) != NULL)
    {
        size += strlen(textFrom);
        size += nsize; 
        if (header->from.address != NULL)
        {
            size += strlen(header->from.address);
        } 
    }  

    if ( (textTo = cfgGetStr( MMS_CFG_TEXT_TO)) != NULL)
    {
        size += strlen( textTo);
        size += nsize; 
        if ((header->to != NULL) &&
            (header->to->current.address != NULL))
        {
            size += strlen(header->to->current.address);
        } 
    } 

    if ( (textMsgId = cfgGetStr( MMS_CFG_TEXT_MSGID)) != NULL)
    {
       serverMsgId = fldrGetServerMsgId(msgId);

        if (serverMsgId == NULL)
        {
            serverMsgIdSize = 0;
        }
        else
        {
            serverMsgIdSize = strlen((char *)serverMsgId);
        } 
       
        size += strlen( textMsgId);
        size += serverMsgIdSize;
        size += nsize; 
    } 

    if ((textSubject = cfgGetStr( MMS_CFG_TEXT_SUBJECT)) != NULL)
    {
        size += strlen( textSubject);
        size += nsize; 
        if (header->subject.text != NULL)
        {
            size += strlen( header->subject.text);
        } 
    } 

    if ( (textSent = cfgGetStr( MMS_CFG_TEXT_SENT)) != NULL)
    {
        size += strlen( textSent);
        size += SIZE_OF_DATE;
        size += nsize; 
    } 

    if ( (textWasReadOn = cfgGetStr(MMS_CFG_TEXT_WAS_READ)) != NULL)
    {
        size += strlen( textWasReadOn);
        size += SIZE_OF_DATE;
        size += nsize; 
    } 
    
    size += nsize;  
    *length = size; 

    pduBody = M_CALLOC( size);
    pduSave = pduBody;
    
    
    if ( textYourMsg != NULL)
    {
        strcpy( (char *)pduBody, textYourMsg);
        pduBody += strlen( textYourMsg);
        *pduBody = '\n';
        pduBody += nsize;
    } 

    if ( textFrom != NULL)
    {
        strcpy( (char *)pduBody, textFrom);
        pduBody += strlen( textFrom);
        if (header->from.address != NULL)
        {
            strcpy( (char *)pduBody, header->from.address);
            pduBody += strlen( header->from.address);
        } 

        *pduBody = '\n';
        pduBody += nsize;
    } 

    if ( textTo != NULL)
    {
        strcpy( (char *)pduBody, textTo);
        pduBody += strlen( textTo);
        if ((header->to != NULL) &&
            (header->to->current.address != NULL))
        {
            strcpy( (char *)pduBody, header->to->current.address);
            pduBody += strlen( header->to->current.address);
        } 

        *pduBody = '\n';
        pduBody += nsize;
    } 

    if ( textSubject != NULL)
    {
        strcpy( (char *)pduBody, textSubject);
        pduBody += strlen( textSubject);
        if ( header->subject.text != NULL)
        {
            
            strcpy( (char *)pduBody, header->subject.text);
            pduBody += strlen( header->subject.text);
        } 
        *pduBody = '\n';
        pduBody += nsize;
    } 

    if ( textMsgId != NULL)
    {
        strcpy( (char *)pduBody, textMsgId);
        pduBody += strlen( textMsgId);

        if ( serverMsgId != NULL)
        {
            memcpy( pduBody, serverMsgId, serverMsgIdSize);
            
            pduBody += serverMsgIdSize; 
        }       

        *pduBody = '\n';
        pduBody += nsize;
    } 

    if ( textSent != NULL)
    {
        
        char dateTemp[SIZE_OF_DATE];
        memset(dateTemp,0,SIZE_OF_DATE);
        strcpy( (char *)pduBody, textSent);
        pduBody += strlen( textSent);
        mmsConvertDateToString( header->date,dateTemp);
        strcpy((char*)pduBody,dateTemp);
        pduBody += strlen(dateTemp);
        *pduBody = '\n';
        pduBody += nsize;
    } 

    if ( textWasReadOn != NULL)
    {
        char dateTemp[SIZE_OF_DATE];
        memset(dateTemp,0,SIZE_OF_DATE);
        strcpy( (char *)pduBody, textWasReadOn);
        pduBody += strlen( textWasReadOn);
        mmsConvertDateToString( date,dateTemp);
        strcpy((char *)pduBody,dateTemp);
        pduBody += strlen(dateTemp);
        *pduBody = '\n';
        pduBody += nsize;
    } 
    *pduBody++ = 0; 
    *length=strlen((char*)pduSave);
    

#if (MMS_READ_REPORT_MULTIPART_MIXED == 1)
    pduSave = createRrMultipartMixedBody(pduSave, length);
#endif  


    return pduSave; 
} 









#if (MMS_READ_REPORT_MULTIPART_MIXED == 1)
static unsigned char *createRrMultipartMixedBody( unsigned char *pdu,
    WE_UINT32 *length)
{
    unsigned char *multipart;
    unsigned char *sum;
    MmsBodyParams params;
    MmsResult result;
    WE_UINT32 len;
    WE_UINT32 size;            

    params.contentType.knownValue = MMS_TYPE_TEXT_PLAIN;
    params.contentType.params = M_ALLOC(sizeof(MmsAllParams));
    params.contentType.params->param = MMS_CHARSET;
    params.contentType.params->type = MMS_PARAM_INTEGER;
    params.contentType.params->value.integer = MMS_UTF8;
    params.contentType.params->next = NULL;
    params.contentType.strValue = NULL;
    params.entryHeader = NULL;
    params.targetType = MMS_MULTIPART_MESSAGE;

    size = strlen((char*)pdu) + 1;

    
    multipart = createWspBodyPartHead(WE_MODID_MMS, &params, &len, size, &result);
    if ( multipart == NULL)
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): unable to createWspBodyPartHead\n",
            __FILE__, __LINE__));
        M_FREE(params.contentType.params);
        M_FREE(pdu);
        return NULL;
    } 

    
    sum = M_CALLOC(len + size);
    memcpy( sum, multipart, len);   
    memcpy( sum + len, pdu, size); 
    M_FREE(multipart);  
    M_FREE(pdu);       
    M_FREE(params.contentType.params);

    *length = len + size;
    return sum;
}
#endif 
