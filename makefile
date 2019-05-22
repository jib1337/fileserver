# File server application | Jack Nelson | CSP2308
# makefile
# Builds the file server application binary

CC = gcc
CFLAGS = -Wall -lssl -lcrypto -pthread
OBJ = fileServer.o logger.o settings.o io.o security.o networking.o clientlist.o
EXEC = fs

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(CFLAGS)

fileServer.o: fileServer.h settings.h logger.h io.h security.h networking.h fileServer.c 
	$(CC) $(CFLAGS) -c fileServer.c

logger.o: logger.h logger.c
	$(CC) $(CFLAGS) -c logger.c

settings.o: settings.h io.h fileServer.h logger.h settings.c
	$(CC) $(CFLAGS) -c settings.c

io.o: io.h logger.h fileServer.h io.c
	$(CC) $(CFLAGS) -c io.c

security.o: security.h io.h settings.h fileServer.h security.c
	$(CC) $(CFLAGS) -c security.c

networking.o: networking.h fileServer.h logger.h io.h security.h clientlist.h networking.c
	$(CC) $(CFLAGS) -c networking.c

clientlist.o: clientlist.h fileServer.h clientlist.c
	$(CC) $(CFLAGS) -c clientlist.c

clean:
	rm -vf $(OBJ) *.log
