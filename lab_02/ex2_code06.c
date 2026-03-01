#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define END 1000
#define NUM_THREADS 4      

typedef struct  {
    pthread_mutex_t *mutex;
    int counter; 
    int *a;
} thread_data;

void check_array(thread_data *my_data) {
    int errors = 0;

    printf("Checking...\n");
    for (int i = 0; i < END; i++) {
        if (my_data->a[i] != 1) {
            errors++;
            printf("%d: %d should be 1\n", i, my_data->a[i]);
        }
    }
    printf("%d errors.\n", errors);
}

void *thread_code(void *threadarg) {
    thread_data *my_data = (thread_data *)threadarg;

    while (1) {
        pthread_mutex_lock(my_data->mutex);
        if (my_data->counter >= END){
            pthread_mutex_unlock(my_data->mutex);
            break;}

        my_data->a[my_data->counter]++;
        my_data->counter++;
        pthread_mutex_unlock(my_data->mutex);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[NUM_THREADS];
    thread_data obj;
    obj.counter = 0;
    obj.a = (int*) malloc(END * sizeof(int));
    for (int i = 0; i < END; i++) {
        obj.a[i] = 0;
    }

    pthread_mutex_t shared_mutex;
    pthread_mutex_init(&shared_mutex, NULL);
    obj.mutex = &shared_mutex;
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_create(&threads[t], NULL, thread_code, (void *)&obj);
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    check_array(&obj);

    pthread_mutex_destroy(&shared_mutex);
    return 0;
}