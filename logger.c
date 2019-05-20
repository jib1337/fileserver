/* File server application | Jack Nelson | CSP2308
 * logger.c
 * Log file management */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

// Do I need this?
#include <signal.h>

#include "logger.h"

void logfileCheck(char* logFilename) {
	// Check the existance of the logfile and create it if nessecary

	FILE* logFile = fopen(logFilename, "r");

	if (logFile == NULL) {
		// Logfile doesn't exist, so create it

		logFile = fopen(logFilename, "w");

		fprintf(logFile, "------------------------------------------------------------------------------------\n"
				 "                                 File Server Log\n"
				 "------------------------------------------------------------------------------------\n");
		fclose(logFile);

	} else {

		fclose(logFile);
	}
}

void getTime(char* timestr) {
	// Create and return a string containing the current date and time

	time_t rawtime = time(NULL);
	strcpy(timestr, ctime(&rawtime));
	timestr[strlen(timestr)-1] = '\0';
}


void logger(char* logFilename, int log) {
	// Logger process logs messages from here

	FILE* logFile;
	char message[255];
	char timestr[25];

	while ((read(log, message, 255))) {
		/* Running logfileCheck before each message ensures that even if the
		* logfile is deleted during runtime, the file will be recreated and messages
		* will continue to be logged. */
		logfileCheck(logFilename);

		getTime(timestr);
		logFile = fopen(logFilename, "a");
		fprintf(logFile, "%s\t%s\n", timestr, message);
		fclose(logFile);

		if (strcmp(message, "Program shut down") == 0) break;
	}

	exit(0);
}

int startLogger(char* logFilename) {
	// Fork off a logging process and have it listen for incoming log messages through a pipe.

	int logPipe[2];
	pid_t pid;

	if (pipe(logPipe) < 0) {

		perror("Could not create logfile pipe");
	}

	if ((pid = fork()) < 0) {

		perror("Could not create logfile process");

	} else if (pid > 0) {

		// Parent process
		close(logPipe[0]);

	} else {

		// child process
		close(logPipe[1]);

		signal(SIGHUP, SIG_IGN);

		logger(logFilename, logPipe[0]);
	}

	return logPipe[1];
}

void logPipe(char* message, int log) {
	// Pipe the message through to the logging process.

	write(log, message, strlen(message)+1);
}

void logProgramStart(int configStatus, int log) {
	// Creates the opening log entry for the server. Depends on what occured with the config file.

	if (configStatus == 1) {

		logPipe("Program started - config file created", log);
	} else {

		logPipe("Program started - config file loaded", log);
	}
}
