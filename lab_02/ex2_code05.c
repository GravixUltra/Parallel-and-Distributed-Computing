#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define END 1000
#define NUM_THREADS 4

typedef struct 
{
   int *a;
   pthread_mutex_t *mutex;
} thread_data;

void check_array(thread_data *data) {
    int errors = 0;

    printf("Checking...\n");
    for (int i = 0; i < END; i++) {
        if (data->a[i] != NUM_THREADS * i) {
            errors++;
            printf("%d: %d should be %d\n", i, data->a[i], NUM_THREADS * i);
        }
    }
    printf("%d errors.\n", errors);
}

void *thread_code(void *threadarg) {
  thread_data *data = (thread_data *) threadarg;

    for (int i = 0; i < END; i++) {
        for (int j = 0; j < i; j++) {
          pthread_mutex_lock(data->mutex);
          data->a[i]++; 
          pthread_mutex_unlock(data->mutex); 
        }
    }

    pthread_exit(NULL);
}

int main() {
  thread_data obj;
  obj.a = (int*) malloc(END * sizeof(int));
   
   int t;
   for(t=0; t<END; t++){
     obj.a[t] = 0;
   }

   pthread_t threads[NUM_THREADS];
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