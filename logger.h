/* File server application | Jack Nelson | CSP2308
 * logger.h
 * logger.c header file */

void logfileCheck(char* logFilename);
// Check the existance of the logfile and create it if nessecary

void getTime(char* timestr);
// Modify a string to contain the current date and time

void logger(char* logFilename, int log);
// Logger process logs messages from here

int startLogger(char* logFilename);
// Fork off a logging process

void logPipe(char* message, int log);
// Pipe the message through to the logging process

void logServerStart(int configStatus, int log);
// Creates the opening log entry for the server. Depends on what occured with the config file
