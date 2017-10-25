/*==================================================================================================
    HEADER NAME : isec.c
    MODULE NAME : SEC
    
    PRE-INCLUDE FILES DESCRIPTION
        
    GENERAL DESCRIPTION
        In this file,define the interface function prototype of sec module.
    
    TECHFAITH Software Confidential Proprietary
    (c) Copyright 2007 by TECHFAITH Software. All Rights Reserved.
====================================================================================================
    Revision History
       
    Modification                   Tracking
    Date          Author            Number      Description of changes
    ----------   ------------    ---------   -------------------------------------------------------
    2007-03-21   Wang Shuhua        None         Draft
==================================================================================================*/
/**************************************************************************************************/
/* Include File Section                                                                           */
/**************************************************************************************************/
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

#include "we_def.h"
#include "we_mem.h"
#include "sec.h"
#include "isecb.h"
#include "isecw.h"
#include "isigntext.h"

#include "sec_uecst.h"
#include "sec_edc.h"

/**************************************************************************************************/
/* Macro Define Section                                                                           */
/**************************************************************************************************/
#define MSG_SIZE  10240
#define WE_LOGERROR  printf
#define ISEC_HANDLE_MAGIC_NUM 0x499602D2
#define UNIXSTR_PATH       "/tmp/.isec"
    
/**************************************************************************************************/
/* Type Define Section                                                                            */
/**************************************************************************************************/
typedef struct tagSt_ISecHandle
{
    WE_INT32 iMagicNumer;    /* for struct pointer valid check */
    WE_INT iSocketFd;   /* communicate with SecD*/
    Fn_ISecEventHandle cbEvtHandler;    /* client event handle callback */
    WE_HANDLE pvCbData;    /* callback function parameter client registered */
    WE_INT32 iIFType;
}St_ISecHandle;

/**************************************************************************************************/
/* Function Define Section                                                                        */
/**************************************************************************************************/


/*==================================================================================================
FUNCTION:
    ISec_Create
CREATE DATE: 
    2007-3-21
AUTHOR: 
    WangShuhua
DESCRIPTION:
    create a new sec interface
ARGUMENTS PASSED:
    WE_INT32 iType :   interface type
    ISEC **ppISec  :   pointer of the interface
RETURN VALUE:
    WE_UINT32 
    G_SEC_OK    :   SUCCESS
    other       :   error code
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
==================================================================================================*/
WE_UINT32 ISec_Create(WE_INT32 iType, ISEC **ppISec)
{
    St_ISecHandle *pstISec = NULL;
    struct sockaddr_un staddr;
    WE_INT32 iSocket = 0;
    WE_UINT32 uiRet = 0;
    WE_VOID *pvMsg = NULL;    
    
    if (NULL == ppISec)
    {
        return G_SEC_INVALID_PARAMETER;
    }
    pstISec = (St_ISecHandle *)WE_MALLOC(sizeof(St_ISecHandle));
    if(NULL == pstISec)
    {
        WE_LOGERROR("ISecW_Create()_____ Fail to allocate memory !");
        return G_SEC_NOTENOUGH_MEMORY;
    }
    WE_MEMSET(pstISec, 0, sizeof(St_ISecHandle));

    pstISec->iMagicNumer = ISEC_HANDLE_MAGIC_NUM;

    if((iSocket = socket(AF_LOCAL,SOCK_STREAM,0)) < 0 )
    {
        WE_LOGERROR("ISecW_Create()_____ Fail to create a valid socket!"); 
        return G_SEC_INVALID_SOCKET;                             
    }
   
    memset(&staddr,0,sizeof(struct sockaddr_in));
    staddr.sun_family = AF_LOCAL;
    strncpy(staddr.sun_path,UNIXSTR_PATH, strlen(UNIXSTR_PATH));
    if(connect(iSocket,( struct sockaddr * )&staddr,sizeof(staddr)) < 0)
    {
        WE_LOGERROR("ISecW_Create()_____ Fail to connect to the server wap daemon!");   
        return G_SEC_INVALID_SOCKET;                                 
    }
    pstISec->iSocketFd = iSocket;
    pstISec->iIFType = iType;
    Sec_EncodeConnType(&pvMsg,iType);    
    uiRet = ISec_SendToSecD(pstISec,pvMsg);
    if (uiRet != G_SEC_OK)
    {
        return uiRet;
    }

    *ppISec = (ISEC*)pstISec;

    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION:
    ISec_SendToSecD
CREATE DATE: 
    2007-3-21
AUTHOR: 
    WangShuhua
DESCRIPTION:
    send message to sec socket
ARGUMENTS PASSED:
    ISEC *pISec     :   SEC handle
    WE_VOID *pvMsg  :   message to send  
RETURN VALUE:
    WE_UINT32 
    G_SEC_OK    :   SUCCESS
    other       :   error code
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
==================================================================================================*/
 WE_UINT32 ISec_SendToSecD
(
    ISEC *pISec,
    WE_VOID *pvMsg
)
{   
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1;; 
    WE_INT32      iSentBytes; 
    WE_INT32      iMsgLen = 0;
    if(NULL == pstISec)
    {
         return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_SendToSecD()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_SendToSecD()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    } 
    iMsgLen = Sec_GetMsgLen(pvMsg);
    if(-1 == (iSentBytes = send(iFd, pvMsg, (ssize_t)iMsgLen, 0))) 
    {
        WE_LOGERROR("ISecW_SendToSecD()_____ fail to send data via socket %d!\r\n",iFd);     
        return G_SEC_INVALID_SOCKET;                                      
    }
    if (pvMsg != NULL)
    {
        WE_FREE(pvMsg);
    }

    return G_SEC_OK;
}
/*==================================================================================================
FUNCTION:
    ISec_RecvFromSecD
CREATE DATE: 
    2007-3-21
AUTHOR: 
    WangShuhua
DESCRIPTION:
    receive data from socket
ARGUMENTS PASSED:
    ISEC *pISec     :   SEC handle
    WE_CHAR *pcBuffer:  store the data received
RETURN VALUE:
    WE_UINT32 
    G_SEC_OK    :   SUCCESS
    other       :   error code
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
==================================================================================================*/
 WE_UINT32 ISec_RecvFromSecD
(
    ISEC *pISec,
    WE_CHAR *pcBuffer
)
{
    St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
    WE_INT        iFd = -1; 
    WE_INT32      iReceivedBytes = -1; 
    WE_INT32      iMsgLen = 0;
    
    if(NULL == pstISec)
    {
        WE_LOGERROR("ISecW_RecvFromSecD()_____ null parameter!\r\n");
        return G_SEC_INVALID_PARAMETER;
    }
    if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
    {
        WE_LOGERROR("ISecW_RecvFromSecD()_____ bad isec pointer!\r\n");
        return G_SEC_BAD_HANDLE;
    }
    if(-1 == (iFd = ISec_GetSocketFd(pstISec)))
    {
        WE_LOGERROR("ISecW_RecvFromSecD()_____ invalid file description\r\n"); 
        return G_SEC_BAD_HANDLE;                                     
    } 
    if((iReceivedBytes = recv(iFd,pcBuffer, SECD_MSG_HEADER_LENGTH, 0)) < 0) 
    {
        WE_LOGERROR("ISecW_RecvFromSecD()_____ fail to send data via socket %d!\r\n",iFd);
        WE_LOGERROR("ISecW_RecvFromSecD()_____ the content of the buffer is  %s!\r\n",pcBuffer);      
        return G_SEC_ERROR_OTHER;                                     
    }
    iMsgLen = Sec_GetMsgLen(pcBuffer) - SECD_MSG_HEADER_LENGTH;
    
    if((iReceivedBytes = recv(iFd, pcBuffer + SECD_MSG_HEADER_LENGTH, iMsgLen, 0)) < 0) 
    {
        WE_LOGERROR("ISecW_RecvFromSecD()_____ fail to send data via socket %d!\r\n",iFd);
        WE_LOGERROR("ISecW_RecvFromSecD()_____ the content of the buffer is  %s!\r\n",pcBuffer);      
        return G_SEC_ERROR_OTHER;                                     
    }
   
    return G_SEC_OK;  
}
/*==================================================================================================
FUNCTION:
    ISec_ExcuteCallBack
CREATE DATE: 
    2007-3-21
AUTHOR: 
    WangShuhua
DESCRIPTION:
         
ARGUMENTS PASSED:
    ISEC *pISec     :   SEC handle
RETURN VALUE:
    WE_UINT32 
    G_SEC_OK    :   SUCCESS
    other       :   error code
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
==================================================================================================*/
 WE_UINT32 ISec_ExcuteCallBack(ISEC *pISec)
 {
     St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
     WE_CHAR acMsg[MSG_SIZE] = {0};
     WE_INT32 iEvent = -1;
     void* pvSecResp = NULL;
     WE_UINT32 uiRet = 0;
 
     if(NULL==pstISec)
     {
         WE_LOGERROR("ISecW_ExcuteCallBack()_____ null parameter!");
         return G_SEC_INVALID_PARAMETER;
     }
     
     if(ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
     {
         WE_LOGERROR("ISecW_ExcuteCallBack()_____ bad isec pointer!");
         return G_SEC_BAD_HANDLE;
     }
     uiRet = ISec_RecvFromSecD(pstISec, acMsg);
     if ( uiRet != G_SEC_OK) /*1 byte Type, 4 bytes data length */
     {
         WE_LOGERROR("ISecW_ExcuteCallBack()_____ Recv error!");
         return uiRet;
     }
     if (Sec_DecodeSecDResp( acMsg, &iEvent, &pvSecResp) < 0)
     {
         WE_LOGERROR("ISecW_ExcuteCallBack()_____ Decode error!");
         return G_SEC_ERROR_OTHER;
     }
     
     pstISec->cbEvtHandler( iEvent, pvSecResp, pstISec->pvCbData );    
 
     return G_SEC_OK;
 }
/*==================================================================================================
FUNCTION:
    ISec_GetSocketFd
CREATE DATE: 
    2007-3-21
AUTHOR: 
    WangShuhua
DESCRIPTION:
    get the socket id communicated with SecD
ARGUMENTS PASSED:
    ISEC *pISec     :   SEC handle
RETURN VALUE:
    WE_INT32    :   the socket id
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
==================================================================================================*/
 WE_INT32 ISec_GetSocketFd(ISEC *pISec)
 {
     St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
     
     if (NULL == pstISec)
     {
         WE_LOGERROR("ISecW_GetSocketFd()_____ null parameter!");
         return G_SEC_INVALID_PARAMETER;
     }
     if (ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
     {
         WE_LOGERROR("ISecW_GetSocketFd()_____ bad isec pointer!");
         return G_SEC_BAD_HANDLE;
     }
     return pstISec->iSocketFd;
 }
/*==================================================================================================
FUNCTION:
    ISec_Release
CREATE DATE: 
    2007-3-21
AUTHOR: 
    WangShuhua
DESCRIPTION:
    release sec handle
ARGUMENTS PASSED:
    ISEC *pISec     :   SEC handle
RETURN VALUE:
    WE_UINT32 
    G_SEC_OK    :   SUCCESS
    other       :   error code
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
==================================================================================================*/
 WE_UINT32 ISec_Release(ISEC *pISec)
 {
     St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
     
     if (NULL == pstISec)
     {
         return G_SEC_INVALID_PARAMETER;
     }
     if (ISEC_HANDLE_MAGIC_NUM != pstISec->iMagicNumer)
     {
         return G_SEC_BAD_HANDLE;
     }
 
     /*close a local socket*/
     close(pstISec->iSocketFd);
     WE_FREE(pstISec);
     return G_SEC_OK;
 }
/*==================================================================================================
FUNCTION:
    ISec_RegClientEventCB
CREATE DATE: 
    2007-3-21
AUTHOR: 
    WangShuhua
DESCRIPTION:
    enregister client event call back
ARGUMENTS PASSED:
    ISEC *pISec,
    WE_HANDLE hPrivData,
    Fn_ISecEventHandle pcbSecEvtFunc
RETURN VALUE:
    WE_UINT32 
    G_SEC_OK    :   SUCCESS
    other       :   error code
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None
CALL BY:
    Omit
IMPORTANT NOTES:
    None 
==================================================================================================*/
 WE_UINT32 ISec_RegClientEventCB
 (
     ISEC *pISec,
     WE_HANDLE hPrivData,
     Fn_ISecEventHandle pcbSecEvtFunc
 )    
 {
     St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
     
     if ((NULL == pISec) || (NULL == hPrivData) || (NULL == pcbSecEvtFunc))
     {
         WE_LOGERROR("ISecW_RegWapEventCB()_____ parameter null!");
         return G_SEC_INVALID_PARAMETER;
     }
     pstISec->cbEvtHandler = pcbSecEvtFunc;
     pstISec->pvCbData = hPrivData;
     return G_SEC_OK;
     
 }
/*==================================================================================================
FUNCTION:
    ISec_DeRegClientEventCB
CREATE DATE: 
    2007-3-21
AUTHOR: 
    WangShuhua
DESCRIPTION:
    deregister client event call back
ARGUMENTS PASSED:
    ISEC *pISec,
    WE_HANDLE hPrivData,
    Fn_ISecEventHandle pcbSecEvtFunc
RETURN VALUE:
    WE_UINT32 
    G_SEC_OK    :   SUCCESS
    other       :   error code
USED GLOBAL VARIABLES:
    None      
USED STATIC VARIABLES:
    None      
CALL BY:
    Omit        
IMPORTANT NOTES:  
    None         
==================================================================================================*/
 WE_UINT32 ISec_DeRegClientEventCB
 (
     ISEC *pISec,
     WE_HANDLE hPrivData,
     Fn_ISecEventHandle pcbSecEvtFunc
 )    
 {
     St_ISecHandle *pstISec = (St_ISecHandle*)pISec;
     
     if ((NULL == pISec) || (NULL == hPrivData) || (NULL == pcbSecEvtFunc))
     {
         WE_LOGERROR("ISecW_DeRegWapEventCB()_____ parameter null!");
         return G_SEC_INVALID_PARAMETER;
     }
     pstISec->cbEvtHandler = NULL;
     pstISec->pvCbData = NULL;
     return G_SEC_OK;
     
 }




