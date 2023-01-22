/*
    METADATA
    - time unit: millisecond (ms)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

/* CONSTANTS */
#define MAX_CUSTOMERS 27
#define CHAIR_CAPACITY 5 // N chairs
#define TRUE 1

/* GLOBAL VARIABLES */
/* INPUT */
int num_customer;
int max_arrival_time;
int max_haircut_duration;
int haircut_repetition;

/* PROPERTIES */
int seats_free = CHAIR_CAPACITY;
int customers_waiting;
int haircut_counts[MAX_CUSTOMERS];
double waiting_times[MAX_CUSTOMERS];

/* SYNCHRONOZATION */
pthread_t barber_t;
pthread_mutex_t mutex;
sem_t barber_sem;
sem_t customer_sem;
sem_t seat_sem;

void *barberf(void *arg)
{
    int got_haircut = 0;
  
    while (got_haircut < (num_customer - customers_waiting))
    {   
        int i;

        // wait for a customer
        sem_wait(&customer_sem);
        // no free chairs, customer leaves
        if (seats_free == CHAIR_CAPACITY)
        {
            pthread_exit(NULL);
            return NULL;
        }
        seats_free++;
        sem_post(&seat_sem);
        // barber ready to give a haircut
        sem_post(&barber_sem);
        // customer takes a seat
        // barber cannot give a haircut to another customer
        pthread_mutex_lock(&mutex);
        // customer gets a haircut
        while (haircut_counts[i] < haircut_repetition)
        {
           usleep(max_haircut_duration * 1000);
           haircut_counts[i]++;
        }
        // free the barber
        pthread_mutex_unlock(&mutex);
        got_haircut++;
    }
}

void *customerf(void *arg)
{
    int *id = (int *)arg;
    // customer i will have the haircut haircut_repetition times

    sem_wait(&seat_sem);
    // if no free chair, leave
    if (seats_free < 1)
    {
        sem_post(&seat_sem);
        customers_waiting++;
        printf("Customer %d leaves.\n", *id);
    }

    // free chair available
    if (seats_free >= 1 && seats_free <= CHAIR_CAPACITY)
    {
        struct timespec begin, end; // START TICKING
        clock_gettime(CLOCK_REALTIME, &begin);
        seats_free--;
        printf("Customer %d waits.\n", *id);
        printf("Seats free: %d\n", seats_free);

        // if barber is sleeping, wake him up and get a haircut
        sem_post(&customer_sem);
        // if barber's working, have a seat and wait
        sem_post(&seat_sem);
        sem_wait(&barber_sem);
        clock_gettime(CLOCK_REALTIME, &end); /*STOP TICKING HUNGRY TIME*/
        long seconds = end.tv_sec - begin.tv_sec;
        long nanoseconds = end.tv_nsec - begin.tv_nsec;
        double elapsed_ms = (seconds + nanoseconds * 1e-9) * 1000;
        waiting_times[*id] = elapsed_ms;
    }
    haircut_counts[*id]++;
    pthread_exit(NULL);
}

int main(int argc, int *argv[])
{
    num_customer = atoi(argv[1]);
    max_arrival_time = atoi(argv[2]);
    max_haircut_duration = atoi(argv[3]);
    haircut_repetition = atoi(argv[4]);

    /* DEBUG */
    printf("******************************\n");
    printf("num_customer: %d\n", num_customer);
    printf("max_arrival_time: %d\n", max_arrival_time);
    printf("max_haircut_duration: %d\n", max_haircut_duration);
    printf("haircut_repetition: %d\n", haircut_repetition);
    printf("******************************\n");
    /* DEBUG */

    int i;
    int customers[num_customer];
    pthread_t customer_tids[num_customer]; /* customers thread */
    pthread_t barber_tid;                  /* barber thread */

    // Initialize synchronization variables
    pthread_mutex_init(&mutex, NULL);
    sem_init(&barber_sem, 0, 0); // barber sleeping
    sem_init(&customer_sem, 0, 0);
    sem_init(&seat_sem, 0, 1);

    // Initialize barber
    barber_tid = pthread_create(&barber_tid, NULL, barberf, NULL);

    // Initialize customers
    for (i = 0; i < num_customer; i++)
    {
        customers[i] = i;
        pthread_create(&customer_tids[i], NULL, customerf, &customers[i]);
        printf("Initialize customer %d\n", i);
        usleep(max_arrival_time * 1000);
    }
    // Wait for barber to finish
    pthread_join(barber_tid, NULL);

    // Wait for all customers to get a haircut
    for (i = 0; i < num_customer; i++)
    {
        pthread_join(customer_tids[i], NULL);
    }

    // Print pesults
    for (i = 0; i < num_customer; i++)
    {
        printf("Customer %d waited %f ms for %d haircuts \n", i, waiting_times[i], haircut_counts[i]);
    }

    exit(EXIT_SUCCESS);
}