/* File server application | Jack Nelson | CSP2308
 * clientlist.c
 * Setup and handling of client linked list data structure */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#include "fileServer.h"
#include "clientlist.h"

static pthread_mutex_t m_listRemoval = PTHREAD_MUTEX_INITIALIZER;

list_t* newClientList() {
	// Create a new client list starting node

	list_t* list = malloc(sizeof(list_t));
	list->head = NULL;
	list->count = 0;
	return list;
}

void insertClient(list_t* list, threadData_t* data) {
	// Insert a new client at the end of the client list
	
	node_t* new = malloc(sizeof(node_t));
	node_t* current;
	
	new->data = data;
	new->next = NULL;

	if (list->head == NULL) {
		list->head = new;
		
	} else {
		
		current = list->head;
		while (current->next != NULL) {
			current = current->next;
		}
			current->next = new;
	}

	list->count++;
}

void removeClient(list_t* list, threadData_t* data) {
	// Remove client node from client list when closing a connection

	node_t* current = list->head;
	node_t* temp;

	// Mutex to prevent multiple threads from modifying the list at once
	pthread_mutex_lock(&m_listRemoval);

	if (current != NULL) {

		if (current->data->clientSocket == data->clientSocket) {
		// The client to remove is at the head of the list
			
			temp = list->head;	
			list->head = current->next;
			free(temp);
			list->count--;

		} else {
		// The client to remove is later in the list, so search through

			while (current->next != NULL) {

				if (current->next->data->clientSocket == data->clientSocket) {
				// Next item after the current is the client we want to remove
				
					temp = current->next;
					current->next = temp->next;
					free(temp);
					list->count--;		
					break;

				} else {
					current = current->next;
				}
			}
		}
	}

	pthread_mutex_unlock(&m_listRemoval);
}

void cleanupClients(list_t* list) {
	// Clean up a list of clients prior to an exit signal
	
	node_t* temp;

	while (list->head != NULL) {
		
		// Signal thread to exit	
		pthread_kill(list->head->data->threadId, SIGUSR1);
		
		// Here we check the file descriptors
		// Should be negative if not in use, so we'll close them if they aren't that.
		if (list->head->data->recFd != -1) {
			close(list->head->data->recFd);
		}

		if (list->head->data->sendFd != -1) {
			close(list->head->data->sendFd);
		}

		// Clean up the list
		free(list->head->data);
		temp = list->head;
		list->head = list->head->next;
		free(temp);
	}

	free(list);
}

