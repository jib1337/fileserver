/* File server application | Jack Nelson | CSP2308
 * files.c
 * File list handling and reading functions */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "fileServer.h"
#include "files.h"

void listFiles(threadData_t* serverInfo) {

	DIR *d;
	struct dirent* dir;
	d = opendir(serverInfo->Config->shareFolder);
	char buffer[1024];
	
	if (d) {

		/* No need to do this.

		while ((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] == '.') continue;
			FileList->fileCount++;
		}

		rewinddir(d);

		FileList->sharedFiles = malloc(sizeof(char*) * FileList->fileCount);
		*/

		while ((dir = readdir(d)) != NULL) {

			// If the filename begins with a period, we don't want to display it. This means that self and parent
			// are skipped, and also allows for hidden files to be placed in the directory that won't be shared.
			if (dir->d_name[0] == '.') continue;

			strncpy(buffer, dir->d_name, strlen(dir->d_name));

			buffer[strlen(dir->d_name)] = '\n';
			buffer[strlen(dir->d_name)+1] = '\0';

			write(serverInfo->clientSocket, buffer, strlen(buffer));
			
			//printf("%s", buffer);

			//FileList->sharedFiles[i] = (char*)calloc(1, strlen(dir->d_name)+1);
			//strncpy(FileList->sharedFiles[i], dir->d_name, strlen(dir->d_name));
			//i++;
		}

		closedir(d);

		write(serverInfo->clientSocket, "/", 1);
		fflush(stdout);

	} else {

		perror("Error: Directory unable to be scanned");
	}
}

/*int fileSize(int fd) {
	// Return the byte size of a file

	lseek(file, 0, 2);
	
	return(tell(file));
}*/

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
