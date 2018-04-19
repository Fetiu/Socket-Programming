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
	struct sockaddr_in from_addr;
	char message[BUFF_SIZE];
	char receive[BUFF_SIZE];
	int str_len;
	int port_num =9100;
	socklen_t addr_len = sizeof(struct sockaddr_in);

	sockfd = socket(PF_INET, SOCK_DGRAM, 0);

	dest_addr.sin_family = PF_INET;
	dest_addr.sin_port = htons(port_num);
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
		sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
		str_len = recvfrom(sockfd, receive, BUFF_SIZE,0, (struct sockaddr *)&from_addr, &addr_len);
		
		receive[str_len] =0;
		printf("from server: %s", receive);
	}

	close(sockfd);
	return 0;
}
