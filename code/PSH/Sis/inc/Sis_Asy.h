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






#ifndef SIS_ASY_H
#define SIS_ASY_H




typedef void* SlsAsyncOperationHandle;




typedef enum
{
    SIS_ASYNC_MODE_READ,
    SIS_ASYNC_MODE_WRITE,
    SIS_ASYNC_MODE_APPEND,
    SIS_ASYNC_MODE_READ_WRITE,
    SIS_ASYNC_MODE_DISPATCH
} SlsAsyncMode;








typedef enum
{
    





    SIS_SIG_ASYNC_NOTIFY,

    





    SIS_SIG_ASYNC_PIPE_NOTIFY
}SlsAsyncSignalId;


typedef enum
{
    SIS_ASIO_RESULT_OK,
    SIS_ASIO_RESULT_EOF,
    SIS_ASIO_RESULT_ERROR,
    SIS_ASIO_RESULT_DELAYED,
    SIS_ASIO_RESULT_FILE_NOT_FOUND,
    SIS_ASIO_RESULT_INSUFFICIENT_PERSISTENT_STORAGE
}SlsAsioResult;







#define SIS_ASIO_COPY_BUFFER_SIZE 1024












WE_BOOL slsAsyncInit(void);
void slsAsyncTerminate(void);

SlsAsyncOperationHandle slsAsyncOperationStart(const char *fileName, 
    SlsStateMachine fsm, long instance, WE_UINT32 signal, SlsAsyncMode mode);
void slsAsyncOperationStop(SlsAsyncOperationHandle *oper);




void slsAsyncRead(SlsAsyncOperationHandle oper, WE_INT32 startPos,
    void *buffer, WE_UINT32 bufSize);
void slsAsyncReadContinue(SlsAsyncOperationHandle oper, WE_UINT32 bufSize);
void slsAsyncWrite(SlsAsyncOperationHandle oper, WE_INT32 startPos, 
    void *buffer, WE_UINT32 bufSize);
void slsAsyncWriteContinue(SlsAsyncOperationHandle oper, WE_UINT32 bufSize);




void slsAsyncDeregister(int fileHandle);
void slsAsyncRegister(int fileHandle, int fsm, int signal, int instance);




SlsAsyncOperationHandle slsAsyncPipeOperationStart(const char *pipeName, 
    SlsStateMachine fsm, long instance, WE_UINT32 signal, SlsAsyncMode mode);
void slsAsyncPipeWrite(SlsAsyncOperationHandle oper, void *buffer, 
    WE_UINT32 bufSize);
void slsAsyncPipeRead(SlsAsyncOperationHandle oper, void *buffer, 
    WE_UINT32 bufSize);

#endif 
