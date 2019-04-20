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
#include "io.h"

void serverConnect(config_t* Config) {

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

			// Send over the username
			write(connectionSocket, Config->username, strlen(Config->username));
			read(connectionSocket, buffer, 255);

			// Prompt the user for the password
			printf("Enter server password: ");
			getKeyboardInput(buffer, 31);
			write(connectionSocket, buffer, 31);

			// read the reply from the server
			read(connectionSocket, buffer, 255);

			printf("%s\n", buffer);	

			if (strcmp(buffer, "Access Denied.") != 0) {
				
				char menuChoiceString[2];
				int menuChoice = 0;

				while (menuChoice != 4) {

					showServerOptions();
					getKeyboardInput(menuChoiceString, 2);
					menuChoice = atoi(menuChoiceString);

					switch (menuChoice) {
						case (1):
							printf("List Files\n");
							write(connectionSocket, "1", 2);
							break;

						case (2):
							printf("Upload\n");
							write(connectionSocket, "2", 2);
							break;

						case (3):
							printf("Download\n");
							write(connectionSocket, "3", 2);
							break;
						
						case (4):
							printf("Quitting\n");
							write(connectionSocket, "4", 2);
							break;
						
						default:
							printf("Invalid choice\n");
					}
				}


			} else {
				printf("Please verfiy your credentials are correct.\n");
			}

		}

		close(connectionSocket);
	}
}
