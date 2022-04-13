#ifndef CLIENT_HEADER_FILE
#define CLIENT_HEADER_FILE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct BankClient {
    int id; // Client ID
    double duration; // banking transaction duration
} client;

void setID(client* Client,int ID); // function to set the client ID
int getID(client Client); // function to get the client ID
void setDuration(client* Client,double lambda); // function to set the client duration
double getDuration(client Client); // function to get the client duration

#endif
