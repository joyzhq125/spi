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
#include "Mmem.h"       
#include "Mcpdu.h"      
#include "Mutils.h"     
#include "Mlpdup.h"     
#include "Mlfieldp.h"   
#include "Mms_Cfg.h"    


















static MmsPriority getPriorityFromValue( const int value);
static MmsSenderVisibility getVisibilityFromValue( const int value);
static MmsReadReply getReadReplyFromValue( const int value);
static MmsDeliveryReport getDeliveryReportValue( const int value);
static unsigned char *getPrevSentBy( WE_UINT8 modId, char *buf, 
    MmsPrevSentBy *prevSentBy, WE_UINT32 bufLen);
static unsigned char *getPrevSentDate( WE_UINT8 modId, char *buf, 
    MmsPrevSentDate *prevSentDate, WE_UINT32 bufLen);












MmsReadOrigInd *parseReadReport( WE_UINT8 modId, unsigned char *pdu, 
    WE_UINT32 len)
{
    MmsReadOrigInd *mmsReadReport = NULL;
    MmsHeaderValue  headerValue;
    WE_UINT32  tempLen = 0;

    if (pdu == NULL)
    {
        return NULL;
    } 

    mmsReadReport = (MmsReadOrigInd *)WE_MEM_ALLOC( modId, sizeof(MmsReadOrigInd));
    if ( mmsReadReport == NULL)
    {
        
        return NULL;
    } 
    memset( mmsReadReport, 0x00, sizeof(MmsReadOrigInd));

    
    if (parseFromAddress( modId, pdu, len, &mmsReadReport->from) == FALSE)
    {
        freeMmsReadOrigInd( modId, mmsReadReport);
        WE_MEM_FREE( modId, mmsReadReport);
        return NULL;
    } 

    
    if (mmsPduGet( pdu, len, MMS_DATE, &headerValue))
    {
        mmsReadReport->date = headerValue.date;
    } 

     
    if (mmsPduGet( pdu, len, MMS_MESSAGE_ID, &headerValue))
    {
        tempLen = strlen((const char *)headerValue.messageId) + 1;
        mmsReadReport->serverMessageId = WE_MEM_ALLOC( modId, tempLen);
        if ( mmsReadReport->serverMessageId == NULL)
        {
            
            freeMmsReadOrigInd( modId, mmsReadReport);
            WE_MEM_FREE( modId, mmsReadReport);
            return NULL;
        } 
        strcpy(mmsReadReport->serverMessageId, headerValue.messageId); 
    } 

    
    if (mmsPduGet( pdu, len, X_MMS_READ_STATUS, &headerValue))
    {
        mmsReadReport->readStatus = (MmsReadStatus)headerValue.readStatus;
    } 

    return mmsReadReport;
} 
  










MmsResult parseMmsNotification( WE_UINT8 modId, unsigned char *headerData, 
    WE_UINT32 headerDataSize, MmsNotification *mmsNotification, 
    MmsVersion *version)
{
    MmsHeaderValue  headerValue;
    WE_UINT32 size;
    WE_UINT32 tempLen = 0;
    MmsResult result = MMS_RESULT_OK;

    if ((mmsNotification == NULL) || (headerData == NULL))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Supplied parameter not correct.",
            __FILE__, __LINE__));   
        return MMS_RESULT_ERROR;
    } 
    
    
    memset( mmsNotification, 0, sizeof(MmsNotification));
    
    
 
    mmsNotification->msgClass.classIdentifier = MMS_MESSAGE_CLASS_NOT_SET;
    mmsNotification->msgClass.textString = NULL;
    mmsNotification->replyChargingId = NULL;

    
    if (parseFromAddress( modId, headerData, headerDataSize, 
        &mmsNotification->from) == FALSE)
    {
        return MMS_RESULT_ERROR;
    } 

    
    if (mmsPduGet(headerData, headerDataSize, X_MMS_VERSION, &headerValue))
    {
        *version = (MmsVersion)headerValue.version;
    }
    else
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Invalid version.", __FILE__, __LINE__));   
        return MMS_RESULT_INVALID_VERSION;
    }  

    
    if ( mmsPduGet( headerData, headerDataSize, MMS_SUBJECT, &headerValue))
    {
        WE_UINT32 notUsed = 0; 
                
        size = (WE_UINT32)(headerData - (unsigned char *)headerValue.subject) + 
            headerDataSize;

        if (parseEncodedStringValue( modId, headerValue.subject, size, &notUsed, 
            &mmsNotification->subject) == FALSE)
        {
            return MMS_RESULT_ERROR;

        } 
    }  

    
    if ( mmsPduGet( headerData, headerDataSize, X_MMS_DELIVERY_REPORT, &headerValue))
    {
        mmsNotification->deliveryReport = (MmsDeliveryReport) headerValue.deliveryReport;
    }
    else
    {
        mmsNotification->deliveryReport = MMS_DELIVERY_REPORT_NOT_SET;
    } 
    
    
    if (mmsPduGet(headerData, headerDataSize, X_MMS_MESSAGE_CLASS, &headerValue))
    {    
        MmsMessageClass mc;
        mc.classIdentifier = 
            (MmsClassIdentifier) *(unsigned char *)headerValue.messageClass;      
        mc.textString = (char *)headerValue.messageClass + 1; 

        switch (mc.classIdentifier)
        {
        case MMS_MESSAGE_CLASS_PERSONAL :
        case MMS_MESSAGE_CLASS_ADVERTISEMENT :
        case MMS_MESSAGE_CLASS_INFORMATIONAL :
        case MMS_MESSAGE_CLASS_AUTO :
            mmsNotification->msgClass.classIdentifier = mc.classIdentifier;
            break;
        case MMS_MESSAGE_CLASS_IS_TEXT :
            mmsNotification->msgClass.classIdentifier = MMS_MESSAGE_CLASS_NOT_SET;

            if (mc.textString != NULL)
            {
                tempLen = strlen(mc.textString) + 1;
                mmsNotification->msgClass.textString = 
                    WE_MEM_ALLOC( modId, tempLen);
                if (mmsNotification->msgClass.textString == NULL)
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                        "Unable to allocate memory in: parseMmsNotification()\n"));
                    freeMmsNotification(modId, mmsNotification);
                    return MMS_RESULT_INSUFFICIENT_MEMORY;                    
                } 

                mmsNotification->msgClass.classIdentifier = 
                    MMS_MESSAGE_CLASS_IS_TEXT;
                strcpy(mmsNotification->msgClass.textString, mc.textString);
            } 
            break;
        case MMS_MESSAGE_CLASS_NOT_SET :
        default :
            mmsNotification->msgClass.classIdentifier = MMS_MESSAGE_CLASS_NOT_SET;
            mmsNotification->msgClass.textString      = NULL;
            break;
        } 
    } 
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "parseMmsNotification: Invalid message class.\n"));
        return MMS_RESULT_INVALID_MSG_CLASS;
    } 

    
    if (mmsPduGet( headerData, headerDataSize, X_MMS_MESSAGE_SIZE, &headerValue))
    {
        mmsNotification->length = headerValue.messageSize;
    }
    else
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Invalid subject.", __FILE__, __LINE__));   
        return MMS_RESULT_INVALID_SIZE;
    }  
    

    
    if (mmsPduGet( headerData, headerDataSize, X_MMS_TRANSACTION_ID, &headerValue) || 
        headerValue.transactionId == NULL)
    {
        tempLen = strlen((const char *)headerValue.transactionId) + 1;
        mmsNotification->transactionId = WE_MEM_ALLOC( modId, tempLen);
        if (mmsNotification->transactionId == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                "Unable to allocate memory in: parseMmsNotification()\n"));
            freeMmsNotification(modId, mmsNotification);
            
            return MMS_RESULT_INSUFFICIENT_MEMORY;                    
        } 
        strcpy(mmsNotification->transactionId, headerValue.transactionId); 
    }
    else
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No X_MMS_TRANSACTION_ID tag\n", __FILE__, __LINE__));
        return MMS_RESULT_TRANSACTION_ID_MISSING; 
    } 


    
    if (mmsPduGet( headerData, headerDataSize, X_MMS_EXPIRY, &headerValue))
    {
        headerValue.expiry = (unsigned char *)headerValue.expiry + 1;

        
        if (*(unsigned char *)headerValue.expiry == MMS_EXPIRY_TIME_ABSOLUTE) 
        {
            mmsNotification->expiryTime.type = MMS_TIME_ABSOLUTE;
            
            
            headerValue.expiry = (unsigned char *)headerValue.expiry + 1;
            size = (WE_UINT32)(headerData - 
                (unsigned char *)headerValue.expiry) + headerDataSize;

            if (cnvLongIntegerToUint32( (unsigned char *)headerValue.expiry, 
                &mmsNotification->expiryTime.value, size) == NULL)
            {
                
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Unable to convert expiry time.\n",
                    __FILE__, __LINE__));   
                mmsNotification->expiryTime.value = 0; 
            }  
        }
        
        else if (*(unsigned char *)headerValue.expiry == MMS_EXPIRY_TIME_RELATIVE) 
        {
            mmsNotification->expiryTime.type = MMS_TIME_RELATIVE;
            
            
            headerValue.expiry = (unsigned char *)headerValue.expiry + 1;
            size = (WE_UINT32)(headerData - 
                (unsigned char *)headerValue.expiry) + headerDataSize;

            



            if (*(unsigned char *)headerValue.expiry >= 128)    
            {
                
                mmsNotification->expiryTime.value = 
                    SKIP_HIGH_BIT( *(unsigned char *)headerValue.expiry);
                    
            }
            else if (cnvLongIntegerToUint32( (unsigned char *)headerValue.expiry, 
               &mmsNotification->expiryTime.value, size) == NULL) 
            {
                
                mmsNotification->expiryTime.value = 0; 
            } 
        }
        else
        {
            
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Unknown token for expiry time.\n", __FILE__, __LINE__));   
            mmsNotification->expiryTime.value = 0;
            mmsNotification->expiryTime.type = MMS_TIME_ABSOLUTE;
            return MMS_RESULT_INVALID_EXPIRY_TIME;
        } 
    } 

    
    if (mmsPduGet( headerData, headerDataSize, X_MMS_CONTENT_LOCATION, &headerValue))
    {
        tempLen = strlen((const char *)headerValue.contentLocation) + 1;
        mmsNotification->contentLocation = WE_MEM_ALLOC( modId, tempLen);
        if (mmsNotification->contentLocation == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                "Unable to allocate memory in: parseMmsNotification()\n"));
            freeMmsNotification(modId, mmsNotification);
                
            return MMS_RESULT_INSUFFICIENT_MEMORY;                    
        } 
        strcpy(mmsNotification->contentLocation, headerValue.contentLocation); 
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "parseMmsNotification: Invalid message class.\n"));
        return MMS_RESULT_INVALID_CONTENT_LOCATION;
    } 


    
    if (mmsPduGet(headerData, headerDataSize, X_MMS_REPLY_CHARGING, &headerValue))
    {
        mmsNotification->replyCharging = 
            (MmsReplyCharging)headerValue.replyCharging;

        
        if (mmsPduGet( headerData, headerDataSize, 
            X_MMS_REPLY_CHARGING_DEADLINE, &headerValue))
        {
            headerValue.replyChargingDeadline = 
                (unsigned char *)headerValue.replyChargingDeadline + 1;

            if (*(unsigned char *)headerValue.replyChargingDeadline == 
                MMS_REPLY_CHARGING_DEADLINE_ABSOLUTE) 
            {
                
                headerValue.replyChargingDeadline = 
                    (unsigned char *)headerValue.replyChargingDeadline + 1;
            
                size = (WE_UINT32)(headerData - 
                    (unsigned char *)headerValue.replyChargingDeadline) + 
                    headerDataSize;

                mmsNotification->replyChargingDeadline.type = MMS_TIME_ABSOLUTE;

                if (cnvLongIntegerToUint32(
                    (unsigned char *)headerValue.replyChargingDeadline, 
                    &mmsNotification->replyChargingDeadline.value, 
                    size) == NULL)
                {
                    
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                        "%s(%d): Unable to convert charging deadline.\n",
                        __FILE__, __LINE__));   
                    mmsNotification->replyChargingDeadline.value = 0; 
                }  
            }
            else if (*(unsigned char *)headerValue.replyChargingDeadline == 
                MMS_REPLY_CHARGING_DEADLINE_RELATIVE) 
            {
                
                headerValue.replyChargingDeadline = 
                    (unsigned char *)headerValue.replyChargingDeadline + 1;
            
                size = (WE_UINT32)(headerData - 
                    (unsigned char *)headerValue.replyChargingDeadline) + 
                    headerDataSize;

                mmsNotification->replyChargingDeadline.type = MMS_TIME_RELATIVE;
                

                


                if (*(unsigned char *)headerValue.replyChargingDeadline >= 128)    
                {
                    
                    mmsNotification->replyChargingDeadline.value = 
                        SKIP_HIGH_BIT(
                        *(unsigned char *)headerValue.replyChargingDeadline);
                }
                else                                                
                {
                    
                    if (cnvLongIntegerToUint32(
                        (unsigned char *)headerValue.replyChargingDeadline, 
                        &mmsNotification->replyChargingDeadline.value, 
                        size) == NULL) 
                    {
                        
                        mmsNotification->replyChargingDeadline.value = 0; 
                        mmsNotification->replyChargingDeadline.type = MMS_TIME_ABSOLUTE;                        
                    } 
                } 
            }
            else
            {
                
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Unknown token for charging deadline.\n", __FILE__, __LINE__));   
                mmsNotification->replyChargingDeadline.value = 0;
                mmsNotification->replyChargingDeadline.type = MMS_TIME_ABSOLUTE;                        
            } 
        } 

        
        if (mmsPduGet(headerData, headerDataSize, X_MMS_REPLY_CHARGING_ID, 
            &headerValue))
        {
            tempLen = strlen((const char *)headerValue.replyChargingId) + 1;
            mmsNotification->replyChargingId = WE_MEM_ALLOC( modId, tempLen);

            if (mmsNotification->replyChargingId == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: parseMmsNotification()\n"));
                freeMmsNotification(modId, mmsNotification);
                
                return MMS_RESULT_INSUFFICIENT_MEMORY;                    
            } 
            
            strcpy(mmsNotification->replyChargingId, headerValue.replyChargingId);
        }

        
        if (mmsPduGet(headerData, headerDataSize, X_MMS_REPLY_CHARGING_SIZE, 
            &headerValue))
        {
            mmsNotification->replyChargingSize = headerValue.replyChargingSize;
        }
        else
        {
            
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Reply-Charging-Size not found.\n", __FILE__, __LINE__));   
            mmsNotification->replyChargingSize = 0;
        } 
    } 

    return result;
} 











WE_BOOL parseDeliveryReport(  WE_UINT8 modId, unsigned char *headerData, 
    WE_UINT32 headerDataSize, MmsDeliveryInd *mmsDelivery)    
{
    MmsHeaderValue  headerValue;
    MmsAddressList  *tmpTo;
    WE_UINT32 tempLen = 0;
  
    if ( headerData == NULL || mmsDelivery == NULL)
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Memmory not allocated!\n", __FILE__, __LINE__));   
        return FALSE;
    } 
    memset( mmsDelivery, 0, sizeof(MmsDeliveryInd));
    
   
     
    if (mmsPduGet(headerData, headerDataSize, X_MMS_VERSION, &headerValue))
    {
        mmsDelivery->version = headerValue.version;
    } 

     
    if (mmsPduGet( headerData, headerDataSize, MMS_MESSAGE_ID, &headerValue))
    {
        tempLen = strlen((const char *)headerValue.messageId) + 1;
        mmsDelivery->messageId = WE_MEM_ALLOC( modId, tempLen);
        if ( mmsDelivery->messageId == NULL)
        {
            
            freeMmsDeliveryInd( modId, mmsDelivery);
            return FALSE;
        } 

        strcpy(mmsDelivery->messageId, headerValue.messageId); 
    } 

     
    mmsDelivery->to = WE_MEM_ALLOC( modId, sizeof(MmsAddress));
    if ( mmsDelivery->to == NULL)
    {
        
        freeMmsDeliveryInd( modId, mmsDelivery);
        return FALSE;
    } 

    tmpTo = parseAddress( modId, MMS_TO, headerData, headerDataSize);
    if (tmpTo == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Unable to parse TO in delivery report.",
            __FILE__, __LINE__));
    }
    else
    {
        mmsDelivery->to->name = tmpTo->current.name;
        mmsDelivery->to->address = tmpTo->current.address;
        mmsDelivery->to->addrType = tmpTo->current.addrType;
        WE_MEM_FREE( modId, tmpTo); 
        tmpTo = NULL;
    } 

    
    if (mmsPduGet( headerData, headerDataSize, MMS_DATE, &headerValue))
    {
        mmsDelivery->date = headerValue.date;
    } 
  
     
    if (mmsPduGet( headerData, headerDataSize, X_MMS_STATUS, &headerValue))
    {
        switch (headerValue.status)
        {
        case MMS_STATUS_EXPIRED: 
           mmsDelivery->status = MMS_STATUS_EXPIRED;
           break;
        case MMS_STATUS_RETRIEVED: 
           mmsDelivery->status = MMS_STATUS_RETRIEVED;
           break;
        case MMS_STATUS_REJECTED:     
           mmsDelivery->status = MMS_STATUS_REJECTED;
           break;
        case MMS_STATUS_DEFERRED:     
           mmsDelivery->status = MMS_STATUS_DEFERRED;
           break;
        case MMS_STATUS_INDETERMINATE:
            mmsDelivery->status = MMS_STATUS_INDETERMINATE;
            break;
        case MMS_STATUS_FORWARDED:
            mmsDelivery->status = MMS_STATUS_FORWARDED;
            break;
        case MMS_STATUS_UNRECOGNIZED: 
        default: 
           mmsDelivery->status = MMS_STATUS_UNRECOGNIZED;
        } 
    } 
    return TRUE;
} 











MmsResult parseMmsHeader(  WE_UINT8 modId, unsigned char *headerData, 
    WE_UINT32 headerDataSize, MmsGetHeader *mHeader)
{
    MmsHeaderValue  headerValue;
    WE_UINT32 size;
    WE_UINT32 tempLen = 0;
    unsigned char *p = NULL;
    MmsPrevSentBy *currentSentBy = NULL;
    MmsPrevSentDate *currentSentDate = NULL;
    
    if ( headerData == NULL || headerDataSize == 0)
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "parseMmsHeader: Invalid input parameters.\n"));
        return MMS_RESULT_ERROR;
    } 
    
     
    memset( mHeader, 0, sizeof( MmsGetHeader));
    mHeader->priority = MMS_PRIORITY_NOT_SET;
    mHeader->visibility = MMS_SENDER_VISIBILITY_NOT_SET;
    mHeader->readReply = MMS_READ_REPLY_NOT_SET;
    mHeader->msgClass.classIdentifier = MMS_MESSAGE_CLASS_NOT_SET;
    mHeader->msgClass.textString      = NULL;
    mHeader->previouslySentBy         = NULL;
    mHeader->previouslySentDate       = NULL;
    mHeader->replyChargingId          = NULL;

    
    
     
    if (mmsPduGet( headerData, headerDataSize, 
        X_MMS_MESSAGE_TYPE, &headerValue) == FALSE) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "parseMmsHeader: Invalid message, message type not found.\n"));
        return MMS_RESULT_ERROR;
    } 

    mHeader->messageType = (MmsMsgType) headerValue.messageType; 

    
    if ( parseFromAddress( modId, headerData, headerDataSize, &mHeader->from) == FALSE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "parseMmsHeader: Invalid message, from address not found.\n"));
        return MMS_RESULT_INVALID_ADDRESS;
    } 

    
    if (mmsPduGet(headerData, headerDataSize, X_MMS_VERSION, &headerValue))
    {
        mHeader->version = (MmsVersion)headerValue.version;
    }
    else 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "parseMmsHeader: Invalid message, version not found.\n"));
        freeMmsAddress( modId, &mHeader->from);
        
        return MMS_RESULT_INVALID_VERSION;
    } 

    
    
     if (mmsPduGet( headerData, headerDataSize, MMS_DATE, &headerValue))
     {
        mHeader->date = headerValue.date;
     }
#if MMS_STRICT_PDU_CHECK
     else if ( mHeader->messageType != MMS_MSG_TYPE_SEND_REQ)
     {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "parseMmsHeader: Invalid message, date not found.\n"));
         freeMmsAddress( modId, &mHeader->from);

         return MMS_RESULT_INVALID_DATE;
     }
#endif  

    
    if (mmsPduGet(headerData, headerDataSize, MMS_CONTENT_TYPE, &headerValue))
    {
        size = (WE_UINT32)(headerData - 
            (unsigned char *)headerValue.contentType) + headerDataSize;
        if (parseContentType( modId, &mHeader->contentType, 
            (unsigned char *)headerValue.contentType, size) == NULL)
        {
            
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "parseMmsHeader: Invalid content type.\n"));
            freeMmsAddress( modId, &mHeader->from);

            return MMS_RESULT_INVALID_CONTENT_TYPE;
        } 
    } 
    else
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "parseMmsHeader: Invalid content type.\n"));
        freeMmsAddress( modId, &mHeader->from);

        return MMS_RESULT_INVALID_CONTENT_TYPE;
    } 

    
    mHeader->to = parseAddress( modId, MMS_TO, headerData, headerDataSize); 
    mHeader->cc = parseAddress( modId, MMS_CC, headerData, headerDataSize); 
    mHeader->bcc = parseAddress( modId, MMS_BCC, headerData, headerDataSize);

    
    if (mmsPduGet( headerData, headerDataSize, MMS_SUBJECT, &headerValue))
    {
        WE_UINT32 notUsed = 0; 
        size = (WE_UINT32)(headerData - (unsigned char *)headerValue.subject) + 
            headerDataSize;

        if (parseEncodedStringValue( modId, headerValue.subject, size, &notUsed, 
            &mHeader->subject) == FALSE)
        {
             WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "parseMmsHeader: Failed to encode .\n"));
            freeMmsAddress( modId, &mHeader->from);
        } 
    }   

    
    if (mmsPduGet(headerData, headerDataSize, X_MMS_MESSAGE_CLASS, &headerValue))
    {
        MmsMessageClass mc;
        
        mc.classIdentifier = 
            (MmsClassIdentifier) *(unsigned char *)headerValue.messageClass; 

        if (mc.classIdentifier <= SHORT_INTEGER_MAX_VALUE)
        {
            mc.textString = (char *)headerValue.messageClass;
            mHeader->msgClass.classIdentifier = MMS_MESSAGE_CLASS_NOT_SET;
            mHeader->msgClass.textString      = NULL;

            if (mc.textString != NULL)
            {
                tempLen = strlen(mc.textString) + 1;
                mHeader->msgClass.textString = WE_MEM_ALLOC( modId, tempLen);
                if ( mHeader->msgClass.textString == NULL)
                {
                    
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                        "parseMmsHeader: Unable to allocate memory.\n"));
                    freeMmsAddress( modId, &mHeader->from);

                    return MMS_RESULT_INSUFFICIENT_MEMORY;
                } 

                memset( mHeader->msgClass.textString, 0, tempLen);
                mHeader->msgClass.classIdentifier = MMS_MESSAGE_CLASS_IS_TEXT;
                strcpy(mHeader->msgClass.textString, mc.textString);
            } 
        }
        else
        {
            switch (mc.classIdentifier)
            {
            case MMS_MESSAGE_CLASS_PERSONAL :
            case MMS_MESSAGE_CLASS_ADVERTISEMENT :
            case MMS_MESSAGE_CLASS_INFORMATIONAL :
            case MMS_MESSAGE_CLASS_AUTO :
                mHeader->msgClass.classIdentifier = mc.classIdentifier;
                mHeader->msgClass.textString      = NULL;
                break;
            case MMS_MESSAGE_CLASS_NOT_SET :
            case MMS_MESSAGE_CLASS_IS_TEXT:         
            default :
                mHeader->msgClass.classIdentifier = MMS_MESSAGE_CLASS_NOT_SET;
                mHeader->msgClass.textString      = NULL;
                break;
            } 
        } 
    } 

    
    if (parseHeaderTime( headerData, headerDataSize, mHeader, 
        X_MMS_EXPIRY) == FALSE)
    {
        mHeader->expiryTime.value = 0;
        mHeader->expiryTime.type = MMS_TIME_ABSOLUTE;
    } 

    
    if (parseHeaderTime( headerData, headerDataSize, mHeader, 
        X_MMS_DELIVERY_TIME) == FALSE)
    {
        mHeader->deliveryTime.value = 0;
        mHeader->deliveryTime.type = MMS_TIME_ABSOLUTE;
    } 

    
    if (mmsPduGet(headerData, headerDataSize, X_MMS_PRIORITY, &headerValue))
    {
        mHeader->priority = getPriorityFromValue( headerValue.priority);
    } 

    
    if (mmsPduGet(headerData, headerDataSize, X_MMS_SENDER_VISIBILITY,
        &headerValue))
    {
        mHeader->visibility = getVisibilityFromValue(
            headerValue.senderVisibility);
    } 

    
    if (mmsPduGet(headerData, headerDataSize, X_MMS_READ_REPLY, &headerValue))
    {
        mHeader->readReply = getReadReplyFromValue( headerValue.readReply);
    } 

    
    if (mmsPduGet(headerData, headerDataSize, X_MMS_DELIVERY_REPORT, &headerValue))
    {
        mHeader->deliveryReport = 
            getDeliveryReportValue( headerValue.deliveryReport);
    } 
    
     
    if (mmsPduGet( headerData, headerDataSize, MMS_MESSAGE_ID, &headerValue))
    {
        tempLen = strlen((const char *)headerValue.messageId) + 1;
        mHeader->serverMessageId = WE_MEM_ALLOC( modId, tempLen);
        if (mHeader->serverMessageId == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                "Unable to allocate memory in: parseMmsHeader()\n"));
            freeMmsGetHeader(modId, mHeader);
            return MMS_RESULT_INSUFFICIENT_MEMORY;                    
        }

        strcpy(mHeader->serverMessageId, headerValue.messageId); 
    } 
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "parseMmsHeader: Server message wid not found.\n"));
        mHeader->serverMessageId = NULL;   
    }

    
    if (mmsPduGet(headerData, headerDataSize, X_MMS_REPLY_CHARGING, &headerValue))
    {
        mHeader->replyCharging = (MmsReplyCharging)headerValue.replyCharging;

        
        if (mmsPduGet( headerData, headerDataSize, 
            X_MMS_REPLY_CHARGING_DEADLINE, &headerValue))
        {
            headerValue.replyChargingDeadline = 
                (unsigned char *)headerValue.replyChargingDeadline + 1;

            if (*(unsigned char *)headerValue.replyChargingDeadline == 
                MMS_REPLY_CHARGING_DEADLINE_ABSOLUTE) 
            {
                mHeader->replyChargingDeadline.type = MMS_TIME_ABSOLUTE;
                
                headerValue.replyChargingDeadline = 
                    (unsigned char *)headerValue.replyChargingDeadline + 1;
            
                size = (WE_UINT32)(headerData - 
                    (unsigned char *)headerValue.replyChargingDeadline) + 
                    headerDataSize;

                if (cnvLongIntegerToUint32((unsigned char *)headerValue.replyChargingDeadline, 
                    &mHeader->replyChargingDeadline.value, size) == NULL)
                {
                    
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                        "%s(%d): Unable to convert charging deadline.\n",
                        __FILE__, __LINE__));   
                    mHeader->replyChargingDeadline.value = 0; 
                }  
            }
            else if (*(unsigned char *)headerValue.replyChargingDeadline == 
                MMS_REPLY_CHARGING_DEADLINE_RELATIVE) 
            {
                headerValue.replyChargingDeadline = 
                    (unsigned char *)headerValue.replyChargingDeadline + 1;
            
                size = (WE_UINT32)(headerData - 
                    (unsigned char *)headerValue.replyChargingDeadline) + 
                    headerDataSize;

                mHeader->replyChargingDeadline.type = MMS_TIME_RELATIVE;
                
                


                if (*(unsigned char *)headerValue.replyChargingDeadline >= 128)    
                {
                    mHeader->replyChargingDeadline.value = 
                        *(unsigned char *)headerValue.replyChargingDeadline & 0x7f;
                }
                else                                                
                {
                    if (cnvLongIntegerToUint32((unsigned char *)headerValue.replyChargingDeadline, 
                       &mHeader->replyChargingDeadline.value, size) == NULL) 
                    {
                        mHeader->replyChargingDeadline.value = 0; 
                        mHeader->replyChargingDeadline.type = MMS_TIME_ABSOLUTE;                        
                    } 
                } 
            }
            else
            {
                
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Unknown token for charging deadline.\n", __FILE__, __LINE__));   
                mHeader->replyChargingDeadline.value = 0;
                mHeader->replyChargingDeadline.type = MMS_TIME_ABSOLUTE;                        
                freeMmsAddress( modId, &mHeader->from);

                return MMS_RESULT_INVALID_REPLY_CHARGING_DEADLINE;
            }
        } 

        
        if (mmsPduGet(headerData, headerDataSize, 
            X_MMS_REPLY_CHARGING_ID, &headerValue))
        {
            tempLen = strlen((const char *)headerValue.replyChargingId) + 1;
            mHeader->replyChargingId = WE_MEM_ALLOC( modId, tempLen);
            if (mHeader->replyChargingId == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: parseMmsHeader()\n"));
                freeMmsGetHeader(modId, mHeader);
                return MMS_RESULT_INSUFFICIENT_MEMORY;                    
            }

            strcpy(mHeader->replyChargingId, headerValue.replyChargingId);
        } 
        else
        {
            mHeader->replyChargingId = NULL;
        }

        
        if (mmsPduGet(headerData, headerDataSize, 
            X_MMS_REPLY_CHARGING_SIZE, &headerValue))
        {
            mHeader->replyChargingSize = headerValue.replyChargingSize;
        } 
    } 
    else
    {
        mHeader->replyCharging = MMS_REPLY_CHARGING_NOT_SET;  
    }

    
    p = headerData;
    size = headerDataSize;
    while ((p != NULL) && ((WE_UINT32)(p - headerData) < headerDataSize))
    {
        if (mmsPduGet(p, headerDataSize, X_MMS_PREVIOUSLY_SENT_BY, &headerValue))
        {
            if (p == headerData)
            {
                mHeader->previouslySentBy = WE_MEM_ALLOC( modId, sizeof(MmsPrevSentBy));
                if (mHeader->previouslySentBy == NULL)
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                        "Unable to allocate memory in: parseMmsHeader()\n"));
                    freeMmsGetHeader(modId, mHeader);
                    return MMS_RESULT_INSUFFICIENT_MEMORY;                    
                }
                memset(mHeader->previouslySentBy, 0, sizeof(MmsPrevSentBy));
                
                currentSentBy = mHeader->previouslySentBy;
                currentSentBy->next = NULL;
            }
            else if (currentSentBy != NULL)
            {
                currentSentBy->next = WE_MEM_ALLOC( modId, sizeof(MmsPrevSentBy));
                if (currentSentBy->next == NULL)
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                        "Unable to allocate memory in: parseMmsHeader()\n"));
                    freeMmsGetHeader(modId, mHeader);
                    return MMS_RESULT_INSUFFICIENT_MEMORY;                    
                }

                memset(currentSentBy->next, 0, sizeof(MmsPrevSentBy));
                currentSentBy = currentSentBy->next;
                currentSentBy->next = NULL;
            } 

            p = getPrevSentBy( modId, (char *)headerValue.previouslySentBy, 
                currentSentBy, (WE_UINT32)((unsigned char *)headerValue.previouslySentBy - 
                headerData));
            if (p != NULL)
            {
                
                ++p;
                size = headerDataSize - (WE_UINT32)(p - headerData);
            } 
        } 
        else 
        {
            p = NULL;
        } 
    } 

    
    p = headerData;
    while ((p != NULL) && ((WE_UINT32)(p - headerData) < headerDataSize))
    {
        if (mmsPduGet(p, headerDataSize, 
            X_MMS_PREVIOUSLY_SENT_DATE, &headerValue))
        {
            if (p == headerData)
            {
                mHeader->previouslySentDate = WE_MEM_ALLOC( modId, sizeof(MmsPrevSentDate));
                if (mHeader->previouslySentDate == NULL)
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                        "Unable to allocate memory in: parseMmsHeader()\n"));
                    freeMmsGetHeader(modId, mHeader);
                    return MMS_RESULT_INSUFFICIENT_MEMORY;                    
                }
                memset(mHeader->previouslySentDate, 0, sizeof(MmsPrevSentDate));
                currentSentDate = mHeader->previouslySentDate;
                currentSentDate->next = NULL;
            }
            else if (currentSentDate != NULL)
            {
                currentSentDate->next = WE_MEM_ALLOC( modId, sizeof(MmsPrevSentDate));
                if (currentSentDate->next == NULL)
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                        "Unable to allocate memory in: parseMmsHeader()\n"));
                    freeMmsGetHeader(modId, mHeader);
                    return MMS_RESULT_INSUFFICIENT_MEMORY;                    
                }
                memset(currentSentDate->next, 0, sizeof(MmsPrevSentDate));
                currentSentDate = currentSentDate ->next;
                currentSentDate->next = NULL;
            } 

            p = getPrevSentDate( modId, (char *)headerValue.previouslySentDate, 
                currentSentDate, (WE_UINT32)((unsigned char *)headerValue.previouslySentDate - 
                headerData));
        } 
        else
        {
            p = NULL;   
        } 
    }
    return MMS_RESULT_OK;
} 









static unsigned char *getPrevSentBy(  WE_UINT8 modId, char *buf, 
    MmsPrevSentBy *prevSentBy, WE_UINT32 bufLen)
{
    WE_UINT32 length = 0;
    unsigned char *p = NULL;
    WE_UINT32 size = 0;

    if (buf == NULL || prevSentBy == NULL || bufLen == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): getPrevSentBy Indata was NULL.",
            __FILE__, __LINE__));
        return NULL;
    } 
    p = cnvValueLengthToUint32(buf, &length, bufLen);
        
    if (*p >= 128)
    {
        
        p = cnvShortIntegerToUchar( p, 
            (unsigned char *)&prevSentBy->forwardedCountValue, 
            (bufLen - length));
    }
    else
    {
        
        p = cnvLongIntegerToUint32( p, &prevSentBy->forwardedCountValue, 
            (bufLen - length));
    } 
    
    size = bufLen - (WE_UINT32)((char *)p - buf);

    
    p = parseOneAddress( modId, p, size, &prevSentBy->sentBy);
    
    return p;
} 









static unsigned char *getPrevSentDate( WE_UINT8 modId, char *buf, 
    MmsPrevSentDate *prevSentDate, WE_UINT32 bufLen)
{
    WE_UINT32 length = 0;
    unsigned char *p = NULL;

    if (buf == NULL || prevSentDate == NULL || bufLen == 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "%s(%d): getPrevSentDate Indata was NULL.",
            __FILE__, __LINE__));
        return NULL;
    } 

    p = cnvValueLengthToUint32(buf, &length, bufLen);
    
    if (*p >= 128)
    {
        
        p = cnvShortIntegerToUchar( p, 
            (unsigned char *)&prevSentDate->forwardedCountValue, 
            (bufLen - length));
    }
    else
    {
        
        p = cnvLongIntegerToUint32( p, &prevSentDate->forwardedCountValue, 
            (bufLen - length));
    } 
    
    length = bufLen - (WE_UINT32)((char *)p - buf);

    
    p = cnvLongIntegerToUint32( p, &prevSentDate->date, length);
    
    return p;
} 







static MmsPriority getPriorityFromValue( const int value)
{
   switch (value)
   {
   case MMS_PRIORITY_LOW:  
       return MMS_PRIORITY_LOW;
   case MMS_PRIORITY_NORMAL:    
       return MMS_PRIORITY_NORMAL;
   case MMS_PRIORITY_HIGH:   
       return MMS_PRIORITY_HIGH; 
   case MMS_PRIORITY_NOT_SET:
   default:     
       return MMS_PRIORITY_NOT_SET;
   } 
} 







static MmsSenderVisibility getVisibilityFromValue( const int value)
{
    switch (value)
    {   
    case MMS_SENDER_HIDE:  
        return MMS_SENDER_HIDE;
    case MMS_SENDER_SHOW:       
        return MMS_SENDER_SHOW;
    case MMS_SENDER_VISIBILITY_NOT_SET:
        
    default:
        return MMS_SENDER_VISIBILITY_NOT_SET;
    } 
} 







static MmsReadReply getReadReplyFromValue( const int value)
{
    switch (value)
    {   
    case MMS_READ_REPLY_YES:
        return MMS_READ_REPLY_YES;
    case MMS_READ_REPLY_NO:
        return MMS_READ_REPLY_NO;     
    case MMS_READ_REPLY_NOT_SET:
        
    default:
        return MMS_READ_REPLY_NOT_SET;
    } 
} 








static MmsDeliveryReport getDeliveryReportValue( const int value)
{
    switch(value)
    {
    case MMS_DELIVERY_REPORT_YES:
        return MMS_DELIVERY_REPORT_YES;
    case MMS_DELIVERY_REPORT_NO:
        return MMS_DELIVERY_REPORT_NO; 
    case MMS_DELIVERY_REPORT_NOT_SET:
    default: 
        return MMS_DELIVERY_REPORT_NOT_SET; 
    }  
} 










void parseForwardConf(  unsigned char *headerData, WE_UINT32 headerDataSize, 
    MmsForwardConf *mmsForwardConf)    
{
    MmsHeaderValue  headerValue;
    WE_UINT32 tempLen = 0;

     
    if (mmsPduGet(headerData, headerDataSize, X_MMS_VERSION, &headerValue))
    {
        mmsForwardConf->version = headerValue.version;
    } 

     
    if (mmsPduGet( headerData, headerDataSize, MMS_MESSAGE_ID, &headerValue))
    {
        tempLen = strlen((const char *)headerValue.messageId) + 1;
        mmsForwardConf->messageId = M_ALLOC( tempLen);
        strcpy(mmsForwardConf->messageId, headerValue.messageId); 
    } 

     
    if (mmsPduGet( headerData, headerDataSize, X_MMS_RESPONSE_TEXT, &headerValue))
    {
        


        tempLen = strlen((const char *)headerValue.responseText) + 1;
        mmsForwardConf->responseText = M_ALLOC( tempLen);
        strcpy(mmsForwardConf->responseText, headerValue.responseText); 
    } 

     
    if (mmsPduGet( headerData, headerDataSize, X_MMS_STATUS, &headerValue))
    {
        mmsForwardConf->status = checkStatus(headerValue.status);
    } 
} 

