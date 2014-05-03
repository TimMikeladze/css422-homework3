#include <stdio.h>
#include <stdlib.h>

typedef struct Job {
	int id;
	int phases;
	int currentPhase;
	int isComplete;
	void (*printJob)(struct Job*);
} Job;

Job createJob(int id);
void printJob(Job *job);
