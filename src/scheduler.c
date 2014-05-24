#include "scheduler.h"

Scheduler createScheduler() {
	static int id = 0;

	Scheduler scheduler;
	scheduler.getID = &getID;
	scheduler.cpuThread = &cpuThread;
	scheduler.getQueue = &getQueue;

	scheduler.id = id;
	scheduler.queue = createQueue();

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_create(&scheduler.thread, &attr, scheduler.cpuThread, NULL);

	pthread_join(scheduler.thread, NULL);
	id++;

	return scheduler;
}

int getID(Scheduler *scheduler) {
	return scheduler->id;
}

void cpuThread(Scheduler *scheduler) {
	puts("cpu thread ran on scheduler");
}

Queue getQueue(Scheduler *scheduler) {
	return scheduler->queue;
}
