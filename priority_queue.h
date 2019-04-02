#include <stdint.h>

#ifndef PRIORITY_QUEUE_H
# define PRIORITY_QUEUE_H

typedef struct queue_node queue_node_t;

struct queue_node {
	uint8_t x;
	uint8_t y;
	int cost;
	queue_node_t *next;
};

typedef struct priority_queue {
	queue_node_t *head;
	int size;
} queue_t;

queue_node_t* new_node(int v, int p);
int queue_init(queue_t *q);
int queue_delete(queue_t *q);
int queue_peek(queue_t *q, uint8_t *x, uint8_t *y, int *c);
int queue_push(queue_t *q, uint8_t x, uint8_t y, int c);
int queue_pop(queue_t *q, uint8_t *x, uint8_t *y, int *c);
//int remove(queue_t *pq, uint8_t x, uint8_t y, int *c);
int queue_size(queue_t *q);

#endif
