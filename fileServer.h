/* File server application | Jack Nelson | CSP2308
 * fileServer.h
 * Main application header file */

// Max length of the message of the day can be specified below
#define MAXMOTDLEN 128

// Config structure definition
// Stores application settings
typedef struct {
	short portNumber;
	char ipAddress[16];
	char shareFolder[256];
	char motd[MAXMOTDLEN];
	char logFile[256];
	char serverCreds[128];
	int logFd;
} config_t;

// Filelist structure definition
// Stores shared file directory details
typedef struct {
	char** sharedFiles;
	int fileCount;
} fileList_t;

typedef struct {
	int clientSocket;
	config_t* Config;
	char clientAddress[30];
} threadData_t;
