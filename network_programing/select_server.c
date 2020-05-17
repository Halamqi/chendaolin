#include <pthread.h>
#include "myInit.h"

#define BUF_SIZE 2048
int FD_LISTENED[BUF_SIZE];
int size=0;
int main(int argc,char* argv[])
{
	if(argc<2){
		printf("usage: %s port\n",argv[0]);
		exit(1);
	}
	
	char buffer[BUF_SIZE];
	bzero(buffer,sizeof(BUF_SIZE));

	int listen_fd=socket(AF_INET,SOCK_STREAM,0);
	if(listen_fd==-1){
		perror("socket failed: ");
		exit(1);
	}

	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port=htons(atoi(argv[1]));
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);

	int ret=bind(listen_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(ret==-1){
		perror("bind failed: ");
		exit(1);
	}

	listen(listen_fd,128);
	//设置当前所创建的最大的文件描述符，来查询文件描述符上的可写时间
	int maxfd=listen_fd;
	//all_set是用来记录所有需要监听读事件的文件描述符
	fd_set read_set,all_set;
	FD_ZERO(&read_set);
	FD_ZERO(&all_set);
	FD_SET(listen_fd,&all_set);
	FD_LISTENED[size++]=listen_fd;
	
	while(1){
		//将所有需要被监听的文件描述符集合给read_set传递给select
		read_set=all_set;
		ret=select(maxfd+1,&read_set,NULL,NULL,NULL);
		if(ret==-1){
			perror("select failed: ");
			exit(1);
		}
		//如果监听socket文件描述符上有可读事件，说明有新连接到来，接受它
		if(FD_ISSET(listen_fd,&read_set)){
			struct sockaddr_in client_addr;
			socklen_t client_addr_len=sizeof(client_addr);
			int conn_fd=accept(listen_fd,(struct sockaddr*)&client_addr,&client_addr_len);
			if(conn_fd==-1){
				perror("accept failed: ");
				exit(1);
			}
			//将这个新建立的用于与客户端通信的文件描述加入到需要被监听的文件描述符集合中
			FD_SET(conn_fd,&all_set);
			FD_LISTENED[size++]=conn_fd;
			//更新最大的文件描述符
			maxfd=conn_fd>maxfd?conn_fd:maxfd;
			//如果处理完监听socket的文件描述符上的可读事件后，就绪事件为0，说明当前没有其它文件描述符上面有可读事件
			if(--ret==0) continue;
		}	
		//遍历整个可能的文件描述符范围，查看是否有可读事件
		for(int i=1;i<size;i++){
			//if(i==listen_fd) continue;
			//如果有可读事件，那么说明有客户端发送数据到服务器，那么我们处理业务逻辑
			if(FD_ISSET(FD_LISTENED[i],&read_set)){
				ret=recv(FD_LISTENED[i],buffer,BUF_SIZE-1,0);
				//如果客户端发送来的数据长度为0，说明客户端发起断开连接，那么我们也断开连接，关闭
				//用于与这个客户端通信文件描述符，并将该文件描述从需要被监听的文件描述符集合中去除
				if(ret==0){
					printf("end of file\n");
					FD_LISTENED[i]=FD_LISTENED[--size];
					close(FD_LISTENED[i]);
					FD_CLR(FD_LISTENED[i],&all_set);
					continue;
				}
				for(int i=0;i<ret;i++)
					buffer[i]=toupper(buffer[i]);
				write(STDOUT_FILENO,buffer,ret);
				write(FD_LISTENED[i],buffer,ret);
			}
		}
	}

	close(listen_fd);
	return 0;
}
