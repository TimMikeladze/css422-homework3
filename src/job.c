#include "job.h"

Job createJob(Phase phases[], int numberOfPhases) {
	static int id = 0;

	Job job;

	job.id = id;
	memcpy(job.phases, phases, sizeof(job.phases));
	job.finished = false;
	job.phaseIndex = 0;
	job.numberOfPhases = numberOfPhases;

	job.printJob = &printJob;
	job.nextPhase = &nextPhase;
	job.currentPhase = &currentPhase;

	id++;
	return job;
}

void printJob(Job *job) {
	char *completed = job->finished ? "Yes" : "No";
	int remaining = job->numberOfPhases - job->phaseIndex - 1;
	printf("Job: %d, Phases: %d, Phase Index: %d, Remaining: %d, Completed: %s\n", job->id, job->numberOfPhases, job->phaseIndex, remaining, completed);
	int i = 0;
	for (i = 0; i < job->numberOfPhases; i++) {
		char *highlight = job->phaseIndex == i ? "*" : " ";
		char *type = job->phases[i].type == 0 ? "CPU" : "IO";
		printf("%s Phase: %d, Type: %s, Duration: %d\n", highlight, i, type, job->phases[i].duration);
	}
}

void nextPhase(Job *job) {
	job->phaseIndex++;
	job->finished = job->phaseIndex == job->numberOfPhases ? true : false;
}

Phase currentPhase(Job *job) {
	Phase finishedPhase;
	finishedPhase.duration = 0;
	finishedPhase.type = FINISHED_PHASE;
	return job->finished == true ? finishedPhase : job->phases[job->phaseIndex];
}
