

#include "Wifunc.h"


static int iMsg_Req = 0;
static int iMsg_Rsp = 0;


int Msg_Init( int msgKey );
int Msg_Kill(int qid);

//请求
int SendMsgReq(St_MsgReq *pMsgReq)
{
	//int iMsg_id = 0;
	int ret_value = 0;  
	St_MsgReq_Queue stMsgReq_Queue = {0};
	/*
	St_MsgReq * pSt_MsgReq = (St_MsgReq *)malloc(sizeof(St_MsgReq));
	*/
	WIFUNC_LOG_MSG((0,0,"SendMsgReq:msg_type= %d \n",pMsgReq->msg_type));
	WIFUNC_LOG_MSG((0,0,"SendMsgReq:op= %d \n",pMsgReq->op));
	WIFUNC_LOG_MSG((0,0,"SendMsgReq:op_data= %d \n",pMsgReq->op_data));
	WIFUNC_LOG_MSG((0,0,"SendMsgReq:socket_id= %d \n",pMsgReq->socket_id));
	WIFUNC_LOG_MSG((0,0,"SendMsgReq:devid= %d \n",pMsgReq->devid));	
	WIFUNC_LOG_MSG((0,0,"SendMsgReq:unit_type= %d \n",pMsgReq->unit_type));		
	WIFUNC_LOG_MSG((0,0,"SendMsgReq:unit_num= %d \n",pMsgReq->unit_num));	
	/*
	if(NULL == pSt_MsgReq)
	{
		WIFUNC_LOG_MSG((0,0,"AddMsgReq: malloc fail"));
		
	}
	else
	*/
	{
		/*
		memset(pSt_MsgReq,0,,sizeof(St_MsgReq));
		memcpy(pSt_MsgReq,pMsgReq,sizeof(St_MsgReq));
		*/
		//add queue here

		iMsg_Req = Msg_Init(MSG_KEY_REQ);
		
		memset(&stMsgReq_Queue,0,sizeof(St_MsgReq_Queue));
		stMsgReq_Queue.msgtype = MSG_TYPE_MSG1;
		memcpy(&stMsgReq_Queue.stMsgReq, pMsgReq,sizeof(St_MsgReq));	
		
		ret_value = msgsnd(iMsg_Req, &stMsgReq_Queue, sizeof(St_MsgReq_Queue), IPC_NOWAIT);
		if ( ret_value ==  -1 ) 
		{  
		       WIFUNC_LOG_MSG((0,0,"msgsnd() write msg failed,errno=%d[%s]\n",errno,strerror(errno)));  
    		} 

	}
	return 0;
	
}


int  GetMsgReq(St_MsgReq  *pstMsgReq)	
{
	int ret_value = 0;
	St_MsgReq_Queue stMsgReq_Queue = {0};

	if (NULL == pstMsgReq)
	{
		return -1;
	}
	iMsg_Req = Msg_Init(MSG_KEY_REQ);
	ret_value = msgrcv(iMsg_Req,&stMsgReq_Queue,sizeof(St_MsgReq_Queue),MSG_TYPE_MSG1,0); 
	if (ret_value == -1)
	{
		WIFUNC_LOG_MSG((0,0,"GetMsgReq:msgrcv() get msg failed,errno=%d[%s]\n",errno,strerror(errno)));  
		return -1;
	}
	else
	{
		WIFUNC_LOG_MSG((0,0,"GetMsgReq:msg_type= %d \n",stMsgReq_Queue.stMsgReq.msg_type));
		WIFUNC_LOG_MSG((0,0,"GetMsgReq:op= %d \n",stMsgReq_Queue.stMsgReq.op));
		WIFUNC_LOG_MSG((0,0,"GetMsgReq:op_data= %d \n",stMsgReq_Queue.stMsgReq.op_data));
		WIFUNC_LOG_MSG((0,0,"GetMsgReq:socket_id= %d \n",stMsgReq_Queue.stMsgReq.socket_id));
		WIFUNC_LOG_MSG((0,0,"GetMsgReq:devid= %d \n",stMsgReq_Queue.stMsgReq.devid));	
		WIFUNC_LOG_MSG((0,0,"GetMsgReq:unit_type= %d \n",stMsgReq_Queue.stMsgReq.unit_type));		
		WIFUNC_LOG_MSG((0,0,"GetMsgReq:unit_num= %d \n",stMsgReq_Queue.stMsgReq.unit_num));
		
		pstMsgReq->msg_type   = stMsgReq_Queue.stMsgReq.msg_type;
		pstMsgReq->op   = stMsgReq_Queue.stMsgReq.op;
		pstMsgReq->op_data   = stMsgReq_Queue.stMsgReq.op_data;
		pstMsgReq->socket_id   = stMsgReq_Queue.stMsgReq.socket_id;
		pstMsgReq->devid   = stMsgReq_Queue.stMsgReq.devid;
		pstMsgReq->unit_type   = stMsgReq_Queue.stMsgReq.unit_type;
		pstMsgReq->unit_num   = stMsgReq_Queue.stMsgReq.unit_num;

		return 0;
	}

	
}


/*
int RmMsgReq(int socked_id)
{
	
}
*/

//响应
int SendMsgRsp(St_MsgRsp *pMsgRsp)
{
	int ret_value = 0;  
	St_MsgRsp_Queue stMsgRsp_Queue = {0};
	//St_MsgRsp * pSt_MsgRsp = (St_MsgRsp *)malloc(sizeof(St_MsgRsp));
	WIFUNC_LOG_MSG((0,0,"SendMsgRsp:msg_type= %d \n",pMsgRsp->msg_type));
	WIFUNC_LOG_MSG((0,0,"SendMsgRsp:op= %d \n",pMsgRsp->op));
	WIFUNC_LOG_MSG((0,0,"SendMsgRsp:socket_id= %d \n",pMsgRsp->socket_id));
	WIFUNC_LOG_MSG((0,0,"SendMsgRsp:devid= %d \n",pMsgRsp->devid));
	WIFUNC_LOG_MSG((0,0,"SendMsgRsp:unit_type= %d \n",pMsgRsp->unit_type));
	WIFUNC_LOG_MSG((0,0,"SendMsgRsp:unit_num= %d \n",pMsgRsp->unit_num));	
	WIFUNC_LOG_MSG((0,0,"SendMsgRsp:result= %d \n",pMsgRsp->result));		
	/*
	if(NULL == pSt_MsgRsp)
	{
		WIFUNC_LOG_MSG((0,0,"AddMsgReq: malloc fail \n"));
		
	}
	else
	*/
	{
		//memset(pSt_MsgRsp,0,,sizeof(St_MsgRsp));
		//memcpy(pSt_MsgRsp,pMsgRsp,sizeof(St_MsgRsp));
		//add queue here
		
		iMsg_Rsp = Msg_Init(MSG_KEY_RSP);
		
		memset(&stMsgRsp_Queue,0,sizeof(St_MsgRsp_Queue));
		stMsgRsp_Queue.msgtype = MSG_TYPE_MSG2;
		memcpy(&stMsgRsp_Queue.stMsgRsp, pMsgRsp,sizeof(St_MsgRsp));
		
		ret_value = msgsnd(iMsg_Rsp, &stMsgRsp_Queue, sizeof(St_MsgRsp_Queue), IPC_NOWAIT);
		if ( ret_value ==  -1 ) 
		{  
		       WIFUNC_LOG_MSG((0,0,"SendMsgRsp:msgsnd() write msg failed,errno=%d[%s]\n",errno,strerror(errno)));  
    		} 
		
	}
	return 0;
}

int  GetMsgRsp(St_MsgRsp* pStMsgRsp)
{
	int ret_value = 0;
	St_MsgRsp_Queue stMsgRsp_Queue = {0};
	if(NULL == pStMsgRsp)
	{
		return -1;
	}
	iMsg_Rsp = Msg_Init(MSG_KEY_RSP);
	ret_value = msgrcv(iMsg_Rsp,&stMsgRsp_Queue,sizeof(St_MsgRsp_Queue),MSG_TYPE_MSG2,0); 
	if (ret_value == -1)
	{
		WIFUNC_LOG_MSG((0,0,"GetMsgRsp:msgrcv() get msg failed,errno=%d[%s]\n",errno,strerror(errno)));  
		return -1;
	}
	else
	{
		WIFUNC_LOG_MSG((0,0,"GetMsgRsp:msg_type= %d \n",stMsgRsp_Queue.stMsgRsp.msg_type));
		WIFUNC_LOG_MSG((0,0,"GetMsgRsp:op= %d \n",stMsgRsp_Queue.stMsgRsp.op));
		WIFUNC_LOG_MSG((0,0,"GetMsgRsp:socket_id= %d \n",stMsgRsp_Queue.stMsgRsp.socket_id));
		WIFUNC_LOG_MSG((0,0,"GetMsgRsp:devid= %d \n",stMsgRsp_Queue.stMsgRsp.devid));
		WIFUNC_LOG_MSG((0,0,"GetMsgRsp:unit_type= %d \n",stMsgRsp_Queue.stMsgRsp.unit_type));
		WIFUNC_LOG_MSG((0,0,"GetMsgRsp:unit_num= %d \n",stMsgRsp_Queue.stMsgRsp.unit_num));	
		WIFUNC_LOG_MSG((0,0,"GetMsgRsp:result= %d \n",stMsgRsp_Queue.stMsgRsp.result));	
		pStMsgRsp->msg_type = stMsgRsp_Queue.stMsgRsp.msg_type;
		pStMsgRsp->op = stMsgRsp_Queue.stMsgRsp.op;
		pStMsgRsp->socket_id = stMsgRsp_Queue.stMsgRsp.socket_id;
		pStMsgRsp->devid = stMsgRsp_Queue.stMsgRsp.devid;
		pStMsgRsp->unit_type = stMsgRsp_Queue.stMsgRsp.unit_type;
		pStMsgRsp->unit_num = stMsgRsp_Queue.stMsgRsp.unit_num;
		pStMsgRsp->result = stMsgRsp_Queue.stMsgRsp.result;
		
		return 0;
	}

}



/*
int RmMsgRsp()
{
	
}
*/




void FreeMsgReqRsp()
{
	if(iMsg_Req > 0)
	{
		Msg_Kill(iMsg_Req);
	}
	if(iMsg_Rsp > 0)
	{
		Msg_Kill(iMsg_Rsp);
	}
	
}




/*  
消息队列初始化  
msgKey:消息队列键值  
qid:返回值，消息队列id  
*/   
int Msg_Init( int msgKey )   
{   
	int qid;   
	key_t key = msgKey;   
	/*  
	消息队列并非私有，因此此键值的消息队列很可能在其他进程已经被创建  
	所以这里尝试打开已经被创建的消息队列  
	*/   
	qid = msgget(key,0);   
	if(qid < 0)
	{   
		/*  
		打开不成功，表明未被创建  
		现在可以按照标准方式创建消息队列  
		*/   
		qid = msgget(key,IPC_CREAT|0666);   

		if(qid <0)
		{  
			WIFUNC_LOG_MSG((0,0,"failed to create msq | errno=%d [%s]\n",errno,strerror(errno)));  
			return -1;
		}  

		WIFUNC_LOG_MSG((0,0,"Create msg queue id:%d\n",qid));
	}   
	WIFUNC_LOG_MSG((0,0,"msg queue id:%d\n",qid));
	return qid;   
}   



/*  
杀死消息队列  
qid:消息队列id  
*/   
int Msg_Kill(int qid)   
{   
	msgctl(qid, IPC_RMID, NULL);    
	WIFUNC_LOG_MSG((0,0,"Kill queue id:%d\n",qid));
	return 0;   
}  





