
#ifndef MSGQUEUE_H
#define MSGQUEUE_H


//��Ϣ����
typedef enum tagE_MsgType
{
    //�����ϱ�,���󷵻�
    E_MSG_PROACT = 0, //�����ϱ���״̬��ʱ���£�����
    
    E_MSG_REQ,             //�����践��
    E_MSG_REQ_NO_RSP,  //�������践��


    
    E_MSG_REQ_LOCAL,   // ��������
    E_MSG_REQ_LOCAL_NO_RSP, //�����������践��
    
    E_MSG_RSP_LOCAL,  //�豸״̬�ϱ�������
    E_MSG_MAX

}E_MsgType;



typedef enum tagE_UnitType
{
    //����������
    E_UNIT_KEY = 0, //����
    E_UNIT_LAMP,
    E_UNIT_IR ,       //���⴫����
    
    E_UNIT_TEMP,  //�¶�
    E_UNIT_HUM,   //ʪ��
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



//�������ݸ�ʽ
typedef struct tagSt_MsgReq
{
    E_MsgType msg_type;
    E_OP          op;  //��д
    int              op_data; //д������,���ͺ���
    
    int socket_id; 
    int devid;     //�豸id
    E_UnitType unit_type; //��Ԫ����
    unsigned char unit_num; //��Ԫ���
    
} St_MsgReq;


//���ڷ������ݸ�ʽ
typedef struct tagSt_MsgRsp
{
    E_MsgType msg_type; //���󷵻�or �����ϱ�
    E_OP          op;  //��д
    int socket_id; 
    int devid;     //�豸id
    E_UnitType unit_type; //��Ԫ����
    unsigned char unit_num; //��Ԫ���
    int result;  //���ؽ��
} St_MsgRsp;


/*��Ϣ���м�ֵ*/   
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


