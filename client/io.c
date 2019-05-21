/* File server application | Jack Nelson | CSP2308
 * io.c
 * Input and output functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/sendfile.h>

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

void showSettings(config_t* Config) {

	printf("\nCurrent configuration:\n");
	printf("[1] Username:\t\t%s\n[2] Port:\t\t%d\n[3] Server IP:\t\t%s\n[4] Shared Folder:\t%s\n",
			Config->username, Config->serverPort, Config->serverIP, Config->shareFolder);
}


void editSettings(config_t* Config) {

	int settingNumber;
	char settingNumberString[2];
	char updatedSettingString[256];

	do {
		showSettings(Config);
		printf("\n[ q = save and quit to main menu ]\nSelect the number of the setting you wish to change (or option from above)\nSelection: ");
		getKeyboardInput(settingNumberString, 2);
		settingNumber = atoi(settingNumberString);

		switch (settingNumber) {

			case(1):
				printf("Username: ");
				getKeyboardInput(Config->username, 11);
				break;

			case(2):
				printf("Port: ");
				getKeyboardInput(updatedSettingString, 6);
				
				if (atoi(updatedSettingString) != 0) {
					Config->serverPort = atoi(updatedSettingString);
				} else {
					printf("\nError: Invalid port number\n");
				}

				break;

			case(3):
				printf("Server IP Address: ");
				getKeyboardInput(Config->serverIP, 16);
				break;

			case(4):
				printf("Shared Folder: ");
				getKeyboardInput(Config->shareFolder, 256);
				break;
			
			case(0):
				if (strcmp(settingNumberString, "q") != 0) {
					printf("\nError: Invalid selection\n");
				}
		}

	} while (strcmp(settingNumberString, "q") != 0);

	configWrite(Config);
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

int getFileList(fileList_t* FileList, int connectionSocket) {
	// Outputs a list of current files in the shared directory.
	
	int i = 0, j = 0, k;
	int fileNum = 1;
	char buffer[1024];
	char c[1];

	fileCleanup(FileList);	       

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

void uploadFileMenu(fileList_t* FileList, char* shareFolder, int connectionSocket) {
	
	char fileNumberString[FILENUMBERSTRINGLEN];
	int fileNumber;

	printf("\nUpload file menu\n");

	// Refresh the file list
	listFiles(FileList, shareFolder);

	do {
		
		printf("\n[ q = quit to main menu | r = relist files ]\n\nSelect number of the file you wish to upload (or option from above)\nSelection: ");
		getKeyboardInput(fileNumberString, FILENUMBERSTRINGLEN);
		fileNumber = atoi(fileNumberString);

		if (strcmp(fileNumberString, "q") == 0) { // User wants to quit

		} else if (strcmp(fileNumberString, "r") == 0) {
			// User wants to relist files
			
			if (listFiles(FileList, shareFolder) == 0) {
				printf("Files relisted.\n");
			}
			continue;
		} else if ((fileNumber <= 0) || (fileNumber > FileList->fileCount)) {

			printf("Error: Invalid selection.\n\n");
			continue;
		} else {
			
			printf("Getting file: %s\n", FileList->sharedFiles[fileNumber-1]);

			write(connectionSocket, "u", 2);

			char* filePath = malloc(strlen(shareFolder) + strlen(FileList->sharedFiles[fileNumber-1]) + 2);

			// Create the file path
			strcpy(filePath, shareFolder);
			strcat(filePath, "/");
			strcat(filePath, FileList->sharedFiles[fileNumber-1]);

			if (uploadFile(FileList->sharedFiles[fileNumber-1], filePath, connectionSocket) == 0) {
				printf("%s successfully uploaded.\n\n", FileList->sharedFiles[fileNumber-1]);
			} else {
				printf("%s failed to uploaded\n\n", FileList->sharedFiles[fileNumber-1]);
			}

			free(filePath);
		}
	} while (strcmp(fileNumberString, "q") != 0);

	write(connectionSocket, "q", 2);
}

int uploadFile(char* fileName, char* filePath, int connectionSocket) {

	char fileSizeString[14];
	int localFile;

	if (((checkAccess(filePath)) >= 4) && ((localFile = open(filePath, O_RDONLY)) > 0)) {
		// File exists with the correct permissions and opened with no errors
		
		struct stat fileStats;
		fstat(localFile, &fileStats);
		char serverClearance[3];
	
		int sentBytes = 0;
		off_t offset = 0;
		long dataRemaining = fileStats.st_size;

		// Send the file size to the client - also lets them know the file is accessable and can be sent
		sprintf(fileSizeString, "%ld", fileStats.st_size);

		// Write the filename and size to the socket
		write(connectionSocket, fileSizeString, 14);
		write(connectionSocket, fileName, strlen(fileName)+1);

		read(connectionSocket, serverClearance, 3);

		if (strcmp(serverClearance, "ok") == 0) {
		
			printf("Data Remaining: %ld\n", dataRemaining);

			while (((sentBytes = sendfile(connectionSocket, localFile, &offset, BUFSIZ)) > 0)
						&& (dataRemaining > 0)) {

				dataRemaining -= sentBytes;
				printf("Data Remaining: %ld\n", dataRemaining);
			}

			printf("Done.\n");

		} else {

			return 1;
		}
			

	} else {
		// If the file fails to open, we cannot proceed, so let the server know so they can handle on their end.

		write(connectionSocket,  "error", 6);
		return 1;	
	}

	// Close the file and free up the file path.
	close(localFile);

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


