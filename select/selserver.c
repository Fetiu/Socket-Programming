#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/time.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define BUF_SIZE 10

int main(){

	struct timeval timeout;
	fd_set readfds, readtemp;
	int max_fd =0;
	int result,i ;

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

	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	max_fd = sockfd;

	if(listen(sockfd, 5) < 0){
		perror("listen");
		return 0;
	}
	while(1){
		readtemp = readfds;
		timeout.tv_sec =5;
		timeout.tv_usec = 0;

		result = select(max_fd +1, &readtemp, NULL, NULL, &timeout);
		if(result ==-1){
			perror("select");
			break;
		}
		else if(result ==0){
			printf("Timeout\n");
			continue;
		}

		for( i =0; i < max_fd; i++){
			if(FD_ISSET(i, &readtemp)) {
				if(i == sockfd) {
					newfd = accept(i, (struct sockaddr *)&their_addr, &sin_size);
					if(newfd ==-1){
						perror("accept");
						exit(0);
					}
					printf("Connected client: %d\n", newfd);
					FD_SET(newfd, &readfds);
					if(max_fd <newfd)
						max_fd =newfd;
				}
				else{
					str_len =read(i, message, BUF_SIZE);
					if(str_len ==0) {
						FD_CLR(i, &readfds);
						close(i);
						printf("Closed client: %d\n",i);
					}
					else {
						write(i, message, str_len);
					}
				}
			}
		}
		close(newfd);
	}
	close(sockfd);
	return 0;
}
