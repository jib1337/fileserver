/* File server application | Jack Nelson | CSP2308
 * settings.c
 * Management of configuration file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "fsClient.h"
#include "settings.h"
#include "files.h"
#include "io.h"

void configRead(char* input, int msgLen, FILE* file) {
	// Reads a line from the config file. If a line cannot be read, the program will exit.
	
	char ch;

	if ((fgets(input, msgLen, file)) == NULL) {
		
		// Exit gracefully if line is not retrieved successfully
		fprintf(stderr, "Error - Settings file data missing\n");
		exit(EXIT_FAILURE);
	}

	if (input[strlen(input)-1] == '\n') {
		
		// Chomp the newline
		input[strlen(input)-1] = '\0';

	} else {

		// Flush the file buffer
		while ((ch = fgetc(file) != '\n') && (ch != EOF));
	}

}

config_t configCheck() {
	/* Check the existance of the config file and it's contents. Either return a config struct
	 * with the file's settings, or a struct with the default settings applied. */

	config_t Config = {DEF_USERNAME, DEF_PORTNUM, DEF_IPADDRESS, DEF_SHAREFOLDER};
	int fileAccess;
	FILE* configFile;
	char portString[6];

	if ((fileAccess = checkAccess("settings.conf")) > -1) {
		// File exists
		if ((fileAccess >= 4) && ((configFile = fopen("settings.conf", "r")) != NULL)) {
			// File has the right permissions and opened okay
			configRead(Config.username, 11, configFile);
			configRead(portString, 6, configFile);
			Config.serverPort = atoi(portString);
			configRead(Config.serverIP, 16, configFile);
			configRead(Config.shareFolder, 255, configFile);
			fclose(configFile);

			/* Attempt to create the share folder. If this fails, either we don't have access
			 * or the folder already exists. In the case of the former, the user will recieve
			 * an error message informing them the directory doesnt exist when trying to list files */
			mkdir(Config.shareFolder, S_IRWXU | S_IRWXG | S_IRWXO);

		} else {
			// The file had the wrong permissions or didn't open
			perror("Error - Settings file not readable");
			exit(EXIT_FAILURE);
		}
	} else {
		// File doesn't exist
		configWrite(&Config);
		
		// Attempt to create the default shared folder
		mkdir(DEF_SHAREFOLDER, S_IRWXU | S_IRWXG | S_IRWXO);
	}

	return Config;
}

void configWrite(config_t* Config) {
	// Write out the config file with current config settings in memory.
	
	int fileAccess;
	FILE* configFile;

	if ((((fileAccess = checkAccess("settings.conf")) == -1) || (fileAccess >= 2)) &&
			((configFile = fopen("settings.conf", "w")) != NULL)) {
		// Either the file doesn't exist or has the right permissions and was able to be opened with no errors

		fprintf(configFile, "%s\n%hi\n%s\n%s\n", Config->username, Config->serverPort, Config->serverIP, Config->shareFolder);
		fclose(configFile);
	} else {
		perror("Error - Settings file not writable");
	}
}
