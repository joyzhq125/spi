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
#include "We_Def.h"
#include "We_Cfg.h"
#include "Int_Pipe.h"
#include "AEEStdLib.h"

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
    WE_BOOL hStatus;
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
 * TPI signal functions
 **********************************************************************/

/*===========================================================================
=======================
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
=============================================================================
=====================*/

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
            stSignal.pSignalNode = (P_St_SignalNode)MALLOC(sizeof(
St_SignalNode) * MAX_COUNT_OF_SIGNAL);
            if(NULL != stSignal.pSignalNode)
            {
                /*Init free node*/
                memset(stSignal.pSignalNode, 0 , sizeof(St_SignalNode) * 
MAX_COUNT_OF_SIGNAL);
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

/*===========================================================================
=======================
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
=============================================================================
=====================*/

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

/*===========================================================================
=======================
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
=============================================================================
=====================*/

void* TPIa_SignalAllocMemory (WE_UINT16 uiSignalDataLength)
{
    return (void*)MALLOC(uiSignalDataLength);
}

/*===========================================================================
=======================
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
=============================================================================
=====================*/

void TPIa_SignalFreeMemory (void* pvSignalData)
{
    if(NULL != pvSignalData)
    {
        FREE(pvSignalData);
    }
}

/*===========================================================================
=======================
    FUNCTION: TPIa_SignalSend

    DESCRIPTION:
        When this function is called the integration must put the signal last 
        in the signal queue labelled with destModId. If the signal queue 
        labelled with destModId doesn't exist or it isn't register yet or 
        the signal queue is full, the signal must be deleted and this
        function must return an error code. The signal data is allocated 
        before by the function TPIa_signalAllocMemory.       

    ARGUMENTS PASSED:
        destModId        : The Mobile Suite id of the destination signal queue.
        signalData       : A pointer to the signal data.
        signalDataLength : The length of the signal data.
        
    RETURN VALUE:
        TPI_SIGNAL_OK, or a negative value if an error occurred - error codes:
        TPI_SIGNAL_ERROR_QUEUE_FULL: The signal queue is full.
        TPI_SIGNAL_ERROR_QUEUE_UNKNOWN: The signal queue doesn¡¯t exist or is
                                        not registering yet.

    IMPORTANT NOTES:
        Describe anything that help understanding the function and important 
        aspect of using the function i.e side effect..etc
=============================================================================
=====================*/

int TPIa_SignalSend (WE_UINT8 uiModuleId, void* pvSignalData, WE_UINT16 
uiSignalDataLength)
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
            tempnode->hStatus = 1;
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

/*===========================================================================
=======================
    FUNCTION: TPIa_SignalRetrieve

    DESCRIPTION:
        Retrieve the first signal in the signal queue labelled destModId. 
        The signal data memory is released later by a call to the function 
        TPIa_signalFreeMemory. If the signal queue labelled with destModId
        doesn't exist, it isn't register yet or the signal queue is empty,
        return MSF_NULL. This function can only be called from a module's run
        function.
        
    ARGUMENTS PASSED:
        destModId: The Mobile Suite id of the module, label of the signal 
        queue.

    RETURN VALUE:
        A pointer to the signal data, if no signal to retrieve return MSF_NULL.

    IMPORTANT NOTES:

=============================================================================
=====================*/

void* TPIa_SignalRetrieve (WE_UINT8 uiDestModuleId)
{
    void *pvResult = NULL;
    P_St_SignalNode pstTempnode = NULL;

    if(TPIa_SignalQueueLength(uiDestModuleId) > 0 &&
        stSignal.pSignalNode != NULL)
    {
        if(stSignal.header[uiDestModuleId].pSignalHead != NULL)
        {
            pvResult = stSignal.header[uiDestModuleId].pSignalHead->
pvSigalData;
            stSignal.header[uiDestModuleId].pSignalHead->hStatus = 0;
            pstTempnode = stSignal.pFreeNode;
            stSignal.pFreeNode = stSignal.header[uiDestModuleId].pSignalHead;
            stSignal.header[uiDestModuleId].pSignalHead = stSignal.header[
uiDestModuleId].pSignalHead->pstNext;
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

/*===========================================================================
=======================
    FUNCTION: TPIa_SignalQueueLength

    DESCRIPTION:
        Returns the number of signals in the signal queue labelled modId. If 
        the signal queue labelled with destModId doesn¡¯t exist or it isn't
        register yet, return -1. This function
        can only be called from a module¡¯s wantsToRun function.

    ARGUMENTS PASSED:
        ModId: The Mobile Suite id of the module, label of the signal queue.
         
    RETURN VALUE:
        The number of signals in the queue.

    IMPORTANT NOTES:
        None
=============================================================================
=====================*/

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

int
TPIa_fileOpen (WE_UINT8 modId, const char *fileName, int mode, long size)
{
  return 0;
}

int
TPIa_fileClose (int fileHandle)
{
  return 0;
}

long
TPIa_fileWrite (int fileHandle, void *data, long size)
{
  return 0;
}

long
TPIa_fileRead (int fileHandle, void *data, long size)
{
   return 0;
}

void
TPIa_fileFlush (int fileHandle)
{
}

long
TPIa_fileSeek (int fileHandle, long offset, int seekMode)
{
  return 0;
}

int
TPIa_fileRemove (const char *fileName)
{
  return 0;
}

int
TPIa_fileRename (const char *srcName, const char *dstName)
{
  return 0;
}

int
TPIa_fileMkDir (const char *dirName)
{
   return 0;
}

int
TPIa_fileRmDir (const char *dirName)
{
   return 0;
}

int
TPIa_fileGetSizeDir (const char *dirName)
{
   return 0;
}

int
TPIa_fileReadDir (const char *dirName, int pos, char *nameBuf, int 
nameBufLength, int *type, long *size)
{
  return 0;
}

void
TPIa_fileSelect (int fileHandle, int eventType)
{
}

long
TPIa_fileSetSize (int fileHandle, long size)
{
   return 0;
}

long
TPIa_fileGetSize (const char *fileName)
{
  return 0;
}

void
TPIa_fileCloseAll (WE_UINT8 modId)
{
}

void
TPIa_fileQuotaGet (WE_UINT8 modId, WE_INT32 requestId, const char *path, 
const char *mimeType)
{
}

void
TPIa_filePathPropertyGet (WE_UINT8 modId, WE_INT32 requestId, const char *path)
{
}

/**********************************************************************
 * TPI Socket 
 **********************************************************************/

int TPIa_socketCreate (WE_UINT8 modId, int socketType, WE_INT32 
networkAccountID)
{  
   return 0;
}

int TPIa_socketClose (int socketId)
{
   return 0;
}

int TPIa_socketAccept (int socketId, we_sockaddr_t *addr)
{
   return 0;
}

int TPIa_socketBind (int socketId, we_sockaddr_t *addr)
{
   return 0;
}

int  TPIa_socketConnect (int socketId, we_sockaddr_t *addr)
{
   return 0;
} 

int TPIa_socketGetName (int socketId, we_sockaddr_t *addr)
{
   return 0;
}

int  TPIa_socketListen (int socketId)
{
   return 0;
}

long TPIa_socketRecv (int socketId, void *buf, long bufLen)
{
  return 0;
}

long TPIa_socketRecvFrom (int socketId, void *buf, long bufLen, we_sockaddr_t 
*fromAddr)
{
   return 0;
}

long TPIa_socketSend (int socketId, void *data, long dataLen)
{
   return 0;
}

long TPIa_socketSendTo (int socketId, void *data, long dataLen, we_sockaddr_t 
*toAddr)
{
   return 0;
}

int TPIa_socketSelect (int socketId, int eventType)
{
   return 0;
}

void TPIa_socketGetHostByName (WE_UINT8 modId, int requestId, const char *
domainName, WE_INT32 networkAccountID)
{

}
   
void TPIa_socketCloseAll (WE_UINT8 modId)
{

}

/**********************************************************************
 * TPI Network Account
 **********************************************************************/
   
int TPIa_networkAccountGetBearer(WE_INT32 networkAccountId)
{
   return 0;
}

int TPIa_networkAccountGetName(WE_INT32 networkAccountId, char *buf, int 
bufLen)
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
TPIa_pbAddEntry (WE_UINT8 modId, WE_UINT16 pbId, const char* name, const char
* number)
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

void TPIa_logData (int type, WE_UINT8 modId, const unsigned char *data, int 
dataLen)
{
   
}

void TPIa_logSignal (WE_UINT8 srcModId, WE_UINT8 dstModId, const char *data)
{

}


WE_UINT8 TPIa_getModuleID (void)
{
  return 0;
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

