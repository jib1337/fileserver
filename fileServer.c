/* File server application | Jack Nelson | CSP2308
 * fileServer.c
 * Main program logic */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "fileServer.h"
#include "files.h"
#include "logger.h"
#include "io.h"
#include "settings.h"
#include "security.h"

int main() {

	char menuChoiceString[2];
	int menuChoice = 0;
	int configStatus = 0;
	int serverStart = 0;

	// Check/create config and log files
	config_t Config = configCheck(&configStatus);

	// Enter credentials to access server control
	controlLogin(&Config, configStatus);

	// Start up the logger
	int log = startLogger(Config.logFile);
	logServerStart(configStatus, log);

	// Create a file list structure for storing filenames
	fileList_t FileList = {NULL, 0};

	printWelcome(Config.motd);

	while (menuChoice != 4) {

		showMainMenuOptions();
		getMenuInput(menuChoiceString, 2);
		menuChoice = atoi(menuChoiceString);

		switch(menuChoice) {

			case(1): // Start file server

				if (serverStart != 1) {
					printf("Start server function here\n");
					serverStart = 1;
				} else {
					printf("\nServer is currently running\n");
				}
				break;

			case(2): // Set server credentials
				setCredentials(&Config);
				break;

			case(3):
				// List files in shared directory
				listFiles(&FileList, Config.shareFolder);
				logPipe("Shared directory listed", log);
				break;

			case(4):
				// Quit
				break;

			default:
				printf("\nError: Invalid menu choice.\n");
		}
	}

	// Free up allocated memory before we exit
	fileCleanup(&FileList);

	logPipe("File server shut down", log);
	wait(NULL);

	return 0;
}
