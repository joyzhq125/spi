
#ifndef MSGQUEUE_H
#define MSGQUEUE_H


//消息类型
typedef enum tagE_MsgType
{
    //主动上报,请求返回
    E_MSG_PROACT = 0, //主动上报，状态定时更新，报警
    
    E_MSG_REQ,             //请求需返回
    E_MSG_REQ_NO_RSP,  //请求无需返回


    
    E_MSG_REQ_LOCAL,   // 本地请求
    E_MSG_REQ_LOCAL_NO_RSP, //本地请求无需返回
    
    E_MSG_RSP_LOCAL,  //设备状态上报到本地
    E_MSG_MAX

}E_MsgType;



typedef enum tagE_UnitType
{
    //传感器类型
    E_UNIT_KEY = 0, //按键
    E_UNIT_LAMP,
    E_UNIT_IR ,       //红外传感器
    
    E_UNIT_TEMP,  //温度
    E_UNIT_HUM,   //湿度
    E_UNIT_MAX
}E_UnitType;

typedef enum tagE_OP
{
    E_OP_READ = 0,
    E_OP_WRITE,
    E_OP_GETSTATUS,
    E_OP_REBOOT,
    E_OP_REBOOT_DAEMON,
    E_OP_READ_STATUS,
    E_OP_MAX
    
}E_OP;



//请求数据格式
typedef struct tagSt_MsgReq
{
    E_MsgType msg_type;
    E_OP          op;  //读写
    int              op_data; //写的数据,读就忽略
    
    int socket_id; 
    int devid;     //设备id
    E_UnitType unit_type; //单元类型
    unsigned char unit_num; //单元编号
    
} St_MsgReq;


//串口返回数据格式
typedef struct tagSt_MsgRsp
{
    E_MsgType msg_type; //请求返回or 主动上报
    E_OP          op;  //读写
    int socket_id; 
    int devid;     //设备id
    E_UnitType unit_type; //单元类型
    unsigned char unit_num; //单元编号
    int result;  //返回结果
} St_MsgRsp;


/*消息队列键值*/   
#define MSG_KEY_REQ 0x12345  
#define MSG_KEY_RSP 0x54321

enum {   
    MSG_TYPE_START = 0,   
   
    MSG_TYPE_MSG1,   
    MSG_TYPE_MSG2,   
    MSG_TYPE_MSG3,   
    MSG_TYPE_MSG4,   
    MSG_TYPE_MSG5,   
   
    MSG_TYPE_END   
};  



typedef struct tagSt_MsgReq_Queue
{  
	long msgtype;  
	St_MsgReq stMsgReq;  
 }St_MsgReq_Queue;  

typedef struct tagSt_MsgRsp_Queue
{  
	long msgtype;  
	St_MsgRsp stMsgRsp;  
 }St_MsgRsp_Queue;  


int SendMsgReq(St_MsgReq *pMsgReq);
int  GetMsgReq(St_MsgReq  *pstMsgReq);
int SendMsgRsp(St_MsgRsp *pMsgRsp);
int  GetMsgRsp(St_MsgRsp* pStMsgRsp);

#endif


