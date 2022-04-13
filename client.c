#include "client.h"

// function to set the client ID
void setID(client* Client,int ID){ 
    Client->id = ID;
}
// function to get the client ID
int getID(client Client){
    return Client.id;
} 
// function to set the client duration randomly
void setDuration(client* Client,double lambda){
    double x;
    x = rand() / (RAND_MAX + 1.0); // get a random number between 0 and 1
    Client->duration = -((log(1- x) )/ lambda); // set the duration
}
// function to get the client duration
double getDuration(client Client){
    return Client.duration;
}
