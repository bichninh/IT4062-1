#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "data.h"
#include "run.h"

#define FILE_NAME "account.txt"

#define BACKLOG 100   
#define BUFF_SIZE 2048
struct sockaddr_in *client; 

int user_count = 0;
int session_count = 0;


void *echo(void *);
int main(int argc, char const *argv[]) {
	
	int listen_sock, *conn_sock; 
	char recv_data[BUFF_SIZE];
	//int bytes_sent, bytes_received;
	struct sockaddr_in server; 
	pthread_t tid;
	int sin_size;

	if(argc != 2){
		printf("Invalid parameter!\n");
		return 0;
	}

	readUserFromFile(FILE_NAME);
	
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

	while(1){
		conn_sock = malloc(sizeof(int));
		if ((*conn_sock = accept(listen_sock,( struct sockaddr *)client, &sin_size)) == -1)
			perror("\nError: ");

		if (createSession(client) == 0) {
			printf("Error: Max session!");
			continue;
		}

		printf("You got a connection from %s\n", inet_ntoa(client->sin_addr) ); 
		pthread_create(&tid, NULL, &echo, (void*)conn_sock);

		/*while(1){
			//receives message from client
			bytes_received = recv(conn_sock, recv_data, BUFF_SIZE - 1, 0); 
			if (bytes_received <= 0) {
				printf("\nConnection closed\n");
				break;
			} else {
				recv_data[bytes_received] = '\0';
				printf("\nReceived: '%s'\n", recv_data);

				Message *msg = runCommand(client, recv_data);
				bytes_output = changeToFullMessage(msg, output, BUFF_SIZE);
			}
			
			//echo to client
			bytes_sent = send(conn_sock, output, bytes_output, 0); 
			if (bytes_sent <= 0){
				printf("\nConnection closed\n");
				break;
			}
		}*/
		//close(conn_sock);

		//removeSession(client);

		//writeUserToFile(FILE_NAME);
	}

	close(listen_sock);
	return 0;
}
void *echo(void *arg){
	int conn_sock;
	int bytes_sent, bytes_received;
	char buff[BUFF_SIZE + 1];
	char output[BUFF_SIZE];
	int bytes_output;

	conn_sock = *((int *) arg);
	free(arg);
	pthread_detach(pthread_self());
	
	while(1){
	bytes_received = recv(conn_sock, buff, BUFF_SIZE, 0); //blocking
	if (bytes_received < 0)
		perror("\nError: ");
	else if (bytes_received == 0)
		printf("Connection closed.");
	else{
		buff[bytes_received] = '\0';			
		printf("\nReceived: '%s'\n", buff);
		Message *msg = runCommand(client, buff);
		bytes_output = changeToFullMessage(msg, output, BUFF_SIZE);

	}
	bytes_sent = send(conn_sock, output, bytes_output, 0); /* send to the client welcome message */
	if (bytes_sent < 0){
		perror("\nError: ");
		}
	}
	close(conn_sock);
	removeSession(client);
	writeUserToFile(FILE_NAME);	
}

