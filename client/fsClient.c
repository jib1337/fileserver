/* File server application | Jack Nelson | CSP2308
 * fileServer.c
 * Main program logic */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "fsClient.h"
#include "io.h"
#include "files.h"
#include "settings.h"
#include "networking.h"

int main() {

	fileList_t FileList = {NULL, 0};

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
				serverConnect(&Config, &FileList);
				break;

			case(2): // Set server credentials
				editSettings(&Config);	
				break;

			case(3):
				// Quit
				break;

			default:
				printf("\nError: Invalid menu choice.\n");
		}
	}

	// Free the filelist before exit
	fileCleanup(&FileList);

	return 0;
}
