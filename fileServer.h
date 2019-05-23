/* File server application | Jack Nelson | CSP2308
 * fileServer.h
 * Main application header file */

// Max length of the message of the day string
#define MAXMOTDLEN 128

// Length of the <access granted> string to be sent when the user authenticates, which contains MOTD
#define ACCMOTDLEN MAXMOTDLEN + 2

/* Config structure definition
 * Stores application settings */
typedef struct {
	short portNumber;
	char ipAddress[16];
	char shareFolder[256];
	char motd[MAXMOTDLEN];
	char logFile[256];
	char serverCreds[128];
	int logFd;
} config_t;

/* File list structure definition
 * Stores shared file directory details */
typedef struct {
	char** sharedFiles;
	int fileCount;
} fileList_t;

/* Client linked list structure definitions
 * Defined futher below - used to storing thread data structs */
typedef struct node node_t;
typedef struct list list_t;

struct list {
	node_t* head;
	//node_t* foot;
	int count;
};

/* Thread data structure definition
 * Stores data to be passed into client handler threads */
typedef struct {
	list_t* clientList;
	pthread_t threadId;
	int clientSocket;
	config_t* Config;
	int recFd;
	int sendFd;
} threadData_t;

struct node {
	threadData_t* data;
	node_t* next;
};
