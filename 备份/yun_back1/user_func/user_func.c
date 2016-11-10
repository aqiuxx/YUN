#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "user_func.h"
#include "ddebug.h"
#include "pthreadpool.h"

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
void verify_user(char user_psswd_buf[2][20], int sockfd, int id,int index)
{
	while(1)
	{
		//打包验证数据
		char buf[100] = {0x01,  0x00,0x29,  0x01};
		buf[3] = index;
		buf[0] = id;
		int ret;
		if(id == 7)
		{
			strncpy(buf+3,user_psswd_buf[0],20);
			strncpy(buf+23,user_psswd_buf[1],20);
			ret = send(sockfd, buf, 43, 0);
		}
		else
		{
			strncpy(buf+4,user_psswd_buf[0],20);
			strncpy(buf+24,user_psswd_buf[1],20);
			ret = send(sockfd, buf, 44, 0);
		}
//		printf("name:%s\n",buf+4);
//		printf("name:%s\n",buf+24);

		ret = recv(sockfd, buf, 4, MSG_WAITALL);

		if ( (ret = ack(buf[3])) && (4 == buf[0]) )
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
void split_user_cmd(char *myarg[], char buf[100],int *i)
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
			printf("get:%s\n",myarg[(*i)+1]);
			myarg[*i-1][rear-head] = 0;
			head = rear;
		}
}



int socket_create(char *myarg[])
{
	// create socket 创建套接字
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(-1 == sockfd)
		syserr("sockfd");
	if(myarg == NULL)
		goto next;
	static	struct sockaddr_in serveraddr = {0};
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(myarg[2]));
	serveraddr.sin_addr.s_addr = inet_addr(myarg[1]);//IPv4
	static int len = sizeof serveraddr;
next:
	if(-1 == connect(sockfd, (struct sockaddr*)&serveraddr, len))
		syserr("connect");

	return sockfd;
}

//用户登录注册
void login(int argc, char *myarg[], char user_psswd_buf[2][20])
{

	//char buf[100] ={0};
	if(3 != argc)
	{
		printf("Usage: %s <IP> <PORT> \n", myarg[0]);
	}

	int sockfd = socket_create(myarg);

		while(1)
		{
			printf("1 for login or 2 for register \n");fflush(stdout);
			char c = getchar();

			while(getchar()!='\n');
			if ( 0 == strcmp(&c,"1"))
			{
				printf("name> ");fflush(stdout);
				gets(user_psswd_buf[0]);

				printf("password> ");fflush(stdout);
				gets(user_psswd_buf[1]);
				verify_user(user_psswd_buf, sockfd,1, 1);

				break;
			}
			else if ( 0 == strcmp(&c,"2"))
				{
					printf("name> ");fflush(stdout);
					gets(user_psswd_buf[0]);

					printf("password> ");fflush(stdout);
					gets(user_psswd_buf[1]);
					verify_user(user_psswd_buf, sockfd,1, 3);

					break;
				}
			else
				{
					printf("argument is not matching!\n ");
					continue;
				}

		}



	close(sockfd);

}



// 用户上传
void upload(char user_psswd_buf[2][20], char *myarg_up[], int num_up)
{


	int sockfd = socket_create(NULL);

	verify_user(user_psswd_buf, sockfd,7, 4);

	printf("in the upload\n");
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

	printf("file:%s\n",myarg_up[1]);
	if (stat(myarg_up[1], &sb) == -1)
	{
	   syserr("stat");
	   exit(EXIT_FAILURE);
	}
	int filesize = (int)(sb.st_size);

	printf("in the upload two\n");


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

	printf("in the upload three\n");


	int ret = send(sockfd, buf, len + 3, 0);//check ret ?


	//////读文件写数据
	int rdfd = open(myarg_up[1], O_RDONLY);
	int send_data_num = 0; /******/

	while(filesize)
	{
		char file_buf[MAX_LEN] = {0x06};

		ret = read(rdfd, file_buf+3, MAX_LEN - 3);
		printf("send:%d\n",ret);
		file_buf[1] = ret / 256;
		file_buf[2] = ret % 256;

		printf("remand:%d\n",filesize);
		ret = send(sockfd, file_buf, ret+3, 0);

		filesize -= ret;
		printf("send: %d%%\n", (int)(send_data_num/filesize) );
		send_data_num += ret;
	}
	//////////////////////////////////////


	ret = recv(sockfd, buf, 4, MSG_WAITALL);

	ret = ack(buf[3]);//看是否成功



	close(sockfd);
	close(rdfd);
}



// 用户下载
void download(char user_psswd_buf[2][20], char *myarg[], int num)
{


	int sockfd = socket_create(NULL);

	verify_user(user_psswd_buf, sockfd,7, 4);

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


	if (107 != ret || 3 != buf[0]) //开始下载
	{
		syserr("wrong num");
	}

	int send_data_num = 0; /******/
	while(filesize)
	{
		char databuf[MAX_LEN] = {0};
		ret = recv(sockfd, databuf, 3, MSG_WAITALL);//接受3头字节
		if (3 != ret || 6 != databuf[0])
		{
			syserr("wrong num");
		}

		int datalen = databuf[1] * 256 + databuf[2];
		ret = recv(sockfd, databuf, datalen, MSG_WAITALL);
		write(wrfd, databuf, ret);

		filesize -= ret;

		printf("send: %d%%\n", (int)(send_data_num/filesize) );
		send_data_num += ret;
	}

	close(sockfd);
	close(wrfd);
}

void dealcommand(char user_psswd_buf[2][20],char *cmdline[])
{
	int sockfd = socket_create(NULL);

	verify_user(user_psswd_buf, sockfd,7, 4);

	printf("in the cmdline\n");

	//命令请求
	int len = strlen(cmdline[1]) + 1;
	char buf[100] = {0x05};
	buf[1] = len / 256;
	buf[2] = len % 256;
	strncpy(buf+3, cmdline[1], len);
	int ret = send(sockfd, buf, len, 0);

	char databuf[MAX_LEN] = {0};
	ret = recv(sockfd, databuf, 3, MSG_WAITALL);//接受3头字节
	if (3 != ret || 5 != databuf[0])//也是返回 5 ？
	{
		syserr("cmdline - wrong num ");
	}


	if (!strcmp(cmdline[0], "ls"))
	{

		//此处需要知道客户端那边协议是传几个字节的长度信息filesize
		int filesize = databuf[1] * 256 + databuf[2];
		while(filesize)
		{
			//char databuf[MAX_LEN] = {0};
			ret = recv(sockfd, databuf, 3, MSG_WAITALL);//接受3头字节
			if (3 != ret || 6 != databuf[0])
			{
				syserr("wrong num");
			}

			int datalen = databuf[1] * 256 + databuf[2];
			ret = recv(sockfd, databuf, datalen, MSG_WAITALL);
			//split_user_cmd(databuf)  //需要吗?????????????
			//write(wrfd, databuf, ret);

			filesize -= ret;

			printf("%s\n", databuf);  //  此处可能需循环显示 ？？？？？
		}
		//传完之后再穿一次，表明一下状态
		ret = recv(sockfd, databuf, 4, MSG_WAITALL);//接受3头字节
		ret = ack(buf[3]);

	}
	else if (!strcmp(cmdline[0], "cd"))
	{
		int ret = recv(sockfd, databuf, 4, MSG_WAITALL);//接受3头字节
		ret = ack(buf[3]);
	}
	else//( !strcmp(cmdline[0], "mkdir") )
	{
		int ret = recv(sockfd, databuf, 4, MSG_WAITALL);//接受3头字节
		ret = ack(buf[3]);
	}

}


//void choose(char user_psswd_buf[2][20],char *cmdline[], int num, THDPL *mypool)
void choose(char user_psswd_buf[2][20],char *cmdline[], int num)
{
	if (!strcmp(cmdline[0],"download"))
	{
/*		if (num > 3)
		{
			int sockfd = socket_create(NULL);
			num = num - 2;

			while (num --)//线判断还是？？？？
			{
				pool_add_task(mypool, download, (void *)sockfd);
			}
		}*/

		download(user_psswd_buf,  cmdline,  num);
	}
	else if(!strcmp(cmdline[0],"upload"))
	{
/*		if (num > 3)
		{
			int sockfd = socket_create(NULL);
			num = num - 2;

			while (num --)//线判断还是？？？？
			{
				pool_add_task(mypool, upload, (void *)sockfd);
			}
		}*/

		upload(user_psswd_buf,  cmdline,  num);
	}
	else if(!strcmp(cmdline[0],"ls") | !strcmp(cmdline[0],"cd") | !strcmp(cmdline[0],"mkdir"))
	{
		dealcommand(user_psswd_buf, cmdline);
	}
	else
	{
		printf("error cmdline\n");
	}

}

//if(strcmp(myarg_up[0],"upload"));




















