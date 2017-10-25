/*=====================================================================================
    FILE NAME :
        Sec_sic.h
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


/*==================================================================================================
*   Include File 
*=================================================================================================*/
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include "sec_comm.h"
/*secd*/
#include "isec.h"
#include "sec_eds.h"
#include "sec_sicmsg.h"
#include "sec_sic.h"
#include "sec_ecdr.h"

/*******************************************************************************
*   Macro Define Section
*******************************************************************************/
#define Sec_LOCAL_MSG_RECV_BUF_SIZE 500
#define Sec_LOCAL_MSG_LISTEN_PORT  5550
#define UNIXSTR_PATH "/tmp/.isec"

/*******************************************************************************
*   Function Define Section
*******************************************************************************/
/*==================================================================================================
FUNCTION: 
    Sec_SendMsg                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    sent encoded data to client. 
ARGUMENTS PASSED:
    WE_INT32 iFd[IN]: socket id
    WE_VOID* pvData[IN]: encoded data which be send to client.
    
RETURN VALUE:
    error code
IMPORTANT NOTES:
    Whatever send success or fail, pvMsg should be free in this function
==================================================================================================*/
WE_INT32 Sec_SendMsg( WE_INT32 iFd, WE_VOID *pvMsg)
{
    WE_INT32 iMsgLen = 0;
    if (pvMsg)
    {
        iMsgLen = Sec_GetMsgLen(pvMsg);
    }
    if (send( iFd, pvMsg, iMsgLen, 0) < 0)
    {
        return -1;
    }
    return 0;
}
/*==================================================================================================
FUNCTION: 
    Sec_RecvMsg                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    recieve encoded data from client. 
ARGUMENTS PASSED:
    WE_INT32 iFd[IN]: socket id
    WE_VOID* pvData[IN]:  encoded data from client.
    
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_RecvMsg( WE_INT32 iFd, WE_VOID **ppvMsg )
{
    WE_CHAR acBuf[Sec_LOCAL_MSG_RECV_BUF_SIZE] = {0};
    WE_VOID *pvBuf = NULL;
    WE_INT32 iRet = 0;
    WE_INT32 iRecvLen = 0;
    
    iRet = recv(iFd, acBuf, SEC_ED_MSG_HEADER_LENGTH, 0);
    if (iRet <= 0)
    {
        WE_LOG_MSG((0,0,"Fail to receive data \r\n"));
        return (-1);
    }
    iRecvLen = Sec_GetMsgLen(acBuf) - SEC_ED_MSG_HEADER_LENGTH;
    
    if (iRecvLen <= 0)
    {
        WE_LOG_MSG((0,0,"Fail to receive data \r\n"));
        return (-1);
    }    

    if (iRecvLen > 0)
    {
        iRet = recv( iFd, acBuf + SEC_ED_MSG_HEADER_LENGTH, iRecvLen, 0);
        if (iRet <= 0)
        {
            WE_LOG_MSG((0,0,"Fail to receive data \r\n"));
            return (-1);
        }
    }    

    *ppvMsg = WE_MALLOC(SEC_ED_MSG_HEADER_LENGTH + iRecvLen);
    if (NULL == *ppvMsg)
    {
        return -1;
    }
    WE_MEMCPY(*ppvMsg, acBuf, SEC_ED_MSG_HEADER_LENGTH + iRecvLen);
    
    return iRet;
}

/*==================================================================================================
FUNCTION: 
    Sec_InitMsg                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    creat socket. 
ARGUMENTS PASSED:
    None.    
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_InitMsg( WE_VOID )
{
    WE_INT iListenSocket = 0;
    struct sockaddr_un  stServerAddr = {0};
    WE_INT iOn = 1;   

    if ((iListenSocket = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0)
    {
        return -1;
    }
    
    if(setsockopt(iListenSocket,SOL_SOCKET,SO_REUSEADDR,(char *)&iOn,sizeof(iOn)) < 0)
    {
        return   -1;       
    }      
    
    unlink(UNIXSTR_PATH);
    bzero(&stServerAddr,sizeof(stServerAddr));
    stServerAddr.sun_family=AF_LOCAL;
    strncpy(stServerAddr.sun_path, UNIXSTR_PATH, strlen(UNIXSTR_PATH));

    if (bind(iListenSocket, (struct sockaddr*)&stServerAddr, sizeof(stServerAddr)) < 0)
    {
        return -1;
    }

    if (listen(iListenSocket, 5) < 0)
    {
        return -1;
    }
    
    return iListenSocket;
}
/*==================================================================================================
FUNCTION: 
    Sec_AcceptMsg                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    accepts an incoming connection attempt on a socket 
ARGUMENTS PASSED:
    WE_INT32 iFd[IN]:socket id.
    struct sockaddr_un *pstSockAddr[IN]: pointer to a buffer that receives the address of the connecting entity.   
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_AcceptMsg( WE_INT32 iFd, struct sockaddr_un *pstSockAddr )
{
    WE_INT32 iRet = 0;
    //struct sockaddr_un  stAddr = {0};
    WE_INT32 iSockLen = 0;
    
    iSockLen = sizeof(struct sockaddr_un);

    if (NULL == pstSockAddr)
    {
        return -1;
    }
    
    iRet = accept(iFd, (struct sockaddr*)pstSockAddr, (socklen_t *)&iSockLen );

    return iRet;
}
/*==================================================================================================
FUNCTION: 
    Sec_TerminateMsg                    
CREATE DATE:
    2007-03-22
AUTHOR:
    Bird Zhang
DESCRIPTION:
    close socket 
ARGUMENTS PASSED:
    WE_INT32 iFd[IN]:socket id.
RETURN VALUE:
    error code
IMPORTANT NOTES:
    None.
==================================================================================================*/
WE_INT32 Sec_TerminateMsg(WE_INT32 iFd)
{
    return close(iFd);
}

