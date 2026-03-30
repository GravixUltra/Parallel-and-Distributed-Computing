// οι καταναλωτές δεν σταματούν: τερματισμός με CTRL-C

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//Δεν μπορούμε να βγάλουμε το mutex λόγω του pthread_cond_wait που απαιτεί εξ ορισμού mutex

#define SIZE 1  // το μέγεθος του κυκλικού buffer
#define LOOP 10   // αριθμός επαναλήψεων παραγωγού
#define NPRO 1   // αριθμός παραγωγών
#define NCON 1   // αριθμός καταναλωτών
#define DPRO 100000  // καθυστέρηση παραγωγού
#define DCON 200000  // καθυστέρηση καταναλωτή

typedef struct {
	int buffer[SIZE];     // ο κυκλικός buffer
	// int front, back;      // αριθμοδείκτες προσθήκης και αφαίρεσης 
	// int counter;          // μετρητής πλήθους στοιχείων
	int full;
	pthread_mutex_t *mut; // mutex για τον αμοιβαίο αποκλεισμό 
	pthread_cond_t *condFull, *condEmpty; // μεταβλητές συνθήκης για συγχρονισμό
} queue;

typedef struct {
	int id;
	queue *fifo;
} thread_data;

// αρχικοποίηση ουράς
queue *queueInit(void) {
	queue *q;

	q = (queue *) malloc(sizeof(queue));
	if (q == NULL)
		return (NULL);

	// αρχικά η ουρά είναι άδεια, αρχικοποιούμε τις μεταβλητές 
	// q->counter = 0;
	// q->front = 0;
	// q->back = SIZE - 1;
	q->full = 0;
	q->mut = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(q->mut, NULL);
	q->condFull = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(q->condFull, NULL);
	q->condEmpty = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(q->condEmpty, NULL);

	return (q);
}

// διαγραφή της ουράς
void queueDelete(queue *q) {
	pthread_mutex_destroy(q->mut);
	free(q->mut);
	pthread_cond_destroy(q->condFull);
	free(q->condFull);
	pthread_cond_destroy(q->condEmpty);
	free(q->condEmpty);
	free(q);
}

// προσθήκη στοιχείου στην ουρά
void queueAdd(queue *q, int in) {
	// παίρνουμε το κλειδί
	pthread_mutex_lock(q->mut);

	// ελέγχουμε αν ο buffer είναι γεμάτος
	// while (q->counter == SIZE) {
		// αν είναι γεμάτος περιμένουμε στην συνθήκη
		while (q->full == 1) {
		printf("Buffer full \n");
		pthread_cond_wait(q->condFull, q->mut);
	 }
	// αλιώς προσθέτουμε κανονικά το στοιχείο 
	// q->back = (q->back + 1) % SIZE;
	// q->buffer[q->back] = in;
	// q->counter++;
	q->buffer[0] = in;
	q->full = 1;

	// αν η ουρά ήταν άδεια ξυπνάμε τυχόν νήματα που περιμένουν και αφήνουμε το κλειδί 
	// if (q->counter == 1)
	pthread_cond_signal(q->condEmpty);
	pthread_mutex_unlock(q->mut);

	return;
}

// αφαίρεση στοιχείου από την ουρά
void queueDel(queue *q, int *out) {

	// παίρνουμε το κλειδί
	pthread_mutex_lock(q->mut);

	// ελέγχουμε ότι ο buffer είναι άδειος
	// while (q->counter == 0) {
		// αν είναι άδειος περιμένουμε στην συνθήκη
		while (q->full == 0) {
		printf("Buffer empty \n");
		pthread_cond_wait(q->condEmpty, q->mut);
	}

	// αλιώς αφαιρούμε κανονικά το στοιχείο 
	// *out = q->buffer[q->front];
	// q->front = (q->front + 1) % SIZE;
	*out = q->buffer[0];
    q->full = 0;
	// q->counter--;

	// αν η ουρά ήταν γεμάτη ξυπνάμε τυχόν νήματα που περιμένουν και αφήνουμε το κλειδί
	// if (q->counter == SIZE-1) 
	pthread_cond_signal(q->condFull);
	pthread_mutex_unlock(q->mut);

	return;
}

// το νήμα του παραγωγού (βάζουμε καθυστέρηση για δοκιμή)
void *producer(void *threadarg) {
	int i, myid;
	queue *fifo;
	thread_data *my_data;
	my_data = (thread_data *) threadarg;
	myid = my_data->id;
	fifo = (queue *) my_data->fifo;

	for (i = 0; i < LOOP; i++) {
		printf("Producer %d item %d \n", myid, i);
		queueAdd(fifo, i);
		usleep(DPRO);
	}
	return (NULL);
}

// το νήμα του καταναλωτή (βάζουμε καθυστέρηση για δοκιμή)
void *consumer(void *threadarg) {
	int i, myid;
	queue *fifo;
	thread_data *my_data;
	my_data = (thread_data *) threadarg;
	myid = my_data->id;
	fifo = (queue *) my_data->fifo;

	for (;;) {
		queueDel(fifo, &i);
		printf("Consumer %d item %d \n", myid, i);
		usleep(DCON);
	}
	return (NULL);
}

// η main για να ξεκινήσουμε τα νήματα
int main() {
	queue *fifo;
	pthread_t pro[NPRO], con[NCON];
	thread_data pro_args[NPRO];
	thread_data con_args[NCON];
	int i;

	fifo = queueInit();
	if (fifo == NULL) {
		exit(1);
	}

	for (i = 0; i < NPRO; i++) {
		pro_args[i].id = i;
		pro_args[i].fifo = fifo;
		pthread_create(&pro[i], NULL, producer, &pro_args[i]);
	}
	for (i = 0; i < NCON; i++) {
		con_args[i].id = i;
		con_args[i].fifo = fifo;
		pthread_create(&con[i], NULL, consumer, &con_args[i]);
	}
	for (i = 0; i < NPRO; i++)
		pthread_join(pro[i], NULL);
	for (i = 0; i < NCON; i++)
		pthread_join(con[i], NULL);

	queueDelete(fifo);

	return 0;
}
