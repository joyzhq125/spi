
/*
wifun  daemon
*/




#include "Wifunc.h"




#define MAXSOCKFD      20
//#define TOTALSOCKFD (MAXSOCKFD * 2)


static int iNumCon = 0 ;
static fd_set  rfds = {{0}},afds = {{0}}; 
static int   iMaxfd = 0; 

static St_ConSocket iIsConnected[MAXSOCKFD] = {0};  


pthread_t   thread_req;
pthread_t   thread_rsp;
pthread_t   thread_uart;

static int CheckSockets(int  iFd);
static int CliConnection(int  iListenFd);  
sem_t  sem_req;
sem_t  sem_rsp;
sem_t  sem_uart;


static int InitSocket();
static int CheckSockets(int iFd);
static int ResData_Recv( int iFd, void **ppvData );
static int ResData_Proc(int socketid,void * pvData);
static int CliConnection(int iListenFd);

int main(int argc, char *argv[])
{
	//建立绑定监听socket

	int iListenSockfd = 0;
	int iSockfd = 0;
	int iRetCon = -1;
	int  iMax = -1;
	int iRet = 0;
	int i = 0;
	void * pvData = NULL;

	
	
	iRet = sem_init(&sem_req, 0, 0);
	iRet = sem_init(&sem_rsp, 0, 0);
	iRet = sem_init(&sem_uart, 0, 0);
	InitDevInfo();
	
	iRet = pthread_create(&thread_req, NULL, (THREAD)MsgReqHdl, NULL);
	if(iRet !=0)
	{
		WIFUNC_LOG_MSG((0,0,"create thread thread_req err \r\n "));
		return 0;
	}
	sem_wait(&sem_req);
	sem_destroy(&sem_req); 
	
	iRet = pthread_create(&thread_rsp, NULL, (THREAD)MsgRspHdl, NULL);
	if(iRet !=0)
	{
		WIFUNC_LOG_MSG((0,0,"create thread MsgRspHdl err \r\n "));
		return 0;
	}
	sem_wait(&sem_rsp);
	sem_destroy(&sem_rsp); 
	
	iRet = pthread_create(&thread_uart, NULL, (THREAD)UartMainHdl, NULL);
	if(iRet !=0)
	{
		WIFUNC_LOG_MSG((0,0,"create thread UartMainHdl err \r\n "));
		return 0;
	}
	sem_wait(&sem_uart);
	sem_destroy(&sem_uart); 

	
	for (i = 0; i < MAXSOCKFD; i ++)
	{   
	    iIsConnected[i].iSocketFd = -1;  
	}   

	 /*listen ,create socket*/
	if ((iListenSockfd = InitSocket()) < 0)     
	{
	    WIFUNC_LOG_MSG((0,0,"secd Fail to Initialize can't create sokcet fd.\r\n "));
	    return (0);
	}

	WIFUNC_LOG_MSG((0,0,"the value of iListenSockfd is %d\r\n",iListenSockfd ));
	
	iMaxfd = iListenSockfd; 
	//FD_ZERO(&afds);   
	//FD_SET(iListenSockfd,&afds); 

	while(1)  
	{  
		FD_ZERO(&afds);   
		FD_SET(iListenSockfd,&afds); 
		
	        WIFUNC_LOG_MSG((0,0,"the max fd is %d\r\n",iMaxfd));
	        //连接
	        //select
	        iRet = CheckSockets(iMaxfd); 
	        if (iRet <= 0)
	        {
	           continue;
	        }     
              
	        if (FD_ISSET(iListenSockfd,&rfds))     
	        {
                   //accept
	            iRetCon = CliConnection(iListenSockfd);
	        }	                                                                                                        
	        if (iRetCon > iMax)                                              
	        {
	            iMax = iRetCon; 
				
	        }                                                                      
	        for (i = 0;i <= iMax;i++)   
	        {                                           
	            if ((iSockfd = iIsConnected[i].iSocketFd) < 0)                                            
	            {
	                continue;                                                                            
	            }

	            if(FD_ISSET(iSockfd, &rfds))
	            {
	                WIFUNC_LOG_MSG((0,0,"New data received fd is %d \r\n", iIsConnected[i].iSocketFd));
	               //接收数据 
	               //recv
	                if((ResData_Recv(iSockfd,&pvData)) > 0 )                
	                {   
	                    if( (iRet = ResData_Proc(iSockfd,pvData)) != 0)
	                    {
	                        WIFUNC_LOG_MSG((0,0,"ResData_Proc Failed err code = %d\n",iRet));
	                        WIFUNC_LOG_MSG((0,0,"close socket id is %d\n",iSockfd));

	                        if (iSockfd == iMaxfd)                                                             
	                        {
	                            iMaxfd--; //???
	                        }
	                        if(0 < iNumCon)                                              
	                        {
	                            iNumCon--;                                                                                                    
	                        }
	                        close(iSockfd);                                                                                                         
	                        iIsConnected[i].iSocketFd = -1;                                                             
	                        FD_CLR(iSockfd,&afds);      
	                        WIFUNC_LOG_MSG((0,0,"There are %d client connected\n",iNumCon));

	                    }
	                    
	                } 
	                /*<= 0*/
			 
			  else if(errno ==  EINTR)
			  {
				FD_ZERO(&rfds);
				WIFUNC_LOG_MSG((0,0,"client errno=%d[%s]\n",errno,strerror(errno))); 

			  }
			  
	                else
	                {
	                     WIFUNC_LOG_MSG((0,0,"socket error happen or socket terminate\n"));
	                     WIFUNC_LOG_MSG((0,0,"close socket id is %d\n",iSockfd));

	                    if (iSockfd == iMaxfd)                                                             
	                    {
	                        iMaxfd--;  //???
	                    }
	                    if(0 < iNumCon)                                              
	                    {
	                        iNumCon--;                                                                                                    
	                    }
	                    close(iSockfd);                                                                                                         
	                    iIsConnected[i].iSocketFd = -1;                                                             
	                    FD_CLR(iSockfd,&afds);      
			      WIFUNC_LOG_MSG((0,0,"There are %d client connected\n",iNumCon));

	                }
	                /*free buffer*/
			 if (NULL != pvData)
			 {
				free(pvData);
				pvData = NULL;
			 }

	            }   
	        }
			
	}
	pthread_join(thread_req, NULL);
	pthread_join(thread_rsp, NULL);
	pthread_join(thread_uart, NULL);
	return 0;
}


static int InitSocket(  )
{
    int  iListenSocket = 0;
    struct sockaddr_in  stServerAddr = {0};
    int iOn = 1;   
    //AF_LOCAL
    if ((iListenSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        return -1;
    }
    
    if(setsockopt(iListenSocket,SOL_SOCKET,SO_REUSEADDR,(char *)&iOn,sizeof(iOn)) < 0)
    {
        return   -1;       
    }      
    
    //unlink(UNIXSTR_PATH);
    bzero(&stServerAddr,sizeof(stServerAddr));
    //AF_LOCAL
    
    stServerAddr.sin_family=AF_INET;
    //strncpy(stServerAddr.sun_path, UNIXSTR_PATH, strlen(UNIXSTR_PATH));
    stServerAddr.sin_addr.s_addr = htons(INADDR_ANY);
    stServerAddr.sin_port = htons(SERVER_PORT);
    
    //绑定
    if (bind(iListenSocket, (struct sockaddr*)&stServerAddr, sizeof(stServerAddr)) < 0)
    {
        return -1;
    }
    //监听
    if (listen(iListenSocket, LENGTH_OF_LISTEN_QUEUE) < 0)
    {
        return -1;
    }
    
    return iListenSocket;
}


static int CheckSockets(int iFd)   
{                                                        
    int iRet = 0;                                    
                                    
    memcpy((char *)&rfds,(char *)&afds,sizeof(rfds));     
    WIFUNC_LOG_MSG((0,0,"Wating......\n"));
    iRet = select(iFd + 1,&rfds,NULL,NULL,NULL);               

    WIFUNC_LOG_MSG((0,0,"New data come in...... \n"));                                                           
    if (iRet < 0)                                       
    {   
        if(errno ==  EINTR) 
        {
           FD_ZERO(&rfds);
	    WIFUNC_LOG_MSG((0,0,"CheckSockets write msg failed,errno=%d[%s]\n",errno,strerror(errno))); 
           return 1;
        }            
        else                                              
        {   
            WIFUNC_LOG_MSG((0,0,"an error took place in the function CheckSockets\r\n"));
            //strerror(errno);                                                  
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



static int ResData_Recv( int iFd, void **ppvData )
{
    int iRet = 0;
    int iRecvLen = 0;
    char * pBuf = NULL;
    int iLeft = 0;
    *ppvData = malloc(REQ_DATA_LEN);
    if (NULL == *ppvData)
    {
        return -1;
    }	
    pBuf = (char*)*ppvData;
    iLeft = REQ_DATA_LEN;
    //141118 modify begin
    while(iLeft > 0)
    {
	    iRecvLen = recv(iFd, pBuf, iLeft, 0);
	    if (iRecvLen <= 0)
	    {
	        WIFUNC_LOG_MSG((0,0,"Fail to receive data \r\n"));
	        return (-1);
	    }
	    iLeft -= iRecvLen;
	    pBuf += iRecvLen;
    }
    if (iLeft == 0)
    {
    	iRet = REQ_DATA_LEN;
    }
    //141118 modify end
    if (iRet != REQ_DATA_LEN)
    {
    	WIFUNC_LOG_MSG((0,0,"recv length is %d not %d \r\n",iRet,REQ_DATA_LEN));
    	return -1;
    }
    return iRet;
}

//请求数据处理
static int ResData_Proc(int socketid,void * pvData)
{
	int iRet = 0;
	St_EcdrDecoder *pstDecoder = NULL;
	St_MsgReq stMsgReq = {0};
	char msgtype;
	char op;
	int op_data;
	int devid;
	char unit_type;
	char unit_num;
	
	
	//解析组合数据
	/*
	    接收的原始数据格式	
	    msgtype  1 bytes 
	    op           1 byte
	    op_data   4 bytes
	    
	    devid       4 bytes
	    unit_type 1 byte
	    unit_num 1 byte
	    total 1+1+4+4+1+1 = 12 bytes
	*/
	pstDecoder = Ecdr_CreateDecoder(pvData, REQ_DATA_LEN);
	if (NULL == pstDecoder)
	{
	    return -1;
	}

	// 1byte
	iRet = Ecdr_DecodeOneByte(pstDecoder,&msgtype);
	if (iRet < 0)
	{
		Ecdr_DestroyDecoder(pstDecoder);
		return -1;
	}
	// 1 byte
	iRet = Ecdr_DecodeOneByte(pstDecoder,&op);
	if (iRet < 0)
	{
		Ecdr_DestroyDecoder(pstDecoder);
		return -1;
	}
	// 4 bytes
	iRet = Ecdr_Decode4Bytes(pstDecoder,&op_data);
	if (iRet < 0)
	{
		Ecdr_DestroyDecoder(pstDecoder);
		return -1;
	}
	// 4 bytes 
	iRet = Ecdr_Decode4Bytes(pstDecoder,&devid);
	if (iRet < 0)
	{
		Ecdr_DestroyDecoder(pstDecoder);
		return -1;
	}
	
	// 1byte
	iRet = Ecdr_DecodeOneByte(pstDecoder,&unit_type);
	if (iRet < 0)
	{
		Ecdr_DestroyDecoder(pstDecoder);
		return -1;
	}
	// 1 byte
	iRet = Ecdr_DecodeOneByte(pstDecoder,&unit_num);
	if (iRet < 0)
	{
		Ecdr_DestroyDecoder(pstDecoder);
		return -1;
	}	
	Ecdr_DestroyDecoder(pstDecoder);

	WIFUNC_LOG_MSG((0,0,"ResData_Proc:msgtype=%d,op=%d,op_data=%d,devid=%d,unit_type=%d,unit_num=%d\r\n",
		msgtype,op,op_data,devid,unit_type,unit_num));
	
	stMsgReq.msg_type = msgtype;
	stMsgReq.op = op;
	stMsgReq.op_data = op_data;
	stMsgReq.socket_id = socketid;
	stMsgReq.devid = devid;
	stMsgReq.unit_type = unit_type;
	stMsgReq.unit_num = unit_num;
	
	//加入消息队列
	SendMsgReq(&stMsgReq);
	/*
	if (NULL != pvData)
	{
		free(pvData);
		pvData = NULL;
	}
	*/
	return 0;
}


static int CliConnection(int iListenFd)                                                                  
{                                                                                                     
    int  i,iConnFd,iSockLen;                                                                                 
    struct sockaddr_un cli_addr= {0};                                                                                    
    iSockLen = sizeof(struct sockaddr_un);


    iConnFd = accept(iListenFd, (struct sockaddr*)&cli_addr, (socklen_t *)&iSockLen );
	
    if(iConnFd  < 0)
    { 
        return   -1;       
    }                                                                                                                                                        
                                                                                                      
    /* Find the first unused array element to store the fd */
    for(i = 0; i < MAXSOCKFD; i++)                                                                                                       
    {                                                                                                                           
        if (iIsConnected[i].iSocketFd  <  0)                                                                                      
        {                                                                                                                       
            iIsConnected[i].iSocketFd = iConnFd;                                                                              
            break;                                                                                                              
        }                                                                                                                       
    }                                                                                                                           
                                                                                                                                 
    if (iMaxfd < iConnFd)                                                                                                      
    {
        iMaxfd = iConnFd;                                                                                                             
    }
                                                                                                                                 
    FD_SET( iConnFd, &afds);
    iNumCon++;           
    WIFUNC_LOG_MSG((0,0,"There are %d client connected\n",iNumCon));

    return i;                                                                                                                 
} 



//返回数据发送
int RspData_Send(St_MsgRsp* pstMsgRsp)
{
/*
    msgtype  1 bytes 
    op           1 byte
    
    devid       4 bytes
    unit_type 1 byte
    unit_num 1 byte	 
    result      4 bytes
    total 1+1+4+1+1+4 = 12 bytes
*/
	St_EcdrEncoder *pstEncoder = NULL;
	int idx = 0;
	int iSendNum = 0;
	char msgtype;
	char op;
	char unit_type;

	msgtype = pstMsgRsp->msg_type;
	op = pstMsgRsp->op;
	unit_type = pstMsgRsp->unit_type;
	//保存状态信息
	SetDevInfo(pstMsgRsp->devid,pstMsgRsp->unit_type,pstMsgRsp->unit_num,pstMsgRsp->result);
	
	pstEncoder = Ecdr_CreateEncoder(RSP_DATA_LEN);
	if (!pstEncoder)
	{
		WIFUNC_LOG_MSG((0,0,"RspData_Send:Ecdr_CreateEncoder ERR\n"));
		return -1;
	}

	// 1 bit
	Ecdr_EncodeOneByte( pstEncoder, (char*)&(msgtype));
	// 1 bit
	Ecdr_EncodeOneByte( pstEncoder, (char*)&(op));
	// 4 bits
	Ecdr_Encode4Bytes( pstEncoder, (int*)&(pstMsgRsp->devid));
	// 1 bit
	Ecdr_EncodeOneByte( pstEncoder, (char*)&(unit_type));
	// 1 bit
	Ecdr_EncodeOneByte( pstEncoder, (char*)&(pstMsgRsp->unit_num));	
	// 4 bits
	Ecdr_Encode4Bytes( pstEncoder, (int*)&(pstMsgRsp->result));	
	
	if(RSP_DATA_LEN  != pstEncoder->iPosition)
	{
		Ecdr_DestroyEncoder( pstEncoder );
		return -1;
	}
	else
	{
		for (idx = 0;idx < RSP_DATA_LEN; idx++)
		{
			WIFUNC_LOG_MSG((0,0,"RspData_Send:pstEncoder->pucData[%d] =%u \n",idx,pstEncoder->pucData[idx]));
		}
		WIFUNC_LOG_MSG((0,0,"RspData_Send_broadcast:begin send data socket is %d\n",pstMsgRsp->socket_id ));
		iSendNum = send(pstMsgRsp->socket_id,pstEncoder->pucData,RSP_DATA_LEN,MSG_DONTWAIT);
		if(iSendNum != RSP_DATA_LEN)
		{
			WIFUNC_LOG_MSG((0,0,"RspData_Send:sen rsp data length is %d not %d\n",iSendNum,RSP_DATA_LEN));
			Ecdr_DestroyEncoder( pstEncoder );			
			return -1;
		}
	}
	
	Ecdr_DestroyEncoder( pstEncoder );
	return 0;
	
}

int RspData_Send_broadcast(St_MsgRsp* pstMsgRsp)
{
/*
    msgtype  1 bytes 
    op           1 byte
    
    devid       4 bytes
    unit_type 1 byte
    unit_num 1 byte	 
    result      4 bytes
    total 1+1+4+1+1+4 = 12 bytes
*/
	St_EcdrEncoder *pstEncoder = NULL;
	int idx = 0;
	int idx_socket = 0;
	int iSendNum = 0;
	int socketid;

	char msgtype;
	char op;
	char unit_type;

	msgtype = pstMsgRsp->msg_type;
	op = pstMsgRsp->op;
	unit_type = pstMsgRsp->unit_type;
       //保存状态信息
	SetDevInfo(pstMsgRsp->devid,pstMsgRsp->unit_type,pstMsgRsp->unit_num,pstMsgRsp->result);
	
	pstEncoder = Ecdr_CreateEncoder(RSP_DATA_LEN);
	if (!pstEncoder)
	{
		WIFUNC_LOG_MSG((0,0,"RspData_Send_broadcast:Ecdr_CreateEncoder ERR\n"));
		return -1;
	}
	// 1 bit
	Ecdr_EncodeOneByte( pstEncoder, (char*)&(msgtype));
	// 1 bit
	Ecdr_EncodeOneByte( pstEncoder, (char*)&(op));
	// 4 bits
	Ecdr_Encode4Bytes( pstEncoder, (int*)&(pstMsgRsp->devid));
	// 1 bit
	Ecdr_EncodeOneByte( pstEncoder, (char*)&(unit_type));
	// 1 bit
	Ecdr_EncodeOneByte( pstEncoder, (char*)&(pstMsgRsp->unit_num));	
	// 4 bits
	Ecdr_Encode4Bytes( pstEncoder, (int*)&(pstMsgRsp->result));	
	
	if(RSP_DATA_LEN  != pstEncoder->iPosition)
	{
		return -1;
	}
	else
	{
		for (idx = 0;idx < RSP_DATA_LEN; idx++)
		{
			WIFUNC_LOG_MSG((0,0,"RspData_Send_broadcast:pstEncoder->pucData[%d] =%u \n",idx,pstEncoder->pucData[idx]));
		}

		 for (idx_socket = 0;idx_socket < MAXSOCKFD;idx_socket++)   
	        {                                           
	            if ((socketid = iIsConnected[idx_socket].iSocketFd) > 0)                                            
	            {
	            		WIFUNC_LOG_MSG((0,0,"RspData_Send_broadcast:begin send data socket is %d\n",socketid ));
				iSendNum = send(socketid,pstEncoder->pucData,RSP_DATA_LEN, MSG_DONTWAIT);
				//忽略错误继续发送
				if(iSendNum != RSP_DATA_LEN)
				{
					WIFUNC_LOG_MSG((0,0,"RspData_Send_broadcast:sen rsp data length is %d not %d,socketid is %d\n",iSendNum,RSP_DATA_LEN,socketid));
					//return -1;
				}                                                                           
	            }
		 }
	
	}
	
	Ecdr_DestroyEncoder( pstEncoder );
	return 0;
	
}

