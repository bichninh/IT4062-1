/*UDP Echo Client*/
#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
//#define SERV_PORT 5550
//#define SERV_IP "127.0.0.1"
#define BUFF_SIZE 1024
//DNS example: get a IP or domain name from client and server return information about this IP/domain name

int main(int argc, char const *argv[]){

	int client_sock;
	char buff[BUFF_SIZE];
	char temp[BUFF_SIZE]; //a copy a buff to check buff = '\0'
	struct sockaddr_in server_addr;
	int bytes_sent,bytes_received, sin_size;
	
	if(argc != 3) {
		printf("Error! Invalid parameter!\n");
		return 0;
	}

	else{ //argc == 3 with argv[1] is the server IP and argv[2] is the server PORT 
		// type command ./client server-IP server-PORT in direct way to run
	//Step 1: Construct a UDP socket
		if ((client_sock=socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){  /* calls socket() */
			perror("\nError: ");
			exit(0);
	}

	//Step 2: Define the address of the server with argv[1] is the server IP and argv[2] is the server PORT 
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	server_addr.sin_port = htons(atoi(argv[2]));
	
	
	//Step 3: Communicate with server
	while(1){
	printf("\nInsert a IP address or a domain name to send: ");
	memset(buff,'\0',(strlen(buff)+1));
	
	fgets(buff, BUFF_SIZE, stdin);
	buff[strlen(buff) - 1] = '\0';
	
	
	strcpy(temp,buff);
	temp[strlen(temp) - 1] = '\0';

	if ( strcmp(temp,"\0") == 0 ){
		close(client_sock);
		return 0;
	}
	
	if( atoi(buff) == 0 ){
		printf("\nMaybe that you've entered a domain name.\n");
	}	
	else printf("\nMaybe that you've entered an IP address.\n");

	sin_size = sizeof(struct sockaddr);
	
	bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *) &server_addr, sin_size);
	if(bytes_sent < 0){
		perror("Error: ");
		close(client_sock);
		return 0;
	}

	bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *) &server_addr, &sin_size);
	if(bytes_received < 0){
		perror("Error: ");
		close(client_sock);
		return 0;
	}

	buff[bytes_received] = '\0';
	printf("\nReply from server: %s", buff);
	}

	close(client_sock);
	return 0;
	}
}