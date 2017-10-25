/*
** 功能:手机控制LEDs服务器
** 作者:F403科技创意室(无涯物联科技)
** 联系方式:www.f403tech.com
*/
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <curses.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/* socket
 * bind
 * listen
 * accept
 * send/recv
 */
int fd;

#define SERVER_PORT 8002
#define BACKLOG     10

#define MYLEDS_LED1_ON 	0
#define MYLEDS_LED1_OFF 	1
#define MYLEDS_LED2_ON 	2
#define MYLEDS_LED2_OFF 	3

int iSocketServer;
int iSocketClient;
struct sockaddr_in tSocketServerAddr;
struct sockaddr_in tSocketClientAddr;
int iRet;
int iAddrLen;

int iRecvLen, iSendLen;
char ucRecvBuf[1000];

int iClientNum = -1;

pthread_t Client_ID;
char status[15] = "0,0,0,0,0,0";

void* ClientThread(void *data)
{
	while(1)
	{
		/* 接收客户端发来的数据并显示出来 */
		iRecvLen = recv(iSocketClient, ucRecvBuf, 999, 0);
		if (iRecvLen <= 0)
		{
			close(iSocketClient);
			return data;
		}
		else
		{
			ucRecvBuf[iRecvLen] = '\0';
			if(strstr(ucRecvBuf,"status") != NULL)
			{
				// 在这里返回所有的开关状态
				iSendLen = send(iSocketClient, status, strlen(status), 0);
				if (iSendLen <= 0)
				{
					close(iSocketClient);
					return data;
				}
			}
			else if(strstr(ucRecvBuf,"set:0=0") != NULL)
			{
				status[0] = '0';
				// 熄灭LED1
				ioctl(fd, MYLEDS_LED1_OFF);
			}
			else if(strstr(ucRecvBuf,"set:0=1") != NULL)
			{
				status[0] = '1';
				// 点亮LED1
				ioctl(fd, MYLEDS_LED1_ON);
			}
			else if(strstr(ucRecvBuf,"set:1=0") != NULL)
			{
				status[2] = '0';
				// 熄灭LED2
				ioctl(fd, MYLEDS_LED2_OFF);
			}
			else if(strstr(ucRecvBuf,"set:1=1") != NULL)
			{
				status[2] = '1';
				// 点亮LED2
				ioctl(fd, MYLEDS_LED2_ON);
			}
			else if(strstr(ucRecvBuf,"set:2=0") != NULL)
			{
				status[4] = '0';
			}
			else if(strstr(ucRecvBuf,"set:2=1") != NULL)
			{
				status[4] = '1';
			}
			else if(strstr(ucRecvBuf,"set:3=0") != NULL)
			{
				status[6] = '0';
			}
			else if(strstr(ucRecvBuf,"set:3=1") != NULL)
			{
				status[6] = '1';
			}
			else if(strstr(ucRecvBuf,"set:4=0") != NULL)
			{
				status[8] = '0';
			}
			else if(strstr(ucRecvBuf,"set:4=1") != NULL)
			{
				status[8] = '1';
			}
			else if(strstr(ucRecvBuf,"set:5=0") != NULL)
			{
				status[10] = '0';
			}
			else if(strstr(ucRecvBuf,"set:5=1") != NULL)
			{
				status[10] = '1';
			}
		}
	}

	return data;
}

int main(int argc, char **argv)
{
	/* 1.打开设备节点 */
	fd = open("/dev/myleds", O_RDWR | O_NONBLOCK);
	if (fd < 0)
	{
		printf("can't open!\n");
		return -1;
	}

	signal(SIGCHLD,SIG_IGN);
	
	iSocketServer = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == iSocketServer)
	{
		printf("socket error!\n");
		return -1;
	}

	tSocketServerAddr.sin_family      = AF_INET;
	tSocketServerAddr.sin_port        = htons(SERVER_PORT);  /* host to net, short */
 	tSocketServerAddr.sin_addr.s_addr = INADDR_ANY;
	memset(tSocketServerAddr.sin_zero, 0, 8);
	
	iRet = bind(iSocketServer, (const struct sockaddr *)&tSocketServerAddr, sizeof(struct sockaddr));
	if (-1 == iRet)
	{
		printf("bind error!\n");
		return -1;
	}

	iRet = listen(iSocketServer, BACKLOG);
	if (-1 == iRet)
	{
		printf("listen error!\n");
		return -1;
	}

	while (1)
	{
		iAddrLen = sizeof(struct sockaddr);
		iSocketClient = accept(iSocketServer, (struct sockaddr *)&tSocketClientAddr, &iAddrLen);
		if (-1 != iSocketClient)
		{
			iClientNum++;
			printf("Get connect from client %d : %s\n",  iClientNum, inet_ntoa(tSocketClientAddr.sin_addr));

			pthread_create(&Client_ID, NULL, &ClientThread, NULL);
		}
	}
	
	close(iSocketServer);
	return 0;
}

