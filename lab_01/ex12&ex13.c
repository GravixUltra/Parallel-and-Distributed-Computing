#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS	10

struct thread_dataA
{
   int	id;
};

struct thread_dataB
{
   int	id;
   int value;
};

void *PrintHelloA(void *threadarg)
{
   int myid;
   struct thread_dataA *my_data;

   my_data = (struct thread_dataA *) threadarg;
   myid = my_data->id;
   printf("Thread: %d\n", myid);
   printf("Hello A! It's me, thread #%d out of %d!\n", myid, NUM_THREADS);
   pthread_exit(NULL);
}

void *PrintHelloB(void *threadarg)
{
   int myid;
   struct thread_dataB *my_data, *my_value;
   int myvalue;

   my_data = (struct thread_dataB *) threadarg;
   my_value = (struct thread_dataB *) threadarg;
   myid = my_data->id;
   myvalue = my_value->value;
   printf("Thread: %d\n", myid);
   printf("Hello B! It's me, thread #%d, with value = %d, out of %d!\n", myid, myvalue, NUM_THREADS);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
   pthread_t threadsA[NUM_THREADS], threadsB[NUM_THREADS];
   struct thread_dataA thread_argsA[NUM_THREADS];
   struct thread_dataB thread_argsB[NUM_THREADS];
   int t;
   
   for(t=0;t<NUM_THREADS;t++){
   thread_argsA[t].id = t;
   thread_argsB[t].id = t;
   printf("In main: creating thread %d\n", t);
   thread_argsB[t].value = t+5;

     pthread_create(&threadsA[t], NULL, PrintHelloA, (void *) &thread_argsA[t]);
   
     pthread_create(&threadsB[t], NULL, PrintHelloB, (void *) &thread_argsB[t]);
   }
   for(t=0;t<NUM_THREADS;t++){
     pthread_join(threadsA[t], NULL);
   
     pthread_join(threadsB[t], NULL);
   }
   printf("All threads have finished\n");
}
