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
	int quantum;

	Queue *ioQueue;
	Queue *finishedQueue;

	void (*worker)(struct Scheduler*);

} Scheduler;

Scheduler createScheduler(int quantum, Queue *ioQueue, Queue *finishedQueue);

void *worker(void *arguments);

#endif
