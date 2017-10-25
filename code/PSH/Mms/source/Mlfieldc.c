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
#include "We_Mime.h"   

#include "Mms_Def.h"    
#include "Mms_Cfg.h"    
#include "Mmem.h"       
#include "Mcpdu.h"      
#include "Mcwap.h"      
#include "Mlfieldc.h"   
#include "Mlfieldp.h"   


#define ENCODED_VALUE_LEN           6   
#define ENCODED_CHAR_SET_LEN        8   


#define ENCODED_DATE_LEN            (ENCODED_VALUE_LEN + \
                                     ENCODED_OCTET_LEN + \
                                     ENCODED_D_INT_LEN)

#define MAX_VALUE_LENGTH_SIZE       5   
#define QUOTE_MARK                 34
#define QUOTE_REQUIRED           0x80
#define TOKEN_GREATER_THEN         62   
#define TOKEN_LESSER_THEN          60   
#define UTF8_SIZE                   1
















static WE_BOOL setContentTypeParams( WE_UINT8 modId, const MmsAllParams *param, 
    unsigned char *value);
static WE_BOOL setContentTypeValue( WE_UINT8 modId, const MmsAllParams *param, 
    WE_UINT32 *size, unsigned char *value, int mediaType, 
    const unsigned char *strValue);
static WE_BOOL createAddressField( WE_UINT8 modId, const MmsAddress *addr, 
    unsigned char **returnData, unsigned long *returnLength);








static WE_BOOL setContentTypeParams( WE_UINT8 modId, const MmsAllParams *param, 
    unsigned char *value)
{
    WE_BOOL errorHandl = TRUE;
    int len;

    while (param != NULL && errorHandl == TRUE)
    {
         
        switch (param->param)
        {
        case MMS_CHARSET :          
        case MMS_LEVEL :            
        case MMS_TYPE :             
        case MMS_TYPE_REL :    
        case MMS_START_REL :        
        case MMS_START_INFO_REL :
        case MMS_START_REL_14 :        
        case MMS_START_INFO_REL_14 :     
        case MMS_NAME :   
        case MMS_FILENAME :   
            *value++ = SET_HIGH_BIT(param->param);
            break;
        case MMS_TEXT_KEY:
        case MMS_UNHANDLED:
        default :
            errorHandl = FALSE;
            break;
        }  
        if (errorHandl == TRUE)
        {
            if (param->type == MMS_PARAM_STRING)   
            {    
                if (param->param == MMS_START_REL || param->param == MMS_START_REL_14)
                {
                    len = sprintf( (char *)value, "<%s>", 
                        (char *)param->value.string);
                    if ( len == 0)
                    {
                        
                        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                            "%s(%d): Content type w. wrong length\n", 
                            __FILE__, __LINE__));
                        return FALSE;
                    } 
                
                    value += len + 1; 
                }
                else
                {   
                    strcpy((char *)value, (char *)param->value.string);
                    value += strlen((char *)param->value.string) + 1;
                } 
            }
            else if (param->value.integer <= SHORT_INTEGER_MAX_VALUE)        
            {
                 
                *value++ = SET_HIGH_BIT(param->value.integer);
            }
            else                                       
            {
                 
                


                value = (unsigned char *)cnvUint32ToLongInteger(
                    param->value.integer, value, 5);
            }  

            param = (MmsAllParams *)param->next;
        } 
    }  
    
    return errorHandl;
}  












static WE_BOOL setContentTypeValue( WE_UINT8 modId, const MmsAllParams *param, WE_UINT32 *size, 
    unsigned char *value, int mediaType, const unsigned char *strValue)
{
    unsigned char *cpyValue; 
    
    cpyValue = value;

    
    if (*size > 1 && param != NULL)
    {
        if (*size > 30)
        {
            *cpyValue++ = 31;
            cpyValue = (unsigned char *)cnvUint32ToUintvar(*size,cpyValue,5);    
        }
        else
        {
            *cpyValue = (unsigned char)(*size & 0x000000FF);
            cpyValue += strlen( ( const char *)value);
        }

        *size += strlen((const char *)value);
    }  

     
    if (mediaType == MMS_VALUE_AS_STRING)
    {    
        strcpy((char *)cpyValue, (const char *)strValue);
        cpyValue += strlen((const char *)strValue) + 1;
    }
    else
    {    
        *cpyValue++ = SET_HIGH_BIT(mediaType);
    }

     
    if (param != NULL )
    {
        if (!setContentTypeParams( modId, param, cpyValue))
        {
            return FALSE;
        }
    }  
    return TRUE;
}  




static WE_INT32 mms_mime_to_int_strict(const char *mime)
{
    WE_INT32 knownValue = -1;
    knownValue = we_mime_to_int(mime);

    if (knownValue < WE_MIME_TYPE_AUDIO_ANY)
    {
        return knownValue;
    }

    



    return -1;
}












unsigned char *createContentType(WE_UINT8 modId, WE_UINT32 *size,      
    const MmsContentType *contentInfo)
{
    unsigned char *value = NULL;
    MmsKnownMediaType knownValue; 

    knownValue = contentInfo->knownValue;
    


    if (knownValue == MMS_VALUE_AS_STRING)
    {
       knownValue = mmsConvertToMmsKnownMediaType((WE_UINT32) mms_mime_to_int_strict((const char*)contentInfo->strValue));
       if (knownValue == MMS_TYPE_ANY_ANY)
       {
           knownValue = MMS_VALUE_AS_STRING;
       }
    }

     
    calculateSize( contentInfo, size);

    


    if ((value = WE_MEM_ALLOC( modId, (unsigned int)(*size + 5))) == NULL)
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "Unable to allocate memory\n"));
        return NULL;
    }
    memset(value, 0, (unsigned int)(*size + 5));

     
    if (knownValue != MMS_VALUE_AS_STRING)
    {
        if (!setContentTypeValue( modId, contentInfo->params, size, value, 
            knownValue, NULL))
        {
            WE_MEM_FREE( modId, value);
            value = NULL;
        } 
    }
    else if (contentInfo->strValue != NULL)
    {
        if (!setContentTypeValue( modId, contentInfo->params, size, value, 
            knownValue, contentInfo->strValue))
        {
            WE_MEM_FREE( modId, value);
            value = NULL;
        } 
    }
    else
    {
        WE_MEM_FREE( modId, value);
        value = NULL;
        *size = 0;
    }  
    
    return value;
}  










unsigned char *createMsgClass( WE_UINT8 modId, unsigned long *size,
    MmsMessageClass msgClass)
{
    unsigned char *value;
    
     



    if (msgClass.classIdentifier == MMS_MESSAGE_CLASS_IS_TEXT)
    {
        *size = strlen(msgClass.textString) + 1;
        
        if (msgClass.textString == NULL)
        {   
            
            value = NULL;
        }
        else if ( (value = WE_MEM_ALLOC( modId, (unsigned)*size)) == NULL)
        {
            
            value = NULL;
        }
        else
        {
            strcpy( (char *)value, msgClass.textString);
        } 
    }
    else
    {
        *size = sizeof(WE_UINT8);
        value = WE_MEM_ALLOC( modId, (unsigned)*size); 
        if ( value == NULL)
        {
            
            return NULL;
        } 
        *value = SET_HIGH_BIT(msgClass.classIdentifier);
    } 
    return value; 
}  















WE_BOOL createFromField( WE_UINT8 modId, const MmsAddress *from, WE_UINT32 fromType,
    unsigned char **buf, unsigned long *bufLength)
{
    unsigned long maxDataSize = 0;
    unsigned char *pTmp;
    unsigned char *ptr;
    unsigned long encTextSize;
    unsigned char *encTextBuf;
   
    WE_BOOL ret = FALSE;
    
    if ( fromType == MMS_FROM_INSERT_ADDRESS)
    {
        maxDataSize = MAX_VALUE_LENGTH_SIZE + TOKEN_SIZE;
        ptr = WE_MEM_ALLOC( modId, (unsigned)maxDataSize);
        if (ptr == NULL)
        {
            
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                "Unable to allocate memory in: createFromField()\n"));
            return FALSE;
        }
        memset( ptr, 0, maxDataSize);

        *buf = ptr;
        pTmp = ptr;
        ptr = cnvUint32ToValueLength( (WE_UINT32)sizeof(unsigned char), 
            (void *)ptr, maxDataSize);
        *ptr = MMS_FROM_INSERT_ADDRESS;
        *bufLength = (unsigned long)(ptr - pTmp);
        *bufLength += TOKEN_SIZE;
        ret = TRUE;
    }
    else if ( fromType == MMS_FROM_ADDRESS_PRESENT && from != NULL) 
    {
         
        
        
        if ( createAddressField( modId, from, &encTextBuf, &encTextSize))
        {
             
            maxDataSize = MAX_VALUE_LENGTH_SIZE + TOKEN_SIZE + encTextSize;
            
            ptr = WE_MEM_ALLOC( modId, (unsigned) maxDataSize);
            if (ptr == NULL)
            {
                
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: createFromField()\n"));
                return FALSE;
            }
            memset( ptr, 0, maxDataSize);
            
            *buf = ptr; 
            ptr = (unsigned char *)cnvUint32ToValueLength((WE_UINT32)TOKEN_SIZE 
                + encTextSize, (void *)ptr, maxDataSize);
            *bufLength = (unsigned long)(ptr - *buf);
            *ptr++ = MMS_FROM_ADDRESS_PRESENT;
            memcpy(ptr, encTextBuf, (unsigned)encTextSize);
            *bufLength += 1 + encTextSize;
            ret = TRUE;

            WE_MEM_FREE( modId,  encTextBuf);
            encTextBuf = NULL;
        }  
    }
    else
    {
        
    }  
    return ret;
}  














WE_BOOL createDestinationField( WE_UINT8 modId, MmsAddressList *addrList, 
    unsigned char *pduHeader, unsigned long bufSize, 
    unsigned long *actLength, MmsHeaderValue *valueOfTag, MmsHeaderTag headerTag)
{
    unsigned char *buffer;
    unsigned long addressLength;
 
    MmsAddressList *listPtr;
    MmsAddress *addr;

    listPtr = addrList;

    while ( listPtr)
    {
        addr = &listPtr->current;

        if ( createAddressField( modId, addr, &buffer, &addressLength) == FALSE)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "%s(%d): Failed to create address field\n", 
                __FILE__, __LINE__));
            return FALSE;    
        }
        else
        {
            valueOfTag->to = buffer;

            if ( !mmsPduAppend( pduHeader,  
                      bufSize,              
                      actLength,            
                      headerTag,            
                      addressLength,        
                      *valueOfTag))         
            { 
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "%s(%d): Couldn't add address to header\n", 
                    __FILE__, __LINE__));

                M_FREE(buffer); 
                buffer = NULL;
                return FALSE;    
            }  
            
            WE_MEM_FREE( modId, buffer); 
            buffer = NULL;
        } 
        
        listPtr = listPtr->next;
    }   
    
    return TRUE;
} 











static WE_BOOL createAddressField( WE_UINT8 modId, const MmsAddress *addr, 
    unsigned char **returnData, unsigned long *returnLength)
{

    WE_BOOL ret;
    MmsCharset charset;
    unsigned int addressLength;
    unsigned int storedBytes;
    unsigned char *ptr;
    unsigned char *tmpPtr;

    if ( addr->address == NULL) 
    {
        return FALSE; 
    } 
   
    if ( addr->name.text == NULL)
    {

        charset = MMS_US_ASCII;
    }
    else
    {

        charset = addr->name.charset;
    } 

    if ( (ptr = WE_MEM_ALLOC( modId, 
        (unsigned)(MAX_ADDRESS_LEN))) == NULL)
    {
        
        return FALSE;
    } 
    
    
    memset( ptr, 0, (unsigned)(MAX_ADDRESS_LEN));

    tmpPtr = ptr;
    
    addressLength = 0;

    


    if ( addr->addrType == MMS_EMAIL) 
    {
        
        if (addr->name.text != NULL)
        {
            


            addressLength = (strlen(addr->name.text) + 3);
        }
        addressLength += strlen(addr->address);               
    }
    else if ( addr->addrType == MMS_IPV4) 
    {
        addressLength = (strlen(ADDRESS_TYPE_IPV4) + strlen(addr->address));       
    }
    else if ( addr->addrType == MMS_PLMN) 
    {
        addressLength = (strlen(ADDRESS_TYPE_PLMN) + strlen(addr->address));       
    }

    
    if (addressLength > MAX_ADDRESS_LEN)
    {
        WE_MEM_FREE( modId, tmpPtr);
        tmpPtr = NULL;
        return FALSE;
    }
    
    storedBytes = 0;
    if ( addr->addrType == MMS_EMAIL) 
    {
        if (addr->name.text != NULL)
        {
            storedBytes = 
            (unsigned int)sprintf( (char *)ptr, "%s <%s>", addr->name.text, addr->address);
        }
        else
        {
            strcpy( (char *)ptr, addr->address);
            storedBytes = addressLength;
        } 
    }
    else if ( addr->addrType == MMS_IPV4) 
    {
        storedBytes = 
        (unsigned int)sprintf( (char *)ptr, "%s%s", addr->address, ADDRESS_TYPE_IPV4);
    }
    else if ( addr->addrType == MMS_PLMN) 
    {
        storedBytes = 
        (unsigned int)sprintf( (char *)ptr, "%s%s", addr->address, ADDRESS_TYPE_PLMN);
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, modId, 
            "%s(%d): Unknown addresstype.\n", 
            __FILE__, __LINE__));
        WE_MEM_FREE( modId, tmpPtr);
        return FALSE;

    }

    if (storedBytes > MAX_ADDRESS_LEN)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, modId, 
            "%s(%d): Fatal Error, sprintf memory overwrite.\n", 
            __FILE__, __LINE__));
        WE_MEM_FREE( modId, tmpPtr);
        return FALSE;
    }

    ret = createEncodedStringValue( modId, (char *)tmpPtr, charset, returnLength, 
        returnData);
    WE_MEM_FREE( modId, tmpPtr);
    tmpPtr = NULL;
    
    return ret;
} 



















unsigned char *createTimeField( WE_UINT8 modId, unsigned long *size, 
    WE_UINT32 theTime, MmsTimeType timeType)
{ 
    unsigned char *value;
    unsigned char *to;
    unsigned char *pos;
    unsigned char cnvTime[ENCODED_DATE_LEN]; 
    unsigned long consumedBytes = 0;

    *size = 0L;
    to = cnvTime;

    if (MMS_TIME_RELATIVE == timeType) 
    {
        
        
        *to++ = SET_HIGH_BIT(MMS_EXPIRY_TIME_RELATIVE); 
    }
    else
    {
        
        *to++ = SET_HIGH_BIT(MMS_EXPIRY_TIME_ABSOLUTE); 
    }

     
    if ((to = cnvUint32ToLongInteger( theTime, to, sizeof(cnvTime) - 1)) == NULL)
    {                     
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): time conversion failed.\n", 
            __FILE__, __LINE__));
        return NULL;
    } 

     
    consumedBytes = (unsigned long)(to - cnvTime);

    if (consumedBytes == 0 || consumedBytes > sizeof(cnvTime))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Date larger than reserved space.\n", 
            __FILE__, __LINE__));
        return NULL;
    } 

    value = WE_MEM_ALLOC( modId, ENCODED_DATE_LEN);
    if ( value == NULL)
    {
        
        return NULL;
    } 
    
         
    if ((pos = cnvUint32ToValueLength((WE_UINT32)consumedBytes, value, 
        ENCODED_DATE_LEN)) == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Value length conversion for time failed.\n", 
            __FILE__, __LINE__));
        WE_MEM_FREE( modId, value);
        value = NULL;
        return NULL;
    } 

    *size = consumedBytes + (unsigned long)(pos - value);

    
    if ((WE_UINT32)pos - (WE_UINT32)value - consumedBytes > 0) 
    {
        
        memcpy( pos, cnvTime, consumedBytes);   
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Final conversion for time failed.\n", 
            __FILE__, __LINE__));
        WE_MEM_FREE ( modId, value); 
        value = NULL;
    }  

    return value; 
} 














WE_BOOL createEncodedStringValue( WE_UINT8 modId, const char *text, 
    MmsCharset charset, unsigned long *size, unsigned char **retPtr)
{
    WE_BOOL ret;
    WE_UINT32  maxDataSize;
    unsigned long sizeWritten = 0;
    WE_UINT32 dataSize;
    unsigned char *ptr;
    unsigned char *pTmp;
    WE_BOOL insertQuoteChar = FALSE;

    
    
    maxDataSize = strlen(text) + 1 + MAX_VALUE_LENGTH_SIZE + UTF8_SIZE; 

    
    if (text[0] & QUOTE_REQUIRED)
    {
        insertQuoteChar = TRUE;
        maxDataSize++;
    }

    ptr = WE_MEM_ALLOC( modId, maxDataSize);
    if ( ptr == NULL)
    {
        
        return FALSE;
    } 
    memset ( ptr, 0x00, maxDataSize);
    *retPtr = ptr;
    *size = 0;

    switch (charset)
    {
    case MMS_US_ASCII:
        
        if (insertQuoteChar)
        {
            *ptr++ = TEXT_QUOTE;
            sizeWritten = 1;
        } 

        strcpy( (char *)ptr, text);
        sizeWritten += strlen(text) + 1; 
        ret = TRUE;
        break;
    case MMS_UTF8:
        dataSize = strlen(text) + 1 + 1; 

        
        if (insertQuoteChar)
        {
            dataSize++;
        } 

        pTmp = ptr;
        ptr = cnvUint32ToValueLength( dataSize, (void *)ptr, maxDataSize);
        
        
        sizeWritten = (unsigned long)(ptr - pTmp); 

        
        *ptr++ = (MMS_UTF8 | 0x80);

        
        if (insertQuoteChar)
        {
            *ptr++ = TEXT_QUOTE;
        } 

        strcpy( (char *)ptr, text);

        sizeWritten += dataSize; 
                                 
        ret = TRUE;
        break;
    
    case MMS_UTF16BE:
    case MMS_UTF16LE:
    case MMS_UTF16:
    case MMS_UNKNOWN_CHARSET:
    case MMS_ISO_8859_1:
    case MMS_UCS2:
    default :
        
        if ( ptr != NULL)
        {               
            WE_MEM_FREE( modId, ptr);
            ptr = NULL;
            *retPtr = NULL;
        } 
                    
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): invalid charset\n", __FILE__, __LINE__));

        ret = FALSE;
        break;
    } 
        
    *size = sizeWritten;
    return ret;
} 







void calculateSize( const MmsContentType *contentInfo, 
    WE_UINT32 *size)
{
    
    MmsAllParams *allParams;
    MmsContentType *cpyContent; 
    unsigned char *tempPointer;
    unsigned char integerBuf[UINTVAR_MAX_LENGTH];
    MmsKnownMediaType knownValue;

    knownValue = contentInfo->knownValue;
    


    if (knownValue == MMS_VALUE_AS_STRING)
    {
       knownValue = mmsConvertToMmsKnownMediaType((WE_UINT32) mms_mime_to_int_strict((const char*)contentInfo->strValue));
       if (knownValue == MMS_TYPE_ANY_ANY)
       {
           knownValue = MMS_VALUE_AS_STRING;
       }
    }

    memset( integerBuf, 0x00, sizeof(integerBuf));
    *size = 0; 
    cpyContent = ( MmsContentType *)contentInfo;

    if (knownValue == MMS_VALUE_AS_STRING)
    {
        if (cpyContent->strValue != NULL)
        {
            
            *size += strlen((const char *)cpyContent->strValue) + 1;
        } 
    }
    else
    {
        
        *size += 1;
    } 

    allParams = cpyContent->params;

    while (allParams != NULL) 
    {
        if (allParams->key != NULL)
        {
            if (allParams->param == MMS_TEXT_KEY)
            {
                *size += strlen((const char *)allParams->key);
            }
        } 
        
        if ( allParams->type == MMS_PARAM_STRING)
        {
            if (NULL != allParams->value.string)
            {
                *size += strlen((const char *)allParams->value.string) + 1;
            }
            *size += 1; 
            if (allParams->param == MMS_START_REL || allParams->param == MMS_START_REL_14)
            {
                
                *size += 2;
            } 
        }
        else
        {
            
            if (allParams->value.integer <= 127)
            {
                
                *size += 2;
            }
            else
            {
                tempPointer = (unsigned char *)cnvUint32ToLongInteger(
                        allParams->value.integer, integerBuf, UINTVAR_MAX_LENGTH);
                
                
                *size += (unsigned long)(tempPointer - 
                    (unsigned char *)integerBuf + 1);
            } 
        } 

        allParams = allParams->next;
    } 
} 







WE_UINT32 calculateEntryHeadersSize( MmsEntryHeader *entryHeader)
{
    MmsEntryHeader *entry = entryHeader;
    WE_UINT32 size = 0;

    while ( entry != NULL)
    {
        switch ( entry->headerType)
        {
        case MMS_SHORT_CUT_SHIFT_DELIMITER:
            
            size += 1;
            break;
        case MMS_SHIFT_DELIMITER:
            
            size += 2; 
            break;
        case MMS_WELL_KNOWN_CONTENT_ID:
            
            
            
            size += 1 + strlen( 
                (const char *)entry->value.wellKnownFieldName)
                + 3  
                + 1; 
            break;
        case MMS_WELL_KNOWN_CONTENT_LOCATION:
            
            
            
            size += 1 + strlen((const char *)
                entry->value.wellKnownFieldName) + 1;
            break;
        case MMS_APPLICATION_HEADER:
            
            
            
            size += 1 + strlen( (const char *)
                entry->value.applicationHeader.value) + 1;
            break;
        case MMS_WELL_KNOWN_CONTENT_BASE:
        case MMS_WELL_KNOWN_CONTENT_ENCODING:
        case MMS_WELL_KNOWN_CONTENT_LANGUAGE:
        case MMS_WELL_KNOWN_CONTENT_LENGTH:
        case MMS_WELL_KNOWN_CONTENT_MD5:
        case MMS_WELL_KNOWN_CONTENT_RANGE:
        case MMS_WELL_KNOWN_CONTENT_TYPE:
        case MMS_WELL_KNOWN_CONTENT_DISPOSITION:
        default :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Received unknown entry header type\n", 
                __FILE__, __LINE__));
            break;
        } 

        entry = entry->next;
    } 

    return size;
} 












unsigned char *createEntryHeaders( MmsEntryHeader *entryHeader, 
    unsigned char *buffer)
{
    MmsEntryHeader *entry = entryHeader;

    while ( entry != NULL)
    {
        switch ( entry->headerType)
        {
        case MMS_SHORT_CUT_SHIFT_DELIMITER:
            
            *buffer++ = entry->value.shortCutShiftDelimiter;
            break;
        case MMS_SHIFT_DELIMITER:
            
            *buffer++ = ( unsigned char)entry->headerType;
            *buffer++ = entry->value.shiftDelimiter;
            break;
        case MMS_WELL_KNOWN_CONTENT_ID:
            
            *buffer++ = SET_HIGH_BIT(( unsigned char)entry->headerType);
            *buffer++ = QUOTE_MARK;
            *buffer++ = TOKEN_LESSER_THEN;
            strcpy( (char *)buffer, ( const char *)
                entry->value.wellKnownFieldName);
            buffer += strlen( (const char *)entry->value.wellKnownFieldName);
            *buffer++ = TOKEN_GREATER_THEN;
            *buffer++ = 0;
            break;

        case MMS_WELL_KNOWN_CONTENT_LOCATION:
            
            *buffer++ = SET_HIGH_BIT(( unsigned char)entry->headerType);
            strcpy( ( char *)buffer, (const char *)
                entry->value.wellKnownFieldName);
            buffer += strlen( (const char *)entry->value.wellKnownFieldName);
            *buffer++ = 0;
            break;

        case MMS_APPLICATION_HEADER:
            
    
             
            strcpy( (char *)buffer, (const char *)
                entry->value.applicationHeader.name);
            buffer += 
                strlen((const char *)entry->value.applicationHeader.name) + 1;
            strcpy( (char *)buffer, (const char *)
                entry->value.applicationHeader.value );
            buffer += strlen( (const char *)entry->value.applicationHeader.value) + 1;
            break;
        case MMS_WELL_KNOWN_CONTENT_BASE:
        case MMS_WELL_KNOWN_CONTENT_ENCODING:
        case MMS_WELL_KNOWN_CONTENT_LANGUAGE:
        case MMS_WELL_KNOWN_CONTENT_LENGTH:
        case MMS_WELL_KNOWN_CONTENT_MD5:
        case MMS_WELL_KNOWN_CONTENT_RANGE:
        case MMS_WELL_KNOWN_CONTENT_TYPE:
        case MMS_WELL_KNOWN_CONTENT_DISPOSITION:
        default :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Received unknown entry header type\n", 
                __FILE__, __LINE__));
            break;
        } 

        entry = entry->next;
    } 

    return buffer;
} 








unsigned long getSizeOfAddress( MmsAddressList *addrList)
{
    unsigned long length = 0;
    MmsAddressList *tmpPtr;
    tmpPtr = addrList;
   
    while (tmpPtr != NULL) 
    {
        if ( tmpPtr->current.address != NULL)
        {
            length += strlen(tmpPtr->current.address);
        } 
        length += getSizeOfEncodedText(tmpPtr->current.name);

        ++length;   

        tmpPtr = tmpPtr->next;
    } 

    return length;
} 







unsigned long getSizeOfEncodedText(MmsEncodedText eText)
{ 
   unsigned long len = 0;             
   
   if (eText.text != NULL)
   {
        len = strlen(eText.text);
        len += ENCODED_VALUE_LEN;    
        len += ENCODED_CHAR_SET_LEN; 
   } 

   return len; 
} 









char *generateMmsTransactionId( WE_UINT8 modId, unsigned long *length)
{    
    char *string = NULL; 

    string  = WE_MEM_ALLOC( modId, MAX_LEN_TRANSACTION_ID);
    if ( string == NULL)
    {
        
        return NULL;
    } 
    *length = (unsigned long)(1 + sprintf( string, "%ld", 
        mmsWapGetTransactionId())); 

    return string; 
} 

