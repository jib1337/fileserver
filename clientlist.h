/* File server application | Jack Nelson | CSP2308
 * clientlist.h
 * Client linked list header file */


list_t* newClientList();
void insertClient(list_t* list, threadData_t* data);
void removeClient(list_t* list, threadData_t* data);
void cleanupClients(list_t* list);
