/*==================================================================================================
    FILE NAME : sec_msg.c
    MODULE NAME : SEC

    GENERAL DESCRIPTION    
        This file offers several functions to manage the message systems.
        Deal with the message queue list and the message free list.
        
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                  Tracking
    Date          Author          Number      Description of changes
    ----------   --------------   ---------   --------------------------------------
    2006-07-07      Sam             None           Init
    2006-08-21      Sam             None        modify Sec_MsgRemove

==================================================================================================*/

/*=================================================================================================
    Include File Section
 *================================================================================================*/
#include "sec_comm.h"

/**************************************************************************************************
 * Macro Define Section
 **************************************************************************************************/
/* the max length of the message free list. */
#define SEC_MSG_MAX_FREE_LIST_LENGTH                10
/* define a little of private data. */
#define SEC_MSG_QUEUE_HEAD                  (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstMsgQueueHead)
#define SEC_MSG_QUEUE_TAIL                  (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstMsgQueueTail)
#define SEC_MSG_FREE_LIST                   (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->pstMsgFreeList)
#define SEC_MSG_FREE_LIST_LEN               (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->iMsgFreeListLen)
#define SEC_MSG_FUNCTION_POINTER            (((St_PublicData *)(((ISec*)hSecHandle)->hPrivateData))->apfnSecMsgFunc)
/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_MsgInit
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION: 
    Initialize the message queue list and message free list, and the 
    array of function pointers.
ARGUMENTS PASSED:
    hSecHandle[IO]: Global data handle.
RETURN VALUE: 
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
void Sec_MsgInit( WE_HANDLE hSecHandle )
{
    WE_INT32    iSubcript = 0;

    if (NULL == hSecHandle)
    {
        return ;
    }
    /* initialize the queue list and free list. */
    SEC_MSG_QUEUE_HEAD = NULL;
    SEC_MSG_QUEUE_TAIL = NULL;
    SEC_MSG_FREE_LIST = NULL;
    SEC_MSG_FREE_LIST_LEN = 0;
    
    /* initialize the array of function pointers. */
    for (iSubcript = 0; iSubcript < M_SEC_MSG_MAX_REGS_FUNC_POINTER; iSubcript++)
    {
        SEC_MSG_FUNCTION_POINTER[iSubcript] = NULL;
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_MsgTerminate
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION: 
    Message system terminated and free the resource of message queue list and freelist.
ARGUMENTS PASSED:
    hSecHandle[IO]: Global data handle.
RETURN VALUE: 
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
void Sec_MsgTerminate( WE_HANDLE hSecHandle )
{
    St_SecMessage   *pstMessage = NULL;
    WE_INT32        iSubcript   = 0;

    if (NULL == hSecHandle)
    {
        return ;
    }
    
    /* free the message queue list */
    while (!Sec_MsgQueueState( hSecHandle )) 
    {
        pstMessage = SEC_MSG_QUEUE_HEAD;
        SEC_MSG_QUEUE_HEAD = pstMessage->pstNext;
        /*added by bird 061114*/
        WE_SIGNAL_DESTRUCT (0, (WE_UINT16)pstMessage->iType, pstMessage->pvParam);
        pstMessage->pvParam = NULL;
        Sec_MsgRemove( hSecHandle, pstMessage );

    }
    
    /* free the message free list */
    while (SEC_MSG_FREE_LIST != NULL) 
    {
        pstMessage = SEC_MSG_FREE_LIST;
        SEC_MSG_FREE_LIST = pstMessage->pstNext;
        if (NULL != pstMessage->pvParam)
        {
            WE_FREE(pstMessage->pvParam);
            pstMessage->pvParam = NULL;
        }
        WE_FREE(pstMessage);
    }
    
    /* free the array of function pointer */
    for (iSubcript = 0; iSubcript < M_SEC_MSG_MAX_REGS_FUNC_POINTER; iSubcript++)
    {
        SEC_MSG_FUNCTION_POINTER[iSubcript] = NULL;
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_MsgProcess
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION: 
    Get the head node from message queue and process the message.
ARGUMENTS PASSED:
    hSecHandle[IO]: Global data handle.
RETURN VALUE: 
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
void Sec_MsgProcess( WE_HANDLE hSecHandle )
{
    St_SecMessage           *pstMessage = NULL;
    Fn_SecMessageFunc       *pfnFunc = NULL;
    WE_INT32                iIndex = 0;

    if (NULL == hSecHandle)
    {
        return ;
    }
    pstMessage = SEC_MSG_QUEUE_HEAD;
    if (pstMessage != NULL) 
    {/* get the head node of the message queue list */
        SEC_MSG_QUEUE_HEAD = pstMessage->pstNext;
        if (NULL == SEC_MSG_QUEUE_HEAD)
        {/* only one node */
            SEC_MSG_QUEUE_TAIL = NULL;
        }
        iIndex = ((WE_UINT32)(pstMessage->iDst)) >> 4;
        
        if ((iIndex >= 0) 
            && (iIndex < M_SEC_MSG_MAX_REGS_FUNC_POINTER) 
            && ((pfnFunc = SEC_MSG_FUNCTION_POINTER[iIndex]) != NULL))
        {/* call function which register already */
            pfnFunc ( hSecHandle, pstMessage);
        }
        else
        {/* error process */
            /*added by bird 061114*/
            WE_SIGNAL_DESTRUCT (0, (WE_UINT16)pstMessage->iType, pstMessage->pvParam);
            pstMessage->pvParam = NULL;
            Sec_MsgRemove( hSecHandle, pstMessage );
        }
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_MsgRegister
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION: 
    Register the function pointers.
ARGUMENTS PASSED:
    hSecHandle[IO]: Global data handle.
    iDst [IN]: destination in scope.
    pfnFunc [IN]: function pointer to register.
RETURN VALUE: 
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
void Sec_MsgRegister
(
    WE_HANDLE   hSecHandle, 
    WE_INT32    iDst, 
    Fn_SecMessageFunc *pfnFunc
)
{
    if ((NULL == hSecHandle) || (NULL == pfnFunc))
    {
        return ;
    }
    /* register the function pointer */
    SEC_MSG_FUNCTION_POINTER[((WE_UINT32)(iDst)) >> 4] = pfnFunc;
}

/*==================================================================================================
FUNCTION: 
    Sec_MsgRemove
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION: 
    Delete the message but not free the resource, move to freelist.
ARGUMENTS PASSED:
    hSecHandle[IO]: Global data handle.
    pstMessage [IN]: pointer to the message which will be delete.
RETURN VALUE: 
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
void Sec_MsgRemove( WE_HANDLE hSecHandle, St_SecMessage *pstMessage )
{
    if ((NULL == pstMessage) || (NULL == hSecHandle))
    {
        return;
    }

    if (SEC_MSG_FREE_LIST_LEN < SEC_MSG_MAX_FREE_LIST_LENGTH) 
    {/* remove the node to the message free list */
        pstMessage->pstNext = SEC_MSG_FREE_LIST;
        SEC_MSG_FREE_LIST = pstMessage;
        SEC_MSG_FREE_LIST_LEN++;
    }
    else
    {
        /* when the length of free list is the max length, free directly. */
        /*delete by bird ,for free memory repeatly,don't free the memory*/
//        WE_SIGNAL_DESTRUCT (0, (WE_UINT16)pstMessage->iType, pstMessage->pvParam);
//        pstMessage->pvParam = NULL;
        WE_FREE(pstMessage);
    }
}
/*==================================================================================================
FUNCTION: 
    Sec_MsgDelById
CREATE DATE:
    2006-11-13
AUTHOR:
    Bird
DESCRIPTION: 
    Delete the message but not free the resource, move to freelist.
ARGUMENTS PASSED:
    hSecHandle[IO]: Global data handle.
    hItype [IN]: I interface type
RETURN VALUE: 
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/

void Sec_MsgDelById(WE_HANDLE hSecHandle)
{
    St_SecMessage   *pstMessage = NULL;
    St_SecMessage   *pstMessagePrev = NULL;
    WE_INT32 iFlag = 0;
    if (NULL == hSecHandle)
    {
        return ;
    }
    
    /* free the message queue list */
    pstMessage = SEC_MSG_QUEUE_HEAD;
    pstMessagePrev = SEC_MSG_QUEUE_HEAD;
    while (NULL != pstMessage) 
    {
        if(hSecHandle == pstMessage->hItype)/*find*/
        {
            /*added by bird for 061113*/
             if (pstMessage == SEC_MSG_QUEUE_HEAD)
             {/* head pointer to the next */
                if(SEC_MSG_QUEUE_HEAD == SEC_MSG_QUEUE_TAIL )
                {
                    SEC_MSG_QUEUE_HEAD = SEC_MSG_QUEUE_TAIL = NULL;
                }
                else
                {
                    SEC_MSG_QUEUE_HEAD = pstMessage->pstNext;
                }
                iFlag = 1;
             }
             else
             {/* get the element pointer */ 
                 pstMessagePrev->pstNext = pstMessage->pstNext;
                 iFlag = 0;
             }
            /*free memory*/
            WE_SIGNAL_DESTRUCT (0, (WE_UINT16)pstMessage->iType, pstMessage->pvParam);
            pstMessage->pvParam = NULL;
            Sec_MsgRemove(hSecHandle, pstMessage);
            pstMessage = pstMessagePrev;
            
        }
        else
        {
            iFlag = 0;
        }
        if(iFlag == 1) /*find head*/
        {
            pstMessage = SEC_MSG_QUEUE_HEAD;
            pstMessagePrev = SEC_MSG_QUEUE_HEAD;
        }
        else
        {
            pstMessagePrev = pstMessage;
            pstMessage = pstMessage->pstNext;
        }
        
    }

    
}
/*==================================================================================================
FUNCTION: 
    Sec_MsgQueueState
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION: 
    Judge that the current state of the message queue list is empty or not.
ARGUMENTS PASSED:
    hSecHandle[IO]: Global data handle.
RETURN VALUE: 
    0 : the message queue is not empty;
    1 : the message queue is empty.
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
WE_INT32 Sec_MsgQueueState( WE_HANDLE hSecHandle )
{
    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    if (SEC_MSG_QUEUE_HEAD == NULL)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*==================================================================================================
FUNCTION: 
    Sec_MsgDispatch
CREATE DATE:
    2006-07-07
AUTHOR:
    Sam
DESCRIPTION: 
    Add the new message to the message queue list.
ARGUMENTS PASSED:
    hSecHandle[IO]: Global data handle.
    iDst [IN]: destination in scope.
    iMessage [IN]: type of message.
    iParam [IN]: param of message.
    uiWParam [IN]: param of message.
    uiLParam [IN]: param of message.
    pvParam [IN]: pointer param of message.
RETURN VALUE: 
    None
USED GLOBAL VARIABLES: 
    None
USED STATIC VARIABLES:
    None
CALL BY: 
    Omit
IMPORTANT NOTES: 
    None
==================================================================================================*/
WE_INT32 Sec_MsgDispatch
(
    WE_HANDLE   hSecHandle, 
    WE_INT32    iDst,
    WE_INT32    iMessage,
    WE_LONG     iParam,
    WE_ULONG    uiWParam,
    WE_ULONG    uiLParam,
    void        *pvParam
)
{
    St_SecMessage    *pstMessage = NULL;

    if (NULL == hSecHandle)
    {
        return M_SEC_ERR_INVALID_PARAMETER;
    }
    if (NULL == SEC_MSG_FREE_LIST) 
    {/* no node. */
        pstMessage = WE_MALLOC(sizeof(St_SecMessage));
        /* malloc failed */
        if (NULL == pstMessage) 
        {            
            return M_SEC_ERR_INSUFFICIENT_MEMORY;
        }
    }
    else 
    {/* get a node from the message free list */
        pstMessage = SEC_MSG_FREE_LIST;
        SEC_MSG_FREE_LIST = pstMessage->pstNext;
        SEC_MSG_FREE_LIST_LEN--;
    }
    /* evaluate */
    pstMessage->iType = iMessage;
    pstMessage->iDst = iDst;
    pstMessage->iParam = iParam;
    pstMessage->uiWParam = uiWParam;
    pstMessage->uiLParam = uiLParam;
    pstMessage->pvParam = pvParam;
    pstMessage->pstNext = NULL;
    pstMessage->hItype = hSecHandle;/*add by Bird 061113*/
    /* intert to the message queue list */
    if (SEC_MSG_QUEUE_TAIL != NULL) 
    {
        SEC_MSG_QUEUE_TAIL->pstNext = pstMessage;
        /*added by bird 061115*/
        SEC_MSG_QUEUE_TAIL = pstMessage;
    }
    else 
    {/* only one node. */
        SEC_MSG_QUEUE_HEAD = SEC_MSG_QUEUE_TAIL = pstMessage;
    }

    return M_SEC_ERR_OK;
}



