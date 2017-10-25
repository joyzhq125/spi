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

#include "Mms_Def.h"    
#include "Mms_Cfg.h"    
#include "Msig.h"       
#include "Mmem.h"       
#include "Mcpdu.h"      
#include "Mcpost.h"     
#include "Mlpduc.h"     
#include "Mutils.h"     
#include "Mconfig.h"    
#include "Mcwsp.h"      


#define POS_TYPE_TAG            0   
#define POS_TYPE_DATA           1   



#define TRANSACTION_ID_ERROR    ((char *)"0")


#define ALLOWED_STRING_CHAR_LIMIT   (unsigned char)128











#define MIN_PDU_LENGTH          6

#define CR 0x0d
#define LF 0x0a
#define TAB 0x09










#define VALID_APPLICATION_HEADER(a)  (((a) > 31 && (a) < 127) || \
    (a) == CR || (a) == LF || (a) == TAB)






static WE_BOOL extractValue(unsigned char *pos, MmsHeaderTag tag, MmsHeaderValue *value,
    unsigned long size);
static WE_UINT8 lenLongInteger(WE_UINT32 n);
static WE_UINT8 lenUintvar(WE_UINT32 n);
static unsigned char *nextTag(unsigned char *pos, unsigned long length);
static unsigned char *skipApplicationHeaderString(unsigned char *pos, unsigned long length);
static unsigned char *skipContentType(unsigned char *pos, unsigned long length);


















void *cnvLongIntegerToUint32( void *from, WE_UINT32 *to, unsigned long size)
{
    WE_UINT32 tmp = 0L;
    unsigned char *ptr = (unsigned char *)from;
    int len = *ptr;

    if (len > 4 || (unsigned long)len > size)
    {
        return NULL;
    } 

    do
    {
        tmp += *++ptr;
        if (--len > 0)
        {
            tmp <<= 8;
        } 
    }  while (len > 0);

    *to = tmp;

    return ++ptr;
} 













void *cnvShortIntegerToUchar( void *from, unsigned char *to, unsigned long size)
{
    unsigned char *ptr = (unsigned char *)from;

    if (size < 1)
    {
        return NULL;
    } 

    *to = SKIP_HIGH_BIT(*ptr);

    return ++ptr;
} 











void *cnvUcharToShortInteger( unsigned char from, void *to, unsigned long size)
{
    unsigned char *ptr = (unsigned char *)to;

    if (size < 1)
    {
        return NULL;
    } 

    *ptr++ = SET_HIGH_BIT(from);

    return ptr;
} 











void *cnvUint32ToLongInteger( WE_UINT32 from, void *to, unsigned long size)
{
    WE_UINT8 len = lenLongInteger(from);
    unsigned char *ptr = (unsigned char *)to;
    
    if (len > size)
    {
        return NULL;
    } 

    *ptr++ = len;
    for (; len > 0; --len) 
    {
        *ptr++ = (WE_UINT8)((from >> ((len - 1) * 8)) & 0xff);
    } 

    return ptr;
} 











void *cnvUint32ToUintvar( WE_UINT32 from, void *to, unsigned long size)
{
    WE_UINT8 len = lenUintvar(from);
    unsigned char b;
    unsigned char *ptr = (unsigned char *)to;
    
    if (len > size)
    {
        return NULL;
    } 

    for (; len > 0; --len) 
    {
        b = (WE_UINT8)((from >> ((len - 1) * 7)) & 0x7f);
        if (len > 1)
        {
            b = SET_HIGH_BIT(b);
        } 

        *ptr++ = b;
    } 

    return ptr;
} 














void *cnvUint32ToValueLength( WE_UINT32 from, void *to, unsigned long size)
{
    unsigned char *ptr = (unsigned char *)to;

    if (size < 1)
    {
        ptr = NULL;
    }
    else if (from < LENGTH_QUOTE)
    {
        *ptr++ = (unsigned char)from;
    }
    else if (size < 2)
    {
        ptr = NULL;
    }
    else 
    {
        *ptr++ = LENGTH_QUOTE;
        ptr = (unsigned char *)cnvUint32ToUintvar( from, ptr, size - 1);
    } 

    return ptr;
} 

















void *cnvUintvarToUint32( void *from, WE_UINT32 *to, unsigned long size)
{
    WE_UINT32  n = 0;
    int     i = 0;
    unsigned char *ptr = (unsigned char *)from;
    unsigned char b;

    do
    {
        if (i >= UINTVAR_MAX_LENGTH || (unsigned long)i >= size)
        {
            return NULL;
        } 

        b = *ptr++;
        n <<= 7;
        n |= (b & 0x7f);
        ++i;
    } while (b > 0x7f);

    *to = n;
    return ptr;
} 











void *cnvValueLengthToUint32( void *from, WE_UINT32 *to, unsigned long size)
{
    unsigned char *ptr = (unsigned char *)from;

    if (size < 1)
    {
        ptr = NULL;
    }
    else if (*ptr < LENGTH_QUOTE)
    {
        *to = (WE_UINT32)*ptr++;
    }
    else
    {
        ptr = (unsigned char *)cnvUintvarToUint32( ptr + OCTET_SIZE, to, size); 
    } 

    return ptr;
} 





























void *cnvTextValueToRFC2616String( void *from, WE_UINT8 modId, unsigned long size, unsigned char **result)
{
    WE_UINT32  n = 0;
    unsigned char *ptr = (unsigned char *)from;

    


    *result = NULL;

    if (*ptr == 0)
    { 
        ++ptr;
        return ptr;
    }
    else if (*ptr == 0x22)
    { 
        
        n=1;
        ++ptr;

        
        while (n<size && *ptr != 0)
        {
            ++n;
            ++ptr;
            




        }
        if (*ptr != 0 || n == 1)
        {
            


            return NULL;
        }

        *result = WE_MEM_ALLOC( modId, n);
        if ( *result == NULL)
        {
            
            return NULL;
        } 
        
        if (n>1)
        {
            memcpy( *result, ((unsigned char*)from)+1, n);
        }
        ++ptr;
        return ptr;
    }
    else
    { 
        if (*ptr == 0x7f) 
        {
            


            ++ptr;
            from = ((unsigned char*)from)+1;
            --size;
        }

        
        while (n<size && *ptr != 0)
        {
            ++n;
            ++ptr;
            












        }
        if (*ptr != 0)
        {
            


            return NULL;
        }

        *result = WE_MEM_ALLOC( modId, n+1);
        if ( *result == NULL)
        {
            
            return NULL;
        } 
        
        memcpy( *result, from, n+1);
        ++ptr;
        return ptr;
    }
} 














static WE_BOOL extractValue(unsigned char *pos, MmsHeaderTag tag, 
    MmsHeaderValue *value, unsigned long size)
{
    WE_BOOL ret = TRUE;

    switch (tag)
    {
    case MMS_BCC :
        value->bcc = ++pos;
        break;
    case MMS_CC :
        value->cc = ++pos;
        break;
    case X_MMS_CONTENT_LOCATION :
        value->contentLocation = ++pos;
        break;
    case MMS_CONTENT_TYPE :
        value->contentType = ++pos;
        break;
    case MMS_DATE :
        if ( !cnvLongIntegerToUint32( ++pos, &value->date, size - 1) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "Date conversion failed.\n"));
            ret = FALSE;
        } 
        break;
    case X_MMS_DELIVERY_REPORT :
        value->deliveryReport = *++pos;
        break;
    case X_MMS_DELIVERY_TIME :
        value->deliveryTime = ++pos;
        break;
    case X_MMS_EXPIRY :
        value->expiry = ++pos;
        break;
    case MMS_FROM :
        value->from = ++pos;
        break;
    case X_MMS_MESSAGE_CLASS :
        value->messageClass = ++pos;
        break;
    case MMS_MESSAGE_ID :
        value->messageId = (char *)++pos;
        break;
    case X_MMS_MESSAGE_TYPE :
        value->messageType = *++pos;
        break;
    case X_MMS_VERSION :
        value->version = SKIP_HIGH_BIT(*++pos);
        break;
    case X_MMS_MESSAGE_SIZE :
        if ( !cnvLongIntegerToUint32( ++pos, &value->messageSize, size - 1) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "Message Size conversion failed.\n"));
            ret = FALSE;
        } 
        break;
    case X_MMS_PRIORITY :
        value->priority = *++pos;
        break;
    case X_MMS_READ_REPLY :
        value->readReply = *++pos;
        break;
    case X_MMS_REPORT_ALLOWED :
        value->reportAllowed = *++pos;
        break;
    case X_MMS_RESPONSE_STATUS :
        value->status = *++pos;
        break;
    case X_MMS_RESPONSE_TEXT :
        value->responseText = ++pos;
        break;
    case X_MMS_SENDER_VISIBILITY :
        value->senderVisibility = *++pos;
        break;
    case X_MMS_STATUS :
        value->status = *++pos;
        break;
    case MMS_SUBJECT :
        value->subject = ++pos;
        break;
    case MMS_TO :
        value->to = ++pos;
        break;
    case X_MMS_TRANSACTION_ID :
        value->transactionId = (char *)++pos;
        break;
    case X_MMS_RETRIEVE_STATUS :
        value->retrieveStatus = *++pos;
        break;
    case X_MMS_RETRIEVE_TEXT :
        value->retrieveText = ++pos;
        break;
    case X_MMS_READ_STATUS :
        value->readStatus = *++pos;
        break;
    case X_MMS_REPLY_CHARGING :
        value->replyCharging = *++pos;
        break;
    case X_MMS_REPLY_CHARGING_DEADLINE :
        value->replyChargingDeadline = ++pos;
        break;
    case X_MMS_REPLY_CHARGING_ID :
        value->replyChargingId = (char *)++pos;
        break;
    case X_MMS_REPLY_CHARGING_SIZE :
        if ( !cnvLongIntegerToUint32( ++pos, &value->replyChargingSize, size - 1) )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "Conversion of ReplyChargingSize failed.\n"));
            ret = FALSE;
        } 
        break;
    case X_MMS_PREVIOUSLY_SENT_BY :
        value->previouslySentBy = ++pos;
        break;
    case X_MMS_PREVIOUSLY_SENT_DATE :
        value->previouslySentDate = ++pos;
        break;
    case APPLICATION_HEADER :
    default :
        ret = FALSE;
        break;
    } 

    return ret;
} 







static WE_UINT8 lenUintvar(WE_UINT32 n)
{
    if (n < 0x80)
    {
        return 1;
    }
    else if (n < 0x4000)
    {
        return 2;
    }
    else if (n < 0x200000)
    {
        return 3;
    }
    else if (n < 0x10000000)
    {
        return 4;
    }
    else
    {
        return 5;
    } 
} 







static WE_UINT8 lenLongInteger(WE_UINT32 n)
{
    if (n < 0x100)
    {
        return 1;
    }
    else if (n < 0x10000)
    {
        return 2;
    }
    else if (n < 0x1000000)
    {
        return 3;
    }
    else 
    {
        return 4;
    } 
} 













void *mmsGetBody( void *data, unsigned long size)
{
    unsigned char *offset = (unsigned char *)data;
    unsigned char *start = (unsigned char *)data;

    while (offset != NULL && (unsigned long)(offset - start) < size)
    {
        
        if (SKIP_HIGH_BIT(*offset) == MMS_CONTENT_TYPE)
        {
            offset = nextTag( offset, (unsigned long)(start - offset) + size);

            return (unsigned long)(offset - start) > size ? NULL : offset;
        } 

        offset = nextTag( offset, (unsigned long)(start - offset) + size);
    } 

    return NULL;
} 


















WE_BOOL mmsPduAppend( void *header, unsigned long size, unsigned long *length,
    MmsHeaderTag tag, unsigned long valueSize, MmsHeaderValue value)
{
    unsigned char *from;
    unsigned char *to;
    unsigned long remainingSize = size - *length;
    WE_BOOL ret = TRUE;

    if (remainingSize <= 1 || remainingSize < valueSize)
    {
        return FALSE;   
    } 

    to = &((unsigned char *)header)[*length];
    *to++ = SET_HIGH_BIT(tag);
    --remainingSize;
    ++*length; 

    


    switch (tag)
    {
    case APPLICATION_HEADER :
    case MMS_BCC :
    case MMS_CC :
    case X_MMS_CONTENT_LOCATION :
    case MMS_CONTENT_TYPE :
    case X_MMS_DELIVERY_TIME :
    case X_MMS_EXPIRY :
    case MMS_FROM :
    case X_MMS_MESSAGE_CLASS :
    case MMS_MESSAGE_ID :
    case X_MMS_PREVIOUSLY_SENT_BY :
    case X_MMS_PREVIOUSLY_SENT_DATE :
    case X_MMS_REPLY_CHARGING_DEADLINE :
    case X_MMS_REPLY_CHARGING_ID :
    case X_MMS_RESPONSE_TEXT :
    case X_MMS_RETRIEVE_TEXT :
    case MMS_SUBJECT :
    case MMS_TO :
    case X_MMS_TRANSACTION_ID :
        from = (unsigned char *)value.bcc;
        memcpy( to, from, valueSize);
        *length += valueSize;
        break;
    case MMS_DATE :
    case X_MMS_MESSAGE_SIZE :
    case X_MMS_REPLY_CHARGING_SIZE :
        
        from = to;
        if ((to = (unsigned char *)cnvUint32ToLongInteger( value.date, 
            to, remainingSize)) == NULL)      
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "mmsPduAppend: Long-integer conversion failed.\n"));
            ret = FALSE;
        }
        else
        {
            *length += (unsigned long)(to - from);
        } 
        break;
    case X_MMS_VERSION :
        *to = SET_HIGH_BIT(value.version);
        ++*length;
        break;
    case X_MMS_DELIVERY_REPORT :
    case X_MMS_MESSAGE_TYPE :
    case X_MMS_PRIORITY :
    case X_MMS_READ_REPLY :
    case X_MMS_REPORT_ALLOWED :
    case X_MMS_SENDER_VISIBILITY :
    case X_MMS_STATUS :
    case X_MMS_RESPONSE_STATUS :
    case X_MMS_RETRIEVE_STATUS :
    case X_MMS_READ_STATUS :
    case X_MMS_REPLY_CHARGING :
        
        *to = value.deliveryReport; 
        ++*length;
        break;
    default :
        ret = FALSE;
        break;
    } 

    return ret;
} 














WE_BOOL mmsPduGet( void *header, unsigned long size, MmsHeaderTag tag, 
    MmsHeaderValue *value)
{
    unsigned char *offset = (unsigned char *)header;
    unsigned char *start = (unsigned char *)header;
    unsigned char searched = (unsigned char)(tag | 0x80);

    if (start == NULL)
    {
        return FALSE;
    } 

    

    if (tag == X_MMS_MESSAGE_TYPE)
    {
        if (SKIP_HIGH_BIT(*offset) == X_MMS_MESSAGE_TYPE && size > 1)
        {
            value->messageType = offset[1];

            return TRUE;
        } 

        return FALSE;
    } 
    
    while (offset != NULL && (unsigned long)(offset - start) < size)
    {
        
        if (searched != *offset)    
        {
            

            if (SKIP_HIGH_BIT(*offset) == MMS_CONTENT_TYPE)
            {
                return FALSE;
            } 

            offset = nextTag( offset, (unsigned long)(start - offset) + size);
        }
        else                        
        {
            return extractValue( offset, tag, value, 
                (unsigned long)(start - offset) + size);
        } 
    } 

    return FALSE;
} 









static unsigned char *nextTag(unsigned char *pos, unsigned long length)
{
    unsigned char *startPos = pos;
    WE_UINT32 aUint32;

    

    if (length < 2)
    {
        return NULL;
    } 

    if (*pos >= ALLOWED_STRING_CHAR_LIMIT)
    {
        
        ++pos;
    }
    else
    {
        
        pos = mmsPduSkipString( pos, (unsigned long)(startPos - pos) + length);
        
        pos = skipApplicationHeaderString( pos, 
            (unsigned long)(startPos - pos) + length);

        return pos;
    } 

    if (*pos < LENGTH_QUOTE)
    {
        
        if (*pos > length)
        {
            pos = NULL;
        }
        else
        {
            pos += *pos + 1;
        } 
    }
    else if (*pos == LENGTH_QUOTE)
    {
        ++pos;  
        pos = (unsigned char *)cnvUintvarToUint32( pos, &aUint32, 
            (unsigned long)(startPos - pos) + length);

        
        if (pos == NULL)
        {
            return NULL;
        } 

        pos += aUint32;
        if (pos > (startPos + length))
        {
            return NULL;
        } 

        
        if ( *(pos - 1) != WE_EOS )
        {
            return NULL;
        } 
    }
    else if (*pos < ALLOWED_STRING_CHAR_LIMIT)
    {
        pos = mmsPduSkipString( pos, (unsigned long)(startPos - pos) + length);
    }
    else
    {
        ++pos;
    } 

    return pos;
} 














WE_BOOL mmsPduSanityCheck( void *data, unsigned long size)
{
    unsigned char *pdu = (unsigned char *)data;
    unsigned char *start = (unsigned char *)data;
    WE_BOOL isHeader = TRUE;
#ifdef WE_LOG_MODULE
    unsigned char tag = 0;
#endif 

    
    if (pdu == NULL || size < MIN_PDU_LENGTH)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): NULL pointer (%d) or size error (%d) in PDU!\n", 
            __FILE__, __LINE__, pdu, size));
        return FALSE;
    } 

    
    if (SKIP_HIGH_BIT(pdu[0]) != X_MMS_MESSAGE_TYPE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): TYPE error (%d) in PDU!\n", 
            __FILE__, __LINE__, (int)pdu[0]));
        return FALSE;
    } 

    


    while (pdu != NULL && isHeader && (unsigned long)(pdu - start) < size - 1)
    {
#ifdef WE_LOG_MODULE
        tag = SKIP_HIGH_BIT(*pdu);
#endif 

        if (SKIP_HIGH_BIT(*pdu) == MMS_CONTENT_TYPE)
        {   
            ++pdu;
            pdu = skipContentType( pdu, (unsigned long)(start - pdu) + size);

            if (pdu != NULL)
            {
                isHeader = FALSE;
            } 
        }
        else
        {
            pdu = nextTag( pdu, (unsigned long)(start - pdu) + size);
        } 
    } 

    if (pdu == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): TAG %d error in PDU!\n", __FILE__, __LINE__, tag));
        return FALSE;
    } 

    return TRUE;
} 
















WE_BOOL mmsPduUnrecognized( void *data, unsigned long size)
{
    WE_UINT32 strSize;
    WE_UINT32 length;
    char *transId;
    unsigned char *pdu = (unsigned char *)data;
    WE_BOOL isUnrecognized = FALSE;
    MmsSigCohPostParam *post;
    MmsNotifyRespInd    notifyResp;
    MmsHeaderValue  ver;
    MmsHeaderValue  type;
    MmsHeaderValue  trId;

    type.messageType = MMS_M_SEND_REQ;
    trId.transactionId = TRANSACTION_ID_ERROR;
    ver.version = MMS_VERSION_PREVIOUS_MAJOR;
    if (pdu == NULL || size <= POS_TYPE_DATA ||
        SKIP_HIGH_BIT(pdu[POS_TYPE_TAG]) != X_MMS_MESSAGE_TYPE )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): PDU is invalid, size=%d, data=0x%x\n", 
            __FILE__, __LINE__, size, pdu == NULL ? 0 : pdu[POS_TYPE_TAG]));
        isUnrecognized = TRUE;
    }
    else if (pdu[POS_TYPE_DATA] < MMS_M_SEND_REQ || 
        pdu[POS_TYPE_DATA] > MMS_M_FORWARD_CONF)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): PDU Type is invalid 0x%x\n",
            __FILE__, __LINE__, pdu[POS_TYPE_DATA]));
        isUnrecognized = TRUE;
    }
    else if ( !mmsPduGet( data, size, X_MMS_VERSION, &ver) )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No X_MMS_VERSION header tag\n", 
            __FILE__, __LINE__));
        isUnrecognized = TRUE;
    } 

    if (isUnrecognized || 
        type.messageType < MMS_M_SEND_REQ || 
        type.messageType > MMS_M_FORWARD_CONF ||
        ver.version <= MMS_VERSION_PREVIOUS_MAJOR ||
        ver.version >= MMS_VERSION_NEXT_MAJOR)
    {
        if ( !mmsPduGet( data, size, X_MMS_TRANSACTION_ID, &trId))
        {
            trId.transactionId = TRANSACTION_ID_ERROR;
        } 

        strSize = strlen(trId.transactionId) + 1;
        if (strSize <= MMS_MAX_CHUNK_SIZE)
        {
            transId = M_CALLOC(strSize);
            strncpy( transId, trId.transactionId, strSize);
            transId[strSize - 1] = WE_EOS;

            
            notifyResp.transactionId = transId;
            notifyResp.status = MMS_STATUS_UNRECOGNIZED;
            notifyResp.allowed = MMS_DELIVERY_REPORT_NOT_SET;

            post = (MmsSigCohPostParam *)M_CALLOC(sizeof(MmsSigCohPostParam));
            if (post) 
            {
                
                post->isResponseRequested = FALSE;
                post->type = MMS_M_NOTIFY_RESP; 
                post->data.pdu.packet = createWspNotifyRespIndMsg( &notifyResp, 
                    &length, MMS_VERSION_10);   
                post->data.pdu.length = length;
                
                if (post->data.pdu.packet == NULL)
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                        "%s(%d): Couldn't create M-NotifyResp.ind!\n",
                        __FILE__, __LINE__));
                    M_FREE(post);
                }
                else
                {
                    M_SIGNAL_SENDTO_IUUP( M_FSM_COH_POST, MMS_SIG_COH_POST, 0, 0, 
                        0, post); 
                } 
                
                isUnrecognized = TRUE;
            }
            else
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                        "%s(%d): Allocation failed!\n",
                        __FILE__, __LINE__));
            }
        } 
    } 

    return isUnrecognized;
} 














static unsigned char *skipApplicationHeaderString(unsigned char *pos, 
    unsigned long length)
{
    
    if (length == 0 || pos == NULL)
    {
        return NULL;
    } 

    
    while (--length && VALID_APPLICATION_HEADER(*pos))
    {
        ++pos;
    } 

    
    if (length > 0 && *pos == WE_EOS)
    {
        ++pos;  
        return pos;
    } 

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "COH: skipApplicationHeaderString found 0x%02x at %d\n", 
        *pos, length));

    return NULL;
} 














static unsigned char *skipContentType(unsigned char *pos, unsigned long length)
{
    unsigned char *startPos = pos;
    WE_UINT32  valueLen;

    
    if (length == 0)
    {
        return NULL;
    } 

    
    if (*pos <= LENGTH_QUOTE)   
    {                           
        
        if ( (pos = (unsigned char *)cnvValueLengthToUint32( pos, 
            &valueLen, length)) == NULL)
        {
            return NULL;
        } 

        pos += valueLen;
    }                           
    else if (*pos & SHORT_FLAG) 
    {                           
        ++pos;
    }
    else                        
    {                   
        pos = mmsPduSkipString( pos, length);
    } 

    return pos > startPos + length ? NULL : pos;
} 











unsigned char *mmsPduSkipString(unsigned char *pos, unsigned long length)
{
    
    if (length == 0 || pos == NULL)
    {
        return NULL;
    } 

    
    while (--length && *++pos)
    {
        
    } 

    
    
    if (length > 0 && *pos == WE_EOS)
    {
        ++pos;  
        return pos;
    } 

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "%s(%d): skipString found no EOS within length.\n",
        __FILE__, __LINE__));
    
    return NULL;
}  
