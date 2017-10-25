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

/*! \file Msa_Async.h
 * \brief Definitions needed for the Asynchronous File API wrapping state 
 *        machine.
 */

#ifndef MSA_ASYNC_H
#define MSA_ASYNC_H

/*--- Definitions/Declarations ---*/

/*--- Types ---*/
typedef void* MsaAsyncOperationHandle;

/*! \enum MsaAsyncMode
 *  Read-write mode for an asynchronous operation.
 */
typedef enum
{
    MSA_ASYNC_MODE_READ,
    MSA_ASYNC_MODE_WRITE,
    MSA_ASYNC_MODE_APPEND,
    MSA_ASYNC_MODE_READ_WRITE,
    MSA_ASYNC_MODE_DISPATCH
} MsaAsyncMode;

/**********************************************************************
 * Here is signals handled by the Async FSM.
 *
 * The values in this enum corresponds to the second parameter in the
 * MSA_SIGNAL_SENDTO macros.
 **********************************************************************/
/*!\enum MsaAsyncSignalId Signals used with the Async FSM */
typedef enum
{
    /*! Call to WE_FILE_SELECT has triggered an event
     *  - Param I : eventType (WE_FILE_READ_READY or WE_FILE_WRITE_READY)
     *  - Param U1: fileHandle that the event is related to
     *  - Param U2: -
     *  - Param P : -
     */
    MSA_SIG_ASYNC_NOTIFY,

    /*! Call to WE_PIPE_SELECT has trigged an event
     *  - Param I : eventType
     *  - Param U1: pipeHandle that the event is related to
     *  - Param U2: -
     *  - Param P : -
     */
    MMS_SIG_ASYNC_PIPE_NOTIFY
}MsaAsyncSignalId;

/*!\enum MsaAsioResult Result codes from ASIO FSM */
typedef enum
{
    MSA_ASIO_RESULT_OK,
    MSA_ASIO_RESULT_EOF,
    MSA_ASIO_RESULT_ERROR,
    MSA_ASIO_RESULT_DELAYED,
    MSA_ASIO_RESULT_FILE_NOT_FOUND,
    MSA_ASIO_RESULT_INSUFFICIENT_PERSISTENT_STORAGE
}MsaAsioResult;

/*--- Constants ---*/

/*! 
 *  This define is used to determine the size of the buffer used when
 *  performing a Async IO copy between two files 
 */
#define MSA_ASIO_COPY_BUFFER_SIZE 1024

/*--- Forwards ---*/

/*--- Externs ---*/

/*--- Macros ---*/

/*--- Prototypes ---*/

/*
 *	Init/clenaup.
 */
WE_BOOL Msa_AsyncInit(void);
void Msa_AsyncTerminate(void);

MsaAsyncOperationHandle Msa_AsyncOperationStart(const char *pcFileName, 
    MsaStateMachine eFsm, long lInstance, WE_UINT32 uiSignal, MsaAsyncMode eMode);
void Msa_AsyncOperationStop(MsaAsyncOperationHandle *pvOper);

/*
 *	File operations
 */
void Msa_AsyncRead(MsaAsyncOperationHandle pvOper, WE_INT32 iStartPos,
    void *pvBuffer, WE_UINT32 uiBufSize);
void Msa_AsyncReadContinue(MsaAsyncOperationHandle pvOper, WE_UINT32 uiBufSize);
void Msa_AsyncWrite(MsaAsyncOperationHandle pvOper, WE_INT32 iStartPos, 
    void *pvBuffer, WE_UINT32 uiBufSize);
void Msa_AsyncWriteContinue(MsaAsyncOperationHandle pvOper, WE_UINT32 bufSize);

/*
 * Subscribe to file notifications
 */
void Msa_AsyncDeregister(int iFileHandle);
void Msa_AsyncRegister(int iFileHandle, int iFsm, int iSignal, int iInstance);

/*
 *	Async pipe operations using buffer.
 */
MsaAsyncOperationHandle Msa_AsyncPipeOperationStart(const char *pcPipeName, 
    MsaStateMachine eFsm, long lInstance, WE_UINT32 uiSignal, MsaAsyncMode eMode);
void Msa_AsyncPipeWrite(MsaAsyncOperationHandle oper, void *buffer, 
    WE_UINT32 bufSize);
void Msa_AsyncPipeRead(MsaAsyncOperationHandle pvOper, void *pvBuffer, 
    WE_UINT32 uiBufferSize);

#endif /* _MAASYNC_H */
