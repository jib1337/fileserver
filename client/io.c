/* File server application | Jack Nelson | CSP2308
 * io.c
 * Input and output functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int listFiles(fileList_t* FileList, char* shareFolder) {
	// Outputs a list of current files in the shared directory.
	
	// To ensure we have the most up to date list, we'll call getFiles here to refresh the list.
	getFiles(FileList, shareFolder);

	int i;

	if (FileList->fileCount == 0) {
		// Print an error message if folder is empty or non existant
		fprintf(stderr, "Error: No files to list\n");

		return 1; // Indicates failure to read filelist

	} else {

		printf("\n");
		for(i=0;i<(FileList->fileCount); i++) {
		
			printf("%d.\t%s\n", i+1, FileList->sharedFiles[i]);
		}

		return 0; // Successful reading of filelist
	}
}
