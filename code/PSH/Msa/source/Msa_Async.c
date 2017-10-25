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

/*! \file Msa_Async.c
 * \brief Implementation of the state machine wrapping the asynchronous File 
 *  API. The reason why we need this state machine is because WE return a
 *  signal as a response to SELECT and the only way of identifying what 
 *  operation it is related to is the file handle and hence we must keep all
 *  open fileHandles in one place.
 */

/*--- Include files ---*/
#include "We_Lib.h"    /* WE: ANSI Standard libs allowed to use */
#include "We_Cfg.h"    /* WE: Module definition */
#include "We_Def.h"    /* WE: Global definitions */ 
#include "We_Log.h"    /* WE: Signal logging */ 
#include "We_Mem.h"    /* WE: Memory handling */
#include "We_Core.h"   /* WE: WE macros, signals and types */
#include "We_File.h"   /* WE: WE File API */ 
#include "We_Pipe.h"

#include "Mms_Def.h"

#include "Msa_Intsig.h"   /* MMS: Signal handling functions */
#include "Msa_Mem.h"      /* MMS: Memory handling functions */
#include "Msa_Async.h"     /* MMS: Async file handling functions */
#include "Msa_Core.h"

/*--- Definitions/Declarations ---*/
#define ASYNC_POOL_SIZE 6

/*--- Types ---*/

/*! \enum OperState
 *  Used to indicate the different states an operation can be in.
 */
typedef enum
{
    OPER_STATE_FREE = 0,    /*!< The asynchronous operation is not used. */
    OPER_STATE_OPENED       /*!< The file is opened */
} OperState;

/*! \struct AsyncOperation 
 *  Data needed to handle asynchronous file operations from other state 
    machines */
typedef struct AsyncOper
{   /* Double linked list */
    struct AsyncOper *next; /*!< Next asynchronous operation in queue */
    struct AsyncOper *previous; /*!< Previous asynchronous operation in 
                                     queue */
    OperState state;        /*!< State for the file in question */
    MsaAsyncMode mode;      /*!< The type of operation see #MsaAsyncMode */

    MsaStateMachine fsm;    /*!< FSM originally ordering the operation */
    long instance;          /*!< FSM instance */
    WE_UINT32 signal;      /*!< Response signal WID */

    int fileHandle;         /*!< File handle as returned from WE */
    WE_UINT32 totalLen;    /*!< Total number of bytes read/written */
    unsigned char *pos;     /*!< Current buffer position for reading/writing */
    unsigned char *buffer;  /*!< Buffer to read/write to/from */
    WE_UINT32 dataLen;     /*!< Length of data to read/write to/from current 
                             *   buffer. */

    char *pipeName;         /*!< Pipe name to operate on */
    int pipeHandle;         /*!< Pipe handle as returned from WE */
} AsyncOperation;

/*--- Constants ---*/ 

/*--- Forwards ---*/ 

/*--- Externs ---*/ 

/*--- Macros ---*/ 

/*--- Global variables ---*/ 

/*--- Static variables ---*/ 
/*lint -esym(956,myOperations,myPoolSize,myCaller) */
/* Non const, non volatile static or external variable. Not a problem since 
 * we're not running anything multithreaded. */

/*! List with all ongoing file operations. */
static AsyncOperation *myOperationPool;
static int myPoolSize;

/*--- Prototypes ---*/ 
static void Msa_asyncMain(MsaSignal *pstSig);
static void Msa_DoRead(AsyncOperation *pstOp);
static void Msa_DoSeek(const AsyncOperation *pstOp, WE_INT32 iStartPos);
static void Msa_DoWrite(AsyncOperation *pstOp);
static void Msa_HandleNotify(int iFileHandle, int iOperationType);
static MsaAsioResult Msa_TranslateFileError(int iFileError);
static void Msa_DoPipeRead(AsyncOperation *pstOp);
static void Msa_DoPipeWrite(AsyncOperation *pstOp);
static void Msa_HandlePipeNotify(int iPipeHandle, int iOperationType);
static AsyncOperation *Msa_AsyncGetNewInstance(void);

/******************************************************************************/

/*!
 * \brief Initialize the Async File API FSM.
 * \return TRUE if succesful, FALSE otherwise
 *****************************************************************************/
WE_BOOL Msa_AsyncInit(void)
{
    int iIndex = 0;
    AsyncOperation *pstPrevious = NULL;
    AsyncOperation *pstANew = NULL;

    msaSignalRegisterDst(MSA_ASYNC_FSM, Msa_asyncMain);
    
    myOperationPool = NULL;
    myPoolSize = 0;
    for (iIndex = 0; iIndex < ASYNC_POOL_SIZE; ++iIndex)
    {
        /* Do not use exception during initiation */
        if (NULL == (pstANew = MSA_ALLOC_NE(sizeof(AsyncOperation))))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                        WE_MODID_MSA,
                                        "%s(%d): Insufficient memory during initialization\n", 
                                        __FILE__, 
                                        __LINE__));
            return FALSE;
        } /* if */

        memset( pstANew, 0, sizeof(AsyncOperation));
        pstANew->state = OPER_STATE_FREE;
        if (NULL == myOperationPool)
        {
            myOperationPool = pstANew;
            myOperationPool->next = NULL;
            myOperationPool->previous = NULL;
            pstPrevious = pstANew;
        }
        else 
        {
            if (NULL != pstPrevious)
            {
                pstPrevious->next = pstANew;
            } /* if */

            pstANew->previous = pstPrevious;
            pstANew->next = NULL;
            pstPrevious = pstANew;
        } /* if */
    } /* for */

    myPoolSize = iIndex;

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, 
                                WE_MODID_MSA,
                                "MMS FSM Async: initialized\n"));

    return TRUE;
} /* asyncInit */

/*!
 * \brief Main function for Async File API FSM
 * 
 * \param sig Incoming signal to the FSM
 *****************************************************************************/
static void Msa_asyncMain(MsaSignal *pstSig)
{
    switch (pstSig->type)
    {
    case MSA_SIG_ASYNC_NOTIFY :
        Msa_HandleNotify( (int)pstSig->u_param1, pstSig->i_param);
        break;
    case MMS_SIG_ASYNC_PIPE_NOTIFY :
        Msa_HandlePipeNotify( (int)pstSig->u_param1, pstSig->i_param);
        break;
    default:
        /* Unknown signal: just ignore. */
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, 
                                    WE_MODID_MSA, 
                                    "%s(%d): received unknown signal %d\n",
                                    __FILE__, 
                                    __LINE__,
                                    pstSig->type));
        break;
    } /* switch */
    
    msaSignalDelete(pstSig);
} /* Msa_asyncMain */

/*!
 * \brief Start a new asynchronous file operation
 *
 *  Whenever an asynchronous file operations is finished or shall be aborted, 
 *  it must be stopped with a call to #asyncOperationStop.
 *
 * \param fileName  Name of file operating on. 
 * \param fsm       FSM ordering the operation
 * \param instance  FSM instance ordering the operation
 * \param signal    FSMs requested response signal
 * \param mode      Read, write or both?
 * \return AsyncOperationHandle, i.e. ptr to item in array with operations,
 *         NULL if no free
 *****************************************************************************/
MsaAsyncOperationHandle Msa_AsyncOperationStart
(
    const char *pcFileName, 
    MsaStateMachine eFsm,
    long lInstance,
    WE_UINT32 uiSignal,
    MsaAsyncMode eMode
)
{
    AsyncOperation *pstAnOper = myOperationPool;
    AsyncOperation *pstLast = NULL;
    int iFileHandle;
    int iOpenMode;

    /* Search a free operation */
    while (NULL != pstAnOper && OPER_STATE_FREE != pstAnOper->state)
    {
        pstLast = pstAnOper;
        pstAnOper = pstAnOper->next;
    } /* for */

    /* If we didn't we find a free then increase the queue */
    if (NULL == pstAnOper)
    {
        if (NULL == (pstAnOper = MSA_ALLOC(sizeof(AsyncOperation))))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                        WE_MODID_MSA, 
                                        "MMS FSM Async: No free async operations\n"));

            return NULL;
        } /* if */
        /*lint -e{794} */
        pstLast->next = pstAnOper;
        pstAnOper->previous = pstLast;
        pstAnOper->next = NULL;
        ++myPoolSize;
    } /* if */
    /*lint -e{788} */
    switch (eMode)
    {
    case MSA_ASYNC_MODE_READ :
        iOpenMode = WE_FILE_SET_RDONLY;
        break;
    case MSA_ASYNC_MODE_WRITE :
        iOpenMode = WE_FILE_SET_WRONLY | WE_FILE_SET_CREATE;
        break;
    case MSA_ASYNC_MODE_APPEND :
        iOpenMode = WE_FILE_SET_WRONLY | WE_FILE_SET_CREATE |
            WE_FILE_SET_APPEND;
        break;
    case MSA_ASYNC_MODE_READ_WRITE :
        /* NOTE! We cannot use WE_FILE_SET_APPEND here since seek is not
         * allowed when using this flag */
        iOpenMode = WE_FILE_SET_RDWR | WE_FILE_SET_CREATE;
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, 
                                    WE_MODID_MSA, 
                                    "%s(%d): Illegal open mode %d\n",
                                    __FILE__, 
                                    __LINE__,
                                    eMode));

        iOpenMode = WE_FILE_SET_RDWR | WE_FILE_SET_CREATE;
        break;
    } /* switch */

    iFileHandle = WE_FILE_OPEN(WE_MODID_MSA, pcFileName, iOpenMode, 0);

    if (iFileHandle < 0) /* i.e. ERROR! */
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, 
                                    WE_MODID_MSA,
                                    "%s(%d): Failed to open file <%s>, %d\n",
                                    __FILE__, 
                                    __LINE__, 
                                    NULL ==  pcFileName? "NULL" : pcFileName,
                                    iFileHandle));
    } /* if */

    pstAnOper->fsm = eFsm;
    pstAnOper->instance = lInstance;
    pstAnOper->signal = uiSignal;
    pstAnOper->fileHandle = iFileHandle;
    pstAnOper->state = OPER_STATE_OPENED;
    pstAnOper->buffer = NULL;
    pstAnOper->dataLen = 0;
    pstAnOper->mode = eMode;

    return pstAnOper;
} /* asyncOperationStart */

/*!
 * \brief Register a delayed file operation, when the file operation can be
 *        executed again a signal will be sent to the registered FSM.
 *
 * \param fileHandle
 * \param fsm The fsm to send a signal to when a file operation can be executed 
 *            again.
 * \param instance FSM instance ordering the operation
 * \param signal FSMs requested response signal.
 *****************************************************************************/
void Msa_AsyncRegister
(
    int iFileHandle,
    int iFsm,
    int iSignal,
    int iInstance
)
{
    AsyncOperation *pstAnOper = myOperationPool;
    AsyncOperation *pstLast = NULL;

    /* Search a free operation */
    while (NULL != pstAnOper && OPER_STATE_FREE != pstAnOper->state)
    {
        pstLast = pstAnOper;
        pstAnOper = pstAnOper->next;
    } /* for */

    /* If we didn't we find a free then increase the queue */
    if (NULL == pstAnOper)
    {
        if (NULL == (pstAnOper = MSA_ALLOC(sizeof(AsyncOperation))))
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, 
                                        WE_MODID_MSA, 
                                        "MMS FSM Async: No free async operations\n"));

            MSA_TERMINATE;
        } /* if */
        /*lint -e{794} */
        pstLast->next = pstAnOper;
        /*lint -e{613} */
        pstAnOper->previous = pstLast;
        /*lint -e{613} */
        pstAnOper->next = NULL;
        ++myPoolSize;
    } /* if */

    if (iFileHandle < 0) /* i.e. ERROR! */
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, 
                                    WE_MODID_MSA,
                                    "%s(%d): Erroneous file handle <%d>\n",
                                    __FILE__,
                                    __LINE__, 
                                    iFileHandle));
        return;
    } /* if */

    /*lint -e{613} */
    pstAnOper->fsm = (MsaStateMachine)iFsm;
    /*lint -e{613} */
    pstAnOper->instance = iInstance;
    /*lint -e{613} */
    pstAnOper->signal = (unsigned)iSignal;
    /*lint -e{613} */
    pstAnOper->fileHandle = iFileHandle;
    /*lint -e{613} */
    pstAnOper->buffer = NULL;
    /*lint -e{613} */
    pstAnOper->dataLen = 0;
    /*lint -e{613} */
    pstAnOper->mode = MSA_ASYNC_MODE_DISPATCH;
}

/*!
 * \brief Deregisters a delayed file operation previously ordered with a call
 *        to Msa_AsyncRegister.
 *
 * \param fileHandle The fileHandle of the ongoing operation that is to be 
 *                   cancelled.
 *****************************************************************************/
void Msa_AsyncDeregister(int iFileHandle)
{
    AsyncOperation *pstOp = myOperationPool;

    while((NULL != pstOp) && (pstOp->fileHandle != iFileHandle))
        pstOp = pstOp->next;
    if (NULL == pstOp)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                    WE_MODID_MSA, 
                                    "%s(%d): Invalid async oper handle op=%d\n", 
                                    __FILE__, 
                                    __LINE__, 
                                    pstOp));
        return;
    }

    if (ASYNC_POOL_SIZE < myPoolSize)
    {
        if (NULL != pstOp->previous)
        {
            pstOp->previous->next = pstOp->next;
        } /* if */

        if (NULL != pstOp->next)
        {
            pstOp->next->previous = pstOp->previous;
        } /* if */
        /*lint -e{774} */
        MSA_FREE(pstOp);

        --myPoolSize;
    } /* if */
}


/*!
 * \brief Halts and cleanup a previously started asynchronous file operation
 *
 * \param oper Handle to an async operation
 *****************************************************************************/
void Msa_AsyncOperationStop(MsaAsyncOperationHandle *pvOper)
{
    AsyncOperation *pvOp = (AsyncOperation *)*pvOper;

    if (NULL == pvOp)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, 
                                    WE_MODID_MSA, 
                                    "%s(%d): Invalid async oper handle op=%d\n", 
                                    __FILE__, 
                                    __LINE__, 
                                    pvOp));

        return;
    } /* if */

    if (OPER_STATE_OPENED ==  pvOp->state&& pvOp->fileHandle > 0)
    {
        (void) WE_FILE_CLOSE(pvOp->fileHandle);
    } /* if */
    if (OPER_STATE_OPENED ==  pvOp->state&& pvOp->pipeHandle > 0)
    {
        (void) WE_PIPE_CLOSE(pvOp->pipeHandle);
        /* Check if the pipe needs to be deleted */
        if (MSA_ASYNC_MODE_READ == pvOp->mode)
        {
            (void)WE_PIPE_DELETE(pvOp->pipeName);
        }
        MSA_FREE(pvOp->pipeName);
    } /* if */

    pvOp->state = OPER_STATE_FREE;
    pvOp->fileHandle = 0;

    if (myPoolSize > ASYNC_POOL_SIZE)
    {
        if (pvOp->previous != NULL)
        {
            pvOp->previous->next = pvOp->next;
        } /* if */

        if (pvOp->next != NULL)
        {
            pvOp->next->previous = pvOp->previous;
        } /* if */
        /*lint -e{774} */
        MSA_FREE(pvOp);

        --myPoolSize;
    } /* if */

    *pvOper = NULL;
} /* asyncOperationStop */

/*!
 * \brief Start a new read operation
 *
 * A read operation is started. The read operation will continue until
 * bufSize bytes is read. If the bufSize is larger than what is available,
 * the read operation will stop and send a EOF to the caller.
 *
 * \param oper Handle to an async operation
 * \param startPos At what position in the file that the read will start,
 *        -1 indicates at EOF.
 * \param buffer Where to store read data
 * \param bufSize Size of the buffer
 *****************************************************************************/
void Msa_AsyncRead
(
    MsaAsyncOperationHandle pvOper,
    WE_INT32 iStartPos, 
    void *pvBuffer,
    WE_UINT32 uiBufSize
)
{
    AsyncOperation *pstOp = (AsyncOperation *)pvOper;

    if (NULL ==  pstOp|| OPER_STATE_OPENED != pstOp->state)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                    WE_MODID_MSA, 
                                    "%s(%d): Wrong handle or state for read, op=%d, state=%d\n", 
                                    __FILE__, 
                                    __LINE__, 
                                    pstOp, 
                                    NULL ==  pstOp? -1 : pstOp->state));

        return;
    }
    else if (pstOp->fileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                    WE_MODID_MSA, 
                                    "%s(%d): File was not opened, check previous errors, op=%d," 
                                    "state=%d\n", 
                                    __FILE__, 
                                    __LINE__, 
                                    pstOp, 
                                    NULL ==  pstOp? -1 : pstOp->state));

        (void)MSA_SIGNAL_SENDTO_IUU( pstOp->fsm,
                                                                (int)pstOp->signal, 
                                                                pstOp->instance, 
                                                                MSA_ASIO_RESULT_FILE_NOT_FOUND, 
                                                                pstOp->totalLen);
        return;
    } /* if */

    /* OK, file is opened. Prepare for reading */
    pstOp->totalLen = 0;
    pstOp->pos = pvBuffer;
    pstOp->buffer = pvBuffer;
    pstOp->dataLen = uiBufSize;

    Msa_DoSeek(pstOp, iStartPos);
    Msa_DoRead(pstOp);
} /* asyncRead */

/*!
 * \brief Continues a previously started read operation
 *
 * A read operation has been stopped with type BUFFER_FULL. The caller 
 * indicates that more data can be read to the buffer.
 *
 * \param oper Handle to an async operation
 * \param bufSize Size of the buffer
 *****************************************************************************/
void Msa_AsyncReadContinue
(
    MsaAsyncOperationHandle pvOper,
    WE_UINT32 uiBufSize
)
{
    AsyncOperation *pstOp = (AsyncOperation *)pvOper;

    if (NULL ==  pstOp|| OPER_STATE_OPENED != pstOp->state)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                    WE_MODID_MSA, 
                                    "%s(%d): Wrong handle or state for readContinue, op=%d, state=%d\n",
                                    __FILE__,
                                    __LINE__,
                                    pstOp,
                                    NULL ==  pstOp? -1 : pstOp->state));

        return;
    }
    else if (pstOp->fileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, 
                                    WE_MODID_MSA, 
                                    "%s(%d): File was not opened, check previous errors, op=%d, ""state=%d\n", 
                                    __FILE__, 
                                    __LINE__, 
                                    pstOp, NULL ==  pstOp? -1 : pstOp->state));

        (void)MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                                (int)pstOp->signal, 
                                                                pstOp->instance, 
                                                                MSA_ASIO_RESULT_FILE_NOT_FOUND,
                                                                pstOp->totalLen);
        return;
    } /* if */

    pstOp->dataLen = uiBufSize;
    Msa_DoRead(pstOp);
} /* asyncReadContinue */

/*!
 * \brief Start a new write operation
 *
 * A write operation is started. The write operation will continue until
 * bufSize bytes is written. At this time, the write operation will stop 
 * and send MMS_RESULT_OK to the caller.
 *
 * \param oper Handle to an async operation
 * \param startPos At what position in the file that the write will start,
 *        -1 indicates append.
 * \param buffer Data to write to file
 * \param bufSize Size of the buffer where to fetch data to write
 *****************************************************************************/
void Msa_AsyncWrite
(
    MsaAsyncOperationHandle pvOper,
    WE_INT32 iStartPos,
    void *pvBuffer,
    WE_UINT32 uiBufSize)
{
    AsyncOperation *pstOp = (AsyncOperation *)pvOper;

    if (NULL ==  pstOp|| OPER_STATE_OPENED != pstOp->state)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                    WE_MODID_MSA, 
                                    "%s(%d): Wrong handle or state for write, op=%d, state=%d\n", 
                                    __FILE__, 
                                    __LINE__, 
                                    pstOp, 
                                    NULL ==  pstOp? -1 : pstOp->state));

        return;
    }
    else if (pstOp->fileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                    WE_MODID_MSA, 
                                    "%s(%d): File was not opened, check previous errors, op=%d, state=%d\n", 
                                    __FILE__, 
                                    __LINE__,
                                    pstOp, 
                                    NULL ==  pstOp? -1 : pstOp->state));

        (void)MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                                    (int)pstOp->signal, 
                                                                    pstOp->instance, 
                                                                    MSA_ASIO_RESULT_FILE_NOT_FOUND,
                                                                    pstOp->totalLen);
        return;
    } /* if */

    /* OK, file is opened. Prepare for reading */
    pstOp->totalLen = 0;
    pstOp->pos = pvBuffer;
    pstOp->buffer = pvBuffer;
    pstOp->dataLen = uiBufSize;

    /* Do not seek in append mode  */
    if (MSA_ASYNC_MODE_APPEND != pstOp->mode)
    {
        Msa_DoSeek(pstOp, iStartPos);
    }
    Msa_DoWrite(pstOp);
} /* asyncWrite */

/*!
 * \brief Continues a previously started write operation
 *
 * A write operation has been stopped with type BUFFER_EMPTY. The caller 
 * indicates that more data to write is available in the buffer.
 *
 * \param oper Handle to an async operation
 * \param bufSize Size of the buffer where to fetch data to write
*****************************************************************************/
void Msa_AsyncWriteContinue
(
    MsaAsyncOperationHandle pvOper,
    WE_UINT32 bufSize
)
{
    AsyncOperation *pstOp = (AsyncOperation *)pvOper;

    if (pstOp == NULL || pstOp->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                    WE_MODID_MSA, 
                                    "%s(%d): Wrong handle or state for writeContinue, op=%d, state=%d\n", 
                                    __FILE__, 
                                    __LINE__,
                                    pstOp,
                                    pstOp == NULL ? -1 : pstOp->state));

        return;
    }
    else if (pstOp->fileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                    WE_MODID_MSA, 
                                    "%s(%d): File was not opened, check previous errors, op=%d, state=%d\n", 
                                    __FILE__, 
                                    __LINE__, 
                                    pstOp,
                                    pstOp == NULL ? -1 : pstOp->state));

        (void)MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                                (int)pstOp->signal, 
                                                                pstOp->instance, 
                                                                MSA_ASIO_RESULT_FILE_NOT_FOUND, 
                                                                pstOp->totalLen);
        return;
    } /* if */

    pstOp->dataLen = bufSize;
    Msa_DoWrite(pstOp);
} /* asyncWriteContinue */

/*!
 * \brief Do the actual read
 *
 * \param op Data for an async operation
 *****************************************************************************/
static void Msa_DoRead(AsyncOperation *pstOp)
{
    int iRet;
    long lLength;

    lLength = pstOp->buffer - pstOp->pos + (long)pstOp->dataLen;

    /* File is opened and we're positioned so we can start to read */
    for (;;)
    {
        if (lLength < 1) /* We have read everything that we should */
        {
            (void)MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                                    (int)pstOp->signal, pstOp->instance, 
                                                                    MSA_ASIO_RESULT_OK,
                                                                    pstOp->totalLen);
            return;
        } /* if */

        iRet = WE_FILE_READ( pstOp->fileHandle, pstOp->pos, lLength);
        switch (iRet)
        {
        case 0 :
        case WE_FILE_ERROR_EOF :
            (void)MSA_SIGNAL_SENDTO_IUU(pstOp->fsm, (int)pstOp->signal,
                                                                        pstOp->instance, 
                                                                        MSA_ASIO_RESULT_EOF, 
                                                                        pstOp->totalLen);
            return;

        case WE_FILE_ERROR_ACCESS :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_MSA, 
                "%s(%d): Read returned %d\n", __FILE__, __LINE__, iRet));

            (void)MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                                    (int)pstOp->signal,
                                                                    pstOp->instance, 
                                                                    Msa_TranslateFileError(iRet),
                                                                    pstOp->totalLen);
            return;

        case WE_FILE_ERROR_DELAYED :
            WE_FILE_SELECT( pstOp->fileHandle, WE_FILE_EVENT_READ);
            return;

        default :
            if (iRet < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, 
                                            WE_MODID_MSA, 
                                            "%s(%d): Unhandled file error %d\n",
                                            __FILE__, 
                                            __LINE__, 
                                            iRet));
                (void)MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                                        (int)pstOp->signal, 
                                                                        pstOp->instance,
                                                                        Msa_TranslateFileError(iRet),
                                                                        pstOp->totalLen);

                return; 
            } /* if */

            pstOp->totalLen += (WE_UINT32)iRet;
            pstOp->pos += iRet;
            lLength -= iRet;
            break;
        } /* switch */
    } /* for */
} /* Msa_DoRead */

/*!
 * \brief Do the actual seek
 *
 * \param op Data for an async operation
 * \param startPos Position in file to start reading/writing
 *****************************************************************************/
static void Msa_DoSeek
(
    const AsyncOperation *pstOp,
    WE_INT32 iStartPos
)
{
    int iSeekMode;
    long lSeekOffset;
    long lRet;

    if (pstOp && (MSA_ASYNC_MODE_APPEND == pstOp->mode)) 
    {
        /* Seek is not allowed when in append mode */
        return;
    }
    if (-1 == iStartPos)
    {
        iSeekMode = WE_FILE_SEEK_END;
        lSeekOffset = 0;
    }
    else
    {
        iSeekMode = WE_FILE_SEEK_SET;
        lSeekOffset = iStartPos;
    } /* if */

    if ((lRet = WE_FILE_SEEK( pstOp->fileHandle, lSeekOffset, iSeekMode)) < 0)
    {
        (void)MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                                (int)pstOp->signal,
                                                                pstOp->instance, 
                                                                Msa_TranslateFileError(lRet),
                                                                pstOp->totalLen);

        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, 
                                        WE_MODID_MSA, 
                                        "%s(%d): Failed to seek file %d, mode=%d, pos=%d, ret=%d\n", 
                                        __FILE__, 
                                        __LINE__, 
                                        pstOp->fileHandle, 
                                        iSeekMode, 
                                        lSeekOffset, 
                                        lRet));

        return;
    } /* if */
} /* Msa_DoSeek */

/*!
 * \brief Do the actual write
 *
 * \param op Data for an async operation
 *****************************************************************************/
static void Msa_DoWrite(AsyncOperation *pstOp)
{
    int iRet;
    int lLength;

    lLength = pstOp->buffer - pstOp->pos + (long)pstOp->dataLen;

    /* File is opened and we're positioned so we can start to write */
    for (;;)
    {
        if (lLength < 1)    /* Have we written everything from the buffer? */
        {
            (void)MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                                    (int)pstOp->signal,
                                                                    pstOp->instance,
                                                                    MSA_ASIO_RESULT_OK,
                                                                    pstOp->totalLen);
            return;
        } /* if */

        iRet = WE_FILE_WRITE( pstOp->fileHandle, pstOp->pos, lLength);
        switch (iRet)
        {
        case 0 :
        case WE_FILE_ERROR_EOF :
        case WE_FILE_ERROR_ACCESS :
        case WE_FILE_ERROR_FULL :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                        WE_MODID_MSA, 
                                        "%s(%d): Write returned %d\n",
                                        __FILE__, 
                                        __LINE__, 
                                        iRet));

            (void)MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                                    (int)pstOp->signal,
                                                                    pstOp->instance,
                                                                    Msa_TranslateFileError(iRet),
                                                                    pstOp->totalLen);
            return;

        case WE_FILE_ERROR_DELAYED :
            WE_FILE_SELECT( pstOp->fileHandle, WE_FILE_EVENT_WRITE);
            return;

        default :
            if (iRet < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                            WE_MODID_MSA, 
                                            "%s(%d): Unhandled file error %d\n",
                                            __FILE__, 
                                            __LINE__, 
                                            iRet));
                (void)MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                                        (int)pstOp->signal,
                                                                        pstOp->instance,
                                                                        Msa_TranslateFileError(iRet),
                                                                        pstOp->totalLen);

                return; 
            } /* if */

            pstOp->totalLen += (unsigned long)iRet;
            pstOp->pos += iRet;
            lLength -= iRet;
            break;
        } /* switch */
    } /* for */
} /* Msa_DoWrite */

/*!
 * \brief Takes care of a NOTIFY signal (response to an WE_FILE_SELECT operation)
 *
 * \param fileHandle File that the signal is related to
 * \param operationType that the notification was related to
 *****************************************************************************/
static void Msa_HandleNotify
(
    int iFileHandle,
    int iOperationType
)
{
    AsyncOperation *pstAnOper = myOperationPool;

    while (NULL != pstAnOper)
    {   /* Only opened files can cause NOTIFY signals */
        if (pstAnOper->fileHandle == iFileHandle &&
            MSA_ASYNC_MODE_DISPATCH == pstAnOper->mode)
        {
            switch (iOperationType)
            {
            case WE_FILE_READ_READY :
            case WE_FILE_WRITE_READY :
                (void)MSA_SIGNAL_SENDTO_IU(pstAnOper->fsm,
                                                                        (int)pstAnOper->signal, 
                                                                        pstAnOper->instance,
                                                                        MSA_ASIO_RESULT_OK); 
                Msa_AsyncDeregister(iFileHandle);
                break;
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, 
                                            WE_MODID_MSA, 
                                            "%s(%d): Illegal operationType %d\n", 
                                            __FILE__, 
                                            __LINE__,
                                            iOperationType));
                break;
            } /* switch */

            return;
        }

        if (OPER_STATE_OPENED ==  pstAnOper->state&& 
            pstAnOper->fileHandle == iFileHandle)
        {
            switch (iOperationType)
            {
            case WE_FILE_READ_READY :
                Msa_AsyncReadContinue(pstAnOper, pstAnOper->dataLen);
                break;
            case WE_FILE_WRITE_READY :
                Msa_AsyncWriteContinue(pstAnOper, pstAnOper->dataLen);
                break;
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, 
                                                WE_MODID_MSA, 
                                                "%s(%d): Illegal operationType %d\n",
                                                __FILE__, 
                                                __LINE__,
                                                iOperationType));
                break;
            } /* switch */

            return;
        } /* if */

        pstAnOper = pstAnOper->next;
    } /* while */
    /*lint -e{774} */
    if (NULL == pstAnOper)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                    WE_MODID_MSA, 
                                    "%s(%d): Couldn't find handle\n", 
                                    __FILE__, 
                                    __LINE__));
    } /* if */
} /* Msa_HandleNotify */

/*!
 * \brief Takes care of a TERMINATE signal
 *****************************************************************************/
void Msa_AsyncTerminate(void)
{
    AsyncOperation *pstAnOper = myOperationPool;
    AsyncOperation *pstNext = NULL;

    myOperationPool = NULL;
    myPoolSize = 0;
    
    while (NULL != pstAnOper)
    {
        pstNext = pstAnOper->next;
        MSA_FREE(pstAnOper);
        pstAnOper = pstNext;
    } /* while */

    msaSignalDeregister(MSA_ASYNC_FSM);
} /* handleTerminate */

/*! \brief
 *	
 *****************************************************************************/
static AsyncOperation *Msa_AsyncGetNewInstance(void)
{
    AsyncOperation *pstAnOper = myOperationPool;
    AsyncOperation *pstLast = NULL;

    /* Search a free operation */
    while (NULL !=  pstAnOper&& OPER_STATE_FREE != pstAnOper->state)
    {
        pstLast = pstAnOper;
        pstAnOper = pstAnOper->next;
    } /* for */

    /* If we didn't find a free item then increase the queue */
    if (NULL == pstAnOper)
    {
        if (NULL == (pstAnOper = MSA_ALLOC(sizeof(AsyncOperation))))
        {
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 
                                        WE_MODID_MSA, 
                                        "MSA FSM Async: No free async operations\n"));

            return NULL;
        } /* if */

        if (NULL != pstLast)
        {
            pstLast->next = pstAnOper;
        }
        pstAnOper->previous = pstLast;
        pstAnOper->next = NULL;
        ++myPoolSize;
    } /* if */

    /*
     *	Reset stuff before usage.
     */
/*    anOper->pipeName = NULL;*/
    pstAnOper->fileHandle = 0;
    pstAnOper->pipeHandle = 0;

    return pstAnOper;
}

/*!
 * \brief Start a new asynchronous pipe operation
 *
 *  Whenever an asynchronous pipe operations is finished or shall be aborted, 
 *  it must be stopped with a call to #asyncOperationStop.
 *
 * \param pipeName  Name of pipe operating on. 
 * \param fsm       FSM ordering the operation
 * \param instance  FSM instance ordering the operation
 * \param signal    FSMs requested response signal
 * \param mode      Read, write or both?
 * \return AsyncOperationHandle, i.e. ptr to item in array with operations,
 *         NULL if no free
 *****************************************************************************/
MsaAsyncOperationHandle Msa_AsyncPipeOperationStart
(
    const char *pcPipeName, 
    MsaStateMachine eFsm,
    long lInstance,
    WE_UINT32 uiSignal,
    MsaAsyncMode eMode
)
{
    AsyncOperation *pstAnOper = myOperationPool;

    /* Search a free operation */
    pstAnOper = Msa_AsyncGetNewInstance();
    if (NULL == pstAnOper)
    {
        return NULL;
    }

    switch (eMode)
    {
    case MSA_ASYNC_MODE_READ :
        pstAnOper->pipeHandle = WE_PIPE_OPEN(WE_MODID_MSA, pcPipeName);
        break;
    case MSA_ASYNC_MODE_WRITE :
        pstAnOper->pipeHandle = WE_PIPE_CREATE(WE_MODID_MSA, pcPipeName);
        break;
    default :
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, 
                                    WE_MODID_MSA, 
                                    "%s(%d): Illegal pipe mode %d\n", 
                                    __FILE__, 
                                    __LINE__, 
                                    eMode));

        break;
    } /* switch */

    if (pstAnOper->pipeHandle < 0) /* i.e. ERROR! */
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, 
                                    WE_MODID_MSA, 
                                    "(%s) (%d): Failed to open/create pipe <%s>, %d\n",
                                    __FILE__, 
                                    __LINE__, 
                                    NULL ==  pcPipeName? "NULL" : pcPipeName, 
                                    pstAnOper->pipeHandle));
        return NULL;
    } /* if */

    pstAnOper->fsm = eFsm;
    pstAnOper->mode = eMode;
    pstAnOper->pipeName = we_cmmn_strdup(WE_MODID_MSA, pcPipeName);
    pstAnOper->instance = lInstance;
    pstAnOper->signal = uiSignal;
    pstAnOper->fileHandle = 0;
    pstAnOper->state = OPER_STATE_OPENED;
    pstAnOper->buffer = NULL;
    pstAnOper->dataLen = 0;

    return pstAnOper;
}

/*!
 * \brief Takes care of a NOTIFY signal (response to an WE_FILE_SELECT operation)
 *
 * \param fileHandle File that the signal is related to
 * \param operationType that the notification was related to
 *****************************************************************************/
static void Msa_HandlePipeNotify
(
    int iPipeHandle,
    int iOperationType
)
{
    AsyncOperation *pstAnOper = myOperationPool;

    while (pstAnOper != NULL)
    {   /* Only opened files can cause NOTIFY signals */
        if (pstAnOper->state == OPER_STATE_OPENED && 
            pstAnOper->pipeHandle == iPipeHandle)
        {
            switch (iOperationType)
            {
            case WE_PIPE_EVENT_READ:
                Msa_DoPipeRead(pstAnOper);
                break;
            case WE_PIPE_EVENT_WRITE:
                Msa_DoPipeWrite(pstAnOper);
                break;
            case WE_PIPE_EVENT_CLOSED:
                MSA_SIGNAL_SENDTO_IUU(pstAnOper->fsm, 
                                                                pstAnOper->signal, 
                                                                pstAnOper->instance, 
                                                                MSA_ASIO_RESULT_EOF, 
                                                                pstAnOper->totalLen);
                break;
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                            WE_MODID_MSA,
                                            "(%s) (%d): Illegal operationType %d\n",
                                            __FILE__, 
                                            __LINE__,
                                            iOperationType));
                break;
            } /* switch */

            return;
        } /* if */

        pstAnOper = pstAnOper->next;
    } /* while */

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, 
                                WE_MODID_MMS, 
                                "%s(%d): Couldn't find handle\n", 
                                __FILE__,
                                __LINE__));
} /* Msa_HandlePipeNotify */

/*!
 * \brief Translates an WE File Error to an MMS Result
 *
 * \param fileError Error as given from call to WE FILE function
 *****************************************************************************/
static MsaAsioResult Msa_TranslateFileError(int iFileError)
{
    MsaAsioResult eRet = MSA_ASIO_RESULT_ERROR;

    switch (iFileError)
    {
    case WE_FILE_OK :
        eRet = MSA_ASIO_RESULT_OK;
        break;
    case WE_FILE_ERROR_ACCESS : /* e.g. file opened in wrong mode */
        eRet = MSA_ASIO_RESULT_ERROR;
        break;
    case WE_FILE_ERROR_DELAYED :
        eRet = MSA_ASIO_RESULT_DELAYED;
        break;
    case WE_FILE_ERROR_PATH :
        eRet = MSA_ASIO_RESULT_FILE_NOT_FOUND;
        break;
    case WE_FILE_ERROR_INVALID : /* e.g. illegal file name */
        eRet = MSA_ASIO_RESULT_ERROR;
        break;
    case WE_FILE_ERROR_SIZE :
        eRet = MSA_ASIO_RESULT_INSUFFICIENT_PERSISTENT_STORAGE;
        break;
    case WE_FILE_ERROR_FULL :
        eRet = MSA_ASIO_RESULT_INSUFFICIENT_PERSISTENT_STORAGE;
        break;
    case WE_FILE_ERROR_EOF :
        eRet = MSA_ASIO_RESULT_EOF;
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, 
                                    WE_MODID_MSA, 
                                    "%s(%d): Unknown fileError %d\n",
                                    __FILE__, 
                                    __LINE__,
                                    iFileError));
        break;
    } /* switch */

    return eRet;
} /* Msa_TranslateFileError */

/*!
 * \brief Translates an WE File Error to an MMS Result
 *
 * \param fileError Error as given from call to WE FILE function
 *****************************************************************************/
static MsaAsioResult Msa_TranslatePipeError(int iPipeError)
{
    MsaAsioResult eRet = MSA_ASIO_RESULT_ERROR;

    switch (iPipeError)
    {
    case WE_PIPE_ERROR_BAD_HANDLE :
    case WE_PIPE_ERROR_INVALID_PARAM :
    case WE_PIPE_ERROR_EXISTS :
    case WE_PIPE_ERROR_IS_OPEN :
    case WE_PIPE_ERROR_RESOURCE_LIMIT :
    case WE_PIPE_ERROR_NOT_FOUND :
        eRet = MSA_ASIO_RESULT_ERROR;
        break;
    case WE_PIPE_ERROR_DELAYED :
        eRet = MSA_ASIO_RESULT_DELAYED;
        break;
    case WE_PIPE_ERROR_CLOSED :
        eRet = MSA_ASIO_RESULT_EOF;
        break;
    default :
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, 
                                    WE_MODID_MSA, 
                                    "%s(%d): Unknown pipeError %d\n",
                                    __FILE__, 
                                    __LINE__,
                                    iPipeError));

        break;
    } /* switch */

    return eRet;
} /* Msa_TranslatePipeError */


/*!
 * \brief Do the actual write
 *
 * \param op Data for an async operation
 *****************************************************************************/
static void Msa_DoPipeWrite(AsyncOperation *pstOp)
{
    long lRet;
    long lLength;

    lLength = pstOp->buffer - pstOp->pos + (long)pstOp->dataLen;

    /* Pipe is opened so we can start to write */
    for (;;)
    {
        if (lLength < 1)    /* Have we written everything from the buffer? */
        {
            MSA_SIGNAL_SENDTO_IUU(pstOp->fsm, pstOp->signal,
                                                            pstOp->instance,
                                                            MMS_RESULT_OK,
                                                            pstOp->totalLen);
            return;
        } /* if */

        lRet = WE_PIPE_WRITE( pstOp->pipeHandle, pstOp->pos, lLength);
        switch (lRet)
        {
        case WE_PIPE_ERROR_CLOSED :
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM,
                                        WE_MODID_MSA, 
                                        "%s(%d): Pipe write returned %d\n",
                                        __FILE__, 
                                        __LINE__,
                                        lRet));

            MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                            pstOp->signal,
                                                            pstOp->instance,
                                                            Msa_TranslatePipeError(lRet),
                                                            pstOp->totalLen);
            return;

        case 0 :
        case WE_PIPE_ERROR_DELAYED :
            WE_PIPE_POLL(pstOp->pipeHandle);
            return;

        default :
            if (lRet < 0)
            {
                WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM,
                                            WE_MODID_MSA,
                                            "%s(%d): Unhandled pipe error %d\n",
                                            __FILE__,
                                            __LINE__,
                                            lRet));
                MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                                pstOp->signal,
                                                                pstOp->instance,
                                                                Msa_TranslatePipeError(lRet),
                                                                pstOp->totalLen);

                return; 
            } /* if */

            pstOp->totalLen += (WE_UINT32) lRet;
            pstOp->pos += lRet;
            lLength -= lRet;
            break;
        } /* switch */
    } /* for */
}

/*!
 * \brief Do the actual read
 *
 * \param op Data for an async operation
 *****************************************************************************/
static void Msa_DoPipeRead(AsyncOperation *pstOp)
{
    int iRet;
    long lLength;

    lLength = pstOp->buffer - pstOp->pos + (long)pstOp->dataLen;

    /* Pipe is opened so we can start to read */
    for (;;)
    {
        if (lLength < 1) /* We have read everything that we should */
        {
            MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                            pstOp->signal,
                                                            pstOp->instance, 
                                                            MSA_ASIO_RESULT_OK,
                                                            pstOp->totalLen);
            return;
        } /* if */

        iRet = WE_PIPE_READ( pstOp->pipeHandle, pstOp->pos, lLength);
        switch (iRet)
        {
        case WE_PIPE_ERROR_CLOSED :
            MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                            pstOp->signal,
                                                            pstOp->instance, 
                                                            MSA_ASIO_RESULT_EOF, 
                                                            pstOp->totalLen);
            return;

        case WE_PIPE_ERROR_NOT_FOUND :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM,
                                            WE_MODID_MMS, 
                                            "%s(%d): Pipe read returned %d\n",
                                            __FILE__,
                                            __LINE__,
                                            iRet));

            MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                            pstOp->signal,
                                                            pstOp->instance,
                                                            Msa_TranslatePipeError(iRet),
                                                            pstOp->totalLen);
            return;

        case 0 :
        case WE_PIPE_ERROR_DELAYED :
            WE_PIPE_POLL(pstOp->pipeHandle);
            return;

        default :
            if (iRet < 0)
            {
                WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM,
                                            WE_MODID_MMS, 
                                            "%s(%d): Unhandled pipe error %d\n",
                                            __FILE__,
                                            __LINE__,
                                            iRet));
                MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                                pstOp->signal,
                                                                pstOp->instance, 
                                                                Msa_TranslatePipeError(iRet),
                                                                pstOp->totalLen);

                return; 
            } /* if */

            pstOp->totalLen += (WE_UINT32)iRet;
            pstOp->pos += iRet;
            lLength -= iRet;
            break;
        } /* switch */
    } /* for */
}

/*! \brief Initiates a read pipe to buffer operation.
 *
 * \param oper The current operation.
 * \param buffer The buffer to write data to. 
 * \param bufferSize The size of the buffer.
 *****************************************************************************/
void Msa_AsyncPipeRead
(
    MsaAsyncOperationHandle pvOper,
    void *pvBuffer, 
    WE_UINT32 uiBufferSize
)
{
    AsyncOperation *pstOp = (AsyncOperation *)pvOper;

    if (NULL ==  pstOp|| OPER_STATE_OPENED != pstOp->state)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM,
                                    WE_MODID_MMS, 
                                    "%s(%d): Wrong handle or state for pipe read, op=%d, state=%d\n",
                                    __FILE__, 
                                    __LINE__, 
                                    pstOp, 
                                    NULL ==  pstOp? -1 : pstOp->state));

        return;
    }
    else if (pstOp->pipeHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, 
                                    WE_MODID_MSA, 
                                    "%s(%d): Pipe was not opened, check previous errors, op=%d, state=%d\n", 
                                    __FILE__, 
                                    __LINE__, 
                                    pstOp, 
                                    NULL ==  pstOp? -1 : pstOp->state));

        MSA_SIGNAL_SENDTO_IUU(pstOp->fsm, 
                                                        pstOp->signal, 
                                                        pstOp->instance, 
                                                        MSA_ASIO_RESULT_ERROR, 
                                                        pstOp->totalLen);
        return;
    } /* if */

    /* OK, pipe is opened. Prepare for reading */
    pstOp->totalLen = 0;
    pstOp->pos = pvBuffer;
    pstOp->buffer = pvBuffer;
    pstOp->dataLen = uiBufferSize;

    Msa_DoPipeRead(pstOp);
}

/*! \brief Async pipe operations using buffer.
 *
 * \param oper The current operation.
 * \param buffer The buffer to read data from.
 * \param bufferSize The size of the buffer.
 *****************************************************************************/
void Msa_AsyncPipeWrite
(
    MsaAsyncOperationHandle pvOper,
    void *pvBuffer, 
    WE_UINT32 uiBufferSize
)
{
    AsyncOperation *pstOp = (AsyncOperation *)pvOper;

    if (NULL ==  pstOp|| OPER_STATE_OPENED != pstOp->state)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM,
                                    WE_MODID_MSA, 
                                    "%s(%d): Wrong handle or state for pipe write, op=%d, state=%d\n", 
                                    __FILE__, 
                                    __LINE__, 
                                    pstOp, 
                                    NULL ==  pstOp? -1 : pstOp->state));

        return;
    }
    else if (pstOp->pipeHandle < 0)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM,
                                    WE_MODID_MSA,
                                    "%s(%d): Pipe was not opened, check previous errors, op=%d, state=%d\n",
                                    __FILE__,
                                    __LINE__,
                                    pstOp,
                                    NULL ==  pstOp? -1 : pstOp->state));

        MSA_SIGNAL_SENDTO_IUU(pstOp->fsm,
                                                        pstOp->signal,
                                                        pstOp->instance,
                                                        MSA_ASIO_RESULT_ERROR,
                                                        pstOp->totalLen);
        return;
    } /* if */

    /* OK, pipe is opened. Prepare for reading writing */
    pstOp->totalLen = 0;
    pstOp->pos = pvBuffer;
    pstOp->buffer = pvBuffer;
    pstOp->dataLen = uiBufferSize;

    Msa_DoPipeWrite(pstOp);
}

