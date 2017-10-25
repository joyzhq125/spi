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
#ifndef MASYNC_H
#define MASYNC_H
typedef void* AsyncOperationHandle;
typedef enum
{
    ASYNC_MODE_READ,
    ASYNC_MODE_WRITE,
    ASYNC_MODE_READ_WRITE,
    ASYNC_MODE_APPEND

} AsyncMode;
typedef enum
{
    MMS_SIG_ASYNC_FILE_NOTIFY,
    MMS_SIG_ASYNC_PIPE_NOTIFY,
    MMS_SIG_ASYNC_INT_TERMINATE
} MmsAsyncSignalId;
void asyncEmergencyAbort(void);
WE_BOOL asyncInit(void);
void asyncTerminate
(
	MmsStateMachine eFsm,
	WE_UINT32 uiSignal,
	long lInstance
);

AsyncOperationHandle asyncOperationStart
(
	const char *pcFileName, 
	MmsStateMachine eFsm,
	long lInstance,
	WE_UINT32 uiSignal,
	AsyncMode eMode
);
void asyncOperationStop(AsyncOperationHandle *ppOper);
void asyncRead
(
	AsyncOperationHandle pOper,
	WE_INT32 iStartPos,
   	void *pBuffer,
   	WE_UINT32 uiBufSize
);
void asyncReadContinue
(
	AsyncOperationHandle pOper,
	WE_UINT32 uiBufSize
);

void asyncWrite
(
	AsyncOperationHandle pOper,
	WE_INT32 iStartPos,
	void *pBuffer, 
    	WE_UINT32 uiBufSize
);
void asyncWriteContinue
(
	AsyncOperationHandle pOper,
	WE_UINT32 uiBufSize
);
WE_BOOL asyncIsPipeOwner(int iPipeHandle);

AsyncOperationHandle asyncPipeOperationStart
(
	const char *pcPipeName, 
    	MmsStateMachine eFsm,
    	long lInstance,
    	WE_UINT32 uiSignal,
    	AsyncMode eMode
);

void asyncPipeWrite
(
	AsyncOperationHandle pOper,
       void *pBuffer,
       WE_UINT32 uiBufSize
);
void asyncPipeRead
(
	AsyncOperationHandle pOper,
       void *pBuffer,
       WE_UINT32 uiBufSize
);
#endif 
