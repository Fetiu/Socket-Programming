#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/time.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define BUF_SIZE 50
#define SERVER_MODE 2
#define CLIENT_MODE 3

int run_server(int port);
int run_client(char ip[], int port);

static char* escape = "exit";
struct sockaddr_in server_addr, client_addr;

int main(int argc, char* argv[])
{
	int dst_fd, ifd;
	int str_len;
	char message[BUF_SIZE];
	struct timeval timeout;

	fd_set readfds, readtemp;
	int max_fd =0;
	int result;
	int mode;

	switch(argc) {
		case 2:
			mode = SERVER_MODE;
			dst_fd = run_server(atoi(argv[1]));
			break;
		case 3:
			mode = CLIENT_MODE;
			dst_fd = run_client(argv[1], atoi(argv[2]));
			break;
		default:
			fprintf(stderr, "Usage: %s [<IP>] <PORT>\n", argv[0]);
			return 0;
	}

	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);
	FD_SET(dst_fd, &readfds);
	max_fd = dst_fd;

	while(1){
		readtemp = readfds;

		result = select(max_fd+1, &readtemp, NULL, NULL, NULL);
		if(result ==-1){
			perror("select");
			exit(EXIT_FAILURE);
		}
		else if(result == 0){
			printf("Timeout\n");
			continue;
		}

		for(ifd = 0; ifd<=max_fd; ifd++){
			if(FD_ISSET(ifd, &readtemp)) {
				if(ifd == STDIN_FILENO) {
					str_len = read(ifd, message, BUF_SIZE);
					if(strcmp(message, escape) ==0){
						close(ifd);
						printf("DISCONNECTED\n");
						return 0;
					}
					//message[str_len -1] =0;
					printf("ME: %s\n", message);
					write(dst_fd, message, strlen(message));
				}
				else if(ifd == dst_fd){
					str_len = read(ifd, message, BUF_SIZE);
					if(str_len == 0){
						close(dst_fd);
						printf("DISCONNECTED\n");
						return 0;
					}
					message[str_len -1] =0;
					printf("SOMEONE: %s\n", message);
				}
			}
		}
	}
	close(ifd);
	return 0;
}

int run_server(int port){
 	
	unsigned int sin_size = sizeof(struct sockaddr_in);
	
	int server_s, client_s;
	server_s = socket(AF_INET, SOCK_STREAM, 0);

	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&(server_addr.sin_zero), 0,8);
	
	if(bind(server_s, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) < 0){
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if(listen(server_s,5) < 0){
		perror("listen");
		exit(EXIT_FAILURE);
	}

	printf("Waiting for connect\n");
	client_s = accept(server_s, (struct sockaddr *)&client_addr, &sin_size);

	if(client_s ==-1){
		perror("accept");
		exit(EXIT_FAILURE);
	}


	printf("Connected client: %s, (Enter \"exit\" to quit)\n", inet_ntoa(client_addr.sin_addr));
	return client_s;
}

int run_client(char ip[], int port){
 	
	int server_s = socket(AF_INET, SOCK_STREAM, 0);

	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(port);
	inet_aton(ip, &server_addr.sin_addr);
	memset(&(server_addr.sin_zero), 0,8);

	if(connect(server_s, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
		perror("connect");
		exit(EXIT_FAILURE);
	}

	printf("Connected, (Enter \"exit\" to quit)\n");

	return server_s;
}

