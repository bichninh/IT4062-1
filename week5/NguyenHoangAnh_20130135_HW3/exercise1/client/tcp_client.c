#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define STRING_SIZE 8196
#define BUFF_SIZE 5

int main(int argc, char* argv[]){
	int client_sock;
	char buff[STRING_SIZE], *data;
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	int ret, nLeft, index = 0;
	int serv_port = 0;
	char serv_ip[16];
	char *endptr;
	
	// Step 0: Initialization
	if(argc != 3){
		printf("Invalid arguments!\n");
		exit(-1);
	}

	strcpy(serv_ip, argv[1]);
	serv_port = (in_port_t) strtol(argv[2], &endptr, 10);
	if(strlen(endptr) != 0){
		printf("Invalid port!\n");
		exit(-1);
	}

	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serv_port);
	server_addr.sin_addr.s_addr = inet_addr(serv_ip);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
	//Step 4: Communicate with server			
	while(1){
		//send message
		printf("\nInsert string to send:");
		memset(buff,'\0', STRING_SIZE);
		fgets(buff, STRING_SIZE, stdin);
		buff[strlen(buff) - 1] = '\0';
		msg_len = strlen(buff);
		if (msg_len == 0) break;
		
		// send the length of message to server
		bytes_sent = send(client_sock, &msg_len, sizeof(int), 0);
		if(bytes_sent <= 0){
			printf("\n1. Connection closed!\n");
			break;
		}
		// send messages to server
		bytes_sent = send(client_sock, buff, msg_len, 0);
		if(bytes_sent <= 0){
			printf("\n2. Connection closed!\n");
			break;
		}
		
		// receive the length of message
		bytes_received = recv(client_sock, &msg_len, sizeof(int), MSG_WAITALL);
		if (bytes_received <= 0){
				printf("\n3. Connection closed");
				break;
		}
		nLeft = msg_len;
		data = (char*) malloc(msg_len);
		memset(data, 0, msg_len);
		index = 0;

		//receives message from server
		while(nLeft > 0){
			ret = recv(client_sock, buff, BUFF_SIZE, 0);
			if(ret == -1){
				printf("\n4. Connection closed");
				break;
			}
			memcpy(data + index, buff, ret);
			index += ret;
			nLeft -= ret;
		}
		data[msg_len] = '\0';
		printf("%s\n", data);
	}
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
