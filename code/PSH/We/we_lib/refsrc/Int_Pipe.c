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
 
#ifdef _cplusplus
extern "C" {
#endif

/*---------- Dependencies --------------------------------------*/
#ifdef WIN32
#include "windows.h"
//#include "portab_new.h"
#else
#include "wcdtypes.h"
#include "portab.h"
#endif

#include "We_Env.h"
#include "We_Cfg.h"
#include "We_Int.h"

#include "int_pipe.h"
#include "string.h"
#include "AEEStdLib.h"
//#include "SP_sysutils.h"

/*---------- Global Definitions and Declarations -----------------------*/
extern void
TPIc_PipeNotify (WE_UINT8 modId, int handle, int eventType);

/*---------- Constant / Macro Definitions ------------------------------*/
/* [TMT Support] :
 * a pipe must not be less than 300 bytes.
 * Regarding limit of simultaneously pipes there are no maxlimit and the size
 * of the pipe is up to the integration. If the size of the pipe is 500 bytes
 * then you can write max 500 bytes to the pipe (when it is empty).
*/
#define AU_PIPE_DATA_BUF_SIZE    1024

/* [TMT Support]: 
 * The minimum required pipesize is in average 3 simultaneous pipes.
 */
#define AU_MAX_PIPES_NUM        8            

/*---------- Type Declarations -----------------------------------------*/

/*---------- Variable Declarations -------------------------------------*/
static PipeStruct    PipeArray[AU_MAX_PIPES_NUM];

/*---------- function Declarations -------------------------------------*/
PipeStruct *GetPipeByHandle(int handle);
PipeStruct *GetPipeByName(const char* name,int* pipeIdx);
void ReadData(WE_UINT8 *pDstDataBuf, 
                long       iReadDataLen,
                long      *iSrcDataOffset,
                WE_UINT8 *pSrcDataBase,
                long      iSrcDataBufSize);
                
void WriteData(WE_UINT8    *pSrcDataBuf, 
                long         iWritenDataLen,
                long         iDstDataOffset,
                WE_UINT8    *pDstDataBase,
                long         iDstDataBufSize );
void FreePipeResource (PipeStruct *pPipe);

/*******************************************************************
 FUNCTION   :  PipeCreate 
--------------------------------------------------------------------
 DESCRIPTION:  Create a named pipe for writing. The handle returned must be system-wide
               unique; in particular, it is important that the handle returned from 
               this function be different from that returned by HDIa_pipeOpen (even if 
               it is for the same pipe).
--------------------------------------------------------------------
 INPUT        :
    modId    : ID of the calling module
     name    : Name of the pipe
--------------------------------------------------------------------
 RETURN        : 
    RETURNS Handle, or a negative value on error. Possible error codes are:
    TPI_PIPE_ERROR_INVALID_PARAM : modId is not a valid module ID, or
                                   name is not a valid path name for a pipe.
    TPI_PIPE_ERROR_EXISTS : A pipe with the specified name exists already
    TPI_PIPE_ERROR_RESOURCE_LIMIT : Too many pipes have already been opened, or
                                    other resource limits prevent the creation 
                                    of a new pipe.
--------------------------------------------------------------------
 IMPORTANT NOTES:    
    
********************************************************************/
int PipeCreate (WE_UINT8 uiModId, const char* pcName)
{
    PipeStruct *pPipe;
    WE_UINT8   iPipeIdx;            
    WE_UINT8   iFreePipe;        /* index of free pipe */
    WE_UINT32  iPipeNameLen = 0;

    /* Ensure that pipes number should be less than AU_MAX_PIPES_NUM */
    iFreePipe = AU_MAX_PIPES_NUM;            

    /* 
     * verify module ID. 
     */
    if(uiModId >= WE_NUMBER_OF_MODULES)    
    {
        //op_printf( "[ERROR!]<PipeCreate> modId >= MSF_NUMBER_OF_MODULES\n");
        return TPI_PIPE_ERROR_INVALID_PARAM;
    }

    /* 
     * verify pipe name. note: If name is OP_NULL, there are no corresponding errors
     * to be returned. Perhaps it has been verified before called this function.
     */
    if( NULL == pcName )
    {
        //op_printf( "[ERROR!]<PipeCreate> OP_NULL == name\n");
        return TPI_PIPE_ERROR_INVALID_PARAM;
    }

    /*
     * Get one free pipe and check whether pipe name is unique or not. 
     * If this name is the same with the name of pipe which has been marked with 
     * deleted flag, this name is unique also. Name resource of pipe whose deleted
     * flag has been marked should be recycle.
     */
    for(iPipeIdx = 0; iPipeIdx < AU_MAX_PIPES_NUM; iPipeIdx++)
    {
        if( ! PipeArray[iPipeIdx].iValid )
        {
            if(iFreePipe == AU_MAX_PIPES_NUM)
            {
                iFreePipe = iPipeIdx;
            }
        }
        else
        {
            if ( STRCMP((const char*)PipeArray[iPipeIdx].pStrPipeName,pcName) == 0 &&
                 ! PipeArray[iPipeIdx].iDeleted )        /* ChenJS added. 2003.3.29 */ 
            {
                //op_printf( "[ERROR!]<PipeCreate> TPI_PIPE_ERROR_EXISTS\n");
                return TPI_PIPE_ERROR_EXISTS;
            }
        }
    }
    
    /* 
     * No free pipes left.
     */
    if(iFreePipe == AU_MAX_PIPES_NUM)    
    {
        //op_printf( "[ERROR!]<PipeCreate> No free pipes left.\n");
        return TPI_PIPE_ERROR_RESOURCE_LIMIT;
    }

    /*
     * Initialize a new pipe 
     */
    pPipe = PipeArray + iFreePipe;

    pPipe->iValid         = 1;
    pPipe->iDeleted         = 0;
    pPipe->iReadModId     = WE_NUMBER_OF_MODULES;
    pPipe->iReadHandle   = 0;
    pPipe->iValidDataLen = 0;
    pPipe->iValidDataPosIdx = 0; 
    pPipe->iWriteModId     = uiModId;

    /* 
     * Pipe's handle can be maped to pipe's index in pipe array. e.g: if pipe's 
     * read side hanle is 0x02, its write end handle is 0x02FFFF(that's 0x02<<16),
     * and this pipe index in pipe array is 1. Because 0 is regarded as invalid
     * handle, pipe's read side handle is euqal to the value of pipe's index plusing
     * 1, pipe's write side handle is it's read side handle after shifting 16 bits left.
     */
    pPipe->iWriteHandle = ((iFreePipe+1) << 16);

    pPipe->iWriteEndPolled = 0;
    pPipe->iReadEndPolled  = 0;
    pPipe->iDataBufSize = AU_PIPE_DATA_BUF_SIZE;
    pPipe->pDataBufBase = (WE_UINT8*)MALLOC((WE_UINT32)pPipe->iDataBufSize);
    if(NULL == pPipe->pDataBufBase)
    {
        memset((void*)pPipe,0,sizeof(PipeStruct));        /* Added on 27.Mar.2003 */
        //op_printf( "[ERROR!]<PipeCreate>TPI_PIPE_ERROR_RESOURCE_LIMIT.\n");
        return TPI_PIPE_ERROR_RESOURCE_LIMIT;
    }
    memset(pPipe->pDataBufBase, 0, pPipe->iDataBufSize);

    iPipeNameLen = STRLEN(pcName)+1;
    pPipe->pStrPipeName    = (char*)MALLOC(iPipeNameLen);  
    if(NULL == pPipe->pStrPipeName)
    {
        FREE(pPipe->pDataBufBase); 
    	memset((void*)pPipe,0,sizeof(PipeStruct));
        //op_printf( "[ERROR!]<PipeCreate>TPI_PIPE_ERROR_RESOURCE_LIMIT.\n");
        return TPI_PIPE_ERROR_RESOURCE_LIMIT;
    }
    STRCPY(pPipe->pStrPipeName,pcName);

    return pPipe->iWriteHandle;
}
/*******************************************************************
 FUNCTION   :  PipeOpen
--------------------------------------------------------------------
 DESCRIPTION:  Open the pipe for reading.
--------------------------------------------------------------------
 INPUT        :
    modId    : ID of the calling module
     name    : Name of the pipe
--------------------------------------------------------------------
 RETURN        : 
    Handle, or a negative value on error. Possible error codes are:
    TPI_PIPE_ERROR_INVALID_PARAM :  modId is not a valid module ID, 
                                    or name is not a valid path name for a pipe.
    TPI_PIPE_ERROR_IS_OPEN : The pipe with the specified name is already open for
                              reading.
    TPI_PIPE_ERROR_NOT_FOUND : No pipe with the specified path name was found.
--------------------------------------------------------------------
 IMPORTANT NOTES:    
        This function can be called only by read side.
********************************************************************/
int PipeOpen (WE_UINT8 uiModId, const char *pcName)
{
    PipeStruct *pstPipe;
    int iPipeIdx;


    /* verify module ID */
    if(uiModId >= WE_NUMBER_OF_MODULES)        
    {
        return TPI_PIPE_ERROR_INVALID_PARAM;
    }

    /* 
     * verify pipe name.note: If name is OP_NULL, there are no corresponding errors
     * to be returned. Perhaps it has been verified before calling this function.
     */
    if( NULL == pcName )
    {
        return TPI_PIPE_ERROR_INVALID_PARAM;
    }


    pstPipe = GetPipeByName(pcName,&iPipeIdx);
    if(NULL == pstPipe)
    {
        return TPI_PIPE_ERROR_NOT_FOUND;
    }

    if(pstPipe->iReadHandle )
    {
        return TPI_PIPE_ERROR_IS_OPEN;
    }
    else
    {
        pstPipe->iReadModId    = uiModId;
        pstPipe->iReadHandle    = iPipeIdx + 1;
        return pstPipe->iReadHandle;
    }
}

/*******************************************************************
 FUNCTION   :  PipeClose
--------------------------------------------------------------------
 DESCRIPTION:   Close the pipe. Closing a pipe does not actually remove it from
                the system. A pipe that has been closed can be opened again, as 
                long as it has not been deleted by calling HDIa_pipeDelete.
--------------------------------------------------------------------
 INPUT        :
    handle    : Pipe handle
--------------------------------------------------------------------
 RETURN        : 
    0, or a negative value on error. Possible error codes are:
    TPI_PIPE_ERROR_BAD_HANDLE : There is no open pipe with the specified handle
--------------------------------------------------------------------
 IMPORTANT NOTES:    
    Parameter(handle) may be either read end handle or write end handle.

    Q: Could a writer create a pipe, and then close it before a reader open it?
    A: [TMT Support] Yes.
********************************************************************/
int PipeClose (int iHandle)
{

    PipeStruct *pstPipe;

    pstPipe = GetPipeByHandle(iHandle);
    if(NULL == pstPipe)
    {
        return TPI_PIPE_ERROR_BAD_HANDLE;
    }

    /* Read end handle */
    if( iHandle == pstPipe->iReadHandle )                                                
    {
        pstPipe->iReadHandle = 0;
        
        /* 
         * If write end has polled and write end is open and pipe is deleted, send 
         * notification to write end.
         */
        if(pstPipe->iWriteEndPolled && pstPipe->iWriteHandle && pstPipe->iDeleted )
        {
            TPIc_PipeNotify(pstPipe->iWriteModId,pstPipe->iWriteHandle ,TPI_PIPE_EVENT_CLOSED);
            pstPipe->iWriteEndPolled = 0; 
        }
    }
    else    /* Write end */
    {
        pstPipe->iWriteHandle  = 0;

        /*
         * If read end has polled and is open,send notification to read end. 
         */
        if(pstPipe->iReadEndPolled && pstPipe->iReadHandle )
        {
            TPIc_PipeNotify(pstPipe->iReadModId,pstPipe->iReadHandle,TPI_PIPE_EVENT_CLOSED);
            pstPipe->iReadEndPolled = 0; 
        }
    }


    /* 
     * Free pipe resource .  2003.3.19
     */
    if( ! pstPipe->iWriteHandle &&  ! pstPipe->iReadHandle && pstPipe->iDeleted )    
    {
        FreePipeResource(pstPipe);
    }
    
    return 0;
}

/*******************************************************************
 FUNCTION   :  PipeDelete                                    
--------------------------------------------------------------------
 DESCRIPTION:   Delete the pipe, i.e., remove it and free any resources associa
                ted with it. If the pipe is currently open by some module, the 
                actual deletion should be postponed until all modules have closed 
                their handles on this pipe. However, after a call to HDIa_pipeDelete,
                it is no longer possible to open the pipe. A call to HDIa_pipeCreate
                using the same name as in HDIa_pipeDelete, should create a new, 
                distinct pipe.
--------------------------------------------------------------------
 INPUT        :
    name    : Name of the pipe
    handle    : Pipe handle
--------------------------------------------------------------------
 RETURN        : 
    0, or a negative value on error. Possible error codes are:
    TPI_PIPE_ERROR_NOT_FOUND : No pipe with the specified path name was found.
--------------------------------------------------------------------
 IMPORTANT NOTES:    
    [TMT Support]: HDIa_pipeDelete() can't be called by the write end.
    Q: Could a module prevent a pipe from being open again by calling HDIa_pipeDelete() 
        before HDIa_pipeClose()?
    A: [TMT Support] It is true, you could delete a pipe before it is closed but it
        will not be removed until the writer has closed it. We recommend the the
        reader to close the pipe before it is deleted
********************************************************************/
int PipeDelete (const char* pcName)
{
    PipeStruct *pstPipe;

    /* 
     * verify pipe name.note: If name is OP_NULL, there are no corresponding errors
     * to be return. Perhaps it has been verified before called this function.
     */
    if( NULL == pcName )
    {
        return TPI_PIPE_ERROR_NOT_FOUND;
    }

    pstPipe = GetPipeByName(pcName,NULL);

    if(NULL == pstPipe)
    {
        return TPI_PIPE_ERROR_NOT_FOUND;
    }

    if( pstPipe->iReadHandle  == 0 && 
        pstPipe->iWriteHandle == 0  )
    {
        FreePipeResource(pstPipe);
    }
    else
    {
        pstPipe->iDeleted = 1;
    }

    return 0;
}

/*******************************************************************
 FUNCTION   :  PipeRead
--------------------------------------------------------------------
 DESCRIPTION:  Read from a pipe.
--------------------------------------------------------------------
 INPUT        :
    handle  : Pipe handle
    buf        : The buffer in which to put the data read from the stream
    bufSize : The number of bytes to read
--------------------------------------------------------------------
 RETURN        : 
    Number of bytes read, or a negative value on error. Possible error codes are:
    TPI_PIPE_ERROR_BAD_HANDLE
            There is no open pipe with the specified handle.
    TPI_PIPE_ERROR_DELAYED
            No data was available for reading, but the write end of
            the pipe is still open (hence, more data may arrive at a
            later time).
    TPI_PIPE_ERROR_CLOSED
            No data was available for reading, AND the write end of
            the pipe has been closed.
--------------------------------------------------------------------
 IMPORTANT NOTES:    
     1> If the valid data byte count in pipe is less than that requested by reader,
       data will be read until pipe is empty.
********************************************************************/
long PipeRead (int iHandle, void *pvBuf, long lBufSize)
{
    PipeStruct *pstPipe;
    long        lDataLen;

    pstPipe = GetPipeByHandle(iHandle);

    if( NULL == pstPipe || pstPipe->iReadHandle != iHandle ) 
    {
        return TPI_PIPE_ERROR_BAD_HANDLE;
    }


    if(pstPipe->iValidDataLen == 0 )
    {
        if( !pstPipe->iWriteHandle )                        /* Write end has closed */
        {
            return TPI_PIPE_ERROR_CLOSED;
        }
        else
        {
            return TPI_PIPE_ERROR_DELAYED;
        }
    }
    else
    {
        if(pstPipe->iValidDataLen <= lBufSize)
        {
            lDataLen = pstPipe->iValidDataLen;
        }
        else
        {
            lDataLen = lBufSize;
        }

        if( lDataLen != 0 )
        {
            ReadData(    (WE_UINT8*)pvBuf,
                            lDataLen,
                            &(pstPipe->iValidDataPosIdx) ,
                            pstPipe->pDataBufBase,
                            pstPipe->iDataBufSize );
            pstPipe->iValidDataLen -= lDataLen;

            if( pstPipe->iWriteEndPolled && pstPipe->iWriteHandle )
            {
                TPIc_PipeNotify(pstPipe->iWriteModId,pstPipe->iWriteHandle ,TPI_PIPE_EVENT_WRITE);
                pstPipe->iWriteEndPolled = 0;
            }
        }

        return lDataLen;
    }
}

/*******************************************************************
 FUNCTION   :  PipeWrite                                     
--------------------------------------------------------------------
 DESCRIPTION:  Write to a pipe.
--------------------------------------------------------------------
 INPUT        :
    handle  : Pipe handle
    buf        : The buffer where the data to write into the stream can be found
    bufSize : The number of bytes to write
--------------------------------------------------------------------
 RETURN        : 
        Number of bytes written, or a negative value on error. Possible
    error codes are:
    TPI_PIPE_ERROR_BAD_HANDLE : There is no open pipe with the specified handle
    TPI_PIPE_ERROR_DELAYED    : No room was avaialable for writing, but it may become
        possible to write more at a later time (that is, either some
        process has the pipe open for reading, or at least the pipe
        has not been deleted).
    TPI_PIPE_ERROR_CLOSED    : The read end of the pipe is closed, AND the pipe has
        been deleted (hence cannot be opened again).
--------------------------------------------------------------------
 IMPORTANT NOTES:    
     1> If the number of free data bytes in pipe is less than that requested by writer,
      the actual written data length is the size of free data buffer.

    Q: You have said that to write pipe, the writer has to wait until the pipe
       is opened by a reader. So what should be return by HDIa_pipeWrite() for such
       a case? 
    A: [TMT Support] If the pipe is not opened and the writer tries to write the
       return value will be TPI_PIPE_ERROR_DELAYED
    Q: But this is not compliant to the explanation for TPI_PIPE_ERROR_DELAYED in
        your documentation.
    A: [TMT Support] I'm sorry, I misunderstood you question. It is correct, you
        can start writing to a pipe before the reader has opened it. When the pipe
        is "filled" the return value will be TPI_PIPE_ERROR_DELAYED
********************************************************************/
long PipeWrite (int iHandle, void *pvBuf, long lBufSize)
{
    PipeStruct * pstPipe;
    long        lDataLen;

    pstPipe = GetPipeByHandle(iHandle);

    if( NULL == pstPipe || pstPipe->iWriteHandle != iHandle) 
    {
        return TPI_PIPE_ERROR_BAD_HANDLE;
    }

    if( pstPipe->iReadHandle == 0 && pstPipe->iDeleted )        
    {
        return TPI_PIPE_ERROR_CLOSED;
    }

    if(lBufSize == 0 ) 
    {
        return lBufSize;
    }

    lDataLen = pstPipe->iDataBufSize - pstPipe->iValidDataLen ;

    if(lDataLen == 0 )
    {
            return TPI_PIPE_ERROR_DELAYED;
    }
    else     
    {
        if( lDataLen > lBufSize )
        {
            lDataLen = lBufSize ;
        }

        WriteData((WE_UINT8*)pvBuf,
                   lDataLen,
                   (pstPipe->iValidDataPosIdx + pstPipe->iValidDataLen) % pstPipe->iDataBufSize ,
                   pstPipe->pDataBufBase,
                   pstPipe->iDataBufSize);
        pstPipe->iValidDataLen += lDataLen;

        if( pstPipe->iReadEndPolled && pstPipe->iReadHandle )
        {
            TPIc_PipeNotify(pstPipe->iReadModId,pstPipe->iReadHandle ,TPI_PIPE_EVENT_READ);
            pstPipe->iReadEndPolled = 0;
        }
    }

    return lDataLen;
}


/*******************************************************************
 FUNCTION   :  PipeCloseAll                                     
--------------------------------------------------------------------
 DESCRIPTION:  Deletes all pipes that a module has opened. The integration must
    handle the deleting of each pipe exactly in the same way as in the 
    HDIa_pipeDelete function. This function can be called even if the module has no 
    open pipes.
--------------------------------------------------------------------
 INPUT        :
    modId    : Module ID.
--------------------------------------------------------------------
 RETURN        : void
--------------------------------------------------------------------
 IMPORTANT NOTES:    
     Q: Will HDIa_pipeCloseAll() effect those pipes created by this specified
        module? Or just those opened by this module as a reader?
    A: [TMT Support] HDIa_pipeCloseAll() should have a behaviour dependent on of
        the module is the reader or writer for each pipe. The pipes that the calling
        module reads from should be closed and deleted (HDIa_pipeClose followed by
        HDIa_pipeDelete) but the pipes the calling moudule writes to shuld only be
        closed (HDIa_pipeClose).
********************************************************************/

void PipeCloseAll (WE_UINT8 uiModId)
{
    WE_UINT8 uiPipeIdx;
 
    for(uiPipeIdx = 0; uiPipeIdx < AU_MAX_PIPES_NUM; uiPipeIdx++)
    {
        if( ! PipeArray[uiPipeIdx].iValid )
            continue;
 
        if( PipeArray[uiPipeIdx].iReadModId == uiModId)
        {
            PipeClose(PipeArray[uiPipeIdx].iReadHandle);
            PipeDelete(PipeArray[uiPipeIdx].pStrPipeName);        
        }
        else if(PipeArray[uiPipeIdx].iWriteModId == uiModId )
        {
            //op_printf( "<PipeCloseAll> close write pipe \n");

            PipeClose(PipeArray[uiPipeIdx].iWriteHandle);
            if ( 0 == PipeArray[uiPipeIdx].iReadModId || 
                WE_MODID_STK == uiModId  )
            {
                //op_printf( "<PipeCloseAll>PipeDelete write \n");
                PipeDelete(PipeArray[uiPipeIdx].pStrPipeName);        
            }
        }
    }    
 
    return;

}

/*******************************************************************
 FUNCTION   :  PipePoll                                     
--------------------------------------------------------------------
 DESCRIPTION:  Request to be notified of change in status of the pipe. The integration 
    should detect whether the request is for the read end (in which case notification 
    should be done when the pipe becomes non-empty) or the write end (in which case 
    notification should be done when the pipe is no longer full). In both cases, if the 
    opposite end is closed     before the desired change of status has occurred, the 
    integration should notify the calling module of the closing. Notification should be 
    carried out by calling the connector function TPIc_PipeNotify.
--------------------------------------------------------------------
 INPUT        :
    handle    : Pipe handle.
--------------------------------------------------------------------
 RETURN        : 
    0, or a negative value on error. Possible error codes are:
    TPI_PIPE_ERROR_BAD_HANDLE : There is no open pipe with the specified handle.
--------------------------------------------------------------------
 IMPORTANT NOTES:    OP_NULL
********************************************************************/

int PipePoll (int iHandle)
{
    PipeStruct * pstPipe;

    pstPipe = GetPipeByHandle(iHandle);
    if( NULL == pstPipe)
        return TPI_PIPE_ERROR_BAD_HANDLE;

    if( iHandle == pstPipe->iReadHandle )           /* Read end  */
        pstPipe->iReadEndPolled = 1;
    else                                            /* Write end */
        pstPipe->iWriteEndPolled = 1;

    return 0;
}

/*******************************************************************
 FUNCTION   :  PipeStatus                                     
--------------------------------------------------------------------
 DESCRIPTION: Inquire about the status of a pipe.
--------------------------------------------------------------------
 INPUT        :
    handle : Pipe handle
    isOpen :    If handle is the write end of a pipe, this parameter (provided it
                is not OP_NULL) should be set to OP_TRUE if the read end is open.
                Similarly, if handle is the read end of a pipe, this parameter
                should be set to OP_TRUE if the write end is open. Otherwise, it
                should be set to OP_FALSE.
    available : If handle is the write end of a pipe, this parameter (provided it
                is not OP_NULL) should be set to the number of bytes that can be
                written in the next call to HDIa_pipeWrite. Similarly, if handle
                is the read end of a pipe, this parameter should be set to the
                number of bytes that are presently available to the reader.
--------------------------------------------------------------------
 RETURN        : 
    The value 0 on success and a negative value if an error
    occurred. Possible error codes are:
    TPI_PIPE_ERROR_BAD_HANDLE : There is no open pipe with the specified handle.
--------------------------------------------------------------------
 IMPORTANT NOTES: OP_NULL
********************************************************************/

int PipeStatus (int iHandle, int *piIsOpen, long *plAvailable)
{
    PipeStruct * pstPipe;

    pstPipe = GetPipeByHandle(iHandle);
    if( NULL == pstPipe)
    {
        return TPI_PIPE_ERROR_BAD_HANDLE;
    }


    if( pstPipe->iReadHandle == iHandle)                    /* Read end */
    {
        if(piIsOpen)                      
        {
            if( pstPipe->iWriteHandle )
                *piIsOpen     = TRUE;
            else *piIsOpen = FALSE;
        }
        
        if(plAvailable)
        {
            *plAvailable = pstPipe->iValidDataLen ;
        }
    }
    else                                                /* Write end */
    {
        if(piIsOpen)
        {
            if( pstPipe->iReadHandle )
                *piIsOpen  = TRUE;
            else *piIsOpen = FALSE;
        }

        if(plAvailable)
        {
            *plAvailable = pstPipe->iDataBufSize - pstPipe->iValidDataLen ;
        }
    }

    return 0;
}

/*******************************************************************
 FUNCTION   :  GetPipeByHandle                                     
--------------------------------------------------------------------
 DESCRIPTION: Get pipe with specify handle.
--------------------------------------------------------------------
 INPUT        :
    handle  : Pipe handle
--------------------------------------------------------------------
 RETURN        : 
    OP_NULL    : bad handle.
    others    : pipe handle.
--------------------------------------------------------------------
 IMPORTANT NOTES: OP_NULL
********************************************************************/
PipeStruct *GetPipeByHandle(int handle)
{
    WE_UINT8 iPipeIdx;
    WE_UINT32 tempHandle;

    if (handle <= 0)
    {
        return NULL;
    }

    tempHandle = (WE_UINT32)handle;
    iPipeIdx = tempHandle > 0XFFFF ? ( (tempHandle>>16) - 1)  : ( tempHandle - 1 );

    /* Verify handle */
    if( iPipeIdx >= AU_MAX_PIPES_NUM ||                 
        !PipeArray[iPipeIdx].iValid )
    {
        return NULL;
    }
    else 
    {
        return PipeArray + iPipeIdx;
    }
}
/*******************************************************************
 FUNCTION   :  GetPipeByName                                     
--------------------------------------------------------------------
 DESCRIPTION: Get pipe with specify handle.
--------------------------------------------------------------------
 INPUT        :
    name : Pipe name.
    pipeIdx : address for the pipe index to be returned.If this address 
              is OP_NULL, ignore this parameter.
--------------------------------------------------------------------
 RETURN        : 
    OP_NULL    : bad handle.
    others    : pipe handle.
--------------------------------------------------------------------
 IMPORTANT NOTES: OP_NULL
********************************************************************/
PipeStruct *GetPipeByName(const char* name,int* pipeIdx)
{
    WE_UINT8 iPipeIdx;
    PipeStruct *pPipe;

    pPipe = NULL;

    for(iPipeIdx = 0; iPipeIdx < AU_MAX_PIPES_NUM; iPipeIdx++)
    {
        if(PipeArray[iPipeIdx].iValid )
        {
            if( strcmp((const char*)PipeArray[iPipeIdx].pStrPipeName,name) == 0 &&
                ! PipeArray[iPipeIdx].iDeleted )                    /* 2003.3.19*/
            {
                pPipe = PipeArray + iPipeIdx;
                if(pipeIdx)
                {
                    *pipeIdx = iPipeIdx;
                }
                break;
            }
        }
    }    

    return pPipe;
}

/*******************************************************************
 FUNCTION   :  ReadData                                     
--------------------------------------------------------------------
 DESCRIPTION:  Read from a pipe.
--------------------------------------------------------------------
 INPUT        :
    pDstDataBuf        : Destination buffer for accepting read data.
    iReadDataLen    : Count of Data bytes to be read.
    iSrcDataOffset    : The offset position of source data buffer where data
                      will be read from.
    pSrcDataBase    : The base address of source data buffer.
    iSrcDataBufSize    : The size of source data buffer.
--------------------------------------------------------------------
 RETURN        : void
--------------------------------------------------------------------
 IMPORTANT NOTES:    
    1> iSrcDataOffset is an address. Any modification to var with this 
       address will be returned. So iSrcDataOffset is variable parameter.
    2> If the valid data byte count in source data buffer is less than 
       that requested by reader,data will be read until data buffer is empty.
********************************************************************/
void ReadData(    WE_UINT8      *pDstDataBuf, 
                long           iReadDataLen,
                long          *iSrcDataOffset,
                WE_UINT8      *pSrcDataBase,
                long           iSrcDataBufSize)
{
        WE_UINT8 *pDstIdx,*pSrcIdx;
        long iLen;

        pDstIdx = pDstDataBuf;
        pSrcIdx = (WE_UINT8*)(pSrcDataBase + *iSrcDataOffset);

        iLen = iSrcDataBufSize - *iSrcDataOffset;
        if( iLen  < iReadDataLen)
        {
            memcpy(pDstIdx,pSrcIdx,(WE_UINT32)iLen);
            pDstIdx += iLen;
            pSrcIdx = pSrcDataBase;
            memcpy(pDstIdx,pSrcIdx,(WE_UINT32)(iReadDataLen - iLen));
            *iSrcDataOffset = iReadDataLen - iLen;
        }
        else
        {
            memcpy(pDstIdx,pSrcIdx,(WE_UINT32)iReadDataLen);

            if( iLen == iReadDataLen )
            {
                *iSrcDataOffset = 0;
            }
            else
            {
                *iSrcDataOffset += iReadDataLen;
            }
        }

        return;
        
}
/*******************************************************************
 FUNCTION   :  WriteData                                     
--------------------------------------------------------------------
 DESCRIPTION:  Read from a pipe.
--------------------------------------------------------------------
 INPUT        :
    pSrcDataBuf        : The base address of source data buffer.
    iWritenDataLen    : Bytes count of data to be written.    
    iDstDataOffset    : The offset position of destination data buffer 
                       where data will be written from.
    pDstDataBase    : The base address of destination data buffer.
    iDstDataBufSize    : The size of destination data buffer.
--------------------------------------------------------------------
 RETURN        : void
--------------------------------------------------------------------
 IMPORTANT NOTES:    
********************************************************************/
void WriteData(WE_UINT8        *pSrcDataBuf, 
                long             iWritenDataLen,
                long             iDstDataOffset,
                WE_UINT8        *pDstDataBase,
                long             iDstDataBufSize )
{
        WE_UINT8 *pDstIdx,*pSrcIdx;
        long iLen;

         pSrcIdx = pSrcDataBuf;
         pDstIdx = (WE_UINT8*)(pDstDataBase + iDstDataOffset);

        iLen = iDstDataBufSize - iDstDataOffset;
        if( iLen  < iWritenDataLen)
        {
            memcpy(pDstIdx,pSrcIdx,(WE_UINT32)iLen);
            pSrcIdx += iLen;
            pDstIdx = pDstDataBase;
            memcpy(pDstIdx,pSrcIdx,(WE_UINT32)(iWritenDataLen - iLen));
        }
        else
        {
            memcpy(pDstIdx,pSrcIdx,(WE_UINT32)iWritenDataLen);
        }

        return;            
}

/*******************************************************************
 FUNCTION   :  FreePipeResource                                     
--------------------------------------------------------------------
 DESCRIPTION:  Free pipe source.
--------------------------------------------------------------------
 INPUT        :
    pPipe   : pipe address.
--------------------------------------------------------------------
 RETURN        : void
--------------------------------------------------------------------
 IMPORTANT NOTES:    
********************************************************************/
void FreePipeResource (PipeStruct *pPipe)
{
    free(pPipe->pStrPipeName );
    free(pPipe->pDataBufBase );
    memset((void*)pPipe,0,sizeof(PipeStruct));
}

/***************************end********************************/
#ifdef _cplusplus
}
#endif





















