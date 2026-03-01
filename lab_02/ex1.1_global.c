#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define END 1000

struct thread_data
{
   int *a;
   int limit;
};

void check_array(int *a, int lim) {
    int errors = 0;

    printf("Checking...\n");
    for (int i = 0; i < lim; i++) {
        if (a[i] != NUM_THREADS * i) {
            errors++;
            printf("%d: %d should be %d\n", i, a[i], NUM_THREADS * i);
        }
    }
    printf("%d errors.\n", errors);
}

void *thread_code(void *threadarg) {
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    for (int i = 0; i < END; i++) {
        for (int j = 0; j < i; j++) {
           my_data -> a[i]++; 
        }
    }

    pthread_exit(NULL);
}

int main() {

    int* a;
    a = (int*) malloc(END * sizeof(int));

    int t;

   for(t=0; t<END; t++){
     a[t] = 0;
   }  

    pthread_t threads[NUM_THREADS];
    struct thread_data thread_args[NUM_THREADS];

    for(t=0; t<NUM_THREADS; t++){
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