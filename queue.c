#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "queue.h"

#define TRUE 1
#define FALSE 0

int queue_empty(struct queue *q);
int queue_full(struct queue *q);

int queue_init(struct queue **q, size_t qsize) {
	struct queue *p;
	size_t s = 0;
	s = sizeof(struct queue) + sizeof(void *)*qsize;
	//printf("Queue allocation size: %d\n",s);
	p = (struct queue *) malloc(s);
	if (p != NULL) {
		*q = p;
		p->max_size = qsize;
		p->head = 0;
		p->tail = 0;

		pthread_mutex_init(&p->m,NULL);
		pthread_cond_init(&p->c,NULL);
		
		return 0;
	} else {
		return -1;
	}
}
int queue_free(struct queue *q) {
	pthread_mutex_destroy(&q->m);
	pthread_cond_destroy(&q->c);
	free(q);
	return 0;
}
int queue_enqueue(struct queue *q, void *ptr) {
	int res;
	pthread_t thr;
	pthread_mutex_lock(&q->m);
	
	thr = pthread_self(); thr -= 1;
	
	while(queue_full(q))
		pthread_cond_wait(&q->c,&q->m);
		
	if(!queue_full(q)) {
		q->tail = q->tail % q->max_size;
		//printf("{%d} v: %d, h: %d, t: %d\n",thr,ptr,q->head,q->tail);
		q->array[q->tail] = ptr;
		q->tail = q->tail + 1;
		
		res = 0;
		pthread_cond_signal(&q->c);
	} else {
		res = -1;
	}
	pthread_mutex_unlock(&q->m);
	return res;
}
void *queue_dequeue(struct queue *q) {
	void *ptr;
	pthread_t thr;
	thr = pthread_self(); thr -=1;

	pthread_mutex_lock(&q->m);
	
	while(queue_empty(q)) 
		pthread_cond_wait(&q->c,&q->m);

	if (!queue_empty(q)) {
		q->head = q->head % q->max_size;
		//printf("{%d} v: %d, h: %d, t: %d\n",thr,q->head,q->tail);
		ptr = q->array[q->head];
		q->array[q->head] = NULL;
		q->head = q->head + 1;

		pthread_cond_signal(&q->c);
	} else {
		ptr = NULL;
	}
	pthread_mutex_unlock(&q->m);

	return ptr;
}
int queue_empty(struct queue *q) {
	if ((q->head == q->tail) || (q->tail == 0 && q->head == q->max_size))
		return TRUE;
	else
		return FALSE;
}
int queue_full(struct queue *q) {
	if((((q->tail % q->max_size)+1) == q->head) || (q->head == 0 && q->tail == q->max_size)) 
		return TRUE;
	else
		return FALSE;	
}

void queue_print(struct queue *q) {
	printf("%4c: %18d\n",'H',q->head);
	printf("%4c: %18d\n",'T',q->tail);
	for (int i=0;i<q->max_size;i++)
		printf("%4d: %#p\n",i,q->array[i]);
}
