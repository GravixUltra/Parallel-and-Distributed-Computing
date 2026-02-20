#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS	10

struct thread_data
{
    int id;
};

void *Print(void *threadarg)
{
    int myid;
    struct thread_data *my_data;
    my_data = (struct thread_data *) threadarg;
    myid = my_data->id;
    printf("Thread: %d\n", myid);
    myid = myid+1;
    for(int i=1; i<=20; i++)
    {
        printf("%d * %d = %d\n",i, myid, i*myid);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   struct thread_data thread_args[NUM_THREADS];
   int t;
   
   for(t=0; t<NUM_THREADS; t++){
      int id = t;
      printf("In main: creating thread %d\n", t);
      thread_args[t].id = t;
      pthread_create(&threads[t], NULL, Print, (void *) &thread_args[t]);
    //   pthread_join(threads[t], NULL);
   }


//    Αν εκτυπωθούν ταυτόχρονα όλα τα νήματα, οι εκτυπώσεις μπορεί να εμπλακούν η μία με την άλλη
// Αν απομονώσουμε τις εκτυπώσεις ενός νήματος, τότε οι εκτυπώσεις δεν μπλέκονται και είναι καθαρή (αρκεί να βάλουμε σε σχόλιο τον παρακάτω κώδικα join και να βγάλουμε τα σχόλια στην join αμέσως μετά την create)

   for(t=0; t<NUM_THREADS; t++){
    pthread_join(threads[t], NULL);
   }
   printf("All threads have finished\n");
}