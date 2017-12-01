/*UDP Echo Server*/
#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

//#define PORT 5550  /* Port that will be opened */ 
#define BUFF_SIZE 1024

int main(int argc, char const *argv[]){
 	int val,sum=0; //for summary value 
	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size;

	if(argc != 2){
		printf("Error!\n");
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
		sin_size=sizeof(struct sockaddr_in);
    		
		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE-1, 0, (struct sockaddr *) &client, &sin_size);
		
		if (bytes_received < 0)
			perror("\nError: ");

		else{
			buff[bytes_received] = '\0';
			val = atoi(buff);
			if(val == 0){ 
				printf("Error! Wrong number-string\n");
				strcpy(buff,"Error! The number-string you enter is invalid");
			}

			else{
				while(val){// calculate summary of digits
					sum = sum + val%10;
					val = val/10;
				}
				sprintf(buff,"%d",sum);//convert variable sum from integer to string and put it to buffer
				printf("[%s:%d]: %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), sum);
				sum = 0;
			}
		}
		
		bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *) &client, sin_size ); /* send to the client welcome message */
		if (bytes_sent < 0)
			perror("\nError: ");					
	}
	
	close(server_sock);
	return 0;
	}
}