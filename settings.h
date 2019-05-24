/* File server application | Jack Nelson | CSP2308
 * settings.h
 * settings.c header file */

#define DEF_PORTNUM 6666
#define DEF_IPADDRESS "127.0.0.1"
#define DEF_SHAREFOLDER "./fileshare"
#define DEF_MOTD "Hi Rob! Thanks for a great semester!"
#define DEF_LOGFILE "./activity.log"
#define DEF_CREDS "user:5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8"

void signalShutdown();
void setConfigHandler(config_t* Config);
void configReload();
void configRead(char* input, int msgLen, FILE* file);
config_t configCheck(int* configStatus);
void configWrite(config_t* Config);
