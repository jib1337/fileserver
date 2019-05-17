/* File server application | Jack Nelson | CSP2308
 * settings.c
 * Management of configuration file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>

#include "fileServer.h"
#include "settings.h"
#include "files.h"
#include "io.h"

// static global for the Config
static config_t* g_Config;

void* signalListener(void* Config) {
	
	// Set the global
	g_Config = (config_t*) Config;


	// In here we listen for signals to arrive then do stuff when they do.
	// We have access to the global to do whatever is required.
	printf("%d\n", g_Config->portNumber);
	
	pthread_exit((void*) 0);

}

void configReload(config_t* g_config) {
	// Reload the config stuff in here?
}

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

config_t configCheck(int* configStatus) {
	/* Check the existance of the config file and it's contents. Either return a config struct
	 * with the file's settings, or a struct with the default settings applied. */

	config_t Config = {DEF_PORTNUM, DEF_IPADDRESS, DEF_SHAREFOLDER, DEF_MOTD, DEF_LOGFILE, DEF_CREDS};
	int fileAccess;
	FILE* configFile;
	char portString[6];

	if ((fileAccess = checkAccess("settings.conf")) > -1) {
		// File exists
		if ((fileAccess >= 4) && ((configFile = fopen("settings.conf", "r")) != NULL)) {
			// File has the right permissions and opened okay
			configRead(portString, 6, configFile);
			Config.portNumber = atoi(portString);
			configRead(Config.ipAddress, 16, configFile);
			configRead(Config.shareFolder, 255, configFile);
			configRead(Config.motd, MAXMOTDLEN, configFile);
			configRead(Config.logFile, 255, configFile);
			configRead(Config.serverCreds, 76, configFile);
			fclose(configFile);

			// Attempt to create the share folder. If this fails, either we don't have access
			// or the folder already exists. In the case of the former, the user will recieve
			// an error message informing them the directory doesnt exist when they try to list files.
			mkdir(Config.shareFolder, S_IRWXU | S_IRWXG | S_IRWXO);

		} else {
			// The file had the wrong permissions or didn't open
			perror("Error - Settings file not readable");
			exit(EXIT_FAILURE);
		}
	} else {
		// File doesn't exist
		configWrite(&Config);
		*configStatus = 1;

		// Create the default sharefolder.
		mkdir(DEF_SHAREFOLDER, S_IRWXU | S_IRWXG | S_IRWXO);
	}

	return Config;
}

void configWrite(config_t* Config) {
	// Write out the config file with current config settings in memory.
	
	FILE* configFile = fopen("settings.conf", "w");
	fprintf(configFile, "%hi\n%s\n%s\n%s\n%s\n%s\n", Config->portNumber, Config->ipAddress, Config->shareFolder, 
			Config->motd, Config->logFile, Config->serverCreds);
	fclose(configFile);
}

int splitCredentials(char* credString, char* username, char* passwordHash) {
	
	int u, i, j=0;

	//printf("%s\n", credString);

	for (u=0; u<strlen(credString); u++) {
		if (credString[u] == ':') {

			if (u == 0) return 1;
			break;
		}
	}
	
	for (i=0; i<strlen(credString); i++) {
		if (i <= u) {
			username[i] = credString[i];

			if (i == u) {
				username[i] = '\0';
			}

		} else {
			passwordHash[j] = credString[i];
			j++;

			if (i+1 == strlen(credString)) {
				passwordHash[j] = '\0';
			}
		}
	}	
	
	//printf("%s and %s\n", username, passwordHash);

	return 0;
}
