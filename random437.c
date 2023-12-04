#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>  // Include for strcmp
#include "random437.h"

#include <stdlib.h>
#include <math.h>




#define MAXWAITPEOPLE 800
#define SIMULATION_MINUTES 600

int CARNUM;
int MAXPERCAR;
int waitingLine = 0;
int totalArrived = 0;
int totalRides = 0;
int totalTurnedAway = 0;

// Mutex for thread synchronization
pthread_mutex_t mutex;



void *arrival_thread(void *arg);
void *ride_thread(void *arg);
void *statistics_thread(void *arg);


int main(int argc, char *argv[]) {
    srand(time(NULL));
    if (argc != 5) {
        fprintf(stderr, "Usage: %s -N CARNUM -M MAXPERCAR\n", argv[0]);
        return 1;
    }

    // Parsing command-line arguments
    for (int i = 1; i < argc; i += 2) {
        if (strcmp(argv[i], "-N") == 0) {
            CARNUM = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-M") == 0) {
            MAXPERCAR = atoi(argv[i + 1]);
        } else {
            fprintf(stderr, "Invalid argument: %s\n", argv[i]);
            return 1;
        }
    }

    // Validate command-line arguments
    if (CARNUM <= 0 || MAXPERCAR <= 0) {
        fprintf(stderr, "CARNUM and MAXPERCAR must be positive integers\n");
        return 1;
    }

    // Initialize mutex
    pthread_mutex_init(&mutex, NULL);

    // Create threads
    pthread_t arr_thread_id, ride_thread_id, stat_thread_id;
    pthread_create(&arr_thread_id, NULL, arrival_thread, NULL);
    pthread_create(&ride_thread_id, NULL, ride_thread, NULL);
    pthread_create(&stat_thread_id, NULL, statistics_thread, NULL);
    // Wait for threads to complete
    pthread_join(arr_thread_id, NULL);
    pthread_join(ride_thread_id, NULL);  // Corrected argument
    pthread_join(stat_thread_id, NULL);

    // Clean up and exit
    pthread_mutex_destroy(&mutex);
    return 0;
}


void *arrival_thread(void *arg) {
    for (int minute = 0; minute < SIMULATION_MINUTES; minute++) {
        pthread_mutex_lock(&mutex);
        
        int meanArrival = 300;
        int arrivals = poissonRandom(meanArrival);
        if (waitingLine + arrivals <= MAXWAITPEOPLE) {
            waitingLine += arrivals;
            totalArrived += arrivals;
        } else {
            totalTurnedAway += arrivals;
        }

        pthread_mutex_unlock(&mutex);
        sleep(1); // Simulate one-minute interval
    }
    return NULL;
}

void *ride_thread(void *arg) {
    for (int minute = 0; minute < SIMULATION_MINUTES; minute++) {
        pthread_mutex_lock(&mutex);

        for (int car = 0; car < CARNUM; car++) {
            int passengers = (waitingLine < MAXPERCAR) ? waitingLine : MAXPERCAR;
            waitingLine -= passengers;
            totalRides += passengers;
        }

        pthread_mutex_unlock(&mutex);
        sleep(1); // Simulate one-minute interval
    }
    return NULL;
}

void *statistics_thread(void *arg) {
    int totalWaitTime = 0;

    for (int minute = 0; minute < SIMULATION_MINUTES; minute++) {
        pthread_mutex_lock(&mutex);

        // Update total wait time
        totalWaitTime += waitingLine; // This is a simplistic way to calculate wait time

        pthread_mutex_unlock(&mutex);
        sleep(1); // Simulate one-minute interval
    }

        if (totalArrived > 0) {
        double averageWaitTime = (double)totalWaitTime / totalArrived;
        printf("Average waiting time per person (in minutes): %.2f\n", averageWaitTime);
    } else {
        printf("No arrivals to calculate average waiting time.\n");
    }

    return NULL;

    // Calculate final statistics
    double averageWaitTime = (double)totalWaitTime / totalArrived;
    printf("Total number of people arrived: %d\n", totalArrived);
    printf("Total number of people taking the ride: %d\n", totalRides);
    printf("Total number of people turned away: %d\n", totalTurnedAway);
    printf("Average waiting time per person (in minutes): %.2f\n", averageWaitTime);

    return NULL;
}

