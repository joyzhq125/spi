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
#include "We_File.h"    
#include "We_Pipe.h"   
#include "We_Cmmn.h"    

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Mms_If.h"     

#include "Msig.h"       
#include "Mtimer.h"     
#include "Mmem.h"       
#include "Masync.h"     
#include "Fldmgr.h"     
#include "Msgread.h"    

#include "Mltypes.h"    
#include "Mlfetch.h"    
#include "Mcpdu.h"      
#include "Mlfieldp.h"   

#ifdef WE_MODID_DRS
#include "Drs_If.h"     
#endif



typedef enum
{
    MMS_MR_READING_INFO_FILE,
    MMS_MR_READING_HEADER,
    MMS_MR_READING_ENTRY_DCF,
    MMS_MR_READING_ENTRY_HEADER,
    MMS_MR_READING_BODY_PART,
    MMS_MR_TRANSFERING_BODY_PART,
    MMS_MR_IDLE
} MmsMRoperationState;




typedef enum
{
    MMS_MR_OP_GET_HEADER,
    MMS_MR_OP_GET_SKELETON,
    MMS_MR_OP_GET_BODY_PART
} MmsMRoperation;




typedef struct MmsSkeletonInfoStruct
{
    WE_UINT32      startPos;        
    WE_UINT32      size;            
    WE_UINT32      numOfEntries;    

    unsigned char   *entryHeader;    
    WE_UINT32      entryHeaderSize; 

    MmsDrmInfo      *drmInfo;        
     
    
    struct MmsSkeletonInfoStruct *next;    
} MmsSkeletonInfoList;




typedef struct MmsMRinstStruct
{
    MmFSM                   *factory;       
    WE_INT16               instanceId;     
    
    
    WE_UINT8               orderer;        
    WE_UINT32              userData;       
    MmsMsgId                msgId;          
    WE_UINT32              bpId;           

    
    MmsMRoperation          opType;         
    MmsMRoperationState     opState;        
    MmsResult               result;         

    
    AsyncOperationHandle    async;          
    unsigned char           *tmpBuffer;     
    WE_UINT32              tmpBufferSize;  
    
    
    unsigned char           *wspHeader;
    WE_UINT32              wspSize;
    MmsSkeletonInfoList     *msgInfoList;   
    MmsSkeletonInfoList     *tmpInfoList;   
    WE_UINT32              currentEntry;   
    WE_UINT32              entriesLeft;    
    WE_INT32               drsFileHandle;  

    struct MmsMRinstStruct  *next;          
} MmsMRinst;

MmsMRinst   *mmsMRfirstInst = NULL;
MmFSM       mmsMRFSM;



typedef enum
{
    





    MMS_SIG_MR_INFO_READ_REPLY,

    





    MMS_SIG_MR_HEADER_READ_REPLY,

    





    MMS_SIG_MR_ENTRY_DCF_PROP,

    





    MMS_SIG_MR_ENTRY_DCF_READ,

    





    MMS_SIG_MR_ENTRY_READ_REPLY,

    





    MMS_SIG_MR_BODY_PART_READ_REPLY,

    





    MMS_SIG_MR_BODY_PART_TRANSFER_REPLY
} MmsMRsignalId;

#ifdef WE_LOG_MODULE



const char *mmsMRgetSigName(WE_UINT32 sigType)
{
    switch (sigType)
    {
    case MMS_SIG_MR_INFO_READ_REPLY:
        return "MMS_SIG_MR_INFO_READ_REPLY";
    case MMS_SIG_MR_HEADER_READ_REPLY:
        return "MMS_SIG_MR_HEADER_READ_REPLY";
    case MMS_SIG_MR_ENTRY_READ_REPLY:
        return "MMS_SIG_MR_ENTRY_READ_REPLY";
    case MMS_SIG_MR_BODY_PART_READ_REPLY:
        return "MMS_SIG_MR_BODY_PART_READ_REPLY";
    default:
        return 0;
    }

} 
#endif












int mms_cvt_IntGetMsgHeaderReply(we_dcvt_t *obj, MmsMRinst *inst)
{
    WE_UINT32 result;
    WE_UINT32 msgId;

    if (obj->operation != WE_DCVT_ENCODE &&
        obj->operation != WE_DCVT_ENCODE_SIZE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, inst->factory->modId, 
            "%s(%d): Conversion failed due to wrong operation.\n", __FILE__, __LINE__));
        return FALSE;
    }

    msgId = (WE_UINT32) inst->msgId;
    result = (WE_UINT32) inst->result;

    if (
        !we_dcvt_uint32(obj, &(result)) ||
        !we_dcvt_uint32(obj, &(msgId)) ||
        !we_dcvt_uint32(obj, &(inst->userData))
        )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, inst->factory->modId, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } 

    


    if (inst->result == MMS_RESULT_OK)
    {
        if (!we_dcvt_uint32(obj, &(inst->wspSize)) ||
            !we_dcvt_uchar_vector(obj, (WE_INT32)inst->wspSize, &(inst->wspHeader))
                )
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, inst->factory->modId, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
            return FALSE;
        }
    }

    return TRUE; 
} 












int mms_cvt_IntGetSkeletonReply(we_dcvt_t *obj, MmsMRinst *inst)
{
    WE_UINT32 result;
    WE_UINT32 msgId;
    MmsSkeletonInfoList *entry;
    WE_UINT32 zero = 0;

    if (obj->operation != WE_DCVT_ENCODE &&
        obj->operation != WE_DCVT_ENCODE_SIZE)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, inst->factory->modId, 
            "%s(%d): Conversion failed due to wrong operation.\n", __FILE__, __LINE__));
        return FALSE;
    }

    msgId = (WE_UINT32) inst->msgId;
    result = (WE_UINT32) inst->result;

    if (
        !we_dcvt_uint32(obj, &(result)) ||
        !we_dcvt_uint32(obj, &(msgId)) ||
        !we_dcvt_uint32(obj, &(inst->userData))
        )
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, inst->factory->modId, 
            "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
        return FALSE;
    } 

    


    if (inst->result == MMS_RESULT_OK)
    {
        entry = inst->msgInfoList;
        while(entry)
        {
            if (!we_dcvt_uint32(obj, &(entry->entryHeaderSize)) ||
                !we_dcvt_uint32(obj, &(entry->size)) ||
                !we_dcvt_uint32(obj, &(entry->startPos)) ||
                !we_dcvt_uint32(obj, &(entry->numOfEntries)) ||
                !we_dcvt_uchar_vector(obj, (WE_INT32)entry->entryHeaderSize, &(entry->entryHeader)) ||
                !mms_cvt_MmsDrmInfo(obj, &entry->drmInfo)
                )
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, inst->factory->modId, 
                    "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
                return FALSE;
            }
            entry = entry->next;
        }

        


        if (!we_dcvt_uint32(obj, &zero))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, inst->factory->modId, 
                "%s(%d): Conversion failed.\n", __FILE__, __LINE__));
            return FALSE;
        }
    }

    return TRUE; 
} 




static MmsMRinst *mmsMRcreateInstance(MmFSM *factory)
{
    MmsMRinst *inst;

    inst = WE_MEM_ALLOC(factory->modId, sizeof(MmsMRinst));

    


    memset(inst, 0, sizeof(MmsMRinst));
    inst->factory = factory;
    inst->instanceId = factory->instId++;
    


    inst->next = factory->first;
    factory->first = inst;

    return inst;
} 




#ifdef WE_MODID_DRS
static MmsMRinst *mmsMRfindInstance(const MmFSM *factory, WE_INT16 instanceId)
{
    MmsMRinst *inst;

    inst = factory->first;
    while (NULL != inst)
    {
        if (inst->instanceId == instanceId)
        {
            return inst;
        }
    }

    return NULL;
} 
#endif



void mmsMRcleanUp(MmsMRinst *inst)
{
    MmFSM *factory = inst->factory;
    MmsMRinst *listp = factory->first;
    MmsMRinst *prevp = NULL;


    


    if (NULL != inst->async)
    {
        asyncOperationStop(&inst->async);
    }

    


    if (NULL != inst->tmpBuffer)
    {
        WE_MEM_FREE(inst->factory->modId, inst->tmpBuffer);
        inst->tmpBuffer = NULL;
    }

    


    if (NULL != inst->wspHeader)
    {
        WE_MEM_FREE(inst->factory->modId, inst->wspHeader);
        inst->wspHeader = NULL;
    }

    if (NULL != inst->msgInfoList)
    {
        


        while (inst->msgInfoList)
        {
            inst->tmpInfoList = inst->msgInfoList->next;
            if (NULL != inst->msgInfoList->entryHeader)
            {
                WE_MEM_FREE(inst->factory->modId, inst->msgInfoList->entryHeader);
                inst->msgInfoList->entryHeader = NULL;
            }
            WE_MEM_FREE(inst->factory->modId, inst->msgInfoList);
            inst->msgInfoList = inst->tmpInfoList;

        }
        inst->tmpInfoList = NULL;
    }

    


    while (NULL != listp)
    {
        if (listp == inst)
        {
            if (prevp == NULL)
            {
                
                factory->first = inst->next;
            }
            else
            {
                prevp->next = inst->next;
            }
            break;
        }
        prevp = listp;
        listp = listp->next;
    }
    inst->next = NULL;

    WE_MEM_FREE(inst->factory->modId, inst);

} 




void mmsMREmergencyAbort(void)
{
    MmFSM *factory = &mmsMRFSM;

    while (factory->first != NULL)
    {
        mmsMRcleanUp(factory->first);
    } 
} 




void mmsMRTerminate( MmsStateMachine fsm, WE_UINT32 signal, long instance)
{
    while (mmsMRfirstInst != NULL)
    {
        mmsMRcleanUp(mmsMRfirstInst);
    } 

    M_SIGNAL_SENDTO_I( fsm, signal, instance);
} 
#ifdef WE_MODID_DRS
MmsDrmInfo *mmsMRcreateDrmInfo(const MmsMRinst *inst, WE_UINT32 size, char *contentType)
{
    MmsDrmInfo *drmInfo;
    char       tmpStr[MMS_PATH_LEN];

    WE_UNUSED_PARAMETER(size);
    drmInfo = WE_MEM_ALLOC(inst->factory->modId, sizeof(MmsDrmInfo));
    if (drmInfo == NULL)
    {
        return NULL;
    }

    fldrDcf2Name(inst->msgId, inst->currentEntry, tmpStr, MMS_PATH_LEN);
    drmInfo->dcfFile = we_cmmn_strdup( inst->factory->modId, tmpStr);
    drmInfo->drmType = MmsForwardLock;
    drmInfo->drmContainerType.knownValue = MMS_VALUE_AS_STRING;
    drmInfo->drmContainerType.params = NULL; 
    drmInfo->drmContainerType.strValue = (unsigned char*) contentType;
    drmInfo->drmContainerType.drmInfo = NULL;

    

    return drmInfo;
} 
#endif

void mmsMRgetHeaderReply(MmsMRinst *inst, MmsResult result)
{
    if (inst->orderer > 0)
    {
        WE_LOG_SIG_BEGIN("MMS_SIG_GET_HEADER_REPLY", LS_SEND, inst->factory->modId, (WE_UINT8)WE_MODID_MMS);
        WE_LOG_SIG_UINT32("msgId", inst->msgId); 
        WE_LOG_SIG_UINT32("userData", inst->userData);
        WE_LOG_SIG_END();
                
        inst->result = result;

        if ( !mmsSendSignalExt(inst->factory->modId, inst->orderer,
            MMS_SIG_GET_MSG_HEADER_REPLY, 
            inst,
            (MmsIfConvertFunction*)mms_cvt_IntGetMsgHeaderReply))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): signal send failed.\n", __FILE__, __LINE__));
        } 
    }
    else
    {
        
    }

    


    mmsMRcleanUp(inst);
} 

void mmsMRgetSkeletonReply(MmsMRinst *inst, MmsResult result)
{
    if (inst->orderer > 0)
    {
        WE_LOG_SIG_BEGIN("MMS_SIG_GET_SKELETON_REPLY", LS_SEND, inst->factory->modId, (WE_UINT8)WE_MODID_MMS);
        WE_LOG_SIG_UINT32("msgId", inst->msgId); 
        WE_LOG_SIG_UINT32("userData", inst->userData);
        WE_LOG_SIG_END();

        inst->result = result;
                
        if ( !mmsSendSignalExt(inst->factory->modId, inst->orderer,
            MMS_SIG_GET_MSG_SKELETON_REPLY, 
            inst,
            (MmsIfConvertFunction*)mms_cvt_IntGetSkeletonReply))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): signal send failed.\n", __FILE__, __LINE__));
        } 
    }
    else
    {
        
    }

    


    mmsMRcleanUp(inst);
} 

static void mmsMRgetBodyPartReply(MmsMRinst *inst, MmsResult result, char *pipeName)
{
    if (inst->orderer > 0)
    {
        MmsGetBodyPartReply data; 

        WE_LOG_SIG_BEGIN("MMS_SIG_GET_BODY_PART_REPLY", LS_SEND, inst->factory->modId, (WE_UINT8)WE_MODID_MMS);
        WE_LOG_SIG_UINT32("msgId", inst->msgId); 
        WE_LOG_SIG_UINT32("userData", inst->userData);
        WE_LOG_SIG_UINT16("bodyPartId", inst->bpId);
        WE_LOG_SIG_END();
        
        data.result = result;
        data.msgId = inst->msgId;
        data.userData = inst->userData;
        data.bodyPartId = (WE_UINT16) inst->bpId;

        if (MMS_RESULT_OK == result)
        {
            if (pipeName == NULL)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_LOW, inst->factory->modId, 
                    "mmsMRgetBodyPartReply(). Error no pipeName"));
                mmsMRcleanUp(inst);
                return ;                
            }
            else
            {
                data.pipeName = (unsigned char*) pipeName;
                data.bodyPartSize = inst->tmpBufferSize;
            }
        }
        else
        {
            data.pipeName = NULL;
            data.bodyPartSize = 0;
        }
        
        if ( !mmsSendSignalExt(inst->factory->modId, inst->orderer,
            MMS_SIG_GET_BODY_PART_REPLY, 
            &data,
            (MmsIfConvertFunction*)mms_cvt_MmsGetBodyPartReply))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, WE_MODID_MMS, 
                "%s(%d): signal send failed.\n", __FILE__, __LINE__));
        } 

        if (pipeName != NULL)
        {
            WE_MEM_FREE(inst->factory->modId, pipeName);
        }
    }
    else
    {
        
    }

    




    if (MMS_RESULT_OK != result)
    {
        


        mmsMRcleanUp(inst);
    }
} 

void mmsMRoperationReply(MmsMRinst *inst, MmsResult result)
{
    switch (inst->opType)
    {
    case MMS_MR_OP_GET_HEADER:
        mmsMRgetHeaderReply(inst, result);
        break;
    case MMS_MR_OP_GET_SKELETON:
        mmsMRgetSkeletonReply(inst, result);
        break;
    case MMS_MR_OP_GET_BODY_PART:
        mmsMRgetBodyPartReply(inst, result, NULL);
        break;
    default:
        mmsMRcleanUp(inst);
        break;
    }
}





MmsResult mmsMRstartLoadMsgInfoFile(MmsMRinst *inst)
{
    int infoFileHandle = 0;
    long fileSize = 0;
    char fileName[MMS_PATH_LEN] = {0}; 

    
    if ((sprintf( fileName, "%s%lx.%s", MMS_FOLDER, inst->msgId, MMS_INFO_SUFFIX)) > (long)sizeof(fileName))
    {
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, inst->factory->modId, 
            "loadMessageInfoFile: file name is too long\n"));
        return MMS_RESULT_ERROR;
    } 
    
    
    if ( (infoFileHandle = WE_FILE_OPEN( inst->factory->modId, fileName, WE_FILE_SET_RDONLY, 0)) < 0 )
    {
        
        return MMS_RESULT_INFO_FILE_ERROR;
    } 
    
    
    if ((fileSize = WE_FILE_GETSIZE ( (const char *)fileName )) < 0)
    {
        
        
        (void) WE_FILE_CLOSE ( infoFileHandle);
        return MMS_RESULT_INFO_FILE_ERROR;
    } 
    
    (void) WE_FILE_CLOSE (infoFileHandle);

    
    inst->tmpBuffer = WE_MEM_ALLOC( inst->factory->modId, (WE_UINT32)fileSize);
    inst->tmpBufferSize = (WE_UINT32)fileSize;

    if( inst->tmpBuffer == NULL)
    {
        
        return MMS_RESULT_INSUFFICIENT_MEMORY;
    } 
    
    inst->async = asyncOperationStart(
            fileName, 
            M_FSM_MR, (long) inst, MMS_SIG_MR_INFO_READ_REPLY,
            ASYNC_MODE_READ);

    if ( inst->async == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d):ReadMsg FSM Failed to start asynch file operation, write mode\n",
            __FILE__, __LINE__));
        return MMS_RESULT_ERROR;
    }

    asyncRead( inst->async, 0, inst->tmpBuffer, inst->tmpBufferSize);

    inst->opState = MMS_MR_READING_INFO_FILE;
    
    return MMS_RESULT_OK;
} 







void mmsMRstartGetMsgHeader( MmsMRinst *inst, WE_UINT32 bytesRead)
{
    MmsMessage tmpHandle;
    WE_UINT32 entrySize;    
    char fileName[MMS_PATH_LEN] = {0}; 
    
    if (bytesRead != inst->tmpBufferSize)
    {
        mmsMRoperationReply(inst, MMS_RESULT_ERROR);        
    }

    


    tmpHandle.infoHandle.buffer = inst->tmpBuffer;
    tmpHandle.infoHandle.bufferSize = inst->tmpBufferSize;

    entrySize = getEntrySize( 0, &tmpHandle);

    if (sprintf( fileName, "%s%lx.%c", MMS_FOLDER, inst->msgId, fldrGetFileSuffix(inst->msgId)) > (int) (MMS_PATH_LEN))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, inst->factory->modId, 
            "mmsMRstartGetMsgHeader(). Error creating filename"));

        mmsMRoperationReply(inst, MMS_RESULT_ERROR);
        return ;
    }

    
    inst->wspHeader = WE_MEM_ALLOC( inst->factory->modId, (WE_UINT32)entrySize);
    inst->wspSize = entrySize;
    if( inst->tmpBuffer == NULL)
    {
        
        mmsMRoperationReply(inst, MMS_RESULT_RESTART_NEEDED);
        return ;
    } 

    inst->async = asyncOperationStart(
        fileName, 
        M_FSM_MR, (long) inst, MMS_SIG_MR_HEADER_READ_REPLY,
        ASYNC_MODE_READ);
    
    if ( inst->async == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d):ReadMsg FSM Failed to start asynch file operation, write mode\n",
            __FILE__, __LINE__));
        mmsMRoperationReply(inst, MMS_RESULT_ERROR);
        return ;
    }
    
    asyncRead( inst->async, 0, inst->wspHeader, inst->wspSize);

    inst->opState = MMS_MR_READING_HEADER;
} 








static void mmsMRstartReadEntry(MmsMRinst *inst)
{
    char                *fileName = NULL;
    MmsMessage          tmpHandle;
    MmsSkeletonInfoList *newEntry = inst->tmpInfoList;
    WE_UINT32          entryStart;
    
    inst->opState = MMS_MR_READING_ENTRY_HEADER;

    


    tmpHandle.infoHandle.buffer = inst->tmpBuffer;
    tmpHandle.infoHandle.bufferSize = inst->tmpBufferSize;

    


    entryStart = getEntryStart(inst->currentEntry, &tmpHandle);
    newEntry->entryHeaderSize = getEntrySize( inst->currentEntry, &tmpHandle);
    newEntry->entryHeader = WE_MEM_ALLOC(inst->factory->modId, newEntry->entryHeaderSize);
    if (NULL == newEntry->entryHeader)
    {
        mmsMRgetSkeletonReply(inst, MMS_RESULT_RESTART_NEEDED);        
        return ;
    }

    


    if (NULL == (fileName = Mms_FldrMsgIdToName(inst->msgId, fldrGetFileSuffix(inst->msgId))))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, inst->factory->modId, 
            "mmsMRstartGetMsgHeader(). Error creating filename"));

        mmsMRgetSkeletonReply(inst, MMS_RESULT_ERROR);
        return ;
    }

    inst->async = asyncOperationStart(
        fileName, 
        M_FSM_MR, (long) inst, MMS_SIG_MR_ENTRY_READ_REPLY,
        ASYNC_MODE_READ);
    
    if ( inst->async == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d):ReadMsg FSM Failed to start asynch file operation, write mode\n",
            __FILE__, __LINE__));
        mmsMRgetHeaderReply(inst, MMS_RESULT_ERROR);
        return ;
    }
    
    asyncRead( inst->async, (WE_INT32)entryStart, newEntry->entryHeader, newEntry->entryHeaderSize);
} 











static void mmsMRstartHandleNextEntry(MmsMRinst *inst)
{
    MmsMessage          tmpHandle;
    MmsSkeletonInfoList *newEntry = NULL;
    WE_UINT32          hasDRM = FALSE;
 #ifdef WE_MODID_DRS
    char                tmpStr[MMS_PATH_LEN];
#endif
    


    tmpHandle.infoHandle.buffer = inst->tmpBuffer;
    tmpHandle.infoHandle.bufferSize = inst->tmpBufferSize;

    


    newEntry = WE_MEM_ALLOC(inst->factory->modId, sizeof(MmsBodyInfoList));
    if (NULL == inst->msgInfoList)
    {
        mmsMRgetSkeletonReply(inst, MMS_RESULT_RESTART_NEEDED);        
        return ;
    }

    inst->currentEntry++;
    inst->tmpInfoList->next = newEntry;

    
    newEntry->entryHeader = NULL; 
    newEntry->size = getDataSize( inst->currentEntry, &tmpHandle);
    newEntry->startPos = getDataStart( inst->currentEntry, &tmpHandle);
    newEntry->numOfEntries = getNumberOfSubParts( inst->currentEntry, &tmpHandle);
    newEntry->next = NULL;
    newEntry->drmInfo = NULL;

    hasDRM = getDrmStatus( inst->currentEntry, &tmpHandle);

    


    inst->entriesLeft += newEntry->numOfEntries;
    inst->tmpInfoList = newEntry;

    if (hasDRM > 0)
    {
 #ifdef WE_MODID_DRS
        DRSif_getDcfProperties(inst->factory->modId,
            inst->instanceId,
            fldrDcf2Name(inst->msgId, inst->currentEntry, tmpStr, MMS_PATH_LEN));
        inst->opState = MMS_MR_READING_ENTRY_DCF;
#endif
    }
    else
    {
        mmsMRstartReadEntry(inst);
    }
} 











void mmsMRstartGetSkeleton( MmsMRinst *inst, WE_UINT32 bytesRead)
{
    MmsHeaderValue  headerValue;
    MmsMessage tmpHandle;
    WE_UINT32 size;
    
    if (bytesRead != inst->wspSize)
    {
        mmsMRgetSkeletonReply(inst, MMS_RESULT_ERROR);   
        return ;
    }

    


    tmpHandle.infoHandle.buffer = inst->tmpBuffer;
    tmpHandle.infoHandle.bufferSize = inst->tmpBufferSize;
    
    
    


    inst->msgInfoList = WE_MEM_ALLOC(inst->factory->modId, sizeof(MmsBodyInfoList));
    if (NULL == inst->msgInfoList)
    {
        mmsMRgetSkeletonReply(inst, MMS_RESULT_RESTART_NEEDED);        
        return ;
    }
    
    inst->currentEntry = 0;
    inst->msgInfoList->size = getDataSize( inst->currentEntry, &tmpHandle);
    inst->msgInfoList->startPos = getDataStart( inst->currentEntry, &tmpHandle);
    inst->msgInfoList->numOfEntries = getNumberOfSubParts( inst->currentEntry, &tmpHandle);
    inst->msgInfoList->next = NULL;
    inst->msgInfoList->drmInfo = NULL;

    


    if (mmsPduGet(inst->wspHeader, inst->wspSize,
                    MMS_CONTENT_TYPE, &headerValue))
    {
        size = (WE_UINT32)(inst->wspHeader - 
            (unsigned char *)headerValue.contentType) + inst->wspSize;

        inst->msgInfoList->entryHeader = WE_MEM_ALLOC(inst->factory->modId, size);
        if (NULL == inst->msgInfoList->entryHeader)
        {
            mmsMRgetSkeletonReply(inst, MMS_RESULT_RESTART_NEEDED);        
            return ;
        }
        inst->msgInfoList->entryHeaderSize = size;

        memcpy(inst->msgInfoList->entryHeader, headerValue.contentType, size);
    }
    
    


    WE_MEM_FREE(inst->factory->modId, inst->wspHeader);
    inst->wspHeader = NULL;
    inst->wspSize = 0;

    if ( inst->msgInfoList->numOfEntries == 0)
    {
        
        mmsMRgetSkeletonReply(inst, MMS_RESULT_OK);        
        return ;
    } 


    


    inst->tmpInfoList = inst->msgInfoList;
    inst->entriesLeft = inst->msgInfoList->numOfEntries;

    mmsMRstartHandleNextEntry(inst);
} 






void mmsMRstartTransferBodyPart(MmsMRinst *inst)
{
    MmsContentEncodingValue  encodingValue;
    MmsMessage  tmpHandle;
    char       *pipeName;
    
    pipeName = WE_MEM_ALLOC(inst->factory->modId, MMS_PATH_LEN);
    if (pipeName == NULL)
    {
        return ;
    }

    



    tmpHandle.infoHandle.buffer     = inst->tmpBuffer;
    tmpHandle.infoHandle.bufferSize = inst->tmpBufferSize;

    encodingValue = getEncodingType(inst->bpId, &tmpHandle);
        
    if (encodingValue == ENCODE_VALUE_BASE64) 
    {
        int            decodedLength;
        int            encodedLength;
        int            outLen;
        char          *encodedData; 
        char          *decodedData;

        encodedLength = (int)inst->tmpBufferSize;
        encodedData   = (char *)inst->tmpBuffer;

        decodedLength = 
            we_cmmn_base64_decode_len(encodedData, encodedLength);

        decodedData = 
            WE_MEM_ALLOC(inst->factory->modId, (WE_UINT32)decodedLength);

        if (decodedData) 
        {
            outLen = (int)decodedLength;
            if (we_cmmn_base64_decode(encodedData, encodedLength, decodedData, &outLen) == -1) 
            {
                WE_MEM_FREE(inst->factory->modId, decodedData);
            }
            else
            {
                if (outLen != decodedLength) 
                {
                     WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                         "%s(%d):Suspected Base64 decode error.\n",
                         __FILE__, __LINE__));
                }
                inst->tmpBuffer     = (unsigned char *)decodedData;
                inst->tmpBufferSize = (WE_UINT32)decodedLength;

                
                WE_MEM_FREE(inst->factory->modId, encodedData);
            }
            
        }                
    }

    if (sprintf( pipeName, "%s%lx", MMS_FOLDER, (long)inst) > (int) (MMS_PATH_LEN))
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_LOW, inst->factory->modId, 
            "mmsMRstartTransferBodyPart(). Error creating filename"));

        mmsMRgetBodyPartReply(inst, MMS_RESULT_ERROR, NULL);
        return ;
    }

    inst->async = asyncPipeOperationStart(
        pipeName, 
        M_FSM_MR, (long) inst, MMS_SIG_MR_BODY_PART_TRANSFER_REPLY,
        ASYNC_MODE_WRITE);
    
    if ( inst->async == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d):ReadMsg FSM Failed to start async file operation, write mode\n",
            __FILE__, __LINE__));
        mmsMRgetBodyPartReply(inst, MMS_RESULT_ERROR, NULL);
        WE_MEM_FREE(inst->factory->modId, pipeName);
        return ;
    }
    
    asyncPipeWrite( inst->async, inst->tmpBuffer, inst->tmpBufferSize);

    inst->opState = MMS_MR_TRANSFERING_BODY_PART;
    mmsMRgetBodyPartReply(inst, MMS_RESULT_OK, pipeName);    
} 







void mmsMRstartGetBodyPart( MmsMRinst *inst, WE_UINT32 bytesRead)
{
    MmsMessage tmpHandle;
    WE_UINT32 entrySize;
    WE_UINT32 entryPos;
    WE_UINT32 hasDRM;
    char fileName[MMS_PATH_LEN] = {0}; 
    
    if (bytesRead != inst->tmpBufferSize)
    {
        mmsMRgetHeaderReply(inst, MMS_RESULT_ERROR);        
    }

    


    tmpHandle.infoHandle.buffer = inst->tmpBuffer;
    tmpHandle.infoHandle.bufferSize = inst->tmpBufferSize;

    


    hasDRM = getDrmStatus(inst->bpId, &tmpHandle);
#ifdef WE_MODID_DRS
    if (hasDRM > 0)
    {
        DRSif_getDcfProperties(inst->factory->modId,
            inst->instanceId,
            fldrDcf2Name(inst->msgId, inst->bpId, fileName, MMS_PATH_LEN));
    }
    else
#endif
    {
        entrySize = getDataSize(inst->bpId, &tmpHandle);
        entryPos = getDataStart(inst->bpId, &tmpHandle);

        if (sprintf( fileName, "%s%lx.%c", MMS_FOLDER, inst->msgId, fldrGetFileSuffix(inst->msgId)) > (int) (MMS_PATH_LEN))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_LOW, inst->factory->modId, 
                "mmsMRstartGetBodyPart(). Error creating filename"));

            mmsMRgetBodyPartReply(inst, MMS_RESULT_ERROR, NULL);
            return ;
        }

        WE_MEM_FREE(inst->factory->modId, inst->tmpBuffer);
        inst->tmpBuffer = NULL;
        inst->tmpBufferSize = 0;

        
        inst->tmpBuffer = WE_MEM_ALLOC( inst->factory->modId, (WE_UINT32)entrySize);
        inst->tmpBufferSize = entrySize;
        if( inst->tmpBuffer == NULL)
        {
            
            mmsMRgetBodyPartReply(inst, MMS_RESULT_RESTART_NEEDED, NULL);
            return ;
        } 

        inst->async = asyncOperationStart(
            fileName, 
            M_FSM_MR, (long) inst, MMS_SIG_MR_BODY_PART_READ_REPLY,
            ASYNC_MODE_READ);


        if ( inst->async == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d):ReadMsg FSM Failed to start asynch file operation, write mode\n",
                __FILE__, __LINE__));
            mmsMRgetBodyPartReply(inst, MMS_RESULT_ERROR, NULL);
            return ;
        }
    
        asyncRead( inst->async, (WE_INT32)entryPos, inst->tmpBuffer, inst->tmpBufferSize);
    }

    inst->opState = MMS_MR_READING_BODY_PART;
} 




static void mmsMRasyncInfoReadReply(MmsMRinst *inst, MmsResult result, WE_UINT32 bytesRead)
{
    if (0 != inst->async)
    {
        asyncOperationStop(&inst->async);
    }
    if (MMS_MR_READING_INFO_FILE != inst->opState)
    {
        


        mmsMRcleanUp(inst);
        return ;
    }

    switch (inst->opType)
    {
    case MMS_MR_OP_GET_HEADER:
        
        if (result != MMS_RESULT_OK)
        {
            mmsMRgetHeaderReply(inst, result);
            break;
        }

        


        mmsMRstartGetMsgHeader(inst, bytesRead);
        break;

    case MMS_MR_OP_GET_SKELETON:
        
        if (result != MMS_RESULT_OK)
        {
            mmsMRgetSkeletonReply(inst, result);
            break;
        }

        


        mmsMRstartGetMsgHeader(inst, bytesRead);
        break;

    case MMS_MR_OP_GET_BODY_PART:
        
        if (result != MMS_RESULT_OK)
        {
            mmsMRgetBodyPartReply(inst, result, NULL);
            break;
        }

        


        mmsMRstartGetBodyPart(inst, bytesRead);
        break;

    default: 
        mmsMRcleanUp(inst);
        break;            
    }
} 




static void mmsMRasyncHeaderReadReply(MmsMRinst *inst, MmsResult result, WE_UINT32 bytesRead)
{
    if (MMS_MR_READING_HEADER != inst->opState)
    {
        


        mmsMRcleanUp(inst);
        return ;
    }

    if (NULL != inst->async)
    {
        asyncOperationStop(&inst->async);
    }

    switch (inst->opType)
    {
    case MMS_MR_OP_GET_HEADER:
        
        mmsMRoperationReply(inst, result);
        break;
        
    case MMS_MR_OP_GET_SKELETON:
        
        if (result != MMS_RESULT_OK)
        {
            mmsMRoperationReply(inst, result);
            break;
        }

        


        mmsMRstartGetSkeleton(inst,bytesRead);
        break;
        
    case MMS_MR_OP_GET_BODY_PART:
    default: 
        mmsMRcleanUp(inst);
        break;            
    }

} 




#ifdef WE_MODID_DRS
static void mmsMRdcfPropertiesReply(MmsMRinst *inst, WE_UINT32 result, WE_UINT32 size, char *contentType)
{
    char fileName[MMS_PATH_LEN] = {0}; 
    
    switch (inst->opType)
    {
    case MMS_MR_OP_GET_SKELETON:        
        if (result == DRS_DCF_OK)
        {
            inst->tmpInfoList->drmInfo = mmsMRcreateDrmInfo(inst, size, contentType);
        }
        mmsMRstartReadEntry(inst);
        break;
    case MMS_MR_OP_GET_BODY_PART:
        if (result == DRS_DCF_OK)
        {
            inst->tmpBufferSize = size;
            DRSif_openFile(inst->factory->modId, inst->instanceId, 
                fldrDcf2Name(inst->msgId, inst->bpId, fileName, MMS_PATH_LEN),
                0);
        }
        else
        {
            


            M_SIGNAL_SENDTO_IUP(M_FSM_MR, MMS_SIG_MR_ENTRY_DCF_READ,
                (WE_INT32) inst, result, NULL);

        }
        break;
        
    case MMS_MR_OP_GET_HEADER:
    default: 
        mmsMRcleanUp(inst);
        break;            
    }
} 




static void mmsMRdcfReadReply(MmsMRinst *inst, WE_INT32 result, char *pipeName)
{
    switch (inst->opType)
    {
    case MMS_MR_OP_GET_BODY_PART:        
        if (result >= 0) 
        {
            


            mmsMRgetBodyPartReply(inst, MMS_RESULT_OK, pipeName);
        }
        else
        {
            


            mmsMRgetBodyPartReply(inst, MMS_RESULT_ERROR, NULL);
            if (pipeName != NULL)
            {
                WE_MEM_FREE(inst->factory->modId, pipeName);
            }
        }
        break;
    case MMS_MR_OP_GET_HEADER:
    case MMS_MR_OP_GET_SKELETON:
    default: 
        mmsMRcleanUp(inst);
        break;            
    }
} 
#endif




static void mmsMRasyncEntryReadReply(MmsMRinst *inst, MmsResult result, WE_UINT32 bytesRead)
{
    MmsMessage tmpHandle;

    if (NULL != inst->async)
    {
        asyncOperationStop(&inst->async);
    }

    if (MMS_MR_READING_ENTRY_HEADER != inst->opState)
    {
        


        mmsMRcleanUp(inst);
        return ;
    }

    switch (inst->opType)
    {
    case MMS_MR_OP_GET_SKELETON:
        
        if (result != MMS_RESULT_OK)
        {
            mmsMRgetSkeletonReply(inst, result);
            break;
        }
        
        


        tmpHandle.infoHandle.buffer = inst->tmpBuffer;
        tmpHandle.infoHandle.bufferSize = inst->tmpBufferSize;

        if (bytesRead != getEntrySize(inst->currentEntry, &tmpHandle))
        {
            mmsMRgetSkeletonReply(inst, MMS_RESULT_ERROR);        
        }
        


        --inst->entriesLeft;
        if (inst->entriesLeft > 0)
        {
            mmsMRstartHandleNextEntry(inst);
        }
        else
        {
            mmsMRgetSkeletonReply(inst, MMS_RESULT_OK);
        }
        break;
    case MMS_MR_OP_GET_HEADER:
    case MMS_MR_OP_GET_BODY_PART:
    default: 
        mmsMRcleanUp(inst);
        break;            
    }
} 




static void mmsMRasyncBodyPartReadReply(MmsMRinst *inst, MmsResult result, WE_UINT32 bytesRead)
{
    
    if (NULL != inst->async)
    {
        asyncOperationStop(&inst->async);
    }

    if (MMS_MR_READING_BODY_PART != inst->opState)
    {
        


        mmsMRcleanUp(inst);
        return ;
    }

    switch (inst->opType)
    {
    case MMS_MR_OP_GET_BODY_PART:
        
        if (result != MMS_RESULT_OK)
        {
            mmsMRgetBodyPartReply(inst, result, NULL);
            break;
        }

        if (bytesRead != inst->tmpBufferSize)
        {
            mmsMRgetBodyPartReply(inst, MMS_RESULT_ERROR, NULL);        
        }

        


        mmsMRstartTransferBodyPart(inst);
        break;
    case MMS_MR_OP_GET_HEADER:
    case MMS_MR_OP_GET_SKELETON:
    default: 
        mmsMRcleanUp(inst);
        break;            
    }
} 



static void mmsMRsignalError(const MmsSignal *sig)
{
    if (NULL == sig)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "RM FSM, Received faulty signal data\n"));
    }
    else
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "RM FSM, Received faulty signal data %d %d\n", sig->i_param, sig->type));
    }
} 







static void mmsMRmain(MmsSignal *sig)
{
    MmsMRinst           *inst;
    WE_UINT32          bytes;
    MmsResult           result;
    
    inst = (MmsMRinst*) sig->i_param;
    

    switch (sig->type)
    {
    
    case MMS_SIG_MR_INFO_READ_REPLY:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "RM FSM, Received MMS_SIG_MR_INFO_READ_REPLY \n"));
         
        if (NULL == inst)
        {
            mmsMRsignalError(sig);
            break;
        }

        
        result = (MmsResult)sig->u_param1;

        
        bytes = sig->u_param2;
            
        mmsMRasyncInfoReadReply(inst, result, bytes);
        break;

        
    case MMS_SIG_MR_HEADER_READ_REPLY:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "RM FSM, Received MMS_SIG_MR_HEADER_READ_REPLY \n"));
        
        if (NULL == inst)
        {
            mmsMRsignalError(sig);
            break;
        }
        
        
        result = (MmsResult)sig->u_param1;
        
        
        bytes = sig->u_param2;
        
        mmsMRasyncHeaderReadReply(inst, result, bytes);
        break;

        
#ifdef WE_MODID_DRS
    case MMS_SIG_MR_ENTRY_DCF_PROP:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "RM FSM, Received MMS_SIG_MR_ENTRY_DCF_PROP \n"));
        
        if (NULL == inst)
        {
            mmsMRsignalError(sig);
            break;
        }
        
        
        mmsMRdcfPropertiesReply(inst, sig->u_param1, sig->u_param2, (char*) sig->p_param);        
        break;

        
    case MMS_SIG_MR_ENTRY_DCF_READ:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "RM FSM, Received MMS_SIG_MR_ENTRY_DCF_READ \n"));
        
        if (NULL == inst)
        {
            mmsMRsignalError(sig);
            break;
        }
        
        
        mmsMRdcfReadReply(inst, (WE_INT32)sig->u_param1, (char*) sig->p_param);        
        break;
#endif      
        
    case MMS_SIG_MR_ENTRY_READ_REPLY:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "RM FSM, Received MMS_SIG_MR_HEADER_READ_REPLY \n"));
        
        if (NULL == inst)
        {
            mmsMRsignalError(sig);
            break;
        }
        
        
        result = (MmsResult)sig->u_param1;
        
        
        bytes = sig->u_param2;
        
        mmsMRasyncEntryReadReply(inst, result, bytes);
        break;
    case MMS_SIG_MR_BODY_PART_READ_REPLY:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "RM FSM, Received MMS_SIG_MR_BODY_PART_READ_REPLY \n"));
        
        if (NULL == inst)
        {
            mmsMRsignalError(sig);
            break;
        }
        
        
        result = (MmsResult)sig->u_param1;
        
        
        bytes = sig->u_param2;
        
        mmsMRasyncBodyPartReadReply(inst, result, bytes);
        break;
    case MMS_SIG_MR_BODY_PART_TRANSFER_REPLY:
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "RM FSM, Received MMS_SIG_MR_BODY_PART_TRANSFER_REPLY \n"));
        
        if (NULL == inst)
        {
            mmsMRsignalError(sig);
            break;
        }
        
        
        result = (MmsResult)sig->u_param1;
        
        
        bytes = sig->u_param2;
        
        
        mmsMRcleanUp(inst);
        break;

    default :
         WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
             "MSR Notify FSM, Error: Received unsupported sig \n"));
         mmsMRsignalError(sig);
         break;
    } 

    mSignalDelete(sig);
} 














void mmsMRgetMsgHeader(MmFSM *factory, WE_UINT8 orderer, WE_UINT32 msgId, WE_UINT32 userData)
{
    MmsMRinst *inst;
    MmsResult result;
    
    inst = mmsMRcreateInstance(factory);
    inst->opType = MMS_MR_OP_GET_HEADER;
    inst->opState = MMS_MR_IDLE;
    inst->orderer = orderer;
    inst->userData = userData;
    inst->msgId = msgId;

    


    result = mmsMRstartLoadMsgInfoFile(inst);
    if (MMS_RESULT_OK != result)
    {
        


        mmsMRgetHeaderReply(inst, result);
    }
} 




void mmsMRreadGetMsgSkeleton(MmFSM *factory, WE_UINT8 orderer, WE_UINT32 msgId, WE_UINT32 userData)
{
    MmsMRinst *inst;
    MmsResult result;
    
    inst = mmsMRcreateInstance(factory);
    inst->opType = MMS_MR_OP_GET_SKELETON;
    inst->opState = MMS_MR_IDLE;
    inst->orderer = orderer;
    inst->userData = userData;
    inst->msgId = msgId;

    


    result = mmsMRstartLoadMsgInfoFile(inst);
    if (MMS_RESULT_OK != result)
    {
        


        mmsMRgetSkeletonReply(inst, result);
    }
} 




void mmsMRgetMsgBodyPart(MmFSM *factory, WE_UINT8 orderer, WE_UINT32 msgId, WE_UINT32 bpId, WE_UINT32 userData)
{
    MmsMRinst *inst;
    MmsResult result;
    
    inst = mmsMRcreateInstance(factory);
    inst->opType = MMS_MR_OP_GET_BODY_PART;
    inst->opState = MMS_MR_IDLE;
    inst->orderer = orderer;
    inst->userData = userData;
    inst->msgId = msgId;
    inst->bpId = bpId;

    


    result = mmsMRstartLoadMsgInfoFile(inst);
    if (MMS_RESULT_OK != result)
    {
        


        mmsMRgetBodyPartReply(inst, result, NULL);
    }
} 









#ifdef WE_MODID_DRS
WE_BOOL mmsMRhandleDcfProperties(const MmFSM *factory, void *sigData)
{
    drs_dcf_properties_t *reply = sigData;
    char *contentType;
    MmsMRinst *inst;
    
    inst = mmsMRfindInstance(factory, reply->opId);
    if (NULL == inst)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, factory->modId, 
            "MMS FSM RM (DCF properties): Error\n"));
        return FALSE;
    }
        
    if (reply->returnCode == DRS_DCF_OK)
    {
        contentType = we_cmmn_strdup( factory->modId, reply->drmProperties.contentType);
        M_SIGNAL_SENDTO_IUUP(M_FSM_MR, MMS_SIG_MR_ENTRY_DCF_PROP,
            (WE_INT32)inst, (WE_UINT32)reply->returnCode,
            (WE_UINT32)reply->drmProperties.size, contentType);
    }
    else
    {
        M_SIGNAL_SENDTO_IUUP(M_FSM_MR, MMS_SIG_MR_ENTRY_DCF_PROP,
            (WE_INT32) inst, (WE_UINT32)reply->returnCode, 0, NULL);
    }

    return TRUE;
} 
#endif








#ifdef WE_MODID_DRS
WE_BOOL mmsMRhandleOpenFileReply(const MmFSM *factory, void *sigData)
{
    drs_open_file_reply_t *reply = sigData;
    MmsMRinst *inst;
    WE_BOOL  isDcfAllowed = FALSE;
    
    inst = mmsMRfindInstance(factory, reply->opId);

    if (NULL == inst)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, factory->modId, 
            "MMS FSM RM (DCF reading, open): Error\n"));
        return FALSE;
    }
       
    if (reply->fileHandle  >= 0)
    {
        


        isDcfAllowed = (reply->constraints == NULL);

        if (isDcfAllowed)
        {
            


            inst->drsFileHandle = reply->fileHandle;
            DRSif_readFile(factory->modId, inst->instanceId, inst->drsFileHandle, 0);
        }
        else
        {
            


            M_SIGNAL_SENDTO_IUP(M_FSM_MR, MMS_SIG_MR_ENTRY_DCF_READ,
                (WE_INT32) inst, (WE_UINT32) DRS_FILE_NO_RIGHTS, NULL);
        }
    }
    else
    {
        


        M_SIGNAL_SENDTO_IUP(M_FSM_MR, MMS_SIG_MR_ENTRY_DCF_READ,
            (WE_INT32) inst, (WE_UINT32)reply->fileHandle, NULL);
    }

    return TRUE;
} 









WE_BOOL mmsMRhandleReadFileReply(const MmFSM *factory, void *sigData)
{
    drs_read_file_reply_t *reply = sigData;
    MmsMRinst *inst;
    char *pipeName;
    
    inst = mmsMRfindInstance(factory, reply->opId);

    if (NULL == inst)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, factory->modId, 
            "MMS FSM RM (DCF reading, read): Error\n"));
        return FALSE;
    }
    
    


    DRSif_closeFile(factory->modId, inst->instanceId, inst->drsFileHandle, 1);
    inst->drsFileHandle = 0;

    if (reply->returnCode == DRS_DCF_OK)
    {
        


        pipeName = we_cmmn_strdup( factory->modId, reply->pipe);
        M_SIGNAL_SENDTO_IUP(M_FSM_MR, MMS_SIG_MR_ENTRY_DCF_READ,
            (WE_INT32) inst, (WE_UINT32)reply->returnCode, pipeName);
    }
    else
    {
        


        M_SIGNAL_SENDTO_IUP(M_FSM_MR, MMS_SIG_MR_ENTRY_DCF_READ,
            (WE_INT32) inst, (WE_UINT32)reply->returnCode, NULL);
    }

    return TRUE;
} 
#endif












void mmsMRInit(void)
{
    mmsMRFSM.first = NULL;
    mmsMRFSM.modId = WE_MODID_MMS;
    mmsMRFSM.instId = 1;
    
    mSignalRegisterDst(M_FSM_MR, mmsMRmain);
    
    mmsMRfirstInst = NULL;
    
    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM RM (read message): initialized\n"));
    
} 

