#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define BUF_SIZE 20

int main(){

	struct timeval timeout;
	fd_set readfds, readtemp;
	int max_fd=0;
	int result, i;

	int sockfd;
	struct sockaddr_in dest_addr;
	char message[BUF_SIZE];
	char input[BUF_SIZE];
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
		readtemp = readfds;//  readfds has previos fd_set information

		result = select(max_fd +1, &readtemp, NULL, NULL, NULL);
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
						close(sockfd);
					printf("from Host: ");
					write(1, input, str_len);	
				}
				else if(i == STDIN_FILENO){
					str_len = read(i, input, BUF_SIZE);
					write( sockfd, input, str_len);	
				}
			}
		}
	}
	close(sockfd);
	
	return 0;
}
