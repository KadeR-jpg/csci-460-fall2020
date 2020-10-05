/*
 * This is the b2bsim.c starter file!
 *
 * TODO: Write a brief summary of your program here.
 *
 * Author: Kade Pitsch :(
 * 
 * includes pthreads, stdlib, stdio, zconf
 * 
*/
#include <pthread.h>
#include <stdlib.h>
#include<stdio.h>
#include <zconf.h>


//Thread stuff
static int target = 1000;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static volatile int counter = 0;


//CAR STUFF
int NUMCARS;
int MAXCARS;
int RANDSEED = 42;
int VERBOSITY = 1;
int ONONEWAY = 0;

char current_destination[15] = "";
static volatile int carO_B = 0;

struct Direction
{
    char *destination;
    int thread;
};




// void output(char *, int, int, int, int);

void *oneVehicle(void *);

// void arriveOneWay();

// void onOneWay();

// void exitOneWay();


/**
// typedefs/structs

// constants

// global data shared by threads (e.g., the "oneway")

// global synchronization primitives (e.g., mutexes, condition variables)

// function declarations
*/

/**
 * TODO: Implement oneVehicle.
 *
 * This is the main function of a car.
 *
 * 1. It simulates a car driving up to one side of the one-way.
 *    The car waits until it is okay to pass.
 *    => arriveOneWay()
 *
 * 2. Once it is safe to pass, the car proceeds onto the oneway and prints the current state of the simulation.
 *    (It is nice to simulate some time passing here - perhaps with a small "sleep" - cars take time to drive!)
 *    => onOneWay()
 *
 * 3. Finally, the thread (car) exits the one-way and finishes.
 *    => exitOneWay()
 
 */

int main(int argc, char* argv[]) {
    int i = 0;
    char *pEnd; 

    if (argc <= 2){
        fprintf(stderr, "Too few arguments provided, 2 are required...exiting\n");
        exit(EXIT_FAILURE);
    }else if (argc > 5)
    {
        fprintf(stderr, "Too many arguments provided. No more than 4 please...exiting\n");
        exit(EXIT_FAILURE);
    }else if(argc == 3){
        printf("Setting RANDSEED and VERBOSITY to default values\n");
        NUMCARS = strtol(argv[1], &pEnd, 10);
        MAXCARS = strtol(argv[2], &pEnd, 10);
        printf("NUMCARS: %d MAXCARS: %d RANDSEED: %d VERBOSITY: %d \n", NUMCARS, MAXCARS, RANDSEED, VERBOSITY);
    }else
    {
        NUMCARS = strtol(argv[1], &pEnd, 10);
        MAXCARS = strtol(argv[2], &pEnd, 10);
        RANDSEED = strtol(argv[3], &pEnd, 10);
        VERBOSITY = strtol(argv[4], &pEnd, 10);
        printf("Values set\n");
        printf("NUMCARS: %d MAXCARS: %d RANDSEED: %d VERBOSITY: %d \n", NUMCARS, MAXCARS, RANDSEED, VERBOSITY);

    }

    pthread_t *thread_group = malloc(sizeof(pthread_t) * NUMCARS);

    for (i = 0; i < NUMCARS; i++){
        pthread_create(&thread_group[i], NULL, oneVehicle, NULL);
        
    }

    for(i = 0; i < NUMCARS; ++i){
        pthread_join(thread_group[i], NULL);
    }
    return EXIT_SUCCESS;

    // TODO: initializations for simulation

    // TODO: main loop that drives the simulation
    // In each iteration of the loop...
    //   - initialize/update any info for keeping track of threads (cars)
    //   - create a thread (car) that starts running "oneVehicle"

    // TODO: wait for car threads to finish & cleanup.

    // TODO: display final state of your oneway simulation before exiting.

    return 0;
}

void *oneVehicle(void *direction){
    struct Direction *carStruct = direction;
    char direction [30];
    strcpy(direction, carStruct->destination);
    int threadID = carStruct->thread;
    int check;
    int checkMAX;


    printf("Car %d: Has arrived at one way Direction: %s" threadID, direction);
    check = pthread_mutex_lock(&lock);
    if (check) {
        printf("Thead %d: acquire failed!\n", threadID);
        EXIT_FAILURE
    }

    // while (checkMAX <= MAXCARS && direction )
    {
        
    }
    


    



}

// void arriveOneWay(){

// }

// void onOneWay(void *direction){



// }

// void exitOneWay(){

// }


// void output(char *direction, int waiting, int onBridge, int passed, int verbose){
//     if(verbose == 1){
//         //verbose output
//     }else{
//         //light-weight output
//     }
// }