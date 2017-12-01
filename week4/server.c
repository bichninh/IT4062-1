#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>


// #define PORT 5550   /* Port that will be opened */ 
#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 4

int main(int argc, char const *argv[])
{	
	if (argc != 2) return -1;
 
 	int PORT = atoi(argv[1]);
	int listen_sock, conn_sock; /* file descriptors */
	char recv_data[BUFF_SIZE], leng[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size, i = 0, j = 0, k = 0, nLeft = 0, check = 1;
	char *data;
	char c;

	
	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);   /* Remember htons() from "Conversions" section? =) */
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

		//receive length of text
		bytes_received = recv(conn_sock, leng, BUFF_SIZE, 0); 
		if (bytes_received <= 0){
			printf("\nConnection closed");
			break;
		}
		leng[bytes_received ] = '\0';
		nLeft = atoi(leng);
		nLeft--; //?
		send(conn_sock, leng, strlen(leng) , 0);

		//start conversation
		data = (char*)malloc(nLeft * sizeof(char));
		while(nLeft > 0){
			bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0); //blocking
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}else{
				recv_data[bytes_received] = '\0';
				printf("\nReceive: -%s- ", recv_data);
				i += bytes_received;
				strcat(data, recv_data);
				nLeft -= bytes_received;
			}
		}
		//echo to client
		*(data + (i)*sizeof(char)) = '\0';
		printf("data:%s-\n",data );
		char data_string[atoi(leng)];
		char data_number[atoi(leng)];
		i = 0; j=0 ; k=0;
		c = *(data + i*sizeof(char));
		while(c != '\0'){
			if(isalpha(c)){
				data_string[j++] = c;
			}else if(isdigit(c)){
				data_number[k++] = c;
			}else{
				check = 0;
				break;
			}
			i++;
			c = *(data + i*sizeof(char));
		}
		free(data);
		//text content invalid
		if(check == 0){
			strcpy(recv_data, "Error!");
			bytes_sent = send(conn_sock, recv_data, strlen(recv_data) , 0);
			check = 1;
			if (bytes_sent <= 0){
				printf("\nConnection closed");
				break;
			}
		}else{
			//valid
			data_string[j] = '\0';
			data_number[k] = '\0';
			strcat(data_string,"\n");
			strcat(data_string,data_number);
			bytes_sent = send(conn_sock, data_string, strlen(data_string) , 0);
			if (bytes_sent <= 0){
				printf("\nConnection closed");
				break;
			}
		}
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}