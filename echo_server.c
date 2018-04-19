#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define BUF_SIZE 10

int main(){

	int sockfd;
	struct sockaddr_in addr;
	char message[BUF_SIZE];
	int str_len;
	int newfd;
	struct sockaddr_in their_addr;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	
	printf("making server socket\n");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
		
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9100);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(addr.sin_zero), 0, 8);

	if(bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1){
		perror("bind");
		return 0;
	}
	printf("bind complete\n");
	
	if(listen(sockfd, 5) < 0){
		perror("listen");
		return 0;
	}
	system("netstat -pa --tcp");

	newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	
	if(newfd == -1)
		perror("listen");
	else
		printf("Connection Established\n");

	while((str_len = read(newfd, message, BUF_SIZE)) != 0 ) {
		write(newfd, message, str_len);
	}

	close(sockfd);
	return 0;
}
