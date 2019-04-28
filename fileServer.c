/* File server application | Jack Nelson | CSP2308
 * fileServer.c
 * Main program logic */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "fileServer.h"
#include "networking.h"
#include "files.h"
#include "logger.h"
#include "io.h"
#include "settings.h"
#include "security.h"

int main() {

	char menuChoiceString[2];
	int menuChoice = 0;
	int configStatus = 0;
	int serverStarted = 0;

	// Check/create config and log files
	config_t Config = configCheck(&configStatus);

	// Enter credentials to access server control
	// controlLogin(&Config, configStatus);

	// Start up the logger
	Config.logFd = startLogger(Config.logFile);
	logProgramStart(configStatus, Config.logFd);

	printWelcome(Config.motd);

	while (menuChoice != 3) {

		showMainMenuOptions(serverStarted);
		getKeyboardInput(menuChoiceString, 2);
		menuChoice = atoi(menuChoiceString);

		switch(menuChoice) {

			case(1): // Start file server

				if (serverStarted == 0) {
					serverStart(&Config, &serverStarted);
				} else {
					printf("\nServer is currently running\n");
				}
				break;

			case(2): // Set server credentials
				setCredentials(&Config);
				break;

			case(3):
				// Quit
				break;

			default:
				printf("\nError: Invalid menu choice.\n");
		}
	}

	logPipe("Program shut down", Config.logFd);
	wait(NULL);

	return 0;
}
