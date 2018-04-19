#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

int main(){

	struct sockaddr_in addr;
	char *serv_ip = "192.168.0.10";
	unsigned short port = 9190;
	int sockfd = socket(PF_INET, SOCK_STREAM, 0);
	socklen_t len = sizeof(struct sockaddr_in);


	memset(&addr, 0 , sizeof(addr));
	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//inet_aton(serv_ip, &addr.sin_addr);
	addr.sin_port = htons(port);


	if(bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) ==-1){
		perror("bind");
		exit(1);
	}

	if(getsockname(sockfd, (struct sockaddr*)&addr, &len) < 0){
		perror("getsockname");
		exit(1);
	}

	printf("Socket IP: %s, PORT %u\n", inet_ntoa(addr.sin_addr),
			(unsigned)ntohs(addr.sin_port));

	if(getpeername(sockfd, (struct sockaddr*)&addr, &len) < 0){
		perror("getpeername");
		exit(1);
	}

	printf("Peer IP: %s, PORT %u\n", inet_ntoa(addr.sin_addr),
			(unsigned)ntohs(addr.sin_port));
}
