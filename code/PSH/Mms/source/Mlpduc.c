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
#include "Mcpdu.h"      
#include "Mlpduc.h"     
#include "Mutils.h"     
#include "Mlfieldc.h"   




















   














unsigned char *createWspSendHeader( WE_UINT8 modId, const MmsSetHeader *header, 
    WE_UINT32 *length, MmsResult *result, MmsVersion version, 
    WE_UINT32 numOfBodyParts)
{    
    unsigned char *pduHeader;       
    char *transactionId;       
    unsigned long transactionIdLen = 0; 
    unsigned long actLength = 0;         
    unsigned long bufSize = 0;          
    unsigned long fromSize = 0;
    unsigned long size = 0;
    unsigned char *codedValue;      
    unsigned char *from; 
    unsigned char *subject; 
    unsigned char *tempPtr = NULL;
    WE_UINT32 tempSize;
    MmsHeaderValue valueOfTag;
    MmsResult resultInit = MMS_RESULT_OK;
 
    
    transactionId = generateMmsTransactionId( modId, &transactionIdLen);

    
    
    if (createFromField( modId, (MmsAddress *)&header->from, 
        (WE_UINT32)header->fromType, &from , &fromSize) == FALSE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspSendHeader: Failed to convert From field\n"));
        WE_MEM_FREE( modId, transactionId);
        transactionId = NULL;
        *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
        return NULL;    
    } 
                                             
    
    
    bufSize += TAG_SIZE + sizeof(MmsMessageType);           
    bufSize += TAG_SIZE + transactionIdLen;                  
    bufSize += TAG_SIZE + sizeof(WE_UINT8);                    
    bufSize += TAG_SIZE + ENCODED_D_INT_LEN;                
    bufSize += TAG_SIZE + fromSize + 1;                     
    bufSize += TAG_SIZE + getSizeOfAddress(header->to);     
    bufSize += TAG_SIZE + getSizeOfAddress(header->cc);     
    bufSize += TAG_SIZE + getSizeOfAddress(header->bcc);    
    bufSize += TAG_SIZE + getSizeOfEncodedText(header->subject) + 1; 
    bufSize += TAG_SIZE + MAX_STR_LEN_MSG_CLASS + 1;        
    bufSize += TAG_SIZE + ENCODED_D_INT_LEN;                
    bufSize += TAG_SIZE + ENCODED_D_INT_LEN;                
    bufSize += TAG_SIZE + sizeof(WE_UINT8);                
    bufSize += TAG_SIZE + sizeof(WE_UINT8);                
    bufSize += TAG_SIZE + sizeof(WE_UINT8);                
    bufSize += TAG_SIZE + sizeof(WE_UINT8);                
    if (numOfBodyParts > 0)
    {
        bufSize += UINTVAR_MAX_LENGTH;
    } 
    
    
    if (version == MMS_VERSION_11 && 
        header->replyCharging != MMS_REPLY_CHARGING_NOT_SET)
    {
        
        bufSize += TAG_SIZE + sizeof(WE_UINT8);

        
        if (header->replyChargingDeadline.value != 0)
        {
            bufSize += TAG_SIZE + ENCODED_D_INT_LEN;    
        }

        
        if (header->replyChargingId != NULL)
        {
            bufSize += TAG_SIZE + 
            strlen((char *)header->replyChargingId) + 1;    
        } 

        
        if (header->replyChargingSize != 0)
        {
            bufSize += TAG_SIZE + ENCODED_D_INT_LEN;            
        }
    } 
    
    
    calculateSize(  header->contentType, &tempSize);         
     
    bufSize += TAG_SIZE + tempSize + UINTVAR_MAX_LENGTH;

    
    
    if (bufSize >= MMS_MAX_CHUNK_SIZE) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspSendHeader: Header size (%d) exceeded chunk size (%d).\n",
            bufSize, MMS_MAX_CHUNK_SIZE));
        resultInit = MMS_RESULT_INVALID_HEADER_SIZE;
    }
    else if ( !mmsValidateAddressList( header->to) ||  
        !mmsValidateAddressList( header->cc) ||
        !mmsValidateAddressList( header->bcc))
     {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspSendHeader: Invalid address size.\n"));
        resultInit = MMS_RESULT_INVALID_ADDRESS;
    }
    else if ( !mmsValidateSubject( &header->subject))      
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspSendHeader: Invalid subject size.\n"));
        resultInit = MMS_RESULT_INVALID_SUBJECT;
    }
    else if ( !mmsValidateMessageClass( &header->msgClass))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspSendHeader: Invalid message class size.\n"));
        resultInit = MMS_RESULT_INVALID_MSG_CLASS;
    } 

    if (resultInit != MMS_RESULT_OK)
    {
        WE_MEM_FREE( modId, from); 
        WE_MEM_FREE( modId, transactionId);
        *result = resultInit;

        return NULL;
    } 
    

    
    pduHeader = WE_MEM_ALLOC( modId, (unsigned int)bufSize);
    memset(pduHeader,0,(unsigned int)bufSize);
    if (pduHeader == NULL )
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspSendHeader: Failed to allocate memory\n"));
        WE_MEM_FREE( modId, transactionId);
        transactionId = NULL;
        *result = MMS_RESULT_INSUFFICIENT_MEMORY;
        return NULL;    
    } 
    
    



    
    valueOfTag.messageType = (WE_UINT8)MMS_M_SEND_REQ;
    
    if (!mmsPduAppend( pduHeader,     
        bufSize,            
        &actLength,         
        X_MMS_MESSAGE_TYPE, 
        sizeof(WE_UINT8),      
        valueOfTag))        
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspSendHeader: Error in X-Mms-Message-Type\n"));
        WE_MEM_FREE( modId, from); 
        WE_MEM_FREE( modId, transactionId);
        WE_MEM_FREE( modId, pduHeader);
        *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;

        return NULL;    
    }   
     
    
    valueOfTag.transactionId = transactionId;

    if (!mmsPduAppend(pduHeader, bufSize, &actLength, X_MMS_TRANSACTION_ID, 
        transactionIdLen, valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspSendHeader: Error in X-Mms-Transaction-Id\n"));
        *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
        WE_MEM_FREE( modId, from); 
        WE_MEM_FREE( modId, transactionId);
        WE_MEM_FREE( modId, pduHeader);

        return NULL;    
    }   
 
    WE_MEM_FREE( modId, transactionId);
    transactionId = NULL;
   
    
    valueOfTag.version = (unsigned char)version;

    if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_VERSION, 
        sizeof(WE_UINT8), valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspSendHeader: Error in X-Mms-Version\n"));
        *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
        WE_MEM_FREE( modId, from); 
        WE_MEM_FREE( modId, pduHeader);

        return NULL;    
    }   

    
    if (header->date != 0) 
    {
        valueOfTag.date = header->date;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, MMS_DATE, 
            sizeof(WE_UINT32), valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Error in Date\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, from); 
            WE_MEM_FREE( modId, pduHeader);

            return NULL;    
        }   
     } 

    
    valueOfTag.from = from;
    if (!mmsPduAppend( pduHeader, bufSize, &actLength, MMS_FROM, fromSize, 
        valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspSendHeader: Could not add From to header\n"));
        *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
        WE_MEM_FREE( modId, from); 
        WE_MEM_FREE( modId, pduHeader);

        return NULL;    
    }  

    WE_MEM_FREE( modId, from);
    from = NULL;
    
    
    if (header->to != NULL) 
    {
        if (createDestinationField( modId, header->to, pduHeader, bufSize,
            &actLength, &valueOfTag, MMS_TO) == FALSE )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Could not add To to header\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId,  pduHeader); 
            pduHeader = NULL;
            return NULL;
        } 
    } 

    
    if (header->cc != NULL) 
    {
        if (createDestinationField( modId, header->cc, pduHeader, bufSize,
            &actLength, &valueOfTag, MMS_CC) == FALSE )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Could not add Cc to header\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader); 
            pduHeader = NULL;
            return NULL;
        } 
    } 

    
    if (header->bcc != NULL) 
    {
        if (createDestinationField( modId, header->bcc, pduHeader, bufSize,
            &actLength, &valueOfTag, MMS_BCC) == FALSE )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Could not add Bcc to header\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader); 
            pduHeader = NULL;
            return NULL;
        } 
    } 

    
    if (header->subject.text != NULL)
    {
        if (createEncodedStringValue( modId, header->subject.text, 
             header->subject.charset, &size, &subject) == FALSE)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Could not encode subject string\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader); 
            pduHeader = NULL;
            return NULL;    
        } 

        valueOfTag.subject = subject;
        if (!mmsPduAppend( pduHeader, bufSize, &actLength, MMS_SUBJECT, size, 
            valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Could not add Subject\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, subject);
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            return NULL;    
        }   

        WE_MEM_FREE( modId, subject);
    } 
    
     
    if (header->msgClass.classIdentifier != MMS_MESSAGE_CLASS_NOT_SET)
    {
        valueOfTag.messageClass = createMsgClass( modId, &size, header->msgClass);

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_MESSAGE_CLASS, 
            size, valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: X-Mms-Message-Class\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, valueOfTag.messageClass);
            valueOfTag.messageClass = NULL;
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            return NULL;    
        }   
        
        WE_MEM_FREE( modId, valueOfTag.messageClass);
        valueOfTag.messageClass = NULL;
    }  
      
    
    if (header->expiryTime.value != 0)
    {
        if ((codedValue = createTimeField( modId, &size, header->expiryTime.value,
            header->expiryTime.type)) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: X-Mms-Expiry conversion failed.\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            WE_MEM_FREE( modId, codedValue);
            codedValue = NULL;
            return NULL;    
        }  
        
        valueOfTag.expiry = codedValue;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_EXPIRY, size, 
            valueOfTag))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Error in X-Mms-Expiry\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            WE_MEM_FREE( modId, codedValue);
            codedValue = NULL;
            return NULL;    
        } 

        WE_MEM_FREE( modId, codedValue);
        codedValue = NULL;
    }    

    
    if (header->deliveryTime.value != 0)
    {
        if ((codedValue = 
            createTimeField( modId, &size, header->deliveryTime.value,
            header->deliveryTime.type)) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: X-Mms-Delivery-Time error.\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            WE_MEM_FREE( modId, codedValue);
            codedValue = NULL;
            return NULL;    
        }  
        
        valueOfTag.deliveryTime = codedValue;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_DELIVERY_TIME, 
            size, valueOfTag))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Error in X-Mms-Delivery-Time\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            WE_MEM_FREE( modId, codedValue);
            codedValue = NULL;
            return NULL;    
        } 

        WE_MEM_FREE( modId, codedValue);
        codedValue = NULL;
    }    

    
    if (header->priority != MMS_PRIORITY_NOT_SET)
    {
        valueOfTag.priority = (WE_UINT8)header->priority;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_PRIORITY, 
            sizeof(WE_UINT8), valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Error in  X-Mms-Priority\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            return NULL;    
        }   
    } 
   
   
    if (header->visibility != MMS_SENDER_VISIBILITY_NOT_SET)
    {
        valueOfTag.senderVisibility = (WE_UINT8)header->visibility;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, 
            X_MMS_SENDER_VISIBILITY, sizeof(WE_UINT8), valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Error in X-Mms-Visibility\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            return NULL;    
        }   
    } 

   
    if (header->deliveryReport != MMS_DELIVERY_REPORT_NOT_SET)
    {
        valueOfTag.deliveryReport = (WE_UINT8)header->deliveryReport;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, 
            X_MMS_DELIVERY_REPORT, sizeof(WE_UINT8), valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Error in X-Mms-DeliveryReport\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            return NULL;    
        }   
    } 

   
    if (header->readReply != MMS_READ_REPLY_NOT_SET)
    {
        valueOfTag.readReply = (WE_UINT8)header->readReply;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_READ_REPLY, 
            sizeof(WE_UINT8), valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Error in X-Mms-Read-Reply\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            return NULL;    
        }   
    }    
 
          
        if (version == MMS_VERSION_11 && header->replyChargingId != NULL)
        {
            valueOfTag.replyChargingId = header->replyChargingId;
    
            if (!mmsPduAppend(pduHeader, bufSize, &actLength, X_MMS_REPLY_CHARGING_ID, 
                strlen(valueOfTag.replyChargingId) + 1, valueOfTag))
            { 
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "createWspSendHeader: Error in X-Mms-Reply-Charging-Id\n"));
                WE_MEM_FREE( modId, pduHeader); 
                pduHeader = NULL;
                return NULL;    
            }   
        } 
    


    if ( 
        header->replyCharging != MMS_REPLY_CHARGING_NOT_SET)
    {
        
        valueOfTag.replyCharging = (WE_UINT8)header->replyCharging;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_REPLY_CHARGING, 
            sizeof(WE_UINT8), valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Error in X-Mms-Read-Reply\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            return NULL;    
        }   
        
        
        if (header->replyChargingDeadline.value != 0)
        {
            if ((codedValue = createTimeField( modId, &size, 
                header->replyChargingDeadline.value,
                header->replyChargingDeadline.type)) == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: X-Mms-Reply-Charging-Deadline \
                conversion failed.\n"));
                *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
                WE_MEM_FREE( modId, pduHeader);
                pduHeader = NULL;
                WE_MEM_FREE( modId, codedValue);
                codedValue = NULL;
                return NULL;    
            }  
            
            valueOfTag.replyChargingDeadline = codedValue;
            
            if (!mmsPduAppend( pduHeader, bufSize, &actLength, 
                X_MMS_REPLY_CHARGING_DEADLINE, size, valueOfTag))
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Error in \
                X-Mms-Reply-Charging-Deadline\n"));
                *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
                WE_MEM_FREE( modId, pduHeader);
                pduHeader = NULL;
                WE_MEM_FREE( modId, codedValue);
                codedValue = NULL;
                return NULL;    
            } 
            
            WE_MEM_FREE( modId, codedValue);
            codedValue = NULL;
        }
    
  

        
        if (header->replyChargingSize != 0)
        {
            valueOfTag.replyChargingSize = header->replyChargingSize;
            
            if (!mmsPduAppend( pduHeader, bufSize, &actLength,
                X_MMS_REPLY_CHARGING_SIZE, sizeof(WE_UINT32), valueOfTag))
            { 
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "createWspForward: Error in X-Mms-Reply-Charging-Size\n"));
                *result = MMS_RESULT_ERROR_CREATING_FORWARD;
                WE_MEM_FREE( modId, pduHeader);
                return NULL;    
            }   
        }
    } 

    


    if (header->contentType != NULL)
    {
        if (( codedValue = (unsigned char *)createContentType( modId, 
            (WE_UINT32 *)&size, header->contentType)) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Content-Type conversion failed.\n"));
            *result = MMS_RESULT_INVALID_CONTENT_TYPE;
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            return NULL;   
        }  
    
        valueOfTag.contentType = codedValue;

        if (!mmsPduAppend(pduHeader, bufSize, &actLength, MMS_CONTENT_TYPE, 
            size, valueOfTag))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspSendHeader: Error appending Content-Type\n"));
            *result = MMS_RESULT_ERROR_CREATING_SEND_HEADER;
            WE_MEM_FREE( modId, pduHeader);
            pduHeader = NULL;
            return NULL;   
        } 

        WE_MEM_FREE( modId, codedValue);
        codedValue = NULL;
    } 

    if (numOfBodyParts > 0)
    {
        
        tempPtr = (unsigned char*)cnvUint32ToUintvar( 
            numOfBodyParts, &((unsigned char *)pduHeader)[actLength], bufSize - actLength);
        actLength += (WE_UINT32)(tempPtr - &((unsigned char *)pduHeader)[actLength]);
    } 
    *length = (WE_UINT32)actLength ; 
    return pduHeader;
} 










unsigned char *createWspBodyPartHead( WE_UINT8 modId, 
    const MmsBodyParams *dataValues, WE_UINT32 *length, WE_UINT32 size, 
    MmsResult *result)
{
    WE_UINT32 sizeOfContentType;
    WE_UINT32 maxSize;
    WE_UINT32 headersLen;

    unsigned char *pduData;
    unsigned char *pduDataRet;
    unsigned char *contentTypeData;
    WE_UINT32 dataSize;

    unsigned char *tmpPtr;    
    
    *length = 0;

    
    headersLen = calculateEntryHeadersSize( dataValues->entryHeader);    
    
    if ( mmsValidateEntryHeaders( modId, dataValues->entryHeader, result) == FALSE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspBodyPart: Invalid entry headers\n"));
        *result = MMS_RESULT_ERROR;
        return  NULL;
    }  
    
    
    if ( (contentTypeData = (unsigned char *)createContentType( modId,
        &sizeOfContentType, &dataValues->contentType)) == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspBodyPart: Error creating contentType\n"));
        *result = MMS_RESULT_ERROR;
        pduDataRet = NULL;
    }
    else
    {
        
        maxSize = UINTVAR_MAX_LENGTH +        
            UINTVAR_MAX_LENGTH +              
            UINTVAR_MAX_LENGTH +              
            headersLen + sizeOfContentType; 
    
        if ( (pduData = WE_MEM_ALLOC( modId, maxSize)) == NULL)
        {
            
            *result = MMS_RESULT_INSUFFICIENT_MEMORY;
            return NULL;
        } 

        pduDataRet = pduData;  

        
        tmpPtr = cnvUint32ToUintvar( sizeOfContentType + headersLen, pduData,
            maxSize - *length);
        *length += ( unsigned long)(tmpPtr - pduData);
        pduData = tmpPtr; 
        
        dataSize = size;
        tmpPtr = cnvUint32ToUintvar( dataSize, pduData, maxSize - *length);
        *length += ( unsigned long)(tmpPtr - pduData);
        pduData = tmpPtr; 
        
        
        memcpy( pduData, contentTypeData, sizeOfContentType);
        pduData += sizeOfContentType;
        *length += sizeOfContentType;
        WE_MEM_FREE( modId, contentTypeData);
        contentTypeData = NULL;
            
        
        if ( dataValues->entryHeader != NULL)
        {
            pduData = createEntryHeaders( dataValues->entryHeader, pduData);
        } 

        if ( pduData != NULL)
        {
            *length = ( unsigned long)(pduData - pduDataRet);
        }
        else
        {
            
            *length = 0;
            WE_MEM_FREE( modId, pduDataRet);
            pduDataRet = NULL;
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "createWspBodyPartHead: Error creating entry headers\n"));
        }
    } 

    return pduDataRet;   
} 












unsigned char *createWspForward( WE_UINT8 modId, const MmsForward *header,
    char *contentLocation, WE_UINT32 *length, MmsResult *result, MmsVersion version)
{    
    unsigned char *pduHeader = NULL;    
    char          *transactionId = NULL; 
    unsigned long transactionIdLen = 0; 
    unsigned long actLength = 0;         
    unsigned long bufSize = 0;          
    unsigned long fromSize = 0;
    unsigned long size = 0;
    unsigned long ctLen = 0;            
    unsigned char *codedValue = NULL;   
    unsigned char *from = NULL; 
    
    MmsHeaderValue valueOfTag;
 
    *result = MMS_RESULT_OK;
    *length = 0;
    
    transactionId = generateMmsTransactionId( modId, &transactionIdLen);

    
    if (createFromField( modId, (MmsAddress *)&header->from, (WE_UINT32)header->fromType,
         &from, &fromSize) == FALSE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspForward: Failed to convert From field\n"));
        WE_MEM_FREE(modId, transactionId);
        transactionId = NULL;
        *result = MMS_RESULT_INVALID_ADDRESS;
        return NULL;    
    } 
                                             
    
    
    bufSize += TAG_SIZE + sizeof(MmsMessageType);        
    bufSize += TAG_SIZE + transactionIdLen;               
    bufSize += TAG_SIZE + sizeof(WE_UINT8);             
    bufSize += TAG_SIZE + ENCODED_D_INT_LEN;             
    bufSize += TAG_SIZE + fromSize + 1;                  
    bufSize += TAG_SIZE + getSizeOfAddress(header->to);  
    bufSize += TAG_SIZE + getSizeOfAddress(header->cc);  
    bufSize += TAG_SIZE + getSizeOfAddress(header->bcc); 
    bufSize += TAG_SIZE + ENCODED_D_INT_LEN;             
    bufSize += TAG_SIZE + ENCODED_D_INT_LEN;             
    bufSize += TAG_SIZE + ENCODED_OCTET_LEN;             
    bufSize += TAG_SIZE + sizeof(WE_UINT8);             
    bufSize += TAG_SIZE + sizeof(WE_UINT8);             
    ctLen = strlen(contentLocation) + 1;
    bufSize += TAG_SIZE + ctLen;                         

    
    if (bufSize >= MMS_MAX_CHUNK_SIZE)              
    {  
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspForward: Header size exceeded chunk size \n"));
        *result = MMS_RESULT_INVALID_HEADER_SIZE;
    }
    else if ( !mmsValidateAddressList( header->to) ||  
        !mmsValidateAddressList( header->cc) ||
        !mmsValidateAddressList( header->bcc))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspForward: Invalid address size.\n"));
        *result = MMS_RESULT_INVALID_ADDRESS;
    }
    else if (contentLocation == NULL || 
        ctLen > MAX_MMS_ENTRY_HEADER_VALUE_LEN)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspForward: Content Location.\n"));
        *result = MMS_RESULT_INVALID_CONTENT_LOCATION;
    } 

    if (*result != MMS_RESULT_OK)
    {
        WE_MEM_FREE(modId, from); 
        WE_MEM_FREE(modId, transactionId);
        return NULL;
    } 
    

    
    
    pduHeader = WE_MEM_ALLOC(modId, (unsigned int)bufSize);
    
    
    valueOfTag.messageType = (WE_UINT8)MMS_M_FORWARD_REQ;
    
    if (pduHeader == NULL )
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspForward: Failed to allocate memory\n"));
        WE_MEM_FREE(modId, transactionId);
        WE_MEM_FREE(modId, from); 
        WE_MEM_FREE(modId, transactionId);
        transactionId = NULL;
        *result = MMS_RESULT_INSUFFICIENT_MEMORY;
        return NULL;    
    } 
    
    if (!mmsPduAppend( pduHeader,     
        bufSize,            
        &actLength,         
        X_MMS_MESSAGE_TYPE, 
        sizeof(WE_UINT8),      
        valueOfTag))        
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspForward: Error in X-Mms-Message-Type\n"));
        WE_MEM_FREE(modId, transactionId);
        WE_MEM_FREE(modId, from); 
        WE_MEM_FREE(modId, transactionId);
        *result = MMS_RESULT_ERROR_CREATING_FORWARD;
        return NULL;    
    }   
     
    
    valueOfTag.transactionId = transactionId;

    if (!mmsPduAppend(pduHeader, bufSize, &actLength, X_MMS_TRANSACTION_ID, 
        transactionIdLen, valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspForward: Error in X-Mms-Transaction-Id"));
        *result = MMS_RESULT_ERROR_CREATING_FORWARD;
        WE_MEM_FREE(modId, transactionId);
        WE_MEM_FREE(modId, from); 
        WE_MEM_FREE(modId, pduHeader);
        return NULL;    
    }   
 
    WE_MEM_FREE(modId, transactionId);
    transactionId = NULL;
   
    
    valueOfTag.version = (unsigned char)version;

    if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_VERSION, 
        sizeof(WE_UINT8), valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "createWspForward: Error in X-Mms-Version\n"));
        *result = MMS_RESULT_ERROR_CREATING_FORWARD;
        WE_MEM_FREE(modId, from); 
        WE_MEM_FREE(modId, pduHeader);
        return NULL;    
    }   

    
    if (header->date != 0) 
    {
        valueOfTag.date = header->date;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, MMS_DATE, 
            sizeof(WE_UINT32), valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId,
                "createWspForward: Error in Date\n"));
            *result = MMS_RESULT_ERROR_CREATING_FORWARD;
            WE_MEM_FREE(modId, from); 
            WE_MEM_FREE(modId, pduHeader);
            return NULL;    
        }   
     } 

    
    valueOfTag.from = from;

    if (!mmsPduAppend( pduHeader, bufSize, &actLength, MMS_FROM, fromSize, 
        valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId,
            "createWspForward: Could not add From to header\n"));
        *result = MMS_RESULT_INVALID_ADDRESS;
        WE_MEM_FREE(modId, from); 
        WE_MEM_FREE(modId, pduHeader);
        return NULL;    
    }  

    WE_MEM_FREE(modId, from); 
    from = NULL;
    
    
    if (header->to != NULL) 
    {
        if (createDestinationField(modId, header->to, pduHeader, bufSize,
            &actLength, &valueOfTag, MMS_TO) == FALSE )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId,
                    "createWspForward: Could not add To to header\n"));
            *result = MMS_RESULT_INVALID_ADDRESS;
            WE_MEM_FREE(modId, pduHeader);
            pduHeader = NULL;
            return NULL;
        } 
    } 

    
    if (header->cc != NULL) 
    {
        if (createDestinationField(modId, header->cc, pduHeader, bufSize,
            &actLength, &valueOfTag, MMS_CC) == FALSE )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId,
                    "createWspForward: Could not add Cc to header\n"));
            *result = MMS_RESULT_INVALID_ADDRESS;
            WE_MEM_FREE(modId, pduHeader);
            pduHeader = NULL;
            return NULL;
        } 
    } 

    
    if (header->bcc != NULL) 
    {
        if (createDestinationField(modId,header->bcc, pduHeader, bufSize,
            &actLength, &valueOfTag, MMS_BCC) == FALSE )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId,
                    "createWspForward: Could not add Bcc to header\n"));
            *result = MMS_RESULT_INVALID_ADDRESS;
            WE_MEM_FREE(modId, pduHeader);
            pduHeader = NULL;
            return NULL;
        } 
    } 
      
    
    if (header->expiryTime.value != 0)
    {
        if ((codedValue = 
            createTimeField( modId, &size, header->expiryTime.value,
            header->expiryTime.type)) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId,
                    "createWspForward: X-Mms-Expiry conversion failed.\n"));
            *result = MMS_RESULT_ERROR_CREATING_FORWARD;
            WE_MEM_FREE(modId, pduHeader);
        
            return NULL;    
        }  
        
        valueOfTag.expiry = codedValue;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_EXPIRY, size, 
            valueOfTag))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId,
                "createWspForward: Error in X-Mms-Expiry\n"));
            *result = MMS_RESULT_ERROR_CREATING_FORWARD;
            WE_MEM_FREE(modId, pduHeader);;
            pduHeader = NULL;
            WE_MEM_FREE(modId, codedValue);
            codedValue = NULL;
            return NULL;    
        } 

        WE_MEM_FREE(modId, codedValue);
        codedValue = NULL;
    }    

    
    if (header->deliveryTime.value != 0)
    {
        if ((codedValue = 
            createTimeField( modId, &size, header->deliveryTime.value,
            header->deliveryTime.type)) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId,
                "createWspForward: X-Mms-Delivery-Time error.\n"));
            *result = MMS_RESULT_ERROR_CREATING_FORWARD;
            WE_MEM_FREE(modId, pduHeader);;
            pduHeader = NULL;
            


            return NULL;    
        }  
        
        valueOfTag.deliveryTime = codedValue;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_DELIVERY_TIME, 
            size, valueOfTag))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId,
                "createWspForward: Error in X-Mms-Delivery-Time\n"));
            *result = MMS_RESULT_ERROR_CREATING_FORWARD;
            WE_MEM_FREE(modId, pduHeader);
            pduHeader = NULL;
            WE_MEM_FREE(modId, codedValue);
            codedValue = NULL;
            return NULL;    
        } 

        WE_MEM_FREE(modId, codedValue);
        codedValue = NULL;
    }    
    
    
    if (codedValue != NULL)
    {
        WE_MEM_FREE(modId, codedValue);
        codedValue = NULL;
    } 
    

    
    if (header->reportAllowed != MMS_DELIVERY_REPORT_ALLOWED_NOT_SET) 
    {
        valueOfTag.reportAllowed = (WE_UINT8)header->reportAllowed;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_REPORT_ALLOWED, 
         sizeof(WE_UINT8), valueOfTag))
        { 
             WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                 "%s(%d): Error in X-Mms-Report-Allowed\n", __FILE__, __LINE__));
            *result = MMS_RESULT_ERROR_CREATING_FORWARD;
            WE_MEM_FREE(modId, pduHeader);
            pduHeader = NULL;
            return NULL;    
        }   
    }   

    
    if (header->readReply != MMS_READ_REPLY_NOT_SET)
    {
        valueOfTag.readReply = (WE_UINT8)header->readReply;

        if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_READ_REPLY, 
            sizeof(WE_UINT8), valueOfTag))
        { 
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId,
                "createWspForward: Error in X-Mms-Read-Reply\n"));
            *result = MMS_RESULT_ERROR_CREATING_FORWARD;
            WE_MEM_FREE(modId, pduHeader);
            pduHeader = NULL;
            return NULL;    
        }   
    }    
    
    
    valueOfTag.contentLocation = contentLocation;

    if (!mmsPduAppend( pduHeader, bufSize, &actLength, X_MMS_CONTENT_LOCATION, 
        ctLen, valueOfTag))
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId,
            "createWspForward: Could not add Content Location\n"));
        *result = MMS_RESULT_ERROR_CREATING_FORWARD;
        WE_MEM_FREE(modId, pduHeader);
        pduHeader = NULL;
        return NULL;    
    }   

    *length = (WE_UINT32)actLength ; 
    return pduHeader;
} 

