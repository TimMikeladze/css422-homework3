#include "job.h"

void printJob(Job *job) {
	printf("ID of job: %d\n", job->id);
}

Job createJob(int id) {
	Job j;
	j.id = id;
	j.phases = 0;
	j.currentPhase = 0;
	j.isComplete = 0;
	j.printJob = &printJob;
	return j;

}
