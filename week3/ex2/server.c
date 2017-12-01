/*UDP Echo Server*/
#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//#define PORT 5550  /* Port that will be opened */ 
#define BUFF_SIZE 1024
//DNS example: get a IP or domain name from client and server return information about this IP/domain name
	
int main(int argc, char const *argv[]){ 
	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE];
	char temp[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	struct hostent *host; /* host information */ 
	struct in_addr **address_list; //list of IPv4 address of a domain name
	struct in_addr **aliases_list;//list of Aliases address of a domain name
	struct in_addr address;
	int sin_size;
	int i,j;
	char **alias;

	if(argc != 2){
		printf("\nError! Invalid parameter!\n");
		return 0;
	}

	else{//argc == 2 with argv[1] is server port
		//type command ./server PORT in direct way to run
	//Step 1: Construct a UDP socket
		if ((server_sock=socket(AF_INET, SOCK_DGRAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		exit(0);
	}
	
	//Step 2: Bind address to socket
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));   /* Remember htons() from "Conversions" section? */
	server.sin_addr.s_addr = INADDR_ANY;  /* INADDR_ANY puts your IP address automatically */   
	bzero(&(server.sin_zero),8); /* zero the rest of the structure */

  
	if(bind(server_sock,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		exit(0);
	}     
	printf("Server started.\n");

	//Step 3: Communicate with clients
	while(1){
		sin_size = sizeof(struct sockaddr_in);
    		
		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE-1, 0, (struct sockaddr *) &client, &sin_size);
		
		if (bytes_received < 0)
			perror("\nError: ");

		else{
			buff[bytes_received] = '\0';
			
			if( atoi(buff) == 0){//client enter a domain name
				host = gethostbyname(buff);
				if (host == NULL)  {//error
					herror("gethostbyname");
					strcpy(temp,"Not found information");
				}
				
				else {
					address_list = (struct in_addr **)host->h_addr_list;
					printf("Offcial IP: %s \n",inet_ntoa(*address_list[0]));
					strcpy(temp,inet_ntoa(*address_list[0]));
					printf("Alias IP:\n");
    					for(i = 1; address_list[i] != NULL; i++) {
       						 printf("%s ", inet_ntoa(*address_list[i]));
    					}
    					printf("\n");
				} 
			}
		

			else {//client enter an IPv4 address
				inet_pton(AF_INET, buff, &address);//convert address from string buff
				host = gethostbyaddr(&address, sizeof address, AF_INET);
				if(host == NULL) {//error 
					herror("gethostbyaddr");
					strcpy(temp,"IP Address is invalid");
			}
				else{ 
					strcpy(temp,host->h_name);
					alias = host->h_aliases;
					printf("Host name: %s\n", host->h_name);
					printf("Alias name:\n");
					for(i = 0; alias[i] != NULL; i++) {
       						 printf("%s ", alias[i]);
    					}
					
				}
			}
		}

		bytes_sent = sendto(server_sock, temp, strlen(temp), 0, (struct sockaddr *) &client, sin_size ); /* send to the client welcome message */
		if (bytes_sent < 0)
			perror("\nError: ");					
	}

	close(server_sock);
	return 0;
	}
}