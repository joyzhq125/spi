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

#include "Mlcreate.h"   
#include "Mlpduc.h"     
#include "Mutils.h"     





























unsigned char *createBodyPartHeader(WE_UINT8 modId, WE_UINT32 *encHeaderLen,
    const MmsBodyParams *bodyParams, WE_UINT32 size, MmsResult *result)
{
    
    unsigned char *encHeader = NULL;   
    
    *result = MMS_RESULT_OK;

    if ( bodyParams->targetType != MMS_PLAIN_MESSAGE)
    {
        



        
        if (( encHeader = createWspBodyPartHead( modId, bodyParams, 
            encHeaderLen, size, result)) == NULL)
        {
            
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "mcreateSetBodyPartHead: unable to createWspBodyPartHead\n"));
            *result = MMS_RESULT_ERROR;
            WE_MEM_FREE( modId, encHeader);
            encHeader = NULL;
        }
    } 
    return encHeader;

} 

  












MmsResult writeDataToFile( WE_UINT8 modId, unsigned char **buf, 
    WE_UINT32 *sizeLeft, int fileHandle, WE_INT32 *filePos)
{
    MmsResult result = MMS_RESULT_OK;     
    long writeResult = 0;                 

    
    if ( *filePos < 0)
    {
        
        if ( ( *filePos = WE_FILE_SEEK ( fileHandle, 0, WE_FILE_SEEK_END)) < 0 )
        {
            
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "WriteDataToFile: Error (%l) moving file pointer.\n",
                *filePos));
            return MMS_RESULT_ERROR;
        } 
    } 
    

    
    writeResult = WE_FILE_WRITE ( fileHandle, (void *)*buf, (long) *sizeLeft);
    if ( writeResult < 0)
    {
        
        switch( writeResult)
        {
            case WE_FILE_ERROR_ACCESS:
                result = MMS_RESULT_FILE_WRITE_ERROR;
                break;
            case WE_FILE_ERROR_DELAYED:
                
                WE_FILE_SELECT( fileHandle, WE_FILE_EVENT_WRITE);
                result = MMS_RESULT_DELAYED;
                break;
            case WE_FILE_ERROR_SIZE:
            case WE_FILE_ERROR_FULL:
                result = MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE;
                break;
            default:
                
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "WriteDataToFile received unknown WE_FILE_WRITE \
                    Result %i\n", writeResult));
                break;
        } 
    } 
    else if ( writeResult == (long) *sizeLeft)
    {
        
        result =  MMS_RESULT_OK;
    } 
    else
    {
        
        *buf += writeResult;  
        *filePos += writeResult;
        *sizeLeft -= (WE_UINT32) writeResult; 
        WE_FILE_SELECT( fileHandle, WE_FILE_EVENT_WRITE);
        result = MMS_RESULT_DELAYED;
    } 
    
    return result;
} 







void freeMmsBodyParams( WE_UINT8 modId, MmsBodyParams *bodyParams)
{
    
    if ( bodyParams == NULL)
    {
        
        return;
    } 
    
    
    freeMmsContentType( modId, &bodyParams->contentType);
    
    
    if ( bodyParams->entryHeader != NULL)
    {
        freeMmsEntryHeader( modId, bodyParams->entryHeader);
        WE_MEM_FREE( modId, bodyParams->entryHeader);
        bodyParams->entryHeader = NULL;
    } 

    return;
} 

