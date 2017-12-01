#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAX_RECV_BUFF 256
#define MAX_SEND_BUFF 256 



int sendFile(int sock, char*file_name){
	int sent_count; /* how many sending chunks, for debugging */
	ssize_t read_bytes, /* bytes read from local file */
		sent_bytes, /* bytes sent to connected socket */
		sent_file_size;
	char send_buf[MAX_SEND_BUFF]; /* max chunk size for sending file */
	char * errmsg_notfound = "File not found\n";
	int file; /* file handle for reading local file*/
	sent_count = 0;
	sent_file_size = 0;
	/* attempt to open requested file for reading */
	if( (file = open(file_name, O_RDONLY)) < 0){
		perror(file_name);
		if( (sent_bytes = send(sock, errmsg_notfound , strlen(errmsg_notfound), 0)) < 0 ){
			perror("Send error");
			return -1;
		}
	}
	else{ /* open file successful */
		printf("Sending file: %s\n", file_name);
		while( (read_bytes = read(file, send_buf, MAX_RECV_BUFF)) > 0 ){
			if( (sent_bytes = send(sock, send_buf, read_bytes, 0))< read_bytes ){
				perror("Send error.");
				return -1;
			}
		sent_count++;
		sent_file_size += sent_bytes;
		}
	close(file);
	}

	printf("Sent %d bytes in %d time(s)\n",sent_file_size, sent_count);
	return sent_count;
}

int main(int argc, char const *argv[]){
	int client_sock;
	char file_name[MAX_RECV_BUFF];
	struct sockaddr_in server_addr; /* server's address information */
	

	
	if(argc != 3){
		printf("Error! Invalid parameter!\n");
		return 0;
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
	//while(1){ //send file 
		printf("Enter link of the file you need to send in direct way:\n");
		fgets(file_name,MAX_SEND_BUFF,stdin);
		file_name[strlen(file_name) -1] = '\0';
		
		sendFile(client_sock, file_name);
		printf("Closing connection\n");
	//}
	//Step 4: Close socket
	close(client_sock);
	return 0;
}