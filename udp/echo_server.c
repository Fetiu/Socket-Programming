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
	struct sockaddr_in client_addr;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in their_addr;
	socklen_t addr_size = sizeof(struct sockaddr_in);
	
	printf("making server socket\n");
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(9100);
	client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(client_addr.sin_zero), 0, 8);

	if(bind(sockfd, (struct sockaddr*)&client_addr, sizeof(struct sockaddr_in)) == -1){
		perror("bind");
		return 0;
	}
	printf("bind complete\n");
	

	while(1){
		addr_size = sizeof(client_addr);
		
		str_len = recvfrom(sockfd, message, BUF_SIZE,0,(struct sockaddr *)&client_addr,&addr_size);
		message[str_len]=0;
		printf("%s:%s\n", inet_ntoa(client_addr.sin_addr), message);
		sendto(sockfd, message, str_len, 0, (struct sockaddr *)&client_addr, addr_size);
		
	}

	close(sockfd);
	return 0;
}
