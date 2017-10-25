


#include "Wifunc.h"

char *dev = "/dev/ttyS0"; 
char startbit = 's';
char endbit = 'e';

int fd_com = 0;


/**

*@brief 设置串口通信速率

*@param fd 类型 int 打开串口的文件句柄

*@param speed 类型 int 串口速度

*@return void

*/

int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,

B38400, B19200, B9600, B4800, B2400, B1200, B300, };

int name_arr[] = {38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 

19200, 9600, 4800, 2400, 1200, 300, };


/*设置波特率*/
void set_speed(int fd, int speed)
{

	int i; 

	int status; 

	struct termios Opt;

	tcgetattr(fd, &Opt); 

	for ( i= 0; i < sizeof(speed_arr) / sizeof(int); i++) 
	{ 

		if (speed == name_arr[i]) 
		{ 

			tcflush(fd, TCIOFLUSH); 

			cfsetispeed(&Opt, speed_arr[i]); 

			cfsetospeed(&Opt, speed_arr[i]); 

			status = tcsetattr(fd, TCSANOW, &Opt); 

			if (status != 0) 
			{ 

				WIFUNC_LOG_MSG((0,0,"tcsetattr fd")); 	
				return; 

			} 

			tcflush(fd,TCIOFLUSH); 

		} 

	}

}



/**
	设置校验函数

	*@brief 设置串口数据位，停止位和效验位

	*@param fd 类型 int 打开的串口文件句柄

	*@param databits 类型 int 数据位 取值 为 7 或者8

	*@param stopbits 类型 int 停止位 取值为 1 或者2

	*@param parity 类型 int 效验类型 取值为N,E,O,,S

*/

int set_Parity(int fd,int databits,int stopbits,int parity)

{ 
	struct termios options; 
	if ( tcgetattr( fd,&options) != 0) 
	{ 
		WIFUNC_LOG_MSG((0,0,"SetupSerial 1\n")); 
		return(FALSE); 

	}
	//bzero(&options, sizeof(options));
	//
	options.c_cflag |= CLOCAL|CREAD;//|HUPCL;
	options.c_cflag &= ~CSIZE; 
	switch (databits) /*设置数据位数*/
	{ 
		case 7: 
			options.c_cflag |= CS7; 
			break;
		case 8: 
			options.c_cflag |= CS8;
			break; 
		default: 
			WIFUNC_LOG_MSG((0,0,"Unsupported data sizen\n")); 
			return (FALSE); 

	}

	switch (parity) 
	{ 
		case 'n':
		case 'N': 
			options.c_cflag &= ~PARENB; /* Clear parity enable */
			options.c_iflag &= ~INPCK; /* Enable parity checking */ 
			break; 
		case 'o': 
		case 'O': 
			options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/ 
			options.c_iflag |= INPCK; /* Disnable parity checking */ 
			break; 
		case 'e': 
		case 'E': 
			options.c_cflag |= PARENB; /* Enable parity */ 
			options.c_cflag &= ~PARODD; /* 转换为偶效验*/ 
			options.c_iflag |= INPCK; /* Disnable parity checking */
			break;
		case 'S': 
		case 's': /*as no parity*/ 
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break; 
		default: 
			WIFUNC_LOG_MSG((0,0,"Unsupported parityn\n")); 
			return (FALSE); 

	} 

	//设置停止位
	switch (stopbits)
	{ 
		case 1: 
			options.c_cflag &= ~CSTOPB; 
			break; 
		case 2: 
			options.c_cflag |= CSTOPB; 
			break;
		default: 
			WIFUNC_LOG_MSG((0,0,"Unsupported stop bitsn")); 
			return (FALSE); 

	} 
	options.c_cflag &=~(CRTSCTS); //no flow control
	options.c_lflag = 0;	
	options.c_iflag = 0;
	options.c_oflag = 0; 

	/* Set input parity option */ 
	if (parity != 'n') 
	{
		//options.c_iflag |= INPCK; 
        }
	tcflush(fd,TCIFLUSH);
	options.c_cc[VTIME] = 0; /* 设置超时*/ 
	options.c_cc[VMIN] = READ_COM_DATA_LEN; 
	if (tcsetattr(fd,TCSANOW,&options) != 0) 
	{ 
		WIFUNC_LOG_MSG((0,0,"SetupSerial 3")); 
		return (FALSE); 
	} 
	return (TRUE); 
}


int OpenDev(char *Dev)

{
	//O_NDELAY
	int fd = open( Dev, O_RDWR|O_NOCTTY|O_NDELAY); 
	if (-1 == fd) 
	{ 
		WIFUNC_LOG_MSG((0,0,"Can't Open Serial Port\n"));
		return -1; 
	} 

	else 
	{	/*FNDELAY*/
		/*
		if (fcntl(fd,F_SETFL,0) < 0)
		{
			WIFUNC_LOG_MSG((0,0,"fcntl failed\n"));
			return -1;
		}
		*/
		return fd;
	}

}


//串口数据主线程
void*  UartMainHdl()
{
	int nread;
	int nleft;
	char * pBuf = NULL;
	char read_buf[16] = {0};
	St_EcdrDecoder *pstDecoder = NULL;
	int ret = 0;
	fd_set  ReadSetFD;
	
	WIFUNC_LOG_MSG((0,0,"start thread UartMainHdl\n"));	
	sem_post(&sem_uart);
	fd_com = OpenDev(dev);
	if (fd_com == -1)
	{
		WIFUNC_LOG_MSG((0,0,"opendev Errorn\n"));
		return (void*)0;
	}
	set_speed(fd_com,9600);
	if (set_Parity(fd_com,8,1,'N') == FALSE) 
	{
		WIFUNC_LOG_MSG((0,0,"Set Parity Errorn\n"));
		return (void*)0;
	}

	//FD_ZERO(&ReadSetFD);   
	//FD_SET(fd_com,&ReadSetFD); 
	while (1) 
	{ 
		int idx = 0;
		bzero(read_buf,sizeof(read_buf));	
		pBuf = read_buf;
		nread = 0;
		nleft = READ_COM_DATA_LEN;

		FD_ZERO(&ReadSetFD);   
		FD_SET(fd_com,&ReadSetFD); 
		//清除数据
		tcflush(fd_com,TCIFLUSH);
		/*
		tv.tv_sec = 0; 
 		tv.tv_usec = 500000; 
		z = select( MaxFd, &readset, 0, 0, &tv); 
 		}while( z==-1 && errno==EINTR ); 

		 if( z == -1 ) 
		 printf("select(2)\n"); 
		 if( z == 0 ) 
		 { 
			 hComm = -1; 
		 } 
		 */
		if (select(fd_com + 1, &ReadSetFD, NULL, NULL, NULL) < 0) 
		{
			WIFUNC_LOG_MSG((0,0,"select err errno=%d[%s]\n",errno,strerror(errno)));
			continue;
		}	
		//接收串口数据
		if (FD_ISSET(fd_com, &ReadSetFD)) 
		{
			while(nleft > 0)
	 		{
	 			WIFUNC_LOG_MSG((0,0,"UartMainHdl:read \n"));
		 		if((nread = read(fd_com, pBuf, nleft)) > 0)
				{
					nleft -= nread;
					pBuf += nread;
				}
				else //nread = -1 nread < 0
				{
					break;
				}
			}
			if(nleft == 0)
			{			
				char msg_type;
				char op;
				int    socket_id;
				int    devid;
				char unit_type;
				char unit_num;
				int    result;
				St_MsgRsp stMsgRsp = {0};
				nread = READ_COM_DATA_LEN;
				if (READ_COM_DATA_LEN == nread)
				{
					for (idx = 0;idx < nread;idx++)
					{
						WIFUNC_LOG_MSG((0,0,"UartMainHdl:read_buf[%d]=%u \n",idx,read_buf[idx]));
					}
					//解码
					/*
					 msg_type     1 byte
					 op               1 byte
					 socket_id     4 bytes
					 devid           4 bytes
					 unit_type     1 byte
					 unit_num     1 byte  
					 result      4 byte
					 total 1+4+4+1+1+4 = 16 bytes
					*/
		
					pstDecoder = Ecdr_CreateDecoder(read_buf, READ_COM_DATA_LEN);
					// 1 byte
					Ecdr_DecodeOneByte(pstDecoder,&msg_type);
					// 1 byte
					Ecdr_DecodeOneByte(pstDecoder,&op);				
					// 4 bytes
					Ecdr_Decode4Bytes(pstDecoder,&socket_id);
					// 4 bytes
					Ecdr_Decode4Bytes(pstDecoder,&devid);    	
					// 1 byte
					Ecdr_DecodeOneByte(pstDecoder,&unit_type);
					// 1 byte
					Ecdr_DecodeOneByte(pstDecoder,&unit_num);
					// 1 byte
					Ecdr_Decode4Bytes(pstDecoder,&result);
					
					Ecdr_DestroyDecoder(pstDecoder);
					stMsgRsp.msg_type = msg_type;
					stMsgRsp.msg_type = op;
					stMsgRsp.socket_id = socket_id;
					stMsgRsp.devid = devid;
					stMsgRsp.unit_type = unit_type;
					stMsgRsp.unit_num = unit_num;
					stMsgRsp.result = result;
					
					ret = SendMsgRsp(&stMsgRsp);
		

				}
				else
				{
					WIFUNC_LOG_MSG((0,0,"UartMainHdl:read length is %d not %d \n",nread,READ_COM_DATA_LEN));
				}
				    
			}
			else
			{
				WIFUNC_LOG_MSG((0,0,"UartMainHdl:read left is %d\n",nleft));
			}
		}
	

	}
	close(fd_com); 
	fd_com = 0;
	return (void *)0;
	//数据返回

}


//写串口数据
int UartWriteData(St_MsgReq * pMsgReq)
{
	St_EcdrEncoder *pstEncoder = NULL;
	int idx = 0;
	int iWriteNum = 0;
	char msgtype;
	char op;
	char unit_type;
		
	if (fd_com == 0)
	{
		return -1;
	}
	/*
	   写串口数据格式
	    msgtype  1 byte 
	    op           1 byte
	    op_data   4 bytes
	    
	    sokect_id  4 bytes
	    devid       4 bytes
	    unit_type 1 byte
	    unit_num 1 byte  
	    total 1+1+4+4+4+1+1 = 16 bytes

	    
	*/
	msgtype = pMsgReq->msg_type;
	op = pMsgReq->op;
	unit_type = pMsgReq->unit_type;
	
	pstEncoder = Ecdr_CreateEncoder(WRITE_COM_DATA_LEN);
	if (!pstEncoder)
	{
		WIFUNC_LOG_MSG((0,0,"UartWriteData:Ecdr_CreateEncoder ERR\n"));
		return -1;
	} 
	//start bit
	Ecdr_EncodeOneByte(pstEncoder,&startbit);
	

	
	if ( -1 == Ecdr_EncodeOneByte( pstEncoder, (char*)&msgtype))
	{
	        Ecdr_DestroyEncoder( pstEncoder );
	        return -1;
	}
	if ( -1 == Ecdr_EncodeOneByte( pstEncoder, (char*)&op))
	{
	        Ecdr_DestroyEncoder( pstEncoder );
	        return -1;
	}
	if ( -1 == Ecdr_Encode4Bytes( pstEncoder, (int*)&(pMsgReq->op_data)))
	{
	        Ecdr_DestroyEncoder( pstEncoder );
	        return -1;
	}
	
	if ( -1 == Ecdr_Encode4Bytes( pstEncoder, (int*)&(pMsgReq->socket_id)))
	{
	        Ecdr_DestroyEncoder( pstEncoder );
	        return -1;
	}

	if ( -1 == Ecdr_Encode4Bytes( pstEncoder, (int*)&(pMsgReq->devid)))
	{
	        Ecdr_DestroyEncoder( pstEncoder );
	        return -1;
	}	
	
	if ( -1 == Ecdr_EncodeOneByte( pstEncoder, (char*)&unit_type))
	{
	        Ecdr_DestroyEncoder( pstEncoder );
	        return -1;
	}
	if ( -1 == Ecdr_EncodeOneByte( pstEncoder, (char*)&(pMsgReq->unit_num)))
	{
	        Ecdr_DestroyEncoder( pstEncoder );
	        return -1;
	}
	//startbit
	Ecdr_EncodeOneByte(pstEncoder,&endbit);
	//WIFUNC_LOG_MSG((0,0,"UartWriteData:Encode data is %s,len is %d\n",pstEncoder->pucData,pstEncoder->iPosition));
	

	if(WRITE_COM_DATA_LEN != pstEncoder->iPosition)
	{
		Ecdr_DestroyEncoder( pstEncoder );
		return -1;
	}
	else
	{
		for (idx = 0;idx < WRITE_COM_DATA_LEN;idx++)
		{
			WIFUNC_LOG_MSG((0,0,"UartMainHdl:pstEncoder->pucData[%d] =%u \n",idx,pstEncoder->pucData[idx]));
			//iWriteNum = write(fd_com,&pstEncoder->pucData[idx],1);
		}

		//清除数据
		//tcflush(fd_com,TCOFLUSH);
		WIFUNC_LOG_MSG((0,0,"UartWriteData:start write\n"));

		iWriteNum = write(fd_com,pstEncoder->pucData,WRITE_COM_DATA_LEN);
		if(iWriteNum != WRITE_COM_DATA_LEN)
		{
			WIFUNC_LOG_MSG((0,0,"UartWriteData:write com data length is %d not %d\n",iWriteNum,WRITE_COM_DATA_LEN));
			Ecdr_DestroyEncoder( pstEncoder );
			return -1;
		}
		WIFUNC_LOG_MSG((0,0,"UartWriteData:end write\n"));
	}
	
	Ecdr_DestroyEncoder( pstEncoder );
	return 0;
}


