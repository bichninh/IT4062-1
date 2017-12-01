#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define BACKLOG 2   /* Number of allowed connections */
#define MAX_RECV_BUFF 256
#define MAX_SEND_BUFF 256
#define FILE_STORE "foo" //file that will be write to.

 int recvFile(int sock, char*file_name){
 	//char send_str [MAX_SEND_BUFF]; /* message to be sent to server*/
	int file; /* file handle for receiving file*/
	ssize_t /*sent_bytes, */rcvd_bytes, rcvd_file_size;
	int recv_count; /* count of recv() calls*/
	char recv_str[MAX_RECV_BUFF]; /* buffer to hold received data */
	//size_t send_strlen; /* length of transmitted string */

	//sprintf(send_str, "%s\n", file_name); /* add CR/LF (new line) */
	//send_strlen = strlen(send_str); /* length of message to be transmitted */
	/*if( (sent_bytes = send(sock, file_name, send_strlen, 0)) < 0 ) {
		perror("Send error");
		return -1;
}*/
/* attempt to create file to save received data. 0644 = rw-r--r-- */
	if ( (file = open(file_name, O_WRONLY | O_CREAT, 0644)) < 0 ){
		perror("Error creating file");
		return -1;
}
	recv_count = 0; /* number of recv() calls required to receive the file */
	rcvd_file_size = 0; /* size of received file */
	/* continue receiving until ? (data or close) */
	while ( (rcvd_bytes = recv(sock, recv_str, MAX_RECV_BUFF, 0)) > 0 ){
		recv_count++;
		rcvd_file_size += rcvd_bytes;
	if (write(file, recv_str, rcvd_bytes) < 0 ){//write to file
		perror("Error writing to file");
		return -1;
		}
	}
	close(file);
	printf("Received: %d bytes in %d time(s)\n", rcvd_file_size,recv_count);
	return rcvd_file_size;
 }

int main(int argc, char const *argv[]){
 
	int listen_sock, conn_sock; /* file descriptors */
	//char recv_data[BUFF_SIZE];
	//int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	
	if(argc != 2){
		printf("Error! Invalid parameter!\n");
		return 0;
	}
	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));   /* Remember htons() from "Conversions" section? =) */
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
		//while(1){
			recvFile(conn_sock,FILE_STORE);
		//}//end conversation
		close(conn_sock);	
	}
	close(listen_sock);
	return 0;
}