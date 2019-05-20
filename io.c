/* File server application | Jack Nelson | CSP2308
 * io.c
 * Input and output functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/sendfile.h>

#include "fileServer.h"
#include "files.h"
#include "logger.h"
#include "io.h"
#include "settings.h"
#include "security.h"

void printWelcome(char* motd) {
	// Prints the main menu along with the MOTD.

	printf("----------------------------------------\n"
	       "  Welcome to Jack's fileshare server!\n"
	       "----------------------------------------\n"
	       "Message of the day:\n"
	       "%s\n"
	       "----------------------------------------\n", motd);
}

void showMainMenuOptions() {
	// Prints the options for the main menu

	printf("[1] Start server\n[2] Set server credentials\n[3] Display configuration\n[4] Exit\n");
	printf("\nSelection: ");
}

void displaySettings(config_t* Config) {
	printf("\nCurrent configuration:\n");
	printf("IP:\t\t%s\nPort:\t\t%d\nShared Folder:\t%s\nMOTD:\t\t%s\n\n",
			Config->ipAddress, Config->portNumber, Config->shareFolder, Config->motd);
}

void getKeyboardInput(char* inputString, int inputLength) {
	// Gets and stores user keyboard input in a given string

	char ch;

	fgets(inputString, inputLength, stdin);

	if (inputString[strlen(inputString)-1] == '\n') {
		inputString[strlen(inputString)-1] = '\0';
	} else {
		while ((ch = getchar() != '\n') && (ch != EOF));
	}
}

void sendFileMenu(threadData_t* ServerInfo) {
	
	char menuChoiceString[2];

	// Send the fileList to the server
	listFiles(ServerInfo);

	do {

		read(ServerInfo->clientSocket, menuChoiceString, 2);

		if (strcmp(menuChoiceString, "r") == 0) {
			listFiles(ServerInfo);
		} else if (strcmp(menuChoiceString, "d") == 0) {
			sendFile(ServerInfo);
		}
	} while (strcmp(menuChoiceString, "q") != 0);
}

void recieveFileMenu(threadData_t* ServerInfo) {

	char menuChoiceString[2];

	read(ServerInfo->clientSocket, menuChoiceString, 2);

	if (strcmp(menuChoiceString, "u") == 0) {
		recieveFile(ServerInfo);
	}
}

int recieveFile(threadData_t* ServerInfo) {
	// Recieve a file from the client
	
	char response[14];
	char fileName[256];

	// In theory this allows file sizes of multiple terabyes

	read(ServerInfo->clientSocket, response, 14);
	
	if (strcmp(response, "error") == 0) {
		fprintf(stderr, "Log file error\n");
	} else {
		printf("Recieving file %s of size %s bytes now...\n", fileName, response);

		char buffer[BUFSIZ];
		size_t recievedBytes;
		int remainingData = atoi(response);
		printf("File Size: %d\n", remainingData);

		read(ServerInfo->clientSocket, fileName, 256);

		printf("File name: %s\n", fileName);

		char* filePath = calloc(1, strlen(ServerInfo->Config->shareFolder) + strlen(fileName) + 2);

		strcpy(filePath, ServerInfo->Config->shareFolder);
		strcat(filePath, "/");
		strcat(filePath, fileName);

		printf("File path: %s\n", filePath);

		// let client know its ok to send the file
		if (remainingData > 0) {
			write(ServerInfo->clientSocket, "ok", 3);
		} else {
			write(ServerInfo->clientSocket, "sz", 3);
		}

		ServerInfo->recFp = fopen(filePath, "w");

		while ((remainingData > 0) && ((recievedBytes = recv(ServerInfo->clientSocket, buffer, BUFSIZ, 0)) > 0)) {

			fwrite(buffer, sizeof(char), recievedBytes, ServerInfo->recFp);
			remainingData -= recievedBytes;
			printf("Remaining: %d\n", remainingData);
		}

		printf("Done.\n");

		fclose(ServerInfo->recFp);
		ServerInfo->recFp = NULL;

		free(filePath);

	}

	return 0;
}


int sendFile(threadData_t* ServerInfo) {
	// Read a hosted file and display it's contents on screen

	// To make the file path, we allocate enough space for both the directory and filename.
	char fileName[255];
	char fileSizeString[14];
	char* filePath = calloc(1, strlen(ServerInfo->Config->shareFolder) + 257);

	read(ServerInfo->clientSocket, fileName, 255);

	// Create the file path
	strcpy(filePath, ServerInfo->Config->shareFolder);
	strcat(filePath, "/");
	strcat(filePath, fileName);

	printf("Sending file: %s\n", filePath);

	if (((checkAccess(filePath)) >= 4) && ((ServerInfo->sendFd = open(filePath, O_RDONLY)) > 0)) {
		// File exists with the correct permissions and opened with no errors
		
		struct stat fileStats;
		fstat(ServerInfo->sendFd, &fileStats);
	
		int sentBytes = 0;
		off_t offset = 0;
		long dataRemaining = fileStats.st_size;

		// Send the file size to the client - also lets them know the file is accessable and can be sent
		sprintf(fileSizeString, "%ld", fileStats.st_size);
		write(ServerInfo->clientSocket, fileSizeString, 14);
		
		printf("Data Remaining: %ld\n", dataRemaining);

		while (((sentBytes = sendfile(ServerInfo->clientSocket, ServerInfo->sendFd, &offset, BUFSIZ)) > 0)
					&& (dataRemaining > 0)) {

			dataRemaining -= sentBytes;
			printf("Data Remaining: %ld\n", dataRemaining);
		}

		printf("Done.");
			

	} else {
		// If the file fails to open, we cannot proceed, so let the client know so they can handle on their end.

		write(ServerInfo->clientSocket,  "error", 6); 
	}

	// Close the file and free up the file path.
	close(ServerInfo->sendFd);
	ServerInfo->sendFd = -1;

	free(filePath);

	return 0;
}

