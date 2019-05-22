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
#include "logger.h"
#include "io.h"
#include "settings.h"
#include "security.h"

int main() {

	// Get the config
	int configStatus = 0;
	config_t Config = configCheck(&configStatus);

	// Start up the logger and log what happened with the config
	Config.logFd = startLogger(Config.logFile);
	logProgramStart(configStatus, Config.logFd);

	// Set up signals
	setConfigHandler(&Config);
	signal(SIGTERM, signalShutdown);
	signal(SIGQUIT, signalShutdown);

	// First run detection and cred setup
	firstRunRegister(&Config, configStatus);

	printWelcome(Config.motd);

	char menuChoiceString[2];
	int menuChoice = 0;

	while (menuChoice != 4) {

		showMainMenuOptions();
		getKeyboardInput(menuChoiceString, 2);
		menuChoice = atoi(menuChoiceString);

		switch(menuChoice) {

			case(1):
				serverStart(&Config);
				break;

			case(2):
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
