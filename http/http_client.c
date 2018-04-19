#include<stdio.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

int main(int argc, char* argv[])
{
	int sockfd, n;
	struct sockaddr_in addr;
	char request[] = "GET /index.html HTTP/1.1\r\n\r\n";
	char buf[BUFSIZ];	
	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(80);
	inet_aton(argv[1], &addr.sin_addr);
	memset(&(addr.sin_zero),0,8);

	if( connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0){
		printf("connection failed\n");
		exit(1);
	}	
	printf("connected...\n");	
	write(sockfd, request, strlen(request));

	while((n = read(sockfd, buf, BUFSIZ)) != 0){
		write(1, buf, n);
	}	
	close(sockfd);

	return 0;
}
