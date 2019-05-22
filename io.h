/* File server application | Jack Nelson | CSP2308
 * io.h
 * io.c header file */

void printWelcome(char* motd);
void showMainMenuOptions();
void displaySettings(config_t* Config);
void getKeyboardInput(char* inputString, int inputLength);
void sendFileMenu(threadData_t* ServerInfo);
void recieveFileMenu(threadData_t* ServerInfo); 
void printFileContent (int* fileNum, char* fileName, config_t* Config, int log);
void readFileMenu(fileList_t* FileList, config_t* Config, int log);
int sendFile(threadData_t* ServerInfo);
int recieveFile(threadData_t* ServerInfo); 
void listFiles(threadData_t* serverInfo);
int checkAccess(char* fileName);
