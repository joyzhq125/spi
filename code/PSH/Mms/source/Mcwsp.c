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
#include "We_Log.h"     
#include "We_Mem.h"    

#include "Mms_Def.h"    
#include "Mms_Cfg.h"    
#include "Mmem.h"       
#include "Mcpdu.h"      
#include "Mlpduc.h"     
#include "Mutils.h"     
#include "Mlfieldc.h"   
#include "Mcwsp.h"      
#include "Mconfig.h"     





































unsigned char *createWspAcknowledgeIndMsg(const MmsAcknowledgeInd *header,
    WE_UINT32 *length, MmsVersion version) 
{
    unsigned char *pduHeader;  
    unsigned long bufSize = 0; 
    MmsHeaderValue valueOfTag;  
    unsigned long actLength = 0;  

     
    
    bufSize += TAG_SIZE + sizeof(MmsMessageType);   
    bufSize += TAG_SIZE + 
        strlen((char *)header->transactionId) + 1;  
    bufSize += TAG_SIZE + sizeof(WE_UINT8);        
    bufSize += TAG_SIZE + sizeof(WE_UINT8);        
   
    pduHeader = (unsigned char *)M_CALLOC((unsigned int)bufSize);

      
    valueOfTag.messageType = (WE_UINT8)MMS_M_ACKNOWLEDGE_IND;
    
    if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_MESSAGE_TYPE, 
        sizeof(WE_UINT8), valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "createWspAcknowledgeIndMsg: Error in X-Mms-Message-Type\n"));
        M_FREE(pduHeader);
        pduHeader = NULL;
        return NULL;    
    }   

      
    valueOfTag.transactionId = header->transactionId;
    
    if (!mmsPduAppend(pduHeader, bufSize, &actLength, X_MMS_TRANSACTION_ID, 
        strlen(valueOfTag.transactionId) + 1, valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "createWspAcknowledgeIndMsg: Error in X-Mms-Transaction-Id\n"));
        M_FREE(pduHeader); 
        pduHeader = NULL;
        return NULL;    
    }   
     
     
    valueOfTag.version = (unsigned char)version;

    if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_VERSION, 
        sizeof(WE_UINT8), valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "createWspAcknowledgeIndMsg: Error in X-Mms-Version\n"));
        M_FREE(pduHeader); 
        pduHeader = NULL;
        return NULL;    
    }   

    
    if ( header->allowed != MMS_DELIVERY_REPORT_ALLOWED_NOT_SET)
    {
        valueOfTag.reportAllowed = (WE_UINT8)header->allowed;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, 
            X_MMS_REPORT_ALLOWED, sizeof(WE_UINT8), valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
                "%s(%d): Error in X-Mms-Report-Allowed\n",
                __FILE__, __LINE__));
            M_FREE(pduHeader);
            pduHeader = NULL;
            return NULL;    
        }   
    }  

    *length = (WE_UINT32)actLength; 
    return pduHeader;    
}  











unsigned char *createWspNotifyRespIndMsg(const MmsNotifyRespInd *header,
    WE_UINT32 *length, MmsVersion version)
{ 
    unsigned char *pduHeader;  
    unsigned long bufSize = 0; 
    MmsHeaderValue valueOfTag;  
    unsigned long actLength = 0;     

    *length = 0;
    if (header->transactionId == NULL || strlen( (char *)header->transactionId) < 1)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Transaction WID mandatory.\n", __FILE__, __LINE__));

        return NULL;
    } 

    
    
    bufSize += TAG_SIZE + sizeof(MmsMessageType);           
    bufSize += TAG_SIZE + 
        strlen((char *)header->transactionId) + 1;          
    bufSize += TAG_SIZE + sizeof(WE_UINT8);                    
    bufSize += TAG_SIZE + sizeof(WE_UINT8);                     
    bufSize += TAG_SIZE + sizeof(WE_UINT8);                    
   
    pduHeader = (unsigned char *)M_CALLOC((unsigned int)bufSize);
     
     
    valueOfTag.messageType = (WE_UINT8)MMS_M_NOTIFY_RESP;
    
    if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_MESSAGE_TYPE,
        sizeof(WE_UINT8), valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Error in X-Mms-Message-Type\n", __FILE__, __LINE__));
        M_FREE(pduHeader);
        pduHeader = NULL;
        return NULL;    
    }   

      
    valueOfTag.transactionId = header->transactionId;
    
    if (!mmsPduAppend(pduHeader, bufSize, &actLength, X_MMS_TRANSACTION_ID, 
        strlen(valueOfTag.transactionId) + 1, valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Error in X-Mms-Transaction-Id\n", __FILE__, __LINE__));
        M_FREE(pduHeader); 
        pduHeader = NULL;
        return NULL;    
    }   
     
     
    valueOfTag.version = (unsigned char)version;

    if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_VERSION, 
        sizeof(WE_UINT8), valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Error in X-Mms-Version\n", __FILE__, __LINE__));
        M_FREE(pduHeader); 
        pduHeader = NULL;
        return NULL;    
    }   

     
    valueOfTag.priority = (WE_UINT8)header->status;

    if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_STATUS, 
        sizeof(WE_UINT8), valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Error in X-Mms-Status\n", __FILE__, __LINE__));
        M_FREE(pduHeader);
        pduHeader = NULL;
        return NULL;    
    }   
 
    if (header->allowed != MMS_DELIVERY_REPORT_ALLOWED_NOT_SET)
    {
         
        valueOfTag.reportAllowed = (WE_UINT8)header->allowed;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_REPORT_ALLOWED, 
            sizeof(WE_UINT8), valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
                "%s(%d): Error in X-Mms-Report-Allowed\n", __FILE__, __LINE__));
            M_FREE(pduHeader);
            pduHeader = NULL;
            return NULL;    
        }   
    } 

    *length = (WE_UINT32)actLength; 
    return pduHeader;   
}  













unsigned char *createWspReadReport(const MmsGetHeader *header, WE_UINT32 *length, 
    MmsResult *result, MmsReadStatus readStatus, MmsVersion version)
{ 

    unsigned char *pduHeader = NULL;    
    unsigned long actLength = 0;         
    unsigned long bufSize = 0;          
    unsigned long fromSize = 0;
    unsigned char *from = NULL; 
    MmsHeaderValue valueOfTag;
    MmsAddressList addressList;
    MmsAddress configFrom;

    *result = MMS_RESULT_OK;
    *length = 0;
    memset(&addressList, 0, sizeof(MmsAddressList));

    if (header->serverMessageId == NULL || 
        strlen( (char *)header->serverMessageId) < 1)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "%s(%d): Message WID mandatory.\n", __FILE__, __LINE__));
        *result = MMS_RESULT_ERROR_CREATING_READ_REPORT;
        return NULL;
    } 

    
    if (copyMmsAddress(WE_MODID_MMS, &addressList.current,
        &header->from) == FALSE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "createWspReadReport: Failed copy From field\n"));
        *result = MMS_RESULT_INVALID_ADDRESS;
        return NULL;    
    }  


    
    configFrom.address = cfgGetStr(MMS_CFG_FROM_ADDRESS);
    configFrom.addrType = (MmsAddressType)cfgGetInt(MMS_CFG_FROM_ADDRESS_TYPE);
    configFrom.name.text = cfgGetStr(MMS_CFG_FROM_NAME);
    configFrom.name.charset = MMS_UTF8;
    if (createFromField( WE_MODID_MMS, (MmsAddress *)&configFrom, 
        (WE_UINT32)cfgGetInt(MMS_CFG_FROM_ADDRESS_INSERT_TYPE),
        &from, &fromSize) == FALSE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "createWspReadReport: Failed to convert From field\n"));
        *result = MMS_RESULT_INVALID_ADDRESS;
        M_FREE(from);
        from = NULL;
        return NULL;    
    } 
                                             
    
    
    bufSize += TAG_SIZE + sizeof(MmsMessageType);           
    bufSize += TAG_SIZE + sizeof(WE_UINT8);                
    bufSize += TAG_SIZE + 
        strlen((char *)header->serverMessageId) + 1;        
    bufSize += TAG_SIZE + getSizeOfAddress(&addressList);   
    bufSize += TAG_SIZE + fromSize + 1;                     
    bufSize += TAG_SIZE + ENCODED_D_INT_LEN;                
    bufSize += TAG_SIZE + sizeof(WE_UINT8);                 

    
    if (bufSize >= MMS_MAX_CHUNK_SIZE)              
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "createWspReadReport: Header size (%d) exceeded chunk size (%d).\n",
             bufSize, MMS_MAX_CHUNK_SIZE));
        *result = MMS_RESULT_INVALID_HEADER_SIZE;
    }
    else if ( !mmsValidateAddressList(&addressList))    
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "createWspReadReport: Invalid address size.\n"));
        *result = MMS_RESULT_INVALID_ADDRESS;
    }

    if (*result != MMS_RESULT_OK)
    {
        M_FREE(from); 
        return NULL;
    } 
    

    pduHeader = (unsigned char *)M_CALLOC((unsigned int)bufSize);
     
     
    valueOfTag.messageType = (WE_UINT8)MMS_M_READ_REC_IND;
    
    if (!mmsPduAppend( pduHeader,    
        bufSize,                     
        &actLength,                  
        X_MMS_MESSAGE_TYPE,          
        sizeof(WE_UINT8),           
        valueOfTag))                 
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Error in X-Mms-Message-Type\n", __FILE__, __LINE__));
        M_FREE(pduHeader);
        pduHeader = NULL;
        M_FREE(from);
        from = NULL;
        *result = MMS_RESULT_ERROR_CREATING_READ_REPORT;
        return pduHeader;     
    }   

     
    valueOfTag.version = (unsigned char)version;

    if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_VERSION, 
        sizeof(WE_UINT8), valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Error in X-Mms-Version\n", __FILE__, __LINE__));
        M_FREE(pduHeader); 
        pduHeader = NULL;
        M_FREE(from);
        from = NULL;
        *result = MMS_RESULT_ERROR_CREATING_READ_REPORT;
        return pduHeader;    
    }   

     
    valueOfTag.messageId = header->serverMessageId;
    
    if (!mmsPduAppend(pduHeader, bufSize, &actLength, MMS_MESSAGE_ID, 
        strlen(valueOfTag.transactionId) + 1, valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, "%s(%d): Error in Message-Id\n", __FILE__, __LINE__));
        M_FREE(pduHeader); 
        pduHeader = NULL;
        M_FREE(from);
        from = NULL;
        *result = MMS_RESULT_ERROR_CREATING_READ_REPORT;
        return pduHeader;    
    }   

     
    if (createDestinationField(WE_MODID_MMS, &addressList, pduHeader, bufSize,
        &actLength, &valueOfTag, MMS_TO) == FALSE )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "createWspReadReport: Could not add To to header\n"));
        *result = MMS_RESULT_INVALID_ADDRESS;
        M_FREE( pduHeader); 
        pduHeader = NULL;
        M_FREE(from);
        from = NULL;
        return pduHeader;    
    } 

    
    valueOfTag.from = from;
    freeMmsAddress(WE_MODID_MMS, &(addressList.current));

    if (!mmsPduAppend( pduHeader, bufSize, &actLength, MMS_FROM, fromSize, 
        valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "createWspReadReport: Could not add From to header\n"));
        *result = MMS_RESULT_INVALID_ADDRESS;
        M_FREE(from); 
        M_FREE(pduHeader);
        pduHeader = NULL;
        from = NULL;
        return pduHeader;    
    }  

    M_FREE(from);
    from = NULL;
    
    
    if (header->date != 0) 
    {
        valueOfTag.date = header->date;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, MMS_DATE, 
            sizeof(WE_UINT32), valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "createWspReadReport: Error in Date\n"));
            *result = MMS_RESULT_ERROR_CREATING_READ_REPORT;
            M_FREE(pduHeader);
            pduHeader = NULL;
            return pduHeader;    
        }   
    } 

    
    valueOfTag.date = readStatus;
    if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_READ_STATUS, 
        sizeof(WE_UINT8), valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,
            "createWspReadReport: Error in Date\n"));
        *result = MMS_RESULT_ERROR_CREATING_READ_REPORT;
        M_FREE(pduHeader);
        pduHeader = NULL;
        return pduHeader;    
    }   

    *length = actLength;

    return pduHeader;   
}  

