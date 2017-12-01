#ifndef MY_SOCKET_H
#define MY_SOCKET_H

#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>

#define BUFF_SIZE 1024


typedef enum {
	USER = 0,
	PASS = 1,
	LOUT = 2
}message_type;

typedef struct message {
	message_type type;
	char payload[BUFF_SIZE];
}message;

#endif
