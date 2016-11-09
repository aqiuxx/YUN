#include "databasectl.h"
#include "serroutine.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>


static int sendack(int socket, char type)
{
	char wbuf[4] = {0};
	wbuf[0] = 4;
	wbuf[1] = 0;
	wbuf[2] = 1;
	wbuf[3] = type;
	send(socket, wbuf, 4,0);
}

void routine(void *arg)
{
	unsigned char head[3]={0};
	char username[20]={0};
	char passwd[20]={0};
	char rbuf[1024] ={0};
	int filelen;
	int ret;

	int socket = (int)arg;
	//send from socket 3byte
	//
	ret = recv(socket,head,3,MSG_WAITALL);
	printf("%d,%d,%d,%d,,\n",ret,head[0],head[1],head[2]);
	
	int len = head[1];
	len = len<<8 + head[2];

	//switch cmmand
	switch(head[0])
	{
	//
	//case:verify()->download.upload.command?
		case 7:
			printf("ready to yanzheng\n");
			ret = recv(socket, rbuf, 40,MSG_WAITALL);
			strncpy(username, rbuf, 20);
			strncpy(passwd, rbuf+20, 20);
			//verify()
			if(0 == verify_user(username,passwd))
			{
				sendack(socket, VERFAIL);
				close(socket);
				return ;
			}
			//send ack;
			sendack(socket, VEROK);
//			send(socket, wbuf, 4);
			//get requirement
			recv(socket, rbuf, 3, MSG_WAITALL);


			//download ??
			if(3 == rbuf[0])
			{
				char wbuf[1024] ={0};
				char filename[50]={0};

				recv(socket, rbuf, 104, MSG_WAITALL);
				//send filename and size
				strncpy(filename,rbuf + 4,50);
				struct stat buf;
				stat(filename,&buf);
				
				filelen = buf.st_size;

				wbuf[0] = 2;
				wbuf[1] = 0;
				wbuf[2] = 107;
				wbuf[3] = filelen>>24;
				wbuf[4] = filelen>>16;
				wbuf[5] = filelen>>8;
				wbuf[6] = (char)filelen;
				strncpy(wbuf+7,filename,50);

				len = 107; //????	
				send(socket, wbuf, len,0);
				
				
				int filefd = open(filename,O_RDONLY);
				head[0] = 6;
				while(1)
				{
					ret = read(filefd, rbuf, sizeof(rbuf)-3);
					if(0 == ret)
						break;
					head[1] = ret / 256;
					head[2] = ret % 256;
					send(socket, head, 3,0);
					send(socket, rbuf, ret,0);
					filelen-=ret;
						if(0 >= filelen )
						break;
				}
				close(socket);
				close(filefd);
				return;

			}
			//upload
			else if(2 == rbuf[0])
			{
				char wbuf[1024] ={0};
				char filename[100]={0};
				int buflen;
				recv(socket, rbuf, 104,MSG_WAITALL);

				//get filename
				strncpy(filename, rbuf+4, 50);
				printf("read to write file %s\n",filename);
				if(0 != file_isexist(username, filename))
				{
					sendack(socket,FIEXIST);
					close(socket);
					return ;
				}

				filelen = (rbuf[0]<<24)+(rbuf[1]<<16)+(rbuf[2]<<8)+rbuf[3];

				//create file,if file exist,return 
				int filefd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666);
				printf("read to open file %s\n",filename);
				while(1)
				{
					ret = recv(socket, head, 3,MSG_WAITALL);
					printf("recv:%d\n",ret);
					if(0 == ret)
						break;
					buflen = head[1]*256+head[2];
					printf("redy to read %d,%d,%d\n",buflen,head[1],head[2]);
					ret = recv(socket, rbuf, buflen,MSG_WAITALL);
					printf("recv:%d\n",ret);
					
					write(filefd, rbuf, ret);
					filelen-=ret;
					if(0 >= filelen)
						break;
				}

				printf("upload ok\n");
				sendack(socket,VEROK);
				printf("send reply ok\n");
				close(socket);
				close(filefd);
				return ;

			}
			else if(5 == rbuf[0])
			{
				
			}
			break;


		
	//login?
	//
	//
		case 1:
			printf("ready to login\n");
			ret = recv(socket,rbuf,41,MSG_WAITALL);
			printf("%d,%d,%s,%s,\n",ret,rbuf[0],rbuf+1,rbuf+21);
		//	ret = recv(socket, rbuf, rbuf[2],MSG_WAITALL);
	//		printf("%d,,%s\n",ret,rbuf);
			if(1 == rbuf[0])
			{
				printf("ready to denglu\n");
				strncpy(username, rbuf+1, 20);
				strncpy(passwd, rbuf+21, 20);
				//verify
				if(0 == verify_user(username,passwd))
				{
					sendack(socket,PSWRO);
					return ;
				}
				sendack(socket,LOGSS);
				
				return ;
			}
			else if(3 == rbuf[0])
			{
				printf("ready to zhuce\n");
				strncpy(username, rbuf+1, 20);
				strncpy(passwd, rbuf+21, 20);
				//verify not exist
				if(1 == user_isexist(username))
				{
					sendack(socket,REFAIL );
					close(socket);
					return ;
				}
				//add the user
				//
				sendack(socket, REOK);
				close(socket);

				return;
				
			}
		}
}
