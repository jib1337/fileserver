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
	// Prints the main menu along with the MOTD

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
	// Display the current configuration settings

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
	// Interfaces with the client when they choose to download a file
	
	char menuChoiceString[2];

	// Send the fileList to the server
	listFiles(ServerInfo);

	do {

		read(ServerInfo->clientSocket, menuChoiceString, 2);

		if (strcmp(menuChoiceString, "r") == 0) {
			listFiles(ServerInfo);
		} else if (strcmp(menuChoiceString, "d") == 0) {
			if (sendFile(ServerInfo) != 0) {
				fflush(stdout);
			}
		}
	} while (strcmp(menuChoiceString, "q") != 0);
}

void recieveFileMenu(threadData_t* ServerInfo) {
	// Interfaces with a client when they choose to upload a file

	char menuChoiceString[2];

	read(ServerInfo->clientSocket, menuChoiceString, 2);

	if (strcmp(menuChoiceString, "u") == 0) {
		if (recieveFile(ServerInfo) != 0) {
			fprintf(stderr, "Error - Attempt by client to upload file failed\n");
			fflush(stdout);
			logPipe("Client failed to upload file", ServerInfo->Config->logFd);
		}
	}
}

int recieveFile(threadData_t* ServerInfo) {
	// Recieve a file from the client and write it to storage.
	
	char response[14];
	char fileName[256];

	read(ServerInfo->clientSocket, response, 14);
	
	if (strcmp(response, "error") == 0) {
		// Something went wrong on the client's end

		return 1;
	} else {

		char buffer[BUFSIZ];
		size_t recievedBytes;
		int remainingData = atoi(response);

		// Set up file lock struct
		struct flock fileLock;
		memset(&fileLock, 0, sizeof(fileLock));
		fileLock.l_type = F_WRLCK;

		read(ServerInfo->clientSocket, fileName, 256);

		char* filePath = calloc(1, strlen(ServerInfo->Config->shareFolder) + strlen(fileName) + 2);

		strcpy(filePath, ServerInfo->Config->shareFolder);
		strcat(filePath, "/");
		strcat(filePath, fileName);

		if ((remainingData > 0) && ((ServerInfo->recFd = open(filePath, O_WRONLY | O_CREAT | O_EXCL)) > 0)) {
			/* If the response was valid and the file was able to be opened without errors
			 * Lock the file and let the client know we're ready to recieve */
			fcntl(ServerInfo->recFd, F_SETLKW, &fileLock);
			write(ServerInfo->clientSocket, "ok", 3);
		} else {
			// Something went wrong, inform the client
			write(ServerInfo->clientSocket, "sz", 3);
			return 1;
		}

		while ((remainingData > 0) && ((recievedBytes = recv(ServerInfo->clientSocket, buffer, BUFSIZ, 0)) > 0)) {

			write(ServerInfo->recFd, buffer, recievedBytes);
			remainingData -= recievedBytes;
		}
		
		// Close file and unlock
		close(ServerInfo->recFd);
		ServerInfo->recFd = -1;
		fileLock.l_type = F_UNLCK;
		fcntl(ServerInfo->recFd, F_SETLKW, &fileLock);
		
		// Log the download
		char* logString = calloc(1, strlen(fileName) + 18);
		sprintf(logString, "File: %s recieved", fileName);
		logPipe(logString, ServerInfo->Config->logFd);
		printf("%s\n", logString);

		free(logString);
		free(filePath);
	}

	return 0;
}


int sendFile(threadData_t* ServerInfo) {
	// Send a file through a socket to a client
	
	char fileName[255];
	char fileSizeString[14];
	char* filePath = calloc(1, strlen(ServerInfo->Config->shareFolder) + 257);
	struct flock fileLock;
	memset(&fileLock, 0, sizeof(fileLock));
	fileLock.l_type = F_RDLCK;

	// Get the filename from the client
	read(ServerInfo->clientSocket, fileName, 255);

	// Create the file path
	strcpy(filePath, ServerInfo->Config->shareFolder);
	strcat(filePath, "/");
	strcat(filePath, fileName);

	if (((checkAccess(filePath)) >= 4) && ((ServerInfo->sendFd = open(filePath, O_RDONLY)) > 0)) {
		// File exists with the correct permissions and opened with no errors
		
		fcntl(ServerInfo->sendFd, F_SETLKW, &fileLock);
		
		struct stat fileStats;
		fstat(ServerInfo->sendFd, &fileStats);
		int sentBytes = 0;
		off_t offset = 0;
		long dataRemaining = fileStats.st_size;

		// Send the file size to the client - also lets them know the file is accessable and can be sent
		sprintf(fileSizeString, "%ld", fileStats.st_size);
		write(ServerInfo->clientSocket, fileSizeString, 14);

		while (((sentBytes = sendfile(ServerInfo->clientSocket, ServerInfo->sendFd, &offset, BUFSIZ)) > 0)
					&& (dataRemaining > 0)) {

			dataRemaining -= sentBytes;
		}

		// Close and remove the lock
		close(ServerInfo->sendFd);
		ServerInfo->sendFd = -1;
		fileLock.l_type = F_UNLCK;
		fcntl(ServerInfo->sendFd, F_SETLKW, &fileLock);

		// Log the operation
		char* logString = calloc(1, strlen(fileName) + 28);
		sprintf(logString, "File: %s downloaded by client", fileName);
		logPipe(logString, ServerInfo->Config->logFd);
		printf("%s\n", logString);
		free(logString);

	} else {
		/* If the file fails to open, we cannot proceed
		 * so let the client know so they can handle on their end */

		write(ServerInfo->clientSocket,  "error", 6);

		// Log and display a message
		fprintf(stderr, "Error - %s unable to be sent to client\n", fileName);
		char* logString = calloc(1, strlen(fileName)+35);
		sprintf(logString, "File: %s failed to be sent to client", fileName);
		logPipe(logString, ServerInfo->Config->logFd);
		free(logString);

		return 1;
	}

	// Close and clean up
	close(ServerInfo->sendFd);
	ServerInfo->sendFd = -1;
	free(filePath);

	return 0;
}

