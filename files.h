/* File server application | Jack Nelson | CSP2308
 * files.h
 * file.c header file */

void fileCleanup(fileList_t* FileList);
// Frees up all memory utilized by the current file list data

void listFiles(threadData_t* serverInfo);
// Retrieves names of files from the shared folder and stores them in a FileList struct

int checkAccess(char* fileName);
// Checks the read, write and execute persmissions of a file and returns a number that represents the current access
