#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#define BUFF_SIZE 2048

#define DATA_LEN 30
#define OPCODE_LEN 4         


void handleLOUT (char *str);

int main(int argc, char const *argv[]) {

	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; 
	int msg_len, bytes_sent, bytes_received;

	if(argc != 3){
		printf("Invalid parameter!\n");
		return 0;
	}


	client_sock = socket(AF_INET,SOCK_STREAM,0);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));


	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0) {
		printf("\nError!Can not connect to sever! Client exit imediately!");
		return 0;
	}

	printf("Connection established. \n");

	while(1){
		__fpurge(stdin);
		printf("\nEnter a command to send: ");
		bzero(buff, BUFF_SIZE);
		fgets(buff, BUFF_SIZE, stdin);
		buff[strlen(buff) - 1] = '\0';

		handleLOUT(buff);
		msg_len = strlen(buff);

		bytes_sent = send(client_sock, buff, msg_len, 0);
		if(bytes_sent <= 0){
			printf("\nConnection closed!\n");
			break;
		}

		bytes_received = recv(client_sock, buff, BUFF_SIZE-1, 0);
		if(bytes_received <= 0){
			printf("\nError!Cannot receive data from sever!\n");
			break;
		}

		buff[bytes_received] = '\0';
		printf("\nReply from server: %s\n", buff);
		printf("********************************************************\n");
	}

	close(client_sock);
	return 0;
}

void handleLOUT (char *str) {
	char opcode[OPCODE_LEN + 1];
	char pass[DATA_LEN + 1];

	sscanf(str, "%s", opcode);

	if (strcmp(opcode, "LOUT") != 0) return;
	
	printf("Enter user's password: ");
	__fpurge(stdin);
	fgets(pass,DATA_LEN+1,stdin);

	strcat(str, " ");
	strncat(str, pass, DATA_LEN);
}
