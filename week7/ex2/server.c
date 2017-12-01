#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/poll.h>
//#include <pthread.h>

#include "data.h"
#include "run.h"

#define BACKLOG 100   
#define BUFF_SIZE 2048
#define FILE_NAME "account.txt"

int user_count = 0;
int session_count = 0;



int main(int argc, char const *argv[]) {
	int listen_sock,conn_sock,fdmax,i;
	int bytes_received,bytes_sent;
	struct pollfd fds[BACKLOG];
	char buff[BUFF_SIZE];
	struct sockaddr_in *client; 
	struct sockaddr_in server; 
	char output[BUFF_SIZE];
	int bytes_output;
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

	fdmax = 1;
	fds[0].fd = listen_sock;
	fds[0].events = POLLIN;

	

	while(1){
		if (poll(fds, fdmax, -1) == -1) {
			perror("Error poll()");
			exit(1);
		}

		for (i = 0; i < fdmax; i++) {
			if (fds[i].revents != 0) {
				if (fds[i].fd == listen_sock) {
					if ((conn_sock = accept(listen_sock,(struct sockaddr *) client, &sin_size)) < 0) {
						perror("Error accept()");
					} 
					else {
						if (conn_sock > fdmax) {
							fdmax = conn_sock;
						}

						printf("Accepted\n");
						fds[conn_sock].fd = conn_sock;
						fds[conn_sock].events = POLLIN;
						fds[conn_sock].revents = 0;
						fdmax++;
						printf("Server: New connection from %s on socket %d\n",
								inet_ntoa(client->sin_addr), conn_sock);
						}
					}

				else if(fds[i].revents & POLLIN){
					
					if (createSession(client) == 0) {
							printf("Error: Max session!");
							continue;
						}
					//recieve data
					if ((bytes_received = recv(fds[i].fd, buff,BUFF_SIZE, 0)) <= 0) {
						if (bytes_received == 0)
							printf("Server: socket %d out\n", fds[i].fd);
						close(fds[i].fd);
						}
					else {
						buff[bytes_received]='\0';
						printf("Data form Socket %d : %s\n", fds[i].fd, buff);

						Message* msg = runCommand(client,buff);
						bytes_output = changeToFullMessage(msg,output,BUFF_SIZE);
						//send data
						bytes_sent =  send(fds[i].fd, output, bytes_output, 0);
						bzero(buff,BUFF_SIZE);
						}
					}
			}
		}

		close(conn_sock);

		removeSession(conn_sock);

		writeUserToFile(FILE_NAME);
	}

	close(listen_sock);
	return 0;
}

