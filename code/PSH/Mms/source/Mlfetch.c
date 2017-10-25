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
#include "We_File.h"   

#include "Mms_Def.h"    
#include "Msig.h"       

#include "Mltypes.h"    
#include "Mlfetch.h"    
#include "Mcpdu.h"      
#include "Mlpdup.h"      
#include "Mlfieldp.h"   
#include "Mutils.h"     

#include "Fldmgr.h" 

































MmsResult readDataFromFile( WE_UINT8 modId, unsigned char **buf, 
    WE_UINT32 *size, int fileHandle, WE_INT32 *filePos)
{
    MmsResult result = MMS_RESULT_OK;     
    long readResult = 0;                  

    
    if ( WE_FILE_SEEK ( fileHandle, *filePos, WE_FILE_SEEK_SET) < 0 )
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "readDataFromFile: Error moving file pointer.\n"));
        return MMS_RESULT_ERROR;
    } 
    
    
    readResult = WE_FILE_READ ( fileHandle, *buf, (long)*size); 
    if ( readResult < 0)
    {
        
        switch( readResult)
        {
            case WE_FILE_ERROR_ACCESS:
                result = MMS_RESULT_FILE_READ_ERROR;
                *size = 0;
                break;
            case WE_FILE_ERROR_DELAYED:
                
                WE_FILE_SELECT( fileHandle, WE_FILE_EVENT_READ);
                result = MMS_RESULT_DELAYED;
                *size = 0;
                break;
            default:
                
                result = MMS_RESULT_ERROR;
                *size = 0;
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "readDataFromFile received unknown WE_FILE_READ Result %i\n", readResult));
                break;
        } 
    } 
    else if ( readResult == (long)*size)
    {
        
        *size = (WE_UINT32)readResult;
        *filePos += readResult;
        result =  MMS_RESULT_OK;
    } 
    else if ( readResult == 0)
    {
        
        *size = 0;
        result = MMS_RESULT_FILE_READ_ERROR;
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "readDataFromFile received zero read bytes from WE_FILE_READ\n"));
    } 
    else
    {
        
        *size = (WE_UINT32)readResult;
        *filePos += readResult;
        *buf += readResult;
        result = MMS_RESULT_DELAYED;
        WE_FILE_SELECT( fileHandle, WE_FILE_EVENT_READ);
    } 
    return result;
} 










MmsResult loadMessageInfoFile( WE_UINT8 modId, MmsMessage *mHandle, 
    MmsTransaction *tHandle)
{
    MmsResult result = MMS_RESULT_OK;
    int infoFileHandle = 0;
    long fileSize = 0;
    char fileName[MMS_PATH_LEN] = {0}; 

    switch ( tHandle->subState)
    {
        case 1:
            
            if ((sprintf( fileName, "%s%lx.%s", MMS_FOLDER, mHandle->msgId, MMS_INFO_SUFFIX)) > (long)sizeof(fileName))
            {
                
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "loadMessageInfoFile: file name is too long\n"));
                return MMS_RESULT_ERROR;
            } 
            
            
            if ( (infoFileHandle = 
                WE_FILE_OPEN( modId, fileName, WE_FILE_SET_RDONLY, 0)) < 0 )
            {
                
                return MMS_RESULT_INFO_FILE_ERROR;
            } 
            
            
            
            if ((fileSize = WE_FILE_GETSIZE ( (const char *)fileName )) < 0)
            {
                
                
                (void) WE_FILE_CLOSE ( infoFileHandle);
                return MMS_RESULT_INFO_FILE_ERROR;
            } 
            
            
            tHandle->buf = WE_MEM_ALLOC( modId, (WE_UINT32)fileSize);
            if( tHandle->buf == NULL)
            {
                
                (void) WE_FILE_CLOSE ( infoFileHandle);
                return MMS_RESULT_INSUFFICIENT_MEMORY;
            } 
            
            
            tHandle->sizeOfBuf = fileSize;
            tHandle->bytesLeft = (WE_UINT32)fileSize;
            tHandle->bufferPos = tHandle->buf;
            tHandle->fileHandle = infoFileHandle;
            infoFileHandle = 0;
            tHandle->isMessageFile = FALSE;
            tHandle->filePos = 0;
            tHandle->subState = 2;
            
        case 2:
            
            result = readDataFromFile( modId, 
                (unsigned char **)&tHandle->bufferPos, &tHandle->bytesLeft,
                tHandle->fileHandle, &tHandle->filePos);
            if ( result == MMS_RESULT_OK)
            {
                
                
                mHandle->infoHandle.buffer = tHandle->buf;
                mHandle->infoHandle.bufferSize = (WE_UINT32)tHandle->sizeOfBuf;
                (void) WE_FILE_CLOSE ( tHandle->fileHandle);
                tHandle->fileHandle = 0;
                tHandle->buf = NULL;
                tHandle->bufferPos = NULL;
                tHandle->bytesLeft = 0;
                tHandle->subState = 0;
            }
            else if (result != MMS_RESULT_DELAYED)
            {
                 
                
                (void) WE_FILE_CLOSE ( tHandle->fileHandle);
                tHandle->fileHandle = 0;
            }  
            break;
        default:
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "loadMessageInfoFile: Received unknown state.\n"));
            
            result = MMS_RESULT_ERROR;
            break;
    } 
    return result;
} 




















MmsResult getMessageInfo( WE_UINT8 modId,
                  MmsGetHeader *header, MmsBodyInfoList *bodyInfoList,
                  const MmsMessage *mHandle, MmsTransaction *tHandle)
{

    MmsResult result = MMS_RESULT_OK;
    MmsBodyInfoList *listPtr = NULL;
    WE_UINT32 nrOfParts = getNumberOfParts( mHandle);
    WE_BOOL prevIsMultipart = 0;
    WE_UINT32 bytesReading = 0;

    
    listPtr = (MmsBodyInfoList *) tHandle->tmpPtr;

    while ((WE_UINT32)tHandle->counter <= nrOfParts)
    {
        switch ( tHandle->subState)
        {
        case 1:
            
            bodyInfoList->entryHeader = NULL; 
            bodyInfoList->number = 0;
            bodyInfoList->size = getDataSize( 0, mHandle);
            bodyInfoList->startPos = getDataStart( 0, mHandle);
            bodyInfoList->numOfEntries = getNumberOfSubParts( 0, mHandle);
            if ( bodyInfoList->numOfEntries == 0)
            {
                bodyInfoList->next = NULL;
                
                return MMS_RESULT_OK;
            } 
            
            
            tHandle->tmpPtr = (void *) bodyInfoList;
            tHandle->subState = 2;
        
        case 2:
            
            
            tHandle->sizeOfBuf = (int)getMaxEntrySize( mHandle);
            tHandle->buf = WE_MEM_ALLOC( modId, (WE_UINT32)tHandle->sizeOfBuf);
            if ( tHandle->buf == NULL)
            {
                
                return MMS_RESULT_INSUFFICIENT_MEMORY;
            } 
            tHandle->tmpPtr = (void *) bodyInfoList;
            tHandle->counter = 1; 
            tHandle->subState = 3;
        
        case 3:
            
            listPtr = (MmsBodyInfoList *) tHandle->tmpPtr;
            
            
            listPtr->next = WE_MEM_ALLOC( modId, sizeof( MmsBodyInfoList));
            if ( listPtr->next == NULL)
            {
                
                tHandle->tmpPtr = NULL;
                return MMS_RESULT_INSUFFICIENT_MEMORY;
            } 
            memset( listPtr->next, 0, sizeof(MmsBodyInfoList));
            
            tHandle->bufferPos = tHandle->buf; 
            
            tHandle->bytesLeft = getEntrySize( (WE_UINT32)tHandle->counter, mHandle);  
            tHandle->fileHandle = mHandle->fileHandle;
            tHandle->isMessageFile = TRUE;
            tHandle->filePos = (long)getEntryStart( (WE_UINT32)tHandle->counter, mHandle);
            tHandle->subState = 4;
        
        case 4:
            
            
            bytesReading = tHandle->bytesLeft;
            
            result = readDataFromFile( modId, 
                (unsigned char **)&tHandle->bufferPos, &bytesReading, 
                tHandle->fileHandle, &tHandle->filePos);
            
            tHandle->bytesLeft -= bytesReading;

            if ( result != MMS_RESULT_OK)
            {
                if ( result != MMS_RESULT_DELAYED)
                {
                    
                    tHandle->tmpPtr = NULL;
                } 
                return result;
            } 
            tHandle->subState = 5;
        
        case 5:
            

            
            prevIsMultipart = mmsIsMultiPart( listPtr->contentType->knownValue);
            

            result = parseBodyHeader(modId, prevIsMultipart, 
                ((MmsBodyInfoList *) tHandle->tmpPtr)->next, 
                tHandle->bufferPos, getEntrySize((WE_UINT32)tHandle->counter, 
                mHandle));
            if (result != MMS_RESULT_OK)
            {
                

                tHandle->tmpPtr = NULL;
                return result;
            } 

            if (NULL != listPtr->next->contentType &&
                (listPtr->next->contentType->knownValue == MMS_TYPE_APP_VND_OMA_DRM_MESSAGE
                
                   || (listPtr->next->contentType->knownValue=MMS_VALUE_AS_STRING && we_cmmn_strcmp_nc((const char *)listPtr->next->contentType->strValue, MMS_MEDIA_TYPE_STRING_FORWARD_LOCKED_OMA) == 0)

                )
                
                
                )
            {
                header->forwardLock = TRUE;
            }

            listPtr->next->number = (WE_UINT32)tHandle->counter;
            listPtr->next->numOfEntries = 
                getNumberOfSubParts( (WE_UINT32)tHandle->counter, mHandle);
            listPtr->next->size = getDataSize( (WE_UINT32)tHandle->counter, mHandle);
            listPtr->next->startPos = getDataStart( (WE_UINT32) tHandle->counter, mHandle);

            
            tHandle->tmpPtr = (void *)listPtr->next; 
            tHandle->counter++;

            tHandle->subState = 3;
            break;
        } 
    } 
    
    
    tHandle->tmpPtr = NULL;  

    return result;
} 










MmsResult getMmsHeader( WE_UINT8 modId, MmsGetHeader *mHeader, 
    const MmsMessage *mHandle, MmsTransaction *tHandle)
{
    MmsResult result = MMS_RESULT_OK;

    switch ( tHandle->subState)
    {
        case 1:
            
            tHandle->bytesLeft = getHeaderSize( mHandle);
            
            tHandle->buf = WE_MEM_ALLOC( modId, tHandle->bytesLeft);
            if ( tHandle->buf == NULL)
            {
                
                return MMS_RESULT_INSUFFICIENT_MEMORY;
            }
            tHandle->bufferPos = tHandle->buf;
            tHandle->filePos = 0; 
            tHandle->fileHandle = mHandle->fileHandle;
            tHandle->isMessageFile = TRUE;
            tHandle->subState = 2;
        
        case 2:
            
            result = readDataFromFile( modId, 
                (unsigned char **)&tHandle->bufferPos, &tHandle->bytesLeft, 
                tHandle->fileHandle, &tHandle->filePos);
            if ( result != MMS_RESULT_OK)
            {
                return result;
            } 
            tHandle->bufferPos = tHandle->buf;
            
            result = parseMmsHeader(  modId, tHandle->bufferPos, getHeaderSize( mHandle), 
                mHeader);

            if ( result != MMS_RESULT_OK)
            {
                
                return result;
            }
            WE_MEM_FREE( modId, tHandle->buf);
            tHandle->buf = NULL;
            break;
        default :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "getMmsHeader: Received unknown state.\n"));
            
            result = MMS_RESULT_ERROR;
            break;
    } 
    return result;
} 










static unsigned char *findContentType( unsigned char *entry, WE_UINT32 entrySize, 
    WE_BOOL prevIsMultipart, WE_UINT32 *headerLen)
{
    unsigned char   *contentStart = NULL;
    WE_UINT32  garbage = 0;
    
    if ( entry == NULL && entrySize == 0)
    {
        return NULL;
    } 

    contentStart = entry;   

    if ( prevIsMultipart)
    {
        
        contentStart = cnvUintvarToUint32(contentStart, &garbage, entrySize);
        if (NULL == contentStart)
        {
            return NULL;
        }
    } 
    
    
    contentStart = cnvUintvarToUint32(contentStart, headerLen, entrySize);
    if (NULL == contentStart)
    {
        return NULL;
    }
    
    contentStart = cnvUintvarToUint32(contentStart, &garbage, entrySize);

    return contentStart;
} 














MmsResult parseBodyHeader( WE_UINT8 modId, WE_BOOL prevIsMultipart, 
    MmsBodyInfoList *msgInfo, unsigned char *data, WE_UINT32 size)
{
    MmsResult result = MMS_RESULT_OK;
    unsigned char *entryPtr = NULL;
    unsigned char *tmpPtr = NULL;
    WE_UINT32 headerLen = 0;
    WE_UINT32 currentSize = 0;

    currentSize = size;
    tmpPtr = data;
    
    
    entryPtr = findContentType( data, size, prevIsMultipart, &headerLen);
    if ( entryPtr == NULL)
    {
        
        return MMS_RESULT_MESSAGE_CORRUPT;
    } 
    currentSize -= (WE_UINT32) (entryPtr - tmpPtr);
    
    tmpPtr = entryPtr;
    entryPtr = parseContentType( modId, &msgInfo->contentType, entryPtr, currentSize);
    currentSize -= (WE_UINT32) (entryPtr - tmpPtr);
    if ( entryPtr == NULL)
    {
        
        result = MMS_RESULT_MESSAGE_CORRUPT;
    }
    else if ( currentSize > 0L)
    {
        
        entryPtr = parseEntryHeader( modId, entryPtr, currentSize, 
            msgInfo);
        if ( entryPtr == NULL)
        {
            
            result = MMS_RESULT_MESSAGE_CORRUPT;
        } 
    } 

    return result;
} 







WE_UINT32 getMaxPartSize( const MmsMessage *mHandle)
{
    WE_UINT32 nrOfParts = 0;
    WE_UINT32 i = 0; 
    WE_UINT32 max = 0;
    WE_UINT32 current = 0;
    unsigned char *buffer = mHandle->infoHandle.buffer;
    WE_UINT32 position = 0;
    
    nrOfParts = getNumberOfParts ( mHandle);

    for ( i = 0; i < nrOfParts; i++ )
    {
        position = ((WE_UINT32)POS_INT_MSG_NUMBER + POS_DATA_SIZE + 
            (POS_INT_PART_NUMBER * i)) * sizeof( WE_UINT32);
        
        
        current = *( (WE_UINT32 *)(buffer + position));
        
        if ( max < current )
        {
            max = current;
        } 
    } 
    return max;
} 








WE_UINT32 getDataSize( WE_UINT32 number, const MmsMessage *mHandle)
{
    unsigned char *buffer = mHandle->infoHandle.buffer;
    WE_UINT32 value = 0;
    WE_UINT32 position = 0;
    
    position = ( (WE_UINT32)POS_INT_MSG_NUMBER + POS_DATA_SIZE + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);

    
    value = *((WE_UINT32 *)(buffer + position));    
    

    return value;
} 








WE_UINT32 getEntrySize( WE_UINT32 number, const MmsMessage *mHandle)
{
    unsigned char *buffer = mHandle->infoHandle.buffer;
    WE_UINT32 value = 0;
    WE_UINT32 position = 0;

    position = ((WE_UINT32)POS_INT_MSG_NUMBER + POS_ENTRY_SIZE + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);

    
    value = *((WE_UINT32 *)(buffer + position)); 
    

    return value;
} 







WE_UINT32 getMaxEntrySize( const MmsMessage *mHandle)
{
    WE_UINT32 nrOfParts = 0;
    WE_UINT8 i = 0; 
    WE_UINT32 max = 0;
    WE_UINT32 current = 0;
    
    nrOfParts = getNumberOfParts ( mHandle);

    if ( nrOfParts >= 1)
    {
        
        for ( i = 1; i <= nrOfParts; i++ )
        {
            current = getEntrySize( i, mHandle);
            if ( max < current )
            {
                max = current;
            } 
        } 
    }
    else
    {
        max = 0;
    } 
    
    return max;
} 








WE_UINT32 getDataStart( WE_UINT32 number, const MmsMessage *mHandle)
{
    unsigned char *buffer = mHandle->infoHandle.buffer;
    WE_UINT32 value = 0;
    WE_UINT32 position = 0;

    position = ( (WE_UINT32)POS_INT_MSG_NUMBER + POS_DATA_START + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);

    
    value = *((WE_UINT32 *)(buffer + position)); 
    

    return value;
} 








WE_UINT32 getEntryStart( WE_UINT32 number, const MmsMessage *mHandle)
{
    unsigned char *buffer = mHandle->infoHandle.buffer;
    WE_UINT32 value = 0;
    WE_UINT32 position = 0;

    position = ( (WE_UINT32)POS_INT_MSG_NUMBER + POS_ENTRY_START + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);

    
    value = *((WE_UINT32 *)(buffer + position));
    

    return value;
} 







WE_UINT32 getHeaderSize( const MmsMessage *mHandle)
{
    return getEntrySize( 0, mHandle);
} 







WE_UINT32 getNumberOfParts( const MmsMessage *mHandle)
{
    WE_UINT32 value = 0;
    unsigned char *buffer = mHandle->infoHandle.buffer;
    WE_UINT32 position = 0;

    position = (WE_UINT32)POS_TOTAL_NO_OF_PARTS * sizeof(WE_UINT32);
    
    
    value = *((WE_UINT32 *)(buffer + position));
    

    return value;
} 








WE_UINT32 getNumberOfSubParts( WE_UINT32 number, const MmsMessage *mHandle)
{
    WE_UINT32 value = 0;
    unsigned char *buffer = mHandle->infoHandle.buffer;
    WE_UINT32 position = 0;

    position = ((WE_UINT32)POS_INT_MSG_NUMBER + POS_NO_OF_SUBPARTS  + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);

    
    value = *((WE_UINT32 *)(buffer + position));
    
    return value;
} 








WE_UINT32 getDrmStatus( WE_UINT32 number, const MmsMessage *mHandle)
{
    WE_UINT32 value = 0;
    unsigned char *buffer = mHandle->infoHandle.buffer;
    WE_UINT32 position = 0;

    position = ((WE_UINT32)POS_INT_MSG_NUMBER + POS_DRM_STATUS  + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);

    
    value = *((WE_UINT32 *)(buffer + position));
    
    return value;
} 








MmsContentEncodingValue getEncodingType( WE_UINT32 number, 
    const MmsMessage *mHandle)
{
    WE_UINT32 value = 0;
    unsigned char *buffer = mHandle->infoHandle.buffer;
    WE_UINT32 position = 0;

    position = ((WE_UINT32)POS_INT_MSG_NUMBER + POS_ENCODING_TYPE  + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);

    
    value = *((WE_UINT32 *)(buffer + position));
    
    return (MmsContentEncodingValue)value;
} 







void setHeaderSize( unsigned char *buffer, WE_UINT32 value)
{
    setEntrySize( buffer, 0, value);
} 






void setHeaderStart( unsigned char *buffer)
{
    setEntryStart( buffer, 0, 0);
} 







void setNumberOfParts( unsigned char *buffer, WE_UINT32 value)
{
    unsigned char *ptr = NULL;

    ptr = buffer;
    ptr += ( POS_TOTAL_NO_OF_PARTS * sizeof(WE_UINT32));
    
    *((WE_UINT32 *)ptr) = value;
    
} 








void setNumberOfSubParts( unsigned char *buffer, WE_UINT32 number, WE_UINT32 value)
{
    unsigned char *ptr = NULL;
    WE_UINT32 position = 0;

    ptr = buffer;
    position = ((WE_UINT32)POS_INT_MSG_NUMBER + POS_NO_OF_SUBPARTS + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);
    
    ptr += position;
    
    *((WE_UINT32 *)ptr) = value;
    
} 








void setDrmStatus( unsigned char *buffer, WE_UINT32 number, WE_UINT32 value)
{
    unsigned char *ptr = NULL;
    WE_UINT32 position = 0;

    ptr = buffer;
    position = ((WE_UINT32)POS_INT_MSG_NUMBER + POS_DRM_STATUS + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);
    
    ptr += position;
    
    *((WE_UINT32 *)ptr) = value;
    
} 









void setEncodingType( unsigned char *buffer, WE_UINT32 number, 
    MmsContentEncodingValue value)
{
    unsigned char *ptr = NULL;
    WE_UINT32 position = 0;

    ptr = buffer;
    position = ((WE_UINT32)POS_INT_MSG_NUMBER + POS_ENCODING_TYPE + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);
    
    ptr += position;
    
    *((WE_UINT32 *)ptr) = value;
    
} 








void setEntrySize( unsigned char *buffer, WE_UINT32 number, WE_UINT32 value)
{
    unsigned char *ptr = NULL;
    WE_UINT32 position = 0;
    
    ptr = buffer;
    position = ((WE_UINT32)POS_INT_MSG_NUMBER + POS_ENTRY_SIZE + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);
    ptr += position;
    
    *((WE_UINT32 *)ptr) = value;
    

} 








void setEntryStart( unsigned char *buffer, WE_UINT32 number, WE_UINT32 value)
{
    unsigned char *ptr = NULL;
    WE_UINT32 position = 0;
    ptr = buffer;
    
    position = ((WE_UINT32)POS_INT_MSG_NUMBER + POS_ENTRY_START + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);

    ptr += position;
    
    *((WE_UINT32 *)ptr) = value;
    
} 








void setDataSize( unsigned char *buffer, WE_UINT32 number, WE_UINT32 value)
{
    unsigned char *ptr = NULL;
    WE_UINT32 position = 0;
    
    ptr = buffer;
    
    position = ((WE_UINT32)POS_INT_MSG_NUMBER + POS_DATA_SIZE + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);
   
    ptr += position;
    
    *((WE_UINT32 *)ptr) = value;
    

} 








void setDataStart( unsigned char *buffer, WE_UINT32 number, WE_UINT32 value)
{
    unsigned char *ptr = NULL;
    WE_UINT32 position = 0;
    ptr = buffer;
    
    position = ((WE_UINT32)POS_INT_MSG_NUMBER + POS_DATA_START + 
        (POS_INT_PART_NUMBER * number)) * sizeof( WE_UINT32);

    ptr += position;
    
    *((WE_UINT32 *)ptr) = value;
    

} 
