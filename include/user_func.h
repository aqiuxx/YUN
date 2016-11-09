#ifndef USER_FUNC
#define USER_FUNC

// 验证用户信息
void verify_user(char user_psswd_buf[2][20], int sockfd,int id, int index);

//分解用户命令
void split_user_cmd(char myarg[10][20], char buf[100], int *i);

// create socket 创建套接字
int socket_create(int argc, char  *argv[]);

//用户登录注册
void login(int argc, char  *argv[], char user_psswd_buf[2][20]);


// 用户上传
void upload(char user_psswd_buf[2][20], char myarg[10][20],int num);


// 用户下载数据
void download(char user_psswd_buf[2][20], char myarg[10][20],int num);


// 用户下载数据
void choose(char user_psswd_buf[2][20],char cmdline[10][20], int num);

// 用户输入命令处理
void dealcommand(char user_psswd_buf[2][20]);

#endif
