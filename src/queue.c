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
	queue.lock = &lock;
	queue.unlock = &unlock;

	return queue;
}

void enqueue(Queue *queue, Job job) {
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
}

Job dequeue(Queue *queue) {
	Node *head = queue->head;
	Job job = head->job;
	queue->head = head->next;
	queue->size--;
	free(head);
	return job;
}

Job peek(Queue *queue) {
	return queue->head->job;
}

void lock(Queue *queue) {
	queue->locked = true;
}

void unlock(Queue *queue) {
	queue->locked = false;
}

void printQueue(Queue *queue) {
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
}

