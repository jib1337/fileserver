/* File server application | Jack Nelson | CSP2308
 * networking.c
 * Provision and handling of network communications */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "fsClient.h"
#include "networking.h"
#include "files.h"
#include "io.h"

void serverConnect(config_t* Config, fileList_t* FileList) {

	struct sockaddr_in serverAddress;
	int connectionSocket;

	if ((connectionSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
		perror("Could not create socket");
	} else {

		serverAddress.sin_family = AF_INET;
		in_addr_t serverIP;
		inet_pton(AF_INET, Config->serverIP, &serverIP);
		serverAddress.sin_addr.s_addr = serverIP;
		serverAddress.sin_port = htons(Config->serverPort);

		if (connect(connectionSocket, (struct sockaddr*) &serverAddress, sizeof(struct sockaddr_in)) < 0) {
			perror("Could not connect");
		} else {

			char buffer[256];
			bzero(buffer, 256);
			char* motd = buffer + 2;
			int passwordReturn;

			// Send over the username
			write(connectionSocket, Config->username, strlen(Config->username));
			read(connectionSocket, buffer, 255);

			// Prompt the user for the password and send
			if ((passwordReturn = getPassword(buffer, 31)) == 1) {
				// Could not set the terminal not echo, so just allow normal entry
				printf("Enter server password: ");
				getKeyboardInput(buffer, 31);
			} else if (passwordReturn == 2) {
				exit(EXIT_FAILURE);
			}

			write(connectionSocket, buffer, 31);

			// read the reply from the server
			read(connectionSocket, buffer, 255);			       

			if (strcmp(buffer, "Access Denied.") != 0 && (buffer[0] == 'g')
					&& (buffer[1] == '/')) {
				// Server sent MOTD with access granted tag, so authentication was successful

			printf("\n----------------------------------------\n"
			       "Message of the day:\n"
			       "%s\n"
			       "----------------------------------------\n", motd);

			serverMenu(Config, FileList, connectionSocket);

			} else {
				printf("\n%s\n", buffer);
				printf("Please verfiy your credentials are correct.\n");
				exit(EXIT_SUCCESS);
			}

		}

		close(connectionSocket);
	}
}
