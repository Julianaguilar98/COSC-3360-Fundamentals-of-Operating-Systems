/* 
COSC 3360 (TuTh 4pm) - Homework 3
Compiles with no warnings in g++ using -fpermissive and -lpthread */

#include <iostream>
#include <pthread.h>
#include <string>
#include <unistd.h>
using namespace std;

//defining global variables
static int maxCars;
static int maxNorthCars;
static int maxSouthCars;
static int numWaiting=0;
static int numCarsInTunnel=0;
static int totalNorthCars=0;
static int totalSouthCars=0;
static int numNorthCars=0;
static int numSouthCars=0;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 

struct car {
    int carNum; //used for total
    int carTravelTime; //Time the car has travelled
    bool waitflag; //Bool to check to see if car has waited or not
};

void* NorthboundCar(void* arg);
void* SouthboundCar(void* arg);

int main() {
    cin >> maxCars;
	cout << "Maximum number of cars in the tunnel: " << maxCars << endl;
    cin >> maxNorthCars;
	cout << "Maximum number of northbound cars: " << maxNorthCars << endl;
    cin >> maxSouthCars;
    cout << "Maximum number of southbound cars: " << maxSouthCars << endl;

    pthread_t cartid[128]; //128 Threads
    unsigned int ArrivalTime;
    unsigned int TraversalTime;
    char Direction;
    int CarCount = 0; //Counting cars will help keep track of thread ID's

    struct car tempCar; //car object used for main function

    while (cin >> ArrivalTime >> Direction >> TraversalTime) 
     {
        sleep(ArrivalTime);    
        tempCar.carTravelTime = TraversalTime;
        tempCar.waitflag = false; //Car has not waited yet

	switch(Direction){
	case 'N' :
            totalNorthCars++; //incrementing global int's
            tempCar.carNum = totalNorthCars; //updating count for tempCar
            pthread_create(&cartid[CarCount], NULL, NorthboundCar, (void *) &tempCar); //use pthread for NorthboundCar function
	    break;
	case 'S' :
            totalSouthCars++;
            tempCar.carNum = totalSouthCars;
            pthread_create(&cartid[CarCount], NULL, SouthboundCar, (void *) &tempCar); //use pthread for SouthboundCar function
	    break;
	}        
        CarCount++; //increment CarCount for the for loop below
    }

    for (int i = 0; i < CarCount; i++) { //keeping track of thread ID's
        pthread_join(cartid[i], NULL);
    } 
    cout << totalNorthCars << " northbound car(s) crossed the tunnel." << endl;
    cout << totalSouthCars << " southbound car(s) crossed the tunnel." << endl;
    cout << numWaiting << " car(s) had to wait." << endl;
}

//function for case 'N'
void *NorthboundCar(void* arg) {
    int TraversalTime;
    int CarNumber;
    bool waitFlag;

    struct car *TempCar; //Creating temporary car
	TempCar = (struct car *) arg; 

    TraversalTime = TempCar->carTravelTime; //Getting info from arg argument
    CarNumber = TempCar->carNum; //Setting info
    waitFlag = TempCar->waitflag;

    pthread_mutex_lock(&lock);

    cout << "Northbound car # " << CarNumber << " arrives at the tunnel." << endl;
    while (numNorthCars >= maxNorthCars || numCarsInTunnel >= maxCars){ //while loop to increment number of cars waiting
        if (waitFlag == false) {
            waitFlag = true;
            numWaiting++; 
        }
        pthread_cond_wait(&cond, &lock);
    }

    numNorthCars++; //incrementing global int's
    numCarsInTunnel++;
    cout << "Northbound car # " << CarNumber << " enters the tunnel." << endl;

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
    sleep(TraversalTime); 
    pthread_mutex_lock(&lock);

	numNorthCars--;
	numCarsInTunnel--;

    cout << "Northbound car # " << CarNumber << " exits the tunnel." << endl;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&lock); 
}

//function for case 'S' is almost identical to function for case 'N'
void *SouthboundCar(void* arg) {
    int TraversalTime;
    int CarNumber;
    bool waitFlag;

    struct car * TempCar;
	TempCar = (struct car *) arg;

    TraversalTime = TempCar->carTravelTime;
    CarNumber = TempCar->carNum;
    waitFlag = TempCar->waitflag;

    pthread_mutex_lock(&lock);

    cout << "Southbound car # " << CarNumber << " arrives at the tunnel." << endl;
    while (numSouthCars >= maxSouthCars || numCarsInTunnel >= maxCars) {
        if (waitFlag == false) {
            waitFlag = true;
            numWaiting++;
        }
        pthread_cond_wait(&cond, &lock);
    }

    numSouthCars++;
    numCarsInTunnel++;
    cout << "Southbound car # " << CarNumber << " enters the tunnel." << endl;

    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
    sleep(TraversalTime);
    pthread_mutex_lock(&lock);

	numSouthCars--;
	numCarsInTunnel--;

    cout << "Southbound car # " << CarNumber << " exits the tunnel." << endl;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&lock);
}