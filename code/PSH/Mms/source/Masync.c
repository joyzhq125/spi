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
#include "We_Pipe.h"    

#include "Mms_Cfg.h"    
#include "Mms_Def.h"    
#include "Msig.h"       
#include "Mtimer.h"     
#include "Mmem.h"       
#include "Masync.h"     


#define ASYNC_POOL_SIZE 6






typedef enum
{
    OPER_STATE_FREE = 0,    
    OPER_STATE_OPENED       
} E_OPERSTATE;



typedef struct ST_ASYNCOPER
{   
    struct ST_ASYNCOPER *pstNext; 
    struct ST_ASYNCOPER *pstPrevious; 
    E_OPERSTATE eState;        

    MmsStateMachine eFsm;    
    long lInstance;          
    WE_UINT32 uiSignal;      

    int iFileHandle;         
    WE_UINT32 uiTotalLen;    
    unsigned char *pucPos;     
    unsigned char *pucBuffer;  
    WE_UINT32 uiDataLen;     


    AsyncMode eMode;         
    char *pcPipeName;         
    int iPipeHandle;         
} ST_ASYNCOPERATION;




typedef struct
{
    MmsStateMachine eFsm;    
    long lInstance;          
    WE_UINT32 uiSignal;      
} Caller;

static ST_ASYNCOPERATION *s_pstMyOperationPool;
static int s_iMyPoolSize;
static Caller s_stMyCaller;

 
#ifdef WE_LOG_MODULE 
const char *pcFsmAyncSigName(WE_UINT32 uiSigType)
{
    switch (uiSigType)
    {
    case MMS_SIG_ASYNC_FILE_NOTIFY:
        return "MMS_SIG_ASYNC_FILE_NOTIFY";
    case MMS_SIG_ASYNC_PIPE_NOTIFY:
        return "MMS_SIG_ASYNC_PIPE_NOTIFY";
    case MMS_SIG_ASYNC_INT_TERMINATE:
        return "MMS_SIG_ASYNC_INT_TERMINATE";        
    default:
        return 0;
    }

} 
#endif
static MmsResult translateFileError(int iFileError)
{
    MmsResult eRet = MMS_RESULT_ERROR;

    switch (iFileError)
    {
    case WE_FILE_OK :
        eRet = MMS_RESULT_OK;
        break;
    case WE_FILE_ERROR_ACCESS : 
        eRet = MMS_RESULT_ERROR;
        break;
    case WE_FILE_ERROR_DELAYED :
        eRet = MMS_RESULT_DELAYED;
        break;
    case WE_FILE_ERROR_PATH :
        eRet = MMS_RESULT_FILE_NOT_FOUND;
        break;
    case WE_FILE_ERROR_INVALID : 
        eRet = MMS_RESULT_ERROR;
        break;
    case WE_FILE_ERROR_SIZE :
        eRet = MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE;
        break;
    case WE_FILE_ERROR_FULL :
        eRet = MMS_RESULT_INSUFFICIENT_PERSISTENT_STORAGE;
        break;
    case WE_FILE_ERROR_EOF :
        eRet = MMS_RESULT_EOF;
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Unknown iFileError %d\n", __FILE__, __LINE__,
            iFileError));
        break;
    } 

    return eRet;
} 






static MmsResult translatePipeError(int iPipeError)
{
    MmsResult eRet = MMS_RESULT_ERROR;

    switch (iPipeError)
    {
    case WE_PIPE_ERROR_BAD_HANDLE :
    case WE_PIPE_ERROR_INVALID_PARAM :
    case WE_PIPE_ERROR_EXISTS :
    case WE_PIPE_ERROR_IS_OPEN :
    case WE_PIPE_ERROR_RESOURCE_LIMIT :
    case WE_PIPE_ERROR_NOT_FOUND :
        eRet = MMS_RESULT_ERROR;
        break;
    case WE_PIPE_ERROR_DELAYED :
        eRet = MMS_RESULT_DELAYED;
        break;
    case WE_PIPE_ERROR_CLOSED :
        eRet = MMS_RESULT_EOF;
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Unknown iPipeError %d\n", __FILE__, __LINE__,
            iPipeError));
        break;
    } 

    return eRet;
} 

static ST_ASYNCOPERATION *asyncGetInstance()
{
    ST_ASYNCOPERATION *pstAnOper = s_pstMyOperationPool;
    ST_ASYNCOPERATION *pstLast = NULL;

    
    while (pstAnOper != NULL && pstAnOper->eState != OPER_STATE_FREE)
    {
        pstLast = pstAnOper;
        pstAnOper = pstAnOper->pstNext;
    } 

    
    if (pstAnOper == NULL)
    {
        if ((pstAnOper = M_ALLOC(sizeof(ST_ASYNCOPERATION))) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "MMS FSM Async: No free async operations\n"));

            return NULL;
        } 

        if (NULL != pstLast)
        {
            pstLast->pstNext = pstAnOper;
        }
        pstAnOper->pstPrevious = pstLast;
        pstAnOper->pstNext = NULL;
        ++s_iMyPoolSize;
    } 
    pstAnOper->iFileHandle = 0;
    pstAnOper->iPipeHandle = 0;

    return pstAnOper;
}






void asyncEmergencyAbort(void)
{
    ST_ASYNCOPERATION *pstAnOper = s_pstMyOperationPool;

    mSignalDeregister(M_FSM_ASYNC);

    
    while (pstAnOper != NULL)
    {
        if (pstAnOper->eState == OPER_STATE_OPENED && pstAnOper->iFileHandle > 0)
        {
            (void) WE_FILE_CLOSE(pstAnOper->iFileHandle);
            pstAnOper->iFileHandle = 0;
        } 
        if (pstAnOper->eState == OPER_STATE_OPENED && pstAnOper->iPipeHandle > 0)
        {
            (void) WE_PIPE_CLOSE(pstAnOper->iPipeHandle);
            
            if (ASYNC_MODE_READ == pstAnOper->eMode)
            {
                (void)WE_PIPE_DELETE(pstAnOper->pcPipeName);
            }
            pstAnOper->iPipeHandle = 0;
        } 

        pstAnOper->eState = OPER_STATE_FREE;
        pstAnOper = pstAnOper->pstNext;
    } 
} 







static void doFileRead(ST_ASYNCOPERATION *pstLast)
{
    int iRet;
    long length;

    length = pstLast->pucBuffer - pstLast->pucPos + (long)pstLast->uiDataLen;

    
    for (;;)
    {
        if (length <= 0) 
        {
            M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                MMS_RESULT_OK, pstLast->uiTotalLen);
            return;
        } 

        iRet = WE_FILE_READ( pstLast->iFileHandle, pstLast->pucPos, length);
        switch (iRet)
        {
        case 0 :
        case WE_FILE_ERROR_EOF :
            M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                MMS_RESULT_EOF, pstLast->uiTotalLen);
            return;

        case WE_FILE_ERROR_ACCESS :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): File read returned %d\n", __FILE__, __LINE__, iRet));

            M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                translateFileError(iRet), pstLast->uiTotalLen);
            return;

        case WE_FILE_ERROR_DELAYED :
            WE_FILE_SELECT( pstLast->iFileHandle, WE_FILE_EVENT_READ);
            return;

        default :
            if (iRet < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Unhandled file error %d\n",
                    __FILE__, __LINE__, iRet));
                M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                    translateFileError(iRet), pstLast->uiTotalLen);

                return; 
            } 

            pstLast->uiTotalLen += (WE_UINT32)iRet;
            pstLast->pucPos += iRet;
            length -= iRet;
            break;
        } 
    } 
} 







static void doFileSeek
(
	const ST_ASYNCOPERATION *pstLast, 
	WE_INT32 iStartPos
)
{
    int iSeekMode;
    long lSeekOffset;
    long lRet;

    if (iStartPos == -1)
    {
        iSeekMode = WE_FILE_SEEK_END;
        lSeekOffset = 0;
    }
    else
    {
        iSeekMode = WE_FILE_SEEK_SET;
        lSeekOffset = iStartPos;
    } 

    if ((lRet = WE_FILE_SEEK( pstLast->iFileHandle, lSeekOffset, iSeekMode)) < 0)
    {
        M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
            translateFileError(lRet), pstLast->uiTotalLen);

        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
            "%s(%d): Failed to seek file %d, eMode=%d, pucPos=%d, lRet=%d\n", 
            __FILE__, __LINE__, pstLast->iFileHandle, iSeekMode, lSeekOffset, lRet));

        return;
    } 
} 






static void doFileWrite(ST_ASYNCOPERATION *pstLast)
{
    long lRet;
    long length;

    length = pstLast->pucBuffer - pstLast->pucPos + (long)pstLast->uiDataLen;

    
    for (;;)
    {
        if (length <= 0)    
        {
            M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                MMS_RESULT_OK, pstLast->uiTotalLen);
            return;
        } 

        lRet = WE_FILE_WRITE( pstLast->iFileHandle, pstLast->pucPos, length);
        switch (lRet)
        {
        case 0 :
        case WE_FILE_ERROR_EOF :
        case WE_FILE_ERROR_ACCESS :
        case WE_FILE_ERROR_FULL :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): File write returned %d\n", __FILE__, __LINE__, lRet));

            M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                translateFileError(lRet), pstLast->uiTotalLen);
            return;

        case WE_FILE_ERROR_DELAYED :
            WE_FILE_SELECT( pstLast->iFileHandle, WE_FILE_EVENT_WRITE);
            return;

        default :
            if (lRet < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Unhandled file error %d\n",
                    __FILE__, __LINE__, lRet));
                M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                    translateFileError(lRet), pstLast->uiTotalLen);

                return; 
            } 

            pstLast->uiTotalLen += (WE_UINT32) lRet;
            pstLast->pucPos += lRet;
            length -= lRet;
            break;
        } 
    } 
} 






static void doPipeRead(ST_ASYNCOPERATION *pstLast)
{
    int iRet;
    long length;

    length = pstLast->pucBuffer - pstLast->pucPos + (long)pstLast->uiDataLen;

    
    for (;;)
    {
        if (length <= 0) 
        {
            M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                MMS_RESULT_OK, pstLast->uiTotalLen);
            return;
        } 

        iRet = WE_PIPE_READ( pstLast->iPipeHandle, pstLast->pucPos, length);
        switch (iRet)
        {
        case WE_PIPE_ERROR_CLOSED :
            M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                MMS_RESULT_EOF, pstLast->uiTotalLen);
            return;

        case WE_PIPE_ERROR_NOT_FOUND :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Pipe read returned %d\n", __FILE__, __LINE__, iRet));

            M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                translatePipeError(iRet), pstLast->uiTotalLen);
            return;

        case 0 :
        case WE_PIPE_ERROR_DELAYED :
            (void)WE_PIPE_POLL(pstLast->iFileHandle);
            return;

        default :
            if (iRet < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Unhandled pipe error %d\n",
                    __FILE__, __LINE__, iRet));
                M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                    translatePipeError(iRet), pstLast->uiTotalLen);

                return; 
            } 

            pstLast->uiTotalLen += (WE_UINT32)iRet;
            pstLast->pucPos += iRet;
            length -= iRet;
            break;
        } 
    } 
} 







static void doPipeWrite(ST_ASYNCOPERATION *pstLast)
{
    long lRet;
    long length;

    length = pstLast->pucBuffer - pstLast->pucPos + (long)pstLast->uiDataLen;

    
    for (;;)
    {
        if (length <= 0)    
        {
            M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                MMS_RESULT_OK, pstLast->uiTotalLen);
            return;
        } 

        lRet = WE_PIPE_WRITE( pstLast->iPipeHandle, pstLast->pucPos, length);
        switch (lRet)
        {
        case WE_PIPE_ERROR_CLOSED :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Pipe write returned %d\n", __FILE__, __LINE__, lRet));

            M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                translatePipeError(lRet), pstLast->uiTotalLen);
            return;

        case 0 :
        case WE_PIPE_ERROR_DELAYED :
            (void)WE_PIPE_POLL(pstLast->iPipeHandle);
            return;

        default :
            if (lRet < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Unhandled pipe error %d\n",
                    __FILE__, __LINE__, lRet));
                M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
                    translatePipeError(lRet), pstLast->uiTotalLen);

                return; 
            } 

            pstLast->uiTotalLen += (WE_UINT32) lRet;
            pstLast->pucPos += lRet;
            length -= lRet;
            break;
        } 
    } 
} 
AsyncOperationHandle asyncOperationStart
(
	const char *pcFileName, 
	MmsStateMachine eFsm,
	long lInstance,
	WE_UINT32 uiSignal,
	AsyncMode eMode
)
{
    ST_ASYNCOPERATION *pstAnOper;
    int iFileHandle;
    int iOpenMode;

    
    pstAnOper = asyncGetInstance();
    if (NULL == pstAnOper)
    {
        return NULL;
    }

    switch (eMode)
    {
    case ASYNC_MODE_READ :
        iOpenMode = WE_FILE_SET_RDONLY;
        break;
    case ASYNC_MODE_WRITE :
        iOpenMode = WE_FILE_SET_WRONLY | WE_FILE_SET_CREATE;
        break;
    case ASYNC_MODE_APPEND :
        iOpenMode = WE_FILE_SET_WRONLY | WE_FILE_SET_CREATE |
            WE_FILE_SET_APPEND;
        break;
    case ASYNC_MODE_READ_WRITE :
        iOpenMode = WE_FILE_SET_RDWR | WE_FILE_SET_CREATE;
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
            "%s(%d): Illegal open eMode %d\n", __FILE__, __LINE__, 
            eMode));

        iOpenMode = WE_FILE_SET_RDWR | WE_FILE_SET_CREATE;
        break;
    } 

    iFileHandle = WE_FILE_OPEN( WE_MODID_MMS, pcFileName, iOpenMode, 0);

    if (iFileHandle < 0) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
            "%s(%d): Failed to open file <%s>, %d\n", __FILE__, __LINE__, 
            pcFileName == NULL ? "NULL" : pcFileName, iFileHandle));
        return NULL;
    } 

    pstAnOper->eFsm = eFsm;
    pstAnOper->lInstance = lInstance;
    pstAnOper->uiSignal = uiSignal;
    pstAnOper->iFileHandle = iFileHandle;
    pstAnOper->iPipeHandle = 0;
    pstAnOper->eState = OPER_STATE_OPENED;
    pstAnOper->pucBuffer = NULL;
    pstAnOper->uiDataLen = 0;
    pstAnOper->eMode = eMode;        

    return pstAnOper;
} 















AsyncOperationHandle asyncPipeOperationStart
(
	const char *pcPipeName, 
    	MmsStateMachine eFsm,
    	long lInstance,
    	WE_UINT32 uiSignal,
    	AsyncMode eMode
)
{
    ST_ASYNCOPERATION *pstAnOper = s_pstMyOperationPool;

    
    pstAnOper = asyncGetInstance();
    if (NULL == pstAnOper)
    {
        return NULL;
    }

    switch (eMode)
    {
    case ASYNC_MODE_READ :
        pstAnOper->iPipeHandle = WE_PIPE_OPEN( WE_MODID_MMS, pcPipeName);
        break;
    case ASYNC_MODE_WRITE :
        pstAnOper->iPipeHandle = WE_PIPE_CREATE( WE_MODID_MMS, pcPipeName);
        break;
    case ASYNC_MODE_READ_WRITE:
    case ASYNC_MODE_APPEND:    
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
            "%s(%d): Illegal pipe eMode %d\n", __FILE__, __LINE__, 
            eMode));

        break;
    } 

    if (pstAnOper->iPipeHandle < 0) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_MMS, 
            "%s(%d): Failed to open/create pipe <%s>, %d\n", __FILE__, __LINE__, 
            pcPipeName == NULL ? "NULL" : pcPipeName, pstAnOper->iPipeHandle));
        return NULL;
    } 

    pstAnOper->eFsm = eFsm;
    pstAnOper->eMode = eMode;
    pstAnOper->pcPipeName = we_cmmn_strdup(WE_MODID_MMS, pcPipeName);
    pstAnOper->lInstance = lInstance;
    pstAnOper->uiSignal = uiSignal;
    pstAnOper->iFileHandle = 0;
    pstAnOper->eState = OPER_STATE_OPENED;
    pstAnOper->pucBuffer = NULL;
    pstAnOper->uiDataLen = 0;
    pstAnOper->eMode = eMode; 

    return pstAnOper;
} 






void asyncOperationStop(AsyncOperationHandle *ppOper)
{
    ST_ASYNCOPERATION *pstLast = (ST_ASYNCOPERATION *)*ppOper;

    if (pstLast == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Invalid async ppOper handle pstLast=%d\n", 
            __FILE__, __LINE__, pstLast));

        return;
    } 

    if (pstLast->eState == OPER_STATE_OPENED && pstLast->iFileHandle > 0)
    {
        (void) WE_FILE_CLOSE(pstLast->iFileHandle);
    } 
    if (pstLast->eState == OPER_STATE_OPENED && pstLast->iPipeHandle > 0)
    {
        (void)WE_PIPE_CLOSE(pstLast->iPipeHandle);
        
        if (ASYNC_MODE_READ == pstLast->eMode)
        {
            (void)WE_PIPE_DELETE(pstLast->pcPipeName);
        }
        
        if (NULL != pstLast->pcPipeName)
        {
            M_FREE(pstLast->pcPipeName);
            pstLast->pcPipeName = NULL;
        }
    } 

    pstLast->eState = OPER_STATE_FREE;
    pstLast->iFileHandle = 0;

    if (s_iMyPoolSize > ASYNC_POOL_SIZE)
    {
        if (pstLast->pstPrevious != NULL)
        {
            pstLast->pstPrevious->pstNext = pstLast->pstNext;
        } 

        if (pstLast->pstNext != NULL)
        {
            pstLast->pstNext->pstPrevious = pstLast->pstPrevious;
        } 

        M_FREE(pstLast);

        --s_iMyPoolSize;
    } 

    *ppOper = NULL;
} 













void asyncReadContinue
(
	AsyncOperationHandle pOper,
	WE_UINT32 uiBufSize
)
{
    ST_ASYNCOPERATION *pstLast = (ST_ASYNCOPERATION *)pOper;

    if (pstLast == NULL || pstLast->eState != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong handle or eState for readContinue, pstLast=%d, eState=%d\n", 
            __FILE__, __LINE__, pstLast, pstLast == NULL ? -1 : pstLast->eState));

        return;
    }
    else if (pstLast->iFileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): File was not opened, check pstPrevious errors, pstLast=%d, eState=%d\n", 
            __FILE__, __LINE__, pstLast, pstLast == NULL ? -1 : pstLast->eState));

        M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
            MMS_RESULT_FILE_NOT_FOUND, pstLast->uiTotalLen);
        return;
    } 

    pstLast->uiTotalLen = 0;
    pstLast->pucPos = pstLast->pucBuffer;
    pstLast->uiDataLen = uiBufSize;
    doFileRead(pstLast);
} 








void asyncTerminate
(
	MmsStateMachine eFsm,
	WE_UINT32 uiSignal,
	long lInstance
)
{
    ST_ASYNCOPERATION *pstAnOper = s_pstMyOperationPool;

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM Async: Terminate\n"));

    s_stMyCaller.eFsm = eFsm;
    s_stMyCaller.uiSignal = uiSignal;
    s_stMyCaller.lInstance = lInstance;

    while (pstAnOper != NULL)
    {
        if (pstAnOper->eState != OPER_STATE_FREE)
        {
            
            M_TIMER_SET( M_FSM_ASYNC, MMS_TERMINATE_FILE_TIMEOUT);
            return;
        } 

        pstAnOper = pstAnOper->pstNext;
    } 

    M_SIGNAL_SENDTO( M_FSM_ASYNC, (int)MMS_SIG_ASYNC_INT_TERMINATE);
} 










void asyncWriteContinue
(
	AsyncOperationHandle pOper,
	WE_UINT32 uiBufSize
)
{
    ST_ASYNCOPERATION *pstLast = (ST_ASYNCOPERATION *)pOper;

    if (pstLast == NULL || pstLast->eState != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong handle or eState for writeContinue, pstLast=%d, eState=%d\n", 
            __FILE__, __LINE__, pstLast, pstLast == NULL ? -1 : pstLast->eState));

        return;
    }
    else if (pstLast->iFileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): File was not opened, check pstPrevious errors, pstLast=%d, eState=%d\n", 
            __FILE__, __LINE__, pstLast, pstLast == NULL ? -1 : pstLast->eState));

        M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
            MMS_RESULT_FILE_NOT_FOUND, pstLast->uiTotalLen);
        return;
    } 

    pstLast->uiTotalLen = 0;
    pstLast->pucPos = pstLast->pucBuffer;
    pstLast->uiDataLen = uiBufSize;
    doFileWrite(pstLast);
} 








static void handleFileNotify
(
	int iFileHandle,
	int iOperationType
)
{
    ST_ASYNCOPERATION *pstAnOper = s_pstMyOperationPool;

    while (pstAnOper != NULL)
    {   
        if (pstAnOper->eState == OPER_STATE_OPENED && 
            pstAnOper->iFileHandle == iFileHandle)
        {
            switch (iOperationType)
            {
            case WE_FILE_READ_READY :
                doFileRead(pstAnOper);
                break;
            case WE_FILE_WRITE_READY :
                doFileWrite(pstAnOper);
                break;
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Illegal operationType %d\n", __FILE__, __LINE__,
                    iOperationType));
                break;
            } 

            return;
        } 

        pstAnOper = pstAnOper->pstNext;
    } 

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "%s(%d): Couldn't find handle\n", __FILE__, __LINE__));
} 







static void handlePipeNotify
(
	int iPipeHandle,
	int iOperationType
)
{
    ST_ASYNCOPERATION *pstAnOper = s_pstMyOperationPool;

    while (pstAnOper != NULL)
    {   
        if (pstAnOper->eState == OPER_STATE_OPENED && 
            pstAnOper->iPipeHandle == iPipeHandle)
        {
            switch (iOperationType)
            {
            case WE_PIPE_EVENT_READ:
                doPipeRead(pstAnOper);
                break;
            case WE_PIPE_EVENT_WRITE:
                doPipeWrite(pstAnOper);
                break;
            case WE_PIPE_EVENT_CLOSED:
                M_SIGNAL_SENDTO_IUU( pstAnOper->eFsm,
                    pstAnOper->uiSignal, pstAnOper->lInstance,
                    MMS_RESULT_EOF, pstAnOper->uiTotalLen);
                break;
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                    "%s(%d): Illegal operationType %d\n", __FILE__, __LINE__,
                    iOperationType));
                break;
            } 

            return;
        } 

        pstAnOper = pstAnOper->pstNext;
    } 

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "%s(%d): Couldn't find handle\n", __FILE__, __LINE__));
} 




static void handleTerminate(void)
{
    ST_ASYNCOPERATION *pstAnOper = s_pstMyOperationPool;
    ST_ASYNCOPERATION *pstNext = NULL;

    M_TIMER_RESET(M_FSM_ASYNC); 

    s_pstMyOperationPool = NULL;
    s_iMyPoolSize = 0;
    while (pstAnOper != NULL)
    {
#ifdef _DEBUG
        if (pstAnOper->eState == OPER_STATE_OPENED)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Operation in eFsm %d not stopped. File is still open!\n", 
                __FILE__, __LINE__, pstAnOper->eFsm));
        } 
#endif 

        pstNext = pstAnOper->pstNext;
        M_FREE(pstAnOper);
        pstAnOper = pstNext;
    } 

    

    M_SIGNAL_SENDTO_I( s_stMyCaller.eFsm, s_stMyCaller.uiSignal, s_stMyCaller.lInstance);

    mSignalDeregister(M_FSM_ASYNC);
} 







static void asyncMain(MmsSignal *pstSig)
{
    switch (pstSig->type)
    {
    case MMS_SIG_ASYNC_FILE_NOTIFY :
        handleFileNotify( (int)pstSig->u_param1, pstSig->i_param);
        break;
    case MMS_SIG_ASYNC_PIPE_NOTIFY :
        handlePipeNotify( (int)pstSig->u_param1, pstSig->i_param);
        break;
    case MMS_SIG_ASYNC_INT_TERMINATE :
        handleTerminate();
        break;
    case MMS_SIG_COMMON_TIMER_EXPIRED :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "MMS FSM ASYNC: Received TIMER_EXPIRED so kill without mercy.\n"));

        handleTerminate();
        break;
    default:
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): received unknown uiSignal %d\n", __FILE__, __LINE__,
            pstSig->type));
        break;
    } 
    
    mSignalDelete(pstSig);
} 





WE_BOOL asyncInit(void)
{
    int i;
    ST_ASYNCOPERATION *pstPrevious = NULL;
    ST_ASYNCOPERATION *pstNew = NULL;

    memset ( &s_stMyCaller, 0, sizeof(s_stMyCaller));
    mSignalRegisterDst(M_FSM_ASYNC, asyncMain);
    
    s_pstMyOperationPool = NULL;
    s_iMyPoolSize = 0;
    for (i = 0; i < ASYNC_POOL_SIZE; ++i)
    {
        if ((pstNew = M_ALLOC(sizeof(ST_ASYNCOPERATION))) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
                "%s(%d): Insufficient memory during initialization\n", 
                __FILE__, __LINE__));
            return FALSE;
        } 

        memset( pstNew, 0, sizeof(ST_ASYNCOPERATION));
        pstNew->eState = OPER_STATE_FREE;
        if (s_pstMyOperationPool == NULL)
        {
            s_pstMyOperationPool = pstNew;
            s_pstMyOperationPool->pstNext = NULL;
            s_pstMyOperationPool->pstPrevious = NULL;
            pstPrevious = pstNew;
        }
        else 
        {
            if (pstPrevious != NULL)
            {
                pstPrevious->pstNext = pstNew;
            } 

            pstNew->pstPrevious = pstPrevious;
            pstNew->pstNext = NULL;
            pstPrevious = pstNew;
        } 
    } 

    s_iMyPoolSize = i;

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
        "MMS FSM Async: initialized\n"));

    return TRUE;
} 


void asyncRead
(
	AsyncOperationHandle pOper,
	WE_INT32 iStartPos,
   	void *pBuffer,
   	WE_UINT32 uiBufSize
)
{
    ST_ASYNCOPERATION *pstLast = (ST_ASYNCOPERATION *)pOper;

    if (pstLast == NULL || pstLast->eState != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong handle or eState for read, pstLast=%d, eState=%d\n", 
            __FILE__, __LINE__, pstLast, pstLast == NULL ? -1 : pstLast->eState));

        return;
    }
    else if (pstLast->iFileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): File was not opened, check pstPrevious errors, pstLast=%d, eState=%d\n", 
            __FILE__, __LINE__, pstLast, pstLast == NULL ? -1 : pstLast->eState));

        M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
            MMS_RESULT_FILE_NOT_FOUND, pstLast->uiTotalLen);
        return;
    } 

    
    pstLast->uiTotalLen = 0;
    pstLast->pucPos = pBuffer;
    pstLast->pucBuffer = pBuffer;
    pstLast->uiDataLen = uiBufSize;

    if (pstLast->eMode != ASYNC_MODE_APPEND)
    {
        doFileSeek(pstLast, iStartPos);
    }
    doFileRead(pstLast);
} 














void asyncWrite
(
	AsyncOperationHandle pOper,
	WE_INT32 iStartPos,
	void *pBuffer, 
    	WE_UINT32 uiBufSize
)
{
    ST_ASYNCOPERATION *pstLast = (ST_ASYNCOPERATION *)pOper;

    if (pstLast == NULL || pstLast->eState != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong handle or eState for write, pstLast=%d, eState=%d\n", 
            __FILE__, __LINE__, pstLast, pstLast == NULL ? -1 : pstLast->eState));

        return;
    }
    else if (pstLast->iFileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): File was not opened, check pstPrevious errors, pstLast=%d, eState=%d\n", 
            __FILE__, __LINE__, pstLast, pstLast == NULL ? -1 : pstLast->eState));

        M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
            MMS_RESULT_FILE_NOT_FOUND, pstLast->uiTotalLen);
        return;
    } 

    
    pstLast->uiTotalLen = 0;
    pstLast->pucPos = pBuffer;
    pstLast->pucBuffer = pBuffer;
    pstLast->uiDataLen = uiBufSize;

    if (pstLast->eMode != ASYNC_MODE_APPEND)
    {
        doFileSeek(pstLast, iStartPos);
    }
    doFileWrite(pstLast);
} 




void asyncPipeRead
(
	AsyncOperationHandle pOper,
       void *pBuffer,
       WE_UINT32 uiBufSize
)
{
    ST_ASYNCOPERATION *pstLast = (ST_ASYNCOPERATION *)pOper;

    if (pstLast == NULL || pstLast->eState != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong handle or eState for pipe read, pstLast=%d, eState=%d\n", 
            __FILE__, __LINE__, pstLast, pstLast == NULL ? -1 : pstLast->eState));

        return;
    }
    else if (pstLast->iPipeHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Pipe was not opened, check pstPrevious errors, pstLast=%d, eState=%d\n", 
            __FILE__, __LINE__, pstLast, pstLast == NULL ? -1 : pstLast->eState));

        M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
            MMS_RESULT_ERROR, pstLast->uiTotalLen);
        return;
    } 

    
    pstLast->uiTotalLen = 0;
    pstLast->pucPos = pBuffer;
    pstLast->pucBuffer = pBuffer;
    pstLast->uiDataLen = uiBufSize;

    doPipeRead(pstLast);
} 





void asyncPipeWrite
(
	AsyncOperationHandle pOper,
       void *pBuffer,
       WE_UINT32 uiBufSize
)
{
    ST_ASYNCOPERATION *pstLast = (ST_ASYNCOPERATION *)pOper;

    if (pstLast == NULL || pstLast->eState != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Wrong handle or eState for pipe write, pstLast=%d, eState=%d\n", 
            __FILE__, __LINE__, pstLast, pstLast == NULL ? -1 : pstLast->eState));

        return;
    }
    else if (pstLast->iPipeHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MMS, 
            "%s(%d): Pipe was not opened, check pstPrevious errors, pstLast=%d, eState=%d\n", 
            __FILE__, __LINE__, pstLast, pstLast == NULL ? -1 : pstLast->eState));

        M_SIGNAL_SENDTO_IUU( pstLast->eFsm, pstLast->uiSignal, pstLast->lInstance, 
            MMS_RESULT_ERROR, pstLast->uiTotalLen);
        return;
    } 

    
    pstLast->uiTotalLen = 0;
    pstLast->pucPos = pBuffer;
    pstLast->pucBuffer = pBuffer;
    pstLast->uiDataLen = uiBufSize;

    doPipeWrite(pstLast);
} 




WE_BOOL asyncIsPipeOwner(int iPipeHandle)
{
    ST_ASYNCOPERATION *pstAnOper = s_pstMyOperationPool;
    
    
    while (pstAnOper != NULL && pstAnOper->iPipeHandle == iPipeHandle && pstAnOper->eState != OPER_STATE_FREE)
    {
        pstAnOper = pstAnOper->pstNext;
    } 
    
    return pstAnOper != NULL;
} 













 














