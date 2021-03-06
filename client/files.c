/* File server application | Jack Nelson | CSP2308
 * files.c
 * File list handling and reading functions */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#include "fsClient.h"
#include "files.h"

void fileCleanup(fileList_t* FileList) {
	// Frees up all memory utilized by the current file list
	
	int i;

	if (FileList->sharedFiles != NULL) {
		// We won't bother doing this if we havent allocated any memory yet

		for (i=0;i<FileList->fileCount;i++) {

			free(FileList->sharedFiles[i]);
		}

		free(FileList->sharedFiles);
		FileList->sharedFiles = NULL;
		FileList->fileCount = 0;
	}
}

int isFolder(char* shareFolder, char* fileName) {
	// Check if a file is a folder or not, so we can avoid listing them
	
	struct stat statBuffer;
	char* filePath = calloc(1,strlen(shareFolder) + strlen(fileName)+2);

	sprintf(filePath, "%s/%s", shareFolder, fileName);
	
	if ((stat(filePath, &statBuffer) != -1) && (S_ISDIR(statBuffer.st_mode))) {
		// We were able to get stats for the file and determine it isn't a folder
		
		free(filePath);
		return 1;
	} else {

		free(filePath);
		return 0;
	}
}

void getFiles(fileList_t* FileList, char* shareFolder) {
	/* Retrieves names of files from the shared folder and stores them in a filelist struct
	 * If we already have information in the list, we'll free up that memory before retrieving again
	 * This function can be used anywhere to refresh the file list */

	fileCleanup(FileList);

	int i = 0;
	DIR *d;
	struct dirent* dir;
	d = opendir(shareFolder);
	
	if (d) {

		while ((dir = readdir(d)) != NULL) {
			if (isFolder(shareFolder, dir->d_name) == 1) continue;
			if (dir->d_name[0] == '.') continue;
			FileList->fileCount++;
		}

		rewinddir(d);

		FileList->sharedFiles = malloc(sizeof(char*) * FileList->fileCount);

		while ((dir = readdir(d)) != NULL) {

			// If the filename begins with a period, we don't want to display it. This means that self and parent
			// are skipped, and also allows for hidden files to be placed in the directory that won't be shared.
			if (isFolder(shareFolder, dir->d_name) == 1) continue;
			if (dir->d_name[0] == '.') continue;

			FileList->sharedFiles[i] = (char*)calloc(1, strlen(dir->d_name)+1);
			strncpy(FileList->sharedFiles[i], dir->d_name, strlen(dir->d_name));
			i++;
		}

		closedir(d);

	} else {

		perror("Error: Directory unable to be scanned");
	}
}

int checkAccess(char* fileName) {
	// Checks the read, write and execute persmissions of a file and returns a number that represents the current access.
	// Similar to the octal value in chmod but not done with bits.

	if (access(fileName, F_OK)) {
		// File doesn't exist so we can end it here

		return -1;
	} else {

		int accessNumber = 0;
	
		if (!access(fileName, R_OK)) {
			accessNumber += 4;
		}

		if (!access(fileName, W_OK)) {
			accessNumber += 2;
		}

		if (!access(fileName, X_OK)) {
			accessNumber++;
		}

		return accessNumber;
	}
}
