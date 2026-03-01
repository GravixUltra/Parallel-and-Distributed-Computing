#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define END 1000
#define NUM_THREADS 4

int counter = 0;
int array[END];         
pthread_mutex_t shared_mutex;

void check_array(void) {
    int errors = 0;

    printf("Checking...\n");
    for (int i = 0; i < END; i++) {
        if (array[i] != 1) {
            errors++;
            printf("%d: %d should be 1\n", i, array[i]);
        }
    }
    printf("%d errors.\n", errors);
}

void *thread_code(void *threadarg) {
    while (1) {
        pthread_mutex_lock(&shared_mutex);
        if (counter >= END){
            pthread_mutex_unlock(&shared_mutex);
            break;}

        array[counter]++;
        counter++;
        pthread_mutex_unlock(&shared_mutex);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&shared_mutex, NULL);

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_create(&threads[t], NULL, thread_code, NULL);
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    check_array();
    pthread_mutex_destroy(&shared_mutex);

    return 0;
}