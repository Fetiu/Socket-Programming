#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/time.h>
#include<sys/epoll.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#define BUF_SIZE 10

int main(){

	int epfd;
	int event_cnt, i;
	struct epoll_event ev, evs[10];

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

	if(listen(sockfd, 5) < 0){
		perror("listen");
		return 0;
	}

	epfd = epoll_create1(0);
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev); //ev is temporary.


	while(1){
		event_cnt = epoll_wait(epfd, evs, 10, -1);
		if(event_cnt == -1) {
			perror("epoll");
			break;
		}
		else if(event_cnt == 0) {
			printf("Timeout\n");
			continue;// jump to end of while
		}
		for(i =0; i <= event_cnt; i++){
			if(evs[i].events == EPOLLIN){
				if(evs[i].data.fd ==sockfd) {
					newfd =accept(evs[i].data.fd, (struct sockaddr*)&their_addr, &sin_size);
					if(newfd ==-1) {
						perror("accept");
						exit(0);
					}
					printf("Connected client: %d\n", newfd);
					ev.events = EPOLLIN;
					ev.data.fd = newfd;
					epoll_ctl(epfd, EPOLL_CTL_ADD, newfd, &ev);//why is it pointer?
				}
				else {
					str_len = read(evs[i].data.fd, message, BUF_SIZE);
					if(str_len == 0) {
						epoll_ctl(epfd, EPOLL_CTL_DEL, evs[i].data.fd, NULL);
						close(evs[i].data.fd);
						printf("Close client: %d\n", evs[i].data.fd);
					}
					else
						write(evs[i].data.fd, message,str_len);
				}
			}
		}
	}
	close(sockfd);
	return 0;
}
