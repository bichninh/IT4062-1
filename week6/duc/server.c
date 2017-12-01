#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "my_socket.h"

#define PORT 5550   /* Port that will be opened */ 
#define BACKLOG 20   /* Number of allowed connections */
#define BUFF_SIZE 1024
#define BLOCKED 0
#define ACTIVE 1
//#define NULL ((void


struct User{
	char id[30];
	char pass[30];
	int accStatus; //0 : blocked, 1: active
};

enum SessionStatus {
    NOT_IDENTIFIED_USER,  
    IDENTIFIED_USER,
    NOT_AUTHENTICATED,      
    AUTHENTICATED,          
    USER_BLOCKED            
};

struct Session{
	struct User user;
	enum SessionStatus sessStatus;
	struct sockaddr_in cliaddr;
};
struct Session newSession(struct sockaddr_in client){
	struct Session ss;
	//ss.user = NULL;;
	memset(&ss.user, 0, sizeof(ss.user));	
	ss.sessStatus = NOT_IDENTIFIED_USER;
	ss.cliaddr = client;
	return ss;
}

struct Session newSession(struct sockaddr_in client);
void *handle(void *);
void writeFile(struct User *users,int users_len);
struct Session addSession(struct User user,enum SessionStatus sessStatus);
int readFile(struct User *users);

struct Session addSession(struct User user,enum SessionStatus sessStatus){
	struct Session ss;
	ss.user = user;
	ss.sessStatus = sessStatus;
	return ss;
}

void writeFile(struct User *users,int users_len){
	FILE * fp;
	int i;
	fp = fopen("account.txt","w");
	if(fp == NULL){
		printf("Open file error\n");
		exit(1);
	}

	for ( i = 0;  i < users_len; ++i)
	{
		fprintf(fp,"%s %s %d\n",users[i].id,users[i].pass,users[i].accStatus);
	}
	fclose(fp);
}
/*
*Read file account.txt and write to users[],return number of user
*/
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
		strcpy(users[users_len].id,userid);
		j = 0;

		while(str[i] != ' '){
			password[j] = str[i];
			i++;
			j++;
		}
		i++;
		password[j] = '\0';
		j = 0;
		strcpy(users[users_len].pass,password);

		users[users_len].accStatus = str[i] - 48;
		users_len ++;

	}
		fclose(fp);
		return users_len;
}

struct Session sess[100];
struct User users[100];
int sess_len = 0;


int main(int argc, char *argv[])
{
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	pthread_t tid;
	socklen_t sin_size;
	int listen_sock,conn_sock,*new_sock;
	//(*sess_len) = -1;
	
	if (argc < 2){
		fprintf(stderr, "ERROR, no port provided\n");
		exit(0);
	}

	//users_len = readFile(users);
	//printf("%d\n",users_len );
	
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
	//signal(SIGCHLD, sig_chld);
	sin_size = sizeof(struct sockaddr_in);
	//client = (struct sockaddr_in *)malloc(sin_size);
	//communicate with all client
	while(conn_sock = accept(listen_sock, (struct sockaddr *)&client, &sin_size)){
		printf("%d\n",sess_len);
		//printf("%d\n",sess_len );
		//interrupt = 0;  //interrupt if BLOCKED or LOGOUT
		new_sock = malloc(1);
        *new_sock = conn_sock;
		//wrong_number = 0;

        	sess_len ++;
			//printf("%d\n",sess_len);
			//printf("%d\n",sess_len );
			sess[sess_len] = newSession(client);
			//printf("1\n");
			pthread_create(&tid,NULL,handle,(void *)new_sock);
			sess_len = sess_len + 1;
		
	}
	close(listen_sock);
	return 0;
}

void *handle(void *arg){
	message incoming_message;
	int wrong_number = 0;
	int  bytes_received/*,*sess_len = -1*/;
	int i,n,sess_user = -1; 
	enum SessionStatus sessStatus = NOT_IDENTIFIED_USER;
	int interrupt = 0,count; 
	int conn_sock,users_len;
	
	conn_sock = *((int *) arg);
	free(arg);
	pthread_detach(pthread_self());
	users_len = readFile(users);
	//printf("%d\n",users_len );
	//printf("%s\n",users[1].pass );
  		//communicate with a client
		while(1){
		//start conversation
			//users_len = readFile(users);
			//interrupt = 0;
		//receives message from client
			//printf("interrupt:%d\n",interrupt );
			if(interrupt == 1){
				break;
			}
			bytes_received = recv(conn_sock,(char*)&incoming_message, BUFF_SIZE - 1, 0); //blocking
			if (bytes_received <= 0){
				printf("\nConnection closed");
				exit(1);
			}
			printf("%s\n",incoming_message.payload );
			switch(incoming_message.type){
				case USER:
					sess_user = sess_len;
					for ( i = 0; i < users_len; i++)
					{
						if(strcmp(incoming_message.payload,users[i].id) == 0 && sess[sess_user].sessStatus == NOT_IDENTIFIED_USER){
							//printf("%d\n",users[i].accStatus );
							if(users[i].accStatus == 0){
								send(conn_sock,"31",2,0);
								n = 1;
								    //store users[i] for compare password in next step
								interrupt = 1;
								//close(conn_sock);
								break;
							}
							else{
								send(conn_sock,"00",2,0);
								sessStatus = IDENTIFIED_USER;
								sess[sess_len] = addSession(users[i],sessStatus);
								sess_user = sess_len;
								n = 1;
								count = i; 
								printf("%d\n",count );
							//	printf("%d\n",users[count].accStatus );    //store users[i] for compare password in next step
							//	sess_len ++;
							//	sess[sess_len].user = users[i];
							//	sess[sess_len].sessStatus = 1;
							//	sess[sess_len].cliaddr = client;
								break;
							}

						}
						else if(strcmp(incoming_message.payload,users[i].id) == 0 && sess[sess_user].sessStatus != NOT_IDENTIFIED_USER){
								send(conn_sock,"12",2,0);
								n = 1;
								break;
						}
					}
					if(n != 1)
						send(conn_sock,"01",2,0);
					break;
				case PASS:
				printf("%s\n",incoming_message.payload );
					if(sess_user != -1){
						//printf("%s\n",users[i].pass );
						if(strcmp(incoming_message.payload,users[count].pass) == 0){
							send(conn_sock,"10",2,0);
							sessStatus = AUTHENTICATED;
							sess[sess_user].sessStatus = AUTHENTICATED;
							continue;
						}

						else{
							wrong_number ++;
							
							if(wrong_number == 3 ){
								interrupt = 1;
								sessStatus = USER_BLOCKED;
								users[count].accStatus = BLOCKED;
								writeFile(users,users_len);
								//printf("%d %d %d\n",interrupt,count,users[count].accStatus );

								send(conn_sock,"31",2,0);
			
								//printf("%d  %d\n",count,users[count].accStatus );
								break;
							}
							send(conn_sock,"11",2,0);
						//printf("%s\n",incoming_message.payload );
						}
					}
					else 
						send(conn_sock,"01",2,0);
					break;
				case LOUT:
						if(sess_user != -1  && sess[sess_user].sessStatus == AUTHENTICATED && strcmp(incoming_message.payload,sess[sess_user].user.id) == 0){
							send(conn_sock,"20",2,0);
								interrupt = 1; 
							//count = -1;			//refresh count
							//sess[i].sessStatus = 0;
							//close(conn_sock);
							break;
						}
					
						if(interrupt != 1)
							send(conn_sock,"21",2,0);
					break;
			}
		}
	close(conn_sock);		
	return 0;
}

