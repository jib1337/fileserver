/* File server application | Jack Nelson | CSP2308
 * io.c
 * Input and output functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

#include "fileServer.h"
#include "files.h"
#include "logger.h"
#include "io.h"
#include "settings.h"
#include "security.h"

void controlLogin(config_t* Config, int configStatus) {
	/* Login screen for access to server control
	 * Also allows for registering of a server username and password.
	 *
	 * Note that if something interrupts the program between the config file being created
	 * and new credentials being set, the server will use hardcoded defaults of admin:password */

	char username[11];
	char password[31];

	printf("----------------------------------------\n"
	       "       Server control panel login       \n"
	       "----------------------------------------\n");

	if (configStatus == 1) {
		printf("No saved credentials: Please register some now!\n");
		setCredentials(Config);
		printf("\nNow test your created credentials by logging in below.\n\n");
	}

	printf("Username: ");
	getKeyboardInput(username, 11);
	printf("Password: ");
	getKeyboardInput(password, 31);

	if (authenticate(Config->serverCreds, username, password) == 1) {
		printf("Access granted.\n\n");
	} else {
		printf("Access denied.\n");
		exit(EXIT_FAILURE);
	}
}

int clientLogin(threadData_t* serverInfo) {

	//char buffer[256];
	char username[11];
	char password[31];

	bzero(username, 11);
	bzero(password, 31);

	//printf("\n");
	//bzero(buffer, 256);

	//write(serverInfo->clientSocket, "Username: ", 11);
	//read(serverInfo->clientSocket, buffer, sizeof(buffer));
	//strncpy(username, buffer, 10);
	getSocketInput(username, 11, serverInfo->clientSocket);

	//bzero(buffer, 256);

	write(serverInfo->clientSocket, "Password: ", 11);
	//read(serverInfo->clientSocket, buffer, sizeof(buffer));
	//strncpy(password, buffer, 30);
	getSocketInput(password, 31, serverInfo->clientSocket);

	//REMOVE THESE WHEN CLIENT SIDE IS DONE!
	// Not sure why the hell its making me chomp at two?
	//username[strlen(username)-2] = '\0';
	//password[strlen(password)-2] = '\0';
	//printf("%s, %ld", username, strlen(username));
	//printf("%s, %ld", password, strlen(password));

	return(authenticate(serverInfo->Config->serverCreds, username, password));
}

void printWelcome(char* motd) {
	// Prints the main menu along with the MOTD.

	printf("----------------------------------------\n"
	       "  Welcome to Jack's fileshare server!\n"
	       "----------------------------------------\n"
	       "Message of the day:\n"
	       "%s\n"
	       "----------------------------------------\n", motd);
}

void showMainMenuOptions(int serverStarted) {
	// Prints the options for the main menu

	if (serverStarted == 0) {
		printf("\n[1] Start server\n");
	} else {
		printf("\n[1] Stop server\n");
	}

	printf("[2] Set server credentials\n[3] Shutdown Server / Exit\n");
	printf("\nSelection: ");
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

void getSocketInput(char* inputString, int inputLength, int sockFd) {
	// Gets and stores input from a socket file descriptor

	read(sockFd, inputString, inputLength);
	
	//if (inputLength > 2) {
		// If the input length is over two, we want to chomp out a newline.

	//	inputString[strlen(inputString)-2] = '\0';
	//}
	
}

void sendFileMenu(threadData_t* ServerInfo) {
	
	char menuChoiceString[2];

	// Send the fileList to the server
	listFiles(ServerInfo);

	do {

		getSocketInput(menuChoiceString, 2, ServerInfo->clientSocket);

		if (strcmp(menuChoiceString, "r") == 0) {
			listFiles(ServerInfo);
		} else if (strcmp(menuChoiceString, "d") == 0) {
			sendFile(ServerInfo);
		}
	} while (strcmp(menuChoiceString, "q") != 0);

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

	int hostedFile;

	if (((checkAccess(filePath)) >= 4) && ((hostedFile = open(filePath, O_RDONLY)) > 0)) {
		// File exists with the correct permissions and opened with no errors
		
		struct stat fileStats;
		fstat(hostedFile, &fileStats);
	
		int sentBytes = 0;
		off_t offset = 0;
		long dataRemaining = fileStats.st_size;

		// Send the file size to the client - also lets them know the file is accessable and can be sent
		sprintf(fileSizeString, "%ld", fileStats.st_size);
		write(ServerInfo->clientSocket, fileSizeString, 14);
		
		printf("Data Remaining: %ld\n", dataRemaining);

		while (((sentBytes = sendfile(ServerInfo->clientSocket, hostedFile, &offset, BUFSIZ)) > 0)
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
	close(hostedFile);
	free(filePath);

	return 0;
}

