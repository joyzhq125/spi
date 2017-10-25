

#include "Wifunc.h"

//请求线程消息处理
void * MsgReqHdl()
{
	St_MsgReq stMsgReq = {0};

	WIFUNC_LOG_MSG((0,0,"start thread MsgReqHdl\n"));
	sem_post(&sem_req);  
	while(1)
	{
		memset(&stMsgReq,0,sizeof(St_MsgReq));
		if (0 == GetMsgReq(&stMsgReq))
		{
			switch(stMsgReq.msg_type)
				{
					case E_MSG_PROACT:					
						break;
					case E_MSG_REQ:	
					case E_MSG_REQ_NO_RSP:
						/* 取本地保存状态
						if(E_OP_GETSTATUS ==stMsgReq.op)
						{
							postDevInfo(&stMsgReq);
						}
						else
						*/
						{
							UartWriteData(&stMsgReq);
						}
						break;

					case E_MSG_REQ_LOCAL:  //reset etc.
						break;
					case E_MSG_REQ_LOCAL_NO_RSP:  //rest etc.
						break;
					case E_MSG_RSP_LOCAL:  
						break;						
					default:
						break;
				}		

			
		}
		else
		{
			WIFUNC_LOG_MSG((0,0,"MsgReqHdl:GetMsgReq ERR\n"));
		}

	}
	return (void *)0;
}


void * MsgRspHdl()
{
	St_MsgRsp stMsgRsp = {0};
	WIFUNC_LOG_MSG((0,0,"start thread MsgRspHdl\n"));	
	sem_post(&sem_rsp);
	while(1)
	{
		memset(&stMsgRsp,0,sizeof(St_MsgRsp));

		if(0 == GetMsgRsp(&stMsgRsp))
		{
			switch(stMsgRsp.msg_type)
			{
				case E_MSG_PROACT:
					RspData_Send_broadcast(&stMsgRsp);
					break;
				case E_MSG_REQ:					
					RspData_Send(&stMsgRsp);
					break;
				case E_MSG_REQ_NO_RSP:
				case E_MSG_REQ_LOCAL:  
				case E_MSG_REQ_LOCAL_NO_RSP:  
					break;
				case E_MSG_RSP_LOCAL:  
					break;						
				default:
					break;
			}


		}
		else
		{
			WIFUNC_LOG_MSG((0,0,"MsgRspHdl:GetMsgRsp ERR\n"));
		}
		
	}
	return (void *)0;
}



