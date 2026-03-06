#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_THREADS 5

pthread_mutex_t forks[NUM_THREADS];
sem_t room;

typedef struct  {
    int identity;
    int scale;
    int next;
}Philosopher;

void delay(int scale) {
    usleep((rand() % scale) * 1000);
}

void* philosopher_function(void* arg) {
    Philosopher* p = (Philosopher*) arg;
    while(1) {
        printf("Philosopher %d is thinking\n", p->identity);
        delay(p->scale);

        sem_wait(&room);
        printf("Philosopher %d is trying to get fork %d\n", p->identity, p->identity);
        pthread_mutex_lock(&forks[p->identity]);
        printf("Philosopher %d got fork %d and is trying to get fork %d\n", p->identity, p->identity, p->next);
        pthread_mutex_lock(&forks[p->next]);

        printf("Philosopher %d is eating\n", p->identity);

        printf("Philosopher %d is releasing left fork %d\n", p->identity, p->next);
        pthread_mutex_unlock(&forks[p->next]);
        printf("Philosopher %d is releasing fork %d\n", p->identity, p->identity);
        pthread_mutex_unlock(&forks[p->identity]);
    }
    return NULL;
}
int main(){
    pthread_t threads[NUM_THREADS];
    Philosopher philosophers[NUM_THREADS];
    for(int i = 0; i < NUM_THREADS; i++)
    pthread_mutex_init(&forks[i], NULL);
    sem_init(&room, 0, NUM_THREADS - 1);
    for (int t = 0; t < NUM_THREADS; t++) {
        philosophers[t].identity = t;
        philosophers[t].scale = 1000;
        philosophers[t].next = (t+1)%NUM_THREADS;
        pthread_create(&threads[t], NULL, philosopher_function, &philosophers[t]);
    }
    for (int t = 0; t < NUM_THREADS; t++)
    pthread_join(threads[t], NULL);
}