CC=gcc
CFLAGS=-I.
OBJ = exercise1.o 
OBJ2 = exercise2.o
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)
all:	exercise1 exercise2
exercise1: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
exercise2: $(OBJ2)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm *.o
