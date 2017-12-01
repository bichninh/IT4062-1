#include <stdio.h>        
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define BACKLOG 2  // Number of allowed connections
#define MAX_RECV_BUFF 256
#define MAX_SEND_BUFF 256

void getFilename(int sock, char* file_name){ //get file name from client
        char recv_str[MAX_RECV_BUFF]; //store received string
        ssize_t rcvd_bytes; // bytes received from socket
        char temp [] = "folder/";
        // read name of requested file from socket
        if ( (rcvd_bytes = recv(sock, recv_str, MAX_RECV_BUFF, 0)) < 0) {
                perror("Recv error");
                return;
}
        sscanf (recv_str, "%s\n", file_name);
        strcat(temp,file_name);
        strcpy(file_name,temp);
        printf("The file will be sent to %s\n", file_name);
}

int recvFile(int sock, char*file_name){// recieve file and return size of recieved file in bytes
        int file;       // status for receiving file
        ssize_t rcvd_bytes, rcvd_file_size;
        int recv_count;          //times of recieving data
        char recv_str[MAX_RECV_BUFF]; //hold received data

        //create file to save received data. 0644 = rw-r--r-- 
        if ( (file = open(file_name, O_WRONLY | O_CREAT, 0644)) < 0 ){
                perror("Error creating file");
                return -1;
        }       
        recv_count = 0; 
        rcvd_file_size = 0; //size of received file

        // continue receiving until data is over or close 
        while ( (rcvd_bytes = recv(sock, recv_str, MAX_RECV_BUFF, 0)) > 0 ){
                recv_count++;
                rcvd_file_size += rcvd_bytes;

        if (write(file, recv_str, rcvd_bytes) < 0 ){//write to file
                perror("Error writing to file");
                return -1;
                }
        }
        close(file);
        printf("Received %ld bytes in %d time(s)\n", rcvd_file_size,recv_count);
        return rcvd_file_size;
 }

int main(int argc, char const *argv[]){
        char file_name[MAX_RECV_BUFF];
        int listen_sock, conn_sock; 
        struct sockaddr_in server; 
        struct sockaddr_in client; 
        int sin_size;
    
        if(argc != 2){
                printf("Error! Invalid parameter!\n");
                return 0;
        }
        

        if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  
                perror("\nError: ");
                return 0;
    }
    
        
        bzero(&server, sizeof(server));
        server.sin_family = AF_INET;         
        server.sin_port = htons(atoi(argv[1]));   
        server.sin_addr.s_addr = htonl(INADDR_ANY);  
        if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ 
                perror("\nError ");
                return 0;
    }     
    

        if(listen(listen_sock, BACKLOG) == -1){
                perror("\nError ");
                return 0;
    }
    
        while(1){
                sin_size = sizeof(struct sockaddr_in);
                if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
                        perror("\nError ");
  
        printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); 
        
        //start conversation
                getFilename(conn_sock,file_name);
                recvFile(conn_sock,file_name);

        close(conn_sock);   
    }
    close(listen_sock);
    return 0;
}