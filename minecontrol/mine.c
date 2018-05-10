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
int get_id(int,int,int);
int get_height(int,int);
void reset_height();
int parse_arg(char*);

struct pos{
	float x,y,z;
}Pos;
int x,y,z;
int sockfd, n, i;
char buffer[BUF_SIZE] = {0};
char* token;

int main(int argc, char* argv[]){
	struct sockaddr_in dest_addr;

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

	while(1){
		fgets(buffer, BUF_SIZE, stdin);

		if(strcmp(buffer,"ls\n") == 0)
			list_command();
		else if(strcmp(buffer,"pwd\n") == 0){
			get_pos();
		}
		else if(strcmp(buffer,"dir\n") == 0){
			get_tile();
		}
		else if(strcmp(buffer,"touch\n") == 0){
			touch();
		}
		else if(strncmp(buffer,"du",2) == 0){
			token = strtok(buffer," ");
			token = strtok(NULL,"/");
			if(token!= NULL){
				x = atoi(token);
				token = strtok(NULL,"/");
				z = atoi(token); 
				get_height(x,z);
			}
			else
				printf("\tusage: du x/z\n");
		}
		else if(strncmp(buffer,"cat",3) == 0){
			token = strtok(buffer," ");
			token = strtok(NULL,"/");
			if(token!= NULL){
				x = atoi(token);
				token = strtok(NULL,"/");
				y = atoi(token);
				token = strtok(NULL,"/");
				z = atoi(token); 
				get_id(x,y,z);
			}
			else
				printf("\tusage: cat x/y/z\n");
		}
		else if(strncmp(buffer,"cd",2) == 0){ 
			token = strtok(buffer," ");
			token = strtok(NULL,"/");
			if(token!= NULL){
				x = atoi(token);
				token = strtok(NULL,"/");
				y = atoi(token);
				token = strtok(NULL,"/");
				z = atoi(token); 
				move(x,y,z);
			}
			else
				printf("\tusage: cd x/y/z\n");
		}
		else if(strncmp(buffer,"w\n",1) == 0){
			n = parse_arg(buffer);
			move_front(n);	
		}
		else if(strncmp(buffer,"a\n",1) == 0){
			n = parse_arg(buffer);
			move_left(n);	
		}
		else if(strncmp(buffer,"s\n",1) == 0){
			n = parse_arg(buffer);
			move_back(n);	
		}
		else if(strncmp(buffer,"d\n",1) == 0){
			n = parse_arg(buffer);
			move_right(n);	
		}
		else if(strcmp(buffer,"v\n") == 0)
			set_view();
		else if(strcmp(buffer,"f\n") == 0)
			fix_view();
		else if(strcmp(buffer,"r\n") == 0)
			reset_height();
		else if(strcmp(buffer,"quit\n") == 0){
			printf("Disconnect from server\n");
			close(sockfd);
			return 0;
		}
		else
			printf("unknown command\n");
		
		memset(buffer,'\0',BUF_SIZE);
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
		cat x/y/z : get block id ( world.getBlock )\n\
		du x/z   : get tile height ( world.getHeight )\n");
}

void move_front(int dist)
{
	get_pos();
	Pos.z += dist;
	if((n = get_height((int)Pos.x,(int)Pos.z)) > Pos.y)
		Pos.y = (float)n;

	sprintf(buffer, "player.setPos(%f,%f,%f)\n", Pos.x, Pos.y, Pos.z);
	write(sockfd, buffer, BUF_SIZE);
}
void move_back(int dist){
	get_pos();
	Pos.z -=dist;

	if((n = get_height((int)Pos.x,(int)Pos.z)) > Pos.y)
		Pos.y = (float)n;
	sprintf(buffer, "player.setPos(%f,%f,%f)\n", Pos.x, Pos.y, Pos.z);
	write(sockfd, buffer, BUF_SIZE);
}
void move_left(int dist){
	get_pos();
	Pos.x +=dist;
	if((n = get_height((int)Pos.x,(int)Pos.z)) > Pos.y)
		Pos.y = (float)n;

	sprintf(buffer, "player.setPos(%f,%f,%f)\n", Pos.x, Pos.y, Pos.z);
	write(sockfd, buffer, BUF_SIZE);
}
void move_right(int dist){
	get_pos();
	Pos.x -=dist;
	if((n = get_height((int)Pos.x,(int)Pos.z)) > Pos.y)
		Pos.y = (float)n;

	sprintf(buffer, "player.setPos(%f,%f,%f)\n", Pos.x, Pos.y, Pos.z);
	write(sockfd, buffer, BUF_SIZE);
}



void move(int x,int y,int z){
	sprintf(buffer, "player.setTile(%d,%d,%d)\n",x,y,z);	
	write(sockfd, buffer, BUF_SIZE);
}

void get_pos(){
	char tmp[BUF_SIZE];
	
	if( write(sockfd, "player.getPos()\n", BUF_SIZE) < 0){
		return;
	}
	if((n = read(sockfd, tmp, BUF_SIZE)) < 0){
		printf("read failed");
		return;
	}

	token = strtok(tmp,",");
	Pos.x = atof(token);
	token = strtok(NULL,",");
	Pos.y = atof(token);
	token = strtok(NULL,",");
	Pos.z = atof(token); 
	
	printf("(%.3f,%.3f,%.3f)\n",Pos.x,Pos.y,Pos.z);
}

void get_tile(){
	char tmp[BUF_SIZE];

	write(sockfd, "player.getTile()\n", BUF_SIZE);
	n = read(sockfd, tmp, BUF_SIZE);

	token = strtok(tmp,",");
	x = atoi(token);
	token = strtok(NULL,",");
	y = atoi(token);
	token = strtok(NULL,",");
	z = atoi(token);

	printf("(%d,%d,%d)\n",x,y,z);
}

int get_height(int x, int z){
	char tmp[BUF_SIZE];
	sprintf(buffer,"world.getHeight(%d,%d)\n", x,z);
	write(sockfd, buffer, BUF_SIZE);
	printf("%s",buffer);
	read(sockfd, tmp, BUF_SIZE);

	printf("height: %s\n",tmp);
	if(strcmp("Fail\n", tmp) == 0){
		return Pos.y;
	}

	return atoi(tmp);
}

void reset_height(){
	//get_tile();//current working tile.
	if((n = get_height((int)Pos.x,(int)Pos.z)) > (int)Pos.y ){
		Pos.y = n;
		sprintf(buffer, "player.setPos(%f,%f,%f)\n", Pos.x, Pos.y, Pos.z);
		write(sockfd, buffer, BUF_SIZE);	
	}
}

int parse_arg(char* buffer){
	token = strtok(buffer," ");
	token = strtok(NULL,"\n");
	if(token !=NULL)
		return atoi(token);
	else
	 	return 1;
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

void touch(){
	int i,j,k;	
	get_tile();
	for(k=0; k < 5; k++){
			sprintf(buffer, "world.setBlock(%d,%d,%d,17,0)\n", x-1, y+k, z-1);
			write(sockfd, buffer, BUF_SIZE);
			sleep(1);
	}
	for(i=0; i < 5; i++){
		for(j=0; j < 5; j++){
			sprintf(buffer, "world.setBlock(%d,%d,%d,18,0)\n", x+i-3, y+5, z+j-3);
			write(sockfd, buffer, BUF_SIZE);
			sleep(1);
		}
	}
}

int get_id(int x, int y, int z){
	char tmp[4];
	sprintf(buffer, "world.getBlock(%d,%d,%d)\n", x, y, z);
	write(sockfd, buffer, BUF_SIZE);	
	read(sockfd, tmp, 4);
	printf("%s",tmp);
	return atoi(tmp);
}

