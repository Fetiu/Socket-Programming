#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define BUFF_SIZE 20

int main(){

	int sockfd;
	struct sockaddr_in dest_addr;
	char message[BUFF_SIZE];
	char receive[BUFF_SIZE];
	int str_len;
	int port_num;

	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(9100);
	inet_aton("127.0.0.1", &(dest_addr.sin_addr));
	memset(&(dest_addr.sin_zero), 0, 8);

	printf("trying to connect\n");
	if( connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) < 0){
		printf("failed\n");
		exit(0);
	}
	

	while(1){
		printf("input message: ");
		fgets(message, BUFF_SIZE, stdin);
		write(sockfd, message, strlen(message));
		str_len= read(sockfd, receive ,BUFF_SIZE);
		receive[str_len] =0;
		printf("from server: %s", receive);
	}

	close(sockfd);
	return 0;
}
