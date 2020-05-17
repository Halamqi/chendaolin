#define _GNU_SOURCE 1 
#include <pthread.h>
#include "myInit.h"
#include <sys/epoll.h>

#define BUF_SIZE 2048
#define USER_LIMIT 100
#define MAX_FD 65535

struct client_info{
	int fd;
	int port;
	char ip[INET_ADDRSTRLEN];
	char* writebuff;
	char readbuff[BUF_SIZE];
	char send_to_other[2*BUF_SIZE];
};

struct client_info* cinfo[MAX_FD];

//记录所有监听的文件描述符
int fd_listen[1024];
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
	int user_count=0;

	struct sockaddr_in client_addr;
	socklen_t client_addr_len=sizeof(client_addr);
	bzero(&client_addr,client_addr_len);
	int conn_fd;

	int epfd=epoll_create(5);
	if(epfd<0) {
		perror("epoll_create failed: ");
		exit(1);
	}

	fd_listen[size++]=listen_fd;

	struct epoll_event evnt;
	evnt.events=EPOLLIN|EPOLLERR;
	evnt.data.fd=listen_fd;

	epoll_ctl(epfd,EPOLL_CTL_ADD,listen_fd,&evnt);

	struct epoll_event events[1024];

	while(1){
		int readyfds=epoll_wait(epfd,events,1024,-1);
		if(readyfds<0){
			perror("epoll failed: ");
			exit(1);
		}

		for(int i=0;i<readyfds;i++){
			if(events[i].data.fd==listen_fd&&events[i].events&EPOLLIN){
				char client_ip[INET_ADDRSTRLEN];
				conn_fd=accept(listen_fd,(struct sockaddr*)&client_addr,&client_addr_len);
				if(conn_fd>0)
					printf("client ip:%s port:%d has connected\n",
							inet_ntop(AF_INET,&client_addr.sin_addr,client_ip,INET_ADDRSTRLEN),
							ntohs(client_addr.sin_port));
				else{
					perror("accept failed: ");
					exit(1);
				}
				if(user_count>=USER_LIMIT){
					char* info="too many user, please connect later\n";
					send(conn_fd,info,sizeof(info),0);
					close(conn_fd);
					continue;
				}
				//为新建立的连接注册事件
				evnt.events=EPOLLIN|EPOLLOUT|EPOLLERR;
				evnt.data.fd=conn_fd;
				epoll_ctl(epfd,EPOLL_CTL_ADD,conn_fd,&evnt);
				
				user_count++;
				fd_listen[size++]=conn_fd;
				
				//初始化存储新建立连接数据结构
				cinfo[conn_fd]=malloc(sizeof(struct client_info));
				cinfo[conn_fd]->fd=conn_fd;
				strcpy(cinfo[conn_fd]->ip,client_ip);
				cinfo[conn_fd]->port=ntohs(client_addr.sin_port);
				cinfo[conn_fd]->writebuff=NULL;
				bzero(cinfo[conn_fd]->readbuff,BUF_SIZE);
				bzero(cinfo[conn_fd]->send_to_other,2*BUF_SIZE);

				printf("a new user joins chat,now have %d users\n",user_count);
				//if(--nreadyfds==0) continue;
			}

			//文件描述符上有异常事件
			/*else if(events[i].events&EPOLLERR){
				printf("get an error form %d\n",events[i].data.fd);
				char errors[100];
				bzero(errors,100);
				socklen_t length=sizeof(errors);
				if(getsockopt(events[i].data.fd,SOL_SOCKET,SO_ERROR,&errors,&length)<0){
					perror("get error failed: ");
					exit(1);
				}
				continue;
			}*/

			//客户端断开连接
			else if(events[i].events&EPOLLRDHUP){
				int conn_fd=events[i].data.fd;
				printf("a user quit the chat,ip=%s,port=%d\n",cinfo[conn_fd]->ip,cinfo[conn_fd]->port);
				free(cinfo[conn_fd]);
				close(conn_fd);
				epoll_ctl(epfd,EPOLL_CTL_DEL,conn_fd,&events[i]);
				user_count--;
			}

			//读事件就绪
			else if(events[i].events&EPOLLIN){
				int conn_fd=events[i].data.fd;
				int nreads=recv(conn_fd,cinfo[conn_fd]->readbuff,BUF_SIZE-1,0);
				if(nreads<0){
					perror("recv failed: ");
					exit(1);
				}

				else if(nreads==0){
					printf("a user quit the chat,ip=%s,port=%d\n",cinfo[conn_fd]->ip,cinfo[conn_fd]->port);
					free(cinfo[conn_fd]);
					close(conn_fd);
					epoll_ctl(epfd,EPOLL_CTL_DEL,conn_fd,&events[i]);
					user_count--;
					continue;
				}

				else{
					//如果读取的数据大于0，则将发送这个数据的客户端的信息和数据打印到屏幕上
					printf("user ip:%s port:%d received:\n",cinfo[conn_fd]->ip,cinfo[conn_fd]->port);
					write(STDOUT_FILENO,cinfo[conn_fd]->readbuff,nreads);
					//对其他所有被监听的文件描述（也就是其他的连接）而言，要将这个客户发送而来的数据其他连接的文件描述符发送给对应的其他客户端
					//将该客户端发送而来的数据格式化，并存放在这个连接独有的缓冲区中
					snprintf(cinfo[conn_fd]->send_to_other,24+sizeof(cinfo[conn_fd]->port)+strlen(cinfo[conn_fd]->ip)+nreads,
							"user ip:%s, port:%d send:\n%s\n",
							cinfo[conn_fd]->ip,cinfo[conn_fd]->port,cinfo[conn_fd]->readbuff);
					for(int j=0;j<size;j++){
						if(fd_listen[j]!=conn_fd&&fd_listen[j]!=listen_fd){
							//其他连接的写缓冲指针指向这个被格式化了的数据，准备发送给其他客户端
							cinfo[fd_listen[j]]->writebuff=cinfo[conn_fd]->send_to_other;
							//关闭其他连接的读事件，开启写事件，将这个连接读过来的数据发送出去后，再开启读关闭写
							//events[i].events|=~EPOLLIN;
							//events[i].events|=EPOLLOUT;
						}
					}
				}
			}
			//当文件描述符上写事件就绪，将该数据发送出去
			else if(events[i].events&EPOLLOUT){
				int conn_fd=events[i].data.fd;
				if(cinfo[conn_fd]->writebuff){
					send(conn_fd,cinfo[conn_fd]->writebuff,strlen(cinfo[conn_fd]->writebuff),0);
					cinfo[conn_fd]->writebuff=NULL;
					//events[i].events|=EPOLLIN;
					//events[i].events|=~EPOLLOUT;
				}
			}
		}
	}

	close(listen_fd);
	return 0;
}
