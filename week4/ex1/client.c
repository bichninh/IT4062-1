#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFF_SIZE 8192

int main(int argc, char const *argv[]){//client enter a string include both digit and letter and send it to server

	int client_sock;
	char buff[BUFF_SIZE];
	char strLength[BUFF_SIZE];
	struct sockaddr_in server_addr; 
	int msg_len, bytes_sent, bytes_received;
	
	if(argc != 3){
		printf("Error! Invalid parameter!\n");
		return 0;
	}


	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));
	
	
	
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
				
		//Start
		printf("\nInsert a string with both letter and digit to send:");
		memset(buff,'\0',(strlen(buff)+1));
		fgets(buff, BUFF_SIZE, stdin);
		buff[strlen(buff) -1] = '\0';

		msg_len = strlen(buff);
		if (msg_len == 0) return 0;
		
		sprintf(strLength,"%d",msg_len);//convert msg_len from int to string and put it to length
		bytes_sent = send(client_sock,strLength,strlen(strLength),0); //first, send to server length of string
		if(bytes_sent <= 0){
			printf("\nConnection closed!\n");
			return 0;
		}

		//recieve confirm from server
		recv(client_sock,strLength,BUFF_SIZE,0);


		bytes_sent = send(client_sock, buff, msg_len, 0);
		if(bytes_sent <= 0){
			printf("\nConnection closed!\n");
			return 0;
		}
		
		//receive result from server
		bytes_received = recv(client_sock, buff, BUFF_SIZE-1, 0);
		if(bytes_received <= 0){
			printf("\nError!Cannot receive data from sever!\n");
			return 0;
		}
		
		buff[bytes_received] = '\0';
		printf("Reply from server: %s", buff);

	close(client_sock);
	return 0;
}