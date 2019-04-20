# File server application | Jack Nelson | CSP2308
# makefile
# Builds the file server application binary

CC = gcc
CFLAGS = -Wall -lssl -lcrypto
OBJ = fileServer.o logger.o files.o settings.o io.o security.o networking.o
EXEC = fs

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(CFLAGS)

fileServer.o: fileServer.h settings.h logger.h io.h files.h security.h networking.h fileServer.c 
	$(CC) $(CFLAGS) -c fileServer.c

logger.o: logger.h logger.c
	$(CC) $(CFLAGS) -c logger.c

settings.o: settings.h files.h io.h fileServer.h settings.c
	$(CC) $(CFLAGS) -c settings.c

files.o: files.h fileServer.h files.c
	$(CC) $(CFLAGS) -c files.c

io.o: io.h logger.h files.h settings.h fileServer.h io.c
	$(CC) $(CFLAGS) -c io.c

security.o: security.h settings.h io.h logger.h networking.h fileServer.h security.c
	$(CC) $(CFLAGS) -c security.c

networking.o: networking.h logger.h io.h security.h networking.c
	$(CC) $(CFLAGS) -c networking.c

clean:
	rm -vf $(OBJ) *.log
