#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <zconf.h>

//Globals for thread stuff
pthread_mutex_t mutexsum;
static volatile int counter = 0;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

//Global car stuff
int NUMCARS;
int MAXCARS;
int RANDSEED = 42;
//0 == False, 1 == True
int VERBOSITY = 0;
int passedCars = 0;
int FULL = 1;
//array for storing the destination of the car
char current_destination[16] = "";
char travelLock[16] = "";
//cars on bridge
static volatile int cars_OB = 0;

//Data structure to store directions and what thread is being used
struct Direction
{
    char *destination;
    int thread;
};

void output(char *, int, int, int);

void *oneVehicle(void *);

void *arriveOneWay(struct Direction *);

void *onOneWay(struct Direction *);

void *exitOneWay();

int main(int argc, char *argv[])
{
    int rc;
    char *pEnd;
    pthread_mutex_init(&mutexsum, NULL);

    if (argc <= 2)
    {
        fprintf(stderr, "Too few arguments provided, 2 are required...exiting\n");
        exit(EXIT_FAILURE);
    }
    else if (argc > 5)
    {
        fprintf(stderr, "Too many arguments provided, less than 5 expected...exiting\n");
        exit(EXIT_FAILURE);
    }
    else if (argc == 3)
    {
        printf("Setting RANDSEED and VERBOSITY to default values\n");
        NUMCARS = strtol(argv[1], &pEnd, 10);
        MAXCARS = strtol(argv[2], &pEnd, 10);
        if (MAXCARS > NUMCARS)
        {
            printf("MAXCARS: %d can not be larger than NUMCARS: %d\n", MAXCARS, NUMCARS);
            printf("Setting them equal\n");
            MAXCARS = NUMCARS;
        }
        printf("NUMCARS: %d MAXCARS: %d RANDSEED: %d VERBOSITY: %d \n", NUMCARS, MAXCARS, RANDSEED, VERBOSITY);
        printf("--------------------------------------------------\n");
    }
    else
    {
        NUMCARS = strtol(argv[1], &pEnd, 10);
        MAXCARS = strtol(argv[2], &pEnd, 10);
        RANDSEED = strtol(argv[3], &pEnd, 10);
        VERBOSITY = strtol(argv[4], &pEnd, 10);
        if (VERBOSITY > 1)
        {
            printf("Value of Verbosity: %d was too large. Can only be 0 or 1\nSetting output verbosity to false\n", VERBOSITY);
            VERBOSITY = 0;
        }
        if (MAXCARS > NUMCARS)
        {
            printf("MAXCARS: %d can not be larger than NUMCARS: %d\n", MAXCARS, NUMCARS);
            printf("Setting them equal\n");
            MAXCARS = NUMCARS;
        }

        printf("Values set\n");
        printf("NUMCARS: %d MAXCARS: %d RANDSEED: %d VERBOSITY: %d \n", NUMCARS, MAXCARS, RANDSEED, VERBOSITY);
        printf("---------------------------------------------------\n");
    }

    srand(RANDSEED);
    struct Direction directionALLOC[NUMCARS];
    char *DESTINATIONS[2];
    pthread_t *thread_group = malloc(sizeof(pthread_t) * NUMCARS);
    DESTINATIONS[0] = "TO BRIDGER";
    DESTINATIONS[1] = "TO BOZEMAN";
    for (int i = 0; i < NUMCARS; i++)
    {
        char *direction = DESTINATIONS[(rand() & 1)];
        if (i == 0)
        {
            strcpy(current_destination, direction);
        }
        directionALLOC[i].destination = direction;
        directionALLOC[i].thread = i + 1; 
        // printf("1 thread %ld and destination %s\n", t, direction);
        rc = pthread_create(&thread_group[i], NULL, oneVehicle, &directionALLOC[i]);
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i <= NUMCARS; i++)
    {
        pthread_join(thread_group[i], NULL);
    }

    if (VERBOSITY)
    {

        output("yuh", 1, 2, 3);
    }
    return 0;
}

void *oneVehicle(void *direction)
{
    int rc, counter;
    // setting the passed variable of our direction
    struct Direction *carStruct = direction;
    int threadID = carStruct->thread;
    char *carDirection = carStruct->destination;
    //Entering critical region 1
    rc = pthread_mutex_lock(&lock1);
    if (rc)
    {
        printf("Thread %d: acquire failed\n", threadID);
        exit(EXIT_FAILURE);
    }
    while (!FULL)
    {
        printf("Car %d going different direction, need to wait...\n", threadID);
        sleep(1);
        pthread_cond_wait(&cond1, &lock1);
        printf("Can we make it to here\n");
    }

    if (strcmp(current_destination, carDirection) == 0)
    {
        printf("%s\n", travelLock);
        arriveOneWay(carStruct);
        sleep(1);
        onOneWay(carStruct);
        printf("Car %d exited one-way\n", threadID);
        // FULL = 1;
    }
    else
    {
        printf("Car %d is here going %s\n", threadID, carDirection);
        sleep(1);
        pthread_cond_signal(&cond1);
    }
    FULL = 1;
    pthread_mutex_unlock(&lock1);
    // pthread_cond_signal(&cond1);
    pthread_exit(NULL);
    return NULL;
}

void *arriveOneWay(struct Direction *carStruct)
{
    printf("Car %d waiting at one-way going %s\n", carStruct->thread, carStruct->destination);
    return NULL;
}

void *onOneWay(struct Direction *direction)
{
    cars_OB++;
    printf("Car %d is currently on the bridge heading to %s\n", direction->thread, direction->destination);
    return NULL;
}

void *exitOneWay()
{
    cars_OB--;
    return NULL;
}

void output(char *direction, int waiting, int onBridge, int passed)
{
    printf("Direction           Waiting Cars         On Bridge        Cars Passed\n");
    printf("-----------         --------------       -----------      -------------\n");
    printf("    %s              Waiting Cars: %d         %d                 %d\n", direction, waiting, onBridge, passed);
}