#include "priority_queue.hpp"
#include <cstdlib>

int queue_init(queue_t *q) {
  q->size = 0;
  q->head = NULL;
  return 0;
}

int queue_delete(queue_t *q) {
  queue_node_t *n;

  if (q->head) {
    for (n = q->head; n; n = q->head) {
      q->head = n->next;
      free(n);
    }

    q->size = 0;
  }

  return 1;
}

int queue_peek(queue_t *q, uint8_t *x, uint8_t *y, int *c) {
  if (q->head) {
    *x = q->head->x;
    *y = q->head->y;
    *c = q->head->cost;
    return 1;
  }
  return 0;
}

int queue_push(queue_t *q, uint8_t x, uint8_t y, int c) {
  queue_node_t *temp = (queue_node_t *)malloc(sizeof(queue_node_t));
  queue_node_t *start = q->head;

  temp->x = x;
  temp->y = y;
  temp->cost = c;
  temp->next = NULL;

  q->size++;

  if (!q->head) {
    q->head = temp;
    return 0;
  }

  if (q->head->cost > c) {
    temp->next = q->head;
    q->head = temp;

    return 0;
  } else {
    while (start->next != NULL && start->next->cost < c) {
      start = start->next;
    }

    temp->next = start->next;
    start->next = temp;
  }

  return 0;
}

int queue_pop(queue_t *q, uint8_t *x, uint8_t *y, int *c) {
  if (q->size == 0)
    return 0;

  queue_node_t *temp;

  temp = q->head;
  q->head = temp->next;
  *x = temp->x;
  *y = temp->y;
  *c = temp->cost;

  q->size--;

  if (temp) {
    free(temp);
  }

  return 1;
}

int queue_size(queue_t *q) { return q->size; }
