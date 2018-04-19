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
	char buf[BUF_SIZE];
	int size;
	int client_cnt=0;
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

	while(1) {
		newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);

		client_cnt++;
		printf("Connected client[%d]: %s %d\n", client_cnt, inet_ntoa(their_addr.sin_addr),ntohs(their_addr.sin_port));

		if(fork() == 0 ){
			close(sockfd);

			while(1) {
				if((size = recv(newfd, buf, BUF_SIZE, 0)) > 0){
					printf("%d lines received", size);
					memcpy(&buf[size], "client\n",6); 
					send(newfd, buf, size, 0);
				}
				else{
					client_cnt--;
					printf("Connection terminated(%d connection left)\n", client_cnt);
					break;
				}
			}
			close(newfd);
			exit(0);
		}
		close(newfd);
	}
	close(sockfd);
	return 0;
}
