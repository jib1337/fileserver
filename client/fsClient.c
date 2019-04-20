/* File server application | Jack Nelson | CSP2308
 * fileServer.c
 * Main program logic */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "fsClient.h"
#include "files.h"
#include "io.h"
#include "settings.h"
#include "networking.h"

int main() {

	char menuChoiceString[2];
	int menuChoice = 0;

	// Check/create config and log files
	config_t Config = configCheck();

	printWelcome();

	while (menuChoice != 3) {

		showMainMenuOptions();
		getKeyboardInput(menuChoiceString, 2);
		menuChoice = atoi(menuChoiceString);

		switch(menuChoice) {

			case(1): // Start file server
				serverConnect(&Config);
				break;

			case(2): // Set server credentials
				printf("Edit stuff\n");
				break;

			case(3):
				// Quit
				break;

			default:
				printf("\nError: Invalid menu choice.\n");
		}
	}

	return 0;
}
