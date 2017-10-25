/*
 * Copyright (C) Techfaith, 2002-2005.
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

#include "Mms_Def.h"

#include "Sis_Isig.h"   
#include "Sis_Mem.h"      
#include "Sis_Asy.h"     



#define ASYNC_POOL_SIZE 6






typedef enum
{
    OPER_STATE_FREE = 0,    
    OPER_STATE_OPENED       
} OperState;




typedef struct AsyncOper
{   
    struct AsyncOper *next; 
    struct AsyncOper *previous; 

    OperState state;        
    SlsAsyncMode mode;      

    SlsStateMachine fsm;    
    long instance;          
    WE_UINT32 signal;      

    int fileHandle;         
    WE_UINT32 totalLen;    
    unsigned char *pos;     
    unsigned char *buffer;  
    WE_UINT32 dataLen;     

    char *pipeName;         
    int pipeHandle;         
} AsyncOperation;

 

 

 

 

 

 





static AsyncOperation *myOperationPool;
static int myPoolSize;

 
static void asyncMain(SlsSignal *sig);
static void doRead(AsyncOperation *op);
static void doSeek(const AsyncOperation *op, WE_INT32 startPos);
static void doWrite(AsyncOperation *op);
static void handleNotify(int fileHandle, int operationType);
static SlsAsioResult translateFileError(int fileError);
static void doPipeRead(AsyncOperation *op);
static void doPipeWrite(AsyncOperation *op);
static void handlePipeNotify(int pipeHandle, int operationType);
static AsyncOperation *slsAsyncGetNewInstance(void);







WE_BOOL slsAsyncInit(void)
{
    int i;
    AsyncOperation *previous = NULL;
    AsyncOperation *aNew = NULL;

    slsSignalRegisterDst(SIS_FSM_ASYNC, asyncMain);
    
    myOperationPool = NULL;
    myPoolSize = 0;
    for (i = 0; i < ASYNC_POOL_SIZE; ++i)
    {
        
        if ((aNew = SIS_ALLOC(sizeof(AsyncOperation))) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                "%s(%d): Insufficient memory during initialization\n", 
                __FILE__, __LINE__));
            return FALSE;
        } 

        memset( aNew, 0, sizeof(AsyncOperation));
        aNew->state = OPER_STATE_FREE;
        if (myOperationPool == NULL)
        {
            myOperationPool = aNew;
            myOperationPool->next = NULL;
            myOperationPool->previous = NULL;
            previous = aNew;
        }
        else 
        {
            if (previous != NULL)
            {
                previous->next = aNew;
            } 

            aNew->previous = previous;
            aNew->next = NULL;
            previous = aNew;
        } 
    } 

    myPoolSize = i;

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
        "MMS FSM Async: initialized\n"));

    return TRUE;
} 






static void asyncMain(SlsSignal *sig)
{
    switch (sig->type)
    {
    case SIS_SIG_ASYNC_NOTIFY :
        handleNotify( (int)sig->u_param1, sig->i_param);
        break;
    case SIS_SIG_ASYNC_PIPE_NOTIFY :
        handlePipeNotify( (int)sig->u_param1, sig->i_param);
        break;
    default:
        
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): received unknown signal %d\n", __FILE__, __LINE__,
            sig->type));
        break;
    } 
    
    slsSignalDelete(sig);
} 















SlsAsyncOperationHandle slsAsyncOperationStart(const char *fileName, 
    SlsStateMachine fsm, long instance, WE_UINT32 signal, SlsAsyncMode mode)
{
    AsyncOperation *anOper = myOperationPool;
    AsyncOperation *last = NULL;
    int fileHandle;
    int openMode;

    
    while (anOper != NULL && anOper->state != OPER_STATE_FREE)
    {
        last = anOper;
        anOper = anOper->next;
    } 

    
    if (anOper == NULL)
    {
        if ((anOper = SIS_ALLOC(sizeof(AsyncOperation))) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                "MMS FSM Async: No free async operations\n"));

            return NULL;
        } 
        
        last->next = anOper;
        anOper->previous = last;
        anOper->next = NULL;
        ++myPoolSize;
    } 
    
    switch (mode)
    {
    case SIS_ASYNC_MODE_READ :
        openMode = WE_FILE_SET_RDONLY;
        break;
    case SIS_ASYNC_MODE_WRITE :
        openMode = WE_FILE_SET_WRONLY | WE_FILE_SET_CREATE;
        break;
    case SIS_ASYNC_MODE_APPEND :
        openMode = WE_FILE_SET_WRONLY | WE_FILE_SET_CREATE |
            WE_FILE_SET_APPEND;
        break;
    case SIS_ASYNC_MODE_READ_WRITE :
        

        openMode = WE_FILE_SET_RDWR | WE_FILE_SET_CREATE;
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_SIS, 
            "%s(%d): Illegal open mode %d\n", __FILE__, __LINE__, 
            mode));

        openMode = WE_FILE_SET_RDWR | WE_FILE_SET_CREATE;
        break;
    } 

    fileHandle = WE_FILE_OPEN(WE_MODID_SIS, fileName, openMode, 0);

    if (fileHandle < 0) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_SIS, 
            "%s(%d): Failed to open file <%s>, %d\n", __FILE__, __LINE__, 
            fileName == NULL ? "NULL" : fileName, fileHandle));
    } 

    anOper->fsm = fsm;
    anOper->instance = instance;
    anOper->signal = signal;
    anOper->fileHandle = fileHandle;
    anOper->state = OPER_STATE_OPENED;
    anOper->buffer = NULL;
    anOper->dataLen = 0;
    anOper->mode = mode;

    return anOper;
} 











void slsAsyncRegister(int fileHandle, int fsm, int signal, int instance)
{
    AsyncOperation *anOper = myOperationPool;
    AsyncOperation *last = NULL;

    
    while (anOper != NULL && anOper->state != OPER_STATE_FREE)
    {
        last = anOper;
        anOper = anOper->next;
    } 

    
    if (anOper == NULL)
    {
        if ((anOper = SIS_ALLOC(sizeof(AsyncOperation))) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                "MMS FSM Async: No free async operations\n"));

            
        } 
        
        last->next = anOper;
        
        anOper->previous = last;
        
        anOper->next = NULL;
        ++myPoolSize;
    } 

    if (fileHandle < 0) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_SIS, 
            "%s(%d): Erroneous file handle <%d>\n", __FILE__, __LINE__, 
            fileHandle));
        return;
    } 

    
    anOper->fsm = (SlsStateMachine)fsm;
    
    anOper->instance = instance;
    
    anOper->signal = (unsigned)signal;
    
    anOper->fileHandle = fileHandle;
    
    anOper->buffer = NULL;
    
    anOper->dataLen = 0;
    
    anOper->mode = SIS_ASYNC_MODE_DISPATCH;
}









void slsAsyncDeregister(int fileHandle)
{
    AsyncOperation *op = myOperationPool;

    while((op != NULL) && (op->fileHandle != fileHandle))
        op = op->next;
    if (NULL == op)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Invalid async oper handle op=%d\n", 
            __FILE__, __LINE__, op));
        return;
    }

    if (myPoolSize > ASYNC_POOL_SIZE)
    {
        if (op->previous != NULL)
        {
            op->previous->next = op->next;
        } 

        if (op->next != NULL)
        {
            op->next->previous = op->previous;
        } 
        
        SIS_FREE(op);

        --myPoolSize;
    } 
}







void slsAsyncOperationStop(SlsAsyncOperationHandle *oper)
{
    AsyncOperation *op = (AsyncOperation *)*oper;

    if (op == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Invalid async oper handle op=%d\n", 
            __FILE__, __LINE__, op));

        return;
    } 

    if (op->state == OPER_STATE_OPENED && op->fileHandle > 0)
    {
        (void) WE_FILE_CLOSE(op->fileHandle);
    } 
    if (op->state == OPER_STATE_OPENED && op->pipeHandle > 0)
    {
        (void) WE_PIPE_CLOSE(op->pipeHandle);

        
        if (SIS_ASYNC_MODE_READ == op->mode)
        {
            (void)WE_PIPE_DELETE(op->pipeName);
        }
        SIS_FREE(op->pipeName);
    } 

    op->state = OPER_STATE_FREE;
    op->fileHandle = 0;

    if (myPoolSize > ASYNC_POOL_SIZE)
    {
        if (op->previous != NULL)
        {
            op->previous->next = op->next;
        } 

        if (op->next != NULL)
        {
            op->next->previous = op->previous;
        } 
        
        SIS_FREE(op);

        --myPoolSize;
    } 

    *oper = NULL;
} 














void slsAsyncRead(SlsAsyncOperationHandle oper, WE_INT32 startPos, 
    void *buffer, WE_UINT32 bufSize)
{
    AsyncOperation *op = (AsyncOperation *)oper;

    if (op == NULL || op->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Wrong handle or state for read, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        return;
    }
    else if (op->fileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): File was not opened, check previous errors, op=%d," 
            "state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        (void)SIS_SIGNAL_SENDTO_IUU( op->fsm, (int)op->signal, op->instance, 
            SIS_ASIO_RESULT_FILE_NOT_FOUND, op->totalLen);
        return;
    } 

    
    op->totalLen = 0;
    op->pos = buffer;
    op->buffer = buffer;
    op->dataLen = bufSize;

    doSeek(op, startPos);
    doRead(op);
} 










void slsAsyncReadContinue(SlsAsyncOperationHandle oper, WE_UINT32 bufSize)
{
    AsyncOperation *op = (AsyncOperation *)oper;

    if (op == NULL || op->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Wrong handle or state for readContinue, op=%d, state=%d\n"
            , __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        return;
    }
    else if (op->fileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): File was not opened, check previous errors, op=%d, "
            "state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, (int)op->signal, op->instance, 
            SIS_ASIO_RESULT_FILE_NOT_FOUND, op->totalLen);
        return;
    } 

    op->dataLen = bufSize;
    doRead(op);
} 














void slsAsyncWrite(SlsAsyncOperationHandle oper, WE_INT32 startPos, 
                   void *buffer, WE_UINT32 bufSize)
{
    AsyncOperation *op = (AsyncOperation *)oper;

    if (op == NULL || op->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Wrong handle or state for write, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        return;
    }
    else if (op->fileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): File was not opened, check previous errors, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, (int)op->signal, op->instance, 
            SIS_ASIO_RESULT_FILE_NOT_FOUND, op->totalLen);
        return;
    } 

    
    op->totalLen = 0;
    op->pos = buffer;
    op->buffer = buffer;
    op->dataLen = bufSize;

     
    if (op->mode != SIS_ASYNC_MODE_APPEND)
    {
        doSeek(op, startPos);
    }

    doWrite(op);
} 










void slsAsyncWriteContinue(SlsAsyncOperationHandle oper, WE_UINT32 bufSize)
{
    AsyncOperation *op = (AsyncOperation *)oper;

    if (op == NULL || op->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Wrong handle or state for writeContinue, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        return;
    }
    else if (op->fileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): File was not opened, check previous errors, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, (int)op->signal, op->instance, 
            SIS_ASIO_RESULT_FILE_NOT_FOUND, op->totalLen);
        return;
    } 

    op->dataLen = bufSize;
    doWrite(op);
} 






static void doRead(AsyncOperation *op)
{
    int ret;
    long length;

    length = op->buffer - op->pos + (long)op->dataLen;

    
    for (;;)
    {
        if (length <= 0) 
        {
            (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, (int)op->signal, op->instance, 
                SIS_ASIO_RESULT_OK, op->totalLen);
            return;
        } 

        ret = WE_FILE_READ( op->fileHandle, op->pos, length);
        switch (ret)
        {
        case 0 :
        case WE_FILE_ERROR_EOF :
            (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, (int)op->signal, op->instance, 
                SIS_ASIO_RESULT_EOF, op->totalLen);
            return;

        case WE_FILE_ERROR_ACCESS :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                "%s(%d): Read returned %d\n", __FILE__, __LINE__, ret));

            (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, (int)op->signal, op->instance, 
                translateFileError(ret), op->totalLen);
            return;

        case WE_FILE_ERROR_DELAYED :
            WE_FILE_SELECT( op->fileHandle, WE_FILE_EVENT_READ);
            return;

        default :
            if (ret < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                    "%s(%d): Unhandled file error %d\n",
                    __FILE__, __LINE__, ret));
                (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, (int)op->signal, 
                    op->instance, translateFileError(ret), op->totalLen);

                return; 
            } 

            op->totalLen += (WE_UINT32)ret;
            op->pos += ret;
            length -= ret;
            break;
        } 
    } 
} 







static void doSeek(const AsyncOperation *op, WE_INT32 startPos)
{
    int seekMode;
    long seekOffset;
    long ret;

    if (op == NULL) 
    {
        return;
    }

    if (SIS_ASYNC_MODE_APPEND == op->mode) 
    {
        
        return;
    }
    if (startPos == -1)
    {
        seekMode = WE_FILE_SEEK_END;
        seekOffset = 0;
    }
    else
    {
        seekMode = WE_FILE_SEEK_SET;
        seekOffset = startPos;
    } 

    if ((ret = WE_FILE_SEEK( op->fileHandle, seekOffset, seekMode)) < 0)
    {
        (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, (int)op->signal, op->instance, 
            translateFileError(ret), op->totalLen);

        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_SIS, 
            "%s(%d): Failed to seek file %d, mode=%d, pos=%d, ret=%d\n", 
            __FILE__, __LINE__, op->fileHandle, seekMode, seekOffset, ret));

        return;
    } 
} 






static void doWrite(AsyncOperation *op)
{
    int ret;
    int length;

    length = op->buffer - op->pos + (long)op->dataLen;

    
    for (;;)
    {
        if (length <= 0)    
        {
            (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, (int)op->signal, op->instance, 
                SIS_ASIO_RESULT_OK, op->totalLen);
            return;
        } 

        ret = WE_FILE_WRITE( op->fileHandle, op->pos, length);
        switch (ret)
        {
        case 0 :
        case WE_FILE_ERROR_EOF :
        case WE_FILE_ERROR_ACCESS :
        case WE_FILE_ERROR_FULL :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                "%s(%d): Write returned %d\n", __FILE__, __LINE__, ret));

            (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, (int)op->signal, op->instance, 
                translateFileError(ret), op->totalLen);
            return;

        case WE_FILE_ERROR_DELAYED :
            WE_FILE_SELECT( op->fileHandle, WE_FILE_EVENT_WRITE);
            return;

        default :
            if (ret < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                    "%s(%d): Unhandled file error %d\n",
                    __FILE__, __LINE__, ret));
                (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, (int)op->signal, 
                    op->instance, translateFileError(ret), op->totalLen);

                return; 
            } 

            op->totalLen += (unsigned long)ret;
            op->pos += ret;
            length -= ret;
            break;
        } 
    } 
} 







static void handleNotify(int fileHandle, int operationType)
{
    AsyncOperation *anOper = myOperationPool;

    while (anOper != NULL)
    {   
        if (anOper->fileHandle == fileHandle &&
            anOper->mode == SIS_ASYNC_MODE_DISPATCH)
        {
            switch (operationType)
            {
            case WE_FILE_READ_READY :
            case WE_FILE_WRITE_READY :
                (void)SIS_SIGNAL_SENDTO_IU(anOper->fsm, (int)anOper->signal, 
                    anOper->instance, SIS_ASIO_RESULT_OK); 
                slsAsyncDeregister(fileHandle);
                break;
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                    "%s(%d): Illegal operationType %d\n", __FILE__, __LINE__,
                    operationType));
                break;
            } 

            return;
        }

        if (anOper->state == OPER_STATE_OPENED && 
            anOper->fileHandle == fileHandle)
        {
            switch (operationType)
            {
            case WE_FILE_READ_READY :
                slsAsyncReadContinue(anOper, anOper->dataLen);
                break;
            case WE_FILE_WRITE_READY :
                slsAsyncWriteContinue(anOper, anOper->dataLen);
                break;
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                    "%s(%d): Illegal operationType %d\n", __FILE__, __LINE__,
                    operationType));
                break;
            } 

            return;
        } 

        anOper = anOper->next;
    } 
    
    if (anOper == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Couldn't find handle\n", __FILE__, __LINE__));
    } 
} 




void slsAsyncTerminate(void)
{
    AsyncOperation *anOper = myOperationPool;
    AsyncOperation *next = NULL;

    myOperationPool = NULL;
    myPoolSize = 0;
    
    while (anOper != NULL)
    {
        next = anOper->next;
        SIS_FREE(anOper);
        anOper = next;
    } 

    slsSignalDeregister(SIS_FSM_ASYNC);
} 




static AsyncOperation *slsAsyncGetNewInstance(void)
{
    AsyncOperation *anOper = myOperationPool;
    AsyncOperation *last = NULL;

    
    while (anOper != NULL && anOper->state != OPER_STATE_FREE)
    {
        last = anOper;
        anOper = anOper->next;
    } 

    
    if (anOper == NULL)
    {
        if ((anOper = SIS_ALLOC(sizeof(AsyncOperation))) == NULL)
        {
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                "SIS FSM Async: No free async operations\n"));

            return NULL;
        } 

        if (NULL != last)
        {
            last->next = anOper;
        }
        anOper->previous = last;
        anOper->next = NULL;
        ++myPoolSize;
    } 

    



    anOper->fileHandle = 0;
    anOper->pipeHandle = 0;

    return anOper;
}















SlsAsyncOperationHandle slsAsyncPipeOperationStart(const char *pipeName, 
    SlsStateMachine fsm, long instance, WE_UINT32 signal, SlsAsyncMode mode)
{
    AsyncOperation *anOper = myOperationPool;

    
    anOper = slsAsyncGetNewInstance();
    if (NULL == anOper)
    {
        return NULL;
    }

    switch (mode)
    {
    case SIS_ASYNC_MODE_READ :
        anOper->pipeHandle = WE_PIPE_OPEN(WE_MODID_SIS, pipeName);
        break;
    case SIS_ASYNC_MODE_WRITE :
        anOper->pipeHandle = WE_PIPE_CREATE(WE_MODID_SIS, pipeName);
        break;
    case SIS_ASYNC_MODE_APPEND:
    case SIS_ASYNC_MODE_READ_WRITE:
    case SIS_ASYNC_MODE_DISPATCH:
    default :
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIS, 
            "%s(%d): Illegal pipe mode %d\n", __FILE__, __LINE__, mode));

        break;
    } 

    if (anOper->pipeHandle < 0) 
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_SIS, 
            "(%s) (%d): Failed to open/create pipe <%s>, %d\n", __FILE__, __LINE__, 
            pipeName == NULL ? "NULL" : pipeName, anOper->pipeHandle));
        return NULL;
    } 

    anOper->fsm = fsm;
    anOper->mode = mode;
    anOper->pipeName = we_cmmn_strdup(WE_MODID_SIS, pipeName);
    anOper->instance = instance;
    anOper->signal = signal;
    anOper->fileHandle = 0;
    anOper->state = OPER_STATE_OPENED;
    anOper->buffer = NULL;
    anOper->dataLen = 0;

    return anOper;
}







static void handlePipeNotify(int pipeHandle, int operationType)
{
    AsyncOperation *anOper = myOperationPool;

    while (anOper != NULL)
    {   
        if (anOper->state == OPER_STATE_OPENED && 
            anOper->pipeHandle == pipeHandle)
        {
            switch (operationType)
            {
            case WE_PIPE_EVENT_READ:
                doPipeRead(anOper);
                break;
            case WE_PIPE_EVENT_WRITE:
                doPipeWrite(anOper);
                break;
            case WE_PIPE_EVENT_CLOSED:
                (void)SIS_SIGNAL_SENDTO_IUU(anOper->fsm, anOper->signal, 
                    anOper->instance, SIS_ASIO_RESULT_EOF, anOper->totalLen);
                break;
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                    "(%s) (%d): Illegal operationType %d\n", __FILE__, __LINE__,
                    operationType));
                break;
            } 

            return;
        } 

        anOper = anOper->next;
    } 

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
        "%s(%d): Couldn't find handle\n", __FILE__, __LINE__));
} 






static SlsAsioResult translateFileError(int fileError)
{
    SlsAsioResult ret = SIS_ASIO_RESULT_ERROR;

    switch (fileError)
    {
    case WE_FILE_OK :
        ret = SIS_ASIO_RESULT_OK;
        break;
    case WE_FILE_ERROR_ACCESS : 
        ret = SIS_ASIO_RESULT_ERROR;
        break;
    case WE_FILE_ERROR_DELAYED :
        ret = SIS_ASIO_RESULT_DELAYED;
        break;
    case WE_FILE_ERROR_PATH :
        ret = SIS_ASIO_RESULT_FILE_NOT_FOUND;
        break;
    case WE_FILE_ERROR_INVALID : 
        ret = SIS_ASIO_RESULT_ERROR;
        break;
    case WE_FILE_ERROR_SIZE :
        ret = SIS_ASIO_RESULT_INSUFFICIENT_PERSISTENT_STORAGE;
        break;
    case WE_FILE_ERROR_FULL :
        ret = SIS_ASIO_RESULT_INSUFFICIENT_PERSISTENT_STORAGE;
        break;
    case WE_FILE_ERROR_EOF :
        ret = SIS_ASIO_RESULT_EOF;
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Unknown fileError %d\n", __FILE__, __LINE__,
            fileError));
        break;
    } 

    return ret;
} 






static SlsAsioResult translatePipeError(int pipeError)
{
    SlsAsioResult ret = SIS_ASIO_RESULT_ERROR;

    switch (pipeError)
    {
    case WE_PIPE_ERROR_BAD_HANDLE :
    case WE_PIPE_ERROR_INVALID_PARAM :
    case WE_PIPE_ERROR_EXISTS :
    case WE_PIPE_ERROR_IS_OPEN :
    case WE_PIPE_ERROR_RESOURCE_LIMIT :
    case WE_PIPE_ERROR_NOT_FOUND :
        ret = SIS_ASIO_RESULT_ERROR;
        break;
    case WE_PIPE_ERROR_DELAYED :
        ret = SIS_ASIO_RESULT_DELAYED;
        break;
    case WE_PIPE_ERROR_CLOSED :
        ret = SIS_ASIO_RESULT_EOF;
        break;
    default :
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Unknown pipeError %d\n", __FILE__, __LINE__,
            pipeError));

        break;
    } 

    return ret;
} 







static void doPipeWrite(AsyncOperation *op)
{
    long ret;
    long length;

    length = op->buffer - op->pos + (long)op->dataLen;

    
    for (;;)
    {
        if (length <= 0)    
        {
            (void)SIS_SIGNAL_SENDTO_IUU( op->fsm, op->signal, op->instance, 
                MMS_RESULT_OK, op->totalLen);
            return;
        } 

        ret = WE_PIPE_WRITE( op->pipeHandle, op->pos, length);
        switch (ret)
        {
        case WE_PIPE_ERROR_CLOSED :
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                "%s(%d): Pipe write returned %d\n", __FILE__, __LINE__, ret));

            (void)SIS_SIGNAL_SENDTO_IUU( op->fsm, op->signal, op->instance, 
                translatePipeError(ret), op->totalLen);
            return;

        case 0 :
        case WE_PIPE_ERROR_DELAYED :
            (void)WE_PIPE_POLL(op->pipeHandle);
            return;

        default :
            if (ret < 0)
            {
                WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS,
                    "%s(%d): Unhandled pipe error %d\n",
                    __FILE__, __LINE__, ret));
                (void)SIS_SIGNAL_SENDTO_IUU( op->fsm, op->signal, op->instance, 
                    translatePipeError(ret), op->totalLen);

                return; 
            } 

            op->totalLen += (WE_UINT32) ret;
            op->pos += ret;
            length -= ret;
            break;
        } 
    } 
}






static void doPipeRead(AsyncOperation *op)
{
    int ret;
    long length;

    length = op->buffer - op->pos + (long)op->dataLen;

    
    for (;;)
    {
        if (length <= 0) 
        {
            (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
                SIS_ASIO_RESULT_OK, op->totalLen);
            return;
        } 

        ret = WE_PIPE_READ( op->pipeHandle, op->pos, length);
        switch (ret)
        {
        case WE_PIPE_ERROR_CLOSED :
            (void)SIS_SIGNAL_SENDTO_IUU( op->fsm, op->signal, op->instance, 
                SIS_ASIO_RESULT_EOF, op->totalLen);
            return;

        case WE_PIPE_ERROR_NOT_FOUND :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                "%s(%d): Pipe read returned %d\n", __FILE__, __LINE__, ret));

            (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
                translatePipeError(ret), op->totalLen);
            return;

        case 0 :
        case WE_PIPE_ERROR_DELAYED :
            (void)WE_PIPE_POLL(op->pipeHandle);
            return;

        default :
            if (ret < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
                    "%s(%d): Unhandled pipe error %d\n",
                    __FILE__, __LINE__, ret));
                (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
                    translatePipeError(ret), op->totalLen);

                return; 
            } 

            op->totalLen += (WE_UINT32)ret;
            op->pos += ret;
            length -= ret;
            break;
        } 
    } 
}







void slsAsyncPipeRead(SlsAsyncOperationHandle oper, void *buffer, 
    WE_UINT32 bufferSize)
{
    AsyncOperation *op = (AsyncOperation *)oper;

    if (op == NULL || op->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Wrong handle or state for pipe read, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        return;
    }
    else if (op->pipeHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Pipe was not opened, check previous errors, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
            SIS_ASIO_RESULT_ERROR, op->totalLen);
        return;
    } 

    
    op->totalLen = 0;
    op->pos = buffer;
    op->buffer = buffer;
    op->dataLen = bufferSize;

    doPipeRead(op);
}







void slsAsyncPipeWrite(SlsAsyncOperationHandle oper, void *buffer, 
    WE_UINT32 bufferSize)
{
    AsyncOperation *op = (AsyncOperation *)oper;

    if (op == NULL || op->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Wrong handle or state for pipe write, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        return;
    }
    else if (op->pipeHandle < 0)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIS, 
            "%s(%d): Pipe was not opened, check previous errors, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        (void)SIS_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
            SIS_ASIO_RESULT_ERROR, op->totalLen);
        return;
    } 

    
    op->totalLen = 0;
    op->pos = buffer;
    op->buffer = buffer;
    op->dataLen = bufferSize;

    doPipeWrite(op);
}

