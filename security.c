/* File server application | Jack Nelson | CSP2308
* security.c
* Functions related to security and authentication */

#include <stdio.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "fileServer.h"
#include "settings.h"
#include "io.h"
#include "security.h"

void firstRunRegister(config_t* Config, int configStatus) {
	/* First run credential registration screen
	 * Allows for registering of a server username and password.
	 *
	 * Note that if something interrupts the program between the config file being created
	 * and new credentials being set, the server will use defaults of admin:password */

	if (configStatus == 1) {

		printf("----------------------------------------\n"
		       "          First Run Registration        \n"
	               "----------------------------------------\n");

		printf("No saved credentials: Please register some now!\n");
		setCredentials(Config);
	}
}

int clientLogin(threadData_t* serverInfo) {
	// Interfaces with client socket to recieve user credentials for authentication

	char username[11];
	char password[31];
	int result = 0;

	bzero(username, 11);
	bzero(password, 31);

	read(serverInfo->clientSocket, username, 11);

	write(serverInfo->clientSocket, "Password: ", 11);
	read(serverInfo->clientSocket, password, 31);

	result = authenticate(serverInfo->Config->serverCreds, username, password);

	// Zero out these spaces in memory
	bzero(username, 11);
	bzero(password, 31);

	return result;
}

int authenticate(char* credString, char* username, char* password) {
	// Check a provided set of credentials against the server credentials
	// and return integer if they match

	char serverUsername[11];
	char serverPasswordHash[65];

	// Unpack the credential string and store in seperate variables
	splitCredentials(credString, serverUsername, serverPasswordHash);

	if ((strcmp(username, serverUsername) == 0) && (checkPassword(password, serverPasswordHash) == 1)) {
		// Access granted, return true
	} else {
		return 0; // Return failure
	}

	return 1;
}

int checkPassword(char* password, char* currentPasswordHash) {
	/* Compares a known current password hash with a hash generated from
	 * given password input string, then returns integer indicating if it's a match or not */

	char hash[65];

	genHash(password, strlen(password), hash);
	
	if (strcmp(hash, currentPasswordHash) == 0) {
		return 1;
	} else {
		return 0;
	}
}

void genHash(char* textString, unsigned long length, char* hashString) {
	// Generates a SHA256 hash of a given string using the openssl library

	unsigned char hash[65];
	SHA256_CTX ctx;
	int i;

	SHA256_Init(&ctx);
	SHA256_Update(&ctx, (unsigned char*)textString, length);
	SHA256_Final(hash, &ctx);

	for (i=0; i<SHA256_DIGEST_LENGTH; i++) {
		sprintf(&hashString[i*2], "%02x", hash[i]);
	}
}

void setCredentials(config_t* Config) {
	/* Provides interface for user to change the server credentials, and then updates
	 * them in the config file */
	
	char username[11];
	char password[31];
	char passwordHash[65];
	char credString[128];

	do {
		printf("\nNew username (max 10 characters): ");
		getKeyboardInput(username, 11);
	} while (validateName(username) == 0);

	printf("New password (max 30 characters): ");
	getKeyboardInput(password, 31);
		
	strcpy(credString, username);
	strcat(credString, ":");
	genHash(password, strlen(password), passwordHash);
	strcat(credString, passwordHash);
	strcpy(Config->serverCreds, credString);
	configWrite(Config);

	printf("Credentials updated.\n\n");

	// zero out the password
	bzero(password, 31);
	bzero(passwordHash, 65);
}

int validateName(char* username) {
	// Validates a username to ensure only alphabetic characters are entered

	int i;

	if (strlen(username) == 0) {
		fprintf(stderr, "\nError - Username must be at least 1 character long.\n");
		return 0;
	}

	for (i=0; i<strlen(username); i++) {
		if (isalpha(username[i]) == 0) {
			fprintf(stderr, "\nError - Username can only consist of alphabet characters.\nPlease try again.\n");
			return 0;
		}
	}

	return 1;
}

int splitCredentials(char* credString, char* username, char* passwordHash) {
	// Split a string of user:password credentials stored in the settings file into two seperate strings

	int u, i, j=0;

	for (u=0; u<strlen(credString); u++) {
		if (credString[u] == ':') {

			if (u == 0) return 1;
			break;
		}
	}
	
	for (i=0; i<strlen(credString); i++) {
		if (i <= u) {

			if (i == u) {
				username[i] = '\0';
			} else {
				username[i] = credString[i];
			}

		} else {
			passwordHash[j] = credString[i];
			j++;

			if (i+1 == strlen(credString)) {
				passwordHash[j] = '\0';
			}
		}
	}	

	return 0;
}
