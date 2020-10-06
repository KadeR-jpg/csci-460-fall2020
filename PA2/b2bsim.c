#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <zconf.h>

//Globals for thread stuff
pthread_mutex_t mutexsum;
static volatile int counter = 0;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

//Global car stuff
int NUMCARS;
int MAXCARS;
int RANDSEED = 42;
//0 == False, 1 == True
int VERBOSITY = 0;
int SAMEDIRECTION = 1;
//array for storing the destination of the car
char current_destination[16] = "";
char travelLock[16] = "";
//cars on bridge
static volatile int cars_OB = 0;
static volatile int carsPassed = 0;
//Data structure to store directions and what thread is being used
struct Car
{
    int thread;
    char *destination;
};

void output(char *, int, int, int);

void *oneVehicle(void *);

void *arriveOneWay(struct Car *);

void *onOneWay(struct Car *);

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
    struct Car CAR_Struct[NUMCARS];
    char *DIRECTION[2];
    pthread_t thread_group[NUMCARS];
    DIRECTION[0] = "TO BRIDGER";
    DIRECTION[1] = "TO BOZEMAN";
    for (int i = 0; i < NUMCARS; i++)
    {
        char *direction = DIRECTION[(rand() & 1)];
        CAR_Struct[i].thread = i + 1;
        CAR_Struct[i].destination = direction;
        rc = pthread_create(&thread_group[i], NULL, oneVehicle, &CAR_Struct[i]);
        if (rc)
        {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < NUMCARS; i++)
    {
        pthread_join(thread_group[i], NULL);
    }
    return 0;
}

void *oneVehicle(void *direction)
{
    //Declaring Function level variables
    int rc;
    struct Car *carStruct = direction;
    int threadID = (int)carStruct->thread;
    char *carDirection = carStruct->destination;
    if (threadID == 1)
    {
        strcpy(travelLock, carDirection);
        printf("Taking cars going %s first\n", carDirection);
    }
    sleep(1);
    rc = pthread_mutex_lock(&lock);
    strcpy(current_destination, carDirection);
    if (rc)
    {
        printf("Thread %d: acquire failed\n", threadID);
        exit(EXIT_FAILURE);
    }
    if (strcmp(travelLock, carDirection) == 0)
    {
        printf("---->\n");
        arriveOneWay(carStruct);
        if (cars_OB < MAXCARS)
        {
            onOneWay(carStruct);
            exitOneWay(carStruct);
        }
        else
        {
            printf("Bridge is fucking Full bro\n");
        }
    }
    else
    {
        printf("<------\n Car %d going different direction, %s \n", threadID, carDirection);
        pthread_cond_wait(&cond, &lock);
        arriveOneWay(carStruct);
        if (cars_OB <= MAXCARS)
        {
            onOneWay(carStruct);
            exitOneWay(carStruct);
        }
        else
        {
            printf("One way is fucing full\n");
        }
    }
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);
    // return NULL;
}

void *arriveOneWay(struct Car *car)
{
    printf("Car %d waiting at one-way going %s\n", car->thread, car->destination);
    sleep(1);
    return NULL;
}

void *onOneWay(struct Car *car)
{
    cars_OB++;
    printf("Car %d is currently on the bridge heading to %s\n", car->thread, car->destination);
    return NULL;
}

void *exitOneWay(struct Car *car)
{
    cars_OB--;
    
    sleep(1);
    printf("Car %d exited the one-way %s\n", car->thread, car->destination);
    return NULL;
}

void output(char *direction, int waiting, int onBridge, int passed)
{
    printf("Direction           Waiting Cars         On Bridge        Cars Passed\n");
    printf("-----------         --------------       -----------      -------------\n");
    printf("    %s              Waiting Cars: %d         %d                 %d\n", direction, waiting, onBridge, passed);
}