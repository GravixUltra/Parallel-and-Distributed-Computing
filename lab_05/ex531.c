#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define MAX_RESULTS 1000000
#define NUM_THREADS	8

struct thread_data
{
   int	from;
   int	to;
   int size;
};

char *output[MAX_RESULTS];
int output_count = 0;
pthread_mutex_t output_mutex;

void saveResult(bool *v, int size, int z) {
    int bufsize = size * 2 + 20;
    char *result = (char *)malloc(bufsize);

    sprintf(result, "%d", z);

    for (int i = 0; i < size; i++)
        if (v[i]) strcat(result, " 1");
        else strcat(result, " 0");
    
    // Just print result for debugging
    // printf("%s\n", result);
    
    // Save result
    pthread_mutex_lock(&output_mutex);
    output[output_count++] = result;
    pthread_mutex_unlock(&output_mutex);
}

void check_circuit(int z, int size) {
    bool v[size];

    for (int i = size - 1; i >= 0; i--)
        v[i] = (z & (1 << i)) != 0;

    bool value =
           (  v[0]  ||  v[1]  )
        && ( !v[1]  || !v[3]  )
        && (  v[2]  ||  v[3]  )
        && ( !v[3]  || !v[4]  )
        && (  v[4]  || !v[5]  )
        && (  v[5]  || !v[6]  )
        && (  v[5]  ||  v[6]  )
        && (  v[6]  || !v[15] )
        && (  v[7]  || !v[8]  )
        && ( !v[7]  || !v[13] )
        && (  v[8]  ||  v[9]  )
        && (  v[8]  || !v[9]  )
        && ( !v[9]  || !v[10] )
        && (  v[9]  ||  v[11] )
        && (  v[10] ||  v[11] )
        && (  v[12] ||  v[13] )
        && (  v[13] || !v[14] )
        && (  v[14] ||  v[15] )
        && (  v[14] ||  v[16] )
        && (  v[17] ||  v[1]  )
        && (  v[18] || !v[0]  )
        && (  v[19] ||  v[1]  )
        && (  v[19] || !v[18] )
        && ( !v[19] || !v[9]  )
        && (  v[0]  ||  v[17] )
        && ( !v[1]  ||  v[20] )
        && ( !v[21] ||  v[20] )
        && ( !v[22] ||  v[20] )
        && ( !v[21] || !v[20] )
        && (  v[22] || !v[20] );

    if (value)
        saveResult(v, size, z);
}

void *VecThread(void *threadarg){
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
   
    int my_from = my_data->from;
    int my_to = my_data->to;
    int my_size = my_data->size;
   
    for(int i=my_from; i<my_to; i++) 
    check_circuit(i, my_size);
     
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int size = 0;

    if (argc != 2) {
        printf("Usage: %s <vector size>\n", argv[0]);
        return 1;
    }

    size = atoi(argv[1]);
    if (size <= 0) {
        printf("size should be positive integer\n");
        return 1;
    }

    int iterations = (int)pow(2, size);

    clock_t start = clock();
                
    // for (int i = 0; i < iterations; i++)
    //     check_circuit(i, size);

    pthread_t threads[NUM_THREADS];
    struct thread_data thread_args[NUM_THREADS];
    pthread_mutex_init(&output_mutex, NULL);
    int block = iterations / NUM_THREADS;
    for(int t=0; t<NUM_THREADS; t++){
     thread_args[t].from = t*block;
     thread_args[t].to = thread_args[t].from+block;
     if (t == NUM_THREADS-1) thread_args[t].to = iterations;
     thread_args[t].size = size;
     pthread_create(&threads[t], NULL, VecThread, (void *) &thread_args[t]);
   }
   
   for(int t=0; t<NUM_THREADS; t++){
     pthread_join(threads[t], NULL);
   }
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    // print output
    for (int i = 0; i < output_count; i++) {
        printf("%s\n", output[i]);
        free(output[i]);
    }

    printf("\nAll done\n");
    printf("time in sec = %.3f\n", elapsed);
    pthread_mutex_destroy(&output_mutex);
    return 0;
}

/* Times measured:
Size: 1       2       4       8       Threads
23    0,508   0,271   0,214   0,227
24    1,100   0,540   0,416   0,432
25    2,162   1,061   0,767   0,837
26    4,335   2,266   1,563   1,546
*/