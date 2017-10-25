/*==================================================================================================
    FILE NAME:
        Sec_daemon.c
    MODULE NAME:
        iwap
    GENERAL DESCRIPTION
        This file implements wapd daemon
        
    Techfaith Software Confidential Proprietary.
    (c) Copyright 2006 by Techfaith Software. All Rights Reserved.
====================================================================================================
    Revision History

    Modification Tracking
    Date             Author               Number       Description of changes
    --------------------------------------------------------------------------------
    2007-03-12  Zhanghuanqing   None         Initialize   
==================================================================================================*/
/*==================================================================================================
*   Include File Section
==================================================================================================*/

#include "sec_comm.h"
/*secd*/
#include "isec.h"
#include "sec_eds.h"
#include "sec_sicmsg.h"
#include "sec_sic.h"
#include "sec_ecdr.h"

#define MAXSOCKFD       16
#define TOTALSOCKFD (MAXSOCKFD * 2)

/*==================================================================================================
* Type Define Section
==================================================================================================*/
typedef struct tagSt_ConnectedFd
{   
    WE_INT32          iSocketFd;
    WE_HANDLE       hSecHandle;  
}St_ConnectedFd;

/*==================================================================================================
*Global variable Define Section
==================================================================================================*/

static WE_UINT32 g_iSec_NumberofConnection = 0 ;
static fd_set         g_rfds = {{0}},g_afds = {{0}}; 
static WE_INT32   g_maxfd = 0; 
static St_ConnectedFd g_iIsConnected[TOTALSOCKFD] = {0};     
static WE_INT32 Secd_CheckSockets(WE_INT32 iFd);
static WE_INT32 Secd_CliConnection(WE_INT32 iListenFd);  

/*==================================================================================================
FUNCTION: 
    main                    
CREATE DATE:
    2007-03-14
AUTHOR:
    Bird Zhang
DESCRIPTION:
    the entry of secd daemon ,initialize sec daemon and start it
ARGUMENTS PASSED:
    None
RETURN VALUE:
     
IMPORTANT NOTES:
    None
==================================================================================================*/ 
WE_INT32 main(WE_INT32 argc, WE_CHAR *argv[])
{
    WE_INT32 iListenSockfd = 0;
    WE_INT32 iSockfd = 0;
    WE_INT32 iRetCon = -1;
    WE_INT32 iMax = -1;
    WE_INT32 iRet = 0;
    WE_INT32 i = 0;
    WE_VOID* pvData = NULL;
    
    for (i = 0; i < TOTALSOCKFD; i ++)
    {   
        g_iIsConnected[i].iSocketFd = -1;
        g_iIsConnected[i].hSecHandle = NULL;   
    }   

     /*listen ,create socket*/
    if ((iListenSockfd = Sec_InitMsg()) < 0)     
    {
        WE_LOG_MSG((0,0,"secd Fail to Initialize can't create sokcet fd.\r\n "));
        return (0);
    }

    WE_LOG_MSG((0,0,"the value of iListenSockfd is %d\r\n",iListenSockfd ));

    g_maxfd = iListenSockfd; 
    FD_ZERO(&g_afds);   
    FD_SET(iListenSockfd,&g_afds); 

    /*start run server end*/
    while(1)  
    {  
        WE_LOG_MSG((0,0,"the max fd is %d\r\n",g_maxfd));
        /*listen socket if the state change ,if not ,blocked*/
        iRet = Secd_CheckSockets(g_maxfd); 
        if (iRet <= 0)
        {
           continue;
        }     
       /*check if a new connection*/
        if (FD_ISSET(iListenSockfd,&g_rfds))     
       {
            /*accept ,setup connection*/
            iRetCon = Secd_CliConnection(iListenSockfd);
        }
                                                                        
        //make maxi equal to the maximum index value of array                                       
        if (iRetCon > iMax)                                              
        {
            iMax = iRetCon;  
        }
     
        /* Handle socket message */                                                                           
        for (i = 0;i <= iMax;i++)   
        {                                           
            if ((iSockfd = g_iIsConnected[i].iSocketFd) < 0)                                            
            {
                continue;                                                                            
            }
            /*check if have data to read*/
            if(FD_ISSET(iSockfd, &g_rfds))
            {
                WE_LOG_MSG((0,0,"New data received fd is %d \r\n", i, g_iIsConnected[i].iSocketFd));
                /*read and add message quque*/
                if((Sec_RecvMsg(iSockfd,&pvData)) > 0 )                
                {   
                    if( (iRet = SecSIc_ReadProc(iSockfd,pvData)) != 0)
                    {
                        WE_LOG_MSG((0,0,"SecSIc_ReadProc Failed err code = %d\n",iRet));
                        WE_LOG_MSG((0,0,"close socket id is %d\n",iSockfd));
                          /*
                         * 1) close socket
                         * 2) remove from select
                         * 3) release interface, such as call isec_release
                         */
                        if (iSockfd == g_maxfd)                                                             
                        {
                            g_maxfd--;
                        }
                        if(0 < g_iSec_NumberofConnection)                                              
                        {
                            g_iSec_NumberofConnection--;                                                                                                    
                        }
                        close(iSockfd);                                                                                                         
                        g_iIsConnected[i].iSocketFd = -1;                                                             
                        FD_CLR(iSockfd,&g_afds);      
                        /*release*/
                       Sec_Release(g_iIsConnected[i].hSecHandle);
                    }
                    
                } 
                /*<= 0*/
                else
                {
                     WE_LOG_MSG((0,0,"socket error happen or socket terminate\n"));
                     WE_LOG_MSG((0,0,"close socket id is %d\n",iSockfd));

                      /* the client was crashed or teminated.
                     * 1) close socket
                     * 2) remove from select
                     * 3) release interface, such as call isec_release
                     */
                    if (iSockfd == g_maxfd)                                                             
                    {
                        g_maxfd--;
                    }
                    if(0 < g_iSec_NumberofConnection)                                              
                    {
                        g_iSec_NumberofConnection--;                                                                                                    
                    }
                    close(iSockfd);                                                                                                         
                    g_iIsConnected[i].iSocketFd = -1;                                                             
                    FD_CLR(iSockfd,&g_afds);      
                    /*release*/
                   Sec_Release(g_iIsConnected[i].hSecHandle);

                }
                /*free buffer*/
                if(NULL != pvData)
                {
                    WE_FREE(pvData);
                    pvData = NULL;
                }
            }   
        }
              
     }           

     /*under the normal case the following is never executed*/
 
     return 0;
}

/*==================================================================================================
FUNCTION: 
    Wapd_CheckSockets                    
CREATE DATE:
    2006-12-05
AUTHOR:
    Bird Zhang
DESCRIPTION:
    check whether some file descriptions of readfds are ready or not  
ARGUMENTS PASSED:
    iFd: maximum file descripiton 
RETURN VALUE:
     0:time out
     -1:an error happens
     >0:the number of ready fd
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 Secd_CheckSockets(WE_INT32 iFd)   
{                                                        
    WE_INT32 iRet = 0;                                    
                                    
    memcpy((char *)&g_rfds,(char *)&g_afds,sizeof(g_rfds));     
    WE_LOG_MSG((0,0,"Wating......\n"));
    iRet = select(iFd + 1,&g_rfds,NULL,NULL,NULL);               

    WE_LOG_MSG((0,0,"New data come in...... \n"));                                                           
    if (iRet < 0)                                       
    {   
        if(errno ==  EINTR) 
        {
           FD_ZERO(&g_rfds);
           return 1;
        }            
        else                                              
        {   
            perror("an error took place in the function CheckSockets\r\n");
            strerror(errno);                                                  
            return   -1;                                            
        }                                
    }   
    else if (iRet == 0)  
    {
        return  iRet;                                            
    } 
    else                                                  
        return 1;
}                                                          
/*==================================================================================================
FUNCTION: 
    Wapd_CliConnection                    
CREATE DATE:
    2006-12-05
AUTHOR:
    Bird Zhang
DESCRIPTION:
    handle the connection from clients   
ARGUMENTS PASSED:
    iListenFd: listening fd 
RETURN VALUE:
     index that indicates the stored position of newly created fd 
IMPORTANT NOTES:
    None
==================================================================================================*/
static WE_INT32 Secd_CliConnection(WE_INT32 iListenFd)                                                                  
{                                                                                                     
    WE_INT32  i,iConnFd;                                                                                 
    struct sockaddr_un cli_addr= {0};                                                                                    
                                                                                  
    if((iConnFd = Sec_AcceptMsg(iListenFd,&cli_addr)) < 0)
    { 
        return   -1;       
    }                                                                                                                                                        
                                                                                                      
    /* Find the first unused array element to store the fd */
    for(i = 0; i < MAXSOCKFD; i++)                                                                                                       
    {                                                                                                                           
        if (g_iIsConnected[i].iSocketFd  <  0)                                                                                      
        {                                                                                                                       
            g_iIsConnected[i].iSocketFd = iConnFd;                                                                              
            break;                                                                                                              
        }                                                                                                                       
    }                                                                                                                           
                                                                                                                                 
    if (g_maxfd < iConnFd)                                                                                                      
    {
        g_maxfd = iConnFd;                                                                                                             
    }
                                                                                                                                 
    FD_SET( iConnFd, &g_afds);
    g_iSec_NumberofConnection++;           
    WE_LOG_MSG((0,0,"There are %d client connected\n",g_iSec_NumberofConnection));

    return i;                                                                                                                 
} 
/*==================================================================================================
FUNCTION: 
    Secd_AddHandle                    
CREATE DATE:
    2006-12-05
AUTHOR:
    Bird Zhang
DESCRIPTION:
    handle the connection from clients   
ARGUMENTS PASSED:
    iListenFd: listening fd 
RETURN VALUE:
     index that indicates the stored position of newly created fd 
IMPORTANT NOTES:
    None
==================================================================================================*/
WE_INT32 Secd_AddHandle(WE_INT32 iFd, WE_HANDLE phSecHandle)
{
    WE_INT32 iIndex= 0;
    WE_INT32 iRet = 0;
    WE_LOG_MSG((0,0,"Secd_AddHandle\n"));
    if((iFd <0) || (NULL == phSecHandle))
    {
        return G_SEC_INVALID_PARAMETER;
    }
    for(iIndex = 0; iIndex < MAXSOCKFD; iIndex++)
    {
        if(g_iIsConnected[iIndex].iSocketFd == iFd)
        {
            g_iIsConnected[iIndex].hSecHandle = phSecHandle;
            iRet = G_SEC_OK;
            break;
        }
    }
    if(iIndex == MAXSOCKFD )
    {
        iRet = G_SEC_GENERAL_ERROR;
    }
    return iRet ;
}
/*==================================================================================================
FUNCTION: 
    Secd_FindMacthHandle                    
CREATE DATE:
    2006-12-05
AUTHOR:
    Bird Zhang
DESCRIPTION:
    handle the connection from clients   
ARGUMENTS PASSED:
    iListenFd: listening fd 
RETURN VALUE:
     index that indicates the stored position of newly created fd 
IMPORTANT NOTES:
    None
==================================================================================================*/

WE_INT32 Secd_FindHandle(WE_INT32 iFd, WE_HANDLE *phSecHandle)
{
    WE_INT32 iIndex= 0;
    WE_INT32 iRet = 0;
    WE_LOG_MSG((0,0,"Secd_FindHandle\n"));
    if((iFd <0) || (NULL == phSecHandle))
    {
        return G_SEC_INVALID_PARAMETER;
    }
    for(iIndex = 0; iIndex < MAXSOCKFD; iIndex++)
    {
        if(g_iIsConnected[iIndex].iSocketFd == iFd)
        {
            *phSecHandle = g_iIsConnected[iIndex].hSecHandle;
            iRet = G_SEC_OK;
            break;
        }
    }
    if(iIndex == MAXSOCKFD )
    {
        *phSecHandle = NULL;
        iRet = G_SEC_GENERAL_ERROR;
    }
    return iRet ;
}





                                                                                                                              
