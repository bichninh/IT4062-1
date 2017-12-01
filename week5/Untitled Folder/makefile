CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 
all: client
client: client.o
	${CC} client.o -o client
client.o: client.c my_socket.h
	${CC} ${CFLAGS} client.c
all: server
server: server.o
	${CC} server.o -o server
server.o: server.c my_socket.h
	${CC} ${CFLAGS} server.c
clean:
	rm -f *.o *~
