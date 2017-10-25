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

/*! \file maasync.c
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
#include "We_File.h"   /* WE: WE File API */ 
#include "We_Pipe.h"

/* SIS */
#include "Sis_cfg.h"
#include "Sis_def.h"
#include "Sis_if.h"

#include "Mms_Def.h"

#include "Saintsig.h"   /* SMA: Signal handling functions */
#include "Samem.h"      /* SMA: Memory handling functions */
#include "Saasync.h"    /* SMA: Async file handling functions */
#include "Satypes.h"
#include "Sia_def.h"
#include "Sacore.h"

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
    SiaAsyncMode mode;      /*!< The type of operation see #SiaAsyncMode */

    SiaStateMachine fsm;    /*!< FSM originally ordering the operation */
    long instance;          /*!< FSM instance */
    int signal;             /*!< Response signal ID */

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
static void asyncMain(SiaSignal *sig);
static void doRead(AsyncOperation *op);
static void doSeek(const AsyncOperation *op, WE_INT32 startPos);
static void doWrite(AsyncOperation *op);
static void handleNotify(int fileHandle, int operationType);
static SiaAsioResult translateFileError(int fileError);
static void doPipeRead(AsyncOperation *op);
static void doPipeWrite(AsyncOperation *op);
static void handlePipeNotify(int pipeHandle, int operationType);
static AsyncOperation *asyncGetNewInstance(void);

/******************************************************************************/

/*!
 * \brief Initialize the Async File API FSM.
 * \return TRUE if succesful, FALSE otherwise
 *****************************************************************************/
WE_BOOL smaAsyncInit(void)
{
    int i;
    AsyncOperation *previous = NULL;
    AsyncOperation *aNew = NULL;

    smaSignalRegisterDst(SIA_FSM_ASYNC, asyncMain);
    
    myOperationPool = NULL;
    myPoolSize = 0;
    for (i = 0; i < ASYNC_POOL_SIZE; ++i)
    {
        /* Do not use exception during initiation */
        if ((aNew = SIA_ALLOC(sizeof(AsyncOperation))) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                "%s(%d): Insufficient memory during initialization\n", 
                __FILE__, __LINE__));
            return FALSE;
        } /* if */

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
            } /* if */

            aNew->previous = previous;
            aNew->next = NULL;
            previous = aNew;
        } /* if */
    } /* for */

    myPoolSize = i;

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
        "MMS FSM Async: initialized\n"));

    return TRUE;
} /* asyncInit */

/*!
 * \brief Main function for Async File API FSM
 * 
 * \param sig Incoming signal to the FSM
 *****************************************************************************/
static void asyncMain(SiaSignal *sig)
{
    switch (sig->type)
    {
    case SIA_SIG_ASYNC_NOTIFY :
        handleNotify( (int)sig->u_param1, sig->i_param);
        break;
    case MMS_SIG_ASYNC_PIPE_NOTIFY :
        handlePipeNotify( (int)sig->u_param1, sig->i_param);
        break;
    default:
        /* Unknown signal: just ignore. */
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): received unknown signal %d\n", __FILE__, __LINE__,
            sig->type));
        break;
    } /* switch */
    
    smaSignalDelete(sig);
} /* asyncMain */

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
SiaAsyncOperationHandle smaAsyncOperationStart(const char *fileName, 
    SiaStateMachine fsm, long instance, int signal, SiaAsyncMode mode)
{
    AsyncOperation *anOper = myOperationPool;
    AsyncOperation *last = NULL;
    int fileHandle;
    int openMode;

    /* Search a free operation */
    while (anOper != NULL && anOper->state != OPER_STATE_FREE)
    {
        last = anOper;
        anOper = anOper->next;
    } /* for */

    /* If we didn't we find a free then increase the queue */
    if (anOper == NULL)
    {
        if ((anOper = SIA_ALLOC(sizeof(AsyncOperation))) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                "MMS FSM Async: No free async operations\n"));

            return NULL;
        } /* if */
        /*lint -e{794} */
        last->next = anOper;
        anOper->previous = last;
        anOper->next = NULL;
        ++myPoolSize;
    } /* if */
    /*lint -e{788} */
    switch (mode)
    {
    case SIA_ASYNC_MODE_READ :
        openMode = WE_FILE_SET_RDONLY;
        break;
    case SIA_ASYNC_MODE_WRITE :
        openMode = WE_FILE_SET_WRONLY | WE_FILE_SET_CREATE;
        break;
    case SIA_ASYNC_MODE_APPEND :
        openMode = WE_FILE_SET_WRONLY | WE_FILE_SET_CREATE |
            WE_FILE_SET_APPEND;
        break;
    case SIA_ASYNC_MODE_READ_WRITE :
        /* NOTE! We cannot use WE_FILE_SET_APPEND here since seek is not
         * allowed when using this flag */
        openMode = WE_FILE_SET_RDWR | WE_FILE_SET_CREATE;
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
            "%s(%d): Illegal open mode %d\n", __FILE__, __LINE__, 
            mode));

        openMode = WE_FILE_SET_RDWR | WE_FILE_SET_CREATE;
        break;
    } /* switch */

    fileHandle = WE_FILE_OPEN(WE_MODID_SIA, fileName, openMode, 0);

    if (fileHandle < 0) /* i.e. ERROR! */
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
            "%s(%d): Failed to open file <%s>, %d\n", __FILE__, __LINE__, 
            fileName == NULL ? "NULL" : fileName, fileHandle));
    } /* if */

    anOper->fsm = fsm;
    anOper->instance = instance;
    anOper->signal = signal;
    anOper->fileHandle = fileHandle;
    anOper->state = OPER_STATE_OPENED;
    anOper->buffer = NULL;
    anOper->dataLen = 0;
    anOper->mode = mode;

    return anOper;
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
void smaAsyncRegister(int fileHandle, int fsm, int signal, int instance)
{
    AsyncOperation *anOper = myOperationPool;
    AsyncOperation *last = NULL;

    /* Search a free operation */
    while (anOper != NULL && anOper->state != OPER_STATE_FREE)
    {
        last = anOper;
        anOper = anOper->next;
    } /* for */

    /* If we didn't we find a free then increase the queue */
    if (anOper == NULL)
    {
        if ((anOper = SIA_ALLOC(sizeof(AsyncOperation))) == NULL)
        {
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                "MMS FSM Async: No free async operations\n"));

            TERMINATE_SMIL_PLAYER(SIA_RESULT_ERROR, SIA_TERM_MODULES);
        } /* if */
        /*lint -e{794} */
        last->next = anOper;
        /*lint -e{613} */
        anOper->previous = last;
        /*lint -e{613} */
        anOper->next = NULL;
        ++myPoolSize;
    } /* if */

    if (fileHandle < 0) /* i.e. ERROR! */
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
            "%s(%d): Erroneous file handle <%d>\n", __FILE__, __LINE__, 
            fileHandle));
        return;
    } /* if */

    /*lint -e{613} */
    anOper->fsm = (SiaStateMachine)fsm;
    /*lint -e{613} */
    anOper->instance = instance;
    /*lint -e{613} */
    anOper->signal = signal;
    /*lint -e{613} */
    anOper->fileHandle = fileHandle;
    /*lint -e{613} */
    anOper->buffer = NULL;
    /*lint -e{613} */
    anOper->dataLen = 0;
    /*lint -e{613} */
    anOper->mode = SIA_ASYNC_MODE_DISPATCH;
}

/*!
 * \brief Deregisters a delayed file operation previously ordered with a call
 *        to smaAsyncRegister.
 *
 * \param fileHandle The fileHandle of the ongoing operation that is to be 
 *                   cancelled.
 *****************************************************************************/
void smaAsyncDeregister(int fileHandle)
{
    AsyncOperation *op = myOperationPool;

    while((op != NULL) && (op->fileHandle != fileHandle))
        op = op->next;
    if (NULL == op)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Invalid async oper handle op=%d\n", 
            __FILE__, __LINE__, op));
        return;
    }

    if (myPoolSize > ASYNC_POOL_SIZE)
    {
        if (op->previous != NULL)
        {
            op->previous->next = op->next;
        } /* if */

        if (op->next != NULL)
        {
            op->next->previous = op->previous;
        } /* if */
        /*lint -e{774} */
        SIA_FREE(op);

        --myPoolSize;
    } /* if */
}


/*!
 * \brief Halts and cleanup a previously started asynchronous file operation
 *
 * \param oper Handle to an async operation
 *****************************************************************************/
void smaAsyncOperationStop(SiaAsyncOperationHandle *oper)
{
    AsyncOperation *op = (AsyncOperation *)*oper;

    if (op == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Invalid async oper handle op=%d\n", 
            __FILE__, __LINE__, op));

        return;
    } /* if */

    if (op->state == OPER_STATE_OPENED && op->fileHandle > 0)
    {
        (void) WE_FILE_CLOSE(op->fileHandle);
    } /* if */
    if (op->state == OPER_STATE_OPENED && op->pipeHandle > 0)
    {
        (void) WE_PIPE_CLOSE(op->pipeHandle);
        /* Check if the pipe needs to be deleted */
        if (SIA_ASYNC_MODE_READ == op->mode)
        {
            (void)WE_PIPE_DELETE(op->pipeName);
        }
        SIA_FREE(op->pipeName);
    } /* if */

    op->state = OPER_STATE_FREE;
    op->fileHandle = 0;

    if (myPoolSize > ASYNC_POOL_SIZE)
    {
        if (op->previous != NULL)
        {
            op->previous->next = op->next;
        } /* if */

        if (op->next != NULL)
        {
            op->next->previous = op->previous;
        } /* if */
        /*lint -e{774} */
        SIA_FREE(op);

        --myPoolSize;
    } /* if */

    *oper = NULL;
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
void smaAsyncRead(SiaAsyncOperationHandle oper, WE_INT32 startPos, 
    void *buffer, WE_UINT32 bufSize)
{
    AsyncOperation *op = (AsyncOperation *)oper;

    if (op == NULL || op->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Wrong handle or state for read, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        return;
    }
    else if (op->fileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): File was not opened, check previous errors, op=%d," 
            "state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        (void)SIA_SIGNAL_SENDTO_IUU( op->fsm, op->signal, op->instance, 
            SIA_ASIO_RESULT_FILE_NOT_FOUND, op->totalLen);
        return;
    } /* if */

    /* OK, file is opened. Prepare for reading */
    op->totalLen = 0;
    op->pos = buffer;
    op->buffer = buffer;
    op->dataLen = bufSize;

    doSeek(op, startPos);
    doRead(op);
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
void smaAsyncReadContinue(SiaAsyncOperationHandle oper, WE_UINT32 bufSize)
{
    AsyncOperation *op = (AsyncOperation *)oper;

    if (op == NULL || op->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Wrong handle or state for readContinue, op=%d, state=%d\n"
            , __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        return;
    }
    else if (op->fileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): File was not opened, check previous errors, op=%d, "
            "state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
            SIA_ASIO_RESULT_FILE_NOT_FOUND, op->totalLen);
        return;
    } /* if */

    op->dataLen = bufSize;
    doRead(op);
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
void smaAsyncWrite(SiaAsyncOperationHandle oper, WE_INT32 startPos, 
                   void *buffer, WE_UINT32 bufSize)
{
    AsyncOperation *op = (AsyncOperation *)oper;

    if (op == NULL || op->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Wrong handle or state for write, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        return;
    }
    else if (op->fileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): File was not opened, check previous errors, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
            SIA_ASIO_RESULT_FILE_NOT_FOUND, op->totalLen);
        return;
    } /* if */

    /* OK, file is opened. Prepare for reading */
    op->totalLen = 0;
    op->pos = buffer;
    op->buffer = buffer;
    op->dataLen = bufSize;

    /* Do not seek in append mode  */
    if (op->mode != SIA_ASYNC_MODE_APPEND)
    {
        doSeek(op, startPos);
    }
    doWrite(op);
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
void smaAsyncWriteContinue(SiaAsyncOperationHandle oper, WE_UINT32 bufSize)
{
    AsyncOperation *op = (AsyncOperation *)oper;

    if (op == NULL || op->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Wrong handle or state for writeContinue, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        return;
    }
    else if (op->fileHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): File was not opened, check previous errors, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
            SIA_ASIO_RESULT_FILE_NOT_FOUND, op->totalLen);
        return;
    } /* if */

    op->dataLen = bufSize;
    doWrite(op);
} /* asyncWriteContinue */

/*!
 * \brief Do the actual read
 *
 * \param op Data for an async operation
 *****************************************************************************/
static void doRead(AsyncOperation *op)
{
    int ret;
    long length;

    length = op->buffer - op->pos + (long)op->dataLen;

    /* File is opened and we're positioned so we can start to read */
    for (;;)
    {
        if (length <= 0) /* We have read everything that we should */
        {
            (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
                SIA_ASIO_RESULT_OK, op->totalLen);
            return;
        } /* if */

        ret = WE_FILE_READ( op->fileHandle, op->pos, length);
        switch (ret)
        {
        case 0 :
        case WE_FILE_ERROR_EOF :
            (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
                SIA_ASIO_RESULT_EOF, op->totalLen);
            return;

        case WE_FILE_ERROR_ACCESS :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                "%s(%d): Read returned %d\n", __FILE__, __LINE__, ret));

            (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
                translateFileError(ret), op->totalLen);
            return;

        case WE_FILE_ERROR_DELAYED :
            WE_FILE_SELECT( op->fileHandle, WE_FILE_EVENT_READ);
            return;

        default :
            if (ret < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                    "%s(%d): Unhandled file error %d\n",
                    __FILE__, __LINE__, ret));
                (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, 
                    op->instance, translateFileError(ret), op->totalLen);

                return; 
            } /* if */

            op->totalLen += (WE_UINT32)ret;
            op->pos += ret;
            length -= ret;
            break;
        } /* switch */
    } /* for */
} /* doRead */

/*!
 * \brief Do the actual seek
 *
 * \param op Data for an async operation
 * \param startPos Position in file to start reading/writing
 *****************************************************************************/
static void doSeek(const AsyncOperation *op, WE_INT32 startPos)
{
    int seekMode;
    long seekOffset;
    long ret;

    if (op == NULL || SIA_ASYNC_MODE_APPEND == op->mode) 
    {
        /* Seek is not allowed when in append mode */
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
    } /* if */

    if ((ret = WE_FILE_SEEK( op->fileHandle, seekOffset, seekMode)) < 0)
    {
        (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
            translateFileError(ret), op->totalLen);

        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
            "%s(%d): Failed to seek file %d, mode=%d, pos=%d, ret=%d\n", 
            __FILE__, __LINE__, op->fileHandle, seekMode, seekOffset, ret));

        return;
    } /* if */
} /* doSeek */

/*!
 * \brief Do the actual write
 *
 * \param op Data for an async operation
 *****************************************************************************/
static void doWrite(AsyncOperation *op)
{
    int ret;
    int length;

    length = op->buffer - op->pos + (long)op->dataLen;

    /* File is opened and we're positioned so we can start to write */
    for (;;)
    {
        if (length <= 0)    /* Have we written everything from the buffer? */
        {
            (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
                SIA_ASIO_RESULT_OK, op->totalLen);
            return;
        } /* if */

        ret = WE_FILE_WRITE( op->fileHandle, op->pos, length);
        switch (ret)
        {
        case 0 :
        case WE_FILE_ERROR_EOF :
        case WE_FILE_ERROR_ACCESS :
        case WE_FILE_ERROR_FULL :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                "%s(%d): Write returned %d\n", __FILE__, __LINE__, ret));

            (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
                translateFileError(ret), op->totalLen);
            return;

        case WE_FILE_ERROR_DELAYED :
            WE_FILE_SELECT( op->fileHandle, WE_FILE_EVENT_WRITE);
            return;

        default :
            if (ret < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                    "%s(%d): Unhandled file error %d\n",
                    __FILE__, __LINE__, ret));
                (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, 
                    op->instance, translateFileError(ret), op->totalLen);

                return; 
            } /* if */

            op->totalLen += (unsigned long)ret;
            op->pos += ret;
            length -= ret;
            break;
        } /* switch */
    } /* for */
} /* doWrite */

/*!
 * \brief Takes care of a NOTIFY signal (response to an WE_FILE_SELECT operation)
 *
 * \param fileHandle File that the signal is related to
 * \param operationType that the notification was related to
 *****************************************************************************/
static void handleNotify(int fileHandle, int operationType)
{
    AsyncOperation *anOper = myOperationPool;

    while (anOper != NULL)
    {   /* Only opened files can cause NOTIFY signals */
        if (anOper->fileHandle == fileHandle &&
            anOper->mode == SIA_ASYNC_MODE_DISPATCH)
        {
            switch (operationType)
            {
            case WE_FILE_READ_READY :
            case WE_FILE_WRITE_READY :
                (void)SIA_SIGNAL_SENDTO_IU(anOper->fsm, anOper->signal, 
                    anOper->instance, SIA_ASIO_RESULT_OK); 
                smaAsyncDeregister(fileHandle);
                break;
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                    "%s(%d): Illegal operationType %d\n", __FILE__, __LINE__,
                    operationType));
                break;
            } /* switch */

            return;
        }

        if (anOper->state == OPER_STATE_OPENED && 
            anOper->fileHandle == fileHandle)
        {
            switch (operationType)
            {
            case WE_FILE_READ_READY :
                smaAsyncReadContinue(anOper, anOper->dataLen);
                break;
            case WE_FILE_WRITE_READY :
                smaAsyncWriteContinue(anOper, anOper->dataLen);
                break;
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                    "%s(%d): Illegal operationType %d\n", __FILE__, __LINE__,
                    operationType));
                break;
            } /* switch */

            return;
        } /* if */

        anOper = anOper->next;
    } /* while */
    /*lint -e{774} */
    if (anOper == NULL)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Couldn't find handle\n", __FILE__, __LINE__));
    } /* if */
} /* handleNotify */

/*!
 * \brief Takes care of a TERMINATE signal
 *****************************************************************************/
void smaAsyncTerminate(void)
{
    AsyncOperation *anOper = myOperationPool;
    AsyncOperation *next = NULL;

    myOperationPool = NULL;
    myPoolSize = 0;
    
    while (anOper != NULL)
    {
        next = anOper->next;
        SIA_FREE(anOper);
        anOper = next;
    } /* while */

    smaSignalDeregister(SIA_FSM_ASYNC);
} /* handleTerminate */

/*! \brief
 *	
 *****************************************************************************/
static AsyncOperation *asyncGetNewInstance(void)
{
    AsyncOperation *anOper = myOperationPool;
    AsyncOperation *last = NULL;

    /* Search a free operation */
    while (anOper != NULL && anOper->state != OPER_STATE_FREE)
    {
        last = anOper;
        anOper = anOper->next;
    } /* for */

    /* If we didn't find a free item then increase the queue */
    if (anOper == NULL)
    {
        if ((anOper = SIA_ALLOC(sizeof(AsyncOperation))) == NULL)
        {
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                "SMA FSM Async: No free async operations\n"));

            return NULL;
        } /* if */

        if (NULL != last)
        {
            last->next = anOper;
        }
        anOper->previous = last;
        anOper->next = NULL;
        ++myPoolSize;
    } /* if */

    /*
     *	Reset stuff before usage.
     */
/*    anOper->pipeName = NULL;*/
    anOper->fileHandle = 0;
    anOper->pipeHandle = 0;

    return anOper;
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
SiaAsyncOperationHandle smaAsyncPipeOperationStart(const char *pipeName, 
    SiaStateMachine fsm, long instance, int signal, SiaAsyncMode mode)
{
    AsyncOperation *anOper = myOperationPool;

    /* Search a free operation */
    anOper = asyncGetNewInstance();
    if (NULL == anOper)
    {
        return NULL;
    }

    switch (mode)
    {
    case SIA_ASYNC_MODE_READ :
        anOper->pipeHandle = WE_PIPE_OPEN(WE_MODID_SIA, pipeName);
        break;
    case SIA_ASYNC_MODE_WRITE :
        anOper->pipeHandle = WE_PIPE_CREATE(WE_MODID_SIA, pipeName);
        break;
    case SIA_ASYNC_MODE_APPEND:
    case SIA_ASYNC_MODE_READ_WRITE:
    case SIA_ASYNC_MODE_DISPATCH:
    default :
        WE_LOG_MSG((WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
            "%s(%d): Illegal pipe mode %d\n", __FILE__, __LINE__, mode));

        break;
    } /* switch */

    if (anOper->pipeHandle < 0) /* i.e. ERROR! */
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_HIGH, WE_MODID_SIA, 
            "(%s) (%d): Failed to open/create pipe <%s>, %d\n", __FILE__, __LINE__, 
            pipeName == NULL ? "NULL" : pipeName, anOper->pipeHandle));
        return NULL;
    } /* if */

    anOper->fsm = fsm;
    anOper->mode = mode;
    anOper->pipeName = we_cmmn_strdup(WE_MODID_SIA, pipeName);
    anOper->instance = instance;
    anOper->signal = signal;
    anOper->fileHandle = 0;
    anOper->state = OPER_STATE_OPENED;
    anOper->buffer = NULL;
    anOper->dataLen = 0;

    return anOper;
}

/*!
 * \brief Takes care of a NOTIFY signal (response to an WE_FILE_SELECT operation)
 *
 * \param fileHandle File that the signal is related to
 * \param operationType that the notification was related to
 *****************************************************************************/
static void handlePipeNotify(int pipeHandle, int operationType)
{
    AsyncOperation *anOper = myOperationPool;

    while (anOper != NULL)
    {   /* Only opened files can cause NOTIFY signals */
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
                (void)SIA_SIGNAL_SENDTO_IUU(anOper->fsm, anOper->signal, 
                    anOper->instance, SIA_ASIO_RESULT_EOF, anOper->totalLen);
                break;
            default :
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                    "(%s) (%d): Illegal operationType %d\n", __FILE__, __LINE__,
                    operationType));
                break;
            } /* switch */

            return;
        } /* if */

        anOper = anOper->next;
    } /* while */

    WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
        "%s(%d): Couldn't find handle\n", __FILE__, __LINE__));
} /* handlePipeNotify */

/*!
 * \brief Translates an WE File Error to an MMS Result
 *
 * \param fileError Error as given from call to WE FILE function
 *****************************************************************************/
static SiaAsioResult translateFileError(int fileError)
{
    SiaAsioResult ret = SIA_ASIO_RESULT_ERROR;

    switch (fileError)
    {
    case WE_FILE_OK :
        ret = SIA_ASIO_RESULT_OK;
        break;
    case WE_FILE_ERROR_ACCESS : /* e.g. file opened in wrong mode */
        ret = SIA_ASIO_RESULT_ERROR;
        break;
    case WE_FILE_ERROR_DELAYED :
        ret = SIA_ASIO_RESULT_DELAYED;
        break;
    case WE_FILE_ERROR_PATH :
        ret = SIA_ASIO_RESULT_FILE_NOT_FOUND;
        break;
    case WE_FILE_ERROR_INVALID : /* e.g. illegal file name */
        ret = SIA_ASIO_RESULT_ERROR;
        break;
    case WE_FILE_ERROR_SIZE :
        ret = SIA_ASIO_RESULT_INSUFFICIENT_PERSISTENT_STORAGE;
        break;
    case WE_FILE_ERROR_FULL :
        ret = SIA_ASIO_RESULT_INSUFFICIENT_PERSISTENT_STORAGE;
        break;
    case WE_FILE_ERROR_EOF :
        ret = SIA_ASIO_RESULT_EOF;
        break;
    default :
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Unknown fileError %d\n", __FILE__, __LINE__,
            fileError));
        break;
    } /* switch */

    return ret;
} /* translateFileError */

/*!
 * \brief Translates an WE File Error to an MMS Result
 *
 * \param fileError Error as given from call to WE FILE function
 *****************************************************************************/
static SiaAsioResult translatePipeError(int pipeError)
{
    SiaAsioResult ret = SIA_ASIO_RESULT_ERROR;

    switch (pipeError)
    {
    case WE_PIPE_ERROR_BAD_HANDLE :
    case WE_PIPE_ERROR_INVALID_PARAM :
    case WE_PIPE_ERROR_EXISTS :
    case WE_PIPE_ERROR_IS_OPEN :
    case WE_PIPE_ERROR_RESOURCE_LIMIT :
    case WE_PIPE_ERROR_NOT_FOUND :
        ret = SIA_ASIO_RESULT_ERROR;
        break;
    case WE_PIPE_ERROR_DELAYED :
        ret = SIA_ASIO_RESULT_DELAYED;
        break;
    case WE_PIPE_ERROR_CLOSED :
        ret = SIA_ASIO_RESULT_EOF;
        break;
    default :
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Unknown pipeError %d\n", __FILE__, __LINE__,
            pipeError));

        break;
    } /* switch */

    return ret;
} /* translatePipeError */


/*!
 * \brief Do the actual write
 *
 * \param op Data for an async operation
 *****************************************************************************/
static void doPipeWrite(AsyncOperation *op)
{
    long ret;
    long length;

    length = op->buffer - op->pos + (long)op->dataLen;

    /* Pipe is opened so we can start to write */
    for (;;)
    {
        if (length <= 0)    /* Have we written everything from the buffer? */
        {
            (void)SIA_SIGNAL_SENDTO_IUU( op->fsm, op->signal, op->instance, 
                MMS_RESULT_OK, op->totalLen);
            return;
        } /* if */
ILogger
        ret = WE_PIPE_WRITE( op->pipeHandle, op->pos, length);
        switch (ret)
        {
        case WE_PIPE_ERROR_CLOSED :
            WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                "%s(%d): Pipe write returned %d\n", __FILE__, __LINE__, ret));

            (void)SIA_SIGNAL_SENDTO_IUU( op->fsm, op->signal, op->instance, 
                translatePipeError(ret), op->totalLen);
            return;

        case 0 :
        case WE_PIPE_ERROR_DELAYED :
            (void)WE_PIPE_POLL(op->pipeHandle);
            return;

        default :
            if (ret < 0)
            {
                WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA,
                    "%s(%d): Unhandled pipe error %d\n",
                    __FILE__, __LINE__, ret));
                (void)SIA_SIGNAL_SENDTO_IUU( op->fsm, op->signal, op->instance, 
                    translatePipeError(ret), op->totalLen);

                return; 
            } /* if */

            op->totalLen += (WE_UINT32) ret;
            op->pos += ret;
            length -= ret;
            break;
        } /* switch */
    } /* for */
}

/*!
 * \brief Do the actual read
 *
 * \param op Data for an async operation
 *****************************************************************************/
static void doPipeRead(AsyncOperation *op)
{
    int ret;
    long length;

    length = op->buffer - op->pos + (long)op->dataLen;

    /* Pipe is opened so we can start to read */
    for (;;)
    {
        if (length <= 0) /* We have read everything that we should */
        {
            (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
                SIA_ASIO_RESULT_OK, op->totalLen);
            return;
        } /* if */

        ret = WE_PIPE_READ( op->pipeHandle, op->pos, length);
        switch (ret)
        {
        case WE_PIPE_ERROR_CLOSED :
            (void)SIA_SIGNAL_SENDTO_IUU( op->fsm, op->signal, op->instance, 
                SIA_ASIO_RESULT_EOF, op->totalLen);
            return;

        case WE_PIPE_ERROR_NOT_FOUND :
            WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                "%s(%d): Pipe read returned %d\n", __FILE__, __LINE__, ret));

            (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
                translatePipeError(ret), op->totalLen);
            return;

        case 0 :
        case WE_PIPE_ERROR_DELAYED :
            (void)WE_PIPE_POLL(op->pipeHandle);
            return;

        default :
            if (ret < 0)
            {
                WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
                    "%s(%d): Unhandled pipe error %d\n",
                    __FILE__, __LINE__, ret));
                (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
                    translatePipeError(ret), op->totalLen);

                return; 
            } /* if */

            op->totalLen += (WE_UINT32)ret;
            op->pos += ret;
            length -= ret;
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
void smaAsyncPipeRead(SiaAsyncOperationHandle oper, void *buffer, 
    WE_UINT32 bufferSize)
{
    AsyncOperation *op = (AsyncOperation *)oper;

    if (op == NULL || op->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Wrong handle or state for pipe read, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        return;
    }
    else if (op->pipeHandle < 0)
    {
        WE_LOG_MSG(( WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Pipe was not opened, check previous errors, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
            SIA_ASIO_RESULT_ERROR, op->totalLen);
        return;
    } /* if */

    /* OK, pipe is opened. Prepare for reading */
    op->totalLen = 0;
    op->pos = buffer;
    op->buffer = buffer;
    op->dataLen = bufferSize;

    doPipeRead(op);
}

/*! \brief Async pipe operations using buffer.
 *
 * \param oper The current operation.
 * \param buffer The buffer to read data from.
 * \param bufferSize The size of the buffer.
 *****************************************************************************/
void smaAsyncPipeWrite(SiaAsyncOperationHandle oper, void *buffer, 
    WE_UINT32 bufferSize)
{
    AsyncOperation *op = (AsyncOperation *)oper;

    if (op == NULL || op->state != OPER_STATE_OPENED)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Wrong handle or state for pipe write, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        return;
    }
    else if (op->pipeHandle < 0)
    {
        WE_LOG_MSG((WE_LOG_DETAIL_MEDIUM, WE_MODID_SIA, 
            "%s(%d): Pipe was not opened, check previous errors, op=%d, state=%d\n", 
            __FILE__, __LINE__, op, op == NULL ? -1 : op->state));

        (void)SIA_SIGNAL_SENDTO_IUU(op->fsm, op->signal, op->instance, 
            SIA_ASIO_RESULT_ERROR, op->totalLen);
        return;
    } /* if */

    /* OK, pipe is opened. Prepare for reading writing */
    op->totalLen = 0;
    op->pos = buffer;
    op->buffer = buffer;
    op->dataLen = bufferSize;

    doPipeWrite(op);
}

