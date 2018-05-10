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

	int sockfd, newfd;
	struct sockaddr_in addr;
	struct sockaddr_in their_addr;
	char input[BUF_SIZE];
	char message[BUF_SIZE];
	int str_len;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
		
	addr.sin_family = AF_INET;
	addr.sin_port = htons(9100);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(addr.sin_zero), 0, 8);

	if(bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1){
		perror("bind");
		return 0;
	}
	printf("chatting server initializing\n");

	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);
	FD_SET(0,  &readfds);
	max_fd =sockfd;

if(listen(sockfd, 5) < 0){
		perror("listen");
		return 0;
	}

	newfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	if(newfd ==-1){
		perror("accept");
		exit(0);
	}
	if(max_fd <newfd)
		max_fd =newfd;
	printf("Connected client: %d\n", newfd);
	
	//client socket listen no polyo?
	while(1){
		readtemp = readfds;//  readfds has previos fd_set information
		timeout.tv_sec =5;
		timeout.tv_usec = 0;

		result = select(max_fd +1, &readtemp, NULL, NULL, &timeout);//null make no timeout.
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
					str_len = read(i, message, BUF_SIZE);
					if(str_len ==0)
						close(newfd);
					printf("from: ");
					write(1, input, str_len);	
				}
				else if(i == 0){
					str_len = read(i, input, BUF_SIZE);
					write( newfd, input, str_len);	
				}
			}
		}
	}

	close(sockfd);
	return 0;
}
