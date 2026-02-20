#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS	5

// Μπορούμε να δημιουργήσουμε νήματα χωρίς δομή δεδομένων, ωστόσο η join θα γίνεται αμέσως μετά από κάθε create,
// διότι δεν έχουμε πίνακα πλέον να αποθηκεύσουμε όλα τα threads! Επομένως εφόσον γίνεται μέσα στο ίδιο loop, δεν γίνονται παράλληλα.
// Το HelloArgs.c γίνεται ίδιο με το HelloArg.c (καθώς το total μπορούμε να το αφήσουμε ως το NUM_THREADS)
void *PrintHello(void *x)
{
   int myid;

   myid = *((int *) x);
   printf("Hello World! It's me, thread #%d out of %d!\n", myid, NUM_THREADS);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
   pthread_t thread;
   int t;
   
   for(t=0; t<NUM_THREADS; t++){
      int id = t;
      printf("In main: creating thread %d\n", t);
      pthread_create(&thread, NULL, PrintHello, (void *) &t);
      pthread_join(thread, NULL);
   }

   printf("All threads have finished\n");
}
