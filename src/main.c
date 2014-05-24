#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "queue.h"
#include "scheduler.h"

#define NUMBER_OF_CPU_THREADS 8
#define NUMBER_OF_IO_THREADS 4
#define NUMBER_OF_SUBMISSION_THREADS 4
#define MIN_NUMBER_OF_PHASES 2
#define MAX_NUMBER_OF_PHASES 5
#define MAX_DURATION 7
#define MIN_CREATION_RATE 3
#define MAX_CREATION_RATE 4
#define MAX_JOBS_PER_THREAD 40

void createQueues();
void setupThreads();
void waitForThreads(pthread_t threads[]);
void createThreads(void *function, pthread_t threads[], int amount);
Job createRandomJob();
void *ioThread(int threadID);
void *submissionThread(int threadID);
int currentTime();
int generateRandom(int min, int max);

pthread_attr_t attr;

Queue ioQueue;
Queue finishedQueue;

int jobCounter;

pthread_t ioThreads[NUMBER_OF_IO_THREADS];
pthread_t submissionThreads[NUMBER_OF_SUBMISSION_THREADS];

int main(void) {
	srand(time(NULL));

	/*
	createQueues();

	setupThreads();

	createThreads(ioThread, ioThreads, NUMBER_OF_IO_THREADS);
	createThreads(submissionThread, submissionThreads, NUMBER_OF_SUBMISSION_THREADS);

	waitForThreads(ioThreads);
	waitForThreads(submissionThreads);
	*/

	Scheduler s = createScheduler();

	printf("All threads complete.\n");

	return EXIT_SUCCESS;
}

void createThreads(void *function, pthread_t threads[], int amount) {
	int i = 0;
	for (i = 0; i < amount; i++) {
		pthread_create(&threads[i], &attr, function, i);
	}
}


void *ioThread(int threadID) {
}


void *submissionThread(int threadID) {
}

void createQueues() {
	ioQueue = createQueue();
	finishedQueue = createQueue();
}

void setupThreads() {
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

Job createRandomJob() {
	int numberOfPhases = generateRandom(MIN_NUMBER_OF_PHASES, MAX_NUMBER_OF_PHASES);
	Phase phases[numberOfPhases];
	int i = 0;
	PhaseType type;

	for (i = 0; i < numberOfPhases; i++) {
		int duration = generateRandom(1, MAX_DURATION);
		Phase phase;
		phase.duration = duration;
		if (type == CPU_PHASE) {
			type = IO_PHASE;
		} else if (type == IO_PHASE) {
			type = CPU_PHASE;
		} else {
			type = duration % 2 == 0 ? CPU_PHASE : IO_PHASE;
		}
		phase.type = type;
		phases[i] = phase;
	}

	Job job = createJob(phases, numberOfPhases);
	printf("Created Job %d\n", job.id);
	job.printJob(&job);

	return job;
}

int generateRandom(int min, int max) {
	return (rand() % (max + 1 - min)) + min;
}

int currentTime() {
	return (int) time(NULL);
}

