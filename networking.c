/* File server application | Jack Nelson | CSP2308
 * networking.c
 * Provision and handling of network communications */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
#include "clientlist.h"

static int g_exit = 0;

void connectionSignalShutdown() {
	/* Signal handler for SIGQUIT and SIGTERM when server is up
	 * Sets a global flag to leave accept loop */

	printf("\nShut down via signal\n");
	g_exit = 1;
}

void threadExit() {
	// Signal handler for exiting a thread, called via SIGUSR1

	pthread_exit(NULL);
}

void serverStart(config_t* Config) {

	struct sockaddr_in serverAddress, clientAddress;
	int listenSocket;

	if ((listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {

			perror("Error - Could not create socket");

	} else {
			
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = inet_addr(Config->ipAddress);
		serverAddress.sin_port = htons(Config->portNumber);

		if (bind(listenSocket, (struct sockaddr*) &serverAddress, sizeof(struct sockaddr_in)) < 0) {
				
			perror("Error - Could not bind port");

		} else {

			// Change the signal handling over for SIGQUIT and SIGTERM
			struct sigaction action;
			action.sa_handler = connectionSignalShutdown;
			action.sa_flags = 0;
			sigemptyset(&action.sa_mask);
			sigaction(SIGTERM, &action, NULL);
			sigaction(SIGQUIT, &action, NULL);

			listen(listenSocket, 128);

			// Log the server running and also inform the user
			char connectionStartMessage[40];
			sprintf(connectionStartMessage, "Server running on %s:%d", Config->ipAddress, Config->portNumber);
			logPipe(connectionStartMessage, Config->logFd);
			printf("\n%s\n", connectionStartMessage);

			int clientSocket;
			unsigned int clientSize = sizeof(struct sockaddr_in);
			list_t* clientList = newClientList();
				
			while ((clientSocket = accept(listenSocket, (struct sockaddr*) &clientAddress, &clientSize))) {
				// Listen for clients and accept incoming connections

				if (g_exit == 1) break; // Break out if global has been flipped
					
				if (clientSocket < 0) {
					perror("Error - Could not accept new connection");
					break;

				} else {
				
					/* Build the theadData struct containing everything we need for client interaction
					 * Since we don't know how long the server will be running, we'll handle the structure
				 	 * containing thread data dynamically, so it can be freed when it's not needed anymore.*/
					
					threadData_t* ServerInfo = calloc(1, sizeof(threadData_t));

					ServerInfo->clientList = clientList;
					ServerInfo->clientSocket = clientSocket;
					ServerInfo->Config = Config;
					ServerInfo->recFd = -1;
					ServerInfo->sendFd = -1;
					
					insertClient(clientList, ServerInfo);

					logPipe("Client connected", Config->logFd);
					printf("Client connected\n");

					if ((pthread_create(&ServerInfo->threadId, NULL, connectionHandler, (void*)ServerInfo)) != 0) {
						perror("Error - Could not create thread");
						exit(EXIT_FAILURE);

					}
				}
			}
			
			// Close the listener socket and clean out the client list
			close(listenSocket);
			cleanupClients(clientList);
		}

		logPipe("Program shut down", Config->logFd);

		wait(NULL);
		exit(EXIT_SUCCESS);
	}
}

void* connectionHandler(void* data) {
	// Starting point for a client handler thread

	threadData_t* ServerInfo = (threadData_t*) data;

	pthread_detach(pthread_self());
	signal(SIGUSR1, threadExit);

	if (clientLogin(ServerInfo) == 1) {
		// Client authenticated successfully

		char menuChoiceString[2];
		int menuChoice = 0;
		char accessMotdString[ACCMOTDLEN];

		// Send access granted tag and motd
		strcpy(accessMotdString, "g/");
		strcat(accessMotdString, ServerInfo->Config->motd);
		write(ServerInfo->clientSocket, accessMotdString, strlen(accessMotdString)+1);

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
					// Client disconnecting
					break;
			}
		}


	} else {
		// User failed authentication
		write(ServerInfo->clientSocket, "Access Denied.", 14);
	}

	// Before exit: remove self from list, close socket and free data
	removeClient(ServerInfo->clientList, ServerInfo);
	close(ServerInfo->clientSocket);
	free(ServerInfo);

	// Log disconnect message and display
	logPipe("Client disconnected", ServerInfo->Config->logFd);
	printf("Client disconnected\n");

	pthread_exit((void*) 0);
	
}
