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
#include <pthread.h>
#include <signal.h>

#include "fileServer.h"
#include "networking.h"
#include "io.h"
#include "logger.h"
#include "security.h"
#include "files.h"
#include "clientlist.h"

static list_t* g_clientList; 

void serverStart(config_t* Config) {

	struct sockaddr_in serverAddress, clientAddress;
	int listenSocket;

	if ((listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {

			perror("Could not create socket");

	} else {
			
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = inet_addr(Config->ipAddress);
		serverAddress.sin_port = htons(Config->portNumber);

		if (bind(listenSocket, (struct sockaddr*) &serverAddress, sizeof(struct sockaddr_in)) < 0) {
				
			perror("Could not bind port");

		} else {

			//signal(SIGTERM, signalConnectionShutdown);
			//signal(SIGQUIT, signalConnectionShutdown);

			listen(listenSocket, 128);

			// Log the server running and also inform the user
			char connectionStartMessage[40];
			sprintf(connectionStartMessage, "Server running on %s:%d", Config->ipAddress, Config->portNumber);
			logPipe(connectionStartMessage, Config->logFd);
			printf("\n%s\n", connectionStartMessage);

			int clientSocket;
			unsigned int clientSize = sizeof(struct sockaddr_in);
			list_t* clientList = newClientList();
			g_clientList = clientList;
				
			while ((clientSocket = accept(listenSocket, (struct sockaddr*) &clientAddress, &clientSize))) {
					
				if (clientSocket < 0) {
					perror("Could not accept new connection");
					break;

				} else {
				
					/* Build the theadData struct containing everything we need for client interaction
					 * Since we don't know how long the server will be running, we'll handle the structure
				 	 * containing thread data dynamically, so it can be freed when it's not needed anymore.*/

					threadData_t* ServerInfo = calloc(1, sizeof(threadData_t));
					clientList = insertClient(clientList, ServerInfo);

					clientList->foot->data->clientSocket = clientSocket;
					clientList->foot->data->Config = Config;
					clientList->foot->data->recFp = NULL;
					clientList->foot->data->sendFd = -1;

					logPipe("Client connected", Config->logFd);
					printf("Client connected\n");

					if ((pthread_create(&clientList->foot->data->threadId, NULL, connectionHandler, (void*)clientList->foot->data)) != 0) {
						perror("Could not create thread");
						exit(1);

					}
					
					// Add the thread data struct to the client linked list
					clientList = removeClient(clientList, ServerInfo);

				}
				
			}
			// The program currently does not leave the loop, so this socket will never close.
			// Nor will the process ever exit.
			// Probs handle this with a signal to break the loop? Check bookmarks
			close(listenSocket);
		}
		printf("Exiting...");
		exit(0);
	}
}

void* connectionHandler(void* data) {

	threadData_t* ServerInfo = (threadData_t*) data;

	if (clientLogin(ServerInfo) == 1) {

		char menuChoiceString[2];
		int menuChoice = 0;
		char accessMotdString[ACCMOTDLEN];

		// User authenticated successfully, so send access granted tag and motd
		strcpy(accessMotdString, "g/");
		strcat(accessMotdString, ServerInfo->Config->motd);
		write(ServerInfo->clientSocket, accessMotdString, sizeof(accessMotdString));

		while (menuChoice != 4) {

			read(ServerInfo->clientSocket, menuChoiceString, 2);
			menuChoice = atoi(menuChoiceString);

			switch (menuChoice) {
				case (1):
					listFiles(ServerInfo);
					break;
				case (2):
					recieveFileMenu(ServerInfo);
					break;
				case (3):
					sendFileMenu(ServerInfo);
					break;
				case (4):
					printf("Quit\n");
			}
		}


	} else {

		// User failed authentication
		write(ServerInfo->clientSocket, "Access Denied.", 14);
	}

	close(ServerInfo->clientSocket);
	free(ServerInfo);

	printf("thread exiting now...");
	pthread_exit((void*) 0);
	
}
