 #include "queue.h"

 // function to add client
void addClient(queue* Queue,client Client){
    // if queue is not full
    if(Queue->currentSize < Queue->maxSize){
        pthread_mutex_lock(&(Queue->mutex_d)); // get access to desk
        //sem_post(&(((queue*)Queue)->sema)); // up the semaphore
        Queue->array[Queue->currentSize +Queue->frontIndex % Queue->maxSize] = Client; // put the new client in the circular queue
        pthread_mutex_lock(&mutex_q); // get access to all queues
        Queue->currentSize++; // increase the size
        pthread_mutex_unlock(&mutex_q); // release access to all queues
        if (pthread_cond_signal(&(Queue->cond)) != 0)
            printf("Error While Condition Signal Generation!");
        pthread_mutex_unlock(&(Queue->mutex_d)); // unlock the specific deskthread mutex
    }
    else {
        pthread_mutex_lock(&mutex_r); // lock the variable
        removedClients++; // increase the removed client number
         if(removedClients == client_number){
            pthread_exit(NULL); // check if all the clients are processed. if so exit thread
        }
        pthread_mutex_unlock(&mutex_r); // unlock the variable
    }
    // if queue is full do nothing
}
// function to remove client
void* removeClient(void* Queue){
    while(1){
        // check if the operation should finish(all the clients are removed)
        pthread_mutex_lock(&(((queue*)Queue)->mutex_d)); // get access to desk
        // wait if the queue size is zero
        while(((queue*)Queue)->currentSize == 0) pthread_cond_wait(&(((queue*)Queue)->cond),&(((queue*)Queue)->mutex_d));
        // if there are clients in the queue

        pthread_mutex_unlock(&(((queue*)Queue)->mutex_d)); // unlock the specific deskthread mutex
        client Client = ((queue*)Queue)->array[((queue*)Queue)->frontIndex]; // removed client
        usleep(Client.duration * 1000000); // sleep for client duration
        pthread_mutex_lock(&(((queue*)Queue)->mutex_d)); // get access to desk

        pthread_mutex_lock(&mutex_q); // get access to all queues
        ((queue*)Queue)->currentSize--; // decrease the size
        pthread_mutex_unlock(&mutex_q); // release access to all queues

        ((queue*)Queue)->frontIndex = (((queue*)Queue)->frontIndex + 1 ) % ((queue*)Queue)->maxSize; // increase the front index circularly
   
        pthread_mutex_unlock(&(((queue*)Queue)->mutex_d)); // unlock the specific deskthread mutex
        // actions on info variable and supervisor thread
        pthread_mutex_lock(&mutex_i); // lock the critical region for info variable
        globalInfo.client = Client; // set the client id to info
        globalInfo.paydeskNo = ((queue*)Queue)->deskNumber ; // set the paydesk no
        pthread_mutex_unlock(&mutex_s); // wake up the supervisor thread
        
    }
}

// printer function for info variable
void* printInfo(){
    while(1){
        pthread_mutex_lock(&mutex_s); // wait for the information to sent
        printf("Desk %d served Client %d in %f seconds.\n",globalInfo.paydeskNo,globalInfo.client.id,globalInfo.client.duration);
        pthread_mutex_unlock(&mutex_i); // unlock the critical region for info variable
        pthread_mutex_lock(&mutex_r); // lock the variable
        removedClients++; // increase the removed client number
        if(removedClients == client_number){
            pthread_exit(NULL); // check if all the clients are processed. if so exit thread
        }
        pthread_mutex_unlock(&mutex_r); // unlock the variable
    }
}