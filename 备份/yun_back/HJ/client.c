#include <stdio.h>

#include "pthreadpool.h"
#include "user_func.h"




// 用户上传
void *upload()
{

}
// 用户下载数据
void *download()
{

}

// 用户输入命令处理
void *dealcommand()
{

}




int main(int argc, char  *argv[])
{

	int i,num;
	char myarg[10][20];
	char buf[100] ={0};

	char user_psswd_buf[2][20]  ={0};

	//login 登陆函数
	int sockfd = login(argc, argv, user_psswd_buf);

	//创建线程 create a tpool
	THDPL *mypool = creat_pool(10);


	while(1)
	{
		//用户输入 命令
		printf("myyun> ");fflush(stdout);
		gets(buf);

		//分解用户命令
		split_user_cmd(myarg, buf, &num);

		//add task, 有线程池
		tpool_add_task(mypool, serv_routine, (void *)sockfd);
	}
	close(sockfd);

}









	return 0;
}

	while(1)
	{
		printf("mydatabase> ");fflush(stdout);
		gets(buf);
		int ret = write(sockfd, buf, sizeof buf);
		ret = read(sockfd, buf, sizeof buf);
		printf("%s\n", buf);
	}
	close(sockfd);
}

