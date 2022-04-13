#ifndef QUEUE_HEADER_FILE
#define QUEUE_HEADER_FILE
#include "client.h"
#include <pthread.h>
//#include <semaphore.h>
#include <unistd.h>

typedef struct Information {
    int paydeskNo;
    client client;
}info;

typedef struct ClientQueue {
    client * array;
    int maxSize;
    int currentSize;
    int frontIndex;
    int deskNumber; // paydesk number
    pthread_mutex_t mutex_d; // mutex specific to a queue
    pthread_cond_t cond; // condition for sleep the desk thread
}queue;

void addClient(queue* Queue,client Client); // function to add client
void* removeClient(void* Queue); // function to remove client 

pthread_mutex_t mutex_q; // mutex for queue critical regions
pthread_mutex_t mutex_s; // mutex for sleep-wake up supervisor thread
pthread_mutex_t mutex_i; // mutex for information variable critical region
info globalInfo; // a global information variable

void* printInfo(); // printer function for supervisor thread

int removedClients; // a global variable to remember the removed client number
pthread_mutex_t mutex_r; // mutex for removedClients variable
int client_number; // a global variable to remember total client number


#endif
