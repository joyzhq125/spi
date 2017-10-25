#include "Wifunc.h"

#define MAX_DEV_NUM 20

stDevInfo devInfo[MAX_DEV_NUM] = {0,};


void InitDevInfo()
{
	memset(devInfo,0,MAX_DEV_NUM * sizeof(stDevInfo));
}

int GetDevInfo(int devid,char unit_type,char unit_num)
{
	int idx = 0;
	for ( idx = 0;idx < MAX_DEV_NUM;idx ++)
	{
		if((devid = devInfo[idx].devid) &&
			(unit_type = devInfo[idx].unit_type) &&
			(unit_num = devInfo[idx].unit_num))
		{
			return devInfo[idx].result;
		}
	}
	return -1;// not found
}

int SetDevInfo(int devid,char unit_type,char unit_num,int result)
{
	int idx = 0;
	for (idx = 0;idx < MAX_DEV_NUM;idx ++)
	{
		if((devid = devInfo[idx].devid) &&
			(unit_type = devInfo[idx].unit_type) &&
			(unit_num = devInfo[idx].unit_num))
		{
			 devInfo[idx].result = result; //found
			 break;
		}
	}
	if(idx == MAX_DEV_NUM) //not found
	{
		for (idx = 0;idx < MAX_DEV_NUM;idx ++)
		{
			if( devInfo[idx].devid == 0)
			{
				devInfo[idx].devid = devid;
				devInfo[idx].unit_type = unit_type;
				devInfo[idx].unit_num = unit_num;	
				devInfo[idx].result = result;	
				break;
			}
		}
	}

	if(idx == MAX_DEV_NUM)
	{
		return -1; //add fail
	}
	else
	{
		return 0;
	}
	
}

int DelDevInfo(int devid,char unit_type,char unit_num)
{
	int idx = 0;
	for (idx = 0;idx < MAX_DEV_NUM;idx ++)
	{
		if((devid = devInfo[idx].devid) &&
			(unit_type = devInfo[idx].unit_type) &&
			(unit_num = devInfo[idx].unit_num))
		{
			devInfo[idx].devid = 0;
			devInfo[idx].unit_type = 0;
			devInfo[idx].unit_num = 0;
			devInfo[idx].result = 0;
			break;
		}
	}
	if(idx == MAX_DEV_NUM)
	{
		return -1; // not found
	}
	else
	{
		return 0;
	}
}

//设备信息上报
int postDevInfo(St_MsgReq *pstMsgReq )
{
	
	St_MsgRsp stMsgRsp = {0,};
	int res = 0;
	res = GetDevInfo(pstMsgReq->devid,pstMsgReq->unit_type,pstMsgReq->unit_num);

	stMsgRsp.msg_type = E_MSG_PROACT; //强制上报
	stMsgRsp.op = pstMsgReq->op;
	stMsgRsp.socket_id = pstMsgReq->socket_id;
	stMsgRsp.devid = pstMsgReq->devid;
	stMsgRsp.unit_type = pstMsgReq->unit_type;
	stMsgRsp.unit_num = pstMsgReq->unit_num;
	stMsgRsp.result = res;
	return SendMsgRsp(&stMsgRsp);
	
}