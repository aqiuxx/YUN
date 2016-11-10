#include <stdio.h>

#include "pthreadpool.h"
#include "user_func.h"

int main(int argc, char  *argv[])
{

	int num;
	char *myarg[10];
	char buf[100] ={0};
	char aaaa[10][20];
	for(num =0;num < 10;num++)
		myarg[num] = aaaa[num];
	

	char user_psswd_buf[2][20]  ={0};

	//login 登陆函数
	login(argc, argv, user_psswd_buf);

	//创建线程 create a tpool
//	THDPL *mypool = creat_pool(10);


	while(1)
	{
		//用户输入 命令
		//upload  download ls
		printf("myyun> ");fflush(stdout);
		gets(buf);

		//分解用户命令
		split_user_cmd(myarg, buf, &num);

		printf("split:,%s,%s\n",myarg[0],myarg[1]);
		printf("name:%s,pass:%s\n",user_psswd_buf[0],user_psswd_buf[1]);

		//choose(user_psswd_buf,myarg, num, mypool);
		choose(user_psswd_buf,myarg, num);

		//add task, 有线程池

		//pool_add_task(mypool, serv_routine, (void *)sockfd);
	}
	//destroy_pool(mypool);

}




