#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>

struct hostent* host;
struct sockaddr_in addr;

void main(int argc, char* argv[]){
	int i;
	host = gethostbyname(argv[1]);
	if(host ==NULL){
		fprintf(stderr, "cannot find IP address from %s\n", argv[1]);
		return;
	}

	printf("Ofiicial name: %s\n", host->h_name);

	for(i=0; host->h_aliases[i]; i++);
		printf("Aliases %d: %s\n", i+1, host->h_aliases[i]);
	
	printf("Address type: %s\n", (host->h_addrtype ==PF_INET) ? "PF_INET":"Unknown");

	for(i=0; host->h_addr_list[i] ; i++)
		printf("IP addr %d: %s\n", i+1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
	
	return;
}
