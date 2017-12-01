#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define BACKLOG 2 
#define BUFF_SIZE 1024
#define USER "USER"
#define PASS "PASS"
#define LOUT "LOUT"
#define C_FOUND_ID "00"
#define C_NOT_FOUND_ID "01"
#define C_FOUND_PASSWORD "10"
#define C_NOT_FOUND_PASSWORD "11"
#define C_LOGOUT_OK "20"
#define C_LOGOUT_FAILS "21"
#define C_BLOCK "31"
#define NOT_IDENTIFIED_USER 1
#define NOT_AUTHENTICATED 2
#define AUTHENTICATED 3
#define BLOCKED 0
#define ACTIVE 1
#define MAX_NUMBER_LOGIN 3
#define MAX_USER 100
#define MAX_SESSION 100

struct User
{
	char id[30];
	char password[30];
	int userStatus;
	int count;
};

struct User users[MAX_USER];

struct Session
{
	struct User user;
	int sessStatus;
	int countLogin;
	struct sockaddr_in cliAddr;
};

struct Session sess[MAX_SESSION];
int sessCount = 0;
//user constructor
struct User newUser(char id[], char password[], int userStatus)
{
	struct User user;
	strcpy(user.id, id);
	strcpy(user.password, password);
	user.userStatus = userStatus;
	user.count = 0;
	return user;
}

//session constructor
struct Session newSession(struct User user, int sessStatus, struct sockaddr_in cliAddr)
{
	struct Session session;
	session.user = user;
	session.sessStatus = sessStatus;
	session.cliAddr = cliAddr;
	session.countLogin = 0;
	return session;
}
//check message from client is valid?
int isValidMessage(char message[], char messCode[], char messAcgument[])
{
	int i = 0, j = 0;
	for (i = 0; i < 4; i++ ){
		messCode[i] = message[i];
	}
	messCode[i] = '\0';
	if (message[i] != ' ') return 0;
	while (message[++i] != '\n'){
		messAcgument[j++] = message[i];
		if (message[i] == ' ') return 0;
	}
	messAcgument[j] = '\0';
	return 1;
}

int receive(int conn_sock, char message[])
{
	int bytes_received = recv(conn_sock, message, BUFF_SIZE -1, 0);
	if (bytes_received > 0)
	{
		message[bytes_received] = '\0';
		return 1;
	} else return 0;
}

int respond(int conn_sock, char respond[])
{
	if (send(conn_sock, respond, strlen(respond), 0) > 0)
	{
		return 1;
	} else return 0;
}

//read file and save to struct User
void readFileUser(char filename[])
{
	FILE *f = fopen(filename,"r");
	int i = 0;
	char id[30], password[30], userStatus[2];
	struct User user;
	if (f == NULL)
	{
		printf("Can't open file %s!\n", filename);
		return; 
	}else {
		while (!feof(f))
		{
			fscanf(f, "%s %s %s\n", id, password, userStatus);
			user = newUser(id, password, atoi(userStatus));
			users[i] = user;
			users[i].count = i + 1;
			i++;
		}
	}
}

//write user to file
void writeUserToFile(char filename[])
{
	FILE *f = fopen(filename,"w+");
	int i = 0;
	struct User user;
	if (f == NULL)
	{
		printf("Can't open file %s!\n",filename);
		return; 
	}else {
		while (!feof(f))
		{
			fprintf(f, "%s %s %d\n", users[i].id, users[i].password, users[i].userStatus);
			i++;
		}
	}
}

//show user detail
void showUser()
{
	int i;
	printf("List user information: \n");
	for (i = 0; users[i].count != 0; i++ )
	{
		printf("----------------------------------------------\n");
		printf("Id : %s\n",users[i].id);
		printf("Password : %s\n",users[i].password);
		printf("Status : %d\n",users[i].userStatus);
	}
	printf("-----------------------------------------------------\n");
}

//find user by userID
struct User findUserById(char messAcgument[])
{
	int i = 0;
	for (i = 0; users[i].count != 0; i++ ){
		if (strcmp(users[i].id, messAcgument) == 0)
		{
			return users[i];
		} 
	}
	return newUser("","",ACTIVE);
}

//add a new session
int addSession(struct Session session)
{
	sess[sessCount++] = session;
}

//find session by cliAddr, return session position
int findSessByAddr(struct sockaddr_in cliAddr)
{
	int i = 0;
	for (i = 0; i < sessCount; i++ ){
	 	if (memcmp(&(sess[i].cliAddr), &cliAddr, sizeof(struct sockaddr_in)) == 0)
		{
			return i;
		}
	}
	return -1;
}

//process request
char *process(char messCode[], char messAcgument[], struct sockaddr_in cliAddr )
{
	struct User user;
	struct Session session;
	int pos;
	pos = findSessByAddr(cliAddr);

	/*******************messcode is USER****************/
	if (strcmp(messCode, USER) == 0 ){

		user = findUserById (messAcgument);
		if (user.userStatus == BLOCKED)	return C_BLOCK;
		if (strcmp(user.id, "") == 0) return C_NOT_FOUND_ID;
 		if (pos == -1)
 		{
 			session = newSession(user, NOT_AUTHENTICATED, cliAddr);
 			addSession(session);
 		}else if (memcmp(&(sess[pos].user), &user, sizeof(struct User)) == 0){
 			sess[pos].sessStatus = NOT_AUTHENTICATED;
 		}
 		return C_FOUND_ID;
	}

	/***********************messcode is PASS*****************/
	else if (strcmp(messCode, PASS) == 0 && pos != -1 && sess[pos].sessStatus == NOT_AUTHENTICATED )
	{

		if (sess[pos].user.userStatus == BLOCKED)
		{
			return C_BLOCK;
		}

		if (strcmp (sess[pos].user.password, messAcgument) == 0)
		{
			sess[pos].sessStatus = AUTHENTICATED;
			sess[pos].countLogin = 0;
			return C_FOUND_PASSWORD;
		} else {
			sess[pos].countLogin++;

			if (sess[pos].countLogin > MAX_NUMBER_LOGIN){
				sess[pos].user.userStatus = BLOCKED;
				return C_BLOCK;
			} else
				return C_NOT_FOUND_PASSWORD;
		}
	}

	/***********************messcode is LOUT********************/
	else if (strcmp(messCode, LOUT) == 0 && pos != -1 && sess[pos].sessStatus == AUTHENTICATED)
	{
		if (strcmp(sess[pos].user.id, messAcgument) == 0)
		{
			sess[pos].sessStatus = NOT_IDENTIFIED_USER;
			return C_LOGOUT_OK;
		} else 
			return C_LOGOUT_FAILS;
	}
	else
	{
		return "Login Sequence Is Wrong";
	}
}

//convert to full message
void changeFull(char message[])
{
	if (strcmp(message, C_FOUND_ID) == 0)
	{
		strcat(message, " -> username ok");
	} else if (strcmp(message, C_NOT_FOUND_ID) == 0)
	{
		strcat(message, " -> user incorrect");
	} else if (strcmp(message, C_FOUND_PASSWORD) == 0)
	{
		strcat(message, " -> password ok, login successful!");
	} else if (strcmp(message, C_NOT_FOUND_PASSWORD) == 0)
	{
		strcat(message, " -> password incorrect , login fails");
	} else if (strcmp(message, C_LOGOUT_OK) == 0)
	{
		strcat(message, " -> logout successful!");
	} else if (strcmp(message, C_LOGOUT_FAILS) == 0)
	{
		strcat(message, " -> logout fails");
	} else if (strcmp(message, C_BLOCK) == 0)
	{
		strcat(message, " -> user blocked");
	}
}
int main(int argc, char const *argv[])
{	
	if (argc != 2) return -1;
 
 	int PORT = atoi(argv[1]);
	int listen_sock, conn_sock; 
	char message[BUFF_SIZE], messCode[BUFF_SIZE], messAcgument[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; 
	struct sockaddr_in client;
	int sin_size;

	
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  
		perror("\nError: ");
		return 0;
	}
	
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);   
	server.sin_addr.s_addr = htonl(INADDR_ANY); 
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){
		perror("\nError: ");
		return 0;
	}     
	
	if(listen(listen_sock, BACKLOG) == -1){  
		perror("\nError: ");
		return 0;
	}
	
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); 
		readFileUser ("account.txt");
		showUser();
		while (1){
			if (!receive(conn_sock, message)){
				printf("message receive from %s fails\n", inet_ntoa(client.sin_addr));
				break;
			} else{
				printf("receive:%s\n", message);
				if (isValidMessage (message, messCode, messAcgument))
				{
					printf("messCode:%s\nmessAcgument:%s\n",messCode, messAcgument );
					strcpy(message, process(messCode, messAcgument, client));
					changeFull(message);
				}
			}
			if (!respond(conn_sock, message)){
				printf("respond fails!\n");
			} else{
				printf("respond:%s\n", message);
			}
		}
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}