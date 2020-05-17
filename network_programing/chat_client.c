#define _GNU_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
int main(int argc,char* argv[])
{
	if(argc<3){
		printf("usage: %s server_ip server_port\n",argv[0]);
		exit(1);
	}

	struct sockaddr_in address;
	bzero(&address,sizeof(address));
	address.sin_family=AF_INET;
	address.sin_port=htons(atoi(argv[2]));
	inet_pton(AF_INET,argv[1],&address.sin_addr);

	int client_fd=socket(AF_INET,SOCK_STREAM,0);
	if(client_fd==-1){
		perror("socket failed\n");
		exit(1);
	}

	int ret=connect(client_fd,(struct sockaddr*)&address,sizeof(address));
	if(ret==-1){
		perror("connect failed\n");
		exit(1);
	}

	struct pollfd fds[2];
	fds[0].fd=0;
	fds[0].events=POLLIN;
	fds[0].revents=0;
	fds[1].fd=client_fd;
	fds[1].events=POLLIN|POLLERR;
	fds[1].revents=0;
	char buffer[2048];
	int pipefd[2];
	ret=pipe(pipefd);
	if(ret==-1){
		perror("pipe failed: ");
		exit(1);
	}

	while(1){
		int nreadys=poll(fds,2,-1);
		if(nreadys<0){
			perror("poll failed: ");
			exit(1);
		}

		if(fds[1].revents&POLLRDHUP){
			printf("connection closed by server\n");
			break;
		}

		else if(fds[1].revents&POLLIN){
			int nreads=recv(fds[1].fd,buffer,sizeof(buffer)-1,0);
			if(nreads<0){
				perror("read failed: ");
				exit(1);
			}
			else if(nreads==0){
				printf("connection closed by server\n");
				break;
			}
			else {
				write(STDOUT_FILENO,buffer,nreads);
			}
		}

		else if(fds[0].revents&POLLIN){
			ret=splice(0,NULL,pipefd[1],NULL,32678,SPLICE_F_MORE|SPLICE_F_MOVE);
			ret=splice(pipefd[0],NULL,client_fd,NULL,32678,SPLICE_F_MORE|SPLICE_F_MOVE);
		}
	}


	close(client_fd);
	return 0;
}

