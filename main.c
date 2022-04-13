#include <ctype.h>
#include "queue.h"

// parser function decleration
int parseArguments(int argc, char **argv,int* client_number,int* desk_threads,int* max_q_size,double* gen_time,double* dur_time);
// sleeper function decleration
void mainSleep(double lambda);

int main (int argc, char **argv)
{
  int desk_threads =  4; // number of desk threads is 4 default
  int max_q_size =  3; // maximum queue size is 3 default
  double gen_time =  100; // the rate of generation time is 100 default
  double dur_time = 10; // the rate of duration time is 10 default
  int thread_check = 0; // checker for desk thread creation
  int minQueueIndex = 0; // index for queue client comparison
  int sup_thread_check = 0; // checker for supervisor thread creation
  client_number = 20; // client number is 20 default
  removedClients = 0; // set the removed client number to 0 initially

   // obtain and validate the optional arguments
  int valid = parseArguments(argc,argv,&client_number,&desk_threads,&max_q_size,&gen_time,&dur_time);
  if( valid == -1) return -1; 
    

  pthread_t deskThreads[desk_threads]; // create a desk thread array
  pthread_t sup; // supervisor thread
  client Clients[client_number]; // create the client array
  queue Queues[desk_threads]; // create the queue array
  // set the parameters of queues
  for(int i=0;i<desk_threads;i++){
     Queues[i].array = (client*) malloc((sizeof(client))*max_q_size); // allocate new space for client array
     Queues[i].maxSize = max_q_size; // max size
     Queues[i].currentSize = 0; // current size
     Queues[i].frontIndex = 0; // front index
     Queues[i].deskNumber = i; // desk number
     pthread_mutex_init(&(Queues[i].mutex_d),0); // initialize the mutex_d
     pthread_cond_init(&(Queues[i].cond),0); // initialize the condition variable
  }

  // initialize he mutexes
  pthread_mutex_init(&mutex_q,0); // mutex for queue critical regions
  pthread_mutex_init(&mutex_s,0); // mutex for sleep-wake up supervisor thread
  pthread_mutex_init(&mutex_i,0); // mutex for information variable critical region
  pthread_mutex_lock(&mutex_s); // lock the supervisor thread

  sup_thread_check = pthread_create(&sup,NULL,printInfo,NULL); // create the supervisor thread
  if (sup_thread_check < 0) {
    printf("Error while creating the supervisor thread."); // check the creation of the thread
    exit(-1);
  }

  // create the desk threads
  for(int i=0;i<desk_threads;i++){
    thread_check = pthread_create(&deskThreads[i],NULL,removeClient,(void*)&Queues[i]);
     if (thread_check < 0) {
         printf("Error while creating the desk thread for desk %d",i); // check the creation of the desk threads
         exit(-1);
    }
  }

  // create clients
  for(int i=0; i<client_number;i++){
    // set the client arguments
    setID(&Clients[i],i); // set ids 1 by 1
    setDuration(&Clients[i],dur_time); // set durations 1 by 1
    printf("Client %d arrived.\n",Clients[i].id);
    pthread_mutex_lock(&mutex_q); // get access to all queues
    // find the queue with minimum clients
    minQueueIndex = 0;
    int j;
    for(j=0; j<desk_threads; j++){
      if(Queues[j].currentSize < Queues[minQueueIndex].currentSize) 
        minQueueIndex = j;
    }
    pthread_mutex_unlock(&mutex_q); // release access to all queues
    addClient(&(Queues[minQueueIndex]),Clients[i]); // add client to the queue with min client
    mainSleep(gen_time); // sleep between generations
  }

  
  // wait for the supervisor thread
  if(pthread_join(sup,0) < 0){
    perror ("Supervisor Thread Join Error!");
    exit(-1);
  }
  
  // destroy the mutexes
  for(int i=0;i<desk_threads;i++){
    pthread_mutex_destroy(&(Queues[i].mutex_d));
  }
  pthread_mutex_destroy(&mutex_q);
  pthread_mutex_destroy(&mutex_s);
  pthread_mutex_destroy(&mutex_i);
  pthread_mutex_destroy(&mutex_r);
}

// parser function decleration
int parseArguments(int argc, char **argv,int* client_number,int* desk_threads,int* max_q_size,double* gen_time,double* dur_time){
  int c;
  while ((c = getopt (argc, argv, "cnqgd")) != -1)
    switch (c)
      {
      case 'c':
        if(argv[optind] != NULL){
          *client_number = atof(argv[optind]);
          break;
        }
        else{
          break; // run with default settings
        }
      case 'n':
        if(argv[optind] != NULL){
          *desk_threads = atof(argv[optind]);
          break;
        }
        else{
          break; // run with default settings
        }
      case 'q':
        if(argv[optind] != NULL){
          *max_q_size = atof(argv[optind]);
          break;
        }
        else{
          break; // run with default settings
        }
      case 'g':
        if(argv[optind] != NULL){
          *gen_time = atof(argv[optind]);
          break;
        }
        else{
          break; // run with default settings
        }
      case 'd':
        if(argv[optind] != NULL){
          *dur_time = atof(argv[optind]);
          break;
        }
        else{
          break; // run with default settings
        }
      case '?':
        printf("Unknown Option! The program is stopped!\n");
        return -1;
      }
  printf("Bank Program runs with the following arguments: \n");
  printf(" Total Number of Clients = %d\n Number of Pay Desks = %d\n Max Queue Size = %d\n Generation Time = %f\n Duration Time = %f\n",*client_number, *desk_threads,*max_q_size,*gen_time,*dur_time);
  return 0;
}

// randomly sleep function for main thread
void mainSleep(double lambda){
    double x;
    x = rand() / (RAND_MAX + 1.0); // get a random number between 0 and 1
    usleep((-((log(1- x) )/ lambda)) * 1000000); // sleep accordingly
}