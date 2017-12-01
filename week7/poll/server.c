#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 5550
#define BACKLOG 30
#define BUFF_SIZE 1024


int main(int argc, char *argv[]) {
	char buff[BUFF_SIZE];
	struct pollfd fds[BACKLOG];
	struct sockaddr_in server_addr, client_addr;
	int sin_size = sizeof(client_addr);
	int listen_sock, fdmax, newfd,nbytes,i;
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) <0) {
		perror("Error socket()");
		exit(1);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	if (bind(listen_sock, (struct sockaddr *) &server_addr, sizeof(server_addr))<0) {
		perror("Error bind()");
		exit(1);
	}
	//printf("Server-bind() is OK...\n");
	if (listen(listen_sock, BACKLOG) == -1) {
		perror("Error listen()");
		exit(1);
	}
	printf("Server-listen() is OK...\n");
	fdmax = 1;
	fds[0].fd = listen_sock;
	fds[0].events = POLLIN;
	while(1) {
		if (poll(fds, fdmax, -1) == -1) {
			perror("Error poll()");
			exit(1);
		}
		for (i = 0; i < fdmax; i++) {
			if (fds[i].revents != 0) {
				if (fds[i].fd == listen_sock) {
					if ((newfd = accept(listen_sock,
						(struct sockaddr *) &client_addr,
						(socklen_t*) &sin_size)) < 0) {
						perror("Error accept()");
					} 
					else {
						if (newfd > fdmax) {
							fdmax = newfd;
						}
						printf("Accepted\n");
						fds[newfd].fd = newfd;
						fds[newfd].events = POLLIN;
						fds[newfd].revents = 0;
						fdmax++;
						printf("Server: New connection from %s on socket %d\n",
								inet_ntoa(client_addr.sin_addr), newfd);
						}
					} 
					else if(fds[i].revents & POLLIN){
						//recieve data
						if ((nbytes = recv(fds[i].fd, buff,BUFF_SIZE, 0)) <= 0) {
							if (nbytes == 0)
								printf("Server: socket %d out\n", fds[i].fd);
							close(fds[i].fd);
						}
						else {
							buff[nbytes]='\0';
							printf("Data form Socket %d : %s\n", fds[i].fd, buff);
							//send data
							send(fds[i].fd, buff, strlen(buff), 0);
							bzero(buff,BUFF_SIZE);
						}
					}
			}
		}
	}
	return 0;
}
