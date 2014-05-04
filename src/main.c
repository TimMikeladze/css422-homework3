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
void waitForThreads(pthread_t threads[]);
void cleanupThreads();
void createThreads(void *function, pthread_t threads[], int amount);
Job createRandomJob(int min, int max);
void *cpuThread(void *args);
void *ioThread(void *args);
void *submissionThread(void *args);
int currentTime();
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

	createThreads(cpuThread, cpuThreads, NUMBER_OF_CPU_THREADS);
	createThreads(ioThread, ioThreads, NUMBER_OF_IO_THREADS);
	createThreads(submissionThread, submissionThreads,
			NUMBER_OF_SUBMISSION_THREADS);

	waitForThreads(cpuThreads);
	waitForThreads(ioThreads);
	waitForThreads(submissionThreads);

	printf("All threads complete.\n");

	cleanupThreads();

	return EXIT_SUCCESS;
}

void createThreads(void *function, pthread_t threads[], int amount) {
	int i = 0;
	for (i = 0; i < amount; i++) {
		pthread_create(&threads[i], &attr, function, (void *) i);
	}
}

void *cpuThread(void *args) {
	printf("cpu thread ran %d \n", args);

}

void *ioThread(void *args) {
	printf("io thread ran %d \n", args);
}

void *submissionThread(void *args) {
	int rate = generateRandom(3, 4);
	printf("Submission thread %d creating jobs every %d seconds\n", args, rate);

	int t = currentTime();
	while (true) {
		if (currentTime() > t + rate) {
			t = currentTime();
			Job job = createRandomJob(1, 15);
			if (job.currentPhase->type == CPU_PHASE) {
				printf("Put job %d on CPU Queue\n", job.id);
				cpuQueue.enqueue(&cpuQueue, job);
			} else if (job.currentPhase->type == IO_PHASE) {
				printf("Put job %d on IO Queue\n", job.id);
				ioQueue.enqueue(&ioQueue, job);
			}
		} else {
			if (finishedQueue.getSize(&finishedQueue) > 0) {
				Job j = finishedQueue.dequeue(&finishedQueue);
				printf("Job %d taken of Finished Queue", j.id);
			}
		}
	}
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

void waitForThreads(pthread_t threads[]) {
	int i = 0;
	int length = sizeof(threads) / sizeof(threads[0]);
	for (i = 0; i < length; i++) {
		pthread_join(threads[i], NULL);
	}
}

void cleanupThreads() {
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&count_mutex);
	pthread_cond_destroy(&count_threshold_conditionvar);
	pthread_exit(NULL);
}

Job createRandomJob(int min, int max) {
	int numberOfPhases = generateRandom(2, MAX_PHASES);
	Phase phases[numberOfPhases];

	int i = 0;
	for (i = 0; i < numberOfPhases; i++) {
		int duration = generateRandom(1, 15);
		Phase phase;
		phase.duration = duration;
		phase.type = duration % 2 == 0 ? CPU_PHASE : IO_PHASE;
		phases[i] = phase;
	}

	return createJob(phases);
}

int generateRandom(int min, int max) {
	return (rand() % (max + 1 - min)) + min;
}

int currentTime() {
	return (int) time(NULL);
}

