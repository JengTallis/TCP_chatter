#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define STR_LENGTH 80

char* exitCommand = "exit";

int main(int argc, char const *argv[])
{
	 struct sockaddr_in sock_var;
	 int soc = socket(AF_INET,SOCK_STREAM,0);
	 char name[STR_LENGTH];
	 char outbuff[STR_LENGTH],inbuff[STR_LENGTH];
	 int active = 0;
	 
	 //sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
	 //sock_var.sin_port = 3000;
	 sock_var.sin_addr.s_addr = inet_addr(argv[1]);
	 sock_var.sin_port = atoi(argv[2]);
	 sock_var.sin_family = AF_INET;
	 strcpy(name,argv[3]);
	 
	 if(connect(soc, (struct sockaddr*)&sock_var, sizeof(sock_var)) >= 0)
	 {
		active = 1;
		printf("%s Connected to server %d \n", name, soc);
		write(soc, name, STR_LENGTH);
		read(soc, inbuff, STR_LENGTH);
		printf("String from Server: %s \n", inbuff);
		while(active){
			printf("Enter string to send\n");
			fgets(outbuff, STR_LENGTH-1, stdin);
			write(soc, outbuff, STR_LENGTH);
			read(soc, inbuff, STR_LENGTH);
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