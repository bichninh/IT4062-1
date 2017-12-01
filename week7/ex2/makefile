CC=gcc
CFLAGS=-w -std=c99

all: server client

server: server.c data.c run.c
	$(CC) -o $@ $^ $(CFLAGS)

client: client.c
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -rf client server
