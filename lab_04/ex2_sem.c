#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 10
#define LOOP 100
#define NPRO 1
#define NCON 1
#define DPRO 100000
#define DCON 200000

typedef struct {
    int buffer[SIZE];
    int front, back;
    sem_t *mut;
    sem_t *toPut;
    sem_t *toGet;
} ArrayBlockingQueue;

typedef struct {
    int id;
    ArrayBlockingQueue *queue;
} thread_data;

ArrayBlockingQueue *queueInit(void) {

    ArrayBlockingQueue *q = malloc(sizeof(ArrayBlockingQueue));
    if (q == NULL) return NULL;

    q->front = 0;
    q->back = SIZE - 1;

    q->mut = malloc(sizeof(sem_t));
    sem_init(q->mut, 0, 1);

    q->toPut = malloc(sizeof(sem_t));
    sem_init(q->toPut, 0, SIZE);

    q->toGet = malloc(sizeof(sem_t));
    sem_init(q->toGet, 0, 0);

    return q;
}

void queueDelete(ArrayBlockingQueue *q) {

    sem_destroy(q->mut);
    free(q->mut);

    sem_destroy(q->toPut);
    free(q->toPut);

    sem_destroy(q->toGet);
    free(q->toGet);

    free(q);
}

void queueAdd(ArrayBlockingQueue *q, int in) {

    sem_wait(q->toPut);

    sem_wait(q->mut);

    q->back = (q->back + 1) % SIZE;
    q->buffer[q->back] = in;

    sem_post(q->mut);

    sem_post(q->toGet);
}

void queueDel(ArrayBlockingQueue *q, int *out) {

    sem_wait(q->toGet);

    sem_wait(q->mut);

    *out = q->buffer[q->front];
    q->front = (q->front + 1) % SIZE;

    sem_post(q->mut);

    sem_post(q->toPut);
}

void *producer(void *threadarg) {

    int i, myid;
    ArrayBlockingQueue *fifo;
    thread_data *my_data;

    my_data = (thread_data *) threadarg;
    myid = my_data->id;
    fifo = my_data->queue;

    for (i = 0; i < LOOP; i++) {

        printf("Producer %d item %d\n", myid, i);
        queueAdd(fifo, i);

        usleep(DPRO);
    }

    return NULL;
}

void *consumer(void *threadarg) {

    int i, myid;
    ArrayBlockingQueue *fifo;
    thread_data *my_data;

    my_data = (thread_data *) threadarg;
    myid = my_data->id;
    fifo = my_data->queue;

    for (;;) {

        queueDel(fifo, &i);

        printf("Consumer %d item %d\n", myid, i);

        usleep(DCON);
    }

    return NULL;
}

int main() {

    ArrayBlockingQueue *queue;

    pthread_t pro[NPRO], con[NCON];
    thread_data pro_args[NPRO], con_args[NCON];

    queue = queueInit();
    if (queue == NULL) exit(1);

    for (int i = 0; i < NPRO; i++) {
        pro_args[i].id = i;
        pro_args[i].queue = queue;
        pthread_create(&pro[i], NULL, producer, &pro_args[i]);
    }

    for (int i = 0; i < NCON; i++) {
        con_args[i].id = i;
        con_args[i].queue = queue;
        pthread_create(&con[i], NULL, consumer, &con_args[i]);
    }

    for (int i = 0; i < NPRO; i++)
        pthread_join(pro[i], NULL);

    for (int i = 0; i < NCON; i++)
        pthread_join(con[i], NULL);

    queueDelete(queue);

    return 0;
}