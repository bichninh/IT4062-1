#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "my_socket.h"

#define PORT 5550   /* Port that will be opened */ 
#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 1024

struct User{
	char id[30];
	char pass[30];
	int accStatus; //0 : blocked, 1: active
};
//struct User users[100];
struct Session{
	struct User user;
	int sessStatus;// NOT IDENTIFIED USER, NOT AUTHENTICATED・
	struct sockaddr_in cliaddr;
};
//struct Session sess[100];

int readFile(struct User *users){
	FILE * fp;
	int users_len = 0;
	int i = 0,j = 0;
	char str[BUFF_SIZE],userid[BUFF_SIZE],password[BUFF_SIZE];
	fp = fopen("account.txt","r");
	if(fp == NULL){
		printf("Open file error\n");
		exit(1);
	}
	//printf("%d\n",i );
	while(fgets(str,BUFF_SIZE,fp) != NULL){
		i = 0;
		j = 0;
		while(str[i] != ' '){
			userid[j] = str[i];
			i++;
			j++;
		}
		i++;
		userid[j] = '\0';
		//printf("%s\n",userid );
		strcpy(users[users_len].id,userid);
		j = 0;

		while(str[i] != ' '){
			password[j] = str[i];
			i++;
			j++;
		}
		i++;
		password[j] = '\0';
		//printf("%s\n",password );
		j = 0;
		strcpy(users[users_len].pass,password);

		users[users_len].accStatus = str[i] - 48;
		//printf("%d\n",users[users_len].accStatus );
		users_len ++;

	}
		return users_len;
}


int main(int argc, char const *argv[])
{
	int listen_sock, conn_sock,users_len = 3,i,n,k,count, j = 0; /* file descriptors */
	int  bytes_received,sess_len = -1;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size ;
	message incoming_message;
	struct User users[100];
	struct Session sess[100];

	
	if (argc < 2){
		fprintf(stderr, "ERROR, no port provided\n");
		exit(0);
	}
	
	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port=htons(atoi(argv[1]));  /* Remember htons() from "Conversions" section? =) */
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
	//printf("%s\n",argv[1] );
	users_len = readFile(users);

	while(1){
		//printf("%d\n",j );
		//j++;
		k = 1;
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
  		
		while(1){
			  		//start conversation
		//receives message from client
			if(k == 2)
				break;
			bytes_received = recv(conn_sock,(char*)&incoming_message, BUFF_SIZE - 1, 0); //blocking
			if (bytes_received <= 0){
				printf("\nConnection closed");
				exit(1);
			}
		//	printf("%d\n",incoming_message.type );
			//else{
			//	incoming_message[bytes_received] = '\0';
				//printf("\nReceive: %s\n ",mess);
				//send(conn_sock,"\n",1,0);
				//mess = &incoming_message.payload;
			//}

			switch(incoming_message.type){
				case USER:
					for ( i = 0; i < users_len; ++i)
					{
						if(strcmp(incoming_message.payload,users[i].id) == 0){
							if(users[i].accStatus == 0){
								send(conn_sock,"31",2,0);
								n = 1;
								count = i;
								break;
							}
							else{
								send(conn_sock,"00",2,0);
								n = 1;
								count = i;
								sess_len ++;
								sess[sess_len].user = users[i];
								sess[sess_len].sessStatus = 1;
								sess[sess_len].cliaddr = client;
								//sess_len++;
								break;
							}

						}
					}
					if(n != 1)
						send(conn_sock,"01",2,0);
					break;
				case PASS:
					//printf("%s\n",incoming_message.payload );
					if(strcmp(incoming_message.payload,users[count].pass) == 0 && users[count].accStatus == 0){
						send(conn_sock,"31",2,0);
						continue;
					}
					else if(strcmp(incoming_message.payload,users[count].pass) == 0 && users[count].accStatus == 1){
						send(conn_sock,"10",2,0);
						sess[sess_len].sessStatus = 2;
						continue;
					}
					else
						send(conn_sock,"11",2,0);
					break;
				case LOUT:
					//printf("%d\n",incoming_message.type );
					//printf("%s\n",incoming_message.payload );
					for ( i = 0; i <= sess_len; ++i)
					{
						if(strcmp((sess[i].user).id,incoming_message.payload) == 0 && sess[i].sessStatus == 2){
							send(conn_sock,"20",2,0);
							sess_len ++;
							k = 2;
							close(conn_sock);
							break;
						}
					}
					if(k != 2)
						send(conn_sock,"21",2,0);
					break;
			}
		}
			
			
	}
	close(listen_sock);
	return 0;
}