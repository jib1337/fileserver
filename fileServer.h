/* File server application | Jack Nelson | CSP2308
 * fileServer.h
 * Main application header file */

// Max length of the message of the day can be specified below
#define MAXMOTDLEN 128
#define ACCMOTDLEN MAXMOTDLEN + 2

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

// One node for each item, and define's the item's position in the list
typedef struct node node_t;
typedef struct list list_t;

// The list points to the first item and contains the count of total items
struct list {
	node_t* head;
	node_t* foot;
	int count;
};

typedef struct {
	list_t* clientList;
	pthread_t threadId;
	int clientSocket;
	config_t* Config;
	FILE* recFp;
	int sendFd;
} threadData_t;

struct node {
	threadData_t* data;
	node_t* next;
};
