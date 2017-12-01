#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "data.h"
#include "run.h"

#define BACKLOG 100   
#define BUFF_SIZE 2048
#define FILE_NAME "account.txt"

int user_count = 0;
int session_count = 0;


void *echo(void *);//handle multi-thread

//=================================MAIN============================
int main(int argc, char const *argv[]) {
	int listen_sock; 
	int *conn_sock;
	struct sockaddr_in *client; 
	struct sockaddr_in server; 
	pthread_t tid;
	int sin_size;

	if(argc != 2){
		printf("Invalid parameter!\n");
		return 0;
	}

	readUserFromFile(FILE_NAME);  //read user infor
	
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  
		perror("\nError: ");
		return 0;
	}
	
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(argv[1]));   
	server.sin_addr.s_addr = htonl(INADDR_ANY);  
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ 
		perror("\nError: ");
		return 0;
	}

	if(listen(listen_sock, BACKLOG) == -1){  
		perror("\nError: ");
		return 0;
	}

	sin_size = sizeof(struct sockaddr_in);
	client = malloc(sin_size);

	//start connect
	while(1){
		conn_sock = malloc(sizeof(int));
		if ((*conn_sock = accept(listen_sock,( struct sockaddr *)client, &sin_size)) == -1)
			perror("\nError: ");

		printf("You got a connection from %s\n", inet_ntoa(client->sin_addr) ); 
		pthread_create(&tid, NULL, &echo, conn_sock); //create multi-thread
	}
	close(listen_sock);
	return 0;
}


void *echo(void *arg){
	int conn_sock;
	int bytes_sent, bytes_received;
	char buff[BUFF_SIZE];
	char output[BUFF_SIZE];
	int bytes_output;

	conn_sock = *((int *) arg);
	if (createSession(conn_sock) == 0) {
		printf("Error: Max session!");
	}
	free(arg);
	pthread_detach(pthread_self());
	
	while(1){

	bytes_received = recv(conn_sock, buff, BUFF_SIZE-1, 0); 
	if (bytes_received < 0)
		perror("\nError: ");
	else if (bytes_received == 0)
		printf("Connection closed.");
	else{
		buff[bytes_received] = '\0';		//recieve mess from client	
		printf("\nReceived: '%s'\n", buff);
		Message *msg = runCommand(conn_sock, buff); //handle mess
		bytes_output = changeToFullMessage(msg, output, BUFF_SIZE); //convert to full mess

	}


	bytes_sent = send(conn_sock, output, bytes_output, 0); //send to client
	if (bytes_sent < 0){
		perror("\nError: ");
		}
	}
	close(conn_sock);
	removeSession(conn_sock);
	writeUserToFile(FILE_NAME);	
}

