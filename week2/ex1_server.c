/*UDP Echo Server*/
#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <stdlib.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <limits.h>

#define BUFF_SIZE 1024

int getTotal(char str[]){
	int total = 0;
	for (int i = 0; i < strlen(str) - 1; ++i)
	{
		if (!isdigit(str[i]))
        {
            return -1;
        }
        total += (str[i] - '0');
	}
	return total;
}

int main(int argc, char const *argv[])
{	
	if (argc != 2) return -1;
	int PORT = atoi(argv[1]);
	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size, total;

	// printf("%s\n", argv[1]);

	//Construct a UDP socket
	if ((server_sock=socket(AF_INET, SOCK_DGRAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		exit(1);
	}
	
	//Bind address to socket
	server.sin_family = AF_INET;     //IP v.4    
	server.sin_port = htons(PORT);   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = INADDR_ANY;  /* INADDR_ANY puts your IP address automatically */   
	bzero(&(server.sin_zero),8); /* zero the rest of the structure */

  
	if(bind(server_sock,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		exit(1);
	}     
	
	//Communicate with clients
	while(1){
		sin_size=sizeof(struct sockaddr_in);
    	
		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE-1, 0, (struct sockaddr *) &client, &sin_size);
		
		if (bytes_received < 0)
			perror("\nError: ");
		else{
			buff[bytes_received] = '\0';
			printf("[%s:%d]: %s", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buff);
		}
		
		total = getTotal(buff);
		bytes_sent = sendto(server_sock, &total, bytes_received, 0, (struct sockaddr *) &client, sin_size ); /* send to the client welcome message */		
		if (bytes_sent < 0)
			perror("\nError: ");			
	}
	
	close(server_sock);
	return 0;
}

