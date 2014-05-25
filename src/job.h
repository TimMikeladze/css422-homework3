#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "phase.h"

#define MAX_PHASES 20


typedef struct Job {
	int id;
	Phase phases[MAX_PHASES];
	bool finished;
	int phaseIndex;
	int numberOfPhases;
	int scheduleID;

	void (*printJob)(struct Job*);
	void (*nextPhase)(struct Job*);
	Phase *(*currentPhase)(struct Job*);
} Job;

Job createJob(int scheduleID, Phase phases[], int numberOfPhases);
void printJob(Job *job);
void nextPhase(Job *job);
Phase *currentPhase(Job *job);
