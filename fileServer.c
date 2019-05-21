/* File server application | Jack Nelson | CSP2308
 * fileServer.c
 * Main program logic */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include "fileServer.h"
#include "networking.h"
#include "files.h"
#include "logger.h"
#include "io.h"
#include "settings.h"
#include "security.h"

int main() {

	int configStatus = 0;

	// Check/create config and log files
	config_t Config = configCheck(&configStatus);

	// Start up the logger
	Config.logFd = startLogger(Config.logFile);
	logProgramStart(configStatus, Config.logFd);

	// Set up signals for SIGHUP, SIGTERM and SIGQUIT
	setConfigHandler(&Config);

	signal(SIGTERM, signalShutdown);
	signal(SIGQUIT, signalShutdown);

	// First run detection and cred setup
	controlLogin(&Config, configStatus);

	printWelcome(Config.motd);

	char menuChoiceString[2];
	int menuChoice = 0;

	while (menuChoice != 4) {

		showMainMenuOptions();
		getKeyboardInput(menuChoiceString, 2);
		menuChoice = atoi(menuChoiceString);

		switch(menuChoice) {

			case(1): // Start file server
				serverStart(&Config);
				break;

			case(2): // Set server credentials
				setCredentials(&Config);
				break;

			case(3):
				displaySettings(&Config);
				break;

			case(4):
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
