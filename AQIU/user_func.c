#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


#include "ddebug.h"

#define MAX_LEN 1024

static int ack(int flag)
{
	int ret;
	switch(flag)
	{
		case	1:
					printf("login sucess\n");
					ret = 1;
					break;
		case	2:
					printf("user is not exist\n");
					break;
		case	3:
					printf("password is not correct\n");
					break;
		case	4:
					printf("register sucess\n");
					ret = 1;
					break;
		case	5:
					printf("register failed\n");
					break;
		case	6:
					printf("file exist\n");
					ret = 1;
					break;
		case	7:
					printf("file is not exist\n");
					break;
		case	8:
					printf("file is delivered\n");
					ret = 1;
					break;
		case	9:
					printf("verify sucess\n");
					ret = 1;
					break;
		case	10:
					printf("verify failed\n");
					break;
		default:
					break;
	}
	return ret;
}



/**
 * [verify_user description]
 * @param user_psswd_buf [description]
 * @param sockfd         [description]
 * @param index          1 login, 4 ack
 */
void verify_user(char user_psswd_buf[2][20], int sockfd, int index)
{
	while(1)
	{
		//打包验证数据
		char buf[100] = {0x01,  0x00,0x29,  0x01};
		strncpy(buf+4,user_psswd_buf[0],20);
		strncpy(buf+24,user_psswd_buf[1],20);

		int ret = send(sockfd, buf, 44, 0);
		ret = recv(sockfd, buf, 4, MSG_WAITALL);

		if ( (ret = ack(buf[3])) && (index == buf[0]) )
		{
			break;
		}
	}

}

/**
 * [split_user_cmd description]
 * @param myarg [description]
 * @param buf   [description]
 * @param i     [description]
 */
void split_user_cmd(char myarg[10][20], char buf[100],int *i)
{
	char *rear,*head;
	*i = 0;
	rear = head =buf;
		while(*rear != '\0')
		{
			while(*head == ' ' && *head != '\0')
			{
				head++;
			}
			if(*head == '\0')
				break;
			rear = head;
			while(*rear != ' ' && *rear != '\0')
			{
				rear++;
			}
			strncpy(myarg[(*i)++],head,rear-head);
			myarg[*i-1][rear-head] = 0;
			head = rear;
		}
}



int socket_create(char myarg[10][20])
{
	// create socket 创建套接字
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sockfd)
		syserr("sockfd");

	struct sockaddr_in serveraddr = {0};
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(myarg[2]));
	serveraddr.sin_addr.s_addr = inet_addr(myarg[1]);//IPv4
	int len = sizeof serveraddr;

	if(-1 == connect(sockfd, (struct sockaddr*)&serveraddr, len))
		syserr("connect");

	return sockfd;
}

//用户登录注册
void login(int argc, char  myarg[10][20], char user_psswd_buf[2][20])
{

	//char buf[100] ={0};
	if(3 != argc)
	{
		printf("Usage: %s <IP> <PORT> \n", myarg[0]);
	}

	int sockfd = socket_create(myarg);

	while(1)
	{
		while(1)
		{
			printf("1 for login or 2 for register \n");fflush(stdout);
			char c = getchar();


			if ( 0 == strcmp(&c,"1"))
			{
				printf("name> ");fflush(stdout);
				gets(user_psswd_buf[0]);

				printf("password> ");fflush(stdout);
				gets(user_psswd_buf[1]);

				break;
			}
			else if ( 0 == strcmp(&c,"2"))
				{
					printf("name> ");fflush(stdout);
					gets(user_psswd_buf[0]);

					printf("password> ");fflush(stdout);
					gets(user_psswd_buf[1]);

					break;
				}
			else
				{
					printf("argument is not matching!\n ");
					continue;
				}

		}
		verify_user(user_psswd_buf, sockfd, 1);

	}


	close(sockfd);

}



// 用户上传
void upload(char user_psswd_buf[2][20], char myarg_up[10][20], int num_up)
{


	int sockfd = socket_create(myarg_up);

	verify_user(user_psswd_buf, sockfd, 4);


/*	//验证完后，开始上传文件
	int num_up;
	char myarg_up[10][20];
	char buf_up[100] ={0};

	//用户输入 命令
	printf("myyun: upload_file> ");fflush(stdout);
	gets(buf_up);

	//分解用户命令
	split_user_cmd(myarg_up, buf_up, &num_up);
*/

	//////////////////////////////////////
	struct stat sb;

	if (stat(myarg_up[1], &sb) == -1)
	{
	   syserr("stat");
	   exit(EXIT_FAILURE);
	}

	int filesize = (int)(sb.st_size);

	int src_filename_len = 50;
	int dest_filename_len = 50;
	//int dest_filename_len = strlen(myarg_up[2]);
	//int len = 4 + src_filename_len + dest_filename_len;
	int len = 104;

	char buf[107] = {0x02};
	//buf[1] = len / 256;
	//buf[2] = len % 256;

	buf[3] = filesize>>24;
	buf[4] = filesize>>16;
	buf[5] = filesize>>8;
	buf[6] = filesize>>0;
	//strncpy(buf+3, filesize ,4);//文件大小
	strncpy(buf+3+4,myarg_up[1],src_filename_len);//原文件名字
	strncpy(buf+3+4+src_filename_len,myarg_up[2],dest_filename_len);//目标路径

	int ret = send(sockfd, buf, len + 3, 0);

	//////读文件写数据
	int rdfd = open(myarg_up[1], O_RDONLY);

	while(filesize)
	{
		char file_buf[MAX_LEN] = {0x06};

		ret = read(rdfd, file_buf+3, MAX_LEN - 3);

		file_buf[1] = ret / 256;
		file_buf[2] = ret % 256;

		filesize -= ret;
		ret = send(sockfd, file_buf, ret+3, 0);
	}
	//////////////////////////////////////


	ret = recv(sockfd, buf, 4, MSG_WAITALL);

	ret = ack(buf[3]);//看是否成功



	close(sockfd);
	close(rdfd);
}



// 用户下载
void download(char user_psswd_buf[2][20], char myarg[10][20], int num)
{


	int sockfd = socket_create(myarg);

	verify_user(user_psswd_buf, sockfd, 4);

	//请求并等待服务器响应，然后下载数据

	//请求下载数据
	char buf[107] = {0x03,  0x00,0x32};
	strncpy(buf+7,myarg[1],50);
	int ret = send(sockfd, buf, 107, 0);


	int wrfd = open(myarg[1], O_RDWR | O_CREAT | O_TRUNC);
	ret = recv(sockfd, buf, 107, MSG_WAITALL);
	//int msglen = buf[1] * 256 + buf[2];
	//int dataname_len = msglen -4;
	int filesize = (buf[3]<<24) + (buf[4]<<16) + (buf[5]<<8) +(buf[6]<<0);


	if (107 == ret && 3 == buf[0]) //开始下载
	{
		while(filesize)
		{
			ret = recv(sockfd, buf, 3, MSG_WAITALL);//接受3头字节
			if (3 != ret || 6 != buf[0])
			{
				syserr("wrong num");
			}
			char databuf[MAX_LEN] = {0};
			int datalen = buf[1] * 256 + buf[2];
			ret = recv(sockfd, buf, datalen, MSG_WAITALL);
			write(wrfd, databuf, ret);

			filesize -= ret;
		}
	}

	close(sockfd);
	close(wrfd);
}

void select(char user_psswd_buf[2][20],char cmdline[10][20], int num)
{
	if (!strcmp(cmdline[0],"download"))
	{
		download(user_psswd_buf,cmdline,num);
	}
	else if(!strcmp(cmdline[0],"upload"))
	{
		upload(user_psswd_buf,cmdline,num);
	}
	else
	{
		printf("error\n");
	}

}

//if(strcmp(myarg_up[0],"upload"));




















