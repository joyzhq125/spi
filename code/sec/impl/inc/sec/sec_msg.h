/*==================================================================================================
    HEADER NAME : sec_msg.h
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
        we_def.h
        
    GENERAL DESCRIPTION
        In this file,define the function prototype 
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2006 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2006-07-04      Sam             None         Init
==================================================================================================*/

/***************************************************************************************************
*   Multi-Include-Prevent Section
***************************************************************************************************/
#ifndef SEC_SIG_H
#define SEC_SIG_H

/***************************************************************************************************
*   include File Section
***************************************************************************************************/

/***************************************************************************************************
*   Macro Define Section
***************************************************************************************************/
#define M_SEC_MSG_DST_HANDLE_MAIN          0x010
#define M_SEC_MSG_DST_WIM_MAIN             0x020

/* the max length of the function pointer array */
#define M_SEC_MSG_MAX_REGS_FUNC_POINTER               3

#define SEC_MSG_DISPATCH_P(s, d, msg, p )        Sec_MsgDispatch ((s),(d), (msg), (WE_LONG)0, (WE_ULONG)0, (WE_ULONG)0, (p) )
#define SEC_MSG_DISPATCH_IP(s, d, msg, i, p )    Sec_MsgDispatch ((s),(d), (msg), (WE_LONG)(i), (WE_ULONG)0, (WE_ULONG)0, (p) )

/***************************************************************************************************
*   Type Define Section
***************************************************************************************************/
/* this strcuture used to store the parameter of message */
typedef struct tagSt_SecMessage 
{
    struct tagSt_SecMessage *pstNext;
    WE_INT32    iDst;       /* decide message type */
    WE_INT32    iType;      /* type of message */
    WE_INT32    iParam;     /* parameter of message */
    WE_UINT32   uiWParam;   /* Specifies additional information about the message */
    WE_UINT32   uiLParam;   /* Specifies additional information about the message */
    void        *pvParam;   /* pointer param of message */
    WE_HANDLE hItype;/*add by bird 061113*/
} St_SecMessage;

/* define the callback function */
typedef void Fn_SecMessageFunc( WE_HANDLE hSecHandle, St_SecMessage *pstMessage );

/***************************************************************************************************
*   Prototype Declare Section
***************************************************************************************************/
/*================================================================================================
 * Initialize 
 *================================================================================================*/
void Sec_MsgInit( WE_HANDLE hSecHandle );

/*================================================================================================
 * Terminate 
 *================================================================================================*/
void Sec_MsgTerminate( WE_HANDLE hSecHandle );

/*================================================================================================
 * Process the message which is in the queue 
 *================================================================================================*/
void Sec_MsgProcess( WE_HANDLE hSecHandle );

/*================================================================================================
 * Judge that the current state of the message queue list is empty or not. 
 *================================================================================================*/
WE_INT32 Sec_MsgQueueState( WE_HANDLE hSecHandle );

/*================================================================================================
 * Delete the message but not free the resource, move to freelist. 
 *================================================================================================*/
void Sec_MsgRemove( WE_HANDLE hSecHandle, St_SecMessage *pstMessage );

/*================================================================================================
 * Register the function pointers.
 *================================================================================================*/
void Sec_MsgRegister
(
    WE_HANDLE   hSecHandle, 
    WE_INT32    iDst, 
    Fn_SecMessageFunc *pfnFunc
);

/*================================================================================================
 * Add the new message to the message queue list. 
 *================================================================================================*/
WE_INT32 Sec_MsgDispatch
(
    WE_HANDLE   hSecHandle, 
    WE_INT32    iDst,
    WE_INT32    iMessage,
    WE_LONG     iParam,
    WE_ULONG    uiWParam,
    WE_ULONG    uiLParam,
    void        *pvParam
);

void Sec_MsgDelById(WE_HANDLE hSecHandle);


#endif /* endif SEC_SIG_H */
