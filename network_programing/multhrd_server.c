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

//存储客户端信息的结构体
struct client_info{	
	int conn_fd;
	char client_ip[INET_ADDRSTRLEN];
	int client_port;
};

//服务器主要的业务逻辑，由子线程来执行
void* echo_server(void* arg){
	//将该线程处理的连接的客户端信息取出来
	struct client_info* temp=(struct client_info*)arg;
	int conn_fd=temp->conn_fd;
	char ip[INET_ADDRSTRLEN];
	strcpy(ip,temp->client_ip);
	int port=temp->client_port;
	//free掉堆上分配的内存
	free(temp);
	
	while(1){
		int ret=read(conn_fd,buffer,BUFFER_SIZE);
		if(ret==0){
			printf("reach end of file ,connection closed by peer ip:%s port:%d\n",ip,port);
			close(conn_fd);
			return NULL;
		}
		
		printf("client ip:%s port:%d\n",ip,port);
		write(STDOUT_FILENO,buffer,ret);
		for(int i=0;i<ret;i++){
			buffer[i]=toupper(buffer[i]);
		}
		
		write(STDOUT_FILENO,buffer,ret);
		write(conn_fd,buffer,ret);
	}
	return NULL;
}

/*void* join_thread(void* arg){
	pthread_t join_id=*((pthread_t*)arg);
	free(arg);

	
}*/

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
	//设置监听套接字，重用端口
	int opt=1;
	setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,(void*)&opt,sizeof(opt));
//	const char* ip=argv[1];
	int port=atoi(argv[1]);

	//服务器的地址
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(port);
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
//	inet_pton(AF_INET,ip,&server_addr.sin_addr);

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
	
	//创建回收线程
	pthread_t thread_id;
	struct sockaddr_in client_addr;	
	bzero(&client_addr,sizeof(client_addr));
	socklen_t client_addr_len=sizeof(client_addr);

	while(1){
		int conn_fd=accept(listen_fd,(struct sockaddr*)&client_addr,&client_addr_len);
		if(conn_fd<0){
			perror("accept failed\n");
			exit(1);
		}

		//pthread_create(join_id,NULL,join_thread,(void*)join_id);
		//记录新建立连接的客户端信息
		char client_ip[INET_ADDRSTRLEN];
		int client_port;
		client_port=ntohs(client_addr.sin_port);
		inet_ntop(AF_INET,&client_addr.sin_addr,client_ip,INET_ADDRSTRLEN);
		printf("client ip=%s,port=%d\n",client_ip,client_port);
		
		//将客户端信息存储在一个结构体中，线程入口函数的参数，传递给线程
		struct client_info* info_ptr=malloc(sizeof(struct client_info));
		info_ptr->conn_fd=conn_fd;
		strcpy(info_ptr->client_ip,client_ip);
		info_ptr->client_port=client_port;
		pthread_create(&thread_id,NULL,echo_server,(void*)info_ptr);
		pthread_detach(thread_id);	
	}
	close(listen_fd);
	return 0;
}
