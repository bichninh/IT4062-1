#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define MAX_RECV_BUFF 256
#define MAX_SEND_BUFF 256 

void sendFileName(int sock, char*file_name){ //send file name after only get the part  behind the last '/'
        int i=0,count=0,j=0;
        char buff[MAX_SEND_BUFF];
        char send_str [MAX_SEND_BUFF]; // message to be sent to server
        size_t send_strlen; // length of transmitted string
        ssize_t sent_bytes;

        sprintf(buff, "%s\n", file_name); 
        printf("%s\n",buff );
        buff[strlen(buff) - 1] = '\0';

        while(buff[i] != '\0'){ //find the last '/' in file_name string
                if(buff[i] == '/'){
                        count = i;
                }
                i++;
        }
        
        for(i=count+1;i<strlen(buff);i++){//cut file name and put it to send_str for sending to server
                send_str[j] = buff[i];
                j++;
        }

        printf("The file name is: %s\n",send_str);
        send_strlen = strlen(send_str);
        if( (sent_bytes = send(sock, send_str, send_strlen, 0)) < 0 ) {
                perror("Send error");
                return;
        }
}

int sendFile(int sock, char*file_name){ //send file and return size of sending file in bytes
        int sent_count; //times of sending data
        ssize_t read_bytes, //bytes read from local file
                sent_bytes, //bytes sent to connected socket
                sent_file_size; //size of file
        char send_buf[MAX_SEND_BUFF];
        char * errmsg_notfound = "File not found\n";
        int file; // status of sending file
        sent_count = 0;
        sent_file_size = 0;
        
        // open requested file for reading
        if( (file = open(file_name, O_RDONLY)) < 0){
                perror(file_name);
                if( (sent_bytes = send(sock, errmsg_notfound , strlen(errmsg_notfound), 0)) < 0 ){
                        perror("Send error");
                        return -1;
                }
        }
        else{ // open file successful
                printf("Sending file: %s\n", file_name);
                while( (read_bytes = read(file, send_buf, MAX_RECV_BUFF)) > 0 ){ //continue send file until data is over or close
                        if( (sent_bytes = send(sock, send_buf, read_bytes, 0))< read_bytes ){
                                perror("Send error");
                                return -1;
                        }
                sent_count++;
                sent_file_size += sent_bytes;
                }
        close(file);
        }

        printf("Sent %ld bytes in %d time(s)\n",sent_file_size, sent_count);
        return sent_count;
}

int main(int argc, char const *argv[]){
        int client_sock;
        char file_name[MAX_SEND_BUFF];
        struct sockaddr_in server_addr; 
        
        if(argc != 3){ //invalid argc
                printf("Error! Invalid parameter!\n");
                return 0;
        }

    
        client_sock = socket(AF_INET,SOCK_STREAM,0);
        
 
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(argv[1]);
        server_addr.sin_port = htons(atoi(argv[2]));

        
     
        if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
                printf("\nError!Can not connect to sever! Client exit imediately! ");
                return 0;
        }
                
                     
                //start
                printf("Enter link of the file you need to send in direct way:\n");
                fgets(file_name,MAX_SEND_BUFF,stdin);
                file_name[strlen(file_name) -1] = '\0';
                
                sendFileName(client_sock,file_name);
                sendFile(client_sock, file_name);
                printf("Closing connection\n");
      
     
        close(client_sock);
        return 0;
}