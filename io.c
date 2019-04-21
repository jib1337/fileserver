/* File server application | Jack Nelson | CSP2308
 * io.c
 * Input and output functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

void showMainMenuOptions(short portNumber) {
	// Prints the options for the main menu

	if (portNumber == 0) {
		printf("\n[1] Start server\n");
	} else {
		printf("Server listening on port %hi...\n", portNumber);
	}

	printf("[2] Set server password\n[3] List hosted files\n[4] Shutdown Server / Exit\n");
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

void printFileContent (int* fileNum, char* fileName, config_t* Config, int log) {
	// Read a hosted file and display it's contents on screen

	// To make the file path, we allocate enough space for both the directory and filename.
	char* filePath = malloc(strlen(Config->shareFolder) + strlen(fileName) + 2);
	char c;

	// Create the file path
	strcpy(filePath, Config->shareFolder);
	strcat(filePath, "/");
	strcat(filePath, fileName);

	printf("\nContents of %s\n------------------------------------------------------------\n\n", filePath);

	FILE* hostedFile;

	if (((checkAccess(filePath)) >= 4) && ((hostedFile = fopen(filePath, "r")) != NULL)) {
		// File exists with the correct permissions and opened with no errors

		c = fgetc(hostedFile);

		while (c != EOF) {

			printf("%c", c);
			c = fgetc(hostedFile);
		}

		fclose(hostedFile);
		logPipe("File contents outputted", log);

	} else {

		perror("Error - Could not open file");
		logPipe("Attempt to read file contents failed.", log);
	}

	printf("\n------------------------------------------------------------\n");
	free(filePath);
}

