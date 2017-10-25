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
/*
 * We_Int.c
 *
 */

#include "We_Int.h"
#include "We_Lib.h"
#include "AEEShell.h"    /* AEE Shell Services*/
#include "AEEFile.h"    /*AEEFile Services*/
#include "AEEMenu.h"    /* Menu Services */
#include "AEEStdLib.h"   /* AEE stdlib services */
#include "We_Def.h"
#include "We_Cfg.h"
#include "We_Core.h"
#include "Int_Pipe.h"
#include "AEEStdLib.h"
#include "AEENet.h"
#include "AEEClassIDs.h"
#include "AEEComdef.h"

/**********************************************************************
 * type define
 **********************************************************************/
/*Max count of signals*/
#define MAX_COUNT_OF_SIGNAL   150

typedef struct tagSt_SignalNode
{
    /*Length of the signal (pvSigalData data size)*/
    WE_UINT32 uiDataLength;
    /*Signal data pointer*/
    void *pvSigalData;
    /*Next node of the queue, if there is no next node, set it with NULL*/
    struct tagSt_SignalNode *pstNext;
    /*Jydge the node was used or not 0:Not used 1:Used*/
    WE_BOOL bStatus;
}St_SignalNode, *P_St_SignalNode;

typedef struct tagSt_SignalHeader
{
    /*Number of this module signals*/
    WE_UINT32 uiNum;
    /*Judge the module was registered or not 0:Not Registered 1:Registered */
    WE_BOOL bReg;
    /*Signal queue, point to the header of Signal node pool*/
    P_St_SignalNode pSignalHead;
    /*The last one of the signal node pool*/
    P_St_SignalNode pSignalLast;
}St_SignalHeader, *P_St_SignalHeader;

typedef struct tagSt_Signal
{
    /*Signal header array, each module has one header, first it need to be 
    set with NULL*/
    St_SignalHeader header[WE_NUMBER_OF_MODULES];
    /*Signal pool pointer, first it must be set with NULL, once need it, init 
    it by allocating memory, 
    if all queues are unregistered, the pool need to be freed*/
    P_St_SignalNode pSignalNode;
    /*The freed node pointer*/
    P_St_SignalNode pFreeNode;
}St_Signal, *P_St_Signal;


/*==============================================================
    LOCAL VARIABLES
===============================================================*/
/*Creat the empty signal queue*/
static St_Signal stSignal = {0};


/**********************************************************************
 * Socket functions data structure
 **********************************************************************/
/*max count of the socket*/
#define MAX_COUNT_OF_SOCKET    15
#define MAX_MESSAGE_SIZE      150

typedef struct tagSt_SockAddrIn{
    WE_UINT16  uiSockInFamily;
    WE_UINT16  uiSockInPort;
    WE_UINT32  uiSockInAddr;
    WE_UINT8   uiSockInZero[8];
}St_SockAddrIn;

typedef struct tagSt_Socket
{
    /*Module ID*/
    WE_UINT8  uiModuleId;
    /*Socket ID*/
    WE_UINT8  uiSocketId;
    /*Network account ID*/
    WE_UINT8  uiNetAccountId;
    /*ISOCKET pointer*/
    ISocket *pSocket;
    /*Socket data pointer*/
    void    *pvUserData;
    /*Length of the socket data*/
    WE_UINT32  uiDataLength;
    /*Jydge the socket was used or not 0:Not used 1:Used*/
    WE_BOOL    bStatus;
}St_Socket, *P_St_Socket;


St_Socket aSocketArray[MAX_COUNT_OF_SOCKET];

static INetMgr *pINetMgr = NULL;
IShell *pIShell = NULL;

/**********************************************************************
 * TPI signal functions
 **********************************************************************/

/*==================================================================================================
    FUNCTION: TPIa_SignalRegisterQueue

    DESCRIPTION:
        When this function is called the integration must create an empty 
        signal queue labelled with modId. After this function is called 
        the integration must be able to add signals last in the queue, 
        and read out signals from the beginning of the queue.

    ARGUMENTS PASSED:
        modId: The Mobile Suite id of the module, label of the signal queue.

    RETURN VALUE:
        None

    IMPORTANT NOTES:
        None
==================================================================================================*/

void TPIa_SignalRegisterQueue (WE_UINT8 uiModuleId)
{
    int i;
    P_St_SignalNode tempnode = NULL;

    if(WE_IS_VALID_MODULE_ID(uiModuleId) && 
        0 == stSignal.header[uiModuleId].bReg)
    {
        /*Init module's header*/
        memset(&(stSignal.header[uiModuleId]), 0, sizeof(St_SignalHeader));

        /*Alloc and Init memory if no memory*/
        if(NULL == stSignal.pSignalNode)
        {
            /*Reserve memory for nodes*/
            stSignal.pSignalNode = (P_St_SignalNode)MALLOC(sizeof(St_SignalNode) * MAX_COUNT_OF_SIGNAL);
            if(NULL != stSignal.pSignalNode)
            {
                /*Init free node*/
                memset(stSignal.pSignalNode, 0 , sizeof(St_SignalNode) * MAX_COUNT_OF_SIGNAL);
                stSignal.pFreeNode = stSignal.pSignalNode;
                tempnode = stSignal.pFreeNode;
                for(i = 0; i < MAX_COUNT_OF_SIGNAL - 1; i ++)
                {
                    tempnode->pstNext = tempnode + 1;
                    tempnode ++;
                }
            }
        }

        /*Set register status*/
        if(NULL != stSignal.pSignalNode)
        {
            stSignal.header[uiModuleId].bReg = 1;
        }
    }
}

/*==================================================================================================
    FUNCTION: TPIa_SignalDeregisterQueue

    DESCRIPTION:
        Remove a signal queue labelled modId. If the signal queue have 
        signals that are not retrieved yet, all that signals are deleted.

    ARGUMENTS PASSED:
        modId: The Mobile Suite id of the module, label of the signal queue.

    RETURN VALUE:
        None

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important 
        aspect of using the function i.e side effect..etc
==================================================================================================*/

void TPIa_SignalDeregisterQueue (WE_UINT8 uiModuleId)
{
    P_St_SignalNode pstTempnode = NULL;
    P_St_SignalNode pstTempnode1 = NULL;
    int i;
    
    if(WE_IS_VALID_MODULE_ID(uiModuleId) && 
        1 == stSignal.header[uiModuleId].bReg)
    {
        if(stSignal.header[uiModuleId].uiNum != 0)
        {
            /*Delete all signal node from this module*/
            pstTempnode = stSignal.header[uiModuleId].pSignalHead;
            while(pstTempnode != NULL && 
                stSignal.header[uiModuleId].uiNum != 0)
            {
                pstTempnode1 = pstTempnode->pstNext;
                if(pstTempnode->pvSigalData != NULL)
                {
                    FREE(pstTempnode->pvSigalData);
                    pstTempnode->pvSigalData = NULL;
                }
                pstTempnode->pstNext = stSignal.pFreeNode;
                stSignal.pFreeNode = pstTempnode;
                
                pstTempnode = pstTempnode1;
                stSignal.header[uiModuleId].uiNum --;
            }
        }
        /*Set the module's header with NULL*/
        memset(&(stSignal.header[uiModuleId]), 0, sizeof(St_SignalHeader));
        /*Detect if all modules are unregistered, if so, free memory of 
        signal nodes*/
        for(i = 0; i < WE_NUMBER_OF_MODULES; i ++)
        {
            if(stSignal.header[i].bReg)
            {
                return;
            }
        }
        /*Free memory of signal nodes*/
        if(stSignal.pSignalNode != NULL)
        {
            FREE(stSignal.pSignalNode);
        }
        /*Set all information with NULL*/
        memset(&stSignal, 0, sizeof(stSignal));
    }
}

/*==================================================================================================
    FUNCTION: TPIa_SignalAllocMemory

    DESCRIPTION:
        Allocate signal memory that has to be sent in a subsequent
        TPIa_SignalSend. If no signal memory is available the 
        integration must return MSF_NULL. It is a very critical
        situation when the signal memory runs out, so if this function 
        returns MSF_NULL, the whole Mobile Suite is terminating.
        
    ARGUMENTS PASSED:
        signalLength: The length of the requested signal memory.
        
    RETURN VALUE:
        RETURNS A pointer to the signal memory, if failure return MSF_NULL.

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important 
        aspect of using the function i.e side effect..etc
==================================================================================================*/

void* TPIa_SignalAllocMemory (WE_UINT16 uiSignalDataLength)
{
    return (void*)MALLOC(uiSignalDataLength);
}

/*==================================================================================================
    FUNCTION: TPIa_SignalFreeMemory

    DESCRIPTION:
        Free the signal memory.
        
    ARGUMENTS PASSED:
        signalData: A pointer to the signal memory.
         
    RETURN VALUE:
        None

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important 
        aspect of using the function i.e side effect..etc
==================================================================================================*/

void TPIa_SignalFreeMemory (void* pvSignalData)
{
    if(NULL != pvSignalData)
    {
        FREE(pvSignalData);
    }
}

/*==================================================================================================
    FUNCTION: TPIa_SignalSend

    DESCRIPTION:
        When this function is called the integration must put the signal last 
        in the signal queue labelled with destModId. If the signal queue 
        labelled with destModId doesn't exist or it isn't register yet or 
        the signal queue is full, the signal must be deleted and this
        function must return an error code. The signal data is allocated 
        before by the function HDIa_signalAllocMemory.       

    ARGUMENTS PASSED:
        destModId        : The Mobile Suite id of the destination signal queue.
        signalData       : A pointer to the signal data.
        signalDataLength : The length of the signal data.
        
    RETURN VALUE:
        HDI_SIGNAL_OK, or a negative value if an error occurred - error codes:
        HDI_SIGNAL_ERROR_QUEUE_FULL: The signal queue is full.
        HDI_SIGNAL_ERROR_QUEUE_UNKNOWN: The signal queue doesn’t exist or is
                                        not registering yet.

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important 
        aspect of using the function i.e side effect..etc
==================================================================================================*/

int TPIa_SignalSend (WE_UINT8 uiModuleId, void* pvSignalData, WE_UINT16 uiSignalDataLength)
{
    int result = TPI_SIGNAL_ERROR_QUEUE_UNKNOWN;
    P_St_SignalNode tempnode = NULL;
    
    if(WE_IS_VALID_MODULE_ID(uiModuleId) &&
        stSignal.pSignalNode != NULL)
    {
        if(NULL == stSignal.pFreeNode)
        {
            result = TPI_SIGNAL_ERROR_QUEUE_FULL;
        }
        else
        {
            tempnode = stSignal.pFreeNode;
            stSignal.pFreeNode = stSignal.pFreeNode->pstNext;
            tempnode->uiDataLength = uiSignalDataLength;
            tempnode->pvSigalData = pvSignalData;
            tempnode->bStatus = 1;
            tempnode->pstNext = NULL;
            
            if(NULL == stSignal.header[uiModuleId].pSignalLast)
            {
                /*This is first node, set last and header with same node*/
                stSignal.header[uiModuleId].pSignalLast = tempnode;
                stSignal.header[uiModuleId].pSignalHead = tempnode;
            }
            else
            {
                /*Add the node to last and set last as this node*/
                stSignal.header[uiModuleId].pSignalLast->pstNext = tempnode;
                stSignal.header[uiModuleId].pSignalLast = tempnode;
            }
            stSignal.header[uiModuleId].uiNum ++;
            
            result = TPI_SIGNAL_OK;
        }
    }

    return result;
}

/*==================================================================================================
    FUNCTION: TPIa_SignalRetrieve

    DESCRIPTION:
        Retrieve the first signal in the signal queue labelled destModId. 
        The signal data memory is released later by a call to the function 
        HDIa_signalFreeMemory. If the signal queue labelled with destModId
        doesn't exist, it isn't register yet or the signal queue is empty,
        return MSF_NULL. This function can only be called from a module's run
        function.
        
    ARGUMENTS PASSED:
        destModId: The Mobile Suite id of the module, label of the signal 
        queue.

    RETURN VALUE:
        A pointer to the signal data, if no signal to retrieve return MSF_NULL.

    IMPORTANT NOTES:

==================================================================================================*/

void* TPIa_SignalRetrieve (WE_UINT8 uiDestModuleId)
{
    void *pvResult = NULL;
    P_St_SignalNode pstTempnode = NULL;

    if(TPIa_SignalQueueLength(uiDestModuleId) > 0 &&
        stSignal.pSignalNode != NULL)
    {
        if(stSignal.header[uiDestModuleId].pSignalHead != NULL)
        {
            pvResult = stSignal.header[uiDestModuleId].pSignalHead->pvSigalData;
            stSignal.header[uiDestModuleId].pSignalHead->bStatus = 0;
            pstTempnode = stSignal.pFreeNode;
            stSignal.pFreeNode = stSignal.header[uiDestModuleId].pSignalHead;
            stSignal.header[uiDestModuleId].pSignalHead = stSignal.header[uiDestModuleId].pSignalHead->pstNext;
            stSignal.pFreeNode->pstNext = pstTempnode;
            stSignal.header[uiDestModuleId].uiNum --;
            if(0 == stSignal.header[uiDestModuleId].uiNum)
            {
                stSignal.header[uiDestModuleId].pSignalLast = NULL;
            }
        }
    }

    return pvResult;
}

/*==================================================================================================
    FUNCTION: TPIa_SignalQueueLength

    DESCRIPTION:
        Returns the number of signals in the signal queue labelled modId. If 
        the signal queue labelled with destModId doesn’t exist or it isn't
        register yet, return -1. This function
        can only be called from a module’s wantsToRun function.

    ARGUMENTS PASSED:
        ModId: The Mobile Suite id of the module, label of the signal queue.
         
    RETURN VALUE:
        The number of signals in the queue.

    IMPORTANT NOTES:
        None
==================================================================================================*/

int TPIa_SignalQueueLength (WE_UINT8 uiModuleId)
{
    int iResult = -1;
    
    if(WE_IS_VALID_MODULE_ID(uiModuleId) && 
        (1 == stSignal.header[uiModuleId].bReg))
    {
        iResult = stSignal.header[uiModuleId].uiNum;
    }

    return iResult;
}

/**********************************************************************
 * TPI Pipe Funktions
 **********************************************************************/

int TPIa_PipeCreate (WE_UINT8 uiModuleId, const char* pcName)
{
   return PipeCreate(uiModuleId,pcName);
}

int TPIa_PipeOpen (WE_UINT8 uiModuleId, const char *pcName)
{
   return PipeOpen(uiModuleId,pcName);
}

int TPIa_PipeClose (int iHandle)
{
   return PipeClose(iHandle); 
}

int TPIa_PipeDelete (const char* pcName)
{
   return PipeDelete(pcName) ;
}

long TPIa_PipeRead (int iHandle, void* pvBuf, long lBufSize)
{
   return PipeRead(iHandle,pvBuf,lBufSize);
}

long TPIa_PipeWrite (int iHandle, void* pvBuf, long lBufSize)
{
   return PipeWrite(iHandle,pvBuf,lBufSize);
}

int  TPIa_PipePoll (int iHandle)
{
   return PipePoll(iHandle);
}

int TPIa_PipeStatus (int iHandle, int *piIsOpen, long *plAvailable)
{
   return PipeStatus(iHandle,piIsOpen,plAvailable);
}

void
TPIa_PipeCloseAll (WE_UINT8 uiModuleId)
{
    PipeCloseAll(uiModuleId);
}


/**********************************************************************
 * TPI File Funktions 
 **********************************************************************/
/*******************************************************************************
*    Macro Define Section
*******************************************************************************/
#define MAX_OPEN_FILE_COUNT 20
#define _FILE_DEBUG
/*******************************************************************************
*    Struct Define Section
*******************************************************************************/
typedef struct tagSt_FileHandle
{
    IFile *pIfile;    /*BREW平台中指向文件的指针*/
    WE_UINT8 uiModuleId; /*WE系统中的模块标识*/
    WE_UINT8 uiMode; /*标识文件的操作模式，如只读，只写或读写*/
}St_FileHandle;
/*******************************************************************************
*    File Static Variable Define Section
*******************************************************************************/
static St_FileHandle     s_astFileHandle[MAX_OPEN_FILE_COUNT];
static IFileMgr * s_pIFileMgr = NULL;
static IShell * s_pIShell = NULL;
/*====================================================================================
FUNCTION: File_GetFileMgr

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
    This function is used to get the IFile Manager object, which can use the IFileMgr API by it. 
    If the IFileMgr object is not exist, then creat it again.

ARGUMENTS PASSED:
    none
    
RETURN VALUE:
         IFileMgr*----The IFileMgr object.

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: s_pIFileMgr

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
static IFileMgr* File_GetFileMgr(void)
{
    /*Create the IFileMgr object again if s_pIFileMgr is invalid*/
    if(NULL == s_pIFileMgr)
    {
        ISHELL_CreateInstance(s_pIShell, AEECLSID_FILEMGR,(void **)&s_pIFileMgr);/*SUCCESS is 0*/
    }
    return s_pIFileMgr;
}
/*====================================================================================
FUNCTION: Tpi_ReleaseFileMgr

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This function is used to release the IFileMgr object when IFileMgr instance is not used again.

ARGUMENTS PASSED:
    none
    
RETURN VALUE:
        none

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: s_pIFileMgr

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
static void File_ReleaseFileMgr(void)
{
    WE_UINT32 uiReferCount = 0;
    /*Create the IFileMgr object again if s_pIFileMgr is invalid*/
    uiReferCount = IFILEMGR_Release(s_pIFileMgr);
#ifdef _FILE_DEBUG
    DBGPRINTF("[File]The FileMgr reference count is %d!\n", uiReferCount);
#endif
    if(uiReferCount == 0)
    {
        s_pIFileMgr = NULL;
    }
}
/*====================================================================================
FUNCTION: File_PathTpiToBrew

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
       This function is used to convert the TPI file path to  BREW file path.

ARGUMENTS PASSED:
        *pcFileName --- TPI file path name
    
RETURN VALUE:
        NULL---- if the source path name is invalid.
        pcName--- the converted BREW file path name.

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
static char * File_PathTpiToBrew
(
    const char *pcFileName
)
{
    WE_UINT8 i, uiStrLen;
    char acTemp[10];
    char *pcName = NULL;


    if ( NULL == pcFileName )
    {
        /*Print output information*/
#ifdef _FILE_DEBUG
        DBGPRINTF("[File]The directory name is invalid!\n");
#endif
        return NULL;
    }

    /*
    *If pathname start with "/external/", then remove the "/external"
    */
    uiStrLen = STRLEN(pcFileName);

    if(uiStrLen>10)
    {
        /*Convert to lower case*/
        STRNCPY((char *)&acTemp, pcFileName, 10);
        for(i=0; i<10; i++)
        {
            if((acTemp[i]>=0x41)&&(acTemp[i]<=0x5A))
            {
                acTemp[i] += 0x20;
            }        
        }
            /*Compare*/
        if(STRNCMP(acTemp, (char *)"/external/", 10) == 0)
        {
            pcName = (char *)(pcFileName + 9);
        }
        else
        {
            pcName = (char *)pcFileName;
        }        
    }
    else
    {
        pcName = (char *)pcFileName;
    }

    return pcName;
}      
/*====================================================================================
FUNCTION: Flie_IFileToFileHandle

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This function is used to convert the IFile pointer of BREW to the filehandle of TPI.

ARGUMENTS PASSED:
        *pIfile--- the pointer to IFile in BREW
        uiFileMode--- the mode of opening file
        uiModeId---the ID of the module which the file locates.
    
RETURN VALUE:
        iFileHandle---the filehandle of the file in TPI.

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: s_astFileHandle

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
static int Flie_IFileToFileHandle
(
    IFile *pIfile,
    WE_UINT8 uiOpenMode,
    WE_UINT8 uiModId
)
{
    WE_UINT8 uiHandle = 0;
    WE_BOOL bFindItem = FALSE;

    for(uiHandle = 0;uiHandle < MAX_OPEN_FILE_COUNT;uiHandle ++)
    {
        if(NULL != s_astFileHandle[uiHandle].pIfile)
        {
            bFindItem = TRUE;
            break;
        }
    }

    if(bFindItem)
    {
        s_astFileHandle[uiHandle].pIfile = pIfile;
        s_astFileHandle[uiHandle].uiModuleId = uiModId;
        s_astFileHandle[uiHandle].uiMode = uiOpenMode;
    }

    return (uiHandle + 1);
}
/*====================================================================================
FUNCTION: Flie_ReleaseFileHandle

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This function is used to release the file handle when the file is closed.

ARGUMENTS PASSED:
        iHandle--- the handle of the opening file
    
RETURN VALUE:
        None

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: s_astFileHandle

CALL BY:TPIa_fileClose()

IMPORTANT NOTES:
    None
=====================================================================================*/
static void Flie_ReleaseFileHandle(int iHandle)
{
    if(iHandle > 0 && iHandle <= MAX_OPEN_FILE_COUNT )
    {
        memset(s_astFileHandle + iHandle - 1, 0x00, sizeof(St_FileHandle));
    }
    else
    {
        /*print the error information*/
#ifdef _FILE_DEBUG
        DBGPRINTF("[File]The file handle is invalid!\n");
#endif
    }
}
/*====================================================================================
FUNCTION: File_FileHandleToIFlie

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This function is used to convert the filehandle of TPI to the IFile pointer of BREW.

ARGUMENTS PASSED:
        iFileHandle--- the handle of the opening file
    
RETURN VALUE:
        NULL---if the input handle is invalid
        IFile*---the converted IFile pointer

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: s_astFileHandle

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
static IFile* File_FileHandleToIFlie
(
    int iFileHandle
)
{
    WE_UINT8 i;
    IFile * pIfile = NULL;

    if(iFileHandle > 0 && iFileHandle <= MAX_OPEN_FILE_COUNT)
    {
        pIfile = s_astFileHandle[iFileHandle - 1].pIfile;
    }
    else
    {
        /*print the error information*/
#ifdef _FILE_DEBUG
        DBGPRINTF("[File]The file handle is invalid!\n");
#endif
    }
    return pIfile;
}
/*====================================================================================
FUNCTION: File_ConvertSeekModeTpiToBrew

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This function is used to convert the seek mode of TPI to the seek type of BREW.

ARGUMENTS PASSED:
        iSeekMode--- the seek mode in TPI
    
RETURN VALUE:
        iSeekType---the seek type in BREW

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:TPIa_fileSeek()

IMPORTANT NOTES:
    None
=====================================================================================*/
static FileSeekType File_ConvertSeekModeTpiToBrew(WE_INT8 iSeekMode)
{
    FileSeekType iSeekType = _SEEK_START;

    switch(iSeekMode)
    {
        case TPI_FILE_SEEK_SET:
            iSeekType = _SEEK_START;
            break;
        case TPI_FILE_SEEK_CUR:
            iSeekType = _SEEK_CURRENT;
            break;
        case TPI_FILE_SEEK_END:
            iSeekType = _SEEK_END;
            break;
        default:
            break;
    }
    return iSeekType;
}
/*====================================================================================
FUNCTION: File_ConvertOpenModeTpiToBrew

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This function is used to convert the mode of opening file from TPI to BREW.

ARGUMENTS PASSED:
        iMode--- the mode for opening file in TPI
    
RETURN VALUE:
        iOpenMode---the mode for opening file in BREW

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:TPIa_fileOpen()

IMPORTANT NOTES:
    None
=====================================================================================*/
static OpenFileMode File_ConvertOpenModeTpiToBrew(int iMode)
{
    OpenFileMode iOpenMode = _OFM_READ;
    switch(iMode)
    {
        case TPI_FILE_SET_RDONLY:
            iOpenMode = _OFM_READ;
            break;
        case TPI_FILE_SET_CREATE:
            iOpenMode = _OFM_CREATE;
            break;
        case TPI_FILE_SET_RDWR:
            iOpenMode = _OFM_READWRITE;
            break;        
        case TPI_FILE_SET_APPEND:

            iOpenMode = _OFM_APPEND;
            break;
        default:
            break;
    }
    return iOpenMode;
}
/*====================================================================================
FUNCTION: File_ConvertResultBrewToTpi

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This function is used to convert the result of operating file from BREW to TPI.

ARGUMENTS PASSED:
        iResult--- the result of operating file in BREW.
    
RETURN VALUE:
        iReturn---the converted result for TPI

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:TPIa_fileOpen()

IMPORTANT NOTES:
    None
=====================================================================================*/
static int File_ConvertResultBrewToTpi(int iResult)
{
    int iReturn = TPI_FILE_ERROR_ACCESS;
    if(iResult < 0)
    {
        switch(iResult)
        {
            case EFILEEXISTS:// File exist
                iReturn = TPI_FILE_ERROR_EXIST;
                break;
            case EDIRNOEXISTS:// Directory does not exist
            case EBADFILENAME:// Bad file name
            case EFILENOEXISTS:// File does not exist
                iReturn = TPI_FILE_ERROR_PATH;
                break;
            case EINVALIDOPERATION:
            case EBADSEEKPOS: // Bad seek position
            case EBADPARM:
                iReturn = TPI_FILE_ERROR_INVALID;
                break;
            case EFILEOPEN:// File already open
            case EDIRNOTEMPTY:// Directory not empty
                iReturn = TPI_FILE_ERROR_ACCESS;
                break;
            case EFILEEOF:// End of file
                iReturn = TPI_FILE_ERROR_EOF;
                break;
            case EFSFULL: // File system full
                iReturn = TPI_FILE_ERROR_FULL;
                break;
            case EOUTOFNODES:
                iReturn = TPI_FILE_ERROR_EXIST;
                break;
            default:
                break;
        }
    }
    else
    {
        iReturn = iResult;
    }
    return iReturn;
}

/*====================================================================================
FUNCTION: TPIa_fileOpen

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for opening/creating a file with the input name  in WE system.        

ARGUMENTS PASSED:
        uiModId--- the module id of the file locating.
        *pcFileName---the name of file to open
        iMode----the mode of opening file.
                  it will creat the new file if the mode is TPI_FILE_SET_CREATE, else it will open the file.
        lSize----the size of the file to create,it is only used when creating a new file.
    
RETURN VALUE:
        iResult---the filehandle if success, or it is the error code.

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
int TPIa_fileOpen
(
    WE_UINT8 uiModId,
    const char *pcFileName,
    int iMode,
    long lSize
)
{
    IFile * pIFile = NULL;
    int iResult = TPI_FILE_ERROR_INVALID;
    WE_BOOL bAbleOpen = FALSE;
    OpenFileMode OpenMode;
    int iFileHandle;
    char *pcTpiName = NULL;
    IFileMgr *pFileMgr = NULL;

    if(NULL == pcFileName)
    {
        return iResult;
    }

    /*Get the Instance for IFileMgr*/
    pFileMgr = File_GetFileMgr();
    /*Convert the file name from WE to BREW*/
    pcTpiName = File_PathTpiToBrew(pcFileName);
    /*Convert the mode from TPI to BREW*/
    OpenMode = File_ConvertOpenModeTpiToBrew(iMode);            
    if(TPI_FILE_SET_CREATE == iMode)
    {
        /*Create the new file with fileName*/
        pIFile = IFILEMGR_OpenFile(pFileMgr, pcTpiName, _OFM_CREATE);
        if(NULL == pIFile)/*Create the new file is failed*/
        {
            /*Get the reason of creating the file failed*/
            iResult = IFILEMGR_GetLastError(pFileMgr);
            /*convert the result from BREW to TPI*/
            iResult = File_ConvertResultBrewToTpi(iResult);
            switch(iResult)
            {
                case TPI_FILE_OK:
                    bAbleOpen = TRUE;/*This case is false*/
                    break;
                case TPI_FILE_ERROR_ACCESS:
                case TPI_FILE_ERROR_DELAYED:
                case TPI_FILE_ERROR_PATH:
                case TPI_FILE_ERROR_INVALID:
                case TPI_FILE_ERROR_SIZE:
                case TPI_FILE_ERROR_FULL:
                case TPI_FILE_ERROR_EOF:
                    break;
                case TPI_FILE_ERROR_EXIST:
                    if(TPI_FILE_SET_EXCL == iMode)
                    {
                        bAbleOpen = FALSE;
                    }
                    else
                    {
                        bAbleOpen = TRUE;/*This case is false*/
                    }
                    break;
                default:
                    break;
            }
        }
        else/*Create the new file successfully*/
        {
            bAbleOpen = TRUE;
        }
    }
    else
    {
        bAbleOpen = TRUE;
    }
    if(TRUE == bAbleOpen)
    {
        /*Open the aim file with fileName*/
        pIFile = IFILEMGR_OpenFile(pFileMgr, pcTpiName, OpenMode);
        
        if(NULL == pIFile)/*Open the file is failed*/
        {
            /*Get the reason of opening file failed*/
            iResult = IFILEMGR_GetLastError(pFileMgr);
            /*convert the result from BREW to TPI*/
            iResult = File_ConvertResultBrewToTpi(iResult);
            /*Print the reason of opening failed*/
#ifdef _FILE_DEBUG
            DBGPRINTF("[TPIa_fileOpen]Open the file is failed!\n");
#endif
        }
        else/*Convert the Ifile pointer to Filehandle*/
        {
            iResult = Flie_IFileToFileHandle(pIFile, (WE_UINT8)OpenMode,uiModId);    
        }
    }
    return iResult;
}
/*====================================================================================
FUNCTION: TPIa_fileClose

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for closing a file in WE system, and it should deallocate the file handle.

ARGUMENTS PASSED:
        iFileHandle--- File handle identifying the file to close.
    
RETURN VALUE:
        iResult---TPI_FILE_OK if success, or a negative valuse will be returned:TPI_FILE_ERROR_INVALID

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
int TPIa_fileClose (int iFileHandle)
{
    IFile *pIfile = NULL;
    int iResult = TPI_FILE_ERROR_INVALID;

    /*Convert TPI Filehandle to BREW IFile pointer*/
    pIfile = File_FileHandleToIFlie(iFileHandle);

    /*Close the file*/
    if(pIfile != NULL)
    {
        iResult = IFILE_Release(pIfile);
        if(iResult == SUCCESS)/*Close the file successfully*/
        {
            /*Release the filehandle*/
            Flie_ReleaseFileHandle(iFileHandle);
        }
        else
        {
#ifdef _FILE_DEBUG
            DBGPRINTF("[TPIa_fileClose]Close the file is failed,and the error code is %d!\n", iResult);
#endif
        }
    }
    /*Convert the result from BREW to WE*/
    iResult = File_ConvertResultBrewToTpi(iResult);
    return iResult;
}
/*====================================================================================
FUNCTION: TPIa_fileWrite

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for attempting to write data to the file,which associates
        with the opened file from the buffer.

ARGUMENTS PASSED:
        iFileHandle---File handle of the opened file.
        *data---pointer to a buffer containing the data to write to the file
        lSize----the number of bytes to write.
    
RETURN VALUE:
        uiResult---the number of bytes written if success.
                  or a negative value will be returned:
            TPI_FILE_ERROR_DELAYED The write access to the file system was delayed 
            TPI_FILE_ERROR_ACCESS E.g. attempt to write in a file opened as read only.
            TPI_FILE_ERROR_INVALID E.g. fileHandle argument is not a valid file handle open for writing.
            TPI_ FILE_ERROR_SIZE
            TPI_ FILE_ERROR_FULL
USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
long TPIa_fileWrite
(
    int iFileHandle,
    void *data,
    long lSize
)
{
    IFile *pIfile = NULL;
    WE_UINT32 uiResult = TPI_FILE_ERROR_INVALID;
    IFileMgr *pFileMgr = NULL;

    /*Get the Instance for IFileMgr*/
    pFileMgr = File_GetFileMgr();
    
    if(NULL == data ||lSize < 0 ||NULL == pFileMgr)
    {
        return uiResult;
    }
    if(lSize == 0)
    {
        return 0;
    }
    /*Convert TPI Filehandle to BREW IFile pointer*/
    pIfile = File_FileHandleToIFlie(iFileHandle);

    /*Write data to the opened file with iFileHandle*/
    uiResult = IFILE_Write(pIfile, data, lSize);
    if(0 == uiResult)/*Write data to file failed*/
    {
        /*Get the failed reason*/
        uiResult = IFILEMGR_GetLastError(pFileMgr);
        /*Convert the result from BREW to WE*/
        uiResult = File_ConvertResultBrewToTpi(uiResult);
        /*Print the error information*/
#ifdef _FILE_DEBUG
        DBGPRINTF("[TPIa_fileWrite]Write data to the file is failed,and the error code is %d!\n", uiResult);
#endif
    }
    return uiResult;
}
/*====================================================================================
FUNCTION: TPIa_fileRead

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for attempting to read data from  the file,which associates
        with the opened file, into the buffer.

ARGUMENTS PASSED:
        iFileHandle---File handle of the file.
        data---pointer to a buffer where the data will be copied.
        lSize----the size in bytes of the data to read from the file.if it is 0,then the function should return 0 and have 
                no other results.
    
RETURN VALUE:
        uiResult---the number of bytes have copied if success.
                  or a negative value will be returned:
            TPI_FILE_ERROR_EOF  
            TPI_FILE_ERROR_ACCESS E.g. attempt to read in a file opened as write only.
            TPI_FILE_ERROR_INVALID E.g. fileHandle argument is not a valid file handle open for reading.
            TPI_ FILE_ERROR_DELAYED E.g. The read access to the file system is delayed.

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
long TPIa_fileRead
(
    int iFileHandle,
    void *data,
    long lSize
)
{
    IFile *pIfile = NULL;
    IFileMgr *pFileMgr = NULL;
    WE_UINT32 uiResult = TPI_FILE_ERROR_INVALID;

    /*Convert TPI Filehandle to BREW IFile pointer*/
    pIfile = File_FileHandleToIFlie(iFileHandle);

    /*Get the Instance for IFileMgr*/
    pFileMgr = File_GetFileMgr();
    
    if(NULL == data ||lSize < 0 ||NULL == pFileMgr)
    {
        return uiResult;
    }
    if(lSize == 0)
    {
        return 0;
    }
    /*Read to data from the opened file with iFileHandle*/
    uiResult = IFILE_Read(pIfile, data, lSize);
    if(0 == uiResult)/*Read data from file failed*/
    {
        /*Get the failed reason*/
        uiResult = IFILEMGR_GetLastError(pFileMgr);
        /*Convert the result from BREW to WE*/
        uiResult = File_ConvertResultBrewToTpi(uiResult);
        /*Print the error information*/
#ifdef _FILE_DEBUG
        DBGPRINTF("[TPIa_fileRead]Write data to the file is failed,and the error code is %d!\n", uiResult);
#endif
    }
    /*Convert the result from BREW to WE*/
    uiResult = File_ConvertResultBrewToTpi(uiResult);
    return uiResult;
}
/*====================================================================================
FUNCTION: TPIa_fileFlush

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for opening a file in WE system.

ARGUMENTS PASSED:
        iFileHandle--- File handle identifying  the file.
    
RETURN VALUE:
        None

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
void TPIa_fileFlush (int iFileHandle)
{
    IFile *pIfile = NULL;
    WE_UINT32 uiResult = TPI_FILE_ERROR_INVALID;

    /*Convert TPI Filehandle to BREW IFile pointer*/
    pIfile = File_FileHandleToIFlie(iFileHandle);
    if(NULL == pIfile)
    {
    }
}
/*====================================================================================
FUNCTION: TPIa_fileSeek

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for moving the file pointer of an open file.

ARGUMENTS PASSED:
        iFileHandle--- File handle identifying  the file.
        lOffset---Distance in bytes to move
        iSeekMode----the mode of seeking,inculding:
                    TPI_FILE_SEEK_SET: offset from start of file
                    TPI_FILE_SEEK_CUR: relative offset from current position
                    TPI_FILE_SEEK_END: offset from end of file
    
RETURN VALUE:
        uiResult---The new position of the file pointer, or a negative value will be returned,inculding
                TPI_FILE_ERROR_ACCESS The file was opened in TPI_FILE_SET_APPEND mode. 
                TPI_FILE_ERROR_INVALID E.g. the fileHandle argument is not an open file descriptor or the requested new cursor position exceeds the limits of the file.

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
long TPIa_fileSeek
(
    int iFileHandle,
    long lOffset,
    int iSeekMode
)
{
    IFile *pIfile = NULL;
    WE_UINT8 uiFileMode;
    WE_UINT32 uiResult = TPI_FILE_ERROR_INVALID;
    FileSeekType SeekType;

        /*If the mode is Append,fileseek is forbidden and return COM_FILE_ERROR_ACCESS*/
        uiFileMode = (WE_UINT8)(iFileHandle & 0xff);
    if(TPI_FILE_SET_APPEND == uiFileMode)
    {
        return TPI_FILE_ERROR_ACCESS;
    }
    
    /*Convert TPI Filehandle to BREW IFile pointer*/
    pIfile = File_FileHandleToIFlie(iFileHandle);
    if(NULL == pIfile)
    {
        return uiResult;
    }
    /*Convert SeekMode of TPI to SeekType of BREW*/
    SeekType = File_ConvertSeekModeTpiToBrew(iSeekMode);
    uiResult = IFILE_Seek(pIfile, SeekType, lOffset);
    if(SUCCESS == uiResult)/*Seek successfully*/
    {
        /*It will return the current pointer position if the mode is _SEEK_CURRENT and offset is 0*/
        uiResult = IFILE_Seek(pIfile, _SEEK_CURRENT, 0);
    }
    else if(EFAILED == uiResult)/*Seek failed*/
    {
        /*Convert the result from BREW to WE*/
        uiResult = File_ConvertResultBrewToTpi(uiResult);
        /*Print the error information*/
#ifdef _FILE_DEBUG
        DBGPRINTF("[TPIa_fileSeek]Write data to the file is failed,and the error code is %d!\n", uiResult);
#endif
    }

    return uiResult;
}
/*====================================================================================
FUNCTION: TPIa_fileRemove

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for removing the file identified by fileName.If the file currently is open,
        the removal is delayed until all users of the file have closed it. However, it is no longer possible to open the file after a call
        to TPIa_fileRemove().

ARGUMENTS PASSED:
        pcFileName---Path name of the file to remove.
    
RETURN VALUE:
        iResult---TPI_FILE_OK if remove successfully, or a negative value will be returned:
        TPI_FILE_ERROR_PATH e.g. pcFileName not found.

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
int TPIa_fileRemove (const char *pcFileName)
{
    char *pcPathName = NULL;
    int iResult = TPI_FILE_ERROR_INVALID;
    IFileMgr *pFileMgr = NULL;

    /*Get the Instance for IFileMgr*/
    pFileMgr = File_GetFileMgr();
    if(NULL == pFileMgr ||NULL == pcFileName)
    {
         return iResult;
    }
    /*Convert the file name from WE to BREW*/
    pcPathName = File_PathTpiToBrew(pcFileName);
    iResult = IFILEMGR_Remove(pFileMgr, pcPathName);
    /*Get the detailed error information if remove failed*/
    if(SUCCESS != iResult)
    {
        iResult = IFILEMGR_GetLastError(pFileMgr);
        /*Print the error information*/
#ifdef _FILE_DEBUG
        DBGPRINTF("[TPIa_fileRemove]Write data to the file is failed,and the error code is %d!\n", iResult);
#endif
    }

    /*Convert the result from BREW to WE*/
    iResult = File_ConvertResultBrewToTpi(iResult);

    return iResult;
}
/*====================================================================================
FUNCTION: TPIa_fileRename

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for renaming the file or directory specified by srcName to the name 
        specified by dstName.The old name must be the path of an existing file or directory.

ARGUMENTS PASSED:
        pcSrcName--- the old file or directory name.
        pcDstName---the new file or directory name.
    
RETURN VALUE:
        iResult---TPI_FILE_OK if rename successfully, or a negative value will be returned:
        TPI_FILE_ERROR_EXISTS: pcDstName has already existed.
        TPI_FILE_ERROR_PATH: source is a directory but target is not. A component of the source path does not exist, or a path 
        prefix of target does not exist.


USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
int TPIa_fileRename
(
    const char *pcSrcName,
    const char *pcDstName
)
{
    char *pcTpiSrcName = NULL;
    char *pcTpiDstName = NULL;
    int iResult = TPI_FILE_ERROR_INVALID;
    IFileMgr *pFileMgr = NULL;

    /*Get the Instance for IFileMgr*/
    pFileMgr = File_GetFileMgr();
    if(NULL == pFileMgr || NULL == pcSrcName ||NULL == pcDstName)
    {
#ifdef _FILE_DEBUG
         DBGPRINTF("[TPIa_fileRename]The file name is invalid!\n", iResult);
#endif
         return iResult;
    }
    /*Convert the file name from WE to BREW*/
    pcTpiSrcName = File_PathTpiToBrew(pcSrcName);
    pcTpiDstName = File_PathTpiToBrew(pcDstName);
    iResult = IFILEMGR_Rename(pFileMgr,pcTpiSrcName,pcTpiDstName);
    
    if(SUCCESS != iResult)/*Get the detailed error information if rename failed*/
    {
        iResult = IFILEMGR_GetLastError(pFileMgr);
        /*Print the error information*/
#ifdef _FILE_DEBUG
        DBGPRINTF("[TPIa_fileRename]Write data to the file is failed,and the error code is %d!\n", iResult);
#endif
    }
    
    /*Convert the result from BREW to WE*/
    iResult = File_ConvertResultBrewToTpi(iResult);
    
    return iResult;
}
/*====================================================================================
FUNCTION: TPIa_fileMkDir

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for creating a directory named by pcDirName.Only one path 
        at each time can be created by this function.

ARGUMENTS PASSED:
        dirName--- Path name of the directory to create.
    
RETURN VALUE:
        iResult---TPI_FILE_OK if make successfully, or a negative value will be returned:
        TPI_FILE_ERROR_FULL: 
        TPI_FILE_ERROR_PATH:
        TPI_FILE_ERROR_EXIST:the dirName has existed.

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
int TPIa_fileMkDir (const char *dirName)
{
    int iResult = TPI_FILE_ERROR_EXIST;
    char *pcTpiDirName = NULL;
    IFileMgr *pFileMgr = NULL;

    /*Get the Instance for IFileMgr*/
    pFileMgr = File_GetFileMgr();
    if(NULL == pFileMgr ||NULL == dirName)
    {
         return iResult;
    }
    /*Convert the file name from WE to BREW*/
    pcTpiDirName = File_PathTpiToBrew(dirName);
    iResult = IFILEMGR_MkDir(pFileMgr, pcTpiDirName);
    /*Convert the result from BREW to WE*/
    iResult = File_ConvertResultBrewToTpi(iResult);
    
    return iResult;
}
/*====================================================================================
FUNCTION: TPIa_fileRmDir

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for removing a directory named by pathName. The directory must be empty
        and the function should return an error if the directory contains files or subdirectories.

ARGUMENTS PASSED:
        dirName--- Path name of the directory to remove.
    
RETURN VALUE:
        iResult---TPI_FILE_OK if remove successfully, or a negative value will be returned:
        TPI_FILE_ERROR_ACCESS: 
        TPI_FILE_ERROR_PATH:

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
int TPIa_fileRmDir (const char *dirName)
{
    int iResult = TPI_FILE_ERROR_INVALID;
    char *pcTpiDirName = NULL;
    IFileMgr *pFileMgr = NULL;

    /*Get the Instance for IFileMgr*/
    pFileMgr = File_GetFileMgr();
    if(NULL == pFileMgr ||NULL == dirName)
    {
         return iResult;
    }
    /*Convert the file name from WE to BREW*/
    pcTpiDirName = File_PathTpiToBrew(dirName);
    iResult = IFILEMGR_RmDir(pFileMgr, pcTpiDirName);
    /*Get the detailed error information if remove failed*/
    if(SUCCESS != iResult)
    {
        iResult = IFILEMGR_GetLastError(pFileMgr);
        /*Print the error information*/
#ifdef _FILE_DEBUG
        DBGPRINTF("[TPIa_fileRmDir]Write data to the file is failed,and the error code is %d!\n", iResult);
#endif
    }

    /*Convert the result from BREW to WE*/
    iResult = File_ConvertResultBrewToTpi(iResult);
    
    return iResult;
}
/*====================================================================================
FUNCTION: TPIa_fileGetSizeDir

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for getting the size, i.e the number of entries, of a directory. Entry is either
        a file or another directory.

ARGUMENTS PASSED:
        dirName--- path name of the directory.
    
RETURN VALUE:
        iResult---Number of entries if success, or a negative value will be returned:
        TPI_FILE_ERROR_PATH:dirName not found.
        
USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
int TPIa_fileGetSizeDir (const char *dirName)
{
    int iResult = TPI_FILE_ERROR_PATH;
    char *pcTpiDirName = NULL;
    IFileMgr *pFileMgr = NULL;
    FileInfo * pInfo = NULL;
    WE_UINT32 uiCount = 0;

    /*Get the Instance for IFileMgr*/
    pFileMgr = File_GetFileMgr();
    if(NULL == pFileMgr ||NULL == dirName)
    {
         return iResult;
    }
    /*Convert the file name from WE to BREW*/
    pcTpiDirName = File_PathTpiToBrew(dirName);

    /*Make the aim directory as the current root dir*/
    iResult = IFILEMGR_EnumInit(pFileMgr, pcTpiDirName, TRUE);/*TRUE means only enum directory, and FALSE is only for file*/
    if(SUCCESS == iResult)
    {
        /*Get the information of directory in the current dir*/
        while(IFILEMGR_EnumNext(pFileMgr, pInfo))
        {
            uiCount ++;
        }
    }
    else
    {
        iResult = TPI_FILE_ERROR_PATH;
        return iResult;
    }
    
    /*Make the aim directory as the current root dir*/
    if(SUCCESS == iResult)
    {
        iResult = IFILEMGR_EnumInit(pFileMgr, pcTpiDirName, FALSE);/*TRUE means only enum directory, and FALSE is only for file*/
    }
    if(SUCCESS == iResult)
    {
        /*Get the information of files in the current dir*/
        while(IFILEMGR_EnumNext(pFileMgr, pInfo))
        {
            uiCount ++;
        }
        iResult = uiCount;
    }
    else
    {
        iResult = TPI_FILE_ERROR_PATH;
    }
    
    return iResult;
}
/*====================================================================================
FUNCTION: TPIa_fileReadDir

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for reading from a directory and extracting information about an entry.
        The entry can be a file or another directory in here.

ARGUMENTS PASSED:
        dirName--- Path name of the directory from which to read.
        pos---Index of the directory entry to get information about
        nameBuf----Buffer where the name of the entry should be stored. The name string must be null terminated.
        nameBufLength----Max length of the buffer as defined by the calling module.
        type---TPI_FILE_DIRTYPE means the type of directory entry.
                 TPI_FILE_FILETYPE means the type of file entry.
        lSize---Size of the file(in bytes) for file entries, 0 for directories.
    
RETURN VALUE:
        TPI_FILE_OK---if read successfully, or a negative value will be returned:
        TPI_FILE_ERROR_PATH:dirName not found.
        TPI_FILE_ERROR_INVALID:the value of pos did not represent a valid entry.

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
int TPIa_fileReadDir
(
    const char *dirName,
    int pos,
    char *nameBuf,
    int nameBufLength,
    int *type,
    long *lSize
)
{
    int i = 0, iResult = TPI_FILE_ERROR_INVALID;
    char *pcTpiDirName = NULL;
    IFileMgr *pFileMgr = NULL;
    FileInfo * pInfo = NULL;
    
    /*Get the Instance for IFileMgr*/
    pFileMgr = File_GetFileMgr();
    if(NULL == pFileMgr ||NULL == dirName ||NULL == nameBuf ||NULL == type ||NULL == lSize)
    {
         return iResult;
    }
    /*Convert the file name from WE to BREW*/
    pcTpiDirName = File_PathTpiToBrew(dirName);
    
    /*Make the aim directory as the current root dir*/
    iResult = IFILEMGR_EnumInit(pFileMgr, pcTpiDirName, TRUE);/*TRUE means only enum directory, and FALSE is only for file*/
    if(SUCCESS == iResult)
    {
        /*Get the information of dirs in the current dir*/
        while(IFILEMGR_EnumNext(pFileMgr, pInfo))
        {
            if(i == pos)
            {
                /*Get the information of directory in the current dir*/
                *type = pInfo->attrib;
                MEMSET(nameBuf, 0, nameBufLength);
                STRNCPY(nameBuf, (char*)pInfo->szName , nameBufLength - 1);
                *lSize = pInfo->dwSize;
                iResult = TPI_FILE_OK;
                break;
            }
            i ++;
        }
    }
    else
    {
#ifdef _FILE_DEBUG
        DBGPRINTF("[TPIa_fileReadDir]IFILEMGR_EnumInit is failed!\n");
#endif
        iResult = TPI_FILE_ERROR_PATH;
        return iResult;
    }

    if( i < pos)/*it should continue to find the aim entry in files*/
    {
        /*Make the aim directory as the current root dir*/
        iResult = IFILEMGR_EnumInit(pFileMgr, pcTpiDirName, FALSE);/*TRUE means only enum directory, and FALSE is only for file*/
        if(SUCCESS == iResult)
        {
            /*Get the information of files in the current dir*/
            while(IFILEMGR_EnumNext(pFileMgr, pInfo))
            {
                if(i == pos)
                {
                    /*Get the information of directory in the current dir*/
                    *type = pInfo->attrib;
                    MEMSET(nameBuf, 0, nameBufLength);
                    STRNCPY(nameBuf, (char*)pInfo->szName , nameBufLength - 1);
                    *lSize = pInfo->dwSize;
                    break;
                }
                i ++;
            }            
        }
        else
        {
#ifdef _FILE_DEBUG
            DBGPRINTF("[TPIa_fileReadDir]IFILEMGR_EnumInit is failed!\n");
#endif
            iResult = TPI_FILE_ERROR_PATH;
            return iResult;
        }
    }

    if(TPI_FILE_OK != iResult)
    {
        /*Convert the result from BREW to WE*/
        iResult = File_ConvertResultBrewToTpi(iResult);
    }
    
    return iResult;
}
/*====================================================================================
FUNCTION: TPIa_fileSelect

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for opening a file in WE system.

ARGUMENTS PASSED:
        iFileHandle--- File handle identifying the file.
        iEventType--- The event type, either TPI_FILE_EVENT_READ or TPI_FILE_EVENT_WRITE
    
RETURN VALUE:
        NONE

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
void TPIa_fileSelect
(
    int iFileHandle,
    int iEventType
)
{
}
/*====================================================================================
FUNCTION: TPIa_fileSetSize

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for setting the size of the file with the iFileHandle, either by extending it or 
        by truncating it  to the new size. If the file previously is larger than the new size, the extra data is lost. If it is previously 
        shorter than the new length, bytes between the old and new lengths are read as zeroes.

ARGUMENTS PASSED:
        iFileHandle--- File handle identifying the file.
        lSize---the new requested size of the file
    
RETURN VALUE:
        uiResult---the new size will be returned if set successfully. or a negative value will be returned:
        TPI_FILE_ERROR_ACCESS:when a file is referenced that is not opened in write mode, or write operations is denied on the file
        TPI_FILE_ERROR_INVALID:the size is less than 0.
        TPI_FILE_ERROR_FULL:
        TPI_FILE_ERROR_SIZE:The size is greater than the maximum file system size.
        

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
long TPIa_fileSetSize
(
    int iFileHandle,
    long lSize
)
{
    IFile *pIfile = NULL;
    WE_UINT32 uiResult = TPI_FILE_ERROR_INVALID;

    /*Convert TPI Filehandle to BREW IFile pointer*/
    pIfile = File_FileHandleToIFlie(iFileHandle);
    if(NULL == pIfile)
    {
        return uiResult;
    }

    uiResult = IFILE_Truncate(pIfile, lSize);
    if(SUCCESS == uiResult)/*Operate successfully*/
    {
        uiResult = lSize;
    }
    else
    {
        /*Print the error information*/
#ifdef _FILE_DEBUG
        DBGPRINTF("[TPIa_fileSetSize]Write data to the file is failed,and the error code is %d!\n", uiResult);
#endif
        /*Convert the result from BREW to WE*/
        uiResult = File_ConvertResultBrewToTpi(uiResult);
    }

    return uiResult;
}
/*====================================================================================
FUNCTION: TPIa_fileGetSize

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for getting the size of a file in bytes.

ARGUMENTS PASSED:
        pcFileName--- the name of the file.
    
RETURN VALUE:
        uiResult---The size of the file will be returned if success, or a negative value will be returned:
        TPI_FILE_ERROR_PATH: fileName not found or fileName specifies a directory.

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
long TPIa_fileGetSize (const char *pcFileName)
{
    WE_UINT32 uiResult = TPI_FILE_ERROR_PATH;
    char *pcTpiDirName = NULL;
    IFileMgr *pFileMgr = NULL;
    FileInfo * pInfo = NULL;

    /*Get the Instance for IFileMgr*/
    pFileMgr = File_GetFileMgr();
    if(NULL == pFileMgr ||NULL == pcFileName)
    {
         return uiResult;
    }
    /*Convert the file name from WE to BREW*/
    pcTpiDirName = File_PathTpiToBrew(pcFileName);
    uiResult = IFILEMGR_GetInfo (pFileMgr, pcTpiDirName, pInfo);
    /*Get the detailed error information if remove failed*/
    if(SUCCESS != uiResult)
    {
        uiResult = IFILEMGR_GetLastError(pFileMgr);
        /*Print the error information*/
#ifdef _FILE_DEBUG
        DBGPRINTF("[TPIa_fileGetSize]Write data to the file is failed,and the error code is %d!\n", uiResult);
#endif
        /*Convert the result from BREW to WE*/
        uiResult = File_ConvertResultBrewToTpi(uiResult);
    }
    else
    {
        uiResult = pInfo->dwSize;
    }

    return uiResult;
}
/*====================================================================================
FUNCTION: TPIa_fileCloseAll

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for closing all files that a module has opened.This function can be called even
        if the module has no open files.

ARGUMENTS PASSED:
        uiModId--- the module id of the file locating.
    
RETURN VALUE:
        none

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
void TPIa_fileCloseAll (WE_UINT8 uiModId)
{
    int iFileHandle = 0;
    
    /*Get the opened file list*/
    for(iFileHandle = 0; iFileHandle < MAX_OPEN_FILE_COUNT; iFileHandle++)
    {
        if((s_astFileHandle[iFileHandle].uiModuleId == uiModId) && (NULL != s_astFileHandle[iFileHandle].pIfile))/*The opened file is just in the mode*/
        {
            /*Close the found file*/
            TPIa_fileClose(iFileHandle + 1);
        }
    }
}
/*====================================================================================
FUNCTION: TPIa_fileQuotaGet

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for opening a file in WE system.

ARGUMENTS PASSED:
        uiModId--- the module id of the file locating.
        iRequestId---
        pcPath----
        pcMimeType----
    
RETURN VALUE:
        none

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
void TPIa_fileQuotaGet
(
    WE_UINT8 uiModId,
    WE_INT32 iRequestId,
    const char *pcPath,
    const char *pcMimeType
)
{
}
/*====================================================================================
FUNCTION: TPIa_filePathPropertyGet

CREATE DATE: 2005-9-1
AUTHOR: Fuqingling

DESCRIPTION:
        This is a target platform interface adapter function for opening a file in WE system.

ARGUMENTS PASSED:
        uiModId--- the module id of the file locating.
        iRequestId---
        pcPath----
    
RETURN VALUE:
        none

USED GLOBAL VARIABLES: none

USED STATIC VARIABLES: none

CALL BY:

IMPORTANT NOTES:
    None
=====================================================================================*/
void TPIa_filePathPropertyGet
(
    WE_UINT8 uiModId,
    WE_INT32 iRequestId,
    const char *pcPath
)
{
}
TPIa_fileReadDir
(
    const char *dirName, 
    int pos, 
    char *nameBuf, 
    int nameBufLength, 
    int *type, long 
    *size
);
/**********************************************************************
 * TPI Socket 
 **********************************************************************/
/*create the INetMgr object*/
static INetMgr* NetGetNetMgr(void)
{
    /*Create the INetMgr object again if pINetMgr is invalid*/
    if(NULL == pINetMgr)
    {
        ISHELL_CreateInstance(pIShell, AEECLSID_NET, (void **)&pINetMgr);
    }
    return pINetMgr;
}

/*creat a new socket*/
int TPIa_SocketCreate
(
    WE_UINT8 modId, 
    int socketType, 
    WE_INT32 networkAccountID
)
{
    int i;
    int iResult = TPI_SOCKET_ERROR_RESOURCE_LIMIT;
    
    ISocket *pISocket = NULL;
    NetSocket Type = socketType;
    pINetMgr = NetGetNetMgr();
    /*creates a socket and returns a pointer to the ISocket Interface*/
    pISocket = INETMGR_OpenSocket(pINetMgr, Type);
    
    if(NULL == pISocket)
    {
        /*returns the last error that occurred at the INetMgr Interface*/
        iResult = INETMGR_GetLastError(pINetMgr);  
        switch(iResult)
        {
            case AEE_NET_GENERAL_FAILURE:
                iResult = TPI_SOCKET_ERROR_RESOURCE_LIMIT;
                break;
            case AEE_NET_ESOCKNOSUPPORT:
                iResult = TPI_SOCKET_ERROR_INVALID_PARAM;
                break;
            default:
                iResult = TPI_SOCKET_ERROR_RESOURCE_LIMIT;
                break;
        }
        return iResult;
    }
    /*initiate the socket array*/
    memset(aSocketArray, 0, sizeof(St_Socket) * MAX_COUNT_OF_SOCKET);
    
    for(i = 0; i < MAX_COUNT_OF_SOCKET; ++i)
    {
        if(aSocketArray[i].uiSocketId == 0)
        {
            aSocketArray[i].pSocket = pISocket;
            aSocketArray[i].uiModuleId = modId;
            aSocketArray[i].uiSocketId = i+1;
            aSocketArray[i].uiNetAccountId = networkAccountID;
            iResult = aSocketArray[i].uiSocketId;
            return iResult;
        }
        else
        {
            iResult = TPI_SOCKET_ERROR_RESOURCE_LIMIT;
            return iResult;
        }
    }
    return iResult;
}

/*close a socket*/
int TPIa_SocketClose (int socketId)
{
    int i;
    void *pvUser = NULL;
    ISocket *pISocket = NULL;
    int iResult = TPI_SOCKET_ERROR_BAD_ID;
    
    for(i = 0; i < MAX_COUNT_OF_SOCKET; ++i)
    {
         if(aSocketArray[i].uiSocketId == socketId)
         {
             if(aSocketArray[i].pvUserData != NULL)
             {
                 iResult = TPIa_SocketSend(aSocketArray[i].uiSocketId, aSocketArray[i].pvUserData, aSocketArray[i].uiDataLength);
                 return iResult;
             }
             else
             {
                 pISocket = aSocketArray[i].pSocket;
                 iResult = ISOCKET_Close(pISocket);
                 iResult = ISOCKET_Release(pISocket);
                 if(AEE_NET_SUCCESS == iResult)
                 {
                     ISOCKET_Release(pISocket);
                     memset(&aSocketArray[i], 0, sizeof(St_Socket));
                     return iResult;
                 }
                 else if(AEE_NET_WOULDBLOCK == iResult)
                 {
                     ISOCKET_Writeable(pISocket, NULL, pvUser);
                     return iResult;
                 }
                 else if(AEE_NET_ERROR == iResult)
                 {
                     iResult = ISOCKET_GetLastError(pISocket);
                     return iResult;
                 }
             }
         }
         else
         {
             iResult = TPI_SOCKET_ERROR_BAD_ID;
             return iResult;
         }
    }
    return iResult;
}

int TPIa_SocketAccept (int socketId, we_sockaddr_t *addr)
{
    int i;
    void *pvUser = NULL;
    ISocket *pISocket; 
    ISocket *ppISocket;
    ISocket pTempISocket = {0};
    int iResult = TPI_SOCKET_ERROR_BAD_ID;
    
    St_SockAddrIn stSockAddrIn;
    memset(&stSockAddrIn, 0, sizeof(St_SockAddrIn));
    
    for(i = 0; i < MAX_COUNT_OF_SOCKET; ++i)
    {
        if(aSocketArray[i].uiSocketId == socketId)
        {
            pISocket = aSocketArray[i].pSocket;
            ppISocket = &pTempISocket;
            iResult = ISOCKET_Accept(pISocket, &ppISocket);

            if(AEE_NET_SUCCESS == iResult)
            {
                 for(i = 0; i < MAX_COUNT_OF_SOCKET; ++i)
                 {
                     if(aSocketArray[i].uiSocketId == 0)
                     {
                         aSocketArray[i].uiSocketId = i;
                         aSocketArray[i].pSocket = ppISocket;
                         aSocketArray[i].bStatus = 1;
                     }
                     else
                     {
                         iResult = TPI_SOCKET_ERROR_BAD_ID;
                         return iResult;
                     }
                 }
                 ISOCKET_GetPeerName(ppISocket, &stSockAddrIn.uiSockInAddr, &stSockAddrIn.uiSockInPort);
                 INET_NTOA(stSockAddrIn.uiSockInAddr, addr->addr, 4);
                 addr->port = stSockAddrIn.uiSockInPort;
            }
            else if(AEE_NET_WOULDBLOCK == iResult)
            {
                ISOCKET_Readable(pISocket, NULL, pvUser);
                iResult = TPI_SOCKET_ERROR_BAD_ID;
            }
            else if(AEE_NET_ERROR == iResult)
            {
                iResult = ISOCKET_GetLastError(pISocket);
            }
        }
        else
        {
            iResult = TPI_SOCKET_ERROR_BAD_ID;
        }
    }  
    return iResult;
}

int TPIa_SocketBind (int socketId, we_sockaddr_t *addr)
{
    int i;
    void *pvUser = NULL;
    ISocket *pISocket; 
    int iResult = TPI_SOCKET_ERROR_BAD_ID;
    
    St_SockAddrIn stSockAddrIn;
    memset(&stSockAddrIn, 0, sizeof(St_SockAddrIn));
    
    stSockAddrIn.uiSockInFamily = 0;
    stSockAddrIn.uiSockInPort = addr->port;
    stSockAddrIn.uiSockInAddr = (WE_UINT32)(addr->addr[0] << 24 
        | addr->addr[1] << 16 | addr->addr[2] << 8 | addr->addr[3]);

    for(i = 0; i < MAX_COUNT_OF_SOCKET; ++i)
    {
        if(socketId == aSocketArray[i].uiSocketId && 0 == aSocketArray[i].bStatus)
        {
            pISocket = aSocketArray[i].pSocket;
            aSocketArray[i].bStatus = 1;
            iResult = ISOCKET_Bind(pISocket, stSockAddrIn.uiSockInAddr, stSockAddrIn.uiSockInPort);
            
            if(AEE_NET_SUCCESS == iResult)
            {
                return iResult;
            }
            else if(AEE_NET_WOULDBLOCK == iResult)
            {
                ISOCKET_Writeable(pISocket, NULL, pvUser);
                iResult = TPI_SOCKET_ERROR_BAD_ID;
            }
            else if(AEE_NET_ERROR == iResult)
            {
                iResult = ISOCKET_GetLastError(pISocket);
            }
        }
        else
        {
            iResult = TPI_SOCKET_ERROR_BAD_ID;
        }
    }
    return iResult;
}

int  TPIa_SocketConnect (int socketId, we_sockaddr_t *addr)
{
    int i;
    ISocket *pISocket; 
    int iResult = ERR_SOCKET_GENERAL;

    St_SockAddrIn stSockAddrIn;
    memset(&stSockAddrIn, 0, sizeof(St_SockAddrIn));
        
    stSockAddrIn.uiSockInFamily = 0;
    stSockAddrIn.uiSockInPort = addr->port;
    stSockAddrIn.uiSockInAddr = (WE_UINT32)(addr->addr[0] << 24 
    | addr->addr[1] << 16 | addr->addr[2] << 8 | addr->addr[3]);
    
    for(i = 0; i < MAX_COUNT_OF_SOCKET; ++i)
    {
        if(socketId == aSocketArray[i].uiSocketId)
        {
            pISocket = aSocketArray[i].pSocket;
            iResult = ISOCKET_Connect(pISocket, stSockAddrIn.uiSockInAddr, stSockAddrIn.uiSockInPort, 
            (PFNCONNECTCB)TPIc_SocketConnectResponse, aSocketArray[i].pvUserData);
                        
            if(AEE_NET_SUCCESS == iResult)
            {
                ISOCKET_Writeable(pISocket, TPIc_SocketConnectResponse, aSocketArray[i].pvUserData);
            }
            else if(AEE_NET_ERROR == iResult)
            {
                TPIc_SocketConnectResponse(aSocketArray[i].uiModuleId, socketId, TPI_SOCKET_ERROR_CONNECTION_FAILED);
                iResult = ISOCKET_GetLastError(pISocket);
            }
        }
        else
        {
            TPIc_SocketConnectResponse(aSocketArray[i].uiModuleId, socketId, TPI_SOCKET_ERROR_CONNECTION_FAILED);
            iResult = TPI_SOCKET_ERROR_BAD_ID;
        }
    }   
    return iResult;
} 

void TPIc_SocketConnectResponse (WE_UINT8 modId, int socketId, int result)
{
    St_WeSockConnectResponse  stResponse;
    we_dcvt_t                 stDcvtObj;
    WE_UINT16                 uiLength;
    void                      *pvSignalBuffer, *pvUserData;

    stResponse.iSocketId = socketId;
    stResponse.iResult = (WE_INT16)result;

    we_dcvt_init(&stDcvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
    we_cvt_socket_connect_response(&stDcvtObj, &stResponse);
    uiLength = (WE_UINT16)stDcvtObj.pos;

    pvSignalBuffer = WE_SIGNAL_CREATE (WE_SIG_SOCKET_CONNECT_RESPONSE,
                                       WE_MODID_FRW, modId, uiLength);
    if (pvSignalBuffer != NULL)
    {
      pvUserData = WE_SIGNAL_GET_USER_DATA (pvSignalBuffer, &uiLength);
      we_dcvt_init(&stDcvtObj, WE_DCVT_ENCODE, pvUserData, uiLength, 0);
      we_cvt_socket_connect_response (&stDcvtObj, &stResponse);
      we_signal_send(pvSignalBuffer);
    }
}

int TPIa_SocketGetName (int socketId, we_sockaddr_t *addr)
{
    int i;
    ISocket *pISocket; 
    int iResult = TPI_SOCKET_ERROR_BAD_ID;

    St_SockAddrIn stSockAddrIn;
        memset(&stSockAddrIn, 0, sizeof(St_SockAddrIn));
            
        stSockAddrIn.uiSockInFamily = 0;
        stSockAddrIn.uiSockInPort = addr->port;
        stSockAddrIn.uiSockInAddr = (WE_UINT32)(addr->addr[0] << 24 
        | addr->addr[1] << 16 | addr->addr[2] << 8 | addr->addr[3]);
    
        for(i = 0; i < MAX_COUNT_OF_SOCKET; ++i)
        {
            if(socketId == aSocketArray[i].uiSocketId && 1 == aSocketArray[i].bStatus)
            {
                pISocket = aSocketArray[i].pSocket;
                iResult = ISOCKET_GetSockName(pISocket, stSockAddrIn.uiSockInAddr, stSockAddrIn.uiSockInPort);
                if(AEE_NET_SUCCESS == iResult)
                {
                    INET_NTOA(stSockAddrIn.uiSockInAddr, addr->addr, 4);
                    addr->port = stSockAddrIn.uiSockInPort;
                }
            }
            else
            {
                iResult = TPI_SOCKET_ERROR_BAD_ID;
            }
        }   
    return iResult;
}

void
TPIc_SocketNotify (WE_UINT8 modId, int socketId, int eventType)
{
  we_socket_notify_t  stNotify;
  we_dcvt_t           stDcvtObj;
  WE_UINT16           uiLength;
  void                *pvSignalBuffer, *pvUserData;

  stNotify.socketId = socketId;
  stNotify.eventType = (WE_INT16)eventType;

  we_dcvt_init (&stDcvtObj, WE_DCVT_ENCODE_SIZE, NULL, 0, 0);
  we_cvt_socket_notify (&stDcvtObj, &stNotify);
  uiLength = (WE_UINT16)stDcvtObj.pos;

  pvSignalBuffer = WE_SIGNAL_CREATE (WE_SIG_SOCKET_NOTIFY, WE_MODID_FRW, modId, uiLength);
  if (pvSignalBuffer != NULL) {
    pvUserData = WE_SIGNAL_GET_USER_DATA (pvSignalBuffer, &uiLength);
    we_dcvt_init (&stDcvtObj, WE_DCVT_ENCODE, pvUserData, uiLength, 0);
    we_cvt_socket_notify (&stDcvtObj, &stNotify);

    WE_SIGNAL_SEND (pvSignalBuffer);
  }
}

int  TPIa_SocketListen (int socketId)
{
    int i;
    void *pvUser = NULL;
    int nMaxBacklog = 1;
    ISocket *pISocket; 
    int iResult = ERR_SOCKET_GENERAL;

    for(i = 0; i < MAX_COUNT_OF_SOCKET; ++i)
    {
        if(socketId == aSocketArray[i].uiSocketId && 1 == aSocketArray[i].bStatus)
        {
            pISocket = aSocketArray[i].pSocket;
            iResult = ISOCKET_Listen(pISocket, nMaxBacklog);
            
            if(AEE_NET_SUCCESS == iResult)
            {
                TPIc_SocketNotify(aSocketArray[i].uiModuleId, socketId, TPI_SOCKET_EVENT_ACCEPT);
                iResult = ERR_SOCKET_SUCC;
            }
            else if(AEE_NET_WOULDBLOCK == iResult)
            {
                ISOCKET_Writeable(pISocket, NULL, pvUser);
                iResult = TPI_SOCKET_ERROR_BAD_ID;
            }
            else if(AEE_NET_ERROR == iResult)
            {
                iResult = ISOCKET_GetLastError(pISocket);
            }
            else
            {
                iResult = TPI_SOCKET_ERROR_INVALID_PARAM;
            }
        }
        else
        {
            iResult = TPI_SOCKET_ERROR_BAD_ID;
        }
    }
    return iResult;
}

long TPIa_SocketRecv (int socketId, void *buf, long bufLen)
{
    int i;
    void *pvUser = NULL;
    ISocket *pISocket;
    int iResult = TPI_SOCKET_ERROR_DELAYED;

    if(NULL == buf || 0 == bufLen)
    {
        iResult = TPI_SOCKET_ERROR_INVALID_PARAM;
        return iResult;
    }
    
    for(i = 0; i < MAX_COUNT_OF_SOCKET; ++i)
    {
        if(socketId == aSocketArray[i].uiSocketId && 1 == aSocketArray[i].bStatus)
        {
            pISocket = aSocketArray[i].pSocket;
            iResult = ISOCKET_Read(pISocket, buf, bufLen);

            switch(iResult)
            {
                case 0:
                   iResult = TPI_SOCKET_ERROR_CLOSED;
                   break;
                case AEE_NET_WOULDBLOCK:
                    ISOCKET_Readable(pISocket, NULL, pvUser);
                    iResult = TPI_SOCKET_ERROR_DELAYED;
                    break;
                case AEE_NET_ERROR:
                    iResult = ISOCKET_GetLastError(pISocket);
                    break;
            }
        }
        else
        {
            iResult = TPI_SOCKET_ERROR_BAD_ID;
        }
    }   
    return iResult;
}

long TPIa_SocketRecvFrom (int socketId, void *buf, long bufLen, we_sockaddr_t *fromAddr)
{
    if (fromAddr)
    {
        memset((void*)fromAddr, 0, sizeof(we_sockaddr_t));
    }
    return TPIa_SocketRecv(socketId, buf, bufLen);
}

long TPIa_SocketSend (int socketId, void *data, long dataLen)
{
    int i;
    void *pvUser = NULL;
    ISocket *pISocket;
    int iResult = TPI_SOCKET_ERROR_DELAYED;

    if(NULL == data || 0 == dataLen)
    {
        iResult = TPI_SOCKET_ERROR_INVALID_PARAM;
        return iResult;
    }
    if(MAX_MESSAGE_SIZE < dataLen)
    {
        iResult = TPI_SOCKET_ERROR_MSG_SIZE;
        return iResult;
    }

    for(i = 0; i < MAX_COUNT_OF_SOCKET; ++i)
    {
        if(socketId == aSocketArray[i].uiSocketId && 1 == aSocketArray[i].bStatus)
        {
            pISocket = aSocketArray[i].pSocket;
            iResult = ISOCKET_Write(pISocket, data, dataLen);
            
            switch(iResult)
            {
                case AEE_NET_WOULDBLOCK:
                    ISOCKET_Writeable(pISocket, NULL, pvUser);
                    iResult = TPI_SOCKET_ERROR_DELAYED;
                    break;
                case AEE_NET_ERROR:
                    iResult = ISOCKET_GetLastError(pISocket);
                    break;
            }
        }
        else
        {
            iResult = TPI_SOCKET_ERROR_BAD_ID;
        }
    }    
    return iResult;
}

long TPIa_SocketSendTo (int socketId, void *data, long dataLen, we_sockaddr_t *toAddr)
{
    int i;
    void *pvUser = NULL;
    int iResult = TPI_SOCKET_ERROR_BAD_ID;
    ISocket *pISocket;
    St_SockAddrIn stSockAddrIn;
    memset(&stSockAddrIn, 0, sizeof(St_SockAddrIn));

    if(NULL == data || 0 == dataLen || NULL == toAddr)
    {
        iResult = TPI_SOCKET_ERROR_INVALID_PARAM;
        return iResult;
    }
     if(MAX_MESSAGE_SIZE < dataLen)
    {
        iResult = TPI_SOCKET_ERROR_MSG_SIZE;
        return iResult;
    }

    stSockAddrIn.uiSockInFamily = 0;
    stSockAddrIn.uiSockInPort = toAddr->port;
    stSockAddrIn.uiSockInAddr = (WE_UINT32)(toAddr->addr[0] << 24 
        | toAddr->addr[1] << 16 | toAddr->addr[2] << 8 | toAddr->addr[3]);

    for(i = 0; i < MAX_COUNT_OF_SOCKET; ++i)
    {
        if(socketId == aSocketArray[i].uiSocketId && 1 == aSocketArray[i].bStatus)
        {
            pISocket = aSocketArray[i].pSocket;
            iResult = ISOCKET_SendTo(pISocket, data, dataLen, 
            0, stSockAddrIn.uiSockInAddr, stSockAddrIn.uiSockInPort);

            if(iResult > 0)
            {
                INET_NTOA(stSockAddrIn.uiSockInAddr, toAddr->addr, 4);
                toAddr->port = stSockAddrIn.uiSockInPort;
            }
            else
            {
                switch(iResult)
                {
                    case AEE_NET_WOULDBLOCK:
                        ISOCKET_Writeable(pISocket, NULL, pvUser);
                        iResult = TPI_SOCKET_ERROR_DELAYED;
                        break;
                    case AEE_NET_ERROR:
                        iResult = ISOCKET_GetLastError(pISocket);
                        break;
                }
            }
        }
        else
        {
            iResult = TPI_SOCKET_ERROR_BAD_ID;
        }
    }  
    return iResult;
}

/*Asynchronous handle for the socket*/
int TPIa_SocketSelect (int socketId, int eventType)
{
   return 0;
}

void TPIa_SocketGetHostByName (WE_UINT8 modId, int requestId, const char *domainName, WE_INT32 networkAccountID)
{

}
   
void TPIa_SocketCloseAll (WE_UINT8 modId)
{
     int i;
     ISocket *pISocket = NULL; 
     for(i = 0; i < MAX_COUNT_OF_SOCKET; ++i)
     {
         if(aSocketArray[i].uiModuleId == modId)
         {
             if(aSocketArray[i].uiSocketId != 0)
             {
                 pISocket = aSocketArray[i].pSocket;
                 //ISOCKET_Close(pISocket);
                 //ISOCKET_Release(pISocket);
                 TPIa_SocketClose(aSocketArray[i].uiSocketId);
             }
             else
             {
                 memset(&aSocketArray[i], 0, sizeof(St_Socket));
             }
         }
     }
}

/**********************************************************************
 * TPI Network Account
 **********************************************************************/
   
int TPIa_networkAccountGetBearer(WE_INT32 networkAccountId)
{
   return 0;
}

int TPIa_networkAccountGetName(WE_INT32 networkAccountId, char *buf, int bufLen)
{
   return 0;

}

WE_INT32 TPIa_networkAccountGetFirst(void)
{
   return 0;
}

WE_INT32 TPIa_networkAccountGetNext(WE_INT32 networkAccountId)
{
   return 0;
}

WE_INT32 TPIa_networkAccountGetId(int bearer)
{
   return 0;
}


/**********************************************************************
 * TPI Misc
 **********************************************************************/

void
TPIa_telMakeCall (WE_UINT8 modId, WE_UINT16 telId, const char* number)
{

}

void
TPIa_telSendDtmf (WE_UINT8 modId, WE_UINT16 telId, const char* dtmf)
{

}

void
TPIa_pbAddEntry (WE_UINT8 modId, WE_UINT16 pbId, const char* name, const char* number)
{

}

WE_UINT32 TPIa_timeGetCurrent (void)
{
   return 0;
}

WE_INT16 TPIa_timeGetTimeZone (void)
{
   return TPI_TIME_ZONE_UNKNOWN;
}

void*
TPIa_memAlloc (WE_UINT8 modId, WE_UINT32 size)
{
  return malloc (size);
}

void
TPIa_memFree (WE_UINT8 modId, void* p)
{
  free (p);
}

void*
TPIa_memExternalAlloc (WE_UINT8 modId, WE_UINT32 size)
{
  return malloc (size);
}

void
TPIa_memExternalFree (WE_UINT8 modId, void* p)
{
  free (p);
}

void TPIa_error (WE_UINT8 modId, int errorNo)
{
      
}


void TPIa_logMsg(int type, WE_UINT8 modId, const char *format, ...)
{
}

void TPIa_logData (int type, WE_UINT8 modId, const unsigned char *data, int dataLen)
{
   
}

void TPIa_logSignal (WE_UINT8 srcModId, WE_UINT8 dstModId, const char *data)
{

}


static signed short _CurNetId = -1 ;
void TPIa_setCurNetID(signed short id)
{
    _CurNetId = id;
}


signed short TPIa_getCurNetID(void)
{
    return _CurNetId;
}


void TPIa_killTask (WE_UINT8 modId)
{

}

/****************************************************************
  Widgets
 ***************************************************************/
int TPIa_widgetDisplayGetProperties(WeDisplayProperties* displayProperties) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetSetInFocus(WeWidgetHandle handle, int focus) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetHasFocus(WeWidgetHandle handle) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetRelease(WeWidgetHandle handle) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetReleaseAll(WE_UINT8 modId) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetRemove(WeWidgetHandle handle, WeWidgetHandle handleToBeRemoved) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetAddAction(WeWidgetHandle handle, WeActionHandle action) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetHandleWeEvt(WeWidgetHandle handle, int eventType, int override,
                            int subscribe) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetChangeWeEvtModule(WeWidgetHandle handle, WE_UINT8 modId) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetHandleWeNotification(WeWidgetHandle handle, WE_UINT8 modId,
                                     int notificationType, int subscribe) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetSetTitle(WeWidgetHandle handle, WeStringHandle title,
                        WeImageHandle image) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetSetPosition(WeWidgetHandle handle, const WePosition* position) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetSetSize(WeWidgetHandle handle, const WeSize* size) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetSetMarquee(WeWidgetHandle handle, WeMarqueeType marqueeType,
                          int direction, int repeat, int scrollAmount, int 
scrollDelay) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetGetSize(WeWidgetHandle handle, WeSize* size) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetWeEvent2Utf8(WeEvent *event, int multitap, char *buffer) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetWindowAddGadget(WeWindowHandle window, WeGadgetHandle gadget,
                               const WePosition* position) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetWindowAddWindow(WeWindowHandle window, WeWindowHandle 
addedWindow,
                               const WePosition* position ) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeScreenHandle TPIa_widgetScreenCreate(WE_UINT8 modId, WeStyleHandle 
defaultStyle) 
{
return 0;
}

int TPIa_widgetDisplayAddScreen(WeScreenHandle screen) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetScreenAddWindow(WeScreenHandle screen, WeWindowHandle window,
                               const WePosition* position) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetWindowSetProperties(WeWindowHandle window, int propertyMask,
                                   int setCondition) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetGetInsideArea(WeWidgetHandle handle, WePosition* position,
                             WeSize* size) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeWindowHandle TPIa_widgetPaintboxCreate(WE_UINT8 modId, const WeSize* size,
                                          int propertyMask, WeStyleHandle 
defaultStyle) 
{
return 0;
}

WeWindowHandle TPIa_widgetFormCreate(WE_UINT8 modId, const WeSize* size,
                                      int propertyMask, WeStyleHandle 
defaultStyle) 
{
return 0;
}

int TPIa_widgetTextSetText(WeWidgetHandle text, WeStringHandle initialString, 
int type,
                           int maxSize, WeStringHandle inputString, int 
singleLine) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeWindowHandle TPIa_widgetEditorCreate(WE_UINT8 modId, WeStringHandle 
initialString,
                                        WeStringHandle inputString, 
WeTextType type,
                                        const char* formatString, int 
inputRequired,
                                        int maxSize, int singleLine, const 
WeSize* size,
                                        int propertyMask, WeStyleHandle 
defaultStyle) 
{
return 0;
}

int TPIa_widgetChoiceSetElement(WeWidgetHandle choice, int index, int header,
                                WeStringHandle string1, WeStringHandle string2,
                                WeImageHandle image1, WeImageHandle image2,
                                WeStringHandle toolTip, WE_UINT32 tag, int 
hasSubMenu) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetChoiceRemoveElement(WeWidgetHandle choice, int index) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetChoiceGetElemState(WeWidgetHandle choice, int index) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetChoiceSetElemState(WeWidgetHandle choice, int index, int state) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetChoiceGetElemTag(WeWidgetHandle choice, int index, WE_UINT32* 
tag) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetChoiceSize(WeWidgetHandle choice) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetChoiceGetSelected(WeWidgetHandle choice, int* selected, 
WE_UINT32* tag) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeWindowHandle TPIa_widgetMenuCreate(WE_UINT8 modId, WeChoiceType type,
                                      const WeSize* size,
                                      const WeElementPosition* elementPos,
                                      int shownAttributes, WeWidgetHandle 
parentHandle,
                                      int parentIndex, int propertyMask,
                                      WeStyleHandle defaultStyle) 
{
return 0;
}

WeWindowHandle TPIa_widgetDialogCreate(WE_UINT8 modId, WeStringHandle 
dialogText,
                                        WeDialogType type, int propertyMask) 
{
return 0;
}

WeActionHandle TPIa_widgetActionCreate(WE_UINT8 modId, WeStringHandle label,
                                        WeImageHandle image, int actionType) 
{
return 0;
}

int TPIa_widgetActionGetType(WeActionHandle action, int* actionType) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetGadgetSetProperties(WeGadgetHandle gadget, int propertyMask,
                                   int setCondition) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeGadgetHandle TPIa_widgetStringGadgetCreate(WE_UINT8 modId, WeStringHandle 
text,
                                              const WeSize* size, WE_INT16 
fixedWidth,
                                              int singleLine, int propertyMask,
                                              WeStyleHandle defaultStyle) 
{
return 0;
}

int TPIa_widgetStringGadgetSet(WeGadgetHandle stringGadget, WeStringHandle 
text,
                               int singleLine) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeGadgetHandle TPIa_widgetTextInputCreate(WE_UINT8 modId, WeStringHandle 
initialString,
                                           WeStringHandle inputString, 
WeTextType type,
                                           const char* formatString, int 
inputRequired,
                                           int maxSize, int singleLine,
                                           const WeSize* size, int 
propertyMask,
                                           WeStyleHandle defaultStyle) 
{
return 0;
}

WeGadgetHandle TPIa_widgetSelectgroupCreate(WE_UINT8 modId, WeChoiceType type,
                                             const WeSize* size,
                                             const WeElementPosition* 
elementPos,
                                             int shownAttributes, int 
propertyMask,
                                             WeStyleHandle defaultStyle) 
{
return 0;
}

WeGadgetHandle TPIa_widgetImageGadgetCreate(WE_UINT8 modId, WeImageHandle 
image,
                                             const WeSize* size, WeImageZoom 
imageZoom,
                                             int propertyMask,
                                             WeStyleHandle defaultStyle) 
{
return 0;
}

int TPIa_widgetImageGadgetSet(WeGadgetHandle imageGadget, WeImageHandle image,
                              WeImageZoom imageZoom) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeGadgetHandle TPIa_widgetBarCreate(WE_UINT8 modId, WeBarType barType, int 
maxValue,
                                     int partSize, int initialValue, const 
WeSize* size,
                                     int propertyMask, WeStyleHandle 
defaultStyle) 
{
return 0;
}

int TPIa_widgetBarSetValues(WeGadgetHandle bar, int value, int maxValue, int 
partSize) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetBarGetValues(WeGadgetHandle bar, int* value, int* maxValue, int
* partSize) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeAccessKeyHandle TPIa_widgetSetAccessKey(WeWidgetHandle handle,
                                           const char* accessKeyDefinition, 
int override,
                                           int index) 
{
return 0;
}

int TPIa_widgetRemoveAccessKey(WeWidgetHandle handle, WeAccessKeyHandle 
accessKey) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetDrawLine(WeWidgetHandle weHandle, const WePosition* start,
                        const WePosition* end) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetDrawRect(WeWidgetHandle weHandle, const WePosition* position,
                        const WeSize* size, int fill) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetDrawCircle(WeWidgetHandle weHandle, const WePosition* position,
                          const WeSize* size, int fill) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetDrawString(WeWidgetHandle weHandle, WeStringHandle string,
                          const WePosition* position, const WeSize* maxSize, 
int index,
                          int nbrOfChars, int useBrushStyle) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetDrawImage(WeWidgetHandle weHandle, WeImageHandle image,
                         const WePosition* position, const WeSize* size,
                         WeImageZoom imageZoom, const WePosition* subPosition,
                         const WeSize* subSize) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetDrawPolygon(WeWidgetHandle weHandle, int nrOfCorners,
                           const WE_INT16* corners, int fill) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetDrawAccessKey(WeWidgetHandle weHandle, WeAccessKeyHandle 
accessKey,
                             const WePosition* position) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetHoldDraw(WeWidgetHandle weHandle) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetPerformDraw(WeWidgetHandle weHandle) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeBrushHandle TPIa_widgetGetBrush(WeWidgetHandle handle) 
{
return 0;
}

int TPIa_widgetSetColor(WeWidgetHandle weHandle, const WeColor* color,
                        int background) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetSetLineProperty(WeWidgetHandle weHandle,
                               const WeLineProperty* lineProperty) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetSetTextProperty(WeWidgetHandle handle,
                               const WeTextProperty* textProperty) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetSetPattern(WeWidgetHandle weHandle, const WePattern* pattern,
                          int background) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetSetFont(WeWidgetHandle weHandle, const WeFont* font) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeFontFamilyNumber TPIa_widgetFontGetFamily( const char* fontName) 
{
return -1;
}

WeStringHandle TPIa_widgetStringCreate(WE_UINT8 modId, const char* stringData,
                                        WeStringFormat stringFormat, int 
length,
                                        WeStyleHandle defaultStyle) 
{
return 0;
}

WeStringHandle TPIa_widgetStringGetPredefined(WE_UINT32 resId) 
{
return 0;
}

WeStringHandle TPIa_widgetStringCreateText(WE_UINT8 modId, WeWidgetHandle 
handle) 
{
return 0;
}

WeStringHandle TPIa_widgetStringCreateDateTime(WE_UINT8 modId, WE_UINT32 time,
                                                int dateTime) 
{
return 0;
}

int TPIa_widgetStringGetLength(WeStringHandle string, int lengthInBytes,
                               WeStringFormat format) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetStringGetData(WeStringHandle string, char* buffer,
                             WeStringFormat format) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetStringGetWidth(WeStringHandle string, WeBrushHandle brush, int 
index,
                              int nbrOfChars, WE_INT16* width) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetStringGetNbrOfChars(WeStringHandle string, WeBrushHandle brush
, int index,
                                   WE_INT16 width, int* nbrOfChars) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

int TPIa_widgetFontGetValues(WeFont* font, int* ascent, int* height, int* 
xHeight) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeImageHandle TPIa_widgetImageCreate(WE_UINT8 modId, WeCreateData* imageData,
                                      const char* imageFormat,
                                      WeResourceType resourceType,
                                      WeStyleHandle defaultStyle) 
{
return 0;
}

WeImageHandle TPIa_widgetImageGetPredefined(WE_UINT32 resId, const char* 
resString) 
{
return 0;
}

WeImageHandle TPIa_widgetImageCreateEmpty(WE_UINT8 modId, const WeSize* size,
                                           WeStyleHandle defaultStyle) 
{
return 0;
}

WeImageHandle TPIa_widgetImageCreateNative(WE_UINT8 modId, const WeSize* size) 
{
return 0;
}

WeStyleHandle TPIa_widgetStyleCreate(WE_UINT8 modId, const WeColor* color,
                                      const WeColor* backgroundColor,
                                      const WePattern* foreground,
                                      const WePattern* background,
                                      const WeLineProperty* lineProperty,
                                      const WeFont* font,
                                      const WeTextProperty* textProperty) 
{
return 0;
}

int TPIa_widgetStyleGetDefault(WeStyleCategory styleCategory, WeColor* color,
                               WeColor* backgroundColor, WePattern* foreground,
                               WePattern* background, WeLineProperty* 
lineProperty,
                               WeFont* font, WeTextProperty* textProperty) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeSoundHandle TPIa_widgetSoundCreate(WE_UINT8 modId, WeCreateData* soundData,
                                      const char* soundFormat,
                                      WeResourceType resourceType) 
{
return 0;
}

int TPIa_widgetSoundPlay(WeSoundHandle sound) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}

WeSoundHandle TPIa_widgetSoundGetPredefined(WE_UINT32 resId) 
{
return 0;
}

int TPIa_widgetSoundStop(WeSoundHandle sound) 
{
return TPI_WIDGET_ERROR_NOT_SUPPORTED;
}


/**********************************************************************
 * Object Actions
 **********************************************************************/
void TPIa_objectAction (const char *action_cmd, const char *mime_type, 
                        WeResourceType data_type, const unsigned char *data, 
                        WE_INT32 data_len, const char *src_path, 
                        const char *default_name, const char *content_type){ }



/*crypt funtions*/
int 
TPIa_crptInitialise (WE_UINT8 modId)
{
    return 0;
}

int 
TPIa_crptTerminate (WE_UINT8 modId)
{
    return 0;
}

int
TPIa_crptHash (WE_UINT8 modId, int alg, const unsigned char *data, 
               int dataLen, unsigned char *digest)
{
    return 0;
}

int
TPIa_crptHashInit (WE_UINT8 modId, int alg, WE_UINT32* handleptr)
{
    return 0;
}

int
TPIa_crptHashUpdate (WE_UINT32 handle, const unsigned char *part, 
                     int partLen)
{
    return 0;
}

int
TPIa_crptHashFinal (WE_UINT32 handle, unsigned char *digest)
{
    return 0;
}

int
TPIa_crptHmacFinal (WE_UINT32 handle, unsigned char *digest)
{
    return 0;
}

int
TPIa_crptHmacUpdate (WE_UINT32 handle, const unsigned char *part, 
                     int partLen)
{
    return 0;
}

int
TPIa_crptHmacInit (WE_UINT8 modId, int alg, const unsigned char *key, 
                   int keyLen, WE_UINT32* handleptr)
{
    return 0;
}

WE_UINT32
TPIa_timeGetTicks (void)
{
    return 0;
}

int
TPIa_crptEncrypt (WE_UINT8 modId, int cipherAlg, we_crpt_key_object_t key,
                  const unsigned char *data, int dataLen,
                  unsigned char *encryptedData)
{
    return 0;
}


int
TPIa_crptDecrypt (WE_UINT8 modId, int cipherAlg, we_crpt_key_object_t key,
                  const unsigned char *data, int dataLen,
                  unsigned char *decryptedData)
{
    return 0;
}

int
TPIa_crptComputeSignature (WE_UINT8 modId, int wid, int pkcAlg,
                           we_crpt_priv_key_t privKey, 
                           const unsigned char *buf, int bufLen)
{
    return 0;
}

int
TPIa_crptVerifySignature (WE_UINT8 modId, int wid, int pkcAlg,
                          we_crpt_pub_key_t pubKey, 
                          const unsigned char *msg, int msgLen, 
                          const unsigned char *sig, int sigLen)
{
    return 0;
}

int
TPIa_crptEncryptPkc (WE_UINT8 modId, int wid,  int pkcAlg, 
                     we_crpt_pub_key_t pubKey, const unsigned char *data,
                     int dataLen)
{
    return 0;
}

int
TPIa_crptGenerateRandom (WE_UINT8 modId, unsigned char *randomData, 
                         int randomLen)
{
    return 0;
}

int
TPIa_crptDhKeyExchange (WE_UINT8 modId, int wid, we_crpt_dh_params_t params)
{
    return 0;
}

int
TPIa_crptGenerateKeyPair (WE_UINT8 modId, int wid,  int pkcAlg, int size)
{
    return 0;
}

int
TPIa_crptEncryptInit (WE_UINT8 modId, int cipherAlg, 
                      const unsigned char *key, int keyLen, 
                      WE_UINT32* handleptr)
{
    return 0;
}

int
TPIa_crptDecryptInit (WE_UINT8 modId, int cipherAlg, 
                      const unsigned char *key, int keyLen,  
                      WE_UINT32* handleptr)
{
    return 0;
}

int
TPIa_crptEncryptUpdate (WE_UINT32 handle, const unsigned char *data, 
                        int dataLen, unsigned char *encryptedData)
{
    return 0;
}

int
TPIa_crptDecryptUpdate (WE_UINT32 handle, const unsigned char *data, 
                        int dataLen, unsigned char *decryptedData)
{
    return 0;
}

int
TPIa_crptEncryptFinal (WE_UINT32 handle)
{
    return 0;
}

int
TPIa_crptDecryptFinal (WE_UINT32 handle)
{
    return 0;
}

