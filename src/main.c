#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "queue.h"

#define NUMBER_OF_CPU_THREADS 8
#define NUMBER_OF_IO_THREADS 4
#define NUMBER_OF_SUBMISSION_THREADS 4
#define MIN_NUMBER_OF_PHASES 2
#define MAX_NUMBER_OF_PHASES 5
#define MAX_DURATION 7
#define MIN_CREATION_RATE 3
#define MAX_CREATION_RATE 4
#define MAX_JOBS_PER_THREAD 20
#define QUANTUM 4

void createQueues();
void setupThreads();
void waitForThreads(pthread_t threads[]);
void createThreads(void *function, pthread_t threads[], int amount);
Job createRandomJob();
void *cpuThread(int threadID);
void *ioThread(int threadID);
void *submissionThread(int threadID);
int currentTime();
int generateRandom(int min, int max);

pthread_attr_t attr;

Queue cpuQueues[NUMBER_OF_CPU_THREADS];
Queue ioQueue;
Queue finishedQueue;

int jobCounter;

pthread_t cpuThreads[NUMBER_OF_CPU_THREADS];
pthread_t ioThreads[NUMBER_OF_IO_THREADS];
pthread_t submissionThreads[NUMBER_OF_SUBMISSION_THREADS];

int main(void) {
	srand(time(NULL));

	createQueues();

	setupThreads();

	createThreads(cpuThread, cpuThreads, NUMBER_OF_CPU_THREADS);
	createThreads(ioThread, ioThreads, NUMBER_OF_IO_THREADS);
	createThreads(submissionThread, submissionThreads, NUMBER_OF_SUBMISSION_THREADS);

	waitForThreads(cpuThreads);
	waitForThreads(ioThreads);
	waitForThreads(submissionThreads);

	printf("All threads complete.\n");

	return EXIT_SUCCESS;
}

void createThreads(void *function, pthread_t threads[], int amount) {
	int i = 0;
	for (i = 0; i < amount; i++) {
		pthread_create(&threads[i], &attr, function, i);
	}
}

void *cpuThread(int threadID) {
	Queue *queue = &cpuQueues[threadID];
	while (jobCounter != MAX_JOBS_PER_THREAD * NUMBER_OF_SUBMISSION_THREADS - 1) {
		if (queue->getSize(queue) > 0) {
			Job job = queue->dequeue(queue);
			// @formatter:off
			int sleepDuration = job.currentPhase(&job)->duration > QUANTUM ? QUANTUM : job.currentPhase(&job)->duration;
			// @formatter:on
			printf("Executing Job %d on Schedule %d for %d seconds\n", job.id, threadID, sleepDuration);
			sleep(sleepDuration);
			Phase *phase = job.currentPhase(&job);
			phase->duration -= sleepDuration;

			if (phase->duration <= 0) {
				job.nextPhase(&job);
				if (job.currentPhase(&job)->type == IO_PHASE) {
					printf("Job %d put on IO Queue by Scheduler %d\n", job.id, threadID);
					ioQueue.enqueue(&ioQueue, job);
				} else {
					printf("Job %d put on Finished Queue by Scheduler %d\n", job.id, threadID);
					finishedQueue.enqueue(&finishedQueue, job);
				}
			} else {
				printf("Job %d put back on queue by Scheduler %d, remaining job duration %d\n", job.id, threadID, phase->duration);
				queue->enqueue(queue, job);
			}
		}
	}

}

void *ioThread(int threadID) {
	while (jobCounter != MAX_JOBS_PER_THREAD * NUMBER_OF_SUBMISSION_THREADS - 1) {
		if (ioQueue.getSize(&ioQueue) > 0) {
			Job job = ioQueue.dequeue(&ioQueue);
			printf("Job %d taken off of IO Queue by IO Thread %d\n", job.id, threadID);
			printf("Job %d executing on IO Thread %d\n", job.id, threadID);
			sleep(job.currentPhase(&job)->duration);
			job.nextPhase(&job);
			if (job.currentPhase(&job)->type == CPU_PHASE) {
				cpuQueues[job.scheduleID].enqueue(&cpuQueues[job.scheduleID], job);
				printf("Job %d put on Scheduler %d by IO Thread %d\n", job.id, job.scheduleID, threadID);
			} else {
				finishedQueue.enqueue(&finishedQueue, job);
				printf("Job %d put on Finished Queue by IO Thread %d\n", job.id, threadID);
			}
		}
		if (jobCounter == MAX_JOBS_PER_THREAD * NUMBER_OF_SUBMISSION_THREADS - 1) {
			break;
		}
	}
}

void *submissionThread(int threadID) {
	int rate = generateRandom(MIN_CREATION_RATE, MAX_CREATION_RATE);
	printf("Submission thread %d creating jobs every %d seconds\n", threadID, rate);

	int t = currentTime();
	int i = 0;
	while (jobCounter != MAX_JOBS_PER_THREAD * NUMBER_OF_SUBMISSION_THREADS - 1) {
		if (currentTime() > t + rate && i < MAX_JOBS_PER_THREAD) {
			t = currentTime();
			Job job = createRandomJob();
			if (job.currentPhase(&job)->type == CPU_PHASE) {
				cpuQueues[job.scheduleID].enqueue(&cpuQueues[job.scheduleID], job);
				printf("Job %d put on Scheduler %d by Submission Thread %d\n", job.id, job.scheduleID, threadID);
			} else if (job.currentPhase(&job)->type == IO_PHASE) {
				ioQueue.enqueue(&ioQueue, job);
				printf("Job %d put on IO Queue by Submission Thread %d\n", job.id, threadID);
			}
			i++;
		} else {
			if (finishedQueue.getSize(&finishedQueue) > 0) {
				Job job = finishedQueue.dequeue(&finishedQueue);
				printf("Job %d taken off of Finished Queue by Submission Thread %d\n", job.id, threadID);
				job.printJob(&job);
				jobCounter++;
				printf("Jobs processed: %d\n", jobCounter + 1);
				if (jobCounter == MAX_JOBS_PER_THREAD * NUMBER_OF_SUBMISSION_THREADS - 1) {
					break;
				}
			}
		}
	}
}

void createQueues() {
	int i = 0;
	for (i = 0; i < NUMBER_OF_CPU_THREADS; i++) {
		cpuQueues[i] = createQueue();
	}
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
	static int id = 0;
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

	Job job = createJob(id % NUMBER_OF_CPU_THREADS, phases, numberOfPhases);
	printf("Created Job %d\n", job.id);
	job.printJob(&job);
	id++;
	return job;
}

int generateRandom(int min, int max) {
	return (rand() % (max + 1 - min)) + min;
}

int currentTime() {
	return (int) time(NULL);
}

