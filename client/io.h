/* File server application | Jack Nelson | CSP2308
 * io.h
 * io.c header file */

#define FILENUMBERSTRINGLEN 11
// Defining how many digits the file number can be. Ten should to be enough right?
// If not, we can update this value later! =)

void printWelcome();
// Prints the main menu along with the MOTD

void showMainMenuOptions();
// Prints the options for the main menu
//
void showServerOptions();

void getKeyboardInput(char* inputString, int inputLength);
// Gets and stores user keyboard input in a given string

void getSocketInput(char* inputString, int inputLength, int sockFd);

int getFileList(fileList_t* FileList, int connectionSocket);
// Outputs a list of current files in the shared directory

void printFileContent (int* fileNum, char* fileName, config_t* Config, int log);
// Read a hosted file and display it's contents on screen

void downloadFileMenu(fileList_t* FileList, char* shareFolder, int connectionSocket);
// Displays a menu for the user to choose which file to download

int downloadFile(char* fileName, char* filePath, int connectionSocket);
// Request and recieve a file from the server

