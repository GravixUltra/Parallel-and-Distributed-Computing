// οι καταναλωτές δεν σταματούν: τερματισμός με CTRL-C

#include <pthread.h>
#include <semaphore.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 1   // το μέγεθος του κυκλικού buffer
#define LOOP 10   // αριθμός επαναλήψεων παραγωγού
#define NPRO 1    // αριθμός παραγωγών
#define NCON 1    // αριθμός καταναλωτών
#define DPRO 100000  // καθυστέρηση παραγωγού
#define DCON 200000  // καθυστέρηση καταναλωτή

typedef struct {
  int buffer[SIZE];     // ο κυκλικός buffer
  // int front, back;      // αριθμοδείκτες προσθήκης και αφαίρεσης 
  // sem_t *mut; 				 binary semaphore για τον αμοιβαίο αποκλεισλμό δεν χρειάζεται αφού ο Consumer γράφει στο front και ο Producer στο back
	// και δεν αγγιζουν την ιδια θεση ταυτοχρονα και για size = 1
  sem_t *toPut, *toGet; // counting semaphores για σηματοδοσία
} queue;

typedef struct 
{
   int	id;
   queue *fifo;
}thread_data;

// αρχικοποίηση ουράς
queue *queueInit (void)
{
  queue *q;

  q = (queue *)malloc (sizeof (queue));
  if (q == NULL) return (NULL);

  // αρχικά η ουρά είναι άδεια, αρχικοποιούμε τις μεταβλητές 
  // q->front = 0;
  // q->back = SIZE - 1;
  // q->mut = (sem_t *) malloc (sizeof (sem_t));
  // sem_init(q->mut, 0, 1);
  q->toPut = (sem_t *) malloc (sizeof (sem_t));
  sem_init(q->toPut, 0, SIZE);
  q->toGet = (sem_t *) malloc (sizeof (sem_t));
  sem_init(q->toGet, 0, 0);
	
  return (q);
}

// διαγραφή της ουράς
void queueDelete (queue *q)
{
  // sem_destroy (q->mut);
  // free (q->mut);	
  sem_destroy (q->toPut);
  free (q->toPut);
  sem_destroy (q->toGet);
  free (q->toGet);
  free (q);
}

// προσθήκη στοιχείου στην ουρά
void queueAdd (queue *q, int in)
{
  // ελέγχουμε αν ο buffer είναι γεμάτος P(q->toPut)
  sem_wait (q->toPut);

  // αν ναι προσπαθούμε να πάρουμε το κλειδί του buffer
  // sem_wait (q->mut);
  
  // προσθέτουμε το στοιχείο 
  // q->back = (q->back + 1) % SIZE; δεν χ
  // q->buffer[q->back] = in;
    q->buffer[0] = in;
  
  // ξεκλειδώνουμε το buffer
  // sem_post (q->mut);
  
  // σηματοδοτούμε οτι ο buffer διαθέτει στοιχεία V(q->toGet) 
  sem_post (q->toGet);
  
 
  return;
}

// αφαίρεση στοιχείου από την ουρά
void queueDel (queue *q, int *out)
{

  // ελέγχουμε αν ο buffer είναι άδειος P(q->toGet)
  sem_wait (q->toGet);

  // αν ναι προσπαθούμε να πάρουμε το κλειδί του buffer
  // sem_wait (q->mut);

  // αφαιρούμε το στοιχείο 
  // *out = q->buffer[q->front];
  // q->front = (q->front + 1) % SIZE;

  *out = q->buffer[0];
  // ξεκλειδώνουμε το buffer
  // sem_post (q->mut);
  
  // σηματοδοτούμε οτι ο buffer διαθέτει κενές θέσεις V(q->toPut) 
  sem_post (q->toPut);
  
  return;
}

// το νήμα του παραγωγού (βάζουμε καθυστέρηση για δοκιμή)
void *producer (void *threadarg)
{
  int i, myid;
  queue *fifo;
  thread_data *my_data;
  my_data = (thread_data *) threadarg;
  myid = my_data->id;
  fifo = (queue *)my_data->fifo;

  for (i = 0; i < LOOP; i++) {
    printf("Producer %d item %d \n", myid,i);
    queueAdd (fifo, i);
    usleep (DPRO);
  }
  return (NULL);
}

// το νήμα του καταναλωτή (βάζουμε καθυστέρηση για δοκιμή)
void *consumer (void *threadarg)
{
  int i, myid;
  queue *fifo;
  thread_data *my_data;
  my_data = (thread_data *) threadarg;
  myid = my_data->id;
  fifo = (queue *)my_data->fifo;

  for (;;) {
    queueDel (fifo, &i);
    printf("Consumer %d item %d \n", myid, i);
    usleep (DCON);
  }
  return (NULL);
}

// η main για να ξεκινήσουμε τα νήματα
int main ()
{
  queue *fifo;
  pthread_t pro[NPRO], con[NCON];
  thread_data pro_args[NPRO];
  thread_data con_args[NCON];
  int i;

  fifo = queueInit ();
  if (fifo ==  NULL) {
    exit (1);
  }
  
  for (i=0; i<NPRO; i++) {
       pro_args[i].id = i;
       pro_args[i].fifo = fifo;
       pthread_create (&pro[i], NULL, producer, &pro_args[i]);
  }     
  for (i=0; i<NCON; i++) {
       con_args[i].id = i;
       con_args[i].fifo = fifo;    
       pthread_create (&con[i], NULL, consumer, &con_args[i]);
  }     
  for (i=0; i<NPRO; i++)     
       pthread_join (pro[i], NULL);
  for (i=0; i<NCON; i++)        
  		pthread_join (con[i], NULL);
  		
  queueDelete (fifo);

  return 0;
}
