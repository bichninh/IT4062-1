#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 5
#define STRING_SIZE 8196

int main(int argc, char* argv[])
{
 
	int listen_sock, conn_sock; /* file descriptors */
	char recv_data[BUFF_SIZE], send_data_char[STRING_SIZE], send_data_num[STRING_SIZE], *data;
	int bytes_sent, bytes_received;
	int ret, nLeft, msg_len, index = 0;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	int serv_port;
	char *endptr;
	int i = 0, j = 0, k = 0;
	int flag = 0;
	char error_message[] = "Error: Invalid character found!";
	
	// Step 0: Initialization
	if(argc != 2){
		printf("Invalid arguments\n");
		exit(-1);
	}
	serv_port = (in_port_t) strtol(argv[1], &endptr, 10);
	if(strlen(endptr) != 0){
		printf("Invalid port!\n");
		exit(-1);
	}

	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(serv_port);   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	}     
	
	//Step 3: Listen request from client
	if(listen(listen_sock, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		//start conversation
		while(1){
			memset(send_data_num, '\0', STRING_SIZE);
			memset(send_data_char, '\0', STRING_SIZE);
			i = j = k = 0;
			flag = 0;

			// receive the length of message
			bytes_received = recv(conn_sock, &msg_len, sizeof(int), MSG_WAITALL);
			if (bytes_received <= 0){
					printf("\nConnection closed");
					break;
			}
			nLeft = msg_len;
			data = (char*) malloc(msg_len);
			memset(data, 0, msg_len);
			index = 0;
			
			//receives message from client
			while(nLeft > 0){
				ret = recv(conn_sock, recv_data, BUFF_SIZE, 0);
				if(ret == -1){
					printf("\nConnection closed");
					break;
				}
				memcpy(data + index, recv_data, ret);
				index += ret;
				nLeft -= ret;
			}
			data[msg_len] = '\0';
			
			for(i = 0; i < msg_len; i++){
				if(data[i] >= '0' && data[i] <= '9'){
					send_data_num[j++] = data[i];
				}else if((data[i] >= 'a' && data[i] <= 'z') || (data[i] >= 'A' && data[i] <= 'Z')){
					send_data_char[k++] = data[i];
				}else{
					flag = 1;
					break;
				}
			}
			send_data_num[j] = '\n';
			send_data_char[k] = '\n';

			//echo to client
			if(flag == 1){
				//send the length of the message to client
				msg_len = strlen(error_message);
				bytes_sent = send(conn_sock, &msg_len, sizeof(int), 0);
				if(bytes_sent <= 0){
					printf("\nConnection closed!\n");
					break;
				}

				// send to the client error message
				bytes_sent = send(conn_sock, error_message, msg_len, 0);
				if (bytes_sent <= 0){
					printf("\nConnection closed");
					break;
				}
				printf("Error!\n");
			}else{
				// send the length of the message to client
				msg_len = strlen(send_data_num) + strlen(send_data_char);
				bytes_sent = send(conn_sock, &msg_len, sizeof(int), 0);
				if(bytes_sent <= 0){
					printf("\nConnection closed!\n");
					break;
				}
				printf("Step 1\n");
				// send to the client number string
				bytes_sent = send(conn_sock, send_data_num, strlen(send_data_num), 0); 
				if (bytes_sent <= 0){
					printf("\nConnection closed");
					break;
				}
				printf("Step 2\n");
				
				// send to the client character string
				bytes_sent = send(conn_sock, send_data_char, strlen(send_data_char), 0); 
				if (bytes_sent <= 0){
					printf("\nConnection closed");
					break;
				}
				printf("Step 3\n");
			}
			
		}//end conversation
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}
