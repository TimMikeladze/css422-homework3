#include "job.h"

Job createJob(Phase phases[]) {
	static int id = 0;

	Job j;

	j.id = id;
	memcpy(j.phases, phases, sizeof(j.phases));
	j.currentPhase = &j.phases[0];
	j.finished = false;
	j.printJob = &printJob;

	id++;
	return j;
}


void printJob(Job *job) {
	//printf("Job: %d, Phase: %d, CPU Duration: %d, IO Duration: %d, Finished %d", job->id, job->phase, job->durations[0], job->durations[1], job->finished);
}
