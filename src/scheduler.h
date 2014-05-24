#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "queue.h"

typedef struct Scheduler {
	int id;
	pthread_t thread;
	Queue queue;

	void (*cpuThread)(struct Scheduler*);
	int (*getID)(struct Scheduler*);
	Queue (*getQueue)(struct Scheduler*);

} Scheduler;

Scheduler createScheduler();

void cpuThread(Scheduler *scheduler);
int getID(Scheduler *scheduler);
Queue getQueue(Scheduler *scheduler);

#endif
