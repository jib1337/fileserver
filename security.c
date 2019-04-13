#include <stdio.h>
#include <openssl/sha.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "fileServer.h"
#include "settings.h"
#include "io.h"
#include "security.h"
#include "logger.h"

int authenticate(char* credString) {
	char username[11];
	char password[31];
	char serverUsername[11];
	char serverPasswordHash[65];

	splitCredentials(credString, serverUsername, serverPasswordHash);
	
	printf("Username: ");
	getMenuInput(username, 11);
	printf("Password: ");
	getMenuInput(password, 31);

	if ((strcmp(username, serverUsername) == 0) && (checkPassword(password, serverPasswordHash) == 1)) {
		//Access granted, return true
	} else {
		return 0; //Return failure
	}

	return 1;
}

int checkPassword(char* password, char* currentPasswordHash) {
	char hash[65];

	genHash(password, strlen(password), hash);
	
	if (strcmp(hash, currentPasswordHash) == 0) {
		return 1;
	} else {
		return 0;
	}
}

void genHash(char* textString, unsigned long length, char* hashString) {

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
	
	char username[11];
	char password[31];
	char passwordHash[65];
	char credString[128];

	printf("New username: ");
	getMenuInput(username, 11);

	printf("New password: ");
	getMenuInput(password, 31);
		
	strcpy(credString, username);
	strcat(credString, ":");
	genHash(password, strlen(password), passwordHash);
	strcat(credString, passwordHash);
	strcpy(Config->serverCreds, credString);
	configWrite(Config);

	//printf("Credentials updated.\nNew username: %s\nNew password: %s\n", username, password);
}

