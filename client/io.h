/* File server application | Jack Nelson | CSP2308
 * io.h
 * io.c header file */

#define FILENUMBERSTRINGLEN 11
// Defining how many digits the file number can be. Ten should to be enough right?
// If not, we can update this value later! =)

void showMainMenuOptions();
void showServerOptions();
void serverMenu(config_t* Config, fileList_t* FileList, int connectionSocket);
void editSettings(config_t* Config);
void getKeyboardInput(char* inputString, int inputLength);
int getPassword(char* passwordString, int inputLength);
void getSocketInput(char* inputString, int inputLength, int sockFd);
int listFiles(fileList_t* FileList, char* shareFolder); 
int getFileList(fileList_t* FileList, int connectionSocket);
void printFileContent (int* fileNum, char* fileName, config_t* Config, int log);
void uploadFileMenu(fileList_t* FileList, char* shareFolder, int connectionSocket); 
int uploadFile(char* fileName, char* filePath, int connectionSocket);
void downloadFileMenu(fileList_t* FileList, char* shareFolder, int connectionSocket);
int downloadFile(char* fileName, char* filePath, int connectionSocket);
