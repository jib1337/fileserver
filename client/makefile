# File server application | Jack Nelson | CSP2308
# makefile
# Builds the file server client application binary

CC = gcc
CFLAGS = -Wall
OBJ = fsClient.o files.o settings.o io.o networking.o
EXEC = fsClient

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(CFLAGS)

fsClient.o: fsClient.h settings.h io.h files.h networking.h fsClient.c 
	$(CC) $(CFLAGS) -c fsClient.c

settings.o: settings.h files.h io.h fsClient.h settings.c
	$(CC) $(CFLAGS) -c settings.c

files.o: files.h fsClient.h files.c
	$(CC) $(CFLAGS) -c files.c

io.o: io.h files.h settings.h fsClient.h io.c
	$(CC) $(CFLAGS) -c io.c

networking.o: fsClient.h networking.h files.h io.h networking.c
	$(CC) $(CFLAGS) -c networking.c

clean:
	rm -vf $(OBJ)
