/* File server application | Jack Nelson | CSP2308
 * io.h
 * io.c header file */

#define FILENUMBERSTRINGLEN 11
// Defining how many digits the file number can be. Ten should to be enough right?
// If not, we can update this value later! =)

void controlLogin(config_t* Config, int configStatus);

int clientLogin(threadData_t* serverInfo);

void printWelcome(char* motd);
// Prints the main menu along with the MOTD

void showMainMenuOptions(int portNumber);
// Prints the options for the main menu

void getKeyboardInput(char* inputString, int inputLength);
// Gets and stores user keyboard input in a given string

void getSocketInput(char* inputString, int inputLength, int sockFd);

//int listFiles(fileList_t* FileList, char* shareFolder);
// Outputs a list of current files in the shared directory
//
void sendFileMenu(threadData_t* ServerInfo);

void printFileContent (int* fileNum, char* fileName, config_t* Config, int log);
// Read a hosted file and display it's contents on screen

void readFileMenu(fileList_t* FileList, config_t* Config, int log);
// Displays a menu for the user to choose which file to output

int sendFile(threadData_t* ServerInfo); 
