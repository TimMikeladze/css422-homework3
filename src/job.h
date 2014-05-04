#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "phase.h"

#define MAX_PHASES 20


typedef struct Job {
	int id;
	Phase phases[MAX_PHASES];
	Phase *currentPhase;
	bool finished;

	void (*printJob)(struct Job*);
} Job;

Job createJob(Phase phases[]);
void printJob(Job *job);
