/* File server application | Jack Nelson | CSP2308
 * fileServer.h
 * Main application header file */

// Config structure definition
// Stores application settings
typedef struct {
	char username[11];
	short serverPort;
	char serverIP[16];
	char shareFolder[256];
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
} threadData_t;

