/* File server application | Jack Nelson | CSP2308
 * logger.h
 * logger.c header file */

void logfileCheck(char* logFilename);
void getTime(char* timestr);
void logger(char* logFilename, int log);
int startLogger(char* logFilename);
void logPipe(char* message, int log);
void logProgramStart(int configStatus, int log);
