#include "scheduler.h"

Scheduler createScheduler(int quantum, Queue *ioQueue, Queue *finishedQueue) {
	static int id = 0;
	Scheduler scheduler;
	scheduler.worker = &worker;

	scheduler.id = id;
	scheduler.queue = createQueue();
	scheduler.quantum = quantum;

	scheduler.ioQueue = ioQueue;
	scheduler.finishedQueue = finishedQueue;


	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&scheduler.thread, &attr, scheduler.worker, NULL);
	puts("Test 1");
	pthread_join(scheduler.thread, NULL);
	puts("Test 2");
	id++;

	return scheduler;
}

void worker(Scheduler *scheduler) {
	while (true) {
		Job job = scheduler->queue.dequeue(&scheduler->queue);
		int sleepDuration =
				job.currentPhase(&job).duration > scheduler->quantum ?
						scheduler->quantum : job.currentPhase(&job).duration;
		sleep(sleepDuration);
		Phase phase = job.currentPhase(&job);
		phase.duration -= sleepDuration;
		if (phase.duration <= 0) {
			job.nextPhase(&job);
			if (job.currentPhase(&job).type == IO_PHASE) {
				printf("Job %d put on IO Queue by Scheduler %d\n", job.id, scheduler->id);
				scheduler->ioQueue->enqueue(scheduler->ioQueue, job);
			} else {
				printf("Job %d put on Finished Queue by Scheduler %d\n", job.id, scheduler->id);
				scheduler->finishedQueue->enqueue(scheduler->finishedQueue, job);
			}
		} else {
			printf("Job %d put back on queue by Scheduler %d\n", job.id, scheduler->id);
			scheduler->queue.enqueue(&scheduler->queue, job);
		}
	}
}
