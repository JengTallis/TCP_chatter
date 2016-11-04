#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define STR_LENGTH 80

char* exitCommand = "exit";

void *writeHandler(void* sock);
void *readHandler(void* sock);
char outbuff[STR_LENGTH], inbuff[STR_LENGTH];
int active;

int main(int argc, char const *argv[])
{
	 struct sockaddr_in sock_var;
	 int soc = socket(AF_INET, SOCK_STREAM, 0);
	 char name[STR_LENGTH];
	 active = 0;
	 
	 //sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
	 //sock_var.sin_port = 3000;
	 sock_var.sin_addr.s_addr = inet_addr(argv[1]);
	 sock_var.sin_port = atoi(argv[2]);
	 sock_var.sin_family = AF_INET;
	 strcpy(name,argv[3]);

	 pthread_t readThread, writeThread;
	 
	 if(connect(soc, (struct sockaddr*)&sock_var, sizeof(sock_var)) >= 0)
	 {
		active = 1;
		printf("%s Connected to server %d \n", name, soc);
		write(soc, name, STR_LENGTH);
		read(soc, inbuff, STR_LENGTH);
		printf("String from Server: %s \n", inbuff);
		//pthread_create(&readThread, NULL, readHandler, (void *)soc);
		pthread_create(&writeThread, NULL, writeHandler, (void *)soc);
	   	while(active){
			//read(soc, inbuff, STR_LENGTH);
			recv(soc, inbuff, STR_LENGTH, MSG_WAITALL);
			printf("String from Server: %s \n", inbuff);
			if(strncmp(inbuff, exitCommand, 4) == 0){
				active = 0;
				close(soc);
			}
		}
	 }
	 else{
		printf("socket creation failed \n");
	 }
	 return 0;
}

// void *readHandler(void* sock)
// {
//    int soc = (int)sock;
//    while(active){
// 		read(soc, inbuff, STR_LENGTH);
// 		printf("String from Server: %s \n", inbuff);
// 		if(strncmp(inbuff, exitCommand, 4) == 0){
// 			active = 0;
// 			close(soc);
// 		}
// 	}
//    return 0;
// }

void *writeHandler(void* sock)
{
   int soc = (int)sock;
   while(active){
		fgets(outbuff, STR_LENGTH-1, stdin);
		write(soc, outbuff, STR_LENGTH);
	}
   return 0;
}