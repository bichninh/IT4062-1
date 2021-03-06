#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 102400
#define STRING_SIZE 819600

char *recv_msg(int conn_sock, int *errnum, int *msg_len){
	int ret, nLeft, index = 0;
	char recv_data[BUFF_SIZE], *data;

	// receive the length of message
	int bytes_received = recv(conn_sock, &(*msg_len), sizeof(int), MSG_WAITALL);
	if (bytes_received <= 0){
		*errnum = -1;
		return NULL;
	}

	if(*msg_len > 0){
		nLeft = *msg_len;
		data = (char*) malloc(*msg_len * sizeof(char));
		memset(data, 0, *msg_len);
		index = 0;
		
		//receives message from client
		while(nLeft > 0){
			ret = recv(conn_sock, recv_data, BUFF_SIZE, 0);
			if(ret == -1){
				*errnum = -1;
				return NULL;
			}
			memcpy(data + index, recv_data, ret);
			index += ret;
			nLeft -= ret;
		}
		data[*msg_len] = '\0';	
		*errnum = 0;
	}else{
		printf("\nSuccessful transfering\n");
		*errnum = 0;
		return NULL;
	}
	
	return data;
}

int send_msg(int conn_sock, char *message){
	int msg_len, bytes_sent;
	//send the length of the message to client
	msg_len = strlen(message);
	bytes_sent = send(conn_sock, &msg_len, sizeof(int), 0);
	if(bytes_sent <= 0){
		return -1;
	}

	// send message to client
	bytes_sent = send(conn_sock, message, msg_len, 0);
	if (bytes_sent <= 0){
		return -1;
	}
	return 0;
}

void welcome(){
	printf("TCP File Transfer Application\n");
	printf("Nguyen Hoang Anh - 20130135\n");
}

int main(int argc, char* argv[])
{
 	int msg_len;
	int listen_sock, conn_sock; /* file descriptors */
	int errnum;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	int serv_port;
	char *endptr, *data;
	char error_message[STRING_SIZE];
	FILE *fp = NULL;
	double bytes_tranfered = 0;

	welcome();
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
			// Step 5: Receive filename from client
			if((data = recv_msg(conn_sock, &errnum, &msg_len)) ){
				printf("Filename : %s\n", data);
				// if file already exists on the server
				if((fp = fopen(data, "rb"))){
					strcpy(error_message, "1");
					send_msg(conn_sock, error_message);
					continue;
				}else{// if not
					fp = fopen(data, "wb+");
					strcpy(error_message, "0");
					send_msg(conn_sock, error_message);
					bytes_tranfered = 0;
					
					while(strcmp(error_message, "0") == 0){		// while there's no error
						data = recv_msg(conn_sock, &errnum, &msg_len);
						if(data != NULL && errnum == 0){		// if file content is received
							printf("Received: %.2lf MB\n", (bytes_tranfered += msg_len) / (1024 * 1024));
							fwrite(data, msg_len, 1, fp);
						}else if(errnum == 0){		// if file reached EOF
							strcpy(error_message, "-1");
							fclose(fp);
						}else{		// if file transfering is interupted
							strcpy(error_message, "2");
							fclose(fp);
						}
						send_msg(conn_sock, error_message);
					}
				}	
			}else{
				break;
			}
		}//end conversation
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}
