#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h> 
#include <pthread.h>
#include <unistd.h>
#include <time.h>

typedef struct {
    int capacity;
    int spaces;
    int waitscale;   
    int inscale;   
    sem_t *mut; 					
    sem_t *free_spaces;
} Park;

void park_init(Park *p, int capacity) {
    p->capacity  = capacity;
    p->spaces    = capacity;
    p->waitscale = 10;
    p->inscale   = 5;
    p->mut = (sem_t *) malloc (sizeof (sem_t));
    sem_init(p->mut, 0, 1);
    p->free_spaces = (sem_t *) malloc(sizeof(sem_t)); 
    sem_init(p->free_spaces, 0, capacity);
}

void park_destroy(Park *p) {
    sem_destroy (p->mut);
    free (p->mut);	
    sem_destroy(p->free_spaces);  
    free(p->free_spaces);
}

void arrive(Park *p) {
    int delay_ms = rand() % p->waitscale;
    struct timespec ts = { 0, delay_ms * 1000000L };
    nanosleep(&ts, NULL);

    sem_wait (p->free_spaces);
    sem_wait (p->mut);
    printf("%lu arrival\n",    pthread_self());
    printf("%lu     parking\n", pthread_self());
    p->spaces--;
    printf("free %d\n", p->spaces);
    sem_post (p->mut);
}

void park_stay(Park *p) {
    int stay_ms = rand() % p->inscale;
    struct timespec ts = { 0, stay_ms * 1000000L };
    nanosleep(&ts, NULL);
}

void depart(Park *p) {
    sem_wait (p->mut);
    printf("%lu         departure\n", pthread_self());
    p->spaces++;
    printf("free %d\n", p->spaces);
    sem_post (p->mut);
    sem_post (p->free_spaces);
}

typedef struct {
    Park *park;
} CarArgs;

void *car_thread(void *arg) {
    CarArgs *a = (CarArgs *)arg;
    for (int i = 0; i < 100; i++) {
        arrive(a->park);
        park_stay(a->park);
        depart(a->park);
    }
    return NULL;
}

int main(void) {
    srand((unsigned)time(NULL));

    int capacity = 4;
    int num_cars = 20;

    Park park;
    park_init(&park, capacity);

    pthread_t threads[num_cars];
    CarArgs   args[num_cars];

    for (int i = 0; i < num_cars; i++) {
        args[i].park = &park;
        if (pthread_create(&threads[i], NULL, car_thread, &args[i]) != 0) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < num_cars; i++) {
        pthread_join(threads[i], NULL);
    }

    park_destroy(&park);
    printf("Simulation complete.\n");
    return EXIT_SUCCESS;
}