#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc,char* argv[])
{
	if(argc<3){
		printf("usage: %s server_ip server_port\n",argv[0]);
		exit(1);
	}

	char buffer_ter[1024];
	char buffer_soc[1024];
	bzero(&buffer_ter,sizeof(buffer_ter));
	bzero(&buffer_soc,sizeof(buffer_soc));

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

	while(1){

		ret=read(STDIN_FILENO,buffer_ter,sizeof(buffer_ter));
		write(client_fd,buffer_ter,ret);

		ret=read(client_fd,buffer_soc,sizeof(buffer_soc));
		if(ret==0){
			printf("reach end of file , connection closed by server\n");
			break;
		}
		write(STDOUT_FILENO,buffer_soc,ret);
	}
	close(client_fd);
	return 0;
}

