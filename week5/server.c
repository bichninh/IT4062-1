#include <stdio.h>         
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 2048
#define ACTIVE 1
#define BLOCKED 0

enum msg_type{
	FOUND_USER = 00,
	NOTFOUND_USER = 01,
	MATCH_PASSW = 10,
	NOTMATCH_PASSW = 11,
	USER_BLOCKED = 31,
	LOGOUT_SUCCESS = 20,
	LOGOUT_FAIL = 21,
	ERROR = 30,
};
enum status{
	USER_BLOCKED = 0,
	NOT_IDENTIFIED_USER  = 1,
	NOT_AUTHENTICATED  = 2,
	AUTHENTICATED  = 3
};

typedef struct User_ {
	char id[30];
	char pass[30];
	int status; //1 if active  and 0 if blocked
}User;

User userList[100];

typedef struct Session_ {
	status  sessStatus; //NOT_IDENT, NOT_AUTH,...
	struct User user;
	struct sockaddr_in cliaddr;
}Session;

Session sessionList[100];
int count = 0;

User createUser(char id[], char pass[], int status){
 	User new;
 	strcpy(new.id,id);
 	strcpy(new.pass,pass);
 	new.status = status;
 	return new;
 }

User findUser(char id[]){
	int i = 0;
	while(userList[i] != NULL){
		if(strcmp(userList[i].id, id) == 0){
			return userList[i];
		}
		i++;
	}
	return createUser("","",ACTIVE);
}

Session createSession(status sessStatus, User user,struct  sockaddr_in cliaddr){
 	Session new;
 	new.sessStatus = sessStatus;
 	new.user = user;
 	new.cliaddr = cliaddr;
 	return new;
 }

int findSession( struct sockaddr_in cliaddr){
	int i = 0;
	while(sessionList[i] != NULL){
		if(memcmp(&(sessionList[i].cliaddr), &cliaddr, sizeof(struct sockaddr_in)) == 0){
			return i;
		}
		i++;
	}
	return -1;
}
void readFile(char filename[]){
	FILE*fptr;
	int i = 0;
	char id[BUFF_SIZE], pass[BUFF_SIZE];
	int status;
	User user;
	if ((fptr = fopen(filename, "r")) == NULL){
		printf("Cannot open %s.\n", filename);
		return;
} 
	else{
		while(feof(fptr)){
			fscanf("%s %s %d\n",id, pass, status);
			user = createUser(id,pass,status);
			userList[i] = user;
			i++;
		}
		fclose(fptr);
	}
}
int handleMessage(char message[], char opCode[], char para[]){
	int i,j=0;
 	strncpy(opCode,message,4);
 	opCode[4] = '\0';
 	if(message[4] != ' ') return 0;
 	for(i=5, i<strlen(message); i++){
 		para[j] = message[i];
 		j++;
 		if(message[i] == ' ') return 0;
 	}
 	para[j] = '\0';
 	return 1;
 }

msg_type handleRun(char opCode[], char para[], struct sockaddr_in cliaddr){
	User user;
	Session session;
	int check = findSession(cliaddr);

	if(strcmp(opCode, "USER") == 0){
		user = findUser(para);
		if(strcmp(user.id, "") == 0) return NOTFOUND_USER;
		else if(user.status == BLOCKED) return USER_BLOCKED;
		if(check < 0){
			session = createSession(user,NOT_AUTHENTICATED,cliaddr);
			sessionList[count++] = session;
		} 
		else if(memcmp(&(sessionList[check].user),&user,sizeof(struct User)) == 0){
			sessionList[check].sessStatus = NOT_AUTHENTICATED;
			return FOUND_USER;
		}
		

	}
	else if(strcmp(opCode,"PASS") == 0 && check >=0 && sessionList[check].sessStatus == NOT_AUTHENTICATED){
		if (strcmp (sessionList[check].user.password, para) == 0){
			sessionList[check].sessStatus = AUTHENTICATED;
			sessionList[check].loginCount = 0;
			return MATCH_PASSW;
		} 
		else {
			sessionList[check].loginCount++;

			if (sessionList[check].loginCount > MAX_LOGIN){
				sess[pos].user.status = BLOCKED;
				return USER_BLOCKED;
			} 
			else return NOTMATCH_PASSW;
		}
	}

	else if(strcmp(opCode, "LOUT") == 0 && ){

	}
	else return 

}



int main(int argc, char const *argv[]){
	int listen_sock, conn_sock; /* file descriptors */
	char recv_data[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;
	
	if(argc != 2){
		printf("Error! Invalid parameters!\n");
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
		while(1){
			//receives message from client
			bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0); //blocking
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}
			else{
				recv_data[bytes_received] = '\0';
				printf("\nReceive: %s ", recv_data);
			}
			
			//echo to client
			bytes_sent = send(conn_sock, recv_data, bytes_received, 0); /* send to the client welcome message */
			if (bytes_sent <= 0){
				printf("\nConnection closed");
				break;
			}
		}//end conversation
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}