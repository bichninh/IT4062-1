#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "my_socket.h"

void getString(char *string){
    fgets(string,100,stdin);
    string[strlen(string)-1] ='\0';
}

void error(char *msg){
	perror(msg);
	exit(0);
}

int sendMessege(int client_sock,char *request){

	int i = 0, j = 0,n;
	message outgoing_message;
	char type[BUFF_SIZE];
	char data[BUFF_SIZE];
	while(request[i] != '\0'){
		while(request[i] != ' '){
			type[j] = request[i];
			i++;
			j++;
		}
		i++;
		type[j] = '\0';
		//printf("%s\n",type );
		if(strcmp(type,"USER") == 0)
			outgoing_message.type = USER;
		else if(strcmp(type,"PASS") == 0)
			outgoing_message.type = PASS;
		else if(strcmp(type,"LOUT") == 0)
			outgoing_message.type = LOUT;
		else
			return 0;
		j = 0;

		while(request[i] != '\0'){
			data[j] = request[i];
			i++;
			j++;
		}
		data[j] = '\0';
		//printf("%s\n",data );
		memcpy((void*)outgoing_message.payload, (void*)&data, sizeof(data));

		n = send(client_sock,(char *)&outgoing_message,sizeof(outgoing_message),0);
		if (n < 0){
			error("Sendto");
		}
	}

	return 1;
}

int main(int argc, char const *argv[])
{
	int client_sock;
	char request[BUFF_SIZE],ret[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int bytes_received;

	if (argc != 3){
	printf("Usage: server port\n");
	exit(1);
	}
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}

	//Step 4: Communicate with server			
	//send message
	while(1){
		printf("Please enter:(USER/PASS/LOUT) USER_NAME/PASSWORD/USER_NAME:\n");
		getString(request);

		if(sendMessege(client_sock,request)){

			bytes_received = recv(client_sock, ret, BUFF_SIZE-1, 0);
			if(bytes_received <= 0){
				printf("\nError!Cannot receive data from sever!\n");
				exit(1);
			}
			ret[bytes_received] = '\0';
			printf("%s\n",ret );
			if(strcmp(ret,"00") == 0)
				printf("%s\n","IDENTIFIED USER:" );
			else if(strcmp(ret,"01") == 0)
				printf("%s\n","NOT IDENTIFIED USER:" );
			else if(strcmp(ret,"10") == 0)
				printf("%s\n","AUTHENTICATED:" );
			else if(strcmp(ret,"11") == 0)
				printf("%s\n","NOT AUTHENTICATED:" );
			else if(strcmp(ret,"12") == 0)
				printf("%s\n","CAN NOT LOGIN FOR 2 USER:" );
			else if(strcmp(ret,"21") == 0)
				printf("%s\n","CAN NOT LOG OUT:" );
			else if(strcmp(ret,"31") == 0){
				printf("%s\n","BLOCK:" );
				break;
			}
			if(strcmp(ret,"20") == 0)
				break;
		}
		else
			printf("%s\n","SYNTAX ERROR:" );
	}
	//close(client_sock);
	return 0;
}
