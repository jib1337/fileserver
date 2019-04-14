/* File server application | Jack Nelson | CSP2308
 * networking.c
 * Provision and handling of network communications */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#include "fileServer.h"
#include "logger.h"

void serverStart(config_t* Config) {

	pid_t pid;

	if ((pid = fork()) < 0) {

		perror("Could not create server process");

	} else if (pid == 0) {

		// Child / Server process
		printf("Child process reporting for duty\n");
		exit(0);
	
	} else {

		// Parent / Control process
	}
}
