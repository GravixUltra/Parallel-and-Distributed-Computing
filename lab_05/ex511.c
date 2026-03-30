#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS	4
#define SIZE 1000

struct thread_data {
    double **a;
    double **b;
    double **c;
    int from;
    int to;
};

void *MatrixThread(void *threadarg)
{

   struct thread_data *my_data;
   my_data = (struct thread_data *) threadarg;
   
   double **my_a = my_data->a;
   double **my_b = my_data->b;
   double **my_c = my_data->c;
   int my_from = my_data->from;
   int my_to = my_data->to;

    // Πρόσθεση πινάκων
    for(int i=my_from; i<my_to; i++) 
        for(int j = 0; j < SIZE; j++)
            my_a[i][j] = my_b[i][j] + my_c[i][j];
   
   pthread_exit(NULL);
}

int main() {

    double **a = malloc(SIZE * sizeof(double*));
    double **b = malloc(SIZE * sizeof(double*));
    double **c = malloc(SIZE * sizeof(double*));

    for(int i = 0; i < SIZE; i++) {
    a[i] = malloc(SIZE * sizeof(double));
    b[i] = malloc(SIZE * sizeof(double));
    c[i] = malloc(SIZE * sizeof(double));
    }
    // Αρχικοποίηση πινάκων
    for(int i = 0; i < SIZE; i++)
        for(int j = 0; j < SIZE; j++) {
            a[i][j] = 0.1;
            b[i][j] = 0.3;
            c[i][j] = 0.5;
        }

    // // Πρόσθεση πινάκων - Η πρόσθεση θα γίνει στην MatrixThread (για να καλεστεί μετά από την pthread_create)
    // for (int t=0; t< NUM_THREADS; t++)
    // for(int i = 0; i < SIZE; i++)
    //     for(int j = 0; j < SIZE; j++)
    //         a[i][j] = b[i][j] + c[i][j];

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
        pthread_create(&threads[t], NULL, MatrixThread, (void *) &thread_args[t]);
        }
        for(int t=0; t<NUM_THREADS; t++){
            pthread_join(threads[t], NULL);
        }
       

        /* Για debugging
        for(int i = 0; i < SIZE; i++) {
            for(int j = 0; j < SIZE; j++)
            printf("%f ", a[i][j]);
            printf("\n");
        } */
        for (int i = 0; i < SIZE; i++) {

            free(a[i]);
            free(b[i]);
            free(c[i]);
        }
        free(a);
        free(b);
        free(c);


        return 0;
}