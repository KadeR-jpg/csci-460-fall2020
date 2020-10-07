#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <zconf.h>

//Globals for thread stuff
pthread_mutex_t mutexsum;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

//CMD args
int NUMCARS;
int MAXCARS;
int RANDSEED = 42;


//0 == False, 1 == True
int VERBOSITY = 0;
int SAMEDIRECTION = 1;


//array for storing the destination of the car
char current_destination[16] = "";
char travelLock[16] = "";


//Globals for keeping track of what is happening on the "one-way"
static volatile int cars_OB = 0;
static volatile int carsPassed = 0;
static volatile int carsWaitingBZ = 0;
static volatile int carsWaitingBR = 0;


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
        if (NUMCARS < 1)
        {
            printf("If there are no cars what do we need to simulate?\n");
            exit(EXIT_FAILURE);
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
        }else if(MAXCARS == 0){
            printf("Can't have 0 MAXCARS travelling on the one-way. Exiting\n");
            exit(EXIT_FAILURE);
        }
        else if(NUMCARS == 0){
            printf("If there are no cars what do we need to simulate? Exiting.\n");
            exit(EXIT_FAILURE);
        }else
        {
            printf("MAXCARS: %d can not be larger than NUMCARS: %d\n", MAXCARS, NUMCARS);
            printf("Setting them equal\n");
            MAXCARS = NUMCARS;
        }
        
        if (NUMCARS < 1)
        {
            printf("If there are no cars what do we need to simulate?\n");
            exit(EXIT_FAILURE);
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
    //Setting cars that are waiting from each direction.

    if (strcmp(carDirection, "TO BOZEMAN") == 0)
    {
        carsWaitingBZ++;
    }
    else
    {
        carsWaitingBR++;
    }

    if (threadID == 1)
    {
        strcpy(travelLock, carDirection);
        printf("Taking cars going %s first\n", carDirection);
    }
    rc = pthread_mutex_lock(&lock);
    strcpy(current_destination, carDirection);
    if (rc)
    {
        printf("Thread %d: acquire failed\n", threadID);
        exit(EXIT_FAILURE);
    }
    if (strcmp(travelLock, carDirection) == 0)
    {
        arriveOneWay(carStruct);
        if (cars_OB < MAXCARS)
        {
            onOneWay(carStruct);
            exitOneWay(carStruct);
        }
        else
        {
            printf("One way is full...\n");
        }
    }
    else
    {
        printf("<---- Car %d going different direction need to wait... ---->%s \n", threadID, carDirection);
        //If not going the same direction, wait here for cond to be signaled.
        pthread_cond_wait(&cond, &lock);
        arriveOneWay(carStruct);
        if (cars_OB <= MAXCARS)
        {
            onOneWay(carStruct);
            exitOneWay(carStruct);
        }
        else
        {
            printf("One way is full...\n");
        }
    }
    //Signal the waiting threads.
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
    //Get rid of threeads that are done.
    pthread_exit(NULL);
    return NULL;
}

void *arriveOneWay(struct Car *car)
{
    if (!VERBOSITY)
    {
        printf("Car %d waiting at one-way going %s\n", car->thread, car->destination);
        sleep(1);
    }
    return NULL;
}

void *onOneWay(struct Car *car)
{
    cars_OB++;
    if (!VERBOSITY)
    {
        printf("Car %d --> %s\n", car->thread, car->destination);
    }
    return NULL;
}

void *exitOneWay(struct Car *car)
{
    carsPassed++;
    sleep(1);
    if (strcmp(car->destination, "TO BOZEMAN") == 0)
    {
        carsWaitingBZ--;
    }
    else
    {
        carsWaitingBR--;
    }
    if (!VERBOSITY)
    {
        printf("Car %d exited the one-way\n", car->thread);
    }
    else
    {
        if (carsWaitingBR == 0 && carsWaitingBZ == 0)
        {
            cars_OB = 0;
        }
        printf("\n");
        printf("+---------------------------------------------------+\n");
        printf("| Car %d is exiting on the one way heading to %s\n", car->thread, car->destination);
        printf("|   ______            | Cars on Bridge: %d\n", cars_OB);
        printf("| /|_||_|`.__         | Cars Waiting To go to Bozeman: %d\n", carsWaitingBZ);
        printf("| (   _    _ _|-------| Cars waiting to go to Bridger: %d\n", carsWaitingBR);
        printf("| =`-(_)--(_)-' - - - | Cars Passed: %d\n", carsPassed);
        printf("+----------------------------------------------------+\n");
        printf("\n");
    }
    cars_OB--;
    return NULL;
}