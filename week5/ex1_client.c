#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFF_SIZE 1024
#define C_FOUND_ID "00"
#define C_NOT_FOUND_ID "01"
#define C_FOUND_PASSWORD "11"
#define C_NOT_FOUND_PASSWORD "01"
#define C_LOGOUT_OK "20"
#define C_LOGOUT_FAILS "21"
#define C_BLOCK "31"
#define NOT_IDENTIFIED_USER 1
#define NOT_AUTHENTICATED 2
#define AUTHENTICATED 3
#define BLOCKED 0
#define ACTIVE 1
#define MAX 100

int request(int client_sock, char message[])
{
	if (send(client_sock, message, strlen(message), 0) > 0)
	{
		return 1;
	} else return 0;
}

int receive(int client_sock, char respond[])
{
	int bytes_received = recv(client_sock, respond, BUFF_SIZE -1, 0);
	if (bytes_received > 0)
	{
		respond[bytes_received] = '\0';
		return 1;
	} else return 0;
}

int main(int argc, char const *argv[])
{

	int SERV_PORT;
	char SERV_IP[MAX];
	int client_sock;
	char buff[BUFF_SIZE], respond[BUFF_SIZE];
	struct sockaddr_in server_addr; 
	int msg_len, bytes_sent, bytes_received;
	
	if (argc != 3) exit(1);

	SERV_PORT = atoi(argv[2]);
	strcpy(SERV_IP, argv[1]);

	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERV_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERV_IP);
	
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
	while(1){
		//send message
		strcpy(buff,"");
		printf("\nEnter your request:");
		memset(buff,'\0',(strlen(buff)+1));
		fgets(buff, BUFF_SIZE, stdin);		
		msg_len = strlen(buff);
		if (msg_len == 1) break;
		
		if (!request(client_sock, buff)){
			printf("message send fails\n");
		} else{
			printf("send:%s\n",buff );
		}
		if (!receive(client_sock, respond)){
			printf("message receive fails\n");
		} else { 
			printf("Respond from server : %s\n", respond);
		}
		
	}
	close(client_sock);
	return 0;
}