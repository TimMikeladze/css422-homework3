#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NUMBER_OF_PHASES 2

typedef enum {
	CPU, IO, FINISHED
} Phase;

typedef struct Job {
	int id;
	int durations[NUMBER_OF_PHASES];
	Phase phase;

	bool finished;

	void (*printJob)(struct Job*);
} Job;

Job createJob(int phases[]);
void printJob(Job *job);
