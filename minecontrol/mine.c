#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/time.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<termios.h>
#include<fcntl.h>
#define BUF_SIZE 50

int linux_kbhit();
void list_command();
void move_front(int dist);
void move_back(int dist);
void move_left(int dist);
void move_right(int dist);
void move(int,int,int);
void get_tile();
void get_pos();
void set_view();
void fix_view();
void touch();
void get_id(int,int,int);
int get_height(int,int);
void reset_height();

struct pos{
	float x,y,z;
}Pos;
int x,y,z;
int sockfd, n, i;
char buffer[BUF_SIZE] = {0};
char* token;

int main(int argc, char* argv[]){
	struct sockaddr_in dest_addr;

	fd_set readfds, readtemp;
	int max_fd = 0;
	int result;

	sockfd = socket(PF_INET, SOCK_STREAM, 0);

	if(argc < 2){
		printf("usage: minecontrol <server ip address>\n");
		return 0;
	}

	dest_addr.sin_family = PF_INET;
	dest_addr.sin_port = htons(4711);
	inet_aton(argv[1], &dest_addr.sin_addr);
	memset(&(dest_addr.sin_zero),0,8);

	if(connect(sockfd,(struct sockaddr*) &dest_addr, sizeof(struct sockaddr_in)) <0){
		printf("Unable to connect to remote host.\n");
		exit(1);
	}
	printf("Connected to %s\n", inet_ntoa(dest_addr.sin_addr));// internet num to array / internet array to num

	puts("\ninsert command:");


	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds);
	FD_SET(sockfd,&readfds);
	max_fd = sockfd;
 //is io multiflexing need? just stdin..
	
	while(1){
		readtemp = readfds;	

		result = select(max_fd+1, &readtemp, NULL,NULL,NULL);
		if(result == -1){
			perror("select");
			exit(1);
		}
		//get pos by realtime.
		
		for(i =0; i<= max_fd; i++){
			if(FD_ISSET(i, &readtemp)){

				if(i == STDIN_FILENO){
					fgets(buffer, BUF_SIZE, stdin);

					if(strcmp(buffer,"ls\n") == 0)
						list_command();
					else if(strcmp(buffer,"pwd\n") == 0){
						get_pos();
						printf("%.3f,%.3f,%.3f\n",Pos.x,Pos.y,Pos.z);
					}
					else if(strcmp(buffer,"dir\n") == 0){
						get_tile();
						printf("%d,%d,%d\n",x,y,z);
					}
					else if(strncmp(buffer,"w\n",1) == 0){
						if(strlen(buffer) == 2)
							n = 1;
						else{
							token = strtok(buffer," ");
							token = strtok(NULL,"\n");
							n = atoi(token);
						}
						move_front(n);	
					}
					else if(strncmp(buffer,"a\n",1) == 0){
						if(strlen(buffer) == 2)
							n = 1;
						else{
							token = strtok(buffer," ");
							token = strtok(NULL,"\n");
							n= atoi(token);
						}
						move_left(n);	
					}
					else if(strncmp(buffer,"s\n",1) == 0){
						if(strlen(buffer) == 2)
							n = 1;
						else{
							token = strtok(buffer," ");
							token = strtok(NULL,"\n");
							n= atoi(token);
						}
						move_back(n);	
					}
					else if(strncmp(buffer,"d\n",1) == 0){
						if(strlen(buffer) == 2)
							n = 1;
						else{
							token = strtok(buffer," ");
							token = strtok(NULL,"\n");
							n= atoi(token);
						}
						move_right(n);	
					}
					else if(strcmp(buffer,"v\n") == 0)
						set_view();
					else if(strcmp(buffer,"f\n") == 0)
						fix_view();
					else if(strcmp(buffer,"touch\n") == 0)
						touch();
					else if(strcmp(buffer,"quit\n") == 0){
						printf("Disconnect from server\n");
						close(sockfd);
						return 0;
					}
					else if(strncmp(buffer,"cd",2) == 0){ 
						if(strlen(buffer) == 3) 
							printf("\tusage: cd x/y/z\n");
						else{
							token = strtok(buffer," ");
							token = strtok(NULL,"/");
							x = atoi(token);
							token = strtok(NULL,"/");
							y = atoi(token);
							token = strtok(NULL,"/");
							z = atoi(token); 

							move(x,y,z);
						}
					}
					else
						printf("unknown command\n");
				}
				else if(i == sockfd){ //message
					n = read(sockfd, buffer, BUF_SIZE);
					if(n == 0){
						close(sockfd);
						printf("disconnected from server\n");
						return 0;
					}
					buffer[n-1] = 0;
					printf("%s\n",buffer);
				}
			}
		}
		puts("\ninsert command:");
	}
	close(sockfd);
	return 0;
}

void list_command(){
	printf(
		"Command List\n\
		\n\
		[PLAYER]\n\
		w,a,s,d  : move (player.setPos)\n\
		cd x/y/z : change tile (player.setTile)\n\
		pwd      : current working position (player.getPos)\n\
		dir      : current working tile (player.getTile)\n\
		\n\
		[CAMERA]\n\
		v      : change camera mode (camera.mode.setNormal-> setFollow)\n\
		f      : set camera fixed (camera.mode.setFixed)\n\
		\n\
		[WORLD]\n\
		touch    : put blocks around player ( world.setBlock )\n\
		cat  x/y/z : get block id ( world.getBlock )\n\
		du x/z   : get tile height ( world.getHeight )\n");
}

void move_front(int dist)
{
	int dst;
	get_pos();
	dst = Pos.z - dist;

	while(Pos.z > dst){
		Pos.z -= 0.10;
		reset_height();
		sprintf(buffer, "player.setPos(%.2f,%.2f,%.2f)\n", Pos.x, Pos.y, Pos.z);
		write(sockfd, buffer, BUF_SIZE);
	}
}
void move_back(int dist){
	int dst;
	get_pos();
	dst = Pos.z + dist;

	while(Pos.z < dst){
		Pos.z += 0.10;
		reset_height();
		sprintf(buffer, "player.setPos(%.2fg,%f,%f)\n", Pos.x, Pos.y, Pos.z);
		write(sockfd, buffer, BUF_SIZE);
	}
}
void move_left(int dist){
	int dst;
	get_pos();
	dst = Pos.x - dist;

	while(Pos.x > dst){
		Pos.x -= 0.10;
		reset_height();
		sprintf(buffer, "player.setPos(%.2fg,%f,%f)\n", Pos.x, Pos.y, Pos.z);
		write(sockfd, buffer, BUF_SIZE);
	}
}
void move_right(int dist){
	int dst;
	get_pos();
	dst = Pos.x - dist;

	while(Pos.x < dst){
		Pos.x += 0.10;
		reset_height();
		sprintf(buffer, "player.setPos(%.2fg,%f,%f)\n", Pos.x, Pos.y, Pos.z);
		write(sockfd, buffer, BUF_SIZE);
	}
}

void move(int x,int y,int z){
	sprintf(buffer, "player.setTile(%d,%d,%d)\n",x,y,z);	
	write(sockfd, buffer, BUF_SIZE);
}
void get_tile(){
	if( write(sockfd, "player.getTile()\n", BUF_SIZE) < 0){
		return;
	}
	if((n = read(sockfd, buffer, BUF_SIZE)) < 0){
		printf("read failed");
		return;
	}

	token = strtok(buffer,",");
	x = atoi(token);
	token = strtok(NULL,",");
	y = atoi(token);
	token = strtok(NULL,",");
	z = atoi(token);
}

void get_pos(){
	if( write(sockfd, "player.getPos()\n", BUF_SIZE) < 0){
		return;
	}
	if((n = read(sockfd, buffer, BUF_SIZE)) < 0){
		printf("read failed");
		return;
	}

	token = strtok(buffer,",");
	Pos.x = atof(token);
	token = strtok(NULL,",");
	Pos.y = atof(token);
	token = strtok(NULL,",");
	Pos.z = atof(token); 
}
void set_view(){
	static int set = 1;
	if(set == 0){
		write(sockfd, "camera.mode.setNormal()\n", BUF_SIZE);
		set = 1;
	}
	else{
		write(sockfd, "camera.mode.setFollow()\n", BUF_SIZE);
		set = 0;
	}


}
void fix_view(){
	write(sockfd, "camera.mode.setFixed()\n", BUF_SIZE);
}
void touch(){}
void get_id(int x, int y, int z){}

int get_height(int x, int z){
	sprintf(buffer,"world.getHeight(%d,%d)\n", x,z);
	write(sockfd, buffer, BUF_SIZE);
	if(read(sockfd, buffer, BUF_SIZE) < 0){
		printf("read failed");
		return 0;
	}
	return atoi(buffer);
}



void reset_height(){
	get_tile();//current working tile.
	if((n = get_height(x,z)) > y )
		Pos.y = (float)n;
}

