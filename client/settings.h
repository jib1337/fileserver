/* File server application | Jack Nelson | CSP2308
 * settings.h
 * settings.c header file */

#define DEF_USERNAME "user"
#define DEF_PORTNUM 6666
#define DEF_IPADDRESS "127.0.0.1"
#define DEF_SHAREFOLDER "./fileshare"

void configRead(char* input, int msgLen, FILE* file);
// Reads a line from the config file. If a line cannot be read, the program will exit

config_t configCheck();
//Check the existance of the config file and it's contents

void configWrite(config_t* Config);
