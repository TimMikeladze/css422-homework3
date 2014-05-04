#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "queue.h"

void *cpuThread(void *args);
void *ioThread(void *args);
void *finishedThread(void *args);
int getTime();

pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_conditionvar;

int main(void) {

	pthread_t threads[3];
	pthread_attr_t attr;

	pthread_mutex_init(&count_mutex, NULL);
	pthread_cond_init(&count_threshold_conditionvar, NULL);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	pthread_create(&threads[0], &attr, cpuThread, (void *) 1);
	pthread_create(&threads[1], &attr, ioThread, (void *) 2);
	pthread_create(&threads[2], &attr, finishedThread, (void *) 3);

	int i = 0;
	int length = sizeof(threads) / sizeof(threads[0]);
	for (i = 0; i < length; i++)
		pthread_join(threads[i], NULL);

	printf("All threads complete.\n");

	Queue queue = createQueue();

	queue.enqueue(&queue, createJob(1032));
	queue.enqueue(&queue, createJob(222));

	queue.printQueue(&queue);

	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&count_mutex);
	pthread_cond_destroy(&count_threshold_conditionvar);
	pthread_exit(NULL);

	return EXIT_SUCCESS;
}

int getTime() {
	return (int) time(NULL);
}

void *cpuThread(void *args) {
	printf("cpu thread ran\n");
}

void *ioThread(void *args) {
	printf("io thread ran\n");
}

void *finishedThread(void *args) {
	printf("finished thread ran\n");
}

