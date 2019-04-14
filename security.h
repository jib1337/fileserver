/* File server application | Jack Nelson | CSP2308
 * security.h
 * security.c header file */


int authenticate(char* credentials, char* username, char* password);
int checkPassword(char* password, char* currentPasswordHash);
void genHash(char* password, unsigned long length, char* hashString);
void setCredentials(config_t* Config);

