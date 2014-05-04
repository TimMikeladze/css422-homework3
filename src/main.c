#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "queue.h"

#define NUMBER_OF_CPU_THREADS 8
#define NUMBER_OF_IO_THREADS 4
#define NUMBER_OF_SUBMISSION_THREADS 4

void createQueues();
void setupThreads();
void cleanupThreads();
void createThreads(void **function, pthread_t threads[], int amount);
Job createRandomJob(int min, int max);
void *cpuThread(void *args);
void *ioThread();
void *submissionThread();
int getTime();
int generateRandom(int min, int max);

pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_conditionvar;
pthread_attr_t attr;

Queue cpuQueue;
Queue ioQueue;
Queue finishedQueue;

pthread_t cpuThreads[NUMBER_OF_CPU_THREADS];
pthread_t ioThreads[NUMBER_OF_IO_THREADS];
pthread_t submissionThreads[NUMBER_OF_SUBMISSION_THREADS];


int main(void) {
	srand(time(NULL));

	createQueues();
	setupThreads();

	createThreads(cpuThread, cpuThreads, 4);

	int i = 0;
	int length = sizeof(cpuThreads) / sizeof(cpuThreads[0]);
	for (i = 0; i < length; i++) {
		pthread_join(cpuThreads[i], NULL);
	}

	printf("All threads complete.\n");

	Job j = createRandomJob(1, 15);
	j.printJob(&j);

	cleanupThreads();

	return EXIT_SUCCESS;
}

void createThreads(void **function, pthread_t threads[], int amount) {
	int i = 0;
	for (i = 0; i < amount; i++) {
		pthread_create(&threads[i], &attr, function, (void *) 1);
	}
}

void *cpuThread(void *args) {
	printf("cpu thread ran\n");
}

void *ioThread() {
	printf("io thread ran\n");
}

void *submissionThread() {
	printf("submission thread ran\n");
}

void createQueues() {
	cpuQueue = createQueue();
	ioQueue = createQueue();
	finishedQueue = createQueue();
}

void setupThreads() {
	pthread_mutex_init(&count_mutex, NULL);
	pthread_cond_init(&count_threshold_conditionvar, NULL);
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
}

void cleanupThreads() {
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&count_mutex);
	pthread_cond_destroy(&count_threshold_conditionvar);
	pthread_exit(NULL);
}

Job createRandomJob(int min, int max) {
	int phases[NUMBER_OF_PHASES] = { generateRandom(min, max), generateRandom(
			min, max) };
	return createJob(phases);
}

int generateRandom(int min, int max) {
	return (rand() % (max + 1 - min)) + min;
}

int getTime() {
	return (int) time(NULL);
}

