#include "job.h"

void printJob(Job *job) {
	printf("Job: %d, Phase: %d, CPU Duration: %d, IO Duration: %d, Finished %d", job->id, job->phase, job->durations[0], job->durations[1], job->finished);
}

Job createJob(int durations[NUMBER_OF_PHASES]) {
	static int id = 0;

	Job j;
	j.id = id;
	memcpy(j.durations, durations, sizeof(j.durations));
	j.phase = CPU;
	j.finished = false;
	j.printJob = &printJob;

	id++;
	return j;
}
