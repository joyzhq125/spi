/*=====================================================================================
    FILE NAME :
        Sec_sicmsg.h
    MODULE NAME :
        secsic
    GENERAL DESCRIPTION

    TECHFAITH Software Confidential Proprietary(c)
    Copyright 2002-2006 by TECHFAITH Software. All Rights Reserved.
    =======================================================================================
    Revision History
    Modification              Tracking
    Date       Author         Number    Description of changes
    ---------- -------------- --------- --------------------------------------
    2007-03-20 Bird           none      Init
  
=====================================================================================*/


/*******************************************************************************
*   Multi-Include-Prevent Section
*******************************************************************************/

#ifndef Sec_SICMSG_H
#define Sec_SICMSG_H

/*******************************************************************************
*   Prototype Declare Section
*******************************************************************************/
WE_INT32 Sec_SendMsg( WE_INT32 iFd, WE_VOID *pvMsg );
WE_INT32 Sec_RecvMsg( WE_INT32 iFd, WE_VOID **ppvMsg );
WE_INT32 Sec_InitMsg( WE_VOID );
WE_INT32 Sec_AcceptMsg( WE_INT32 iFd, struct sockaddr_un *pstSockAddr );
WE_INT32 Sec_TerminateMsg(WE_INT32 iFd);

#endif

