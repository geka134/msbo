#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <pthread.h>
#include <signal.h>
#include "queue.h"

#define THREADS (1024*32)
#define QSIZE 1024
#define MESSAGES (1024*1024*2)

/* run this program using the console pauser or add your own getch, system("pause") or input loop */
void *thr_func_enq(void *ptr);
void *thr_func_deq(void *ptr);
void term_handler(int i);

struct queue *q1;

int main(int argc, char *argv[]) {
	int res;
	pthread_t thr[THREADS];
	unsigned long long arr[THREADS];
	unsigned long long total = 0;

	for (int i=0; i<THREADS;i++) arr[i] = 0;
	
	if(signal(SIGINT,term_handler) ==  SIG_ERR)
		printf("Cann't catch signals\n"),exit(1);
	
	res = queue_init(&q1,QSIZE);
	
	for(int i=0; i<THREADS; i++)
		pthread_create(&thr[i],NULL,(i<THREADS/2) ? thr_func_enq:thr_func_deq,NULL);

	printf("threads created\n");
	for(int i=0; i<THREADS; i++)
		pthread_join(thr[i],(void **)&arr[i]);
	printf("threads ended\n");
	
	for(int i=0;i<THREADS;i++) {
		printf("[%d] = %llu\n",i,arr[i]);
		total += arr[i];
	}
	printf("total = %llu | %llu \n", total,  (unsigned long long )((1 + MESSAGES)/2.0 * MESSAGES)); /* total == (a1 + an)/2 * n*/
	
	queue_free(q1);
	return 0;
} 
void *thr_func_enq(void *ptr) {
	pthread_t thr;
	thr = pthread_self(); thr -= 1;
		
	for(int i=1; i<=MESSAGES;i++) {
		//printf("{%d} enq starting\n",thr);
		if (i % (THREADS/2) != (int) thr) {
		//printf("contunue: %d, %d %% %d != %d (%d)\n",i,i,THREADS/2,(int)thr,i % (THREADS/2));
		continue;
		}
		printf("[%d] enqueue value is %d\n",thr,i);
		if (queue_enqueue(q1,(void *)i) == -1)
			printf("enqueue error\n"),exit(1);
		//printf("{%d} enq ended\n",thr);
	}
	printf("[%d] enq counter is over\n",thr);
	return NULL;
}
void *thr_func_deq(void *ptr) {
	pthread_t thr;
	void *value;
	unsigned int messages = 0;
	unsigned long long count = 0;
	thr = pthread_self(); thr -= 1;
	messages = MESSAGES/(THREADS - (THREADS/2));
	printf("[%d] messages = %d deq thr = %d\n",thr,messages, (THREADS - (THREADS/2)));
	for(int i=1; i<=messages;i++) {
		//printf("[%d] i = %d \n",thr,i);
		value = queue_dequeue(q1);
		count += (unsigned long long) value;
		printf("[%d] dequeue value is %d, i = %d\n",thr,value,i);
		//printf("|%d| deq ended\n",thr);
	}
	printf("[%d] deq counter is over\n",thr);
		
	return (void *)count;
}
void term_handler(int i) {
	queue_print(q1);
}

