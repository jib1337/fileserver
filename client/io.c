/* File server application | Jack Nelson | CSP2308
 * io.c
 * Input and output functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "fsClient.h"
#include "files.h"
#include "io.h"
#include "settings.h"

void printWelcome() {
	// Prints the main menu along with the MOTD.

	printf("----------------------------------------\n"
	       "  Welcome to Jack's fileserver client\n"
	       "----------------------------------------\n");
}

void showMainMenuOptions() {
	// Prints the options for the main menu

	printf("\n[1] Connect to server\n[2] View/edit settings\n[3] Exit\n");
	printf("\nSelection: ");
}

void showServerOptions() {
	printf("\n[1] List shared files\n[2] Upload file\n[3] Download file\n[4] Disconnect\n");
	printf("Selection: ");
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
	
	if (inputLength > 2) {
		// If the input length is over two, we want to chomp out a newline.

		inputString[strlen(inputString)-2] = '\0';
	}
	
}

void freeFileList(fileList_t* FileList) {

	if (FileList->fileCount > 0) {

		int i;

		for (i=0; i<FileList->fileCount; i++) {
			free(FileList->sharedFiles[i]);
		}

		free(FileList->sharedFiles);

		FileList->sharedFiles = NULL;
		FileList->fileCount = 0;
	}
}

int getFileList(fileList_t* FileList, int connectionSocket) {
	// Outputs a list of current files in the shared directory.
	
	int i = 0, j = 0, k;
	int fileNum = 1;
	char buffer[1024];
	char c[1];

	freeFileList(FileList);	       

	FileList->sharedFiles = malloc(sizeof(char*) * 20);

	while ((read(connectionSocket, c, 1)) > 0) {

		if (c[0] != '\n') {

			buffer[i] = c[0];
			i++;
			
			if (c[0] == '/') {
				break;
			}

		} else {

			buffer[i] = '\0';

			FileList->sharedFiles[j] = (char*)calloc(1, strlen(buffer)+1);
			strncpy(FileList->sharedFiles[j], buffer, strlen(buffer)+1);
			FileList->fileCount++;
			i=0;
			fileNum++;
			j++;
		}
	}

	
	printf("\nTotal files: %d\n", FileList->fileCount);
	for(k=0; k<FileList->fileCount; k++) {
		printf("%d. %s\n", k+1, FileList->sharedFiles[k]);
	}

	return 0;

		
}

void downloadFileMenu(fileList_t* FileList, char* shareFolder, int connectionSocket) {
	
	char fileNumberString[FILENUMBERSTRINGLEN];
	int fileNumber;

	printf("\nDownload file menu\n");
	// Refresh the file list
	getFileList(FileList, connectionSocket);

	do {
		
		printf("\n[ q = quit to main menu | r = relist files ]\n\nSelect number of the file you wish to download (or option from above)\nSelection: ");
		getKeyboardInput(fileNumberString, FILENUMBERSTRINGLEN);
		fileNumber = atoi(fileNumberString);

		if (strcmp(fileNumberString, "q") == 0) { // User wants to quit

		} else if (strcmp(fileNumberString, "r") == 0) {
			// User wants to relist files
			
			write(connectionSocket, "r", 2);
			if (getFileList(FileList, connectionSocket) == 0) {
				printf("Files relisted.\n");
			}
			continue;
		} else if ((fileNumber <= 0) || (fileNumber > FileList->fileCount)) {

			printf("Error: Invalid selection.\n\n");
			continue;
		} else {
			
			write(connectionSocket, "d", 2);

			char* filePath = (char*)calloc(1, strlen(shareFolder) + strlen(FileList->sharedFiles[fileNumber-1] + 1));
			strncpy(filePath, shareFolder, strlen(shareFolder));
			strcat(filePath, "/");
			strncat(filePath, FileList->sharedFiles[fileNumber-1], strlen(FileList->sharedFiles[fileNumber-1]));
			printf("FILE: %s\n", filePath);
			if (downloadFile(FileList->sharedFiles[fileNumber-1], filePath, connectionSocket) == 0) {
				printf("%s successfully downloaded.\n\n", FileList->sharedFiles[fileNumber-1]);
			} else {
				printf("%s failed to download.\n\n", FileList->sharedFiles[fileNumber-1]);
			}

			free(filePath);
		}
	} while (strcmp(fileNumberString, "q") != 0);

	write(connectionSocket, "q", 2);
}

int downloadFile(char* fileName, char* filePath, int connectionSocket) {

	FILE* fileData;
	char response[14];
	// In theory this allows file sizes of multiple terabyes

	write(connectionSocket, fileName, strlen(fileName)+1);
	read(connectionSocket, response, 14);
	
	if (strcmp(response, "error") == 0) {
		fprintf(stderr, "Error: File was unable to be downloaded\n");
	} else {
		printf("Downloading %s of size %s bytes now...\n", fileName, response);

		char buffer[BUFSIZ];
		size_t recievedBytes;
		int remainingData = atoi(response);
		printf("File Size: %d\n", remainingData);

		fileData = fopen(filePath, "w");

		while ((remainingData > 0) && ((recievedBytes = recv(connectionSocket, buffer, BUFSIZ, 0)) > 0)) {

			fwrite(buffer, sizeof(char), recievedBytes, fileData);
			remainingData -= recievedBytes;
			printf("Remaining: %d\n", remainingData);
		}

		fclose(fileData);

	}

	return 0;
}


