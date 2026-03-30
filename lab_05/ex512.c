#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS	4
#define SIZE 10

struct thread_data {
    double *a;
    double *b;
    double *c;
    int from;
    int to;
};

void *VectorThread(void *threadarg)
{

   struct thread_data *my_data;
   my_data = (struct thread_data *) threadarg;
   
   double *my_a = my_data->a;
   double *my_b = my_data->b;
   double *my_c = my_data->c;
   int my_from = my_data->from;
   int my_to = my_data->to;

    // Πρόσθεση διανυσμάτων
    for(int i=my_from; i<my_to; i++) 
    my_a[i] = my_b[i] + my_c[i];
   
   pthread_exit(NULL);
}

int main() {
    double *a = malloc(SIZE * sizeof(double));
    double *b = malloc(SIZE * sizeof(double));
    double *c = malloc(SIZE * sizeof(double));

    // Αρχικοποίηση διανυσμάτων
    for(int i = 0; i < SIZE; i++) {
        a[i] = 0.0;
        b[i] = 1.0;
        c[i] = 0.5;
    }

    pthread_t threads[NUM_THREADS];
    struct thread_data thread_args[NUM_THREADS];

    int block = SIZE / NUM_THREADS;
    for(int t=0; t<NUM_THREADS; t++){
        thread_args[t].from = t*block;
        thread_args[t].to = thread_args[t].from+block;
        if (t == NUM_THREADS-1) thread_args[t].to = SIZE;
        thread_args[t].a = a;
        thread_args[t].b = b;
        thread_args[t].c = c;
        pthread_create(&threads[t], NULL, VectorThread, (void *) &thread_args[t]);
        }
        for(int t=0; t<NUM_THREADS; t++){
            pthread_join(threads[t], NULL);
        }

    // Πρόσθεση διανυσμάτων - Η πρόσθεση θα γίνει στην VectorThread (για να καλεστεί μετά από την pthread_create)
    // for(int i = 0; i < SIZE; i++)
    //     a[i] = b[i] + c[i];

    /* Για debugging
    for(int i = 0; i < SIZE; i++)
        printf("%f\n", a[i]);
    */
        free(a);
        free(b);
        free(c);

    return 0;
}