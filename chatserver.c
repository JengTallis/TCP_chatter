#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_CLIENTS 24 /* max num of clients */
#define STR_LENGTH 80

char* broadcastCommand = "broadcast ";
char* listCommand = "list";
char* exitCommand = "exit";
int active[MAX_CLIENTS + 10];
int named[MAX_CLIENTS + 10];
char* names[MAX_CLIENTS + 10];

void *clientHandler(void* clientID);
char* parseStr(char* name, char* buffer);
void broadcast(char* outbuff);
void sendTo(char* outbuff, char* name, int sender);

int main(int argc, char const *argv[])
{
   struct sockaddr_in sock_var;
   int serverSoc = socket(AF_INET,SOCK_STREAM,0);
   int clientSoc;
   int i;
   pthread_t t[MAX_CLIENTS]; 

   memset(active, 0, sizeof(active));
   
   sock_var.sin_addr.s_addr = inet_addr("127.0.0.1");
   //sock_var.sin_port = 3000;
   sock_var.sin_port = atoi(argv[1]);
   sock_var.sin_family=AF_INET;
   if(bind(serverSoc,(struct sockaddr*)&sock_var,sizeof(sock_var))>=0)
   {
	printf("N socket has been created \n");
	listen(serverSoc,0); 
	while(1)
	{
		for(i = 0; i < MAX_CLIENTS; i++)
		{
			clientSoc = accept(serverSoc, NULL, NULL);
			printf("Connected to client %d \n",clientSoc);
			pthread_create(&t[i], NULL, clientHandler, (void *)clientSoc);
			active[clientSoc] = 1;
			named[clientSoc] = 0;
		}
	}
	close(serverSoc);
   }
   else{
		printf("N socket creation failed \n");
   }
   return 0;
}

void *clientHandler(void* clientID)
{
   int clientSoc = (int)clientID;
   char inbuff[STR_LENGTH],outbuff[3 * STR_LENGTH];
   char parsebuff[2 * STR_LENGTH];
   while(active[clientSoc]){
   	   memset(inbuff, 0, sizeof(inbuff));
   	   memset(outbuff, 0, sizeof(outbuff));
	   read(clientSoc, inbuff, STR_LENGTH);
	   if(named[clientSoc] == 0){
	   		printf("new client %d: %s \n", clientSoc, inbuff);
	   		char nameBuffer[STR_LENGTH];
	   		strcpy(nameBuffer, inbuff);
	   		names[clientSoc] = nameBuffer;
			named[clientSoc] = 1;
			printf("client %d is %s\n", clientSoc, names[clientSoc]);
			strcpy(outbuff, "Hello ");
			strcat(outbuff, nameBuffer);
			write(clientSoc, outbuff, STR_LENGTH);
	   }else{
	   		printf("read from client %d %s: %s \n", clientSoc, names[clientSoc], inbuff);
		   	if(strncmp(inbuff, exitCommand, 4) == 0){
		   		write(clientSoc, inbuff, STR_LENGTH);
		   		printf("reply to client %d %s: %s\n", clientSoc, names[clientSoc], inbuff);
		   		printf("closing client %d %s\n", clientSoc, names[clientSoc]);
		   		printf("\n");
		   		named[clientSoc] = 0;
		   		active[clientSoc] = 0;
		   		close(clientSoc);
		   }else{
		   		char msgbuff[STR_LENGTH];
		   		// Broadcast msg
		   		if(strncmp(inbuff, broadcastCommand, 9) == 0)
		   		{
		  			strncpy(msgbuff, inbuff + 10, strlen(inbuff) - 10);
		  			strcpy(outbuff, "Broadcast from ");
					strcat(outbuff, names[clientSoc]);
					strcat(outbuff, " : ");
					strcat(outbuff, msgbuff);
		   			broadcast(outbuff);
		   		}
		   		// Send to specific client
		   		else
		   		{
		   			char parsebuff[STR_LENGTH];
		   			strcpy(parsebuff, inbuff);
		   			char* recName = strtok(parsebuff, " ");
		   			int len = strlen(recName) + 1;
		   			strncpy(msgbuff, inbuff + len, strlen(inbuff) - len);
		   			char name[STR_LENGTH];
		   			strcpy(name, recName);
		   			strcpy(outbuff, "message from ");
					strcat(outbuff, names[clientSoc]);
					strcat(outbuff, " : ");
					strcat(outbuff, msgbuff);
		   			sendTo(outbuff, recName, clientSoc);
		   		}
		   		// write(clientSoc, inbuff, STR_LENGTH);
		   		// printf("reply to client %d %s:", clientSoc, names[clientSoc]);
		   		printf("\n");
		   }
	   }  
   } 
   return 0;
}

void broadcast(char* outbuff){
	int i;
	for(i = 0; i < MAX_CLIENTS + 10; i++){
		if(active[i] > 0){
			write(i, outbuff, 2 * STR_LENGTH);
		}
	}
	return;
}

void sendTo(char* outbuff, char* name, int sender){
	int i;
	char strbuff[2 * STR_LENGTH];
	for(i = 0; i < MAX_CLIENTS + 10; i++){
		if((named[i] > 0) && (strcmp(name, names[i]) == 0)){
			write(i, outbuff, 2 * STR_LENGTH);
			strcpy(strbuff, "Message sent.");
			write(sender, strbuff, STR_LENGTH);
			return;
		}
	}
	strcpy(strbuff, "Invalid command!");
	write(sender, strbuff, STR_LENGTH);
	return;
}
