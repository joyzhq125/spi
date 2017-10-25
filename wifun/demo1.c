
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <error.h>


#define FALSE 0
#define TRUE 1



/**

*@brief ���ô���ͨ������

*@param fd ���� int �򿪴��ڵ��ļ����

*@param speed ���� int �����ٶ�

*@return void

*/

int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,

B38400, B19200, B9600, B4800, B2400, B1200, B300, };

int name_arr[] = {38400, 19200, 9600, 4800, 2400, 1200, 300, 38400, 

19200, 9600, 4800, 2400, 1200, 300, };


/*���ò�����*/
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

				perror("tcsetattr fd"); 	
				return; 

			} 

			tcflush(fd,TCIOFLUSH); 

		} 

	}

}



/**
	����У�麯��

	*@brief ���ô�������λ��ֹͣλ��Ч��λ

	*@param fd ���� int �򿪵Ĵ����ļ����

	*@param databits ���� int ����λ ȡֵ Ϊ 7 ����8

	*@param stopbits ���� int ֹͣλ ȡֵΪ 1 ����2

	*@param parity ���� int Ч������ ȡֵΪN,E,O,,S

*/

int set_Parity(int fd,int databits,int stopbits,int parity)

{ 
	struct termios options; 
	if ( tcgetattr( fd,&options) != 0) 
	{ 
		perror("SetupSerial 1"); 
		return(FALSE); 

	}
	//bzero(&options, sizeof(options));
	//
	options.c_cflag |= CLOCAL|CREAD;//|HUPCL;
	options.c_cflag &= ~CSIZE; 
	switch (databits) /*��������λ��*/
	{ 
		case 7: 
			options.c_cflag |= CS7; 
			break;
		case 8: 
			options.c_cflag |= CS8;
			break; 
		default: 
			fprintf(stderr,"Unsupported data sizen"); 
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
			options.c_cflag |= (PARODD | PARENB); /* ����Ϊ��Ч��*/ 
			options.c_iflag |= INPCK; /* Disnable parity checking */ 
			break; 
		case 'e': 
		case 'E': 
			options.c_cflag |= PARENB; /* Enable parity */ 
			options.c_cflag &= ~PARODD; /* ת��ΪżЧ��*/ 
			options.c_iflag |= INPCK; /* Disnable parity checking */
			break;
		case 'S': 
		case 's': /*as no parity*/ 
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break; 
		default: 
			fprintf(stderr,"Unsupported parityn"); 
			return (FALSE); 

	} 

	//����ֹͣλ
	switch (stopbits)
	{ 
		case 1: 
			options.c_cflag &= ~CSTOPB; 
			break; 
		case 2: 
			options.c_cflag |= CSTOPB; 
			break;
		default: 
			fprintf(stderr,"Unsupported stop bitsn"); 
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
	options.c_cc[VTIME] = 5; /* ���ó�ʱ*/ 
	options.c_cc[VMIN] = 1; 
	if (tcsetattr(fd,TCSANOW,&options) != 0) 
	{ 
		perror("SetupSerial 3"); 
		return (FALSE); 
	} 
	return (TRUE); 
}


int OpenDev(char *Dev)

{

	int fd = open( Dev, O_RDWR|O_NOCTTY| O_NDELAY|O_NONBLOCK); 
	if (-1 == fd) 
	{ 
		perror("Can't Open Serial Port");
		return -1; 
	} 

	else 
	{
		return fd;
	}

}



int main(int argc, char **argv)
{
	int fd;
	int nread;
	char read_buf[32] = {0};
	char write_buf[32] = {0};
	char *dev = "/dev/ttyS0"; 
	fd = OpenDev(dev);
	if (fd == -1)
	{
		exit(0);
	}
	set_speed(fd,9600);
	if (set_Parity(fd,8,1,'N') == FALSE) 
	{
		printf("Set Parity Errorn");
		exit (0);
	}
	while (TRUE) 
	{ 
		int idx = 0;
		bzero(read_buf,sizeof(read_buf));
		bzero(write_buf,sizeof(write_buf));	
	/*
		if (select(fd + 1, &ReadSetFD, NULL, NULL, NULL) < 0) 
		{
		     //Error(strerror(errno));
                     fprintf(stderr,"select err");
		     exit(0);
		}	
		if (FD_ISSET(fd, &ReadSetFD)) 
	*/	
		//tcflush(fd,TCIOFLUSH); 
 		if((nread = read(fd, read_buf, 1)) > 0)
		{
			for (idx = 0;idx < nread;idx++)
			{
				//fprintf(stderr,"len:%u ",nread);
				fprintf(stderr,"%u ",read_buf[idx]);
				if (idx == nread -1)
				{
				//    read_buf[nread]='\n';	
				}
			}
			    
		}
	
		//if(strcmp(read_buf,"closeserial!")==0)
		//break;
		/*
		for(i=0;i<result;i++)
		{
			write_buf[i]=read_buf[i];
		}
		len+=result;
		write_buf[result]='\0';
		if(strlen(write_buf)>0)
		{
			result=write(fd,write_buf,sizeof(write_buf));
			if(result<0)
			{
				perror("write");
				exit(0);
			}
		}
		*/
		//sleep(2);
	}
	close(fd); 
	return 0;
}





