#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "queue.h"

#define NUMBER_OF_CPU_THREADS 4
#define NUMBER_OF_IO_THREADS 4
#define NUMBER_OF_SUBMISSION_THREADS 4
#define MIN_NUMBER_OF_PHASES 2
#define MAX_NUMBER_OF_PHASES 2
#define MAX_DURATION 15
#define MIN_CREATION_RATE 3
#define MAX_CREATION_RATE 4


void createQueues();
void setupThreads();
void waitForThreads(pthread_t threads[]);
void cleanupThreads();
void createThreads(void *function, pthread_t threads[], int amount);
Job createRandomJob();
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
	/*
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
	*/

	Job j = createRandomJob();

	j.printJob(&j);
	return EXIT_SUCCESS;
}

void createThreads(void *function, pthread_t threads[], int amount) {
	int i = 0;
	for (i = 0; i < amount; i++) {
		pthread_create(&threads[i], &attr, function, (void *) i);
	}
}

void *cpuThread(void *args) {
	while (true) {
		if (cpuQueue.getSize(&cpuQueue) > 0) {
			Job job = cpuQueue.dequeue(&cpuQueue);

			if (job.currentPhase->duration > 0) {
				printf("CPU Thread %d running job %d for %d seconds\n", args, job.id, job.currentPhase->duration);
				sleep(job.currentPhase->duration);

				job.currentPhase++;

				if (job.currentPhase->type == CPU_PHASE) {
					printf("Put job %d on CPU Queue by CPU Thread %d\n", job.id, args);
					cpuQueue.enqueue(&cpuQueue, job);
				} else if (job.currentPhase->type == IO_PHASE) {
					printf("Put job %d on IO Queue by CPU Thread %d\n", job.id, args);
					ioQueue.enqueue(&ioQueue, job);
				} else {
					printf("Put job %d on Finished Queue by CPU Thread %d\n", job.id, args);
					finishedQueue.enqueue(&finishedQueue, job);
				}
			} else {
				printf("Put job %d on Finished Queue by CPU Thread %d\n", job.id, args);
				finishedQueue.enqueue(&finishedQueue, job);
			}
		}
	}
}

void *ioThread(void *args) {
	while (true) {
		if (ioQueue.getSize(&ioQueue) > 0) {
			Job job = ioQueue.dequeue(&ioQueue);

			if (job.currentPhase->duration > 0) {
				printf("IO Thread %d running job %d for %d seconds\n", args, job.id, job.currentPhase->duration);
				sleep(job.currentPhase->duration);

				job.currentPhase++;

				if (job.currentPhase->type == CPU_PHASE) {
					printf("Put job %d on CPU Queue by IO Thread %d\n", job.id, args);
					ioQueue.enqueue(&ioQueue, job);
				} else if (job.currentPhase->type == IO_PHASE) {
					printf("Put job %d on IO Queue by IO Thread %d\n", job.id, args);
					ioQueue.enqueue(&ioQueue, job);
				} else {
					printf("Put job %d on Finished Queue by IO Thread %d\n", job.id, args);
					finishedQueue.enqueue(&finishedQueue, job);
				}
			} else {
				printf("Put job %d on Finished Queue by IO Thread %d\n", job.id, args);
				finishedQueue.enqueue(&finishedQueue, job);
			}
		}
	}
}

void *submissionThread(void *args) {
	int rate = generateRandom(MIN_CREATION_RATE, MAX_CREATION_RATE);
	printf("Submission thread %d creating jobs every %d seconds\n", args, rate);

	int t = currentTime();
	while (true) {
		if (currentTime() > t + rate) {
			t = currentTime();
			Job job = createRandomJob(MAX_DURATION);
			if (job.currentPhase->type == CPU_PHASE) {
				printf("Put job %d on CPU Queue by Submission Thread\n", job.id);
				cpuQueue.enqueue(&cpuQueue, job);
			} else if (job.currentPhase->type == IO_PHASE) {
				printf("Put job %d on IO Queue by Submission Thread\n", job.id);
				ioQueue.enqueue(&ioQueue, job);
			}
		} else {
			if (finishedQueue.getSize(&finishedQueue) > 0) {
				Job j = finishedQueue.dequeue(&finishedQueue);
				printf("Job %d taken off of Finished Queue\n", j.id);
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

	return createJob(phases, numberOfPhases);
}

int generateRandom(int min, int max) {
	return (rand() % (max + 1 - min)) + min;
}

int currentTime() {
	return (int) time(NULL);
}

