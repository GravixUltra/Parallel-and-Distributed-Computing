#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

typedef struct {
    int capacity;
    int spaces;
    int waitscale;   
    int inscale;   
    pthread_mutex_t *mut;
    pthread_cond_t *condFull;
} Park;

void park_init(Park *p, int capacity) {
    p->capacity  = capacity;
    p->spaces    = capacity;
    p->waitscale = 10;
    p->inscale   = 5;
    p->mut = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(p->mut, NULL);
    p->condFull = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(p->condFull, NULL);
}

void park_destroy(Park *p) {
    pthread_mutex_destroy(p->mut);
	free(p->mut);
	pthread_cond_destroy(p->condFull);
	free(p->condFull);
}

void arrive(Park *p) {
    int delay_ms = rand() % p->waitscale;
    struct timespec ts = { 0, delay_ms * 1000000L };
    nanosleep(&ts, NULL);

    pthread_mutex_lock(p->mut);
    printf("%lu arrival\n",    pthread_self());
    printf("%lu     parking\n", pthread_self());
    // ελέγχουμε αν ο buffer είναι γεμάτος
	while (p->spaces == 0) {
		// αν είναι γεμάτος περιμένουμε στην συνθήκη
		printf("Buffer full \n");
		pthread_cond_wait(p->condFull, p->mut);
	}
    p->spaces--;
    printf("free %d\n", p->spaces);
    pthread_mutex_unlock(p->mut);
}

void park_stay(Park *p) {
    int stay_ms = rand() % p->inscale;
    struct timespec ts = { 0, stay_ms * 1000000L };
    nanosleep(&ts, NULL);
}

void depart(Park *p) {
    pthread_mutex_lock(p->mut);
    printf("%lu         departure\n", pthread_self());
    p->spaces++;
    printf("free %d\n", p->spaces);
    pthread_cond_signal(p->condFull);
    pthread_mutex_unlock(p->mut);
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