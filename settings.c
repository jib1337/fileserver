/* File server application | Jack Nelson | CSP2308
 * settings.c
 * Management of configuration file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>

#include "fileServer.h"
#include "settings.h"
#include "io.h"
#include "logger.h"

// Static globals for the config and a settings file pointer for signal handling
static config_t* g_Config;
static FILE* g_configFile = NULL;

void signalShutdown() {
	// Shut down the server if signal recieved in main menu
	
	signal(SIGTERM, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);

	// If the settings file is currently open, close it
	if (g_configFile != NULL) {
		printf("closing file!");
		fclose(g_configFile);
	}

	logPipe("Program shut down", g_Config->logFd);
	printf("\nShut down via signal\n");
	
	wait(NULL);
	exit(0);
}

void setConfigHandler(config_t* Config) {
	// Set the global config pointer for the SIGHUP signal
	
	g_Config = (config_t*) Config;
	signal(SIGHUP, configReload);
}

void configReload() {
	// Reload the configuration from the settings file

	signal(SIGHUP, SIG_IGN);

	int fileAccess;
	char portString[6];

	if ((fileAccess = checkAccess("settings.conf")) > -1) {
		// File exists
		if ((fileAccess >= 4) && ((g_configFile = fopen("settings.conf", "r")) != NULL)) {
			// File has the right permissions and opened okay
			
			configRead(portString, 6, g_configFile);
			g_Config->portNumber = atoi(portString);
			configRead(g_Config->ipAddress, 16, g_configFile);
			configRead(g_Config->shareFolder, 255, g_configFile);
			configRead(g_Config->motd, MAXMOTDLEN, g_configFile);
			configRead(g_Config->logFile, 255, g_configFile);
			configRead(g_Config->serverCreds, 76, g_configFile);
			fclose(g_configFile);
			g_configFile = NULL;

			logPipe("Updated server configuration from file", g_Config->logFd);
			printf("\nUpdated server configuration from file\n");

		} else {
			// The file had the wrong permissions or didn't open
			
			logPipe("Error on attempt to update configuration", g_Config->logFd);
			perror("\nError - Could not update configuration");
		}
	} else {
		// File doesn't exist
		
		logPipe("Error on attempt to update configuration", g_Config->logFd);
		perror("\nError - Could not update configuration");
	}
	
	fflush(stdout);
	signal(SIGHUP, configReload);
}

void configRead(char* input, int msgLen, FILE* file) {
	// Reads a line from the config file. If a line cannot be read, the program will exit
	
	char ch;

	if ((fgets(input, msgLen, file)) == NULL) {
		
		// Exit gracefully if line is not retrieved successfully
		fprintf(stderr, "Error - Settings file data missing\n");
		fclose(file);
		exit(EXIT_FAILURE);
	}

	if (input[strlen(input)-1] == '\n') {
		input[strlen(input)-1] = '\0';

	} else {
		while ((ch = fgetc(file) != '\n') && (ch != EOF));
	}

}

config_t configCheck(int* configStatus) {
	/* Check the existance of the config file and it's contents. Either return a config struct
	 * with the file's settings, or a struct with the default settings applied */

	config_t Config = {DEF_PORTNUM, DEF_IPADDRESS, DEF_SHAREFOLDER, DEF_MOTD, DEF_LOGFILE, DEF_CREDS};
	int fileAccess;
	char portString[6];

	if ((fileAccess = checkAccess("settings.conf")) > -1) {
		// File exists
		if ((fileAccess >= 4) && ((g_configFile = fopen("settings.conf", "r")) != NULL)) {
			// File has the right permissions and opened okay

			configRead(portString, 6, g_configFile);
			Config.portNumber = atoi(portString);
			configRead(Config.ipAddress, 16, g_configFile);
			configRead(Config.shareFolder, 255, g_configFile);
			configRead(Config.motd, MAXMOTDLEN, g_configFile);
			configRead(Config.logFile, 255, g_configFile);
			configRead(Config.serverCreds, 76, g_configFile);
			fclose(g_configFile);
			g_configFile = NULL;

			// Attempt to create the share folder. If this fails, either we don't have access
			// or the folder already exists. In the case of the former, the user will recieve
			// an error message informing them the directory doesnt exist when they try to read from it
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
	
	int fileAccess;

	if ((((fileAccess = checkAccess("settings.conf")) == -1) || (fileAccess >= 2)) &&
			((g_configFile = fopen("settings.conf", "w")) != NULL)) {
		// Either the file doesn't exist or has the right permissions and was able to be opened with no errors
			
		fprintf(g_configFile, "%hi\n%s\n%s\n%s\n%s\n%s\n", Config->portNumber, Config->ipAddress, Config->shareFolder, 
			Config->motd, Config->logFile, Config->serverCreds);
		fclose(g_configFile);
		g_configFile = NULL;
	} else {
				perror("\nError - Settings file not writable");
	}
}
