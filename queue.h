#include <pthread.h>
struct queue {
	pthread_mutex_t m;
	pthread_cond_t c;
	unsigned int max_size;
	volatile unsigned int head;
	volatile unsigned int tail;
	void *array[];
};

int queue_init(struct queue **q, size_t qsize);
int queue_free(struct queue *q);
int queue_enqueue(struct queue *q, void *ptr);
void *queue_dequeue(struct queue *q);
void queue_print(struct queue *q);

