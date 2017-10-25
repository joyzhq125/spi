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
#include "We_File.h"   
#include "We_Core.h"    
#include "We_Chrs.h"

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    

#include "Mltypes.h"    
#include "Mlcreate.h"   
#include "Mlfetch.h"    
#include "Mutils.h"     






























WE_BOOL copyMmsEncText( WE_UINT8 modId, MmsEncodedText *to, const MmsEncodedText *from)
{
    WE_BOOL ret = FALSE;

    if (from == NULL)
    {   
        return TRUE;
    } 

    to->charset = from->charset;
    if (from->text == NULL)
    {
        to->text = NULL;
        ret = TRUE;
    }
    else if ( (to->text = WE_MEM_ALLOC( modId, strlen(from->text) + 1)) != NULL)
    {
        strcpy( to->text, from->text);
        ret = TRUE;
    } 

    return ret;
} 








WE_BOOL copyMmsAddress( WE_UINT8 modId, MmsAddress *to, const MmsAddress *from)
{
    WE_BOOL ret = FALSE;

    if (from == NULL)
    {   
        return FALSE;
    } 

    to->addrType = from->addrType;
    if ( !copyMmsEncText( modId, &to->name, &from->name))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "%s(%d) copy failed\n", __FILE__, __LINE__));
    }
    else if (from->address == NULL)
    {
        to->address = NULL;
        ret = TRUE;
    }
    else if ( (to->address = WE_MEM_ALLOC( modId, strlen(from->address) + 1)) != NULL)
    {
        strcpy( to->address, from->address);
        ret = TRUE;
    } 

    return ret;
}  








WE_BOOL copyMmsEntryHeader( WE_UINT8 modId, MmsEntryHeader **to, const MmsEntryHeader *from)
{
    MmsEntryHeader *toPtr   = NULL;
    MmsEntryHeader *fromPtr = (MmsEntryHeader *)from;
    WE_UINT32 tmpVal = 0;
    
    *to = NULL;
    
    if (fromPtr == NULL)
    {   
        return TRUE;
    }
    else if ( (toPtr = (MmsEntryHeader *) WE_MEM_ALLOC( modId, sizeof(MmsEntryHeader))) == NULL)
    { 
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "%s(%d) Out of memory\n", __FILE__, __LINE__));
        return FALSE;
    } 

    *to = toPtr;
    while (fromPtr != NULL)
    {
        
        toPtr->headerType = fromPtr->headerType;
        switch ( fromPtr->headerType)
        {
        case MMS_SHORT_CUT_SHIFT_DELIMITER:
            toPtr->value.shortCutShiftDelimiter = 
                fromPtr->value.shortCutShiftDelimiter;
            break;
        case MMS_SHIFT_DELIMITER:
            toPtr->value.shiftDelimiter = fromPtr->value.shiftDelimiter;
            break;
        case MMS_APPLICATION_HEADER:
             if (fromPtr->value.applicationHeader.name != NULL)
            {
                tmpVal = strlen( (const char *)
                    fromPtr->value.applicationHeader.name) + 1;
                toPtr->value.applicationHeader.name = WE_MEM_ALLOC(modId, tmpVal);
                if (toPtr->value.applicationHeader.name == NULL)
                {
					WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
						"%s(%d) Out of memory\n", __FILE__, __LINE__));

                    freeMmsEntryHeader(modId, *to);
                    WE_MEM_FREE( modId, *to);
                    *to = NULL;
                    
                    return FALSE;
                } 

                strcpy( (char *)toPtr->value.applicationHeader.name,
                    (char *)fromPtr->value.applicationHeader.name);
            } 
            if (fromPtr->value.applicationHeader.value != NULL)
            {
                tmpVal = strlen( (const char *)
                    fromPtr->value.applicationHeader.value) + 1;
                toPtr->value.applicationHeader.value = 
                    WE_MEM_ALLOC( modId, tmpVal);
                if (toPtr->value.applicationHeader.value == NULL)
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                        "%s(%d) Out of memory\n", __FILE__, __LINE__));
                    if ( *to)
                    {
                        freeMmsEntryHeader( modId, *to);
                        WE_MEM_FREE( modId, *to);
                        *to = NULL;
                    } 
                    
                    return FALSE;
                } 
                
                memset (toPtr->value.applicationHeader.value, 0, tmpVal);

                strcpy( (char *)toPtr->value.applicationHeader.value,
                    (char *)fromPtr->value.applicationHeader.value);
            } 
            break;
        case MMS_WELL_KNOWN_CONTENT_ID:
        case MMS_WELL_KNOWN_CONTENT_LOCATION:
            if (fromPtr->value.wellKnownFieldName != NULL)
            {
                tmpVal = strlen( (const char *)
                    fromPtr->value.wellKnownFieldName) + 1;
                toPtr->value.wellKnownFieldName = WE_MEM_ALLOC( modId, tmpVal);
                if (toPtr->value.wellKnownFieldName == NULL)
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                        "%s(%d) Out of memory\n", __FILE__, __LINE__));
                    if ( *to)
                    {
                        freeMmsEntryHeader( modId, *to);
                        WE_MEM_FREE( modId, *to);
                        *to = NULL;
                    } 
                    return FALSE;
                } 
                memset ( toPtr->value.wellKnownFieldName, 0, tmpVal);

                strcpy( (char *)toPtr->value.wellKnownFieldName, 
                    (char *)fromPtr->value.wellKnownFieldName);
            } 
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
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "copyEntryHeader: unknown type!\n"));
            break;
        } 

        fromPtr = fromPtr->next;
        if ( fromPtr != NULL)
        {
            toPtr->next = (MmsEntryHeader *) WE_MEM_ALLOC( modId, sizeof( MmsEntryHeader));
            if (toPtr->next == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "%s(%d) Out of memory\n", __FILE__, __LINE__));
                if ( *to)
                {
                    freeMmsEntryHeader( modId, *to);
                    WE_MEM_FREE( modId, *to);
                    *to = NULL;
                } 
                return FALSE;
            } 

            toPtr = toPtr->next;
        }
        else
        {
            toPtr->next = NULL;
        }
    } 

    return TRUE;
} 








WE_BOOL copyMmsAddressList( WE_UINT8 modId, MmsAddressList **toList, const MmsAddressList *fromList)
{
    MmsAddressList *to = NULL;
    MmsAddressList *aNew = NULL;
  
    *toList = NULL;
   
    if (fromList == NULL)
    {   
        return TRUE;
    } 

    while (fromList != NULL)
    {
        aNew = (MmsAddressList *) WE_MEM_ALLOC( modId, sizeof( MmsAddressList));
        if (aNew == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "%s(%d) Out of memory\n", __FILE__, __LINE__));
            if ( *toList)
            {
                freeMmsAddressList( modId, *toList);
                WE_MEM_FREE( modId, *toList);
                *toList = NULL;
            } 
            return FALSE;
        }
        else if (to != NULL)     
        {
            to->next = aNew;
        } 

        to = aNew;
        if ( !copyMmsAddress( modId, &to->current, &fromList->current))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "%s(%d) Copy failed\n", __FILE__, __LINE__));
            if ( *toList)
            {
                freeMmsAddressList( modId, *toList);
                WE_MEM_FREE( modId, *toList);
                *toList = NULL;
            } 
            return FALSE;
        } 

        to->next = NULL;

        if (*toList == NULL)   
        {
            *toList = to;
        } 

        fromList = fromList->next;
    } 

    return TRUE;
}  








WE_BOOL copyMmsContentType( WE_UINT8 modId, MmsContentType *toContent, const MmsContentType *fromContent)
{
    MmsAllParams *to;
    MmsAllParams *from;
    WE_UINT32 len = 0;

    if (fromContent == NULL)
    {   
        return TRUE;
    } 

    toContent->knownValue = fromContent->knownValue;
    if ( fromContent->strValue == NULL)
    {
        toContent->strValue = NULL;
    }
    else
    {
        len = strlen( (const char *)fromContent->strValue) + 1;
        toContent->strValue = WE_MEM_ALLOC( modId, len);
        if (toContent->strValue == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "%s(%d) Out of memory\n", __FILE__, __LINE__));
            return FALSE;
        } 

        strcpy((char *)toContent->strValue, (const char *)fromContent->strValue);
    } 

    if (NULL == fromContent->drmInfo)
    {
        toContent->drmInfo = NULL;
    }
    else
    {
        toContent->drmInfo = WE_MEM_ALLOC(modId, sizeof(MmsDrmInfo));
        if (NULL == toContent->drmInfo)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                "%s(%d) Out of memory\n", __FILE__, __LINE__));
            return FALSE;
        } 
        
        copyMmsDrmInfo(modId, toContent->drmInfo, fromContent->drmInfo);
    }

    if (fromContent->params == NULL)
    {
        toContent->params = NULL;
        return TRUE;
    } 

    from = fromContent->params;
    to = toContent->params = WE_MEM_ALLOC( modId, sizeof(MmsAllParams));
    if (to == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
            "%s(%d) Out of memory\n", __FILE__, __LINE__));
        if ( toContent)
        {
            freeMmsContentType( modId, toContent);
            WE_MEM_FREE( modId, toContent);
            toContent = NULL;
        } 
        
        return FALSE;
    } 

    while (from != NULL)
    {
        if (from->key == NULL)
        {
            to->key = NULL;
        }
        else
        {
            len = strlen( (const char *)from->key) + 1;
            to->key = WE_MEM_ALLOC( modId, len);
            if (to->key == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "%s(%d) Out of memory\n", __FILE__, __LINE__));
                if ( toContent)
                {
                    freeMmsContentType( modId, toContent);
                    WE_MEM_FREE( modId, toContent);
                    toContent = NULL;
                } 
                return FALSE;
            } 
            
            strcpy( (char *)to->key, 
                (const char *)from->key);
            
        }
        
        to->param = from->param;
        to->type = from->type;
        if (from->type == MMS_PARAM_STRING)
        {
            if (from->value.string == NULL)
            {
                to->value.string = NULL;
            }
            else
            {
                len = strlen( (const char *)from->value.string) + 1;
                to->value.string = WE_MEM_ALLOC( modId, len);
                if (to->value.string == NULL)
                {
                    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                        "%s(%d) Out of memory\n", __FILE__, __LINE__));
                    if ( toContent)
                    {
                        freeMmsContentType( modId, toContent);
                        WE_MEM_FREE( modId, toContent);
                        toContent = NULL;
                    } 
                    return FALSE;
                } 

                strcpy( (char *)to->value.string, 
                    (const char *)from->value.string);
            } 
        
        }
        else if (from->type == MMS_PARAM_INTEGER)
        {
            to->value.integer = from->value.integer;
        } 
    
        from = from->next;
        if (from != NULL)
        {
            to->next = WE_MEM_ALLOC( modId, sizeof(MmsAllParams));
            if (to->next == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "%s(%d) Out of memory\n", __FILE__, __LINE__));
                if ( toContent)
                {
                    freeMmsContentType( modId, toContent);
                    WE_MEM_FREE( modId, toContent);
                    toContent = NULL;
                } 
                return FALSE;
            } 

            to = to->next;
        }
        else
        {
            to->next = NULL;
        } 
    } 

    return TRUE;
} 








WE_BOOL copyMmsDrmInfo( WE_UINT8 modId, MmsDrmInfo *toDrmInfo, const MmsDrmInfo *fromDrmInfo)
{
    
    toDrmInfo->drmType = fromDrmInfo->drmType;

    
    if (copyMmsContentType(modId, &toDrmInfo->drmContainerType, &fromDrmInfo->drmContainerType) == FALSE)
    {
        return FALSE;
    }

    
    if (NULL == fromDrmInfo->dcfFile)
    {
        toDrmInfo->dcfFile = NULL;
    }
    else
    {
        toDrmInfo->dcfFile = we_cmmn_strdup( modId, fromDrmInfo->dcfFile);
        if (NULL == toDrmInfo->dcfFile)
        {
            freeMmsContentType(modId, &toDrmInfo->drmContainerType);
            return FALSE;
        }
    }

    return TRUE;
} 





void freeMmsAddress( WE_UINT8 modId, MmsAddress *address)
{
    if (address != NULL && address->address != NULL)
    {
        freeMmsEncodedText( modId, &address->name);
        WE_MEM_FREE( modId, address->address);
        address->address = NULL;
    } 
} 






void freeMmsBodyInfoList( WE_UINT8 modId, MmsBodyInfoList *bodyInfoList)
{
    MmsBodyInfoList *infoList = NULL;
    MmsBodyInfoList *tmpPtr = NULL;
    WE_BOOL doDelete = FALSE;
    
    infoList = bodyInfoList;
    
    while( infoList != NULL)
    {
        
        if (infoList->contentType != NULL)
        {
            freeMmsContentType( modId, infoList->contentType);
            WE_MEM_FREE( modId, infoList->contentType);
            infoList->contentType = NULL;
        } 
        
        if (infoList->entryHeader)
        {
            freeMmsEntryHeader( modId, infoList->entryHeader);
            WE_MEM_FREE( modId, infoList->entryHeader);
            infoList->entryHeader = NULL;
        } 
        
        infoList->number = 0;
        infoList->numOfEntries = 0;
        infoList->size = 0;
        tmpPtr = infoList->next;
        infoList->next = NULL;
        
        
        if ( doDelete)
        {
            WE_MEM_FREE( modId, infoList);
        }
        else
        {
            doDelete = TRUE;
        } 
        infoList = tmpPtr;
    } 

} 








void freeMmsAddressList( WE_UINT8 modId, MmsAddressList *addressList)
{
    MmsAddressList *listPointer = NULL;
    MmsAddressList *aList = NULL;

    freeMmsAddress( modId, &addressList->current);

    
    aList = addressList->next;
    while (aList != NULL)
    {
        listPointer = aList->next;
        freeMmsAddress(modId, &aList->current);
        WE_MEM_FREE( modId, aList);
        aList = listPointer;
    } 

    memset( addressList, 0x00, sizeof(MmsAddressList));

} 






void freeMmsEncodedText( WE_UINT8 modId, MmsEncodedText *encodedText)
{
    if (encodedText != NULL && encodedText->text != NULL) 
    {
        WE_MEM_FREE( modId, encodedText->text);
        encodedText->text = NULL;
    }  
} 






void freeMmsMessageClass( WE_UINT8 modId, MmsMessageClass *messageClass)
{
    if ( messageClass != NULL)
    {
        if (messageClass->textString != NULL)
        {
            WE_MEM_FREE( modId, messageClass->textString);
            messageClass->textString = NULL;
        } 
    } 
} 

void freeMmsContentTypeParams( WE_UINT8 modId, MmsAllParams *params)
{
    MmsAllParams *paramPointer = NULL;  
    WE_BOOL doFree = FALSE;
    
    while (params != NULL)
    {
        paramPointer = params->next;
        if (params->key != NULL)
        {
            WE_MEM_FREE( modId, params->key);
            params->key = NULL;
        } 
        
        if ((params->type == MMS_PARAM_STRING) &&
            (params->value.string != NULL))
        {
            WE_MEM_FREE( modId, params->value.string);
            params->value.string = NULL;
        } 
        
        if (!doFree)
        {
            
            doFree = TRUE;
        }
        else
        {
            WE_MEM_FREE( modId, params);
        } 
        
        params = paramPointer;
    } 
} 






void freeMmsContentType( WE_UINT8 modId, MmsContentType *contentType)
{
        
    if (contentType == NULL)  
    {
        return; 
    }  

    if ( contentType->strValue != NULL)
    {
        WE_MEM_FREE( modId,  contentType->strValue);
        contentType->strValue = NULL;
    } 
    if ( contentType->params)
    {
        freeMmsContentTypeParams( modId, contentType->params);
        WE_MEM_FREE( modId, contentType->params);
        contentType->params = NULL;
    } 
 
    
    if (contentType->drmInfo)
    {
        freeMmsDrmInfo(modId, contentType->drmInfo);
        WE_MEM_FREE(modId, contentType->drmInfo);
        contentType->drmInfo = NULL;
    }
    
} 






void freeMmsDrmInfo( WE_UINT8 modId, MmsDrmInfo *drmInfo)
{
    


    if (NULL != drmInfo->dcfFile)
    {
        WE_MEM_FREE(modId, drmInfo->dcfFile);
        drmInfo->dcfFile = NULL;
    }

    


    freeMmsContentType(modId, &drmInfo->drmContainerType);
} 








void freeMmsSetHeader( WE_UINT8 modId, MmsSetHeader *mHeader)
{
    if ( !mHeader)
    {
        return;
    } 

    freeMmsAddress( modId, &mHeader->from);
    if ( mHeader->to)
    {
        freeMmsAddressList( modId, mHeader->to);
        WE_MEM_FREE( modId, mHeader->to);
    } 
    if ( mHeader->cc)
    {
        freeMmsAddressList( modId, mHeader->cc);
        WE_MEM_FREE( modId, mHeader->cc);
    } 
    if ( mHeader->bcc)
    {
        freeMmsAddressList( modId, mHeader->bcc);
        WE_MEM_FREE( modId, mHeader->bcc);
    } 
    freeMmsEncodedText( modId, &(mHeader->subject));
    if ( mHeader->contentType)
    {
        freeMmsContentType( modId, mHeader->contentType);
        WE_MEM_FREE( modId, mHeader->contentType);
    } 
    if ( mHeader->replyChargingId)
    {
        WE_MEM_FREE( modId, mHeader->replyChargingId);
    } 
    freeMmsMessageClass( modId, &(mHeader->msgClass));
    
    memset( mHeader, 0, sizeof(MmsSetHeader));
} 







void freeMmsGetHeader( WE_UINT8 modId, MmsGetHeader *mHeader)
{
    if ( !mHeader)
    {
        return;
    } 
    freeMmsAddress( modId, &mHeader->from);
    if ( mHeader->to)
    {
        freeMmsAddressList( modId, mHeader->to);
        WE_MEM_FREE( modId, mHeader->to);
    } 
    if ( mHeader->cc)
    {
        freeMmsAddressList( modId, mHeader->cc);
        WE_MEM_FREE( modId, mHeader->cc);
    } 
    if ( mHeader->bcc)
    {
        freeMmsAddressList( modId, mHeader->bcc);
        WE_MEM_FREE( modId, mHeader->bcc);
    } 
    freeMmsEncodedText( modId, &(mHeader->subject));
    if ( mHeader->previouslySentBy)
    {
        freeMmsPrevSentBy( modId, mHeader->previouslySentBy);
        WE_MEM_FREE( modId, mHeader->previouslySentBy);
    } 
    if ( mHeader->previouslySentDate)
    {
        freeMmsPrevSentDate( modId, mHeader->previouslySentDate);
        WE_MEM_FREE( modId, mHeader->previouslySentDate);
    } 
    if ( mHeader->serverMessageId)
    {
        WE_MEM_FREE( modId, mHeader->serverMessageId);
    } 
    if ( mHeader->replyChargingId)
    {
        WE_MEM_FREE( modId, mHeader->replyChargingId);
    } 
    freeMmsMessageClass( modId, &(mHeader->msgClass));
    freeMmsContentType( modId, mHeader->contentType);
    WE_MEM_FREE( modId, mHeader->contentType);
    memset( mHeader, 0, sizeof(MmsGetHeader));
} 








void freeMmsPrevSentBy( WE_UINT8 modId, MmsPrevSentBy *mmsPrevSentBy)
{
    MmsPrevSentBy *p = NULL;
    MmsPrevSentBy *tmp = mmsPrevSentBy;
    WE_BOOL doFree = FALSE;

    if (tmp != NULL)
    {
        p = tmp->next;
        while (tmp)
        {
            freeMmsAddress( modId, &tmp->sentBy);
            if ( !doFree)
            {
                
                doFree = TRUE;
            }
            else
            {
                WE_MEM_FREE( modId, tmp);
            } 
            tmp = p;
            if (p != NULL)
            {
                p = p->next;
            } 
        } 
    } 
} 






void freeMmsPrevSentDate( WE_UINT8 modId, MmsPrevSentDate *mmsPrevSentDate)
{
    MmsPrevSentDate *p = NULL;
    MmsPrevSentDate *tmp = mmsPrevSentDate;
    WE_BOOL doFree = FALSE;

    if (tmp != NULL)
    {
        p = tmp->next;
        while (tmp)
        {
            if ( !doFree)
            {
                doFree = TRUE;
            }
            else
            {
                WE_MEM_FREE( modId, tmp);
            } 
            
            tmp = p;
            if (p != NULL)
            {
                p = p->next;
            } ;
        } 
    } 
} 






void freeMmsForward( WE_UINT8 modId, MmsForward *mmsForward)
{
    if ( !mmsForward)
    {
        return;
    } 
    freeMmsAddress( modId, &mmsForward->from);
    if ( mmsForward->to)
    {
        freeMmsAddressList( modId, mmsForward->to);
        WE_MEM_FREE( modId, mmsForward->to);
    } 
    if ( mmsForward->cc)
    {
        freeMmsAddressList( modId, mmsForward->cc);
        WE_MEM_FREE( modId, mmsForward->cc);
    } 
    
    if ( mmsForward->bcc)
    {
        freeMmsAddressList( modId, mmsForward->bcc);
        WE_MEM_FREE( modId, mmsForward->bcc);
    } 
    memset( mmsForward, 0x00, sizeof(MmsForward));
} 






void freeMmsReadOrigInd( WE_UINT8 modId, MmsReadOrigInd *mmsOrigInd)
{
    if (mmsOrigInd != NULL)
    {
        freeMmsAddress( modId, &mmsOrigInd->from);
        if ( mmsOrigInd->serverMessageId)
        {
            WE_MEM_FREE( modId, mmsOrigInd->serverMessageId);
            mmsOrigInd->serverMessageId = NULL;
        } 
    } 
} 






void freeMmsNotification( WE_UINT8 modId, MmsNotification *mmsNotification)
{
    if (mmsNotification != NULL)
    {
        freeMmsAddress( modId, &mmsNotification->from);
        freeMmsEncodedText( modId, &mmsNotification->subject);
        freeMmsMessageClass( modId, &mmsNotification->msgClass);
        
        if (mmsNotification->replyChargingId)
        {
            WE_MEM_FREE( modId, mmsNotification->replyChargingId);
            mmsNotification->replyChargingId = NULL;
        } 

        if (mmsNotification->contentLocation)
        {
            WE_MEM_FREE( modId, mmsNotification->contentLocation);
            mmsNotification->contentLocation = NULL;
        } 
        if (mmsNotification->transactionId)
        {
            WE_MEM_FREE( modId, mmsNotification->transactionId);
            mmsNotification->transactionId = NULL;
        } 
        
    } 
} 






void freeMmsEntryHeader( WE_UINT8 modId, MmsEntryHeader *entryHeader)
{
    MmsEntryHeader *tempPointer = NULL;
    WE_BOOL doDelete = FALSE; 

    while (entryHeader != NULL)
    {
        tempPointer = entryHeader->next;
        entryHeader->next=NULL;
        if (entryHeader->headerType == MMS_WELL_KNOWN_CONTENT_LOCATION ||
            entryHeader->headerType == MMS_WELL_KNOWN_CONTENT_ID)
        {
            WE_MEM_FREE( modId, entryHeader->value.wellKnownFieldName);
            entryHeader->value.wellKnownFieldName = NULL;
        } 
        else if (entryHeader->headerType == MMS_APPLICATION_HEADER)
        {
            WE_MEM_FREE( modId, entryHeader->value.applicationHeader.value);
            entryHeader->value.applicationHeader.value = NULL;
        } 
        if ( doDelete)
        {
            WE_MEM_FREE( modId, entryHeader);
        }
        else
        {
            doDelete = TRUE;
        } 
        
        entryHeader = tempPointer;

    } 
} 







void freeMmsMessageInfo( WE_UINT8 modId, MmsMessageInfo *messageInfo)
{
    MmsBodyInfoList     *ctList;
    
    while (messageInfo->list != NULL)
    {
        ctList = messageInfo->list->next;
        if ( messageInfo->list->contentType)
        {
            freeMmsContentType( modId, messageInfo->list->contentType);
            WE_MEM_FREE( modId, messageInfo->list->contentType);
        } 
        if ( messageInfo->list->entryHeader)
        {
            freeMmsEntryHeader( modId, messageInfo->list->entryHeader);
            WE_MEM_FREE( modId, messageInfo->list);
        } 
        messageInfo->list = ctList;
    }
    memset( messageInfo, 0x00, sizeof( MmsMessageInfo));

} 


void freeMmsDeliveryInd( WE_UINT8 modId, MmsDeliveryInd *deliveryInd)
{
    if ( deliveryInd == NULL)
    {
        
        return;
    } 

    if ( deliveryInd->messageId != NULL)
    {
        WE_MEM_FREE( modId, deliveryInd->messageId);
        deliveryInd->messageId = NULL;
    } 
    freeMmsAddress( modId, deliveryInd->to);
    deliveryInd->to = NULL;
} 



 







void mmsConvertDateToString( WE_UINT32 t, char *s)
{
    we_cmmn_time2str (t, s);
} 

 





WE_UINT32 mmsGetGMTtime(WE_BOOL ensureGMT)
{
    WE_UINT32 time;
    WE_INT16 zone;
    
    


    time = WE_TIME_GET_CURRENT();
    
    zone = WE_TIME_GET_TIME_ZONE();
    if (zone != WE_TIME_ZONE_UNKNOWN)
    {
       


        time -= zone*60;
        
        return time;
    }
    
    if (ensureGMT)
    {
       


        return 0;
    }
    else
    {
       


        return time;
    }   
} 






WE_BOOL mmsIsMultiPart( MmsKnownMediaType value)
{
    WE_BOOL ret;

    
    switch ( value)
    {
    case MMS_VND_WAP_MULTIPART_ANY:
    case MMS_VND_WAP_MULTIPART_MIXED:
    case MMS_VND_WAP_MULTIPART_ALTERNATIVE:
    case MMS_VND_WAP_MULTIPART_RELATED:
        
        
        ret = TRUE;
        break;
    default :
        
        ret = FALSE;
        break;
    } 

    return ret;
} 







WE_BOOL isText( MmsKnownMediaType value)
{
    WE_BOOL ret;

    
    switch( value)
    {
    case MMS_TYPE_TEXT_ANY:
    case MMS_TYPE_TEXT_HTML:
    case MMS_TYPE_TEXT_PLAIN:
    case MMS_TYPE_TEXT_X_VCALENDAR:
    case MMS_TYPE_TEXT_X_VCARD:
        
        ret = TRUE;
        break;
    default :
        
        ret = FALSE;
        break;
    } 

    return ret;
} 
 












unsigned char *mmsCvtBuf( WE_UINT8 modId, int *charset, 
    const unsigned char *srcData, int *srcSize, int *dstSize)
{
    char *dstData = NULL;
    long length = 0;
    long totalSize = 0;
    int lengthTmp = 0;
    we_charset_convert_t *cvtFunction = NULL;

    if (charset == NULL || srcData == NULL || srcSize == NULL || 
        dstSize == NULL)
    {
        return NULL;
    } 
    
    length = *srcSize;
    lengthTmp = length;
    totalSize = MMS_MAX_CHUNK_SIZE;

    cvtFunction = we_charset_find_function_bom(*charset, MMS_UTF8, (const char *)srcData, &lengthTmp);
    if (cvtFunction != NULL)
    {
        



        (void)cvtFunction((const char *)srcData, &length, NULL, &totalSize);
        *charset = MMS_UTF8;
        

        totalSize += 1; 
        if ((WE_UINT32)totalSize < MMS_MAX_CHUNK_SIZE)
        {
            dstData = WE_MEM_ALLOC( modId, (unsigned int)totalSize); 
            if (dstData == NULL)
            {
                
                return NULL;
            }
            memset( dstData, 0, (size_t) totalSize);
            (void)cvtFunction((const char *)srcData, &length, dstData, &totalSize);
        }
        else
        {
            totalSize = MMS_MAX_CHUNK_SIZE;
            dstData = WE_MEM_ALLOC( modId, (unsigned int)totalSize); 
            if (dstData == NULL)
            {
                
                return NULL;
            }
            memset( dstData, 0, (size_t) totalSize);
            (void)cvtFunction((const char *)srcData, &length, dstData, &totalSize);            
        } 
        
        *srcSize = length;
        *dstSize = totalSize;
    }
    else
    {
        dstData = WE_MEM_ALLOC( modId, (size_t) *srcSize); 
        if (dstData == NULL)
        {
            
            return NULL;
        }
        memset( dstData, 0, (size_t) *srcSize);
        memcpy( dstData, srcData, (unsigned int)*srcSize);
        *dstSize = *srcSize;            
    } 

    return (unsigned char *)dstData;
} 










WE_BOOL mmsValidateCharSet(int charSet)
{
    WE_BOOL retValue;

    switch ( charSet)
    {
    case MMS_US_ASCII:        
    case MMS_UCS2:               
    case MMS_UTF16BE:          
    case MMS_UTF16LE:          
    case MMS_UTF16:           
    case MMS_ISO_8859_1:      
        retValue = TRUE;
        break;
    case MMS_UTF8:            
    default:
        retValue = FALSE;
        break;
    }  

    return retValue;
} 








WE_BOOL mmsValidateAddressList( const MmsAddressList *addrList)
{
    MmsAddressList *tmpPtr;
    WE_UINT32 tempSize;
    tmpPtr = (MmsAddressList *)addrList;
   
    while (tmpPtr != NULL) 
    {
        tempSize = 0;
        if ( tmpPtr->current.name.text != NULL)
        {
            tempSize = strlen( tmpPtr->current.name.text);
        } 

        if ( tmpPtr->current.address != NULL)
        {
            tempSize += strlen( tmpPtr->current.address);
        
        } 

        if ( tempSize > MAX_ADDRESS_LEN)
        {
            return FALSE;
        } 

        tmpPtr = tmpPtr->next;
    } 

    return TRUE;
} 







WE_BOOL mmsValidateAddress( const MmsAddress *address)
{
    WE_UINT32 tempSize = 0;

    if ( address != NULL) 
    {
        if ( address->name.text != NULL)
        {
            tempSize = strlen( address->name.text);
        } 

        if ( address->address != NULL)
        {
            tempSize += strlen( address->address);
        
        } 

        if ( tempSize > MAX_ADDRESS_LEN)
        {
            return FALSE;
        } 
    } 

    return TRUE;
} 







WE_BOOL mmsValidateSubject( const MmsEncodedText *subject)
{
    if ( subject->text != NULL)
    {
        if ( strlen( subject->text) > MAX_SUBJECT_LEN)
        {
            return FALSE;        
        }
    } 

    return TRUE;
} 







WE_BOOL mmsValidateMessageClass( const MmsMessageClass *msgClass)
{
    if( msgClass->classIdentifier == MMS_MESSAGE_CLASS_IS_TEXT)
    {
        if ( msgClass->textString != NULL )
        {
            if( strlen( msgClass->textString) > MAX_STR_LEN_MSG_CLASS)
            {
                return FALSE;
            } 
        } 
    } 

    return TRUE;
} 







WE_BOOL mmsValidateEntryHeaders( WE_UINT8 modId, MmsEntryHeader *entryHeader, MmsResult *result)
{
    MmsEntryHeader *entry = entryHeader;
    WE_UINT32 size = 0;

    while ( entry != NULL)
    {
        size = 0;
        switch ( entry->headerType)
        {
        case MMS_WELL_KNOWN_CONTENT_ID:
            
            
            
            size += 1 + strlen( 
                (const char *)entry->value.wellKnownFieldName)
                + 2  
                + 1; 

            if ( size > MAX_MMS_ENTRY_HEADER_VALUE_LEN)
            {
                
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "validateEntryHeaders: Content wid field to long\n"));
                *result = MMS_RESULT_INVALID_CONTENT_ID;
                return FALSE;
            } 
            break;
        case MMS_WELL_KNOWN_CONTENT_LOCATION:
            
            
            
            size += 1 + strlen((const char *)
                entry->value.wellKnownFieldName) + 1;
            
            if ( size > MAX_MMS_ENTRY_HEADER_VALUE_LEN)
            {
                
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "validateEntryHeaders: Content location field to long\n"));
                *result = MMS_RESULT_INVALID_CONTENT_LOCATION;
                return FALSE;
            } 
            break;
        case MMS_APPLICATION_HEADER:
            
            
            
            size += 2 + strlen( (const char *)
                entry->value.applicationHeader.value) + 1;

            if ( size > MAX_MMS_ENTRY_HEADER_VALUE_LEN)
            {
                
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, modId, 
                    "validateEntryHeaders: Application header to long\n"));
                *result = MMS_RESULT_INVALID_APPLICATION_HEADER;
                return FALSE;
            } 
            break;
        case MMS_SHORT_CUT_SHIFT_DELIMITER:
        case MMS_SHIFT_DELIMITER:
        case MMS_WELL_KNOWN_CONTENT_BASE:
        case MMS_WELL_KNOWN_CONTENT_ENCODING:
        case MMS_WELL_KNOWN_CONTENT_LANGUAGE:
        case MMS_WELL_KNOWN_CONTENT_LENGTH:
        case MMS_WELL_KNOWN_CONTENT_MD5:
        case MMS_WELL_KNOWN_CONTENT_RANGE:
        case MMS_WELL_KNOWN_CONTENT_TYPE:
        case MMS_WELL_KNOWN_CONTENT_DISPOSITION:
        default :
            break;
        } 

        entry = entry->next;
    } 
    
    return TRUE;
} 







MmsForward *copyMmsForward( WE_UINT8 modId, const MmsForward *mmsForward)
{
    MmsForward *retForward = NULL;
    WE_BOOL copyIsOk = TRUE;

    if (mmsForward != NULL)
    {
        retForward = WE_MEM_ALLOC( modId, sizeof(MmsForward));
        if ( retForward == NULL)
        {
            
            return NULL;
        } 
        memset( retForward, 0, sizeof(MmsForward));

        copyIsOk = copyMmsAddressList( modId, &retForward->bcc, mmsForward->bcc);
        if (copyIsOk == TRUE)
        {
            copyIsOk = copyMmsAddressList( modId, &retForward->cc, mmsForward->cc);
        } 
        if (copyIsOk == TRUE)
        {
            copyIsOk = copyMmsAddressList( modId, &retForward->to, mmsForward->to);
        } 
        if (copyIsOk == TRUE)
        {
            retForward->date = mmsForward->date;
            retForward->deliveryReport = mmsForward->deliveryReport;
            retForward->deliveryTime = mmsForward->deliveryTime;
            retForward->expiryTime = mmsForward->expiryTime;
            retForward->readReply = mmsForward->readReply;
        } 
        else
        {
            freeMmsForward( modId, retForward);
            WE_MEM_FREE( modId, retForward);
            retForward = NULL;          
        } 
    } 

    return retForward;
} 








MmsBodyInfoList *copyMmsBodyInfoList( WE_UINT8 modId, MmsBodyInfoList *list)
{
    MmsBodyInfoList *result = NULL;
    MmsBodyInfoList *tmpPtr = NULL;
    MmsBodyInfoList *currentPtr = NULL;
    
    
    while( list != NULL)
    {
        


        tmpPtr = WE_MEM_ALLOC(modId, sizeof(MmsBodyInfoList));
        if (NULL == result)
        {
            result = tmpPtr;
        }
        if (NULL == tmpPtr)
        {
            
            freeMmsBodyInfoList(modId, result);
            return NULL;
        }
        memset(tmpPtr, 0, sizeof(MmsBodyInfoList));

        
        if (NULL != list->contentType)
        {
            tmpPtr->contentType = WE_MEM_ALLOC(modId, sizeof(MmsContentType));
            if (NULL == tmpPtr->contentType)
            {
                
                freeMmsBodyInfoList(modId, result);
                return NULL;            
            }
            copyMmsContentType(modId, tmpPtr->contentType, list->contentType);
        }
        
        
        if ( NULL != list->entryHeader)
        {
            tmpPtr->entryHeader = WE_MEM_ALLOC(modId, sizeof(MmsContentType));
            if (NULL == tmpPtr->entryHeader)
            {
                
                freeMmsBodyInfoList(modId, result);
                return NULL;            
            }
            copyMmsEntryHeader(modId, &tmpPtr->entryHeader, list->entryHeader);
        } 
        
        
        tmpPtr->number = list->number;
        tmpPtr->numOfEntries = list->numOfEntries;
        tmpPtr->size = list->size;
        tmpPtr->startPos = tmpPtr->startPos;

        if (NULL != currentPtr)
        {
            currentPtr->next = tmpPtr;
        }
        currentPtr = tmpPtr;

        list = list->next;
    } 

    return result;
} 












unsigned long mmsStrnlen(const char *str, unsigned long length)
{
    unsigned long i = 0;

    for ( i = 0; i < length && *str++; ++i)
    {
        
    } 

    return i;
} 








char *mmsStrtok(char *string, const char *control)
{
    unsigned char       *str;
    const unsigned char *ctrl = (unsigned char*)control;
    unsigned char        map[32];
    int                  count;
    
    static char         *nextoken;
    
    
    for (count = 0; count < 32; count++)
    {
        map[count] = 0;
    } 
    
    
    do 
    {
        map[*ctrl >> 3] |= (1 << (*ctrl & 7));
    } while (*ctrl++);
    
    


    if (string)
    {
        str = (unsigned char*)string;
    }
    else
    {
        str = (unsigned char*)nextoken; 
    } 
    
    


    while ( (map[*str >> 3] & (1 << (*str & 7))) && *str )
    {
        str++;
    } 
    
    string = (char*)str;
    
    

    for ( ; *str ; str++ )
    {
        if ( map[*str >> 3] & (1 << (*str & 7)) ) 
        {
            *str++ = '\0';
            break;
        } 
    } 
    
    

    nextoken = (char*)str;
    
    
    if ( string == (char*)str )
    {
        return NULL;
    }
    else
    {
        return string;
    } 
} 
