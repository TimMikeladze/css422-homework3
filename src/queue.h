#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "job.h"

typedef struct Node {
	Job job;
	struct Node *next;
} Node;

typedef struct Queue {
	Node *head;
	Node *tail;
	int size;
	bool locked;

	void (*enqueue)(struct Queue*, struct Job);
	Job (*dequeue)(struct Queue*);
	Job (*peek)(struct Queue*);
	void (*printQueue)(struct Queue*);
	void (*wait)(struct Queue*);
	int (*getSize)(struct Queue*);
} Queue;

Queue createQueue();
void enqueue(Queue *queue, Job job);
Job dequeue(Queue *queue);
Job peek(Queue *queue);
void printQueue(Queue *queue);
void wait(Queue *queue);
int getSize(Queue *queue);

#endif


