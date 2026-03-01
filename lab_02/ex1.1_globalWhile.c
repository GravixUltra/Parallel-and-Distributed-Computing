#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define END 1000
#define NUM_THREADS 4

struct thread_data
{
   int *counter;
   int *a;
   int limit;
};

void check_array(int *a, int limit) {
    int errors = 0;

    printf("Checking...\n");
    for (int i = 0; i < limit; i++) {
        if (a[i] != 1) {
            errors++;
            printf("%d: %d should be 1\n", i, a[i]);
        }
    }
    printf("%d errors.\n", errors);
}

void *thread_code(void *threadarg) {
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;

    while (1) {
        if (*(my_data->counter) >= my_data->limit)
            break;

        my_data -> a[*(my_data->counter)]++;
        (*(my_data->counter))++;
    }

    pthread_exit(NULL);
}

int main() {

    int* a;
    a = (int*) malloc(END * sizeof(int));

    int t;

    int shared_counter = 0;
   for(t=0; t<END; t++){
     a[t] = 0;
   }  

    pthread_t threads[NUM_THREADS];
    struct thread_data thread_args[NUM_THREADS];

    for (int t = 0; t < NUM_THREADS; t++) {
        thread_args[t].counter = &shared_counter;
        thread_args[t].a = a;
        thread_args[t].limit = END;
        pthread_create(&threads[t], NULL, thread_code, (void *)&thread_args[t]);
    }

    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    check_array(a, END);
    return 0;
}