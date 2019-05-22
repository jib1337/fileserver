/* File server application | Jack Nelson | CSP2308
 * security.h
 * security.c header file */

void firstRunRegister(config_t* Config, int configStatus);
int clientLogin(threadData_t* serverInfo);
int authenticate(char* credentials, char* username, char* password);
int checkPassword(char* password, char* currentPasswordHash);
void genHash(char* password, unsigned long length, char* hashString);
void setCredentials(config_t* Config);
int validateName(char* username); 

