#include "queue.h"

Queue createQueue() {
	Queue queue;
	queue.size = 0;
	queue.head = NULL;
	queue.tail = NULL;
	queue.locked = false;

	queue.enqueue = &enqueue;
	queue.dequeue = &dequeue;
	queue.peek = &peek;
	queue.printQueue = &printQueue;
	queue.wait = &wait;
	queue.getSize = &getSize;

	return queue;
}

void wait(Queue *queue) {
	while (queue->locked) {
	//	puts("Queue is locked");
	}
}

void enqueue(Queue *queue, Job job) {
	wait(queue);
	queue->locked = true;

	Node *n = (Node*) malloc(sizeof(Node));
	n->job = job;
	n->next = NULL;

	if (queue->head == NULL) {
		queue->head = n;
	} else {
		queue->tail->next = n;
	}

	queue->tail = n;
	queue->size++;

	queue->locked = false;
}

Job dequeue(Queue *queue) {
	wait(queue);
	queue->locked = true;

	Node *head = queue->head;
	Job job = head->job;
	queue->head = head->next;
	queue->size--;
	free(head);

	queue->locked = false;
	return job;
}

Job peek(Queue *queue) {
	wait(queue);
	queue->locked = true;

	Job j = queue->head->job;

	queue->locked = false;
	return j;
}

void lock(Queue *queue) {
	wait(queue);
	queue->locked = true;
}

void unlock(Queue *queue) {
	wait(queue);
	queue->locked = false;
}

int getSize(Queue *queue) {
	wait(queue);
	queue->locked = true;
	int size = queue->size;
	queue->locked = false;
	return size;
}

void printQueue(Queue *queue) {
	wait(queue);
	queue->locked = true;
	printf("\nQueue contents: ");
	if (queue->size > 0) {
		Node* head = queue->head;
		int i, size = queue->size;
		printf("%d job(s):\n", queue->size);
		for (i = 0; i < size; i++) {
			head->job.printJob(&head->job);
			head = head->next;
		}
	}
	printf("\n\n");
	queue->locked = false;
}

