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

/*! \file maasync.h
 * \brief Definitions needed for the Asynchronous File API wrapping state 
 *        machine.
 */

#ifndef SAASYNC_H
#define SAASYNC_H

/*--- Definitions/Declarations ---*/

/*--- Types ---*/
typedef void* SiaAsyncOperationHandle;

/*! \enum SiaAsyncMode
 *  Read-write mode for an asynchronous operation.
 */
typedef enum
{
    SIA_ASYNC_MODE_READ,
    SIA_ASYNC_MODE_WRITE,
    SIA_ASYNC_MODE_APPEND,
    SIA_ASYNC_MODE_READ_WRITE,
    SIA_ASYNC_MODE_DISPATCH
} SiaAsyncMode;

/**********************************************************************
 * Here is signals handled by the Async FSM.
 *
 * The values in this enum corresponds to the second parameter in the
 * SIA_SIGNAL_SENDTO macros.
 **********************************************************************/
/*!\enum SiaAsyncSignalId Signals used with the Async FSM */
typedef enum
{
    /*! Call to WE_FILE_SELECT has triggered an event
     *  - Param I : eventType (WE_FILE_READ_READY or WE_FILE_WRITE_READY)
     *  - Param U1: fileHandle that the event is related to
     *  - Param U2: -
     *  - Param P : -
     */
    SIA_SIG_ASYNC_NOTIFY,

    /*! Call to WE_PIPE_SELECT has trigged an event
     *  - Param I : eventType
     *  - Param U1: pipeHandle that the event is related to
     *  - Param U2: -
     *  - Param P : -
     */
    MMS_SIG_ASYNC_PIPE_NOTIFY
}SiaAsyncSignalId;

/*!\enum SiaAsioResult Result codes from ASIO FSM */
typedef enum
{
    SIA_ASIO_RESULT_OK,
    SIA_ASIO_RESULT_EOF,
    SIA_ASIO_RESULT_ERROR,
    SIA_ASIO_RESULT_DELAYED,
    SIA_ASIO_RESULT_FILE_NOT_FOUND,
    SIA_ASIO_RESULT_INSUFFICIENT_PERSISTENT_STORAGE
}SiaAsioResult;

/*--- Constants ---*/

/*! 
 *  This define is used to determine the size of the buffer used when
 *  performing a Async IO copy between two files 
 */
#define SIA_ASIO_COPY_BUFFER_SIZE 1024

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Prototypes ---*/

/*
 *	Init/clenaup.
 */
WE_BOOL smaAsyncInit(void);
void smaAsyncTerminate(void);

SiaAsyncOperationHandle smaAsyncOperationStart(const char *fileName, 
    SiaStateMachine fsm, long instance, int signal, SiaAsyncMode mode);
void smaAsyncOperationStop(SiaAsyncOperationHandle *oper);

/*
 *	File operations
 */
void smaAsyncRead(SiaAsyncOperationHandle oper, WE_INT32 startPos,
    void *buffer, WE_UINT32 bufSize);
void smaAsyncReadContinue(SiaAsyncOperationHandle oper, WE_UINT32 bufSize);
void smaAsyncWrite(SiaAsyncOperationHandle oper, WE_INT32 startPos, 
    void *buffer, WE_UINT32 bufSize);
void smaAsyncWriteContinue(SiaAsyncOperationHandle oper, WE_UINT32 bufSize);

/*
 * Subscribe to file notifications
 */
void smaAsyncDeregister(int fileHandle);
void smaAsyncRegister(int fileHandle, int fsm, int signal, int instance);

/*
 *	Async pipe operations using buffer.
 */
SiaAsyncOperationHandle smaAsyncPipeOperationStart(const char *pipeName, 
    SiaStateMachine fsm, long instance, int signal, SiaAsyncMode mode);
void smaAsyncPipeWrite(SiaAsyncOperationHandle oper, void *buffer, 
    WE_UINT32 bufSize);
void smaAsyncPipeRead(SiaAsyncOperationHandle oper, void *buffer, 
    WE_UINT32 bufSize);

#endif /* _MAASYNC_H */
