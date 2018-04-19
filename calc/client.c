#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define BUFF_SIZE 20

struct formula{
	int num;
	char operator;
	int* operand;
};

int main(){

	int sockfd;
	struct sockaddr_in dest_addr;
	char message[BUFF_SIZE];
	char receive[BUFF_SIZE];
	int str_len;
	int port_num;

	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	printf("input port: ");
	scanf("%d", &port_num);

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port_num);
	inet_aton("127.0.0.1", &(dest_addr.sin_addr));
	memset(&(dest_addr.sin_zero), 0, 8);

	printf("trying to connect\n");
	if( connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr)) < 0){
		printf("failed\n");
		exit(0);
	}
	
	struct formula a;
	
	while(1){
		
		printf("Number of OP: ");
		scanf("%d", &a.num);	
		
		for(int i=0; i < a.num; i++){
			printf("Operand %d: ",i);
			scanf("%d", a.operand+i);
		}	
		printf("operator: ");
		scanf("%c", &a.operator);
		
		send(sockfd, &a.num, sizeof(int),0);
		for(int i=0; i< a.num; i++)
			send(sockfd, &a.operand[i], sizeof(int),0);
		send(sockfd, &a.num, sizeof(char),0);
		
		str_len= read(sockfd, receive ,BUFF_SIZE -1);
		message[str_len] =0;
		printf("from server: %s", message);
	}

	close(sockfd);
	return 0;
}
