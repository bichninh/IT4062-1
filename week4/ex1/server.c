#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define BACKLOG 2   
#define BUFF_SIZE 1024 //we use small size of buffer

int main(int argc, char const *argv[]){ //server recieve a string which have both letter and digit. server divide it to 2 string: digit only 
 					//and letter only -> send it to client. if string recieved has special char -> send client error.
	int listen_sock, conn_sock; 
	char recv_data[BUFF_SIZE];
	char strLength[BUFF_SIZE],*data,recvBuff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; 
	struct sockaddr_in client; 
	int sin_size;
	int nLeft,idx;
	int flag,i=0,j=0,k=0;
	int temp;

	if(argc != 2){
		printf("Error! Invalid parameter!\n");
		return 0;
	}
	
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  
		perror("\nError: ");
		return 0;
	}
	
	
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));   /* Remember htons() from "Conversions" section?*/
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	}     
	

	if(listen(listen_sock, BACKLOG) == -1){ 
		perror("\nError: ");
		return 0;
	}
	
	
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); 
		
		//start conversation
		while(1){
			//first, receives length of string from client
			bytes_received = recv(conn_sock,strLength,BUFF_SIZE,0);
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}
			strLength[bytes_received] = '\0';

			//send to client a confirm
			send(conn_sock,strLength,strlen(strLength),0);

			//receives message from client
			nLeft = atoi(strLength);
			data = (char*) malloc (nLeft);
			memset(data,0,nLeft);
			idx = 0;

			while(nLeft > 0){
				bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0);
				if (bytes_received <= 0){
					printf("\nConnection closed");
					break;
				}
			else{
				recv_data[bytes_received] = '\0';
				printf("\nReceived: %s ", recv_data);
				idx += bytes_received;
				strcat(data, recv_data);
				nLeft -= bytes_received;
				}
			}
			data[idx] = '\0';
			printf("\nData recieved: %s\n", data);
			temp = atoi(strLength);
			char letter[temp];//for storing letter char
			char digit[temp];//for stroring digit char

			for(i=0;i<strlen(data);i++){
				if(isalpha(data[i])){
					letter[j++] = data[i];
					flag  = 0;
				}
				else if(isdigit(data[i])){
					digit[k++] = data[i];
					flag = 0;
				}
				else {
					flag = 1;
					break;
				}
			}
			free(data);

			if(flag){
				strcpy(recvBuff,"Error!");
				bytes_sent = send(conn_sock,recvBuff,strlen(recvBuff),0);
					if (bytes_sent <= 0){
						printf("\nConnection closed");
						break;
					}
			}
			else{
				letter[j] = '\0';
				digit[k] = '\0';
				strcat(letter,"\n");
				strcat(letter,digit);
				bytes_sent = send(conn_sock, letter, strlen(letter), 0);
				if (bytes_sent <= 0){
					printf("\nConnection closed");
					break;
				}
			}
		}//end conversation
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}
