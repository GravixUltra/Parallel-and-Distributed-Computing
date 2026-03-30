#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 10   
#define LOOP 100 
#define NPRO 1    
#define NCON 1   

typedef struct {
    int buffer[SIZE];
    int front, back, counter;
    pthread_mutex_t *mut;
    pthread_cond_t *condFull, *condEmpty;
} ArrayBlockingQueue;

typedef struct {
    int id;
    ArrayBlockingQueue *queue;
} thread_data;

ArrayBlockingQueue *queueInit(void) {
    ArrayBlockingQueue *q = (ArrayBlockingQueue *) malloc(sizeof(ArrayBlockingQueue));
    if (q == NULL) return NULL;

    q->counter = 0;
    q->front   = 0;
    q->back    = SIZE - 1;

    q->mut = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(q->mut, NULL);

    q->condFull = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(q->condFull, NULL);

    q->condEmpty = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(q->condEmpty, NULL);

    return q;
}

void queueDelete(ArrayBlockingQueue *q) {
    pthread_mutex_destroy(q->mut);    free(q->mut);
    pthread_cond_destroy(q->condFull);  free(q->condFull);
    pthread_cond_destroy(q->condEmpty); free(q->condEmpty);
    free(q);
}

void queueAdd(ArrayBlockingQueue *q, int in) {
    pthread_mutex_lock(q->mut);

    while (q->counter == SIZE) {
        printf("Buffer full\n");
        pthread_cond_wait(q->condFull, q->mut);
    }

    q->back = (q->back + 1) % SIZE;
    q->buffer[q->back] = in;
    q->counter++;

    if (q->counter == 1) pthread_cond_signal(q->condEmpty);
    pthread_mutex_unlock(q->mut);
}

void queueDel(ArrayBlockingQueue *q, int *out) {
    pthread_mutex_lock(q->mut);

    while (q->counter == 0) {
        printf("Buffer empty\n");
        pthread_cond_wait(q->condEmpty, q->mut);
    }

    *out = q->buffer[q->front];
    q->front = (q->front + 1) % SIZE;
    q->counter--;

    if (q->counter == SIZE - 1) pthread_cond_signal(q->condFull);

    pthread_mutex_unlock(q->mut);
}

void *producer(void *threadarg) {
    thread_data *my_data = (thread_data *) threadarg;
    ArrayBlockingQueue *queue = my_data->queue;

    for (int i = 0; i < LOOP; i++) {
        usleep(i*1000);
        queueAdd(queue, i); 
        printf("Produced %d\n", i);
    }
    return NULL;
}

void *consumer(void *threadarg) {
    int i, myid;
    ArrayBlockingQueue *queue;
    thread_data *my_data;

    my_data = (thread_data *) threadarg;
    myid = my_data->id;
    queue = (ArrayBlockingQueue *) my_data->queue;

    for (;;) {
        queueDel(queue, &i);
        printf("Consumer %d item %d\n", myid, i);
        usleep(10000);
    }
    return NULL;
}

int main() {
    ArrayBlockingQueue *queue = queueInit();
    if (queue == NULL) exit(1);

    pthread_t pro[NPRO], con[NCON];
    thread_data pro_args[NPRO], con_args[NCON];

    for (int i = 0; i < NPRO; i++) {
        pro_args[i].id    = i;
        pro_args[i].queue = queue;
        pthread_create(&pro[i], NULL, producer, &pro_args[i]);
    }
    for (int i = 0; i < NCON; i++) {
        con_args[i].id    = i;
        con_args[i].queue = queue;
        pthread_create(&con[i], NULL, consumer, &con_args[i]);
    }

    for (int i = 0; i < NPRO; i++) pthread_join(pro[i], NULL);
    for (int i = 0; i < NCON; i++) pthread_join(con[i], NULL);

    queueDelete(queue);
    return 0;
}