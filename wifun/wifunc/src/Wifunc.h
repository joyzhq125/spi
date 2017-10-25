#ifndef WIFUNC_H
#define WIFUNC_H


#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>   
#include <string.h> 
#include <stdarg.h>


#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <error.h>
#include <sys/errno.h>
#include <sys/msg.h>
#include <errno.h>


#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/time.h>
#include <semaphore.h>


#include "MsgQueue.h"
#include "log.h"
#include "Ecdr.h"
#include "MsgHdl.h"
#include "UartHdl.h"
#include "DevInfo.h"

typedef void *(* THREAD)(void *);

#define FALSE 0
#define TRUE 1

#define SERVER_PORT                                   6666
#define LENGTH_OF_LISTEN_QUEUE             5


 /*
    接收的原始数据格式	
    msgtype  1 byte 
    op           1 byte
    op_data   4 bytes
    
    devid       4 bytes
    unit_type 1 byte
    unit_num 1 byte
    total 1+1+4+4+1+1 = 12 bytes
  */
#define REQ_DATA_LEN    12

/*
    msgtype  1 bytes 
    op           1 byte
    
    devid       4 bytes
    unit_type 1 byte
    unit_num 1 byte
    result      4 bytes
    total 1+1+4+1+1+4 = 12 bytes
*/
#define RSP_DATA_LEN 12

/*
   写串口数据格式
    startbit     1 byte
    msgtype  1 byte 
    op           1 byte
    op_data   4 bytes
    
    sokect_id  4 bytes
    devid       4 bytes
    unit_type 1 byte
    unit_num 1 byte  
    endbit      1 byte
    total 1+1+1+4+4+4+1+1+1 = 18 bytes
*/
#define WRITE_COM_DATA_LEN    18

/*
 msg_type     1 byte
 op               1 byte
 socket_id     4 bytes
 devid           4 bytes
 unit_type     1 byte
 unit_num     1 byte  
 int result      4 byte
 total 1+1+4+4+1+1+4 = 16 bytes
*/
#define READ_COM_DATA_LEN 16


//在连的socket
typedef struct tagSt_ConSocket
{
    int iSocketFd; 
}St_ConSocket;



//err code  
#define SECCESS                                   0x00
#define GENERAL_ERROR                      0x01                 
#define NOT_SUPPORT                          0x02          
#define INVALID_PARAMETER              0x03      
#define INVALID_FORMAT			     0x04


//信号量
extern sem_t  sem_req;
extern sem_t  sem_rsp;
extern sem_t  sem_uart;



int RspData_Send(St_MsgRsp* pstMsgRsp);
int RspData_Send_broadcast(St_MsgRsp* pstMsgRsp);

#endif //WIFUNC_H
