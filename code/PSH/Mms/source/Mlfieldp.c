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
#include "We_Chrt.h"   
#include "We_Mime.h"   

#include "Mms_Def.h"    
#include "Mms_Cfg.h"    
#include "Mcpdu.h"      
#include "Mlfieldp.h"   
#include "Mutils.h"     


#define TOKEN_GREATER_THEN  62          
#define TOKEN_LESSER_THEN   60          

#define ANY_CHARSET_VALUE   128
#define ANY_LANGUAGE_VALUE  128
#define SHIFT_DELIMITER     127
#define SHORT_CUT_SHIFT_DELIMITER_MAX_VALUE 31


typedef struct {
    MmsParam    knownValue;
    const char *stringValue;
} ParamWellKnowValueType;










ParamWellKnowValueType ParamWellKnowValueTypeTable[] = {
    { MMS_CHARSET,  "charset" },                
    { MMS_LEVEL,    "level" },                  
    { MMS_TYPE_REL, "type" },                   
    { MMS_NAME,     "name" },                   
    { MMS_FILENAME, "filename" },               
    { MMS_START_REL,"start" },                  
    { MMS_START_INFO_REL, "start-info" },       
    { MMS_START_REL_14,"start" },                  
    { MMS_START_INFO_REL_14, "start-info" },       

    { 0, NULL} };                               






static unsigned char *parseContentTypeParams( WE_UINT8 modId, 
     unsigned char *buf, WE_UINT32 size, MmsAllParams **params, 
     WE_UINT32 paramLen);
static unsigned char *parseParam( WE_UINT8 modId, unsigned char *buf, 
    WE_UINT32 size, MmsAllParams *params);
static WE_BOOL splitAddressString(WE_UINT8 modId, unsigned char *str, 
    char **addr, char **text, MmsAddressType* adrType);
static MmsEncodedText *charsetCvtEncodedString(WE_UINT8 modId, 
    const MmsEncodedText *fromText, const WE_UINT32 fromBufferSize);
static WE_BOOL skipEntryHeader( WE_UINT8 modId, unsigned char **entryHeader, 
    WE_UINT32 size);
static WE_BOOL isValidIp4Address( char *addr, int len);











unsigned char *parseContentType( WE_UINT8 modId, MmsContentType **contType,
    const unsigned char *contentTypeData, WE_UINT32 length)
{
    unsigned char *cPtr;
    unsigned char value = 0;
    MmsContentType *cType;
    WE_UINT32 cLen; 
    WE_UINT32 paramLen;
    WE_UINT32 tempLen = 0;
    const char *mime;
    
    if (contentTypeData == NULL)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS,"%s(%d): illegal indata\n", __FILE__, __LINE__));
        
        return NULL;
    } 

    
    *contType = WE_MEM_ALLOC( modId, sizeof(MmsContentType));
    
    if ( *contType == NULL)
    {
        
        return NULL;
    } 
    memset( *contType, 0, sizeof( MmsContentType));
    cType = *contType;
 
    cPtr = (unsigned char *)contentTypeData;
    paramLen = 0; 

    if (*cPtr >= SHORT_FLAG)
    {
        
        value = SKIP_HIGH_BIT( *cPtr);
        cPtr++;
        cType->knownValue = mmsConvertToMmsKnownMediaType( (WE_UINT32)value);
        cType->strValue = NULL;
        cType->params = NULL;
    }
    else if (*cPtr > LENGTH_QUOTE && *cPtr < SHORT_FLAG)
    {
        
        cType->knownValue = MMS_VALUE_AS_STRING;
        tempLen =  strlen( (const char *)cPtr) + 1;
        cType->strValue = WE_MEM_ALLOC( modId, tempLen);
        if (cType->strValue == NULL)
        {
            WE_MEM_FREE( modId, *contType);
            *contType = NULL;
            return NULL;
        } 
        memset (cType->strValue, 0, tempLen) ;
        strcpy( (char *)cType->strValue, (char *)cPtr);
        cType->params = NULL;
        cPtr += strlen( (const char *)cPtr) + 1;
    }
    else if (*cPtr <= LENGTH_QUOTE)
    {
        
        cPtr = (unsigned char *)cnvValueLengthToUint32( (void *)cPtr, &cLen, length);
        if (cPtr == NULL)  
        {
            
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "parseContentType: Unable to convert size"));
            freeMmsContentType( modId, *contType);
            WE_MEM_FREE( modId, *contType); 
            *contType = NULL;
            
            return NULL; 
        }
        else if ( *cPtr >= SHORT_FLAG)
        {
            
            value = SKIP_HIGH_BIT( *cPtr);
            cPtr++;
            cType->knownValue = mmsConvertToMmsKnownMediaType( (WE_UINT32)value);
            cType->strValue = NULL;
            
            paramLen = cLen - 1;  
        }
        else
        {
            unsigned int len = strlen( (const char *)cPtr) + 1;
            
            cType->knownValue = MMS_VALUE_AS_STRING;
        
            cType->strValue = WE_MEM_ALLOC( modId, len);
            if (cType->strValue == NULL)
            {
                WE_MEM_FREE( modId, *contType); 
                *contType = NULL;
                return NULL;
            } 

            memcpy( cType->strValue, cPtr, len);

            cPtr += len; 
            paramLen = cLen - len;
        } 

        if ( contType != NULL && paramLen > 0)
        {
            
            if ( parseContentTypeParams( modId, (unsigned char *)cPtr, length,
                 &cType->params, paramLen) == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "parseContentType: Unable to parse ContentType params"));
                freeMmsContentType( modId, *contType);
                WE_MEM_FREE( modId, *contType); 
                *contType = NULL; 
                cPtr = NULL;
            } 
            else
            {   
                cPtr += paramLen;
            } 
        } 
    } 

    if (NULL == cType->strValue)
    {
        mime = we_int_to_mime (cType->knownValue);
        cType->strValue = (unsigned char *)we_cmmn_strdup(modId, mime);
    }
    return cPtr;
} 











unsigned char *parseEntryHeader( WE_UINT8 modId, unsigned char *headerStart, 
    WE_UINT32 bufSize, MmsBodyInfoList *bodyInfoList)
{
    unsigned char *headerEnd = NULL;
    WE_UINT32  valueSize = 0;
    WE_UINT32  processedLength = 0;
    MmsEntryHeader *entryStart = NULL;
    MmsEntryHeader *prevEntryHeader;

    WE_BOOL firstEntry = TRUE;

    if (headerStart == NULL || bodyInfoList == NULL || bufSize == 0)
    {
        return NULL;
    } 
    headerEnd = headerStart;

    
    
    
    while (processedLength < bufSize)
    {
        if ( firstEntry)
        {
            
            bodyInfoList->entryHeader = 
                WE_MEM_ALLOC( modId, sizeof( MmsEntryHeader));
            if (bodyInfoList->entryHeader == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: parseEntryHeader()\n"));
                return NULL;
            } 
            prevEntryHeader = bodyInfoList->entryHeader;
            entryStart = bodyInfoList->entryHeader;
            firstEntry = FALSE;
        } 
        else
        {
            
            bodyInfoList->entryHeader->next = WE_MEM_ALLOC( modId,  
                            sizeof(MmsEntryHeader));
            if (bodyInfoList->entryHeader->next == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, modId, 
                    "Unable to allocate memory in: parseEntryHeader()\n"));
                freeMmsEntryHeader( modId, entryStart);
                WE_MEM_FREE( modId, entryStart);
                bodyInfoList->entryHeader = NULL;
                return NULL;
            } 
            prevEntryHeader = bodyInfoList->entryHeader;
            bodyInfoList->entryHeader = bodyInfoList->entryHeader->next;
        }
                    
        if ( bodyInfoList->entryHeader  == NULL)
        {
            
            return NULL;
        } 
        memset( bodyInfoList->entryHeader, 0x00, sizeof( MmsEntryHeader));
        bodyInfoList->entryHeader->next = NULL;
        
        
        if ( *headerEnd <= SHORT_CUT_SHIFT_DELIMITER_MAX_VALUE)
        {
            
            bodyInfoList->entryHeader->headerType = 
                MMS_SHORT_CUT_SHIFT_DELIMITER;
            bodyInfoList->entryHeader->value.shortCutShiftDelimiter =
                *headerEnd;
            ++headerEnd;
        }
        else if ( *headerEnd < SHIFT_DELIMITER)   
        {
            
            bodyInfoList->entryHeader->headerType = MMS_APPLICATION_HEADER;
            
            
            valueSize = mmsStrnlen( (char *)headerEnd, (bufSize - 1));
            
            if (valueSize < (bufSize - 1) && 
                valueSize <= MAX_MMS_ENTRY_HEADER_VALUE_LEN)
            {
                bodyInfoList->entryHeader->value.applicationHeader.name = 
                    WE_MEM_ALLOC( modId, valueSize + 1);
                
                if ( bodyInfoList->entryHeader->value.applicationHeader.name == NULL)
                {
                    
                    return NULL;
                } 
                memset( bodyInfoList->entryHeader->value.applicationHeader.name, 0, valueSize + 1);
                
                
                strncpy((char *)bodyInfoList->entryHeader->value.applicationHeader.name, 
                    (char *)headerEnd, valueSize);
                
                headerEnd += valueSize + 1;
              
                
            }
            else
            {
                
               bodyInfoList->entryHeader->value.applicationHeader.value = NULL;
                return NULL;
            } 
            valueSize = mmsStrnlen( (char *)headerEnd, (bufSize - 1));
        
            
            if ( (valueSize <= (bufSize - 1)) &&
                (valueSize <= MAX_MMS_ENTRY_HEADER_VALUE_LEN))
            {
                bodyInfoList->entryHeader->value.applicationHeader.value = 
                    WE_MEM_ALLOC( modId, valueSize + 1);
                if ( bodyInfoList->entryHeader->value.applicationHeader.value
                    == NULL)
                {
                    
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                        "parseEntryHeader: Unable to allocate memory"));
                    freeMmsEntryHeader( modId, entryStart);
                    WE_MEM_FREE( modId, entryStart);
                    bodyInfoList->entryHeader = NULL;
                    return NULL;
                } 
 
                memset(bodyInfoList->entryHeader->value.applicationHeader.value, 
                    0, valueSize + 1);
                strcpy((char *)bodyInfoList->entryHeader->value.applicationHeader.value, 
                    (char *)headerEnd);
                
                headerEnd += valueSize + 1;
            }
            else
            {
                
                bodyInfoList->entryHeader->value.applicationHeader.value = NULL;
                return NULL;
            } 
        }
        else if (*headerEnd == SHIFT_DELIMITER)   
        {
            
            bodyInfoList->entryHeader->headerType = MMS_SHIFT_DELIMITER;
            ++headerEnd;
            bodyInfoList->entryHeader->value.shiftDelimiter = *headerEnd;
            ++headerEnd;
        }
        else                            
        {
            
            bodyInfoList->entryHeader->headerType = 
                (MmsEntryHeaderValueType) SKIP_HIGH_BIT( *headerEnd);

            if (SKIP_HIGH_BIT(*headerEnd) == MMS_WELL_KNOWN_CONTENT_ENCODING)
            {
                ++headerEnd;                 

                switch (*headerEnd)
                {
                case 128: 
                case 129: 
                case 130: 
                    bodyInfoList->entryHeader->value.shortCutShiftDelimiter = *headerEnd;
                    headerEnd++;
                    break;
                default: 
                    bodyInfoList->entryHeader->value.shortCutShiftDelimiter = *headerEnd;
                    headerEnd++;
                    headerEnd++;
                    break;
                }
            }       
            else if ((SKIP_HIGH_BIT(*headerEnd) == MMS_WELL_KNOWN_CONTENT_ID) ||
                (SKIP_HIGH_BIT(*headerEnd) == MMS_WELL_KNOWN_CONTENT_LOCATION))
            {
                ++headerEnd; 

                valueSize = mmsStrnlen((char *)headerEnd, 
                    (bufSize - (WE_UINT32)(headerEnd - headerStart)));

                if ( valueSize > MAX_MMS_ENTRY_HEADER_VALUE_LEN )
                {              
                    
                    if (entryStart == bodyInfoList->entryHeader)
                    {
                        
                        freeMmsEntryHeader( modId, bodyInfoList->entryHeader);
                        WE_MEM_FREE( modId, bodyInfoList->entryHeader);
                        bodyInfoList->entryHeader = NULL;
                        entryStart = NULL;
                        prevEntryHeader = NULL;
                        firstEntry = TRUE;
                    }
                    else
                    {
                        freeMmsEntryHeader( modId, bodyInfoList->entryHeader);
                        WE_MEM_FREE( modId, bodyInfoList->entryHeader);
                        bodyInfoList->entryHeader = prevEntryHeader;
                        bodyInfoList->entryHeader->next = NULL;
                    } 
                    
                    headerEnd = headerStart + bufSize;
                } 
                else
                {
                    bodyInfoList->entryHeader->value.wellKnownFieldName = 
                        WE_MEM_ALLOC( modId, valueSize + 1);
                    if ( bodyInfoList->entryHeader->value.wellKnownFieldName == NULL)
                    {
                        
                        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                            "parseEntryHeader: Unable to allocate memory"));
                        freeMmsEntryHeader( modId, entryStart);
                        WE_MEM_FREE( modId, entryStart);
                        bodyInfoList->entryHeader = NULL;

                        return NULL;
                    } 
                    
                    memset(bodyInfoList->entryHeader->value.wellKnownFieldName, 
                        0x00, valueSize + 1);

                    if (bodyInfoList->entryHeader->headerType == 
                        MMS_WELL_KNOWN_CONTENT_ID)
                    {
                        
                        headerEnd += 2;
                        memcpy((char *)
                            bodyInfoList->entryHeader->value.wellKnownFieldName,
                            (char *)headerEnd, (valueSize - 3));
                        headerEnd -= 2;
                    }
                    else
                    {
                        strcpy((char *)
                            bodyInfoList->entryHeader->value.wellKnownFieldName, 
                            (char *)headerEnd);
                    } 

                    headerEnd += valueSize + 1;
                } 
            }
            else
            {
                
                if ( skipEntryHeader( modId, &headerEnd, 
                    bufSize - processedLength) == FALSE)
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "parseEntryHeader: Received an unsupported well-known "
                    "field name\n"));

                    
                    headerEnd = headerStart + bufSize;
                } 
                
                
                if (entryStart == bodyInfoList->entryHeader)
                {
                    freeMmsEntryHeader( modId, bodyInfoList->entryHeader);
                    WE_MEM_FREE( modId, bodyInfoList->entryHeader);
                    bodyInfoList->entryHeader = NULL;
                    entryStart = NULL;
                    prevEntryHeader = NULL;
                    firstEntry = TRUE;
                }
                else
                {
                    freeMmsEntryHeader( modId, bodyInfoList->entryHeader);
                    WE_MEM_FREE( modId, bodyInfoList->entryHeader);
                    bodyInfoList->entryHeader = prevEntryHeader;
                    bodyInfoList->entryHeader->next = NULL;
                } 
                
            } 
        } 

        
        processedLength = (WE_UINT32)(headerEnd - headerStart);
       
    } 
    bodyInfoList->entryHeader = entryStart;
    return headerEnd;
} 










static WE_BOOL skipEntryHeader( WE_UINT8 modId, unsigned char **entryHeader, 
    WE_UINT32 size)
{
    unsigned char  *eHeader;
    WE_UINT32      value;
    WE_BOOL        result;

    result = TRUE; 
    
    if ( entryHeader == NULL || *entryHeader == NULL)
    {
        
        return FALSE;
    } 
    eHeader = *entryHeader;

    switch( SKIP_HIGH_BIT( *eHeader))
    {
        
    case MMS_WELL_KNOWN_CONTENT_DISPOSITION:
    case MMS_WELL_KNOWN_CONTENT_MD5:
    case MMS_WELL_KNOWN_CONTENT_RANGE:
        
        eHeader++;
        eHeader = cnvValueLengthToUint32( eHeader, &value, size);
        if ( eHeader == NULL)
        {
            
            result = FALSE;
            break;
        } 
        
        eHeader += value; 
        break;

        
    case MMS_WELL_KNOWN_CONTENT_BASE:
        eHeader++;
        
        for (;*eHeader; eHeader++, size--)
        {
            if (!size)
            {
                
                return FALSE;
            }
        }
        break;

        
    case MMS_WELL_KNOWN_CONTENT_ENCODING:
        switch (*eHeader)
        {
        case 128: 
        case 129: 
        case 130: 
            eHeader++;
            break;
        default: 
            eHeader++;
            eHeader++;
            break;
        }
        break;

        
    case MMS_WELL_KNOWN_CONTENT_LANGUAGE:
        eHeader++;
        
        if ( *eHeader == ANY_LANGUAGE_VALUE)
        { 
            
            eHeader++; 
        } 
        
        else if ( *eHeader > SHORT_FLAG)
        {   
            
            eHeader++;
        }
        
        else if ( *eHeader <= MAX_SHORT_LENGTH_VALUE)
        {   
            eHeader = cnvLongIntegerToUint32( eHeader, &value, size);
            if ( eHeader == NULL)
            {
                
                result = FALSE;
            }
        } 
        
        else
        {
            
            for (;*eHeader; eHeader++, size--)
            {
                if (!size)
                {
                    
                    return FALSE;
                }
            }
        }
        break;

        
    case MMS_WELL_KNOWN_CONTENT_LENGTH:
        eHeader++;
        
        if ( *eHeader > SHORT_FLAG)
        {   
            
            eHeader++;
        }
        
        else if ( *eHeader <= MAX_SHORT_LENGTH_VALUE)
        {   
            eHeader = cnvLongIntegerToUint32( eHeader, &value, size);
            if ( eHeader == NULL)
            {
                
                result = FALSE;
            }
        } 
        else
        {
            
            result = FALSE;
        }
        break;
        
    default:
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "skipUnknownEntryHeader: Received an unknown well-known field name\n"));
        result = FALSE;
        break;
    } 

    *entryHeader = eHeader;

    return result;
} 












WE_BOOL parseEncodedStringValue( WE_UINT8 modId, unsigned char *buf, 
    WE_UINT32 size, WE_UINT32 *lengthOfString, MmsEncodedText *eText)                                         
{
    unsigned char *tmpPtr;       
    WE_UINT32 length = 0;     
    WE_UINT32 charset;        
    unsigned int len = 0;     
    MmsEncodedText *tmpText;    
    
    *lengthOfString = 0;
    
    
    memset( eText, 0, sizeof( MmsEncodedText));
    
    eText->charset = MMS_UNKNOWN_CHARSET;
    eText->text = NULL; 
    
    
    if ( (SHORT_INTEGER_MIN_VALUE < *buf) && (*buf <= SHORT_INTEGER_MAX_VALUE))
    {
        
        


        eText->charset = MMS_UTF8;
        len = strlen((const char *)buf) + 1;
        eText->text = (char *)WE_MEM_ALLOC( modId, len);
        if ( eText->text == NULL)
        {
            freeMmsEncodedText( modId, eText);
            return FALSE;
        } 
        else
        {
            strcpy( eText->text, (const char *)buf);
            *lengthOfString = (WE_UINT32) len;
        } 
    }  
    else
    {
        
        
        buf = (unsigned char *)cnvValueLengthToUint32( (void *)buf, &length, size);

        if (length > size)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "%s(%d): Illegal EncodedStringValue, length=%d\n", 
                __FILE__, __LINE__, length));
            return FALSE;
        } 

        *lengthOfString = (WE_UINT32) length;

        if (buf == NULL) 
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "%s(%d): Illegal EncodedStringValue, length=%d, buf=%d\n", 
                __FILE__, __LINE__, length, buf));
            freeMmsEncodedText( modId, eText);
            return FALSE;   
        } 
        else if (length == 0) 
        {
            return TRUE; 
        }
        else if (*buf > SHORT_INTEGER_MAX_VALUE) 
        {            
            
            charset = (WE_UINT32) *buf++;
            eText->charset = (MmsCharset)SKIP_HIGH_BIT( (unsigned char)charset);
        }
        else
        {
            tmpPtr = cnvLongIntegerToUint32( (void *)buf, &charset, size);
            if (NULL ==tmpPtr)
            {
                WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, modId,
                    "%s(%d): Error converting charset as Long-integer\n", 
                    __FILE__, __LINE__));
                eText->charset = MMS_UNKNOWN_CHARSET;
                length = 0;
            }
            else
            {
                eText->charset = (MmsCharset) charset;
                length -= (WE_UINT32)(tmpPtr - buf);
                buf = tmpPtr;
            } 
        } 

        
        if (length > 0) 
        {
            if (*buf == 127)
            {
                ++buf;
                --length;
            }

            
            eText->text = WE_MEM_ALLOC( modId, (unsigned int)length + 2);
            if ( eText->text == NULL)
            {
                
                freeMmsEncodedText( modId, eText);
                return FALSE;
            } 
            memcpy(eText->text, buf, (unsigned int)length);
        } 

        

        


        if ( ( tmpText = charsetCvtEncodedString( modId, eText, length)) != NULL)
        {
            
            
            WE_MEM_FREE( modId,  eText->text);
            eText->text = tmpText->text;
            eText->charset = tmpText->charset;
            WE_MEM_FREE( modId,  tmpText);
            length = strlen( eText->text);
        } 
        
        *lengthOfString = length;
    } 

    return TRUE;
} 













WE_BOOL parseFromAddress( WE_UINT8 modId, unsigned char *headerData, 
    const WE_UINT32 headerSize, MmsAddress *from)
{ 
    MmsHeaderValue  headerValue;    
    WE_UINT32 valueLength = 0;      
    unsigned char *tmpBuf;             
    unsigned long size;
    MmsEncodedText eText;           
    WE_UINT32 lengthOfString;      
    WE_BOOL result = TRUE;

    if (from == NULL)
    {
        return FALSE; 
    } 

    memset ( &headerValue, 0x00, sizeof(headerValue));
    memset( &eText, 0x00, sizeof(MmsEncodedText));
    from->address = NULL;
    from->name.text = NULL;
    from->name.charset = MMS_UNKNOWN_CHARSET;   
    from->addrType = MMS_EMAIL;

     
    if (mmsPduGet(headerData,  headerSize, MMS_FROM, &headerValue))
    {  
        size = (WE_UINT32)headerData - (WE_UINT32)headerValue.from + headerSize;

         
        tmpBuf = (unsigned char *)cnvValueLengthToUint32( headerValue.from, 
            &valueLength, size);

        if (valueLength == 0 || tmpBuf == NULL)
        {
            return FALSE;  
        } 
       
        
        if (((MmsFromType)*tmpBuf == MMS_FROM_INSERT_ADDRESS) || 
            (valueLength <= 1L))    
        {
            


        }    
        
        else if ((MmsFromType)*tmpBuf == MMS_FROM_ADDRESS_PRESENT)  
        {
            tmpBuf++; 
            size = (WE_UINT32)headerData - (WE_UINT32)tmpBuf + headerSize;
            if ( !parseEncodedStringValue( modId, tmpBuf, size, 
                &lengthOfString, &eText))
            {
                
                freeMmsAddress( modId, from);
                
                return FALSE;
            } 

            
	    if (lengthOfString == 0)
            {
                
                return TRUE;
            }
            else if (lengthOfString > valueLength)
            { 
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Field is to long \n", __FILE__, __LINE__));
            } 
            else if ( (result = splitAddressString( modId, 
                (unsigned char *)eText.text, &from->address, &from->name.text, 
                &from->addrType)) == TRUE)
            {
                from->name.charset = eText.charset;
            } 
            else if ( result == FALSE)
            {
                
                freeMmsAddress( modId, from);
                return FALSE;
            } 

            freeMmsEncodedText( modId, &eText);
        } 
    } 

    return TRUE;
} 















static unsigned char *parseContentTypeParams( WE_UINT8 modId,  unsigned char *buf, 
    WE_UINT32 size, MmsAllParams **params, WE_UINT32 paramLen)
{
    MmsAllParams *tmpParam;  
    MmsAllParams *previous;  
    unsigned char *tmpBuf = NULL;  
   
    tmpBuf = buf;

    *params = WE_MEM_ALLOC( modId, sizeof( MmsAllParams));
    if ( *params == NULL)
    {
        
        return NULL;
    } 
    memset( *params, 0, sizeof( MmsAllParams));
    tmpParam = *params;
    previous = tmpParam;
    
    while ( tmpBuf < (buf + paramLen))
    {
        if ( (tmpBuf = parseParam( modId, tmpBuf, size, tmpParam)) == NULL)
        {
             
            
            
            if ( tmpParam == *params)
            {
                *params = NULL;
            } 
            
            
            if (previous != NULL)
            {
                previous->next = NULL;
            } 
            
            freeMmsContentTypeParams( modId, tmpParam);
            
            WE_MEM_FREE( modId, tmpParam);
            return NULL;
        } 
        else
        {
            if(tmpBuf < (buf + paramLen))
            {
                tmpParam->next = WE_MEM_ALLOC( modId, sizeof( MmsAllParams));
                if ( tmpParam->next == NULL)
                {
                     
                    if ( *params)
                    {
                        freeMmsContentTypeParams( modId, *params);
                        WE_MEM_FREE( modId, *params);
                        *params = NULL; 
                    } 
                    return NULL;
                } 
                previous = tmpParam;
                tmpParam = tmpParam->next;
                memset( tmpParam, 0, sizeof( MmsAllParams));                
            } 
        }       
    } 
    return tmpBuf;
} 












static unsigned char *parseParam( WE_UINT8 modId, unsigned char *buf, 
    WE_UINT32 size, MmsAllParams *params)
{
    WE_BOOL    canBeStr = TRUE;
    WE_BOOL    canBeShort = TRUE;
    WE_BOOL    canBeLong = TRUE;
    WE_UINT32 tmpUint;
    WE_UINT32 value = 0; 
    unsigned char tmpValue;
    unsigned char *tmpPtr;
    unsigned char *lastPos = buf + size;
    unsigned char *startPos = buf;
    int i=0;
	WE_BOOL    isQuoted = FALSE;

    params->key = NULL;
    params->type = MMS_PARAM_INTEGER;
    params->next = NULL;

    if (size < 2)         
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Too siall amount of data for a parameter\n",
            __FILE__, __LINE__));
        return NULL;
    }
    else if (*buf >= SHORT_FLAG)
    {   
        tmpValue = *buf++;
        value = (WE_UINT32) SKIP_HIGH_BIT( tmpValue);
    }
    else if ( *buf < LENGTH_QUOTE)
    {   
        buf = cnvLongIntegerToUint32( buf, &value, size);
        if ( buf == NULL)
        {   
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Unable to convert LongInteger to WE_UINT32",
                    __FILE__, __LINE__));
            return NULL;
        } 
    } 
    else
    { 
        tmpValue = *buf;
        value = MMS_TEXT_KEY;
        














        tmpPtr = mmsPduSkipString( buf, size - (unsigned long)(buf - startPos));
        if (tmpPtr == NULL)
        {
            return NULL;
        }
        tmpUint = tmpPtr-buf;
        params->key = WE_MEM_ALLOC( modId, tmpUint);
        if ( params->key == NULL)
        {
            
            return NULL;
        } 
        memcpy( params->key, buf, tmpUint);
        buf += tmpUint;




        



        i = 0;
        while(ParamWellKnowValueTypeTable[i].stringValue != NULL)
        {
            if (0 == we_cmmn_strcmp_nc( (const char *)params->key, 
                                         ParamWellKnowValueTypeTable[i].stringValue))
            {
                value = ParamWellKnowValueTypeTable[i].knownValue;
                break;
            }
            i++;
        }
    } 


    


    switch ((MmsParam)value)
    {
    case MMS_CHARSET:
        break;  
    case MMS_NAME:
    case MMS_FILENAME:
    case MMS_NAME_14:
    case MMS_FILENAME_14:
    case MMS_LEVEL:
        
        canBeLong = FALSE;
        break;
    case MMS_TYPE:
        
        canBeStr = FALSE;
        break;
    case MMS_START_REL:
    case MMS_START_REL_14:
        
    case MMS_TYPE_REL:
        
    case MMS_START_INFO_REL:
    case MMS_START_INFO_REL_14:
        canBeShort = FALSE;
        canBeLong = FALSE;
        break;    
    case MMS_TEXT_KEY:
        


        break;
    case MMS_UNHANDLED:
    default :
        


        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "parseContentTypeParams: \
                    Received unknown param."));
        break;
    } 
    
    



    
    if (*buf >= SHORT_FLAG)
    {   
        if (FALSE == canBeShort)
        {
            ++buf;
            params->param = MMS_UNHANDLED;
            params->type = MMS_PARAM_INTEGER;
            params->value.integer = 0;

            return buf;
        }
    }
    else if (*buf < LENGTH_QUOTE)
    {   
        if (FALSE == canBeLong)
        {
            buf = cnvLongIntegerToUint32( buf, &value,
                      size - (unsigned long)(buf - startPos));
            params->param = MMS_UNHANDLED;
            params->type = MMS_PARAM_INTEGER;
            params->value.integer = 0;
            
            return buf;
        }
    } 
    else if (FALSE == canBeStr)
    {   
        buf = mmsPduSkipString( buf, size - (unsigned long)(buf - startPos));

        params->param = MMS_UNHANDLED;
        params->type = MMS_PARAM_INTEGER;
        params->value.integer = 0;
        
        return buf;
    } 


    


    switch ((MmsParam)value)
    {
    case MMS_CHARSET:
        
        if ( *buf == ANY_CHARSET_VALUE)
        {   
            
            params->type = MMS_PARAM_INTEGER;
            params->value.integer = MMS_UTF8; 
        }
        else if ( *buf >= SHORT_FLAG)
        {   
            tmpValue = SKIP_HIGH_BIT(*buf);
            buf++;
            params->type = MMS_PARAM_INTEGER;
            params->value.integer = (MmsCharset)tmpValue;
        }
        else if ( *buf < LENGTH_QUOTE)
        {   
            tmpPtr = cnvLongIntegerToUint32( buf, &tmpUint, 
                (WE_UINT32)(lastPos - buf));
            if ( tmpPtr == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Unable to convert LongInteger to WE_UINT32",
                    __FILE__, __LINE__));
                return NULL;  
            }
            else
            {
                params->type = MMS_PARAM_INTEGER;
                params->value.integer = (MmsCharset)tmpUint;
                buf = tmpPtr; 
            } 
        }
        else
        {   
            
            buf = mmsPduSkipString( buf, size - (unsigned long)(buf - startPos));
            if (buf == NULL)
            {
                return NULL;
            }
            params->type = MMS_PARAM_INTEGER;
            params->value.integer = MMS_UNKNOWN_CHARSET;
        }
        break;  

    case MMS_LEVEL:
        
        if ( *buf >= SHORT_FLAG)
        {   
            params->type = MMS_PARAM_INTEGER;
            params->value.integer = SKIP_HIGH_BIT(*buf);
            buf++;
            params->value.string = NULL;
        }
        else
        {   
            params->type = MMS_PARAM_STRING;
            buf = cnvTextValueToRFC2616String(buf, modId, size - (unsigned long)(buf - startPos), &params->value.string );
            if (NULL == buf)
            {
                return NULL;
            }
        } 
        break;
    case MMS_TYPE:
        
        params->type = MMS_PARAM_INTEGER;
        params->value.string = NULL;

        if (*buf >= SHORT_FLAG)
        {   
            params->value.integer = SKIP_HIGH_BIT(*buf);
            buf++;
        }
        else
        {   
            buf = cnvLongIntegerToUint32( buf, &tmpUint, 
                (WE_UINT32)(lastPos - buf));
            if ( buf == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Unable to convert LongInteger to WE_UINT32",
                    __FILE__, __LINE__));
                return NULL;  
            }
            else
            {
                params->type = MMS_PARAM_INTEGER;
                params->value.integer = (MmsCharset)tmpUint;
            } 
        } 
        break;
    case MMS_START_REL:
    case MMS_START_REL_14:    
        
        params->type = MMS_PARAM_STRING;
        


        if (*buf == 0x22)
        {
            buf++; 
			isQuoted = TRUE;
        }
        
        


        tmpPtr = mmsPduSkipString( buf, size - (unsigned long)(buf - startPos));
        if (tmpPtr == NULL)
        {
            return NULL;
        }

        


        tmpUint = tmpPtr-buf;
		if(*buf=='<' && isQuoted == TRUE && buf[tmpUint-3] == '>')
		{
			 params->value.string = WE_MEM_ALLOC( modId, tmpUint-3); 
			if ( params->value.string == NULL)
            {
                
                return NULL;
            } 
            tmpPtr = params->value.string;

            


            memcpy( params->value.string, buf+1, tmpUint - 4);  
            tmpPtr += (tmpUint - 4);
            *tmpPtr = 0; 
            buf += tmpUint; 
        }
        else if (*buf=='<' && buf[tmpUint-2] == '>')
        {
            params->value.string = WE_MEM_ALLOC( modId, tmpUint-2); 
            if ( params->value.string == NULL)
            {
                
                return NULL;
            } 
            tmpPtr = params->value.string;

            


            memcpy( params->value.string, buf+1, tmpUint - 3);  
            tmpPtr += (tmpUint - 3);
            *tmpPtr = 0; 
            buf += tmpUint; 
        }
        else
        {
            params->value.string = WE_MEM_ALLOC( modId, tmpUint); 
            if ( params->value.string == NULL)
            {
                
                return NULL;
            } 

            tmpPtr = params->value.string;
            


            memcpy( params->value.string, buf, tmpUint);  
            buf += tmpUint; 
        }
        break;
    case MMS_TYPE_REL:
        
    case MMS_START_INFO_REL:
    case MMS_START_INFO_REL_14:    
        
    case MMS_NAME_14:
        
    case MMS_FILENAME_14: 
         
    case MMS_NAME:
        
    case MMS_FILENAME:
        
        params->type = MMS_PARAM_STRING;
        tmpPtr = mmsPduSkipString( buf, size - (unsigned long)(buf - startPos));
        if (tmpPtr == NULL)
        {
            return NULL;
        }
        tmpUint = tmpPtr-buf;
        params->value.string = WE_MEM_ALLOC( modId, tmpUint);
        if ( params->value.string == NULL)
        {
            
            return NULL;
        } 

        memcpy( params->value.string, buf, tmpUint);
        buf += tmpUint;
        break;    
    case MMS_TEXT_KEY:
    case MMS_UNHANDLED:
    default:
        



        
        

        if ( *buf >= SHORT_FLAG)
        {   
            params->type = MMS_PARAM_INTEGER;
            params->value.integer = SKIP_HIGH_BIT(*buf);
            buf++;
            params->value.string = NULL;
        }
        else if ( *buf < LENGTH_QUOTE)
        {   
            buf = cnvLongIntegerToUint32( buf, &tmpUint, 
                (WE_UINT32)(lastPos - buf));
            if ( buf == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Unable to convert LongInteger to WE_UINT32",
                    __FILE__, __LINE__));
                return NULL;  
            }
            else
            {
                params->type = MMS_PARAM_INTEGER;
                params->value.integer = (MmsCharset)tmpUint;
            } 
        } 
        else
        {   
            params->type = MMS_PARAM_STRING;
            buf = cnvTextValueToRFC2616String(buf, modId, size - (unsigned long)(buf - startPos), &params->value.string );
            if (NULL == buf)
            {
                return NULL;
            }
        } 
        break;
    } 

    params->param = (MmsParam)value;

    return buf;
} 










MmsAddressList* parseAddress( WE_UINT8 modId, MmsHeaderTag tag, 
    unsigned char *headerData, WE_UINT32 headerDataSize) 
{
    unsigned int index = 0;
    MmsAddressList *adrList = NULL;
    MmsAddressList *startOfList = NULL; 
    MmsHeaderValue  headerValue;
    unsigned char *buffer; 
    WE_UINT32 size = 0;

    memset( &headerValue, 0x00, sizeof( headerValue));

     
    while (index <= headerDataSize)
    {
        if (mmsPduGet( headerData + index,  (headerDataSize - index),
            tag, &headerValue))
        {
            
            switch (tag) 
            {
            case MMS_BCC:
                buffer = (unsigned char *)headerValue.bcc;
                break;
            case MMS_CC:
                buffer = (unsigned char *)headerValue.cc;
                break;
            case MMS_TO: 
                buffer = (unsigned char *)headerValue.to;
                break; 
            case MMS_FROM:  
            default: 
                return (MmsAddressList*) NULL; 
            }   
            
            if (index == 0)
            {  
                adrList = WE_MEM_ALLOC( modId, sizeof(MmsAddressList));
                if ( adrList == NULL)
                {
                    
                    if ( startOfList)
                    {
                        freeMmsAddressList( modId, startOfList);
                        WE_MEM_FREE( modId, startOfList);
                        startOfList = NULL;
                    } 
                    return NULL;
                } 
                memset( adrList, 0, sizeof(MmsAddressList));
                startOfList = adrList; 
            } 
            else if (adrList != NULL)
            {
                adrList->next = WE_MEM_ALLOC( modId, sizeof(MmsAddressList));
                if ( adrList->next == NULL)
                {
                    
                    if ( startOfList)
                    {
                        freeMmsAddressList( modId, startOfList);
                        WE_MEM_FREE( modId, startOfList);
                        startOfList = NULL;
                    } 
                    return NULL;
                } 
                memset( adrList->next, 0, sizeof(MmsAddressList));
                adrList= adrList->next;
            } 
            else 
            {
                if ( startOfList)
                {
                    freeMmsAddressList( modId, startOfList);
                    WE_MEM_FREE( modId, startOfList);
                    startOfList = NULL;
                } 
                return NULL;  
            }  
            
            size = (WE_UINT32)headerData - (WE_UINT32)buffer + headerDataSize;

           (void)parseOneAddress( modId, buffer, size, &adrList->current);
            
             
            index = (unsigned int)(buffer - headerData) + 
                strlen( (const char *)buffer) + 1; 
            adrList->next = NULL;
            
        } 
        else 
        {
            return startOfList;
        } 
    }  
    
    return startOfList;
} 













unsigned char *parseOneAddress( WE_UINT8 modId, unsigned char *buffer, 
    WE_UINT32 size, MmsAddress *address)
{
    WE_UINT32 encodedLength = 0;
    MmsEncodedText etext; 
    WE_UINT32  len = 0;
    unsigned char *endPointer = NULL;

    if ( buffer == NULL || size == 0)
    {
        
        return NULL;
    } 

    memset( &etext, 0x00, sizeof(MmsEncodedText));
    len = mmsStrnlen((const char *)buffer, size);

    endPointer = (unsigned char *)((WE_UINT32)buffer + len);

     
    if (*buffer <= LENGTH_QUOTE)
    {
        if (parseEncodedStringValue( modId, buffer, size, 
            &encodedLength, &etext) == FALSE)
        {
            
            return NULL;
        } 
        
        if (encodedLength > 0)
        {
            address->name.charset = etext.charset;                  
            
            if (!splitAddressString( modId, (unsigned char*)etext.text, 
                &address->address, &address->name.text, &address->addrType))
            {
               
               WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "parseOneAddress: Failed to parse one address."));                        
            }
        }   

        freeMmsEncodedText( modId, &etext);
    }
    else  
    {
        address->name.charset = MMS_UTF8;
        
        if (!splitAddressString( modId, buffer,  &address->address,
            &address->name.text, &address->addrType))
        {
             
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "parseOneAddress: Failed to parse one address."));
        }                
    } 

    return endPointer;
} 








static WE_BOOL isValidIp4Address( char *addr, int len)
{
#define NUM_OF_GROUPS           4
#define NUM_OF_DIGITS_PER_GROUP 3
    unsigned int i;  
    unsigned int j;  

    for ( i = 0; i < NUM_OF_GROUPS; ++i)
    {
        for ( j = 0; j < NUM_OF_DIGITS_PER_GROUP; ++j)
        {
            if (*addr == '.')
            {
                

                if (j == 0 || i > NUM_OF_GROUPS - 1)
                {
                    return FALSE;
                } 

                break;
            }
            else if ( !ct_isdigit(*addr))
            {
                return FALSE;
            } 

            ++addr;
            --len;
            
            if (len <= 0)
            {
                if(i==NUM_OF_GROUPS-1 )
                {
                    
                    return TRUE;
                }
                
                return FALSE;
            }
        } 

        ++addr;
        --len;
        
        if (len <= 0)
        {
            return FALSE;
        }
    } 

    while (len > 0)
    {
        if (*addr != ' ')
        {
            return FALSE;
        } 

        ++addr;
        --len;
    } 

    return TRUE;
} 















static WE_BOOL splitAddressString( WE_UINT8 modId, unsigned char *str, 
    char **addr,  char **text, MmsAddressType* adrType)
{
    unsigned char *lesser;   
    unsigned char *greater;  
    unsigned char *tmpPtr;  
    int lengtOfStr = 0;     
    char *tptr;             
    
    if (str == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): No memory allocated \n", __FILE__, __LINE__));

        return FALSE; 
    } 

    
    *addr = NULL; 
    *text = NULL; 
    *adrType = MMS_EMAIL; 
    tmpPtr = str; 
    lesser = NULL;
    greater = NULL;

      
    while (*tmpPtr != '\0')
    {
        if (*tmpPtr == TOKEN_LESSER_THEN)
        {
            if (greater != NULL)  
            {
                return FALSE; 
            } 
            else 
            {
                lesser = tmpPtr; 
            }  
        } 

        if (*tmpPtr == TOKEN_GREATER_THEN)
        {
            if (lesser == NULL) 
            {
                return FALSE; 
            }
            else 
            {
                greater = tmpPtr; 
            } 
        } 
        
        tmpPtr++; 
        lengtOfStr++;
    }  
    
    
    
    if (lengtOfStr == 0) 
    {
        return FALSE; 
    }
    else if (lesser != NULL && greater == NULL)
    {
        return FALSE; 
    }
    else if (lesser == NULL && greater != NULL)
    {
        return FALSE; 
    }
    else if (lesser == NULL && greater == NULL) 
    {  
        unsigned int len  = (unsigned int) (lengtOfStr + 1);

         
        *addr = (char *)WE_MEM_ALLOC( modId, len);
        if ( *addr == NULL )
        {
            
            return FALSE;
        } 

        memset( *addr, 0, len);
        memcpy( *addr, str, (unsigned int)lengtOfStr);
        

        if (strstr(*addr, ADDRESS_TYPE_TOKEN))
        {
            if ( (tptr = strstr(*addr, ADDRESS_TYPE_PLMN)) != NULL)
            {
                *tptr = '\0'; 
                *adrType =  MMS_PLMN;
                *text = NULL;
            }        
            else if ( (tptr = strstr(*addr, ADDRESS_TYPE_EMAIL)) != NULL)
            {
                *tptr = '\0'; 
                *adrType =  MMS_EMAIL;
                *text = NULL;
            }        
            else if ( (tptr = strstr(*addr, ADDRESS_TYPE_IPV4)) != NULL && 
                isValidIp4Address( *addr, (int)(tptr - *addr)))
            {
                *tptr = '\0'; 
                *adrType =  MMS_IPV4;
                *text = NULL;
            }
            else
            {
                


                tptr = strstr(*addr, ADDRESS_TYPE_TOKEN);  
                if (tptr) 
                {
                    *tptr = '\0';
                }                 
                *adrType =  MMS_UNSUPPORTED;
                *text = NULL;
                return FALSE;
            } 
        } 
        else
        {
            
            *text = NULL;
            
            *adrType =  MMS_EMAIL;
            return TRUE;
        }
    }
    
    else if ((lesser != NULL && greater != NULL) && 
        (*lesser == TOKEN_LESSER_THEN && *greater == TOKEN_GREATER_THEN))    
    { 
           
        unsigned int len = (unsigned int)(greater - lesser); 
        
        
        

        if (len > 1)
        {
            *addr = (char *)WE_MEM_ALLOC( modId, len);
            if ( *addr == NULL )
            {
                
                return FALSE;
            } 
            memset( *addr, 0, len);
            memcpy( *addr, lesser + 1, len - 1); 
        }
        else
        {
            
            *addr = NULL;
        }

         
        len = (unsigned int) ( lesser - str);
        
        if (len > 0)
        {
            ++len; 
            *text = (char *)WE_MEM_ALLOC( modId, len);
            if ( *text == NULL)
            {
               
                if (NULL != *addr)
                {
                    WE_MEM_FREE(modId, *addr);
                    *addr = NULL;
                }
                return FALSE;
            } 

            memset( *text, 0, len);
            memcpy( *text, str, len - 1); 

             
            tptr = *text + strlen(*text) - 1;
            
            while((tptr >= *text) && (*tptr == ' '))
            {
               *tptr = '\0';
               tptr--; 
            }  
        }
        else
        {
            *text = NULL;
        }
    }
    else 
    {
        return FALSE; 
    } 

    return TRUE; 
} 










WE_BOOL parseHeaderTime( unsigned char *headerData, WE_UINT32 headerDataSize, 
    MmsGetHeader *mHeader, MmsHeaderTag tag)
{
    MmsHeaderValue  headerValue;
    WE_UINT32 size;

    memset( &headerValue, 0x00, sizeof( MmsHeaderValue));
    if (headerData == NULL || mHeader == NULL)
    {
        return FALSE;
    }
    else if (mmsPduGet( headerData, headerDataSize, tag, &headerValue))
    {
        headerValue.expiry = (unsigned char *)headerValue.expiry + 1;
        if (*(unsigned char *)headerValue.expiry == MMS_EXPIRY_TIME_ABSOLUTE) 
        {
            
            mHeader->expiryTime.type = MMS_TIME_ABSOLUTE; 
            headerValue.expiry = (unsigned char *)headerValue.expiry + 1;
            size = (WE_UINT32)headerData - (WE_UINT32)headerValue.expiry + 
                headerDataSize;

            if (cnvLongIntegerToUint32( (unsigned char *)headerValue.expiry, 
                &mHeader->expiryTime.value, size) == NULL) 
            {
                return FALSE; 
            } 
        }
        else if (*(unsigned char *)headerValue.expiry == MMS_EXPIRY_TIME_RELATIVE)   
        {
            
            mHeader->expiryTime.type = MMS_TIME_RELATIVE; 
            headerValue.expiry = (unsigned char *)headerValue.expiry + 1;
            if (*(unsigned char *)headerValue.expiry >= SHORT_FLAG)    
            {
                
                mHeader->expiryTime.value = *(unsigned char *)headerValue.expiry & 0x7f;
            }
            else                                                
            {
                
                size = (WE_UINT32)headerData - 
                    (WE_UINT32)headerValue.expiry + headerDataSize;

                if (cnvLongIntegerToUint32( (unsigned char *)headerValue.expiry, 
                    &mHeader->expiryTime.value, size) == NULL)
                {
                  return FALSE; 
                } 
            } 
        } 
        else
        {
            mHeader->expiryTime.type = MMS_TIME_ABSOLUTE;
            mHeader->expiryTime.value = 0;                    
        } 
    } 

    return TRUE;
} 








static MmsEncodedText *charsetCvtEncodedString( WE_UINT8 modId, 
    const MmsEncodedText *fromText, const WE_UINT32 fromBufferSize)
{
    MmsEncodedText *toText;
    unsigned char *destBuffer = NULL;
    int destBufferSize = 0;
    const unsigned char *sourceBuffer = NULL;
    int sourceBufferSize = 0;
    int charset = 0;
    
    toText = NULL; 

    
    if ( fromText == NULL || fromText->text == NULL)
    {
        return NULL;
    } 
    
    sourceBuffer = (const unsigned char *)fromText->text;
    sourceBufferSize = (int)fromBufferSize;
    charset = fromText->charset;

    destBuffer = mmsCvtBuf( modId, &charset, sourceBuffer, 
        &sourceBufferSize, &destBufferSize);
      
    if ( destBuffer == NULL)
    {
        
        toText = NULL;
    }
    else 
    {
        
        
        toText = (MmsEncodedText *)WE_MEM_ALLOC( modId, sizeof( MmsEncodedText));
        if (toText == NULL)
        {
            
            WE_MEM_FREE( modId, destBuffer);
            destBuffer = NULL;
            return NULL;
        } 
        memset( toText, 0, sizeof(MmsEncodedText));
        toText->text = (char *)destBuffer;
        toText->charset = (MmsCharset)charset;
    }

    return toText;

} 








MmsKnownMediaType mmsConvertToMmsKnownMediaType( const WE_UINT32 mediaValue)
{
    MmsKnownMediaType mmsValue;

    switch ((MmsKnownMediaType)mediaValue)
    {
    case MMS_TYPE_ANY_ANY:
    case MMS_TYPE_TEXT_ANY:
    case MMS_TYPE_TEXT_HTML:
    case MMS_TYPE_TEXT_PLAIN:
    case MMS_TYPE_TEXT_X_HDML:
    case MMS_TYPE_TEXT_X_TTML:
    case MMS_TYPE_TEXT_X_VCALENDAR:
    case MMS_TYPE_TEXT_X_VCARD:
    case MMS_TYPE_TEXT_VND_WAP_WML:
    case MMS_TYPE_TEXT_VND_WAP_WMLSCRIPT:
    case MMS_TYPE_TEXT_VND_WAP_CHANNEL:
    case MMS_TYPE_MULTIPART_ANY:
    case MMS_TYPE_MULTIPART_MIXED:
    case MMS_TYPE_MULTIPART_FORM_DATA:
    case MMS_TYPE_MULTIPART_BYTERANGES:
    case MMS_TYPE_MULTIPART_ALTERNATIVE:
    case MMS_TYPE_APP_ANY:
    case MMS_TYPE_APP_JAVA_VM:
    case MMS_TYPE_APP_X_WWW_FORM_URLENCODED:
    case MMS_TYPE_APP_X_HDMLC:
    case MMS_TYPE_APP_VND_WAP_WMLC:
    case MMS_TYPE_APP_VND_WAP_WMLSCRIPTC:
    case MMS_TYPE_APP_VND_WAP_CHANNELC:
    case MMS_TYPE_APP_VND_WAP_UAPROF:
    case MMS_TYPE_APP_VND_WAP_WTLS_CA_CERTIFICATE:
    case MMS_TYPE_APP_VND_WAP_WTLS_USER_CERTIFICATE:
    case MMS_TYPE_APP_X_X509_CA_CERT:
    case MMS_TYPE_APP_X_X509_USER_CERT:
    case MMS_TYPE_IMAGE_ANY:
    case MMS_TYPE_IMAGE_GIF:
    case MMS_TYPE_IMAGE_JPEG:
    case MMS_TYPE_IMAGE_TIFF:
    case MMS_TYPE_IMAGE_PNG:
    case MMS_TYPE_IMAGE_VND_WAP_WBMP:
    case MMS_VND_WAP_MULTIPART_ANY:
    case MMS_VND_WAP_MULTIPART_MIXED:
    case MMS_TYPE_APP_VND_WAP_MPART_FORM_DATA:
    case MMS_TYPE_APP_VND_WAP_MPART_BYTERANGES:
    case MMS_VND_WAP_MULTIPART_ALTERNATIVE:
    case MMS_TYPE_APP_XML:
    case MMS_TYPE_TEXT_XML:
    case MMS_TYPE_APP_VND_WAP_WBXML:
    case MMS_TYPE_APP_X_X968_CROSS_CERT:
    case MMS_TYPE_APP_X_X968_CA_CERT:
    case MMS_TYPE_APP_X_X968_USER_CERT:
    case MMS_TYPE_TEXT_VND_WAP_SI:
    case MMS_TYPE_APP_VND_WAP_SIC:
    case MMS_TYPE_TEXT_VND_WAP_SL:
    case MMS_TYPE_APP_VND_WAP_SLC:
    case MMS_TYPE_TEXT_VND_WAP_CO:
    case MMS_TYPE_APP_VND_WAP_COC:
    case MMS_VND_WAP_MULTIPART_RELATED:
    case MMS_TYPE_APP_VND_WAP_SIA:
    case MMS_TYPE_TEXT_VND_WAP_CONNECTIVITY_XML:
    case MMS_TYPE_APP_VND_WAP_CONNECTIVITY_WBXML:
    case MMS_TYPE_APP_VND_OMA_DRM_MESSAGE:
    case MMS_TYPE_APP_VND_OMA_DRM_CONTENT:
    case MMS_TYPE_APP_VND_OMA_DRM_R_XML:
    case MMS_TYPE_APP_VND_OMA_DRM_R_WBXML:
            
        mmsValue = (MmsKnownMediaType)mediaValue;
        break;

    case MMS_VALUE_AS_STRING: 
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "convertMediaType: Received unknown media value: %d\n", 
            mediaValue ));
        mmsValue = MMS_TYPE_ANY_ANY;
        break;
    } 

    return mmsValue;
} 






MmsStatus checkStatus(WE_UINT8 status)
{
    MmsStatus retStatus = MMS_STATUS_UNRECOGNIZED;
    
    switch (status)
    {
    case MMS_STATUS_EXPIRED: 
    case MMS_STATUS_RETRIEVED: 
    case MMS_STATUS_REJECTED:     
    case MMS_STATUS_DEFERRED:     
       retStatus = (MmsStatus)status;
       break;
    case MMS_STATUS_UNRECOGNIZED: 
    default: 
       retStatus = MMS_STATUS_UNRECOGNIZED;
    } 

    return retStatus;
} 
