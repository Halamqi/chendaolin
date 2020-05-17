#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/wait.h>
#include <signal.h>

#define BUFFER_SIZE 1024	
char buffer[BUFFER_SIZE];

//服务器主要的业务逻辑，由子进程来执行
void echo_server(int conn_fd,int port,char* ip){
	while(1){
		int ret=read(conn_fd,buffer,BUFFER_SIZE);
		if(ret==0){
			printf("reach end of file ,connection closed by peer ip:%s port:%d\n",ip,port);
			return;
		}

		printf("client ip:%s port:%d\n",ip,port);
		write(STDOUT_FILENO,buffer,ret);
		for(int i=0;i<ret;i++){
			buffer[i]=toupper(buffer[i]);
		}

		write(STDOUT_FILENO,buffer,ret);
		write(conn_fd,buffer,ret);
	}
}

//当客户端断开连接以后，负责与它进行通信的子进程结束，
//设置一个信号处理函数来回收退出的子进程。接受SIGCHLD信号
void sig_handler(int sig){
	pid_t pid=waitpid(-1,NULL,WNOHANG);
	if(pid==-1){
		perror("waitpid failed\n");
		exit(1);
	}
	if(pid>0)
		printf("wait %d child process\n",pid);
}

int main(int argc,char* argv[])
{
	if(argc<2){
		printf("usage: %s port\n",argv[0]);
		exit(1);
	}


	bzero(buffer,BUFFER_SIZE);

	int listen_fd;
	listen_fd=socket(AF_INET,SOCK_STREAM,0);
	if(listen_fd==-1)
	{
		perror("socket failed\n");
		exit(1);
	}
	//const char* ip=argv[1];
	int port=atoi(argv[1]);

	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port);
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	//inet_pton(AF_INET,ip,&server_addr.sin_addr);

	int ret;
	ret=bind(listen_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(ret==-1){
		perror("bind failed\n");
		exit(1);
	}

	ret=listen(listen_fd,5);
	if(ret==-1){
		perror("listen failed\n");
		exit(1);
	}

	struct sockaddr_in client_addr;
	socklen_t client_addr_len=sizeof(client_addr);	
	while(1){
		int conn_fd=accept(listen_fd,(struct sockaddr*)&client_addr,&client_addr_len);
		if(conn_fd<0){
			perror("accept failed\n");
			exit(1);
		}

		char client_ip[INET_ADDRSTRLEN];
		int client_port;
		client_port=ntohs(client_addr.sin_port);
		inet_ntop(AF_INET,&client_addr.sin_addr,client_ip,INET_ADDRSTRLEN);
		printf("client ip=%s,port=%d\n",client_ip,client_port);

		pid_t pid=fork();
		if(pid==-1){
			perror("fork error\n");
			exit(1);
		}
		else if(pid==0){
			close(listen_fd);//子进程负责与客户端通信，不需要监听可能到来的客户连接
			echo_server(conn_fd,client_port,client_ip);
			exit(1);
		}
		else if(pid>0){
			signal(SIGCHLD,sig_handler);
			close(conn_fd);//父进程只需要监听可能到来的客户端连接，不需要与客户端进行通信
			//			waitpid(-1,NULL,WNOHANG);
		}
	}
	close(listen_fd);
	return 0;
}
