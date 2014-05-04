#include "job.h"

Job createJob(Phase phases[], int numberOfPhases) {
	static int id = 0;

	Job j;

	j.id = id;
	memcpy(j.phases, phases, sizeof(j.phases));
	j.currentPhase = &j.phases[0];
	j.finished = false;
	j.phaseIndex = 0;
	j.numberOfPhases = numberOfPhases;

	j.printJob = &printJob;

	id++;
	return j;
}

void printJob(Job *job) {
	printf("Job: %d, Phases: %d, Phase Index: %d\n", job->id, job->numberOfPhases, job->phaseIndex);
	int i = 0;
	for (i = 0; i < job->numberOfPhases; i++) {
		char *highlight = job->phaseIndex == i ? "*" : " ";
		char *type = job->phases[i].type == 0 ? "CPU" : "IO";
		printf("%s Phase: %d, Type: %s, Duration: %d\n", highlight, i, type, job->phases[i].duration);
	}
}
