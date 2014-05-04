#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "queue.h"

#define NUMBER_OF_CPU_THREADS 3
#define NUMBER_OF_IO_THREADS 3
#define NUMBER_OF_SUBMISSION_THREADS 3
#define MIN_NUMBER_OF_PHASES 2
#define MAX_NUMBER_OF_PHASES 3
#define MAX_DURATION 15
#define MIN_CREATION_RATE 3
#define MAX_CREATION_RATE 4
#define MAX_JOBS 20

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
	while (true) {
		if (cpuQueue.getSize(&cpuQueue) > 0) {
			Job job = cpuQueue.dequeue(&cpuQueue);
			printf("Job %d taken off of CPU Queue by CPU Thread %d\n", job.id, args);
			job.printJob(&job);
			sleep(job.currentPhase(&job).duration);
			job.nextPhase(&job);
			if (job.currentPhase(&job).type == IO_PHASE) {
				printf("Job %d put on IO Queue by CPU Thread %d\n", job.id, args);
				ioQueue.enqueue(&ioQueue, job);
			} else {
				printf("Job %d put on Finished Queue by CPU Thread %d\n", job.id, args);
				finishedQueue.enqueue(&finishedQueue, job);
			}
		}
	}
}

void *ioThread(void *args) {
	while (true) {
		if (ioQueue.getSize(&ioQueue) > 0) {
			Job job = ioQueue.dequeue(&ioQueue);
			printf("Job %d taken off of IO Queue by IO Thread %d\n", job.id, args);
			job.printJob(&job);
			sleep(job.currentPhase(&job).duration);
			job.nextPhase(&job);
			if (job.currentPhase(&job).type == CPU_PHASE) {
				printf("Job %d put on CPU Queue by IO Thread %d\n", job.id, args);
				cpuQueue.enqueue(&cpuQueue, job);
			} else {
				printf("Job %d put on Finished Queue by IO Thread %d\n", job.id, args);
				finishedQueue.enqueue(&finishedQueue, job);
			}
		}
	}
}

void *submissionThread(void *args) {
	int rate = generateRandom(MIN_CREATION_RATE, MAX_CREATION_RATE);
	printf("Submission thread %d creating jobs every %d seconds\n", args, rate);

	int t = currentTime();
	int i = 0;
	while (true) {
		if (currentTime() > t + rate && i < MAX_JOBS) {
			t = currentTime();
			Job job = createRandomJob();
			job.printJob(&job);
			if (job.currentPhase(&job).type == CPU_PHASE) {
				cpuQueue.enqueue(&cpuQueue, job);
				printf("Job %d put on CPU Queue by Submission Thread %d\n", job.id, args);
			} else if (job.currentPhase(&job).type == IO_PHASE) {
				ioQueue.enqueue(&ioQueue, job);
				printf("Job %d put on IO Queue by Submission Thread %d\n", job.id, args);
			}
			i++;
		} else {
			if (finishedQueue.getSize(&finishedQueue) > 0) {
				Job job = finishedQueue.dequeue(&finishedQueue);
				printf("Job %d taken off of Finished Queue by Submission Thread %d\n", job.id, args);
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

