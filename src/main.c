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
#define MAX_JOBS_PER_THREAD 40

void createQueues();
void setupThreads();
void waitForThreads(pthread_t threads[]);
void createThreads(void *function, pthread_t threads[], int amount);
Job createRandomJob();
void *cpuThread(int threadID);
void *ioThread(int threadID);
void *submissionThread(int threadID);
void workerThread(int threadID, PhaseType phaseType);
int currentTime();
int generateRandom(int min, int max);

pthread_attr_t attr;

Queue cpuQueue;
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
	workerThread(threadID, CPU_PHASE);
}

void *ioThread(int threadID) {
	workerThread(threadID, IO_PHASE);
}

void workerThread(int threadID, PhaseType phaseType) {
	Queue *currentQueue = phaseType == CPU_PHASE ? &cpuQueue : &ioQueue;
	Queue *destinationQueue = phaseType == CPU_PHASE ? &ioQueue : &cpuQueue;
	char *currentPhaseTitle = phaseType == CPU_PHASE ? "CPU" : "IO";
	char *nextPhaseTitle = phaseType == CPU_PHASE ? "IO" : "CPU";

	while (jobCounter != MAX_JOBS_PER_THREAD * NUMBER_OF_SUBMISSION_THREADS - 1) {
		if (currentQueue->getSize(currentQueue) > 0) {
			Job job = currentQueue->dequeue(currentQueue);
			int duration = job.currentPhase(&job).duration;
			printf("Job %d taken off of %s Queue by %s Thread %d\n", job.id, currentPhaseTitle, currentPhaseTitle, threadID);
			printf("Job %d executing on %s Thread %d for %d seconds\n", job.id, currentPhaseTitle, threadID, duration);
			//job.printJob(&job);
			sleep(duration);
			job.nextPhase(&job);
			if (job.currentPhase(&job).type == FINISHED_PHASE) {
				printf("Job %d put on Finished Queue by %s Thread %d\n", job.id, currentPhaseTitle, threadID);
				finishedQueue.enqueue(&finishedQueue, job);
			} else {
				printf("Job %d put on %s Queue by %s Thread %d\n", job.id, nextPhaseTitle, currentPhaseTitle, threadID);
				destinationQueue->enqueue(destinationQueue, job);
			}
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
			//job.printJob(&job);
			if (job.currentPhase(&job).type == CPU_PHASE) {
				cpuQueue.enqueue(&cpuQueue, job);
				printf("Job %d put on CPU Queue by Submission Thread %d\n", job.id, threadID);
			} else if (job.currentPhase(&job).type == IO_PHASE) {
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
	cpuQueue = createQueue();
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

